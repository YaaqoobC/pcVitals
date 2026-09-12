#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"
#include "stats.h"
#include "display.h"

#define DISPLAY_REFRESH_MS 250

int main() {
    stdio_init_all();
    display_init();

    const PcStats stats = {
        .cpu_temp = 43.0f,
        .cpu_load = 13.0f,
        .cpu_clk = 4200.0f,

        .gpu_temp = 51.0f,
        .gpu_load = 35.0f,
        .gpu_vram = 2057.0f,

        .ram_used = 15.0f
    };

    while (true) {
        display_show_stats(&stats);
        sleep_ms(DISPLAY_REFRESH_MS);
    }
}
