#include "raylib.h"
#include "GameObject.hpp"
#include "ObjectUpdateStructs.hpp"
#include "SIMD_Finder.hpp"

// Forward declarations for SIMD helper functions
void ProcessVelocities_SIMD_AVX2(
    float* PosX, float* PosY, 
    const float* VelX, const float* VelY, 
    size_t Size
);

void ProcessVelocities_SIMD_AVX512(
    float* PosX, float* PosY, 
    const float* VelX, const float* VelY, 
    size_t Size
);

void ProcessVelocities_SIMD_SSE2(
    float* PosX, float* PosY, 
    const float* VelX, const float* VelY, 
    size_t Size
);

// Update a range of positions
void ObjectManager::UpdateRangeOfPositions(IndexRange TRange)
{
    float *XPos = &DynamicXPositions[TRange.Start];
    float *YPos = &DynamicYPositions[TRange.Start];
    float *XVels = &XVelocities[TRange.Start];
    float *YVels = &YVelocities[TRange.Start];

    size_t Size = TRange.GetSize();
    UpdatePositions(XPos, YPos, XVels, YVels, Size);
}

void ObjectManager::UpdatePositions(
    float* PosX, float* PosY, 
    const float* VelX, const float *VelY, 
    size_t Size
) {
    switch(SIMDRegisterLevel) 
    {
        // 256 bit
        case SIMDLevel::AVX:
        case SIMDLevel::AVX2:
            ProcessVelocities_SIMD_AVX2(PosX, PosY, VelX, VelY, Size);
            break;

        // 512 bit
        case SIMDLevel::AVX512:
            ProcessVelocities_SIMD_AVX512(PosX, PosY, VelX, VelY, Size);
            break;

        // 128 bit
        default:
            ProcessVelocities_SIMD_SSE2(PosX, PosY, VelX, VelY, Size);
    }
    
}

// Helpers for SIMD velocities ---------------------------

// AXV / AXV2 (256 bit - 8 floats) - 8 at a time
void ProcessVelocities_SIMD_AVX2(
    float* PosX, float* PosY, 
    const float* VelX, const float* VelY, 
    size_t Size
) {
    // Loop
    size_t i = 0;
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



// AXV512 (512 bit, 16 floats) - 16 at a time
void ProcessVelocities_SIMD_AVX512(
    float* PosX, float* PosY, 
    const float* VelX, const float* VelY, 
    size_t Size
) {
    // Loop
    size_t i = 0;
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



// SSE2 (128 bit, 4 floats) - 4 at a time
void ProcessVelocities_SIMD_SSE2(
    float* PosX, float* PosY, 
    const float* VelX, const float* VelY, 
    size_t Size
) {
    // Loop
    size_t i = 0;
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