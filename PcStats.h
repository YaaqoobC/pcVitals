// Internal representation of the PC's Stats:
#pragma once

struct PcStats
{
    float cpuTemperature = 0.0f;
    float gpuTemperature = 0.0f;

    float cpuUsage = 0.0f;
    float gpuUsage = 0.0f;

    float ramUsage = 0.0f;
};