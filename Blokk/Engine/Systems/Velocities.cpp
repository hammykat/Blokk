#include "GameObject.hpp"
#include "ObjectUpdateStructs.hpp"
#include "SIMD_Finder.hpp"

#if defined(__x86_64__) || defined(__i386__) || \
    defined(_M_X64) || defined(_M_IX86)
    #include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
    #include <arm_neon.h>
#endif

namespace Blokk
{

    // Forward declarations for SIMD helper functions
    namespace InternalHelpers
    {
        #if defined(__x86_64__) || defined(__i386__) || \
            defined(_M_X64) || defined(_M_IX86)

        void ProcessVelocities_SIMD_AVX2(
            float* PosX, float* PosY,
            const float* VelX, const float* VelY,
            uint32_t Size
        );

        void ProcessVelocities_SIMD_AVX512(
            float* PosX, float* PosY,
            const float* VelX, const float* VelY,
            uint32_t Size
        );

        void ProcessVelocities_SIMD_SSE2(
            float* PosX, float* PosY,
            const float* VelX, const float* VelY,
            uint32_t Size
        );

        #elif defined(__aarch64__) || defined(_M_ARM64)

        void ProcessVelocities_SIMD_NEON(
            float* PosX, float* PosY,
            const float* VelX, const float* VelY,
            uint32_t Size
        );

        #endif
    }


// Update a range of positions
void ObjectManager::UpdateRangeOfPositions(
    IndexRange TRange,
    Worker* Thread
)
{
    float* XPos = &XPositions[TRange.Start];
    float* YPos = &YPositions[TRange.Start];

    float* XVels = &XVelocities[TRange.Start];
    float* YVels = &YVelocities[TRange.Start];

    uint32_t Size = TRange.GetSize();

    (this->*UpdatePositions)(
        XPos,
        YPos,
        XVels,
        YVels,
        Size
    );
}


template <SIMDLevel Level>
void ObjectManager::UpdatePositionsFn(
    float* PosX,
    float* PosY,
    const float* VelX,
    const float* VelY,
    uint32_t Size
)
{
    #if defined(__x86_64__) || defined(__i386__) || \
        defined(_M_X64) || defined(_M_IX86)

    // 256 bit
    if constexpr (Level == SIMDLevel::AVX2)
    {
        InternalHelpers::ProcessVelocities_SIMD_AVX2(
            PosX,
            PosY,
            VelX,
            VelY,
            Size
        );
    }

    // 512 bit
    else if constexpr (Level == SIMDLevel::AVX512)
    {
        InternalHelpers::ProcessVelocities_SIMD_AVX512(
            PosX,
            PosY,
            VelX,
            VelY,
            Size
        );
    }

    // 128 bit
    else if constexpr (Level == SIMDLevel::SSE2)
    {
        InternalHelpers::ProcessVelocities_SIMD_SSE2(
            PosX,
            PosY,
            VelX,
            VelY,
            Size
        );
    }


    #elif defined(__aarch64__) || defined(_M_ARM64)

    // NEON - 128 bit
    if constexpr (Level == SIMDLevel::NEON)
    {
        InternalHelpers::ProcessVelocities_SIMD_NEON(
            PosX,
            PosY,
            VelX,
            VelY,
            Size
        );
    }

    #endif

    // Scalar
    else
    {
        InternalHelpers::ProcessVelocities_Scalar(
            PosX,
            PosY,
            VelX,
            VelY,
            Size
        );
    }
}


// Helpers for SIMD velocities ---------------------------
namespace InternalHelpers
{

#if defined(__x86_64__) || defined(__i386__) || \
    defined(_M_X64) || defined(_M_IX86)

__attribute__((target("avx2")))
void ProcessVelocities_SIMD_AVX2(
    float* PosX,
    float* PosY,
    const float* VelX,
    const float* VelY,
    uint32_t Size
)
{
    // 256 bit - 8 floats at a time
    uint32_t i = 0;

    for (; i + 8 <= Size; i += 8)
    {
        // Positions
        __m256 XPos =
            _mm256_loadu_ps(&PosX[i]);

        __m256 YPos =
            _mm256_loadu_ps(&PosY[i]);

        // Velocities
        __m256 XVel =
            _mm256_loadu_ps(&VelX[i]);

        __m256 YVel =
            _mm256_loadu_ps(&VelY[i]);

        // Add velocities to positions
        XPos = _mm256_add_ps(XPos, XVel);
        YPos = _mm256_add_ps(YPos, YVel);

        // Store back
        _mm256_storeu_ps(&PosX[i], XPos);
        _mm256_storeu_ps(&PosY[i], YPos);
    }

    // Remainder
    for (; i < Size; ++i)
    {
        PosX[i] += VelX[i];
        PosY[i] += VelY[i];
    }
}


__attribute__((target("avx512f")))
void ProcessVelocities_SIMD_AVX512(
    float* PosX,
    float* PosY,
    const float* VelX,
    const float* VelY,
    uint32_t Size
)
{
    // 512 bit - 16 floats at a time
    uint32_t i = 0;

    for (; i + 16 <= Size; i += 16)
    {
        // Positions
        __m512 XPos =
            _mm512_loadu_ps(&PosX[i]);

        __m512 YPos =
            _mm512_loadu_ps(&PosY[i]);

        // Velocities
        __m512 XVel =
            _mm512_loadu_ps(&VelX[i]);

        __m512 YVel =
            _mm512_loadu_ps(&VelY[i]);

        // Add velocities to positions
        XPos = _mm512_add_ps(XPos, XVel);
        YPos = _mm512_add_ps(YPos, YVel);

        // Store back
        _mm512_storeu_ps(&PosX[i], XPos);
        _mm512_storeu_ps(&PosY[i], YPos);
    }

    // Remainder
    for (; i < Size; ++i)
    {
        PosX[i] += VelX[i];
        PosY[i] += VelY[i];
    }
}


__attribute__((target("sse2")))
void ProcessVelocities_SIMD_SSE2(
    float* PosX,
    float* PosY,
    const float* VelX,
    const float* VelY,
    uint32_t Size
)
{
    // 128 bit - 4 floats at a time
    uint32_t i = 0;

    for (; i + 4 <= Size; i += 4)
    {
        // Positions
        __m128 XPos =
            _mm_loadu_ps(&PosX[i]);

        __m128 YPos =
            _mm_loadu_ps(&PosY[i]);

        // Velocities
        __m128 XVel =
            _mm_loadu_ps(&VelX[i]);

        __m128 YVel =
            _mm_loadu_ps(&VelY[i]);

        // Add velocities to positions
        XPos = _mm_add_ps(XPos, XVel);
        YPos = _mm_add_ps(YPos, YVel);

        // Store back
        _mm_storeu_ps(&PosX[i], XPos);
        _mm_storeu_ps(&PosY[i], YPos);
    }

    // Remainder
    for (; i < Size; ++i)
    {
        PosX[i] += VelX[i];
        PosY[i] += VelY[i];
    }
}

#endif

#if defined(__aarch64__) || defined(_M_ARM64)

void ProcessVelocities_SIMD_NEON(
    float* PosX,
    float* PosY,
    const float* VelX,
    const float* VelY,
    uint32_t Size
)
{
    // 128 bit - 4 floats at a time
    uint32_t i = 0;

    for (; i + 4 <= Size; i += 4)
    {
        // Positions
        float32x4_t XPos =
            vld1q_f32(&PosX[i]);

        float32x4_t YPos =
            vld1q_f32(&PosY[i]);

        // Velocities
        float32x4_t XVel =
            vld1q_f32(&VelX[i]);

        float32x4_t YVel =
            vld1q_f32(&VelY[i]);

        // Add velocities to positions
        XPos = vaddq_f32(XPos, XVel);
        YPos = vaddq_f32(YPos, YVel);

        // Store back
        vst1q_f32(&PosX[i], XPos);
        vst1q_f32(&PosY[i], YPos);
    }

    // Remainder
    for (; i < Size; ++i)
    {
        PosX[i] += VelX[i];
        PosY[i] += VelY[i];
    }
}

#endif

// Scalar
void ProcessVelocities_Scalar(
    float* PosX,
    float* PosY,
    const float* VelX,
    const float* VelY,
    uint32_t Size
) {
    for(uint32_t i = 0; i < Size; i++)
    {
        PosX[i] += VelX[i];
        PosY[i] += VelY[i];
    }
}


}

}