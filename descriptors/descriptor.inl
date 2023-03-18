namespace magma
{
namespace descriptor
{
inline Sampler& Sampler::operator=(std::shared_ptr<const magma::Sampler> sampler) noexcept
{
    MAGMA_ASSERT(sampler);
    updateSampler(std::move(sampler));
    return *this;
}

inline ImmutableSampler& ImmutableSampler::operator=(std::shared_ptr<const magma::Sampler> sampler) noexcept
{   // Immutable sampler must be updated only once
    MAGMA_ASSERT(!binding.pImmutableSamplers);
    if (!binding.pImmutableSamplers)
    {
        updateSampler(nullptr);
        // If pImmutableSamplers is not NULL, then it is a pointer to an array of sampler handles
        // that will be copied into the set layout and used for the corresponding binding.
        MAGMA_ASSERT(sampler);
        binding.pImmutableSamplers = sampler->getHandleAddress();
        updated = true;
    }
    return *this;
}

inline CombinedImageSampler& CombinedImageSampler::operator=(const std::pair<std::shared_ptr<const ImageView>, std::shared_ptr<const magma::Sampler>>& combinedImageSampler) noexcept
{
    MAGMA_ASSERT(combinedImageSampler.first);
    MAGMA_ASSERT(combinedImageSampler.second);
    MAGMA_ASSERT(combinedImageSampler.first->getImage()->getUsage() & VK_IMAGE_USAGE_SAMPLED_BIT);
    if ((imageDescriptor.imageView != *combinedImageSampler.first) ||
        (imageDescriptor.sampler != *combinedImageSampler.second))
    {
        imageDescriptor = combinedImageSampler.first->getDescriptor(combinedImageSampler.second);
        updated = true;
    }
    return *this;
}

inline CombinedImageImmutableSampler& CombinedImageImmutableSampler::operator=(const std::pair<std::shared_ptr<const ImageView>, std::shared_ptr<const magma::Sampler>>& combinedImageSampler) noexcept
{
    updateImageView(combinedImageSampler.first, VK_IMAGE_USAGE_SAMPLED_BIT);
    // Immutable sampler must be updated only once
    if (!binding.pImmutableSamplers)
    {   // If pImmutableSamplers is not NULL, then it is a pointer to an array of sampler handles
        // that will be copied into the set layout and used for the corresponding binding.
        MAGMA_ASSERT(combinedImageSampler.second);
        binding.pImmutableSamplers = combinedImageSampler.second->getHandleAddress();
        updated = true;
    }
    return *this;
}

inline CombinedImageImmutableSampler& CombinedImageImmutableSampler::operator=(std::shared_ptr<const ImageView> imageView) noexcept
{   // Check that sampler is already set and stop carrying around it
    MAGMA_ASSERT(binding.pImmutableSamplers);
    updateImageView(std::move(imageView), VK_IMAGE_USAGE_SAMPLED_BIT);
    return *this;
}

inline SampledImage& SampledImage::operator=(std::shared_ptr<const ImageView> imageView) noexcept
{
    updateImageView(std::move(imageView), VK_IMAGE_USAGE_SAMPLED_BIT);
    return *this;
}

inline StorageImage& StorageImage::operator=(std::shared_ptr<const ImageView> imageView) noexcept
{
    updateImageView(std::move(imageView), VK_IMAGE_USAGE_STORAGE_BIT);
    return *this;
}

inline UniformTexelBuffer& UniformTexelBuffer::operator=(std::shared_ptr<const BufferView> bufferView) noexcept
{
    updateBufferView(std::move(bufferView), VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT);
    return *this;
}

inline StorageTexelBuffer& StorageTexelBuffer::operator=(std::shared_ptr<const BufferView> bufferView) noexcept
{
    updateBufferView(std::move(bufferView), VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT);
    return *this;
}

inline UniformBuffer& UniformBuffer::operator=(std::shared_ptr<const Buffer> buffer) noexcept
{
    updateBuffer(std::move(buffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    return *this;
}

inline StorageBuffer& StorageBuffer::operator=(std::shared_ptr<const Buffer> buffer) noexcept
{
    updateBuffer(std::move(buffer), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    return *this;
}

inline DynamicUniformBuffer& DynamicUniformBuffer::operator=(std::shared_ptr<const Buffer> buffer) noexcept
{
    updateBuffer(std::move(buffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    return *this;
}

inline DynamicStorageBuffer& DynamicStorageBuffer::operator=(std::shared_ptr<const Buffer> buffer) noexcept
{
    updateBuffer(std::move(buffer), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    return *this;
}


} // namespace descriptor
} // namespace magma
