/*
Magma - Abstraction layer over Khronos Vulkan API.
Copyright (C) 2018-2024 Victor Coda.

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
#include "pipelineBatch.h"

namespace magma
{
#ifdef VK_AMD_pipeline_compiler_control
VkPipelineCompilerControlFlagsAMD PipelineBatch::compilerControlFlags = 0;
#endif

std::future<void> PipelineBatch::buildPipelinesAsync(std::shared_ptr<Device> device,
    std::shared_ptr<PipelineCache> pipelineCache /* nullptr */,
    std::shared_ptr<IAllocator> allocator /* nullptr */)
{
    return std::async(std::launch::async,
        [this, device, pipelineCache, allocator]()
        {
            buildPipelines(std::move(device), std::move(pipelineCache), std::move(allocator));
        });
}

void PipelineBatch::collectShaderStageInfos() const
{
    std::size_t stageCount = 0;
    for (const auto& shaderStages: stages)
        stageCount += shaderStages.size();
    shaderStageInfos.clear();
    shaderStageInfos.reserve(stageCount);
    for (const auto& shaderStages: stages)
    {   // Copy to array of Vulkan structures due to alignment
        for (const auto& stage: shaderStages)
            shaderStageInfos.push_back(stage);
    }
}
} // namespace magma
