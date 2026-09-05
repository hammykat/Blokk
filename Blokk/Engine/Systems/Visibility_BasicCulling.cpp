#include <cstdint>
#include <vector>

#include <emmintrin.h>
#include <immintrin.h>

#include "EngineClassData.hpp"

#if (Blokk_Visibility_CullType == 0)

namespace Blokk
{

    // Forward declarations for SIMD helper functions
    namespace InternalHelpers
    {
        std::vector<std::uint32_t> CheckVisible_SIMD_AVX2(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
        );

        std::vector<std::uint32_t> CheckVisible_SIMD_AVX512(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
        );

        std::vector<std::uint32_t> CheckVisible_SIMD_SSE2(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
        );
    }


    // ============================================================
    // ObjectManager Visibility Function
    // ============================================================

    template <SIMDLevel Level>
    void ObjectManager::CheckVisibilityFn_Basic(
        IndexRange Range,
        Worker* Thread
    )
    {
        // 256-bit
        if constexpr (
            Level == SIMDLevel::AVX ||
            Level == SIMDLevel::AVX2
        )
        {
            Thread->IdxResult =
                InternalHelpers::CheckVisible_SIMD_AVX2(
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

        // 512-bit
        else if constexpr (Level == SIMDLevel::AVX512)
        {
            Thread->IdxResult =
                InternalHelpers::CheckVisible_SIMD_AVX512(
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

        // 128-bit - default
        else
        {
            Thread->IdxResult =
                InternalHelpers::CheckVisible_SIMD_SSE2(
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


    namespace InternalHelpers
    {


        // ============================================================
        // AVX2
        // ============================================================

        __attribute__((target("avx2")))
        std::vector<std::uint32_t> CheckVisible_SIMD_AVX2(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
        )
        {
            std::vector<std::uint32_t> ResultIdx;
            ResultIdx.reserve(Size);

            const __m256 Zero =
                _mm256_setzero_ps();

            const __m256 ScrWidth =
                _mm256_set1_ps(
                    static_cast<float>(ScreenWidth)
                );

            const __m256 ScrHeight =
                _mm256_set1_ps(
                    static_cast<float>(ScreenHeight)
                );

            #ifdef Blokk_CamEnabled

            const __m256 CamX =
                _mm256_set1_ps(CameraX);

            const __m256 CamY =
                _mm256_set1_ps(CameraY);

            #endif


            std::uint32_t i = 0;


            // --------------------------------------------------------
            // Process 8 objects at a time
            // --------------------------------------------------------

            for (; i + 8 <= Size; i += 8)
            {
                // Positions
                __m256 MinXPos =
                    _mm256_loadu_ps(&PosX[i]);

                __m256 MinYPos =
                    _mm256_loadu_ps(&PosY[i]);


                // Animation sizes
                __m256i IntHeights =
                    _mm256_loadu_si256(
                        reinterpret_cast<const __m256i*>(
                            &AnimHeights[i]
                        )
                    );

                __m256i IntWidths =
                    _mm256_loadu_si256(
                        reinterpret_cast<const __m256i*>(
                            &AnimWidths[i]
                        )
                    );


                // Convert uint32 -> float
                __m256 Widths =
                    _mm256_cvtepi32_ps(IntWidths);

                __m256 Heights =
                    _mm256_cvtepi32_ps(IntHeights);


                // Maximum positions
                __m256 MaxXPos =
                    _mm256_add_ps(
                        MinXPos,
                        Widths
                    );

                __m256 MaxYPos =
                    _mm256_add_ps(
                        MinYPos,
                        Heights
                    );


                // ----------------------------------------------------
                // Camera transformation
                // ----------------------------------------------------

                #ifdef Blokk_CamEnabled

                MinXPos =
                    _mm256_sub_ps(
                        MinXPos,
                        CamX
                    );

                MaxXPos =
                    _mm256_sub_ps(
                        MaxXPos,
                        CamX
                    );

                MinYPos =
                    _mm256_sub_ps(
                        MinYPos,
                        CamY
                    );

                MaxYPos =
                    _mm256_sub_ps(
                        MaxYPos,
                        CamY
                    );

                #endif


                // ----------------------------------------------------
                // Visibility tests
                // ----------------------------------------------------

                // MinX < ScreenWidth
                __m256 XRes =
                    _mm256_cmp_ps(
                        MinXPos,
                        ScrWidth,
                        _CMP_LT_OQ
                    );

                // MinY < ScreenHeight
                __m256 YRes =
                    _mm256_cmp_ps(
                        MinYPos,
                        ScrHeight,
                        _CMP_LT_OQ
                    );

                // MaxX > 0
                __m256 XRes0 =
                    _mm256_cmp_ps(
                        MaxXPos,
                        Zero,
                        _CMP_GT_OQ
                    );

                // MaxY > 0
                __m256 YRes0 =
                    _mm256_cmp_ps(
                        MaxYPos,
                        Zero,
                        _CMP_GT_OQ
                    );


                // Combine
                __m256 XResComb =
                    _mm256_and_ps(
                        XRes,
                        XRes0
                    );

                __m256 YResComb =
                    _mm256_and_ps(
                        YRes,
                        YRes0
                    );

                __m256 FinalRes =
                    _mm256_and_ps(
                        XResComb,
                        YResComb
                    );


                // Convert to mask
                int Result =
                    _mm256_movemask_ps(
                        FinalRes
                    );


                // Store visible indexes
                if (Result != 0)
                {
                    for (int lane = 0; lane < 8; ++lane)
                    {
                        if (Result & (1 << lane))
                        {
                            ResultIdx.push_back(
                                StartIdx +
                                i +
                                static_cast<std::uint32_t>(lane)
                            );
                        }
                    }
                }
            }


            // --------------------------------------------------------
            // Remaining objects
            // --------------------------------------------------------

            for (; i < Size; ++i)
            {
                float XMin = PosX[i];
                float YMin = PosY[i];

                float XMax =
                    XMin +
                    static_cast<float>(
                        AnimWidths[i]
                    );

                float YMax =
                    YMin +
                    static_cast<float>(
                        AnimHeights[i]
                    );


                #ifdef Blokk_CamEnabled

                XMin -= CameraX;
                XMax -= CameraX;

                YMin -= CameraY;
                YMax -= CameraY;

                #endif


                if (
                    XMax > 0 &&
                    YMax > 0 &&
                    XMin < ScreenWidth &&
                    YMin < ScreenHeight
                )
                {
                    ResultIdx.push_back(
                        StartIdx + i
                    );
                }
            }


            return ResultIdx;
        }


        // ============================================================
        // SSE2
        // ============================================================

        __attribute__((target("sse2")))
        std::vector<std::uint32_t> CheckVisible_SIMD_SSE2(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
        )
        {
            std::vector<std::uint32_t> ResultIdx;
            ResultIdx.reserve(Size);

            const __m128 Zero =
                _mm_setzero_ps();

            const __m128 ScrWidth =
                _mm_set1_ps(
                    static_cast<float>(ScreenWidth)
                );

            const __m128 ScrHeight =
                _mm_set1_ps(
                    static_cast<float>(ScreenHeight)
                );

            #ifdef Blokk_CamEnabled

            const __m128 CamX =
                _mm_set1_ps(CameraX);

            const __m128 CamY =
                _mm_set1_ps(CameraY);

            #endif


            std::uint32_t i = 0;


            // --------------------------------------------------------
            // Process 4 objects at a time
            // --------------------------------------------------------

            for (; i + 4 <= Size; i += 4)
            {
                // Positions
                __m128 MinXPos =
                    _mm_loadu_ps(
                        &PosX[i]
                    );

                __m128 MinYPos =
                    _mm_loadu_ps(
                        &PosY[i]
                    );


                // Load widths/heights
                __m128i IntWidths =
                    _mm_loadu_si128(
                        reinterpret_cast<const __m128i*>(
                            &AnimWidths[i]
                        )
                    );

                __m128i IntHeights =
                    _mm_loadu_si128(
                        reinterpret_cast<const __m128i*>(
                            &AnimHeights[i]
                        )
                    );


                // Convert int -> float
                __m128 Widths =
                    _mm_cvtepi32_ps(
                        IntWidths
                    );

                __m128 Heights =
                    _mm_cvtepi32_ps(
                        IntHeights
                    );


                // Maximum positions
                __m128 MaxXPos =
                    _mm_add_ps(
                        MinXPos,
                        Widths
                    );

                __m128 MaxYPos =
                    _mm_add_ps(
                        MinYPos,
                        Heights
                    );


                // ----------------------------------------------------
                // Camera transformation
                // ----------------------------------------------------

                #ifdef Blokk_CamEnabled

                MinXPos =
                    _mm_sub_ps(
                        MinXPos,
                        CamX
                    );

                MaxXPos =
                    _mm_sub_ps(
                        MaxXPos,
                        CamX
                    );

                MinYPos =
                    _mm_sub_ps(
                        MinYPos,
                        CamY
                    );

                MaxYPos =
                    _mm_sub_ps(
                        MaxYPos,
                        CamY
                    );

                #endif


                // ----------------------------------------------------
                // Visibility tests
                // ----------------------------------------------------

                // MinX < ScreenWidth
                __m128 XRes =
                    _mm_cmplt_ps(
                        MinXPos,
                        ScrWidth
                    );

                // MinY < ScreenHeight
                __m128 YRes =
                    _mm_cmplt_ps(
                        MinYPos,
                        ScrHeight
                    );

                // MaxX > 0
                __m128 XRes0 =
                    _mm_cmpgt_ps(
                        MaxXPos,
                        Zero
                    );

                // MaxY > 0
                __m128 YRes0 =
                    _mm_cmpgt_ps(
                        MaxYPos,
                        Zero
                    );


                // Combine
                __m128 XResComb =
                    _mm_and_ps(
                        XRes,
                        XRes0
                    );

                __m128 YResComb =
                    _mm_and_ps(
                        YRes,
                        YRes0
                    );

                __m128 FinalRes =
                    _mm_and_ps(
                        XResComb,
                        YResComb
                    );


                // Convert 4 lanes -> 4-bit mask
                int Mask =
                    _mm_movemask_ps(
                        FinalRes
                    );


                // Store visible indexes
                if (Mask != 0)
                {
                    for (int lane = 0; lane < 4; ++lane)
                    {
                        if (Mask & (1 << lane))
                        {
                            ResultIdx.push_back(
                                StartIdx +
                                i +
                                static_cast<std::uint32_t>(
                                    lane
                                )
                            );
                        }
                    }
                }
            }


            // --------------------------------------------------------
            // Remaining objects
            // --------------------------------------------------------

            for (; i < Size; ++i)
            {
                float XMin =
                    PosX[i];

                float YMin =
                    PosY[i];

                float XMax =
                    XMin +
                    static_cast<float>(
                        AnimWidths[i]
                    );

                float YMax =
                    YMin +
                    static_cast<float>(
                        AnimHeights[i]
                    );


                #ifdef Blokk_CamEnabled

                XMin -= CameraX;
                XMax -= CameraX;

                YMin -= CameraY;
                YMax -= CameraY;

                #endif


                if (
                    XMax > 0 &&
                    YMax > 0 &&
                    XMin < ScreenWidth &&
                    YMin < ScreenHeight
                )
                {
                    ResultIdx.push_back(
                        StartIdx +
                        static_cast<std::uint32_t>(i)
                    );
                }
            }


            return ResultIdx;
        }


        // ============================================================
        // AVX-512
        // ============================================================

        __attribute__((target("avx512f")))
        std::vector<std::uint32_t> CheckVisible_SIMD_AVX512(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
        )
        {
            std::vector<std::uint32_t> ResultIdx;
            ResultIdx.reserve(Size);

            const __m512 Zero =
                _mm512_setzero_ps();

            const __m512 ScrWidth =
                _mm512_set1_ps(
                    static_cast<float>(ScreenWidth)
                );

            const __m512 ScrHeight =
                _mm512_set1_ps(
                    static_cast<float>(ScreenHeight)
                );

            #ifdef Blokk_CamEnabled

            const __m512 CamX =
                _mm512_set1_ps(CameraX);

            const __m512 CamY =
                _mm512_set1_ps(CameraY);

            #endif


            std::uint32_t i = 0;


            // --------------------------------------------------------
            // Process 16 objects at a time
            // --------------------------------------------------------

            for (; i + 16 <= Size; i += 16)
            {
                // Positions
                __m512 MinXPos =
                    _mm512_loadu_ps(
                        &PosX[i]
                    );

                __m512 MinYPos =
                    _mm512_loadu_ps(
                        &PosY[i]
                    );


                // Load widths/heights
                __m512i IntWidths =
                    _mm512_loadu_si512(
                        reinterpret_cast<const void*>(
                            &AnimWidths[i]
                        )
                    );

                __m512i IntHeights =
                    _mm512_loadu_si512(
                        reinterpret_cast<const void*>(
                            &AnimHeights[i]
                        )
                    );


                // Convert int -> float
                __m512 Widths =
                    _mm512_cvtepi32_ps(
                        IntWidths
                    );

                __m512 Heights =
                    _mm512_cvtepi32_ps(
                        IntHeights
                    );


                // Maximum positions
                __m512 MaxXPos =
                    _mm512_add_ps(
                        MinXPos,
                        Widths
                    );

                __m512 MaxYPos =
                    _mm512_add_ps(
                        MinYPos,
                        Heights
                    );


                // ----------------------------------------------------
                // Camera transformation
                // ----------------------------------------------------

                #ifdef Blokk_CamEnabled

                MinXPos =
                    _mm512_sub_ps(
                        MinXPos,
                        CamX
                    );

                MaxXPos =
                    _mm512_sub_ps(
                        MaxXPos,
                        CamX
                    );

                MinYPos =
                    _mm512_sub_ps(
                        MinYPos,
                        CamY
                    );

                MaxYPos =
                    _mm512_sub_ps(
                        MaxYPos,
                        CamY
                    );

                #endif


                // ----------------------------------------------------
                // Visibility tests
                // ----------------------------------------------------

                // MinX < ScreenWidth
                __mmask16 XRes =
                    _mm512_cmp_ps_mask(
                        MinXPos,
                        ScrWidth,
                        _CMP_LT_OQ
                    );

                // MinY < ScreenHeight
                __mmask16 YRes =
                    _mm512_cmp_ps_mask(
                        MinYPos,
                        ScrHeight,
                        _CMP_LT_OQ
                    );

                // MaxX > 0
                __mmask16 XRes0 =
                    _mm512_cmp_ps_mask(
                        MaxXPos,
                        Zero,
                        _CMP_GT_OQ
                    );

                // MaxY > 0
                __mmask16 YRes0 =
                    _mm512_cmp_ps_mask(
                        MaxYPos,
                        Zero,
                        _CMP_GT_OQ
                    );


                // Combine masks
                __mmask16 FinalRes =
                    XRes &
                    YRes &
                    XRes0 &
                    YRes0;


                // Store visible indexes
                if (FinalRes != 0)
                {
                    for (int lane = 0; lane < 16; ++lane)
                    {
                        if (FinalRes & (1u << lane))
                        {
                            ResultIdx.push_back(
                                StartIdx +
                                i +
                                static_cast<std::uint32_t>(
                                    lane
                                )
                            );
                        }
                    }
                }
            }


            // --------------------------------------------------------
            // Remaining objects
            // --------------------------------------------------------

            for (; i < Size; ++i)
            {
                float XMin =
                    PosX[i];

                float YMin =
                    PosY[i];

                float XMax =
                    XMin +
                    static_cast<float>(
                        AnimWidths[i]
                    );

                float YMax =
                    YMin +
                    static_cast<float>(
                        AnimHeights[i]
                    );


                #ifdef Blokk_CamEnabled

                XMin -= CameraX;
                XMax -= CameraX;

                YMin -= CameraY;
                YMax -= CameraY;

                #endif


                if (
                    XMax > 0 &&
                    YMax > 0 &&
                    XMin < ScreenWidth &&
                    YMin < ScreenHeight
                )
                {
                    ResultIdx.push_back(
                        StartIdx +
                        static_cast<std::uint32_t>(i)
                    );
                }
            }


            return ResultIdx;
        }

    }

}

#endif