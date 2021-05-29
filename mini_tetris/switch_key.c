#include "switch_key.h"

uint8_t g_now_switch_states[SWITCH_KEY_SIZE];
uint8_t g_old_switch_states[SWITCH_KEY_SIZE];

bool is_switch_key_pressed(const switch_key_t key)
{
    return g_now_switch_states[key] > 0;
}

bool is_switch_key_triggered(const switch_key_t key)
{
    if (0 == g_old_switch_states[key] && g_now_switch_states[key] > 0) {
        g_old_switch_states[key] = 1;

        return true;
    }

    return false;
}