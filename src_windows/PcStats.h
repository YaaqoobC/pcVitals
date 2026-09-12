// Internal representation of the PC's Stats:
#pragma once

#include <iostream>

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

inline void PrintStats(const PcStats& stats) {
    std::cout << "\n===== PC Stats =====\n";

    std::cout << "CPU Temperature: " << stats.cpu.temp << " C\n";
    std::cout << "CPU Usage: " << stats.cpu.load << " %\n";
    std::cout << "CPU Clock: " << stats.cpu.clk << " MHz\n";

    std::cout << "GPU Temperature: " << stats.gpu.temp << " C\n";
    std::cout << "GPU Usage: " << stats.gpu.load << " %\n";
    std::cout << "GPU VRAM: " << stats.gpu.vram << " MB\n";

    std::cout << "RAM Usage: " << stats.ram.usage << " MB\n";

    std::cout << "====================\n";
}
