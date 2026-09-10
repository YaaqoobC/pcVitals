#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"

typedef struct {
    float cpuTemp;
    float cpuLoad;
    float cpuClk;

    float gpuTemp;
    float gpuLoad;
    float gpuVram;

    float ramUsage;
} Stats;


int main() {
    stdio_init_all();
    lcd_init();
    
    while (true) {
                
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("PC Vitals - Temp");
        lcd_set_cursor(1, 0);
        lcd_print("CPU: 45C GPU:50C");

        sleep_ms(2000);
        
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("PC Vitals - Load");
        lcd_set_cursor(1, 0);
        lcd_print("CPU: 85% GPU:27%");
        
        sleep_ms(2000);
        
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("PC Vitals - MEM");
        lcd_set_cursor(1, 0);
        lcd_print("GPU-VRAM: 1.29GB");
        
        sleep_ms(2000);

        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("PC Vitals - MEM");
        lcd_set_cursor(1, 0);
        lcd_print("RAM: 16.24GB");
        
        sleep_ms(2000);
        
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("PC Vitals - CLK");
        lcd_set_cursor(1, 0);
        lcd_print("CPU: 3.8 GHz");
        
        sleep_ms(2000);
    }
}
