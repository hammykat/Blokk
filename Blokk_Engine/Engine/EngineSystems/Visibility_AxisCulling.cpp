#include <emmintrin.h>
#include <immintrin.h>

#include "EngineClassData.hpp"

// Forward declarations

vector<uint32_t> CheckVisible_Axis_SIMD_AVX2(
    uint32_t ScreenWidth,
    uint32_t ScreenHeight,
    uint32_t* AnimWidths,
    uint32_t* AnimHeights,
    float* PosX,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
);

vector<uint32_t> CheckVisible_Axis_SIMD_AVX512(
    uint32_t ScreenWidth,
    uint32_t ScreenHeight,
    uint32_t* AnimWidths,
    uint32_t* AnimHeights,
    float* PosX,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
);

vector<uint32_t> CheckVisible_Axis_SIMD_SSE2(
    uint32_t ScreenWidth,
    uint32_t ScreenHeight,
    uint32_t* AnimWidths,
    uint32_t* AnimHeights,
    float* PosX,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
);

// ObjectManager Visibility Function

template <SIMDLevel Level>
void ObjectManager::CheckVisibilityFn_Axis(IndexRange Range, Worker* Thread) 
{
    // AVX / AVX2
    // 256-bit = 8 floats

    if constexpr (Level == SIMDLevel::AVX2) {
        Thread->IdxResult =
            CheckVisible_Axis_SIMD_AVX2(
                ScreenWidth,
                ScreenHeight,

                &AnimWidths[Range.Start],
                &AnimHeights[Range.Start],
                &XPositions[Range.Start],
                &YPositions[Range.Start],

                Range.GetSize(),
                Range.Start
            );
    }

    // AVX-512
    // 512-bit = 16 floats

    else if constexpr (Level == SIMDLevel::AVX512) {
        Thread->IdxResult =
            CheckVisible_Axis_SIMD_AVX512(
                ScreenWidth,
                ScreenHeight,

                &AnimWidths[Range.Start],
                &AnimHeights[Range.Start],
                &XPositions[Range.Start],
                &YPositions[Range.Start],

                Range.GetSize(),
                Range.Start
            );
    }

    // SSE2
    // 128-bit = 4 floats

    else {
        Thread->IdxResult =
            CheckVisible_Axis_SIMD_SSE2(
                ScreenWidth,
                ScreenHeight,

                &AnimWidths[Range.Start],
                &AnimHeights[Range.Start],
                &XPositions[Range.Start],
                &YPositions[Range.Start],

                Range.GetSize(),
                Range.Start
            );
    }
}

// AVX2 - X + Y Visibility

__attribute__((target("avx2")))
vector<uint32_t> CheckVisible_Axis_SIMD_AVX2(
    uint32_t ScreenWidth,
    uint32_t ScreenHeight,
    uint32_t* AnimWidths,
    uint32_t* AnimHeights,
    float* PosX,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
) {
    vector<uint32_t> Result;

    // First cull against Y axis
    vector<uint32_t> YAxisCulled = CheckVisible_AxisY_SIMD_AVX2(
        ScreenHeight,
        AnimHeights,
        PosY,
        Size,
        StartIdx
    );

    // Then check X axis
    for (uint32_t Idx : YAxisCulled) {
        float MinX = PosX[Idx];
        float MaxX = MinX + AnimWidths[Idx];

        if (MaxX > 0 && MinX < ScreenWidth) {
            Result.push_back(Idx);
        }
    }

    return Result;
}

// AVX2 - Y Visibility

__attribute__((target("avx2")))
vector<uint32_t> CheckVisible_AxisY_SIMD_AVX2(
    uint32_t ScreenHeight,
    uint32_t* AnimHeights,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
) {
    __m256 Zero = _mm256_set1_ps(0.0f);
    __m256 ScrHeight = _mm256_set1_ps(static_cast<float>(ScreenHeight));

    vector<uint32_t> ResultIdx;

    uint32_t i = 0;

    // Process 8 objects at a time

    for (; i + 8 <= Size; i += 8) {
        // Positions
        __m256 MinYPos = _mm256_loadu_ps(&PosY[i]);

        // Animation heights
        __m256i IntHeights = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(&AnimHeights[i])
        );

        // uint32 -> float
        __m256 Heights = _mm256_cvtepi32_ps(IntHeights);

        // Max Y
        __m256 MaxYPos = _mm256_add_ps(MinYPos, Heights);

        // MinY < ScreenHeight
        __m256 YRes = _mm256_cmp_ps(MinYPos, ScrHeight, _CMP_LT_OQ);

        // MaxY > 0
        __m256 YRes0 = _mm256_cmp_ps(MaxYPos, Zero, _CMP_GT_OQ);

        // Combine
        __m256 YResult = _mm256_and_ps(YRes, YRes0);

        // Convert to 8-bit mask
        int Result = _mm256_movemask_ps(YResult);

        // Store visible indexes
        if (Result != 0) {
            for (int l = 0; l < 8; ++l) {
                if (Result & (1 << l)) {
                    ResultIdx.push_back(StartIdx + i + l);
                }
            }
        }
    }

    // Remaining objects

    for (; i < Size; ++i) {
        float YMin = PosY[i];
        float YMax = PosY[i] + AnimHeights[i];

        if (YMax > 0 && YMin < ScreenHeight) {
            ResultIdx.push_back(StartIdx + i);
        }
    }

    return ResultIdx;
}

// SSE2 - X + Y Visibility

__attribute__((target("sse2")))
vector<uint32_t> CheckVisible_Axis_SIMD_SSE2(
    uint32_t ScreenWidth,
    uint32_t ScreenHeight,
    uint32_t* AnimWidths,
    uint32_t* AnimHeights,
    float* PosX,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
) {
    vector<uint32_t> Result;

    // First cull against Y axis
    vector<uint32_t> YAxisCulled = CheckVisible_AxisY_SIMD_SSE2(
        ScreenHeight,
        AnimHeights,
        PosY,
        Size,
        StartIdx
    );

    // Then check X axis
    for (uint32_t Idx : YAxisCulled) {
        float MinX = PosX[Idx];
        float MaxX = MinX + AnimWidths[Idx];

        if (MaxX > 0 && MinX < ScreenWidth) {
            Result.push_back(Idx);
        }
    }

    return Result;
}

// SSE2 - Y Visibility

__attribute__((target("sse2")))
vector<uint32_t> CheckVisible_AxisY_SIMD_SSE2(
    uint32_t ScreenHeight,
    uint32_t* AnimHeights,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
) {
    __m128 Zero = _mm_set1_ps(0.0f);
    __m128 ScrHeight = _mm_set1_ps(static_cast<float>(ScreenHeight));

    vector<uint32_t> ResultIdx;

    uint32_t i = 0;

    // Process 4 objects at a time

    for (; i + 4 <= Size; i += 4) {
        // Positions
        __m128 MinYPos = _mm_loadu_ps(&PosY[i]);

        // Animation heights
        __m128i IntHeights = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(&AnimHeights[i])
        );

        // uint32 -> float
        __m128 Heights = _mm_cvtepi32_ps(IntHeights);

        // Max Y
        __m128 MaxYPos = _mm_add_ps(MinYPos, Heights);

        // MinY < ScreenHeight
        __m128 YRes = _mm_cmplt_ps(MinYPos, ScrHeight);

        // MaxY > 0
        __m128 YRes0 = _mm_cmpgt_ps(MaxYPos, Zero);

        // Combine
        __m128 YResult = _mm_and_ps(YRes, YRes0);

        // Convert to 4-bit mask
        int Result = _mm_movemask_ps(YResult);

        // Store visible indexes
        if (Result != 0) {
            for (int l = 0; l < 4; ++l) {
                if (Result & (1 << l)) {
                    ResultIdx.push_back(StartIdx + i + l);
                }
            }
        }
    }

    // Remaining objects

    for (; i < Size; ++i) {
        float YMin = PosY[i];
        float YMax = PosY[i] + AnimHeights[i];

        if (YMax > 0 && YMin < ScreenHeight) {
            ResultIdx.push_back(StartIdx + i);
        }
    }

    return ResultIdx;
}

// AVX-512 - X + Y Visibility

__attribute__((target("avx512f")))
vector<uint32_t> CheckVisible_Axis_SIMD_AVX512(
    uint32_t ScreenWidth,
    uint32_t ScreenHeight,
    uint32_t* AnimWidths,
    uint32_t* AnimHeights,
    float* PosX,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
) {
    vector<uint32_t> Result;

    // First cull against Y axis
    vector<uint32_t> YAxisCulled = 
    CheckVisible_AxisY_SIMD_AVX512(
        ScreenHeight,
        AnimHeights,
        PosY,
        Size,
        StartIdx
    );

    // Then check X axis
    for (uint32_t Idx : YAxisCulled) 
    {
        float MinX = PosX[Idx];
        float MaxX = MinX + AnimWidths[Idx];

        if (MaxX > 0 && MinX < ScreenWidth) {
            Result.push_back(Idx);
        }
    }

    return Result;
}

// AVX-512 - Y Visibility

__attribute__((target("avx512f")))
vector<uint32_t> CheckVisible_AxisY_SIMD_AVX512(
    uint32_t ScreenHeight,
    uint32_t* AnimHeights,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
) {
    __m512 Zero = _mm512_set1_ps(0.0f);
    __m512 ScrHeight = _mm512_set1_ps(static_cast<float>(ScreenHeight));

    vector<uint32_t> ResultIdx;

    uint32_t i = 0;

    // Process 16 objects at a time

    for (; i + 16 <= Size; i += 16) {
        // Positions
        __m512 MinYPos = _mm512_loadu_ps(&PosY[i]);

        // Animation heights
        __m512i IntHeights = _mm512_loadu_si512(
            reinterpret_cast<const void*>(&AnimHeights[i])
        );

        // uint32 -> float
        __m512 Heights = _mm512_cvtepi32_ps(IntHeights);

        // Max Y
        __m512 MaxYPos = _mm512_add_ps(MinYPos, Heights);

        // MinY < ScreenHeight
        __mmask16 YRes = _mm512_cmp_ps_mask(
            MinYPos,
            ScrHeight,
            _CMP_LT_OQ
        );

        // MaxY > 0
        __mmask16 YRes0 = _mm512_cmp_ps_mask(
            MaxYPos,
            Zero,
            _CMP_GT_OQ
        );

        // Combine masks
        __mmask16 YResult = YRes & YRes0;

        // Store visible indexes
        if (YResult != 0) {
            for (int l = 0; l < 16; ++l) {
                if (YResult & (1u << l)) {
                    ResultIdx.push_back(StartIdx + i + l);
                }
            }
        }
    }

    // Remaining objects

    for (; i < Size; ++i) {
        float YMin = PosY[i];
        float YMax = PosY[i] + AnimHeights[i];

        if (YMax > 0 && YMin < ScreenHeight) {
            ResultIdx.push_back(StartIdx + i);
        }
    }

    return ResultIdx;
}