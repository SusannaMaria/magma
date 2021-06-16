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
#pragma once

namespace magma
{
    class PhysicalDevice;

    /* Describes the queue that is requested to be created along with the logical device. */

    class DeviceQueueDescriptor final : public VkDeviceQueueCreateInfo
    {
    public:
        explicit DeviceQueueDescriptor(std::shared_ptr<const PhysicalDevice> device,
            VkQueueFlagBits queueType,
            const std::vector<float>& queuePriorities = {1.f});
        DeviceQueueDescriptor(const DeviceQueueDescriptor&);
        DeviceQueueDescriptor& operator=(const DeviceQueueDescriptor&);
        ~DeviceQueueDescriptor();

    private:
        uint32_t chooseFamilyIndex(VkQueueFlagBits queueType,
            const std::vector<VkQueueFamilyProperties>& queueFamilyProperties) const;
    };
} // namespace magma
