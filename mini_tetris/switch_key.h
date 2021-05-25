#ifndef SWITCH_KEY_H
#define SWITCH_KEY_H

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

#endif /* SWITCH_KEY_H */