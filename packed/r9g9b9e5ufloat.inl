namespace magma
{
namespace packed
{
inline int floorLog2(float x)
{
    Float32 f;
    f.f = x;
    return f.e - Float32::bias;
}

inline R9g9b9e5Ufloat::R9g9b9e5Ufloat(float r, float g, float b) noexcept
{
    constexpr int MANTISSA_BITS = 9;
    constexpr int EXP_BIAS = 15;
    constexpr int MAX_VALID_BIASED_EXP = 31;
    constexpr int MAX_EXP = MAX_VALID_BIASED_EXP - EXP_BIAS;
    constexpr int MAX_MANTISSA = (1 << MANTISSA_BITS) - 1;
    constexpr float MAX_RGB9E5 = float(MAX_MANTISSA) / (1 << MANTISSA_BITS) * (1 << MAX_EXP);
    // 1/(2^(exp - EXP_BIAS - MANTISSA_BITS))
    constexpr double invExpPow2[MAX_VALID_BIASED_EXP + 1] = {
        1.67772e+07,
        8.38861e+06,
        4.1943e+06,
        2.09715e+06,
        1.04858e+06,
        524288.0,
        262144.0,
        131072.0,
        65536.0,
        32768.0,
        16384.0,
        8192.0,
        4096.0,
        2048.0,
        1024.0,
        512.0,
        256.0,
        128.0,
        64.0,
        32.0,
        16.0,
        8.0,
        4.0,
        2.0,
        1.0,
        0.5,
        0.25,
        0.125,
        0.0625,
        0.03125,
        0.015625,
        0.0078125
    };
#ifdef MAGMA_SSE
    __m128 v = _mm_set_ps(0.f, b, g, r);
    v = _mm_max_ps(v, _mm_setzero_ps());
    v = _mm_min_ps(v, _mm_set_ps1(MAX_RGB9E5));
    // Horizontal max3
    __m128 v1 = _mm_movehl_ps(v, v);
    __m128 v2 = _mm_max_ps(v, v1);
    v1 = mm_permute_ps(v, _MM_SHUFFLE(3, 1, 1, 1));
    __m128 max3 = _mm_max_ps(v1, v2);
    // TODO: begin vectorize
    float maxRgb;
    _mm_store_ss(&maxRgb, max3);
    int fl = floorLog2(maxRgb);
    int exp = (-EXP_BIAS - 1 > fl ? -EXP_BIAS - 1 : fl) + 1 + EXP_BIAS;
    // TODO: end vectorize
    v2 = _mm_mul_ss(max3, _mm_set_ss((float)invExpPow2[exp])); // max(r, g, b) / (2^(exp - EXP_BIAS - MANTISSA_BITS))
    int maxm = _mm_extract_epi32(_mm_cvtps_epi32(v2), 0); // Convert to int with rounding
    if (MAX_MANTISSA + 1 == maxm) ++exp;
    __m128 scale = _mm_set_ps1((float)invExpPow2[exp]);
    v = _mm_mul_ps(v, scale);
    __m128i iv = _mm_cvtps_epi32(v); // Convert to int with rounding
    this->v =
        (((uint32_t)exp & 0x1F) << 27) |
        (((uint32_t)_mm_extract_epi32(iv, 2) & 0x1FF) << 18) |
        (((uint32_t)_mm_extract_epi32(iv, 1) & 0x1FF) << 9) |
        ((uint32_t)_mm_extract_epi32(iv, 0) & 0x1FF);
#else
    r = std::min(std::max(0.f, r), MAX_RGB9E5);
    g = std::min(std::max(0.f, g), MAX_RGB9E5);
    b = std::min(std::max(0.f, b), MAX_RGB9E5);
    float maxRgb = std::max(std::max(r, g), b);
    int exp = std::max(-EXP_BIAS - 1, floorLog2(maxRgb)) + 1 + EXP_BIAS;
    MAGMA_ASSERT(exp >= 0);
    MAGMA_ASSERT(exp <= MAX_VALID_BIASED_EXP);
    int maxm = (int)std::round(maxRgb * invExpPow2[exp]); // max(r, g, b) / (2^(exp - EXP_BIAS - MANTISSA_BITS))
    MAGMA_ASSERT(maxm <= MAX_MANTISSA);
    if (MAX_MANTISSA + 1 == maxm)
    {
        ++exp;
        MAGMA_ASSERT(exp <= MAX_VALID_BIASED_EXP);
    }
    double scale = invExpPow2[exp];
    rm = (uint32_t)std::round(r * scale);
    gm = (uint32_t)std::round(g * scale);
    bm = (uint32_t)std::round(b * scale);
    e = (uint32_t)exp;
#endif // MAGMA_SSE
}

inline void R9g9b9e5Ufloat::unpack(float v[3]) noexcept
{
    constexpr int MANTISSA_BITS = 9;
    constexpr int EXP_BIAS = 15;
    int exp = e - EXP_BIAS - MANTISSA_BITS;
    float scale = powf(2.f, (float)exp);
    v[0] = rm * scale;
    v[1] = gm * scale;
    v[2] = bm * scale;
}
} // namespace packed
} // namespace magma

/* Lookup table generation code:

constexpr int MANTISSA_BITS = 9;
constexpr int EXP_BIAS = 15;
constexpr int MAX_VALID_BIASED_EXP = 31;
for (int exp = 0; exp <= MAX_VALID_BIASED_EXP; ++exp)
{
    double val = pow(2.0, (double)exp - EXP_BIAS - MANTISSA_BITS);
    std::cout << 1.0/val << "," << std::endl;
}
*/
