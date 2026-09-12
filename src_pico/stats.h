#pragma once

#include <stdbool.h>

typedef struct {
    float cpu_temp;
    float cpu_load;
    float cpu_clk;

    float gpu_temp;
    float gpu_load;
    float gpu_vram;

    float ram_used;

    bool inactive;
} PcStats;
