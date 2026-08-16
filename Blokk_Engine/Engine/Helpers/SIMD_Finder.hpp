#pragma once
#include <intrin.h>

enum class SIMDLevel
{
    SSE2,
    AVX,
    AVX2,
    AVX512
};

inline SIMDLevel DetectSIMD()
{
    int cpuInfo[4];

    // Find the highest CPUID leaf supported by the CPU.
    __cpuid(cpuInfo, 0);
    int highestLeaf = cpuInfo[0];

    // SSE2
    // On x86-64, SSE2 is guaranteed, but check it anyway.
    bool hasSSE2 = false;

    if (highestLeaf >= 1)
    {
        __cpuid(cpuInfo, 1);

        // EDX bit 26 = SSE2
        hasSSE2 = (cpuInfo[3] & (1 << 26)) != 0;
    }

    if (!hasSSE2)
        return SIMDLevel::SSE2; // Your fallback would be scalar if needed.

    if (highestLeaf < 1)
        return SIMDLevel::SSE2;

    // CPUID leaf 1
    __cpuid(cpuInfo, 1);

    // ECX bit 28 = AVX
    bool hasAVX = (cpuInfo[2] & (1 << 28)) != 0;

    // ECX bit 27 = OSXSAVE
    bool hasOSXSAVE = (cpuInfo[2] & (1 << 27)) != 0;

    if (!hasAVX || !hasOSXSAVE)
        return SIMDLevel::SSE2;

    // Check which extended registers the OS actually saves/restores.
    unsigned __int64 xcr0 = _xgetbv(0);

    // XMM state (bit 1) + YMM state (bit 2)
    bool osSupportsAVX = (xcr0 & 0x6) == 0x6;

    if (!osSupportsAVX)
        return SIMDLevel::SSE2;

    // At this point AVX is usable.
    if (highestLeaf < 7)
        return SIMDLevel::AVX;

    // CPUID leaf 7, subleaf 0
    __cpuidex(cpuInfo, 7, 0);

    // EBX bit 5 = AVX2
    bool hasAVX2 = (cpuInfo[1] & (1 << 5)) != 0;

    if (!hasAVX2)
        return SIMDLevel::AVX;

    // AVX2 is usable.
    // Now check AVX-512.
    //
    // EBX bit 16 = AVX-512 Foundation
    bool hasAVX512F = (cpuInfo[1] & (1 << 16)) != 0;

    if (!hasAVX512F)
        return SIMDLevel::AVX2;

    // AVX-512 requires the OS to enable:
    //
    // bit 1 = XMM
    // bit 2 = YMM
    // bit 5 = opmask
    // bit 6 = ZMM_Hi256
    // bit 7 = Hi16_ZMM
    //
    // 0xE6 = 1110 0110
    bool osSupportsAVX512 = (xcr0 & 0xE6) == 0xE6;

    if (!osSupportsAVX512)
        return SIMDLevel::AVX2;

    return SIMDLevel::AVX512;
}