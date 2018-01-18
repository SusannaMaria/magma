/*
Magma - C++1x interface over Khronos Vulkan API.
Copyright (C) 2018 Victor Coda.

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
#include "vertexInputState.h"
#include "../shared.h"

namespace magma
{
VertexInputState::VertexInputState()
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pNext = nullptr;
    flags = 0;
    vertexBindingDescriptionCount = 0;
    pVertexBindingDescriptions = nullptr;
    vertexAttributeDescriptionCount = 0;
    pVertexAttributeDescriptions = nullptr;
}

VertexInputState::VertexInputState(const VertexInputBinding& binding, const VertexInputAttribute& attribute):
    VertexInputState({binding}, {attribute})
{}

VertexInputState::VertexInputState(const VertexInputBinding& binding, const std::initializer_list<VertexInputAttribute>& attributes):
    VertexInputState({binding}, attributes)
{}

VertexInputState::VertexInputState(const std::initializer_list<VertexInputBinding>& bindings, 
    const std::initializer_list<VertexInputAttribute>& attributes)
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pNext = nullptr;
    flags = 0;
    vertexBindingDescriptionCount = MAGMA_COUNT(bindings);
    pVertexBindingDescriptions = helpers::copy(new VkVertexInputBindingDescription[vertexBindingDescriptionCount], bindings);
    vertexAttributeDescriptionCount = MAGMA_COUNT(attributes);
    pVertexAttributeDescriptions = helpers::copy(new VkVertexInputAttributeDescription[vertexAttributeDescriptionCount], attributes);
}

VertexInputState::VertexInputState(const VertexInputState& other)
{
    helpers::copy(this, &other);
    pVertexBindingDescriptions = helpers::copy(new VkVertexInputBindingDescription[vertexBindingDescriptionCount],
        other.pVertexBindingDescriptions, vertexBindingDescriptionCount);
    pVertexAttributeDescriptions = helpers::copy(new VkVertexInputAttributeDescription[vertexAttributeDescriptionCount],
        other.pVertexAttributeDescriptions, vertexAttributeDescriptionCount);
}

VertexInputState& VertexInputState::operator=(const VertexInputState& other)
{
    helpers::copy(this, &other);
    pVertexBindingDescriptions = helpers::copy(new VkVertexInputBindingDescription[vertexBindingDescriptionCount],
        other.pVertexBindingDescriptions, vertexBindingDescriptionCount);
    pVertexAttributeDescriptions = helpers::copy(new VkVertexInputAttributeDescription[vertexAttributeDescriptionCount],
        other.pVertexAttributeDescriptions, vertexAttributeDescriptionCount);
    return *this;
}

VertexInputState::~VertexInputState()
{
    delete[] pVertexBindingDescriptions;
    delete[] pVertexAttributeDescriptions;
}

namespace states
{
const VertexInputState noVertices;

const VertexInputState vertexHalf(VertexInputBinding(0, sizeof(short)), VertexInputAttribute(0, 0, VK_FORMAT_R16_SFLOAT, 0));
const VertexInputState vertex2Half(VertexInputBinding(0, sizeof(short) * 2), VertexInputAttribute(0, 0, VK_FORMAT_R16G16_SFLOAT, 0));
const VertexInputState vertex3Half(VertexInputBinding(0, sizeof(short) * 3), VertexInputAttribute(0, 0, VK_FORMAT_R16G16B16_SFLOAT, 0));
const VertexInputState vertex4Half(VertexInputBinding(0, sizeof(short) * 4), VertexInputAttribute(0, 0, VK_FORMAT_R16G16B16A16_SFLOAT, 0));

const VertexInputState vertexFloat(VertexInputBinding(0, sizeof(float)), VertexInputAttribute(0, 0, VK_FORMAT_R32_SFLOAT, 0));
const VertexInputState vertex2Float(VertexInputBinding(0, sizeof(float) * 2), VertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, 0));
const VertexInputState vertex3Float(VertexInputBinding(0, sizeof(float) * 3), VertexInputAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0));
const VertexInputState vertex4Float(VertexInputBinding(0, sizeof(float) * 4), VertexInputAttribute(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0));
} // namespace states
} // namespace magma
