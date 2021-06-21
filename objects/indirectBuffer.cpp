/*
Magma - abstraction layer to facilitate usage of Khronos Vulkan API.
Copyright (C) 2018-2020 Victor Coda.

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
#include "indirectBuffer.h"
#include "deviceMemory.h"

namespace magma
{
IndirectBuffer::IndirectBuffer(std::shared_ptr<Device> device, uint32_t maxDrawCommands, std::size_t stride,
    bool persistentlyMapped, VkBufferCreateFlags flags, const Sharing& sharing, std::shared_ptr<Allocator> allocator):
    Buffer(std::move(device), static_cast<VkDeviceSize>(maxDrawCommands * stride),
        VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        flags, sharing, std::move(allocator)),
    maxDrawCommands(maxDrawCommands),
    stride(static_cast<uint32_t>(stride)),
    persistent(persistentlyMapped),
    cmdCount(0)
{}

IndirectBuffer::~IndirectBuffer()
{
    if (persistent)
        memory->unmap();
}

DrawIndirectBuffer::DrawIndirectBuffer(std::shared_ptr<Device> device, uint32_t maxDrawCommands,
    std::shared_ptr<Allocator> allocator /* nullptr */,
    bool persistentlyMapped /* false */,
    VkBufferCreateFlags flags /* 0 */,
    const Sharing& sharing /* Sharing() */):
    IndirectBuffer(std::move(device), maxDrawCommands, sizeof(VkDrawIndirectCommand), persistentlyMapped, flags, sharing, std::move(allocator)),
    mappedData(persistentlyMapped ? memory->map<VkDrawIndirectCommand>() : nullptr)
{}

uint32_t DrawIndirectBuffer::writeDrawCommand(uint32_t vertexCount,
    uint32_t firstVertex /* 0 */) noexcept
{
    MAGMA_ASSERT(cmdCount < maxDrawCommands);
    VkDrawIndirectCommand *drawIndirectCmd;
    if (persistentlyMapped())
        drawIndirectCmd = mappedData + cmdCount;
    else
        drawIndirectCmd = memory->map<VkDrawIndirectCommand>(sizeof(VkDrawIndirectCommand) * cmdCount, sizeof(VkDrawIndirectCommand));
    if (drawIndirectCmd)
    {
        drawIndirectCmd->vertexCount = vertexCount;
        drawIndirectCmd->instanceCount = 1;
        drawIndirectCmd->firstVertex = firstVertex;
        drawIndirectCmd->firstInstance = 0;
        if (!persistentlyMapped())
            memory->unmap();
    }
    return cmdCount++;
}

uint32_t DrawIndirectBuffer::writeDrawInstancedCommand(uint32_t vertexCount, uint32_t instanceCount,
    uint32_t firstVertex /* 0 */,
    uint32_t firstInstance /* 0 */) noexcept
{
    MAGMA_ASSERT(cmdCount < maxDrawCommands);
    VkDrawIndirectCommand *drawIndirectCmd;
    if (persistentlyMapped())
        drawIndirectCmd = mappedData + cmdCount;
    else
        drawIndirectCmd = memory->map<VkDrawIndirectCommand>(sizeof(VkDrawIndirectCommand) * cmdCount, sizeof(VkDrawIndirectCommand));
    if (drawIndirectCmd)
    {
        drawIndirectCmd->vertexCount = vertexCount;
        drawIndirectCmd->instanceCount = instanceCount;
        drawIndirectCmd->firstVertex = firstVertex;
        drawIndirectCmd->firstInstance = firstInstance;
        if (!persistentlyMapped())
            memory->unmap();
    }
    return cmdCount++;
}

uint32_t DrawIndirectBuffer::writeDrawCommand(const VkDrawIndirectCommand& drawCmd) noexcept
{
    MAGMA_ASSERT(cmdCount < maxDrawCommands);
    if (persistentlyMapped())
        memcpy(mappedData + cmdCount, &drawCmd, sizeof(VkDrawIndirectCommand));
    else if (auto *drawIndirectCmd = memory->map<VkDrawIndirectCommand>(sizeof(VkDrawIndirectCommand) * cmdCount, sizeof(VkDrawIndirectCommand)))
    {
        memcpy(drawIndirectCmd, &drawCmd, sizeof(VkDrawIndirectCommand));
        if (!persistentlyMapped())
            memory->unmap();
    }
    return cmdCount++;
}

DrawIndexedIndirectBuffer::DrawIndexedIndirectBuffer(std::shared_ptr<Device> device, uint32_t maxDrawIndexedCommands,
    std::shared_ptr<Allocator> allocator /* nullptr */,
    bool persistentlyMapped /* false */,
    VkBufferCreateFlags flags /* 0 */,
    const Sharing& sharing /* Sharing() */):
    IndirectBuffer(std::move(device), maxDrawIndexedCommands, sizeof(VkDrawIndexedIndirectCommand), persistentlyMapped, flags, sharing, std::move(allocator)),
    mappedData(persistentlyMapped ? memory->map<VkDrawIndexedIndirectCommand>() : nullptr)
{}

uint32_t DrawIndexedIndirectBuffer::writeDrawIndexedCommand(uint32_t indexCount,
    uint32_t firstIndex /* 0 */,
    uint32_t vertexOffset /* 0 */) noexcept
{
    MAGMA_ASSERT(cmdCount < maxDrawCommands);
    VkDrawIndexedIndirectCommand *drawIndexedIndirectCmd;
    if (persistentlyMapped())
        drawIndexedIndirectCmd = mappedData + cmdCount;
    else
        drawIndexedIndirectCmd = memory->map<VkDrawIndexedIndirectCommand>(sizeof(VkDrawIndexedIndirectCommand) * cmdCount, sizeof(VkDrawIndexedIndirectCommand));
    if (drawIndexedIndirectCmd)
    {
        drawIndexedIndirectCmd->indexCount = indexCount;
        drawIndexedIndirectCmd->instanceCount = 1;
        drawIndexedIndirectCmd->firstIndex = firstIndex;
        drawIndexedIndirectCmd->vertexOffset = vertexOffset;
        drawIndexedIndirectCmd->firstInstance = 0;
        if (!persistentlyMapped())
            memory->unmap();
    }
    return cmdCount++;
}

uint32_t DrawIndexedIndirectBuffer::writeDrawIndexedInstancedCommand(uint32_t indexCount, uint32_t instanceCount,
    uint32_t firstIndex /* 0 */,
    uint32_t vertexOffset /* 0 */,
    uint32_t firstInstance /* 0 */) noexcept
{
    MAGMA_ASSERT(cmdCount < maxDrawCommands);
    VkDrawIndexedIndirectCommand *drawIndexedIndirectCmd;
    if (persistentlyMapped())
        drawIndexedIndirectCmd = mappedData + cmdCount;
    else
        drawIndexedIndirectCmd = memory->map<VkDrawIndexedIndirectCommand>(sizeof(VkDrawIndexedIndirectCommand) * cmdCount, sizeof(VkDrawIndexedIndirectCommand));
    if (drawIndexedIndirectCmd)
    {
        drawIndexedIndirectCmd->indexCount = indexCount;
        drawIndexedIndirectCmd->instanceCount = instanceCount;
        drawIndexedIndirectCmd->firstIndex = firstIndex;
        drawIndexedIndirectCmd->vertexOffset = vertexOffset;
        drawIndexedIndirectCmd->firstInstance = firstInstance;
        if (!persistentlyMapped())
            memory->unmap();
    }
    return cmdCount++;
}

uint32_t DrawIndexedIndirectBuffer::writeDrawIndexedCommand(const VkDrawIndexedIndirectCommand& drawCmd) noexcept
{
    MAGMA_ASSERT(cmdCount < maxDrawCommands);
    if (persistentlyMapped())
        memcpy(mappedData + cmdCount, &drawCmd, sizeof(VkDrawIndexedIndirectCommand));
    else if (auto *drawIndexedIndirectCmd = memory->map<VkDrawIndexedIndirectCommand>(sizeof(VkDrawIndexedIndirectCommand) * cmdCount, sizeof(VkDrawIndexedIndirectCommand)))
    {
        memcpy(drawIndexedIndirectCmd, &drawCmd, sizeof(VkDrawIndexedIndirectCommand));
        if (!persistentlyMapped())
            memory->unmap();
    }
    return cmdCount++;
}
} // namespace magma
