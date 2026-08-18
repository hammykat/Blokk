#include <emmintrin.h>
#include <immintrin.h>

#include "EngineClassData.hpp"

void ObjectManager::CheckVisibleRange(IndexRange Range, Worker* Thread)
{
    uint32_t Start = Range.Start;
    switch(SIMDRegisterLevel)
    {
        // 256 bit
        case SIMDLevel::AVX:
        case SIMDLevel::AVX2:
            Thread->IdxResult =
            CheckVisible_SIMD_AVX2(
                ScreenWidth, ScreenHeight,
                &AnimWidths[Start],
                &AnimHeights[Start],
                &XPositions[Start],
                &YPositions[Start],
                Range.GetSize(),
                Range.Start
            );
            break;

        // 512 bit
        case SIMDLevel::AVX512:
            Thread->IdxResult =
            CheckVisible_SIMD_AVX512(
                ScreenWidth, ScreenHeight,
                &AnimWidths[Start],
                &AnimHeights[Start],
                &XPositions[Start],
                &YPositions[Start],
                Range.GetSize(),
                Range.Start
            );
            break;

        // 128 bit
        default:
            Thread->IdxResult =
            CheckVisible_SIMD_SSE2(
                ScreenWidth, ScreenHeight,
                &AnimWidths[Start],
                &AnimHeights[Start],
                &XPositions[Start],
                &YPositions[Start],
                Range.GetSize(),
                Range.Start
            );
    }
}

// AXV / AXV2 (256 bit - 8 floats) - 8 at a time
vector<uint32_t> CheckVisible_SIMD_AVX2( 
    uint32_t ScreenWidth, uint32_t ScreenHeight,
    uint32_t *AnimWidths, uint32_t *AnimHeights,
    float* PosX, float* PosY,
    uint32_t Size, uint32_t StartIdx
) {
    __m256 Zero = _mm256_set1_ps(0);
    vector<uint32_t> ResultIdx;

    // Screen dimensions
    __m256 ScrHeight = _mm256_set1_ps(ScreenHeight);
    __m256 ScrWidth = _mm256_set1_ps(ScreenWidth);

    // Loop
    size_t i = 0;
    for(; i + 8 <= Size; i += 8)
    {
        // Positions
        __m256 MinXPos = _mm256_loadu_ps(&PosX[i]);
        __m256 MinYPos = _mm256_loadu_ps(&PosY[i]);
        
        // Anim sizes
        __m256i IntHeights = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(&AnimHeights[i]));
        __m256i IntWidths = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(&AnimWidths[i]));
        __m256 Widths = _mm256_cvtepi32_ps(IntWidths);
        __m256 Heights = _mm256_cvtepi32_ps(IntHeights);

        // Add to get max positions
        __m256 MaxXPos = _mm256_add_ps(MinXPos, Widths);
        __m256 MaxYPos = _mm256_add_ps(MinYPos, Heights);

        // Compare them
        __m256 XRes = _mm256_cmp_ps(MinXPos, ScrWidth, _CMP_LT_OQ); // MinX < ScrWidth
        __m256 YRes = _mm256_cmp_ps(MinYPos, ScrHeight, _CMP_LT_OQ); // MinY < ScrHeight
        __m256 XRes0 = _mm256_cmp_ps(MaxXPos, Zero, _CMP_GT_OQ); // MaxX > 0
        __m256 YRes0 = _mm256_cmp_ps(MaxYPos, Zero, _CMP_GT_OQ); // MaxY > 0

        // Combine
        __m256 XResComb = _mm256_and_ps(XRes, XRes0);
        __m256 YResComb = _mm256_and_ps(YRes, YRes0);
        __m256 FinalRes = _mm256_and_ps(XResComb, YResComb);

        // Convert to int
        int Result = _mm256_movemask_ps(FinalRes);

        // If at least one result is true
        if(Result != 0)
        {
            // Loop through bits
            for (int l = 0; l < 8; ++l)
            {
                // If bit i true
                if (Result & (1 << l))
                {
                    // Store the index
                    ResultIdx.push_back(StartIdx + i + l);
                }
            }
        }
    }

    // Loop through the rest of the items
    for(; i < Size; ++i)
    {
        // Store Min
        float XMin = PosX[i];
        float YMin = PosY[i];

        // Get Max
        float XMax = PosX[i] + AnimWidths[i];
        float YMax = PosY[i] + AnimHeights[i];

        // Comparisons
        if(XMax > 0 && YMax > 0 && XMin < ScreenWidth && YMin < ScreenHeight) 
        {
            // Store index
            ResultIdx.push_back(StartIdx + i);
        }
    }

    // Return the result
    return ResultIdx;
}


vector<uint32_t> CheckVisible_SIMD_SSE2(
    uint32_t ScreenWidth,
    uint32_t ScreenHeight,
    uint32_t* AnimWidths,
    uint32_t* AnimHeights,
    float* PosX,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
) {
    vector<uint32_t> ResultIdx;
    ResultIdx.reserve(Size);

    const __m128 Zero = _mm_setzero_ps();
    const __m128 ScrWidth = _mm_set1_ps(
        static_cast<float>(ScreenWidth));
    const __m128 ScrHeight = _mm_set1_ps(
        static_cast<float>(ScreenHeight));

    size_t i = 0;

    // Process 4 at a time
    for (; i + 4 <= Size; i += 4)
    {
        // Positions
        __m128 MinXPos = _mm_loadu_ps(&PosX[i]);
        __m128 MinYPos = _mm_loadu_ps(&PosY[i]);

        // Load 4 integer widths/heights
        __m128i IntWidths = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(&AnimWidths[i]));

        __m128i IntHeights = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(&AnimHeights[i]));

        // Convert int -> float
        __m128 Widths = _mm_cvtepi32_ps(IntWidths);
        __m128 Heights = _mm_cvtepi32_ps(IntHeights);

        // Max positions
        __m128 MaxXPos = _mm_add_ps(MinXPos, Widths);
        __m128 MaxYPos = _mm_add_ps(MinYPos, Heights);

        // Comparisons
        __m128 XRes = _mm_cmplt_ps(MaxXPos, ScrWidth);
        __m128 YRes = _mm_cmplt_ps(MaxYPos, ScrHeight);

        __m128 XRes0 = _mm_cmpgt_ps(MinXPos, Zero);
        __m128 YRes0 = _mm_cmpgt_ps(MinYPos, Zero);

        // Combine
        __m128 XResComb = _mm_and_ps(XRes, XRes0);
        __m128 YResComb = _mm_and_ps(YRes, YRes0);
        __m128 FinalRes = _mm_and_ps(XResComb, YResComb);

        // Convert 4 lanes -> 4-bit mask
        int Mask = _mm_movemask_ps(FinalRes);

        if (Mask != 0)
        {
            for (int lane = 0; lane < 4; ++lane)
            {
                if (Mask & (1 << lane))
                {
                    ResultIdx.push_back(
                        StartIdx +
                        static_cast<uint32_t>(i) +
                        static_cast<uint32_t>(lane)
                    );
                }
            }
        }
    }

    // Remainder
    for (; i < Size; ++i)
    {
        float XMin = PosX[i];
        float YMin = PosY[i];

        float XMax =
            XMin + static_cast<float>(AnimWidths[i]);

        float YMax =
            YMin + static_cast<float>(AnimHeights[i]);

        if (XMin > 0 &&
            YMin > 0 &&
            XMax < ScreenWidth &&
            YMax < ScreenHeight)
        {
            ResultIdx.push_back(
                StartIdx + static_cast<uint32_t>(i)
            );
        }
    }

    return ResultIdx;
}


vector<uint32_t> CheckVisible_SIMD_AVX512(
    uint32_t ScreenWidth,
    uint32_t ScreenHeight,
    uint32_t* AnimWidths,
    uint32_t* AnimHeights,
    float* PosX,
    float* PosY,
    uint32_t Size,
    uint32_t StartIdx
) {
    vector<uint32_t> ResultIdx;
    ResultIdx.reserve(Size);

    const __m512 Zero = _mm512_setzero_ps();
    const __m512 ScrWidth = _mm512_set1_ps(
        static_cast<float>(ScreenWidth));
    const __m512 ScrHeight = _mm512_set1_ps(
        static_cast<float>(ScreenHeight));

    size_t i = 0;

    // Process 16 at a time
    for (; i + 16 <= Size; i += 16)
    {
        // Positions
        __m512 MinXPos = _mm512_loadu_ps(&PosX[i]);
        __m512 MinYPos = _mm512_loadu_ps(&PosY[i]);

        // Load 16 integer widths/heights
        __m512i IntWidths = _mm512_loadu_si512(
            reinterpret_cast<const void*>(&AnimWidths[i]));

        __m512i IntHeights = _mm512_loadu_si512(
            reinterpret_cast<const void*>(&AnimHeights[i]));

        // Convert int -> float
        __m512 Widths = _mm512_cvtepi32_ps(IntWidths);
        __m512 Heights = _mm512_cvtepi32_ps(IntHeights);

        // Max positions
        __m512 MaxXPos = _mm512_add_ps(MinXPos, Widths);
        __m512 MaxYPos = _mm512_add_ps(MinYPos, Heights);

        // Comparisons
        __mmask16 XRes =
            _mm512_cmp_ps_mask(
                MaxXPos,
                ScrWidth,
                _CMP_LT_OQ
            );

        __mmask16 YRes =
            _mm512_cmp_ps_mask(
                MaxYPos,
                ScrHeight,
                _CMP_LT_OQ
            );

        __mmask16 XRes0 =
            _mm512_cmp_ps_mask(
                MinXPos,
                Zero,
                _CMP_GT_OQ
            );

        __mmask16 YRes0 =
            _mm512_cmp_ps_mask(
                MinYPos,
                Zero,
                _CMP_GT_OQ
            );

        // Combine masks
        __mmask16 FinalRes =
            XRes & YRes & XRes0 & YRes0;

        // If at least one lane passed
        if (FinalRes != 0)
        {
            for (int lane = 0; lane < 16; ++lane)
            {
                if (FinalRes & (1u << lane))
                {
                    ResultIdx.push_back(
                        StartIdx +
                        static_cast<uint32_t>(i) +
                        static_cast<uint32_t>(lane)
                    );
                }
            }
        }
    }

    // Remainder
    for (; i < Size; ++i)
    {
        float XMin = PosX[i];
        float YMin = PosY[i];

        float XMax =
            XMin + static_cast<float>(AnimWidths[i]);

        float YMax =
            YMin + static_cast<float>(AnimHeights[i]);

        if (XMin > 0 &&
            YMin > 0 &&
            XMax < ScreenWidth &&
            YMax < ScreenHeight)
        {
            ResultIdx.push_back(
                StartIdx + static_cast<uint32_t>(i)
            );
        }
    }

    return ResultIdx;
}