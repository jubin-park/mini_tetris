#include <assert.h>
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
    LCD_TEXT_DATA_LENGTH, // CRASH
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
    write(s_driver_file_descriptors[DRIVER_LED], zeros, DRIVER_DATA_LENGTHS[DRIVER_LED]);
    write(s_driver_file_descriptors[DRIVER_SEVEN_SEGMENT], zeros, DRIVER_DATA_LENGTHS[DRIVER_SEVEN_SEGMENT]);
    write(s_driver_file_descriptors[DRIVER_DOT_MATRIX], zeros, DRIVER_DATA_LENGTHS[DRIVER_DOT_MATRIX]);
    write(s_driver_file_descriptors[DRIVER_BUZZER], zeros, DRIVER_DATA_LENGTHS[DRIVER_BUZZER]);
    write(s_driver_file_descriptors[DRIVER_PUSH_SWITCH], zeros, DRIVER_DATA_LENGTHS[DRIVER_PUSH_SWITCH]);

    char blanks[LCD_TEXT_DATA_LENGTH];
    memset(blanks, ' ', LCD_TEXT_DATA_LENGTH);
    write(s_driver_file_descriptors[DRIVER_LCD_TEXT], blanks, LCD_TEXT_DATA_LENGTH);
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

bool update_led_lamp(uint8_t level)
{
    level %= LED_LEVEL;

    const uint8_t data = 1 << level | 1 << (level + 4);
    
    return write(s_driver_file_descriptors[DRIVER_LED], &data, LED_DATA_LENGTH) >= 0;
}

bool set_lcd_text(const char* line1, const char* line2)
{
    char data[LCD_TEXT_DATA_LENGTH];
    
    memset(data, ' ', LCD_TEXT_DATA_LENGTH);

    {
        char* p = data;
        while (*line1 != '\0') {
            *p++ = *line1++;
        }
    }

    {
        char* p = data + (LCD_TEXT_DATA_LENGTH >> 1);
        while (*line2 != '\0') {
            *p++ = *line2++;
        }
    }

    return write(s_driver_file_descriptors[DRIVER_LCD_TEXT], data, LCD_TEXT_DATA_LENGTH) >= 0;
}

bool set_lcd_text_one_line(const char* data)
{
    return write(s_driver_file_descriptors[DRIVER_LCD_TEXT], data, LCD_TEXT_DATA_LENGTH) >= 0;
}