#include <vector>
#include <cstdint>

#include <immintrin.h>

#include "SIMD_Finder.hpp"
#include "EngineClassData.hpp"

using namespace std;

// Forward declarations for SIMD helper functions
void IncrementFrames_SIMD_AVX2(
    vector<uint32_t>& FrameNums,
    size_t Size
);

void IncrementFrames_SIMD_AVX512(
    vector<uint32_t>& FrameNums,
    size_t Size
);

void IncrementFrames_SIMD_SSE2(
    vector<uint32_t>& FrameNums,
    size_t Size
);

void ObjectManager::IncrementFrames(
    vector<uint32_t>& FrameNums
) {
    size_t Size = FrameNums.size();
    switch(SIMDRegisterLevel)
    {
        // 256 bit
        case SIMDLevel::AVX:
        case SIMDLevel::AVX2:
            IncrementFrames_SIMD_AVX2(FrameNums, Size);
            break;

        // 512 bit
        case SIMDLevel::AVX512:
            IncrementFrames_SIMD_AVX512(FrameNums, Size);
            break;

        // 128 bit
        default:
            IncrementFrames_SIMD_SSE2(FrameNums, Size);
    }
}

// Helpers for incrementing with SIMD --------------------

__attribute__((target("avx2")))
// AXV / AXV2 (256 bit - 8 floats) - 8 at a time
void IncrementFrames_SIMD_AVX2(
    vector<uint32_t>& FrameNums,
    size_t Size
) {
    // Helper
    __m256i One = _mm256_set1_epi32(1);

    // Loop
    size_t i = 0;
    for(; i + 8 <= Size; i += 8)
    {
        // Frame nums
        __m256i FrameData = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(&FrameNums[i])
        );

        // Add them
        FrameData = _mm256_add_epi32(FrameData, One);

        // Store back
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(&FrameNums[i]),
            FrameData
        );
    }

    // Loop through the rest of the items
    for(; i < Size; ++i)
    {
        // Update x
        FrameNums[i]++;
    }
}


__attribute__((target("avx512f")))
// AXV512 (512 bit, 16 floats) - 16 at a time
void IncrementFrames_SIMD_AVX512(
    vector<uint32_t>& FrameNums,
    size_t Size
) {
    // Helper
    __m512i One = _mm512_set1_epi32(1);

    // Loop
    size_t i = 0;
    for(; i + 16 <= Size; i += 16)
    {
        // Frame nums
        __m512i FrameData = _mm512_loadu_si512(
            reinterpret_cast<const __m512i*>(&FrameNums[i])
        );

        // Add them
        FrameData = _mm512_add_epi32(FrameData, One);

        // Store back
        _mm512_storeu_si512(
            reinterpret_cast<__m512i*>(&FrameNums[i]),
            FrameData
        );
    }

    // Loop through the rest of the items
    for(; i < Size; ++i)
    {
        // Update x
        FrameNums[i]++;
    }
}


__attribute__((target("sse2")))
// SSE2 (128 bit, 4 floats) - 4 at a time
void IncrementFrames_SIMD_SSE2(
    vector<uint32_t>& FrameNums,
    size_t Size
) {
    // Helper
    __m128i One = _mm_set1_epi32(1);

    // Loop
    size_t i = 0;
    for(; i + 4 <= Size; i += 4)
    {
        // Frame nums
        __m128i FrameData = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(&FrameNums[i])
        );

        // Add them
        FrameData = _mm_add_epi32(FrameData, One);

        // Store back
        _mm_storeu_si128(
            reinterpret_cast<__m128i*>(&FrameNums[i]),
            FrameData
        );
    }

    // Loop through the rest of the items
    for(; i < Size; ++i)
    {
        // Update x
        FrameNums[i]++;
    }
}