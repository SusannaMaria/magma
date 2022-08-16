namespace magma
{
constexpr SamplerState::SamplerState(const VkFilter magFilter, const VkFilter minFilter,
    const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) noexcept:
    VkSamplerCreateInfo{
        VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        nullptr, // pNext
        0, // flags
        magFilter,
        minFilter,
        mipmapMode,
        addressMode,
        addressMode,
        addressMode,
        0.f, // mipLodBias
        VK_FALSE, // anisotropyEnable
        1.f, // maxAnisotropy
        VK_FALSE, // compareEnable
        VK_COMPARE_OP_NEVER, // compareOp
        0.f, // minLod
        VK_LOD_CLAMP_NONE, // maxLod
        VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, // borderColor
        VK_FALSE // unnormalizedCoordinates
    }
{}

constexpr SamplerState::SamplerState(const SamplerState& other) noexcept:
    VkSamplerCreateInfo{
        other.sType,
        other.pNext,
        other.flags,
        other.magFilter,
        other.minFilter,
        other.mipmapMode,
        other.addressModeU,
        other.addressModeV,
        other.addressModeW,
        other.mipLodBias,
        other.anisotropyEnable,
        other.maxAnisotropy,
        other.compareEnable,
        other.compareOp,
        other.minLod,
        other.maxLod,
        other.borderColor,
        other.unnormalizedCoordinates
    }
{}

constexpr MagFilter SamplerState::getMagFilter() const noexcept
{
    return (VK_FILTER_LINEAR == magFilter) ? MagFilter::Bilinear : MagFilter::Nearest;
}

constexpr MipFilter SamplerState::getMipFilter() const noexcept
{
    if (VK_SAMPLER_MIPMAP_MODE_NEAREST == mipmapMode)
        return (VK_FILTER_LINEAR == minFilter) ? MipFilter::Bilinear : MipFilter::Nearest;
    // VK_SAMPLER_MIPMAP_MODE_LINEAR
    if (VK_FILTER_LINEAR == minFilter)
        return anisotropyEnable ? MipFilter::Anisotropic : MipFilter::Trilinear;
    else // VK_FILTER_NEAREST
        return MipFilter::Partial;
}

constexpr hash_t SamplerState::hash() const noexcept
{
    return core::hashArgs(
        sType,
        flags,
        magFilter,
        minFilter,
        mipmapMode,
        addressModeU,
        addressModeV,
        addressModeW,
        mipLodBias,
        anisotropyEnable,
        maxAnisotropy,
        compareEnable,
        compareOp,
        minLod,
        maxLod,
        borderColor,
        unnormalizedCoordinates);
}

constexpr bool SamplerState::operator==(const SamplerState& other) const noexcept
{
    return (flags == other.flags) &&
        (magFilter == other.magFilter) &&
        (minFilter == other.minFilter) &&
        (mipmapMode == other.mipmapMode) &&
        (addressModeU == other.addressModeU) &&
        (addressModeV == other.addressModeV) &&
        (addressModeW == other.addressModeW) &&
        (mipLodBias == other.mipLodBias) &&
        (anisotropyEnable == other.anisotropyEnable) &&
        (maxAnisotropy == other.maxAnisotropy) &&
        (compareEnable == other.compareEnable) &&
        (compareOp == other.compareOp) &&
        (minLod == other.minLod) &&
        (maxLod == other.maxLod) &&
        (borderColor == other.borderColor) &&
        (unnormalizedCoordinates == other.unnormalizedCoordinates);
}

inline hash_t SamplerState::chainedHash() const noexcept
{
    hash_t hash = SamplerState::hash();
    const VkBaseInStructure *next = reinterpret_cast<const VkBaseInStructure *>(pNext);
    while (next)
    {   // Hash depends on chained states order
        switch (next->sType)
        {
    #ifdef VK_EXT_sampler_filter_minmax
        case VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO_EXT:
            {
                auto reduction = reinterpret_cast<const VkSamplerReductionModeCreateInfoEXT *>(next);
                hash = core::hashCombine(hash, core::hashArgs(
                    reduction->sType,
                    reduction->reductionMode));
            }
    #endif // VK_EXT_sampler_filter_minmax
        default:
            MAGMA_ASSERT(false && "unknown chained structure");
        }
        next = reinterpret_cast<const VkBaseInStructure *>(next->pNext);
    }
    return hash;
}

constexpr AnisotropicSamplerState::AnisotropicSamplerState(const VkSamplerAddressMode addressMode,
    const float maxAnisotropy_ /* std::numeric_limits<float>::max() */) noexcept:
    SamplerState(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, addressMode)
{
    anisotropyEnable = VK_TRUE;
    // If anisotropyEnable is VK_TRUE, maxAnisotropy must be between
    // 1.0 and VkPhysicalDeviceLimits::maxSamplerAnisotropy, inclusive.
    maxAnisotropy = std::min(1.f, maxAnisotropy_);
}
} // namespace magma
