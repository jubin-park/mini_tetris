#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "driver.h"

static const char* DRIVER_NAMES[DRIVER_SIZE] = {
    "/dev/csemad_led",
    "/dev/csemad_seven_segment",
    "/dev/csemad_dot_matrix",
    "/dev/csemad_lcd_text",
    "/dev/csemad_buzzer",
    "/dev/csemad_push_switch"
};

static const uint8_t DRIVER_DATA_LENGTHS[DRIVER_SIZE] = {
    LED_DATA_LENGTH,
    SEVEN_SEGMENT_DATA_LENGTH,
    DOT_MATRIX_DATA_LENGTH,
    LCD_TEXT_DATA_LENGTH,
    BUZZER_DATA_LENGTH,
    PUSH_SWITCH_DATA_LENGTH
};

static int s_driver_file_descriptors[DRIVER_SIZE] = { -1, -1, -1, -1, -1, -1 };

uint8_t g_score_text[SEVEN_SEGMENT_DATA_LENGTH];

bool open_drivers(void)
{
    bool has_error = false;

    for (size_t i = 0; i < DRIVER_SIZE; ++i) {
        printf("Loading driver '%s' ... ", DRIVER_NAMES[i]);

        s_driver_file_descriptors[i] = open(DRIVER_NAMES[i], O_RDWR);
        has_error |= s_driver_file_descriptors[i] < 0;

        puts(s_driver_file_descriptors[i] < 0 ? "FAILED" : "success");
    }

    if (has_error) {
        fprintf(stderr, "Try to execute it in sudo mode\n");

        return false;
    }

    return true;
}

void close_drivers(void)
{
    for (size_t i = 0; i < DRIVER_SIZE; ++i) {
        close(s_driver_file_descriptors[i]);
    }
}

void clear_drivers(void)
{
    uint8_t zeros[32] = { 0 };

    for (size_t i = 0; i < DRIVER_SIZE; ++i) {
        write(s_driver_file_descriptors[i], zeros, DRIVER_DATA_LENGTHS[i]);
    }
}

void update_score_text(const uint32_t original_score)
{
    uint32_t score = original_score;
    uint8_t* p = g_score_text + SEVEN_SEGMENT_DATA_LENGTH;

    memset(g_score_text, 0, SEVEN_SEGMENT_DATA_LENGTH);

    if (score > 9999) {
        score = 9999;
    }

    do {
        *--p = score % 10;
        score /= 10;
    } while (score > 0 && p >= g_score_text);

    if (write(s_driver_file_descriptors[DRIVER_SEVEN_SEGMENT], g_score_text, SEVEN_SEGMENT_DATA_LENGTH) < 0) {
        fprintf(stderr, "update_score_text Error\n");
    }
}

int get_driver_file_descriptor(const driver_t driver)
{
    return s_driver_file_descriptors[driver];
}