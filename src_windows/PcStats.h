// Internal representation of the PC's Stats:
#pragma once

struct CPUStats {
    float temp = 0.0f;
    float clk = 0.0f;
    float load = 0.0f;
};

struct GPUStats {
    float temp = 0.0f;
    float load = 0.0f;
    float vram = 0.0f;
};

struct RAMStats {
    float usage = 0.0f;
};

struct PcStats {
    CPUStats cpu;
    GPUStats gpu;
    RAMStats ram;
};