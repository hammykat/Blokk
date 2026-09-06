#pragma once

#include <cstdint>

#if defined(__aarch64__) || defined(_M_ARM64)

    #include <arm_neon.h>

#elif defined(__x86_64__) || defined(__i386__) || \
      defined(_M_X64) || defined(_M_IX86)

    #if defined(__GNUC__) || defined(__clang__)
        #include <cpuid.h>
    #endif

    #if defined(_MSC_VER)
        #include <intrin.h>
    #endif

#endif

enum class SIMDLevel
{
    SSE2,

    AVX2,
    AVX512,

    NEON,

    Unsupported
};


// ARM64
#if defined(__aarch64__) || defined(_M_ARM64)

inline SIMDLevel DetectSIMD()
{
    // NEON is mandatory on AArch64.
    return SIMDLevel::NEON;
}


// x86 / x86-64
#elif defined(__x86_64__) || defined(__i386__) || \
      defined(_M_X64) || defined(_M_IX86)

    #if defined(_MSC_VER)

    inline SIMDLevel DetectSIMD()
    {
        int cpuInfo[4];

        // Get highest supported CPUID leaf.
        __cpuid(cpuInfo, 0);
        int highestLeaf = cpuInfo[0];

        // We need CPUID leaf 1 to check SSE2.
        if (highestLeaf < 1)
            return SIMDLevel::Unsupported;

        __cpuid(cpuInfo, 1);

        // EDX bit 26 = SSE2
        bool hasSSE2 =
            (cpuInfo[3] & (1 << 26)) != 0;

        if (!hasSSE2)
            return SIMDLevel::Unsupported;

        // AVX is intentionally disabled for now.
        return SIMDLevel::SSE2;
    }

    #elif defined(__GNUC__) || defined(__clang__)

    inline SIMDLevel DetectSIMD()
    {
        // x86/x86-64.
        // AVX is intentionally disabled for now.

        if (__builtin_cpu_supports("sse2"))
            return SIMDLevel::SSE2;

        return SIMDLevel::Unsupported;
    }

    #else

    inline SIMDLevel DetectSIMD()
    {
        return SIMDLevel::Unsupported;
    }

    #endif


// Unknown architecture
#else

inline SIMDLevel DetectSIMD()
{
    return SIMDLevel::Unsupported;
}

#endif