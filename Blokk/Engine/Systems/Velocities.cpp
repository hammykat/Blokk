#include "GameObject.hpp"
#include "ObjectUpdateStructs.hpp"
#include "SIMD_Finder.hpp"

namespace Blokk {

    // Forward declarations for SIMD helper functions
    namespace InternalHelpers 
    {
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
    }

// Update a range of positions
void ObjectManager::UpdateRangeOfPositions(IndexRange TRange, Worker* Thread)
{
    float *XPos = &XPositions[TRange.Start];
    float *YPos = &YPositions[TRange.Start];
    float *XVels = &XVelocities[TRange.Start];
    float *YVels = &YVelocities[TRange.Start];

    uint32_t Size = TRange.GetSize();
    (this->*UpdatePositions)(XPos, YPos, XVels, YVels, Size);
}

template <SIMDLevel Level>
void ObjectManager::UpdatePositionsFn(
    float* PosX, float* PosY, 
    const float* VelX, const float *VelY, 
    uint32_t Size
) {
    // 256 bit
    if constexpr (Level == SIMDLevel::AVX2)
    {
        InternalHelpers::ProcessVelocities_SIMD_AVX2(PosX, PosY, VelX, VelY, Size);
    }
    // 512 bit
    else if constexpr (Level == SIMDLevel::AVX512)
    {
        InternalHelpers::ProcessVelocities_SIMD_AVX512(PosX, PosY, VelX, VelY, Size);
    }
    // 128 bit - default
    else
    {
        InternalHelpers::ProcessVelocities_SIMD_SSE2(PosX, PosY, VelX, VelY, Size);
    }
}

// Helpers for SIMD velocities ---------------------------
namespace InternalHelpers 
{

__attribute__((target("avx2")))
// AXV / AXV2 (256 bit - 8 floats) - 8 at a time
void ProcessVelocities_SIMD_AVX2(
    float* PosX, float* PosY, 
    const float* VelX, const float* VelY, 
    uint32_t Size
) {
    // Loop
    uint32_t i = 0;
    for(; i + 8 <= Size; i += 8)
    {
        // Positions
        __m256 XPos = _mm256_loadu_ps(&PosX[i]);
        __m256 YPos = _mm256_loadu_ps(&PosY[i]);
                
        // Velocities
        __m256 XVel = _mm256_loadu_ps(&VelX[i]);
        __m256 YVel = _mm256_loadu_ps(&VelY[i]);

        // Add them
        XPos = _mm256_add_ps(XPos, XVel);
        YPos = _mm256_add_ps(YPos, YVel);

        // Store back
        _mm256_storeu_ps(&PosX[i], XPos);
        _mm256_storeu_ps(&PosY[i], YPos);
    }

    // Make sure to do the last ones
        
    // Loop through the rest of the items
    for(; i < Size; ++i)
    {
        // Update x
        PosX[i] += VelX[i];
            
        // Update y
        PosY[i] += VelY[i];
    }
}


__attribute__((target("avx512f")))
// AXV512 (512 bit, 16 floats) - 16 at a time
void ProcessVelocities_SIMD_AVX512(
    float* PosX, float* PosY, 
    const float* VelX, const float* VelY, 
    uint32_t Size
) {
    // Loop
    uint32_t i = 0;
    for(; i + 16 <= Size; i += 16)
    {
        // Positions
        __m512 XPos = _mm512_loadu_ps(&PosX[i]);
        __m512 YPos = _mm512_loadu_ps(&PosY[i]);
                
        // Velocities
        __m512 XVel = _mm512_loadu_ps(&VelX[i]);
        __m512 YVel = _mm512_loadu_ps(&VelY[i]);

        // Add them
        XPos = _mm512_add_ps(XPos, XVel);
        YPos = _mm512_add_ps(YPos, YVel);

        // Store back
        _mm512_storeu_ps(&PosX[i], XPos);
        _mm512_storeu_ps(&PosY[i], YPos);
    }

    // Make sure to do the last ones
        
    // Loop through the rest of the items
    for(; i < Size; ++i)
    {
        // Update x
        PosX[i] += VelX[i];
            
        // Update y
        PosY[i] += VelY[i];
    }
}


__attribute__((target("sse2")))
// SSE2 (128 bit, 4 floats) - 4 at a time
void ProcessVelocities_SIMD_SSE2(
    float* PosX, float* PosY, 
    const float* VelX, const float* VelY, 
    uint32_t Size
) {
    // Loop
    uint32_t i = 0;
    for(; i + 4 <= Size; i += 4)
    {
        // Positions
        __m128 XPos = _mm_loadu_ps(&PosX[i]);
        __m128 YPos = _mm_loadu_ps(&PosY[i]);
                
        // Velocities
        __m128 XVel = _mm_loadu_ps(&VelX[i]);
        __m128 YVel = _mm_loadu_ps(&VelY[i]);

        // Add them
        XPos = _mm_add_ps(XPos, XVel);
        YPos = _mm_add_ps(YPos, YVel);

        // Store back
        _mm_storeu_ps(&PosX[i], XPos);
        _mm_storeu_ps(&PosY[i], YPos);
    }

    // Make sure to do the last ones
        
    // Loop through the rest of the items
    for(; i < Size; ++i)
    {
        // Update x
        PosX[i] += VelX[i];
            
        // Update y
        PosY[i] += VelY[i];
    }
}

}

}