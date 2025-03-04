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
#include "pch.h"
#pragma hdrstop
#include "structureChain.h"

namespace magma
{
VkBaseOutStructure *StructureChain::chainNodes() noexcept
{
    if (chain.empty())
        return nullptr;
    auto head = chain.begin(), curr = head, next = head;
    while (++next != chain.end())
    {
        VkBaseOutStructure *node = curr->getNode();
        node->pNext = next->getNode();
        curr = next;
    }
    VkBaseOutStructure *last = curr->getNode();
    last->pNext = nullptr;
    return head->getNode();
}

const VkBaseInStructure *StructureChain::chainNodes() const noexcept
{
    if (chain.empty())
        return nullptr;
    auto head = chain.begin(), curr = head, next = head;
    while (++next != chain.end())
    {
        VkBaseOutStructure *node = curr->getNode();
        node->pNext = next->getNode();
        curr = next;
    }
    VkBaseOutStructure *last = curr->getNode();
    last->pNext = nullptr;
    return (const VkBaseInStructure *)head->getNode();
}

hash_t StructureChain::getHash() const noexcept
{
    hash_t hash = 0ull;
    for (auto const& node: chain)
        hash = core::hashCombine(hash, node.getHash());
    return hash;
}
} // namespace magma
