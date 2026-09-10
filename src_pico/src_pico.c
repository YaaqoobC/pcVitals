#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"
#include "stats.h"
#include "display.h"
typedef struct {
    float cpuTemp;
    float cpuLoad;
    float cpuClk;

    float gpuTemp;
    float gpuLoad;
    float gpuVram;

    float ramUsage;
} Stats;

void printScreen(char *header, char *line2) {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print(header);
    lcd_set_cursor(1, 0);
    lcd_print(line2);

    sleep_ms(2000);
}


int main() {
    stdio_init_all();
    display_init();

    const PcStats stats = {
        .cpu_temp = 45.0f,
        .cpu_load = 85.0f,
        .cpu_clk = 3.8f,

        .gpu_temp = 50.0f,
        .gpu_load = 27.0f,
        .gpu_vram = 1.29f,

        .ram_used = 16.24f
    };

    while (true) {
        display_show_stats(&stats);
        sleep_ms(2000);
    }
}
