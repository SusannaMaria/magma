namespace magma
{
namespace packed
{
inline X10y10z10w2unorm::X10y10z10w2unorm(float x, float y, float z, int w /* 0 */) noexcept
{
#ifdef MAGMA_SSE
    constexpr float hi[4] = {1.f, 1.f, 1.f, 1.f};
    constexpr float scale[4] = {1023.f, 1023.f * 1024.f, 1023.f * 1048576.f, 3.0f * 536870912.0f}; // 2^10, 2^20, 2^30
    __m128 v = _mm_set_ps(0.f, z, y, x);
    v = _mm_max_ps(v, _mm_setzero_ps());
    v = _mm_min_ps(v, _mm_load_ps(hi));
    v = _mm_mul_ps(v, _mm_load_ps(scale));
    __m128i iv = _mm_cvtps_epi32(v);
    iv = _mm_and_si128(iv, _mm_set_epi32(0x0, 0x3FF << 20, 0x3FF << 10, 0x3FF)); // Mask off any fraction
    // Horizontal bitwise OR
    v = XM_PERMUTE_PS(_mm_castsi128_ps(iv), _MM_SHUFFLE(0, 3, 2, 1));
    iv = _mm_or_si128(iv, _mm_castps_si128(v));
    v = XM_PERMUTE_PS(v, _MM_SHUFFLE(0, 3, 2, 1));
    iv = _mm_or_si128(iv, _mm_castps_si128(v));
    this->v = ((w & 0x3) << 30) | _mm_cvtsi128_si32(iv);
#else
    x = std::min(std::max(x, 0.f), 1.f);
    y = std::min(std::max(y, 0.f), 1.f);
    z = std::min(std::max(z, 0.f), 1.f);
    x = std::roundf(x * 1023.f);
    y = std::roundf(y * 1023.f);
    z = std::roundf(z * 1023.f);
    v = ((w & 0x3) << 30) |
        (((uint32_t)z & 0x3FF) << 20) |
        (((uint32_t)y & 0x3FF) << 10) |
        ((uint32_t)x & 0x3FF);
#endif // MAGMA_SSE
}
} // namespace packed
} // namespace magma
