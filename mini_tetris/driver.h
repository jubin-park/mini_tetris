#ifndef DRIVER_H
#define DRIVER_H

#include <unistd.h>

#include "stdint.h"
#include "stdbool.h"

enum {
    LED_DATA_LENGTH = 1,
    SEVEN_SEGMENT_DATA_LENGTH = 4,
    DOT_MATRIX_DATA_LENGTH = 20,
    LCD_TEXT_DATA_LENGTH = 32,
    BUZZER_DATA_LENGTH = 1,
    PUSH_SWITCH_DATA_LENGTH = 9
};

typedef enum {
    DRIVER_LED,
    DRIVER_SEVEN_SEGMENT,
    DRIVER_DOT_MATRIX,
    DRIVER_LCD_TEXT,
    DRIVER_BUZZER,
    DRIVER_PUSH_SWITCH,
    
    /* SIZE */
    DRIVER_SIZE
} driver_t;

bool open_drivers(void);
void close_drivers(void);

void clear_drivers(void);

void update_score_text(const uint32_t original_score);
int get_driver_file_descriptor(const driver_t driver);

bool update_led_lamp(uint8_t level);

#endif /* DRIVER_H */