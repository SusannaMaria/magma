/*
Magma - Abstraction layer over Khronos Vulkan API.
Copyright (C) 2018-2023 Victor Coda.

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
#ifndef MAGMA_NO_EXCEPTIONS
#include "../exceptions/unsupportedExtension.h"
#endif

namespace magma
{
    /* Holds a pointer to the Vulkan ICD proc address. */

    template<class Fn>
    class Extension
    {
    public:
        operator Fn() const noexcept { return procAddr; }
        operator bool() const noexcept { return procAddr != nullptr; }

    protected:
        Extension(PFN_vkVoidFunction procAddr) noexcept;
        void requireProcAddress(const char *extensionName,
            bool device) const;

    private:
        const Fn procAddr;
    };

    /* Represents enabled instance extension dispatchable command. */

    template<class Fn>
    class InstanceExtension final : public Extension<Fn>
    {
    public:
        explicit InstanceExtension(VkInstance instance,
            const char *name) noexcept;
        explicit InstanceExtension(VkInstance instance,
            const char *name,
            const char *extensionName);
    };

    /* In order to support systems with multiple Vulkan
       implementations, the function pointers returned by
       vkGetInstanceProcAddr may point to dispatch code
       that calls a different real implementation for
       different VkDevice objects or their child objects.
       The overhead of the internal dispatch for VkDevice
       objects can be avoided by obtaining device-specific
       function pointers for any commands that use a device
       or device-child object as their dispatchable object. */

    template<class Fn>
    class DeviceExtension final : public Extension<Fn>
    {
    public:
        explicit DeviceExtension(VkDevice device,
            const char *name) noexcept;
        explicit DeviceExtension(VkDevice device,
            const char *name,
            const char *extensionName);
    };
} // namespace magma

#define MAGMA_INSTANCE_EXTENSION(proc) static const magma::InstanceExtension<PFN_##proc> proc(MAGMA_HANDLE(instance), MAGMA_STRINGIZE(proc))
#define MAGMA_REQUIRED_INSTANCE_EXTENSION(proc, extensionName) static const magma::InstanceExtension<PFN_##proc> proc(MAGMA_HANDLE(instance), MAGMA_STRINGIZE(proc), extensionName)
#define MAGMA_DEVICE_EXTENSION(proc) static const magma::DeviceExtension<PFN_##proc> proc(MAGMA_HANDLE(device), MAGMA_STRINGIZE(proc))
#define MAGMA_REQUIRED_DEVICE_EXTENSION(proc, extensionName) static const magma::DeviceExtension<PFN_##proc> proc(MAGMA_HANDLE(device), MAGMA_STRINGIZE(proc), extensionName)

#include "extension.inl"
