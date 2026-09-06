#include <cstdint>
#include <vector>

#if defined(__x86_64__) || defined(__i386__) || \
    defined(_M_X64) || defined(_M_IX86)

    #include <emmintrin.h>
    #include <immintrin.h>

#elif defined(__aarch64__) || defined(_M_ARM64)

    #include <arm_neon.h>

#endif

#include "EngineClassData.hpp"

#if (Blokk_Visibility_CullType == 1)

namespace Blokk {

    // Forward declarations
    namespace InternalHelpers
    {

        #if defined(__x86_64__) || defined(__i386__) || \
            defined(_M_X64) || defined(_M_IX86)

        std::vector<std::uint32_t> CheckVisible_Axis_SIMD_AVX2(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraX,
            float CameraY
            #endif
        );

        std::vector<std::uint32_t> CheckVisible_Axis_SIMD_AVX512(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraX,
            float CameraY
            #endif
        );

        std::vector<std::uint32_t> CheckVisible_Axis_SIMD_SSE2(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraX,
            float CameraY
            #endif
        );

        std::vector<std::uint32_t> CheckVisible_AxisY_SIMD_AVX2(
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimHeights,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraY
            #endif
        );

        std::vector<std::uint32_t> CheckVisible_AxisY_SIMD_AVX512(
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimHeights,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraY
            #endif
        );

        std::vector<std::uint32_t> CheckVisible_AxisY_SIMD_SSE2(
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimHeights,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraY
            #endif
        );

        #endif


        #if defined(__aarch64__) || defined(_M_ARM64)

        std::vector<std::uint32_t> CheckVisible_Axis_SIMD_NEON(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraX,
            float CameraY
            #endif
        );

        std::vector<std::uint32_t> CheckVisible_AxisY_SIMD_NEON(
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimHeights,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraY
            #endif
        );

        #endif
    }


    // ObjectManager Visibility Function

    template <SIMDLevel Level>
    void ObjectManager::CheckVisibilityFn_Axis(
        IndexRange Range,
        Worker* Thread
    )
    {
        #ifdef Blokk_CamEnabled

        const float CameraX = static_cast<float>(Camera.GetXPosition());
        const float CameraY = static_cast<float>(Camera.GetYPosition());

        #endif

        #if defined(__x86_64__) || defined(__i386__) || \
            defined(_M_X64) || defined(_M_IX86)

        // AVX2
        // 256-bit = 8 floats

        if constexpr (Level == SIMDLevel::AVX2)
        {
            Thread->IdxResult =
                InternalHelpers::CheckVisible_Axis_SIMD_AVX2(
                    ScreenWidth,
                    ScreenHeight,
                    &AnimWidths[Range.Start],
                    &AnimHeights[Range.Start],
                    &XPositions[Range.Start],
                    &YPositions[Range.Start],
                    Range.GetSize(),
                    Range.Start
                    #ifdef Blokk_CamEnabled
                    , CameraX,
                    CameraY
                    #endif
                );
        }

        // AVX-512
        // 512-bit = 16 floats

        else if constexpr (Level == SIMDLevel::AVX512)
        {
            Thread->IdxResult =
                InternalHelpers::CheckVisible_Axis_SIMD_AVX512(
                    ScreenWidth,
                    ScreenHeight,
                    &AnimWidths[Range.Start],
                    &AnimHeights[Range.Start],
                    &XPositions[Range.Start],
                    &YPositions[Range.Start],
                    Range.GetSize(),
                    Range.Start
                    #ifdef Blokk_CamEnabled
                    , CameraX,
                    CameraY
                    #endif
                );
        }

        
        // SSE2
        // 128-bit = 4 floats
        
        else if constexpr (Level == SIMDLevel::SSE2)
        {
            Thread->IdxResult =
            InternalHelpers::CheckVisible_Axis_SIMD_SSE2(
                ScreenWidth,
                ScreenHeight,
                &AnimWidths[Range.Start],
                &AnimHeights[Range.Start],
                &XPositions[Range.Start],
                &YPositions[Range.Start],
                Range.GetSize(),
                Range.Start
                #ifdef Blokk_CamEnabled
                , CameraX,
                CameraY
                #endif
            );
        }

        #elif defined(__aarch64__) || defined(_M_ARM64)
        
        // NEON
        // 128-bit = 4 floats

        if constexpr (Level == SIMDLevel::NEON)
        {
            Thread->IdxResult =
                InternalHelpers::CheckVisible_Axis_SIMD_NEON(
                    ScreenWidth,
                    ScreenHeight,
                    &AnimWidths[Range.Start],
                    &AnimHeights[Range.Start],
                    &XPositions[Range.Start],
                    &YPositions[Range.Start],
                    Range.GetSize(),
                    Range.Start
                    #ifdef Blokk_CamEnabled
                    , CameraX,
                    CameraY
                    #endif
                );
        }

        #endif

        // Scalar
        else
        {
            Thread->IdxResult =
                InternalHelpers::CheckVisible_Axis_Scalar(
                    ScreenWidth,
                    ScreenHeight,
                    &AnimWidths[Range.Start],
                    &AnimHeights[Range.Start],
                    &XPositions[Range.Start],
                    &YPositions[Range.Start],
                    Range.GetSize(),
                    Range.Start
                    #ifdef Blokk_CamEnabled
                    , CameraX,
                    CameraY
                    #endif
                );
        }
    }


    namespace InternalHelpers {

        #if defined(__x86_64__) || defined(__i386__) || \
            defined(_M_X64) || defined(_M_IX86)


        // AVX2 - X + Y Visibility

        __attribute__((target("avx2")))
        std::vector<std::uint32_t> CheckVisible_Axis_SIMD_AVX2(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraX,
            float CameraY
            #endif
        )
        {
            std::vector<std::uint32_t> Result;

            // First cull against Y axis
            std::vector<std::uint32_t> YAxisCulled =
                CheckVisible_AxisY_SIMD_AVX2(
                    ScreenHeight,
                    AnimHeights,
                    PosY,
                    Size,
                    StartIdx
                    #ifdef Blokk_CamEnabled
                    , CameraY
                    #endif
                );

            // Then check X axis
            for (std::uint32_t Idx : YAxisCulled)
            {
                float MinX = PosX[Idx];
                float MaxX = MinX + AnimWidths[Idx];

                // Camera offsets
                #ifdef Blokk_CamEnabled
                MinX -= CameraX;
                MaxX -= CameraX;
                #endif

                if (MaxX > 0 && MinX < ScreenWidth)
                {
                    Result.push_back(Idx);
                }
            }

            return Result;
        }


        // AVX2 - Y Visibility

        __attribute__((target("avx2")))
        std::vector<std::uint32_t> CheckVisible_AxisY_SIMD_AVX2(
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimHeights,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraY
            #endif
        )
        {
            __m256 Zero = _mm256_set1_ps(0.0f);
            __m256 ScrHeight = _mm256_set1_ps(
                static_cast<float>(ScreenHeight)
            );

            #ifdef Blokk_CamEnabled
            __m256 CamY = _mm256_set1_ps(CameraY);
            #endif

            std::vector<std::uint32_t> ResultIdx;
            std::uint32_t i = 0;

            // Process 8 objects at a time

            for (; i + 8 <= Size; i += 8)
            {
                __m256 MinYPos = _mm256_loadu_ps(&PosY[i]);

                __m256i IntHeights = _mm256_loadu_si256(
                    reinterpret_cast<const __m256i*>(&AnimHeights[i])
                );

                __m256 Heights = _mm256_cvtepi32_ps(IntHeights);

                __m256 MaxYPos = _mm256_add_ps(
                    MinYPos,
                    Heights
                );

                #ifdef Blokk_CamEnabled
                MinYPos = _mm256_sub_ps(MinYPos, CamY);
                MaxYPos = _mm256_sub_ps(MaxYPos, CamY);
                #endif

                __m256 YRes = _mm256_cmp_ps(
                    MinYPos,
                    ScrHeight,
                    _CMP_LT_OQ
                );

                __m256 YRes0 = _mm256_cmp_ps(
                    MaxYPos,
                    Zero,
                    _CMP_GT_OQ
                );

                __m256 YResult = _mm256_and_ps(
                    YRes,
                    YRes0
                );

                int Result = _mm256_movemask_ps(YResult);

                if (Result != 0)
                {
                    for (int l = 0; l < 8; ++l)
                    {
                        if (Result & (1 << l))
                        {
                            ResultIdx.push_back(
                                StartIdx + i + l
                            );
                        }
                    }
                }
            }

            // Remaining objects

            for (; i < Size; ++i)
            {
                float YMin = PosY[i];
                float YMax = PosY[i] + AnimHeights[i];

                #ifdef Blokk_CamEnabled
                YMin -= CameraY;
                YMax -= CameraY;
                #endif

                if (YMax > 0 && YMin < ScreenHeight)
                {
                    ResultIdx.push_back(StartIdx + i);
                }
            }

            return ResultIdx;
        }


        // SSE2 - X + Y Visibility

        __attribute__((target("sse2")))
        std::vector<std::uint32_t> CheckVisible_Axis_SIMD_SSE2(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraX,
            float CameraY
            #endif
        )
        {
            std::vector<std::uint32_t> Result;

            // First cull against Y axis
            std::vector<std::uint32_t> YAxisCulled =
                CheckVisible_AxisY_SIMD_SSE2(
                    ScreenHeight,
                    AnimHeights,
                    PosY,
                    Size,
                    StartIdx
                    #ifdef Blokk_CamEnabled
                    , CameraY
                    #endif
                );

            // Then check X axis
            for (std::uint32_t Idx : YAxisCulled)
            {
                float MinX = PosX[Idx];
                float MaxX = MinX + AnimWidths[Idx];

                // Camera offsets
                #ifdef Blokk_CamEnabled
                MinX -= CameraX;
                MaxX -= CameraX;
                #endif

                if (MaxX > 0 && MinX < ScreenWidth)
                {
                    Result.push_back(Idx);
                }
            }

            return Result;
        }


        // SSE2 - Y Visibility

        __attribute__((target("sse2")))
        std::vector<std::uint32_t> CheckVisible_AxisY_SIMD_SSE2(
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimHeights,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraY
            #endif
        )
        {
            __m128 Zero = _mm_set1_ps(0.0f);
            __m128 ScrHeight = _mm_set1_ps(
                static_cast<float>(ScreenHeight)
            );

            #ifdef Blokk_CamEnabled
            __m128 CamY = _mm_set1_ps(CameraY);
            #endif

            std::vector<std::uint32_t> ResultIdx;
            std::uint32_t i = 0;

            // Process 4 objects at a time

            for (; i + 4 <= Size; i += 4)
            {
                __m128 MinYPos = _mm_loadu_ps(&PosY[i]);

                __m128i IntHeights = _mm_loadu_si128(
                    reinterpret_cast<const __m128i*>(&AnimHeights[i])
                );

                __m128 Heights = _mm_cvtepi32_ps(IntHeights);

                __m128 MaxYPos = _mm_add_ps(
                    MinYPos,
                    Heights
                );

                #ifdef Blokk_CamEnabled
                MinYPos = _mm_sub_ps(MinYPos, CamY);
                MaxYPos = _mm_sub_ps(MaxYPos, CamY);
                #endif

                __m128 YRes = _mm_cmplt_ps(
                    MinYPos,
                    ScrHeight
                );

                __m128 YRes0 = _mm_cmpgt_ps(
                    MaxYPos,
                    Zero
                );

                __m128 YResult = _mm_and_ps(
                    YRes,
                    YRes0
                );

                int Result = _mm_movemask_ps(YResult);

                if (Result != 0)
                {
                    for (int l = 0; l < 4; ++l)
                    {
                        if (Result & (1 << l))
                        {
                            ResultIdx.push_back(
                                StartIdx + i + l
                            );
                        }
                    }
                }
            }

            // Remaining objects

            for (; i < Size; ++i)
            {
                float YMin = PosY[i];
                float YMax = PosY[i] + AnimHeights[i];

                #ifdef Blokk_CamEnabled
                YMin -= CameraY;
                YMax -= CameraY;
                #endif

                if (YMax > 0 && YMin < ScreenHeight)
                {
                    ResultIdx.push_back(StartIdx + i);
                }
            }

            return ResultIdx;
        }


        // AVX-512 - X + Y Visibility

        __attribute__((target("avx512f")))
        std::vector<std::uint32_t> CheckVisible_Axis_SIMD_AVX512(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraX,
            float CameraY
            #endif
        )
        {
            std::vector<std::uint32_t> Result;

            // First cull against Y axis
            std::vector<std::uint32_t> YAxisCulled =
                CheckVisible_AxisY_SIMD_AVX512(
                    ScreenHeight,
                    AnimHeights,
                    PosY,
                    Size,
                    StartIdx
                    #ifdef Blokk_CamEnabled
                    , CameraY
                    #endif
                );

            // Then check X axis
            for (std::uint32_t Idx : YAxisCulled)
            {
                float MinX = PosX[Idx];
                float MaxX = MinX + AnimWidths[Idx];

                // Camera offsets
                #ifdef Blokk_CamEnabled
                MinX -= CameraX;
                MaxX -= CameraX;
                #endif

                if (MaxX > 0 && MinX < ScreenWidth)
                {
                    Result.push_back(Idx);
                }
            }

            return Result;
        }


        // AVX-512 - Y Visibility

        __attribute__((target("avx512f")))
        std::vector<std::uint32_t> CheckVisible_AxisY_SIMD_AVX512(
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimHeights,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraY
            #endif
        )
        {
            __m512 Zero = _mm512_set1_ps(0.0f);
            __m512 ScrHeight = _mm512_set1_ps(
                static_cast<float>(ScreenHeight)
            );

            #ifdef Blokk_CamEnabled
            __m512 CamY = _mm512_set1_ps(CameraY);
            #endif

            std::vector<std::uint32_t> ResultIdx;
            std::uint32_t i = 0;

            // Process 16 objects at a time

            for (; i + 16 <= Size; i += 16)
            {
                __m512 MinYPos = _mm512_loadu_ps(&PosY[i]);

                __m512i IntHeights = _mm512_loadu_si512(
                    reinterpret_cast<const void*>(&AnimHeights[i])
                );

                __m512 Heights = _mm512_cvtepi32_ps(IntHeights);

                __m512 MaxYPos = _mm512_add_ps(
                    MinYPos,
                    Heights
                );

                #ifdef Blokk_CamEnabled
                MinYPos = _mm512_sub_ps(MinYPos, CamY);
                MaxYPos = _mm512_sub_ps(MaxYPos, CamY);
                #endif

                __mmask16 YRes = _mm512_cmp_ps_mask(
                    MinYPos,
                    ScrHeight,
                    _CMP_LT_OQ
                );

                __mmask16 YRes0 = _mm512_cmp_ps_mask(
                    MaxYPos,
                    Zero,
                    _CMP_GT_OQ
                );

                __mmask16 YResult = YRes & YRes0;

                if (YResult != 0)
                {
                    for (int l = 0; l < 16; ++l)
                    {
                        if (YResult & (1u << l))
                        {
                            ResultIdx.push_back(
                                StartIdx + i + l
                            );
                        }
                    }
                }
            }

            // Remaining objects

            for (; i < Size; ++i)
            {
                float YMin = PosY[i];
                float YMax = PosY[i] + AnimHeights[i];

                #ifdef Blokk_CamEnabled
                YMin -= CameraY;
                YMax -= CameraY;
                #endif

                if (YMax > 0 && YMin < ScreenHeight)
                {
                    ResultIdx.push_back(StartIdx + i);
                }
            }

            return ResultIdx;
        }

        #endif


        // NEON - X + Y Visibility

        #if defined(__aarch64__) || defined(_M_ARM64)

        std::vector<std::uint32_t> CheckVisible_Axis_SIMD_NEON(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraX,
            float CameraY
            #endif
        )
        {
            std::vector<std::uint32_t> Result;

            // First cull against Y axis
            std::vector<std::uint32_t> YAxisCulled =
                CheckVisible_AxisY_SIMD_NEON(
                    ScreenHeight,
                    AnimHeights,
                    PosY,
                    Size,
                    StartIdx
                    #ifdef Blokk_CamEnabled
                    , CameraY
                    #endif
                );

            // Then check X axis
            for (std::uint32_t Idx : YAxisCulled)
            {
                float MinX = PosX[Idx];
                float MaxX = MinX + AnimWidths[Idx];

                // Camera offsets
                #ifdef Blokk_CamEnabled
                MinX -= CameraX;
                MaxX -= CameraX;
                #endif

                if (MaxX > 0 && MinX < ScreenWidth)
                {
                    Result.push_back(Idx);
                }
            }

            return Result;
        }


        // NEON - Y Visibility

        std::vector<std::uint32_t> CheckVisible_AxisY_SIMD_NEON(
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimHeights,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraY
            #endif
        )
        {
            float32x4_t Zero = vdupq_n_f32(0.0f);
            float32x4_t ScrHeight = vdupq_n_f32(
                static_cast<float>(ScreenHeight)
            );

            #ifdef Blokk_CamEnabled
            float32x4_t CamY = vdupq_n_f32(CameraY);
            #endif

            std::vector<std::uint32_t> ResultIdx;
            std::uint32_t i = 0;

            // Process 4 objects at a time

            for (; i + 4 <= Size; i += 4)
            {
                float32x4_t MinYPos = vld1q_f32(&PosY[i]);

                // Animation heights
                uint32x4_t IntHeights =
                    vld1q_u32(&AnimHeights[i]);

                // uint32 -> float
                float32x4_t Heights =
                    vcvtq_f32_u32(IntHeights);

                float32x4_t MaxYPos = vaddq_f32(
                    MinYPos,
                    Heights
                );

                #ifdef Blokk_CamEnabled
                MinYPos = vsubq_f32(MinYPos, CamY);
                MaxYPos = vsubq_f32(MaxYPos, CamY);
                #endif

                uint32x4_t YRes = vcltq_f32(
                    MinYPos,
                    ScrHeight
                );

                uint32x4_t YRes0 = vcgtq_f32(
                    MaxYPos,
                    Zero
                );

                uint32x4_t YResult = vandq_u32(
                    YRes,
                    YRes0
                );

                uint32_t Masks[4];
                vst1q_u32(Masks, YResult);

                for (int l = 0; l < 4; ++l)
                {
                    if (Masks[l] != 0)
                    {
                        ResultIdx.push_back(
                            StartIdx + i + l
                        );
                    }
                }
            }

            // Remaining objects

            for (; i < Size; ++i)
            {
                float YMin = PosY[i];
                float YMax = PosY[i] + AnimHeights[i];

                #ifdef Blokk_CamEnabled
                YMin -= CameraY;
                YMax -= CameraY;
                #endif

                if (YMax > 0 && YMin < ScreenHeight)
                {
                    ResultIdx.push_back(StartIdx + i);
                }
            }

            return ResultIdx;
        }

        #endif

        // Scalar - Y Axis
        std::vector<std::uint32_t> CheckVisible_AxisY_Scalar(
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimHeights,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraY
            #endif
        )
        {
            std::vector<uint32_t> ResultIdx;


            for (uint32_t i = 0; i < Size; i++)
            {
                float YMin = PosY[i];
                float YMax = PosY[i] + AnimHeights[i];

                #ifdef Blokk_CamEnabled
                YMin -= CameraY;
                YMax -= CameraY;
                #endif

                if (YMax > 0 && YMin < ScreenHeight)
                {
                    ResultIdx.push_back(StartIdx + i);
                }
            }

            return ResultIdx;
        }

        // Scalar - Both Axis
        std::vector<std::uint32_t> CheckVisible_Axis_SIMD_NEON(
            std::uint32_t ScreenWidth,
            std::uint32_t ScreenHeight,
            std::uint32_t* AnimWidths,
            std::uint32_t* AnimHeights,
            float* PosX,
            float* PosY,
            std::uint32_t Size,
            std::uint32_t StartIdx
            #ifdef Blokk_CamEnabled
            , float CameraX,
            float CameraY
            #endif
        )
        {
            std::vector<std::uint32_t> Result;

            // First cull against Y axis
            std::vector<std::uint32_t> YAxisCulled =
                CheckVisible_AxisY_Scalar(
                    ScreenHeight,
                    AnimHeights,
                    PosY,
                    Size,
                    StartIdx
                    #ifdef Blokk_CamEnabled
                    , CameraY
                    #endif
                );

            // Then check X axis
            for (uint32_t Idx : YAxisCulled)
            {
                float MinX = PosX[Idx];
                float MaxX = MinX + AnimWidths[Idx];

                // Camera offsets
                #ifdef Blokk_CamEnabled
                MinX -= CameraX;
                MaxX -= CameraX;
                #endif

                if (MaxX > 0 && MinX < ScreenWidth)
                {
                    Result.push_back(Idx);
                }
            }

            return Result;
        }

    }

}

#endif