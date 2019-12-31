/*
Magma - abstraction layer to facilitate usage of Khronos Vulkan API.
Copyright (C) 2018-2019 Victor Coda.

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

namespace magma
{
    class CommandPool;
    class CommandBuffer;

    namespace helpers
    {
        bool executeCommandBuffer(std::shared_ptr<CommandPool> cmdPool,
            std::function<void(std::shared_ptr<CommandBuffer>)> callback,
            bool primaryLevel = true,
            VkQueueFlagBits queueType = VK_QUEUE_GRAPHICS_BIT,
            const char *blockName = "magma::helpers::executeCommandBuffer",
            uint32_t blockColor = 0x0);
    } // namespace helpers
} // namespace magma
