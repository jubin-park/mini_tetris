#ifndef DRIVER_H
#define DRIVER_H

#include <unistd.h>

#include "stdint.h"
#include "stdbool.h"

enum {
    LCD_TEXT_LENGTH = 4
};

typedef enum {
    DRIVER_LED,
    DRIVER_SEVEN_SEGMENT,
    DRIVER_DOT_MATRIX,
    DRIVER_LCD_TEXT,
    DRIVER_BUZZER,
    DRIVER_PUSH_SWITCH,
    DRIVER_SIZE
} driver_t;

bool open_drivers(void);
void close_drivers(void);

void clear_drivers(void);

void update_score_text(const uint32_t original_score);
int get_driver_file_descriptor(const driver_t driver);

#endif /* DRIVER_H */