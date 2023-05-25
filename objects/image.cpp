/*
Magma - abstraction layer to facilitate usage of Khronos Vulkan API.
Copyright (C) 2018-2022 Victor Coda.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include "pch.h"
#pragma hdrstop
#include "image.h"
#include "srcTransferBuffer.h"
#include "device.h"
#include "physicalDevice.h"
#include "deviceMemory.h"
#include "queue.h"
#include "fence.h"
#include "commandBuffer.h"
#include "../barriers/imageMemoryBarrier.h"
#include "../misc/format.h"
#include "../exceptions/errorResult.h"
#include "../core/round.h"

// Vulkan validation layers may complain about image regions for block-compressed formats. See:
// https://vulkan.lunarg.com/doc/view/1.3.224.1/windows/1.3-extensions/vkspec.html#VUID-vkCmdCopyBufferToImage-pRegions-06218
// "For each element of pRegions, imageOffset.x and (imageExtent.width + imageOffset.x) must both be
// greater than or equal to 0 and less than or equal to the width of the specified imageSubresource of dstImage".
#define MAGMA_VIRTUAL_MIP_EXTENT 1

namespace magma
{
Image::Image(std::shared_ptr<Device> device, VkImageType imageType, VkFormat format,
    const VkExtent3D& extent, uint32_t mipLevels, uint32_t arrayLayers, uint32_t samples,
    VkImageCreateFlags flags, VkImageUsageFlags usage, VkImageTiling tiling,
    const Descriptor& optional, const Sharing& sharing, std::shared_ptr<Allocator> allocator):
    NonDispatchableResource(VK_OBJECT_TYPE_IMAGE, device, sharing, allocator),
    flags(flags),
    imageType(imageType),
    format(format),
    layout(VK_IMAGE_LAYOUT_UNDEFINED),
    extent(extent),
    mipLevels(mipLevels),
    arrayLayers(arrayLayers),
    samples(samples),
    tiling(tiling),
    usage(usage)
{
    VkImageCreateInfo imageInfo;
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.flags = flags | optional.flags;
    imageInfo.imageType = imageType;
    imageInfo.format = format;
    imageInfo.extent = extent;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.samples = getSampleCountBit(samples);
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;
    imageInfo.sharingMode = sharing.getMode();
    imageInfo.queueFamilyIndexCount = sharing.getQueueFamiliesCount();
    imageInfo.pQueueFamilyIndices = sharing.getQueueFamilyIndices().data();
    imageInfo.initialLayout = layout;
#ifdef VK_KHR_image_format_list
    VkImageFormatListCreateInfoKHR imageFormatListInfo;
    viewFormats = optional.viewFormats;
    const bool imageFormatList = device->extensionEnabled(VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME);
    if (imageFormatList && !viewFormats.empty())
    {
        imageInfo.pNext = &imageFormatListInfo;
        imageFormatListInfo.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO_KHR;
        imageFormatListInfo.pNext = nullptr;
        imageFormatListInfo.viewFormatCount = MAGMA_COUNT(viewFormats);
        imageFormatListInfo.pViewFormats = viewFormats.data();
    }
#endif // VK_KHR_image_format_list
    const VkResult result = vkCreateImage(MAGMA_HANDLE(device), &imageInfo, MAGMA_OPTIONAL_INSTANCE(hostAllocator), &handle);
    MAGMA_THROW_FAILURE(result, "failed to create image");
    const VkMemoryRequirements memoryRequirements = getMemoryRequirements();
    VkMemoryPropertyFlags memoryFlags = (VK_IMAGE_TILING_LINEAR == tiling)
        ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | (optional.lazy ? VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT : 0);
    std::shared_ptr<DeviceMemory> memory = std::make_shared<DeviceMemory>(
        std::move(device), memoryRequirements, memoryFlags, optional.memoryPriority,
        &handle, VK_OBJECT_TYPE_IMAGE, std::move(allocator));
    bindMemory(std::move(memory));
}

Image::Image(std::shared_ptr<Device> device, VkImage handle, VkImageType imageType, VkFormat format, const VkExtent3D& extent):
    NonDispatchableResource(VK_OBJECT_TYPE_IMAGE, std::move(device), Sharing(), std::shared_ptr<Allocator>() /* FIX IT */),
    flags(0),
    imageType(imageType),
    format(format),
    layout(VK_IMAGE_LAYOUT_UNDEFINED),
    extent(extent),
    mipLevels(1),
    arrayLayers(1),
    samples(1),
    tiling(VK_IMAGE_TILING_OPTIMAL),
    usage(0)
{
    this->handle = handle;
}

Image::~Image()
{
    vkDestroyImage(MAGMA_HANDLE(device), handle, MAGMA_OPTIONAL_INSTANCE(hostAllocator));
}

VkExtent3D Image::calculateMipExtent(uint32_t level) const noexcept
{
    MAGMA_ASSERT(level < mipLevels);
    if (0 == level)
        return extent;
    if (level >= mipLevels)
        return VkExtent3D{0, 0, 0};
    VkExtent3D mipExtent = extent;
    mipExtent.width = std::max(1u, extent.width >> level);
    mipExtent.height = std::max(1u, extent.height >> level);
    mipExtent.depth = std::max(1u, extent.depth >> level);
    const Format imageFormat(format);
    if (imageFormat.blockCompressed())
    {   // Extents must be a multiple of the compressed texel block footprint
        const auto blockSize = imageFormat.blockFootprint();
        mipExtent.width = core::roundUp(mipExtent.width, blockSize.first);
        if (imageType > VK_IMAGE_TYPE_1D)
        {
            mipExtent.height = core::roundUp(mipExtent.height, blockSize.second);
            if (imageType > VK_IMAGE_TYPE_2D)
                mipExtent.depth = core::roundUp(mipExtent.depth, blockSize.second); // ?
        }
    }
    return mipExtent;
}

VkSubresourceLayout Image::getSubresourceLayout(uint32_t mipLevel, uint32_t arrayLayer /* 0 */) const noexcept
{
    VkImageSubresource subresource;
    const Format imageFormat(format);
    if (imageFormat.depth())
        subresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    else if (imageFormat.stencil())
        subresource.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
    else if (imageFormat.depthStencil())
        subresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    else
        subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.mipLevel = mipLevel;
    subresource.arrayLayer = this->arrayLayers > 1 ? arrayLayer : 0; // Ignore for non-arrays
    VkSubresourceLayout subresourceLayout;
    vkGetImageSubresourceLayout(MAGMA_HANDLE(device), handle, &subresource, &subresourceLayout);
    return subresourceLayout;
}

VkImageSubresourceLayers Image::getSubresourceLayers(uint32_t mipLevel, uint32_t arrayLayer /* 0 */) const noexcept
{
    VkImageSubresourceLayers subresourceLayers;
    const Format imageFormat(format);
    if (imageFormat.depth())
        subresourceLayers.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    else if (imageFormat.stencil())
        subresourceLayers.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
    else if (imageFormat.depthStencil())
        subresourceLayers.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    else
        subresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceLayers.mipLevel = mipLevel;
    subresourceLayers.baseArrayLayer = this->arrayLayers > 1 ? arrayLayer : 0; // Ignore for non-arrays
    subresourceLayers.layerCount = this->arrayLayers;
    return subresourceLayers;
}

VkMemoryRequirements Image::getMemoryRequirements() const noexcept
{
    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(MAGMA_HANDLE(device), handle, &memoryRequirements);
    return memoryRequirements;
}

std::vector<VkSparseImageMemoryRequirements> Image::getSparseMemoryRequirements() const
{
    uint32_t sparseMemoryRequirementCount;
    vkGetImageSparseMemoryRequirements(MAGMA_HANDLE(device), handle, &sparseMemoryRequirementCount, nullptr);
    std::vector<VkSparseImageMemoryRequirements> sparseMemoryRequirements(sparseMemoryRequirementCount);
    if (sparseMemoryRequirementCount > 0)
        vkGetImageSparseMemoryRequirements(MAGMA_HANDLE(device), handle, &sparseMemoryRequirementCount, sparseMemoryRequirements.data());
    return sparseMemoryRequirements;
}

void Image::bindMemory(std::shared_ptr<DeviceMemory> memory,
    VkDeviceSize offset /* 0 */)
{
    memory->bind(&handle, VK_OBJECT_TYPE_IMAGE, offset);
    this->size = memory->getSize();
    this->offset = offset;
    this->memory = std::move(memory);
}

#ifdef VK_KHR_device_group
void Image::bindMemoryDeviceGroup(std::shared_ptr<DeviceMemory> memory,
    const std::vector<uint32_t>& deviceIndices,
    VkDeviceSize offset /* 0 */)
{
    VkBindImageMemoryInfoKHR bindMemoryInfo;
    VkBindImageMemoryDeviceGroupInfoKHR bindMemoryDeviceGroupInfo;
    bindMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO_KHR;
    bindMemoryInfo.pNext = &bindMemoryDeviceGroupInfo;
    bindMemoryInfo.image = handle;
    bindMemoryInfo.memory = *memory;
    bindMemoryInfo.memoryOffset = memory->getOffset() + offset;
    bindMemoryDeviceGroupInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_DEVICE_GROUP_INFO_KHR;
    bindMemoryDeviceGroupInfo.pNext = nullptr;
    bindMemoryDeviceGroupInfo.deviceIndexCount = MAGMA_COUNT(deviceIndices);
    bindMemoryDeviceGroupInfo.pDeviceIndices = deviceIndices.data();
    bindMemoryDeviceGroupInfo.splitInstanceBindRegionCount = 0;
    bindMemoryDeviceGroupInfo.pSplitInstanceBindRegions = nullptr;
    MAGMA_REQUIRED_DEVICE_EXTENSION(vkBindImageMemory2KHR, VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
    const VkResult result = vkBindImageMemory2KHR(MAGMA_HANDLE(device), 1, &bindMemoryInfo);
    MAGMA_THROW_FAILURE(result, "failed to bind image memory within device group");
    this->size = memory->getSize();
    this->offset = offset;
    this->memory = std::move(memory);
}

void Image::bindMemoryDeviceGroup(std::shared_ptr<DeviceMemory> memory,
    const std::vector<uint32_t>& deviceIndices,
    const std::vector<VkRect2D>& splitInstanceBindRegions,
    VkDeviceSize offset /* 0 */)
{
    VkBindImageMemoryInfoKHR bindMemoryInfo;
    VkBindImageMemoryDeviceGroupInfoKHR bindMemoryDeviceGroupInfo;
    bindMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO_KHR;
    bindMemoryInfo.pNext = &bindMemoryDeviceGroupInfo;
    bindMemoryInfo.image = handle;
    bindMemoryInfo.memory = *memory;
    bindMemoryInfo.memoryOffset = memory->getOffset() + offset;
    bindMemoryDeviceGroupInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_DEVICE_GROUP_INFO_KHR;
    bindMemoryDeviceGroupInfo.pNext = nullptr;
    bindMemoryDeviceGroupInfo.deviceIndexCount = MAGMA_COUNT(deviceIndices);
    bindMemoryDeviceGroupInfo.pDeviceIndices = deviceIndices.data();
    bindMemoryDeviceGroupInfo.splitInstanceBindRegionCount = MAGMA_COUNT(splitInstanceBindRegions);
    bindMemoryDeviceGroupInfo.pSplitInstanceBindRegions = splitInstanceBindRegions.data();
    MAGMA_REQUIRED_DEVICE_EXTENSION(vkBindImageMemory2KHR, VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
    const VkResult result = vkBindImageMemory2KHR(MAGMA_HANDLE(device), 1, &bindMemoryInfo);
    MAGMA_THROW_FAILURE(result, "failed to bind image memory within device group");
    this->size = memory->getSize();
    this->offset = offset;
    this->memory = std::move(memory);
}
#endif // VK_KHR_device_group

void Image::onDefragment()
{
    VkImageCreateInfo imageInfo;
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.flags = flags;
    imageInfo.imageType = imageType;
    imageInfo.format = format;
    imageInfo.extent = extent;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.samples = getSampleCountBit(samples);
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;
    imageInfo.sharingMode = sharing.getMode();
    imageInfo.queueFamilyIndexCount = sharing.getQueueFamiliesCount();
    imageInfo.pQueueFamilyIndices = sharing.getQueueFamilyIndices().data();
    imageInfo.initialLayout = layout;
    vkDestroyImage(MAGMA_HANDLE(device), handle, MAGMA_OPTIONAL_INSTANCE(hostAllocator));
    const VkResult result = vkCreateImage(MAGMA_HANDLE(device), &imageInfo, MAGMA_OPTIONAL_INSTANCE(hostAllocator), &handle);
    MAGMA_THROW_FAILURE(result, "failed to recreate defragmented image");
    bindMemory(std::move(memory), offset);
}

VkImageLayout Image::layoutTransition(VkImageLayout newLayout, std::shared_ptr<CommandBuffer> cmdBuffer)
{
    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = mipLevels;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = arrayLayers;
    const VkImageLayout oldLayout = layout;
    layout = VK_IMAGE_LAYOUT_UNDEFINED; // Hack to assing 0 to srcAccessMask inside ImageMemoryBarrier
    const ImageMemoryBarrier memoryBarrier(shared_from_this(), newLayout, subresourceRange);
    cmdBuffer->pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, memoryBarrier);
    return oldLayout;
}

void Image::copyMip(std::shared_ptr<CommandBuffer> cmdBuffer, uint32_t mipLevel, uint32_t arrayLayer,
    std::shared_ptr<const SrcTransferBuffer> srcBuffer, const CopyLayout& bufferLayout, const VkOffset3D& imageOffset,
    VkImageLayout dstLayout, VkPipelineStageFlags dstStageMask /* VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT */)
{
    VkBufferImageCopy region;
    region.bufferOffset = bufferLayout.offset;
    region.bufferRowLength = bufferLayout.rowLength;
    region.bufferImageHeight = bufferLayout.imageHeight;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = mipLevel;
    region.imageSubresource.baseArrayLayer = arrayLayer;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = imageOffset;
    // Note: Vulkan validation layer expects virtual mip extent for block-compressed formats
    region.imageExtent = virtualMipExtent(mipLevel);
    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = mipLevel;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = arrayLayer;
    subresourceRange.layerCount = 1;
    // We couldn't call shared_from_this() from ctor, so use custom ref object w/ empty deleter
    const std::shared_ptr<Image> self = std::shared_ptr<Image>(this, [](Image *) {});
    // Transition image layout as a destination of a transfer command
    if (VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL == layout)
        layout = VK_IMAGE_LAYOUT_UNDEFINED; // Hack to select proper srcAccessMask inside ImageMemoryBarrier
    const ImageMemoryBarrier transferDst(shared_from_this(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        subresourceRange);
    cmdBuffer->pipelineBarrier(VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, transferDst);
    {
        cmdBuffer->copyBufferToImage(std::move(srcBuffer), self, region);
    }
    // Transition image layout to read-only access in a shader as a sampled image
    const ImageMemoryBarrier shaderRead(self,
        dstLayout,
        subresourceRange);
    // Insert memory dependency between transfer and fragment shader stages
    cmdBuffer->pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, dstStageMask, shaderRead);
}

VkDeviceSize Image::setupMipMaps(std::vector<Mip>& dstMips, const std::vector<MipData>& srcMips) const
{
    VkDeviceSize bufferOffset = 0;
    Mip mip;
    mip.bufferOffset = bufferOffset;
    mip.extent = srcMips[0].extent;
    dstMips.reserve(srcMips.size());
    dstMips.push_back(mip);
    bufferOffset += MAGMA_ALIGN(srcMips[0].size); // By default memory copies are aligned
    for (std::size_t mipIndex = 1, mipCount = srcMips.size(); mipIndex < mipCount; ++mipIndex)
    {
        mip.bufferOffset = bufferOffset;
        mip.extent = srcMips[mipIndex].extent;
        dstMips.push_back(mip);
        bufferOffset += MAGMA_ALIGN(srcMips[mipIndex].size);
    }
    return bufferOffset;
}

void Image::copyMipMaps(std::shared_ptr<CommandBuffer> cmdBuffer, std::shared_ptr<const SrcTransferBuffer> srcBuffer,
    const std::vector<Mip>& mipMaps, const CopyLayout& bufferLayout)
{
    std::vector<VkBufferImageCopy> regions;
    regions.reserve(mipMaps.size());
    uint32_t mipIndex = 0;
    for (const Mip& mip: mipMaps)
    {   // Define buffer -> image copy region
        VkBufferImageCopy region;
        region.bufferOffset = bufferLayout.offset + mip.bufferOffset;
        region.bufferRowLength = bufferLayout.rowLength;
        region.bufferImageHeight = bufferLayout.imageHeight;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = mipIndex % mipLevels;
        region.imageSubresource.baseArrayLayer = mipIndex / mipLevels;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        // Note: Vulkan validation layer expects virtual mip extent for block-compressed formats
        region.imageExtent = virtualMipExtent(region.imageSubresource.mipLevel);
        regions.push_back(region);
        ++mipIndex;
    }
    // Define resource range
    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = mipLevels;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = arrayLayers;
    // We couldn't call shared_from_this() from ctor, so use custom ref object w/ empty deleter
    const std::shared_ptr<Image> self = std::shared_ptr<Image>(this, [](Image *) {});
    // Transition image layout as a destination of a transfer command
    const ImageMemoryBarrier transferDst(self,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        subresourceRange);
    // Insert memory dependency between host and transfer stages
    cmdBuffer->pipelineBarrier(VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, transferDst);
    {
        cmdBuffer->copyBufferToImage(srcBuffer, self, regions);
    }
    // Transition image layout to read-only access in a shader as a sampled image
    const ImageMemoryBarrier shaderRead(self,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        subresourceRange);
    // Insert memory dependency between transfer and fragment shader stages
    cmdBuffer->pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, shaderRead);
}

VkExtent3D Image::virtualMipExtent(uint32_t level) const noexcept
{
    MAGMA_ASSERT(level < mipLevels);
    if (!level)
        return extent;
#if MAGMA_VIRTUAL_MIP_EXTENT
    return VkExtent3D{
        std::max(1u, extent.width >> level),
        std::max(1u, extent.height >> level),
        std::max(1u, extent.depth >> level)
    };
#else
    return calculateMipExtent(level); // Real extent
#endif // MAGMA_VIRTUAL_MIP_EXTENT
}

VkSampleCountFlagBits Image::getSampleCountBit(uint32_t samples) noexcept
{
    switch (samples)
    {
    case 1: return VK_SAMPLE_COUNT_1_BIT;
    case 2: return VK_SAMPLE_COUNT_2_BIT;
    case 4: return VK_SAMPLE_COUNT_4_BIT;
    case 8: return VK_SAMPLE_COUNT_8_BIT;
    case 16: return VK_SAMPLE_COUNT_16_BIT;
    case 32: return VK_SAMPLE_COUNT_32_BIT;
    case 64: return VK_SAMPLE_COUNT_64_BIT;
    }
    return VK_SAMPLE_COUNT_1_BIT;
}

VkFormat Image::checkFormatFeature(std::shared_ptr<Device> device, VkFormat format, VkFormatFeatureFlags requiredFeature)
{
    std::shared_ptr<PhysicalDevice> physicalDevice = device->getPhysicalDevice();
    const VkFormatProperties properties = physicalDevice->getFormatProperties(format);
    const bool hasFeature = (properties.optimalTilingFeatures & requiredFeature);
    if (!hasFeature)
        MAGMA_THROW("format doesn't suport required feature");
    return format;
}
} // namespace magma
