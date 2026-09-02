#pragma once

#include <cstdint>

#if defined(__GNUC__) || defined(__clang__)
    #include <cpuid.h>
#endif

enum class SIMDLevel
{
    SSE2,

    AVX2,
    AVX512,

    Unsupported
};

#if defined(_MSC_VER)

    #include <intrin.h>

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
        #if defined(__x86_64__) || defined(__i386__)

            // x86/x86-64.
            // AVX is intentionally disabled for now.

            if (__builtin_cpu_supports("sse2"))
                return SIMDLevel::SSE2;

            return SIMDLevel::Unsupported;

        #elif defined(__aarch64__) || defined(_M_ARM64)

            // ARM64 is not supported yet.
            // NEON support can be added later.
            return SIMDLevel::Unsupported;

        #else

            // Unknown architecture.
            return SIMDLevel::Unsupported;

        #endif
    }

#else

    // Unknown compiler/platform.
    inline SIMDLevel DetectSIMD()
    {
        return SIMDLevel::Unsupported;
    }

#endif