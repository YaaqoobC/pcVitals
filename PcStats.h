// Internal representation of the PC's Stats:
#pragma once

struct PcStats
{
    // CPU
    float cpuTemperature = 0.0f;
    float cpuUsage = 0.0f;
    float cpuClockMHz = 0.0f;

    // GPU
    float gpuTemperature = 0.0f;
    float gpuUsage = 0.0f;
    float gpuClockMHz = 0.0f;
    float gpuMemoryUsedMB = 0.0f;

    // RAM
    float ramUsage = 0.0f;
    float ramUsedGB = 0.0f;
    float ramAvailableGB = 0.0f;
};