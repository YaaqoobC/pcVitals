// Internal representation of the PC's Stats:
#pragma once

struct PcStats
{
    // CPU
    float cpuTemp = 0.0f;
    float cpuClk = 0.0f;
    float cpuLoad = 0.0f;

    // GPU
    float gpuTemp = 0.0f;
    float gpuVRAM = 0.0f;
    float gpuLoad = 0.0f;

    // RAM
    float ramUsage = 0.0f;
};