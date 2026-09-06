#include <vector>
#include <cstdint>

#if defined(__x86_64__) || defined(__i386__) || \
    defined(_M_X64) || defined(_M_IX86)

    #include <immintrin.h>

#elif defined(__aarch64__) || defined(_M_ARM64)

    #include <arm_neon.h>

#endif

#include "SIMD_Finder.hpp"
#include "EngineClassData.hpp"

namespace Blokk {

    namespace InternalHelpers 
    {
        // Forward declarations for SIMD helper functions

        #if defined(__x86_64__) || defined(__i386__) || \
            defined(_M_X64) || defined(_M_IX86)

        void IncrementFrames_SIMD_AVX2(
            std::vector<uint32_t>& FrameNums,
            uint32_t Size
        );

        void IncrementFrames_SIMD_AVX512(
            std::vector<uint32_t>& FrameNums,
            uint32_t Size
        );

        void IncrementFrames_SIMD_SSE2(
            std::vector<uint32_t>& FrameNums,
            uint32_t Size
        );

        #endif

        #if defined(__aarch64__) || defined(_M_ARM64)

        void IncrementFrames_SIMD_NEON(
            std::vector<uint32_t>& FrameNums,
            uint32_t Size
        );

        #endif
    }

void ObjectManager::IncrementFrames(
    std::vector<uint32_t>& FrameNums
) {
    uint32_t Size = FrameNums.size();

    switch(SIMDRegisterLevel)
    {
        #if defined(__x86_64__) || defined(__i386__) || \
            defined(_M_X64) || defined(_M_IX86)

        // 256 bit
        case SIMDLevel::AVX2:
            InternalHelpers::IncrementFrames_SIMD_AVX2(FrameNums, Size);
            break;

        // 512 bit
        case SIMDLevel::AVX512:
            InternalHelpers::IncrementFrames_SIMD_AVX512(FrameNums, Size);
            break;

        // 128 bit
        case SIMDLevel::SSE2:
            InternalHelpers::IncrementFrames_SIMD_SSE2(FrameNums, Size);
            break;

        #endif

        #if defined(__aarch64__) || defined(_M_ARM64)

        // 128 bit - NEON
        case SIMDLevel::NEON:
            InternalHelpers::IncrementFrames_SIMD_NEON(FrameNums, Size);
            break;

        #endif

        // Scalar fallback
        default:
            for(uint32_t i = 0; i < Size; ++i)
            {
                FrameNums[i]++;
            }
            break;
    }
}

// Helpers for incrementing with SIMD --------------------

namespace InternalHelpers {

    #if defined(__x86_64__) || defined(__i386__) || \
        defined(_M_X64) || defined(_M_IX86)

    __attribute__((target("avx2")))
    // AVX2 (256 bit - 8 uint32_t) - 8 at a time
    void IncrementFrames_SIMD_AVX2(
        std::vector<uint32_t>& FrameNums,
        uint32_t Size
    ) {
        // Helper
        __m256i One = _mm256_set1_epi32(1);

        // Loop
        uint32_t i = 0;
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
    // AVX512 (512 bit, 16 uint32_t) - 16 at a time
    void IncrementFrames_SIMD_AVX512(
        std::vector<uint32_t>& FrameNums,
        uint32_t Size
    ) {
        // Helper
        __m512i One = _mm512_set1_epi32(1);

        // Loop
        uint32_t i = 0;
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
    // SSE2 (128 bit, 4 uint32_t) - 4 at a time
    void IncrementFrames_SIMD_SSE2(
        std::vector<uint32_t>& FrameNums,
        uint32_t Size
    ) {
        // Helper
        __m128i One = _mm_set1_epi32(1);

        // Loop
        uint32_t i = 0;
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

    #endif


    #if defined(__aarch64__) || defined(_M_ARM64)

    // NEON (128 bit, 4 uint32_t) - 4 at a time
    void IncrementFrames_SIMD_NEON(
        std::vector<uint32_t>& FrameNums,
        uint32_t Size
    ) {
        // Helper
        uint32x4_t One = vdupq_n_u32(1);

        // Loop
        uint32_t i = 0;
        for(; i + 4 <= Size; i += 4)
        {
            // Frame nums
            uint32x4_t FrameData = vld1q_u32(
                &FrameNums[i]
            );

            // Add them
            FrameData = vaddq_u32(FrameData, One);

            // Store back
            vst1q_u32(
                &FrameNums[i],
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

    #endif

    // Scalar
    void IncrementFrames_Scalar(
        std::vector<uint32_t>& FrameNums,
        uint32_t Size
    ) {
        for(uint32_t i = 0; i < Size; i++)
        {
            FrameNums[i]++;
        }
    }

}

}