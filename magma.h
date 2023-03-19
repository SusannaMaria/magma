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
#pragma once

#include "core/pch.h"

#include "allocator/alignedAllocator.h"
#include "allocator/cxxAllocator.h"
#include "allocator/deviceMemoryAllocator.h"

#include "objects/accelerationStructure.h"
#include "objects/accelerationStructureInstanceBuffer.h"
#include "objects/buffer.h"
#include "objects/bufferView.h"
#include "objects/commandBuffer.h"
#include "objects/commandPool.h"
#include "objects/computePipeline.h"
#include "objects/computePipelines.h"
#include "objects/debugReportCallback.h"
#include "objects/debugUtilsMessenger.h"
#include "objects/deferredOperation.h"
#include "objects/descriptorPool.h"
#include "objects/descriptorSet.h"
#include "objects/descriptorSetLayout.h"
#include "objects/device.h"
#include "objects/deviceMemory.h"
#include "objects/display.h"
#include "objects/displayMode.h"
#include "objects/dstTransferBuffer.h"
#include "objects/event.h"
#include "objects/fence.h"
#include "objects/fragmentDensityMap.h"
#include "objects/fragmentShadingRateAttachment.h"
#include "objects/framebuffer.h"
#include "objects/fullScreenExclusiveSwapchain.h"
#include "objects/graphicsPipeline.h"
#include "objects/graphicsPipelines.h"
#include "objects/image.h"
#include "objects/image1D.h"
#include "objects/image1DArray.h"
#include "objects/image2D.h"
#include "objects/image2DArray.h"
#include "objects/imageAttachment.h"
#include "objects/image3D.h"
#include "objects/imageCube.h"
#include "objects/imagelessFramebuffer.h"
#include "objects/imageView.h"
#include "objects/indexBuffer.h"
#include "objects/indirectBuffer.h"
#include "objects/instance.h"
#include "objects/mutableImage.h"
#include "objects/mutableImageView.h"
#include "objects/physicalDevice.h"
#include "objects/pipeline.h"
#include "objects/pipelineCache.h"
#include "objects/pipelineLayout.h"
#include "objects/primaryCommandBuffer.h"
#include "objects/queryPool.h"
#include "objects/queue.h"
#include "objects/rayTracingBuffer.h"
#include "objects/rayTracingPipeline.h"
#include "objects/rayTracingPipelines.h"
#include "objects/renderPass.h"
#include "objects/renderPassMultiview.h"
#include "objects/resource.h"
#include "objects/sampler.h"
#include "objects/samplerYcbcrConversion.h"
#include "objects/secondaryCommandBuffer.h"
#include "objects/semaphore.h"
#include "objects/shaderModule.h"
#include "objects/srcTransferBuffer.h"
#include "objects/storageBuffer.h"
#include "objects/storageTexelBuffer.h"
#include "objects/surface.h"
#include "objects/swapchain.h"
#include "objects/swapchainImage.h"
#include "objects/transformFeedbackBuffer.h"
#include "objects/typedefs.h"
#include "objects/uniformBuffer.h"
#include "objects/uniformTexelBuffer.h"
#include "objects/validationCache.h"
#include "objects/vertexBuffer.h"

#include "shaders/pipelineShaderStage.h"
#include "shaders/rayTracingShaderGroup.h"
#include "shaders/shaderBindingTable.h"
#include "shaders/shaderReflection.h"
#include "shaders/shaderReflectionFactory.h"
#include "shaders/shaderStages.h"
#include "shaders/specialization.h"

#include "states/colorBlendAttachmentState.h"
#include "states/colorBlendState.h"
#include "states/colorBlendStateAdvanced.h"
#include "states/colorLogicOpState.h"
#include "states/colorMask.h"
#include "states/conservativeRasterizationState.h"
#include "states/depthComparisonSamplerState.h"
#include "states/depthStencilState.h"
#include "states/inputAssemblyState.h"
#include "states/lineRasterizationState.h"
#include "states/minmaxSamplerState.h"
#include "states/multisampleCoverageReductionState.h"
#include "states/multisampleState.h"
#include "states/multiViewportState.h"
#include "states/provokingVertexState.h"
#include "states/rasterizationOrderState.h"
#include "states/rasterizationState.h"
#include "states/samplerState.h"
#include "states/samplerYcbcrConversionState.h"
#include "states/streamRasterizationState.h"
#include "states/tesselationState.h"
#include "states/vertexInputState.h"
#include "states/vertexInputStructure.h"
#include "states/viewportState.h"

#include "descriptors/accelerationStructure.h"
#include "descriptors/attachment.h"
#include "descriptors/bufferDescriptor.h"
#include "descriptors/bufferDescriptorArray.h"
#include "descriptors/descriptor.h"
#include "descriptors/descriptorPool.h"
#include "descriptors/descriptorSetTable.h"
#include "descriptors/imageDescriptor.h"
#include "descriptors/imageDescriptorArray.h"
#include "descriptors/inlineUniformBlock.h"
#include "descriptors/loadStoreOp.h"
#include "descriptors/subpass.h"

#include "barriers/bufferMemoryBarrier.h"
#include "barriers/imageMemoryBarrier.h"
#include "barriers/memoryBarrier.h"
#include "barriers/subpassDependency.h"
#include "barriers/subpassDependency2.h"

#include "misc/clearAttachment.h"
#include "misc/clearValue.h"
#include "misc/deviceFaultInfo.h"
#include "misc/deviceFeatures.h"
#include "misc/deviceQueueDescriptor.h"
#include "misc/extProcAddress.h"
#include "misc/format.h"
#include "misc/framebufferAttachmentImage.h"
#include "misc/geometry.h"
#include "misc/imageSubresourceRange.h"
#include "misc/pipelineExecutable.h"
#include "misc/pushConstantRange.h"
#include "misc/scissor.h"
#include "misc/scopedDebugMarker.h"
#include "misc/sharing.h"
#include "misc/structureChain.h"
#include "misc/viewport.h"

#include "extensions/instanceExtensions.h"
#include "extensions/instanceLayers.h"
#include "extensions/physicalDeviceExtensions.h"

#include "auxiliary/accumulationBuffer.h"
#include "auxiliary/baseFramebuffer.h"
#include "auxiliary/blitRectangle.h"
#include "auxiliary/colorFramebuffer.h"
#include "auxiliary/colorMultisampleFramebuffer.h"
#include "auxiliary/depthFramebuffer.h"
#include "auxiliary/fillRectangleVertexShader.h"
#include "auxiliary/frameGrabber.h"
#include "auxiliary/generateMipmap.h"
#include "auxiliary/graphicsPipelineCache.h"
#include "auxiliary/immediateRender.h"
#include "auxiliary/multiAttachmentFramebuffer.h"
#include "auxiliary/profiler.h"
#include "auxiliary/shaderCompiler.h"
#include "auxiliary/sprite.h"
#include "auxiliary/swapchainFramebuffer.h"
#include "auxiliary/textShader.h"
#include "auxiliary/typedefs.h"

#include "helpers/alignedUniformArray.h"
#include "helpers/executeCommandBuffer.h"
#include "helpers/executeDeferredOperation.h"
#include "helpers/mapScoped.h"
#include "helpers/stackArray.h"
#include "helpers/stringize.h"
#include "helpers/uniformArray.h"

#include "packed/color.h"
#include "packed/vector.h"

#include "exceptions/compileError.h"
#include "exceptions/errorResult.h"
#include "exceptions/notImplemented.h"
#include "exceptions/reflectionErrorResult.h"
#include "exceptions/unsupportedExtension.h"
