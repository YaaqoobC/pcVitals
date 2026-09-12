#include "display.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "pico/stdlib.h"
#include "lcd.h"

#define LCD_COLS 16

#define CPU_PREFIX "CPU ->"
#define GPU_PREFIX "GPU ->"
#define RAM_PREFIX "RAM ->"

// A page will only change if its been at least this many msS
#define CAROUSEL_INTERVAL_MS 4000

typedef enum {
    STAT_NONE,

    STAT_CPU_TEMP,
    STAT_CPU_LOAD,
    STAT_CPU_CLK,

    STAT_GPU_TEMP,
    STAT_GPU_LOAD,
    STAT_GPU_VRAM,

    STAT_RAM_USED
} StatField;

typedef enum {
    LINE_EMPTY,
    LINE_TEXT,
    LINE_STATS
} LineType;

typedef struct {
    StatField field;
    const char *unit;
    uint8_t width;
    uint8_t precision;
} LineStatBlock;

typedef struct {
    LineType type;

    const char *text;
    const char *prefix;

    LineStatBlock first_stat_block;
    LineStatBlock second_stat_block;
} LineSpec;


typedef struct {
    LineSpec line0;
    LineSpec line1;
} PageSpec;

static uint8_t current_page = 0;
static uint32_t last_page_change_ms = 0;

static float get_stat_value(const PcStats *stats, StatField field);
static void display_print_line(uint8_t row, const char *text);
static void format_line(char *out, size_t out_size, const PcStats *stats, const LineSpec *spec);

// The different pages. To display something else just add a page.
static const PageSpec pages[] = {
    {
        // Page 0: CPU/GPU temperature + load
        .line0 = {
            .type = LINE_STATS,
            .prefix = CPU_PREFIX,
            .first_stat_block = {
                .field = STAT_CPU_TEMP,
                .unit = "C",
                .width = 2,
                .precision = 0,
            },
            .second_stat_block = {
                .field = STAT_CPU_LOAD,
                .unit = "%",
                .width = 3,
                .precision = 0
            }
            
        },
        .line1 = {
            .type = LINE_STATS,
            .prefix = GPU_PREFIX,
            .first_stat_block = {
                .field = STAT_GPU_TEMP,
                .unit = "C",
                .width = 2,
                .precision = 0,
            },
            .second_stat_block = {
                .field = STAT_GPU_LOAD,
                .unit = "%",
                .width = 3,
                .precision = 0
            }
        }
    },

    {
        // Page 1: CPU clock + GPU VRAM
        .line0 = {
            .type = LINE_STATS,
            .prefix = CPU_PREFIX,
            .first_stat_block = {
                .field = STAT_CPU_CLK,
                .unit = "MHz",
                .width = 4,
                .precision = 0,
            },
            .second_stat_block = {
                .field = STAT_NONE
            }
        },
        .line1 = {
            .type = LINE_STATS,
            .prefix = GPU_PREFIX,
            .first_stat_block = {
                .field = STAT_GPU_VRAM,
                .unit = "MB",
                .width = 4,
                .precision = 0,
            },
            .second_stat_block = {
                .field = STAT_NONE
            }
        }
    },

    {
        // Page 2: RAM page
        .line0 = {
            .type = LINE_STATS,
            .prefix = RAM_PREFIX,
            .first_stat_block = {
                .field = STAT_RAM_USED,
                .unit = "GB",
                .width = 5,
                .precision = 2,
            },
            .second_stat_block = {
                .field = STAT_NONE
            }
        },
        .line1 = {
            .type = LINE_TEXT,
            .text = "PC Vitals"
        }
    }
};

#define PAGE_COUNT ((uint8_t)(sizeof(pages) / sizeof(pages[0])))

static float get_stat_value(const PcStats *stats, StatField field) {
    switch (field) {
        case STAT_CPU_TEMP:
            return stats->cpu_temp;

        case STAT_CPU_LOAD:
            return stats->cpu_load;

        case STAT_CPU_CLK:
            return stats->cpu_clk;

        case STAT_GPU_TEMP:
            return stats->gpu_temp;

        case STAT_GPU_LOAD:
            return stats->gpu_load;

        case STAT_GPU_VRAM:
            return stats->gpu_vram;

        case STAT_RAM_USED:
            return stats->ram_used;

        case STAT_NONE:
        default:
            return 0.0f;
    }
}

static void display_print_line(uint8_t row, const char *text) {
    char buffer[LCD_COLS + 1];

    snprintf(buffer, sizeof(buffer), "%-16s", text);

    lcd_set_cursor(row, 0);
    lcd_print(buffer);
}

static void format_line(char *out, size_t out_size, const PcStats *stats, const LineSpec *spec) {
    switch (spec->type) {
        case LINE_EMPTY:
            snprintf(out, out_size, "");
            break;

        case LINE_TEXT:
            snprintf(out, out_size, "%s", spec->text);
            break;

        case LINE_STATS: {
            float first_value = get_stat_value(stats, spec->first_stat_block.field);

            if (spec->second_stat_block.field == STAT_NONE) {
                snprintf(out,
                         out_size,
                         "%s %*.*f%s",
                         spec->prefix,
                         spec->first_stat_block.width,
                         spec->first_stat_block.precision,
                         first_value,
                         spec->first_stat_block.unit);
            } else {
                float second_value = get_stat_value(stats, spec->second_stat_block.field);

                snprintf(out,
                         out_size,
                         "%s %*.*f%s %*.*f%s",
                         spec->prefix,
                         spec->first_stat_block.width,
                         spec->first_stat_block.precision,
                         first_value,
                         spec->first_stat_block.unit,
                         spec->second_stat_block.width,
                         spec->second_stat_block.precision,
                         second_value,
                         spec->second_stat_block.unit);
            }

            break;
        }

        default:
            snprintf(out, out_size, "");
            break;
    }
}


void display_init(void) {
    lcd_init();
    lcd_clear();

    display_print_line(0, "PC Vitals");
    display_print_line(1, "Starting...");
    sleep_ms(1000);

    current_page = 0;
    last_page_change_ms = to_ms_since_boot(get_absolute_time());
}

void display_show_stats(const PcStats *stats) {
    uint32_t now = to_ms_since_boot(get_absolute_time());

    // Check if enough time has passed to display the next page. If we are at the end loop back
    if (now - last_page_change_ms >= CAROUSEL_INTERVAL_MS) {
        current_page++;

        if (current_page >= PAGE_COUNT) {
            current_page = 0;
        }

        last_page_change_ms = now;
    }

    char line0[LCD_COLS + 1];
    char line1[LCD_COLS + 1];

    // Format the strings then display
    format_line(line0, sizeof(line0), stats, &pages[current_page].line0);
    format_line(line1, sizeof(line1), stats, &pages[current_page].line1);
    display_print_line(0, line0);
    display_print_line(1, line1);
}