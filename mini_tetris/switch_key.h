#ifndef SWITCH_KEY_H
#define SWITCH_KEY_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
// keys
    SWITCH_KEY_0,    SWITCH_KEY_UP,           SWITCH_KEY_2,
    SWITCH_KEY_LEFT, SWITCH_KEY_OK_OR_ROTATE, SWITCH_KEY_RIGHT,
    SWITCH_KEY_6,    SWITCH_KEY_DOWN,         SWITCH_KEY_8,
// size
    SWITCH_KEY_SIZE,
// cancel key
    SWITCH_KEY_CANCEL = SWITCH_KEY_0 | SWITCH_KEY_2 | SWITCH_KEY_6 | SWITCH_KEY_8,
} switch_key_t;

extern uint8_t g_now_switch_states[SWITCH_KEY_SIZE];
extern uint8_t g_old_switch_states[SWITCH_KEY_SIZE];

bool is_switch_key_pressed(const switch_key_t key);
bool is_switch_key_triggered(const switch_key_t key);

#endif /* SWITCH_KEY_H */