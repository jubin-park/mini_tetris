#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "config.h"
#include "block.h"
#include "driver.h"
#include "scene.h"
#include "switch_key.h"
#include "dot10x7/scene_gameover.h"
#include "dot10x7/scene_intro.h"
#include "dot10x7/scene_pause.h"

extern const uint8_t BLOCK_TILES[BLOCK_COUNT * BLOCK_HEIGHT * ANGLE_SIZE][BLOCK_WIDTH];
extern uint8_t g_score_text[SEVEN_SEGMENT_DATA_LENGTH];
extern uint8_t g_now_switch_states[SWITCH_KEY_SIZE];
extern uint8_t g_old_switch_states[SWITCH_KEY_SIZE];

static const char* MESSAGE_INTRO[2] = { "* TETRIS GAME *", " PRESS OK START" };
static const char* MESSAGE_PAUSE[2] = { "     PAUSE", "SHUTDOWN? (Y/N)" };
static const char* MESSAGE_PLAYING[2] = { "    LEVEL #%d    ", "CANCEL TO PAUSE " }; // warning! format
static const char* MESSAGE_GAMEOVER[2] = { "   GAME OVER", "PRESS OK REGAME" };

static bool s_is_game_running = true;
static uint32_t s_score;
static uint32_t s_frame_count;
static uint8_t s_old_screen_buffer[SCREEN_HEIGHT];
static scene_t s_now_scene = SCENE_INTRO;
static block_t s_now_block;
static uint8_t s_lamp_index;
static uint8_t s_level;

void signal_exit(int sig);
void print_matrix(const uint8_t* screen_buffer);
bool render_matrix_to_device(const uint8_t* screen_buffer);
void update_scene_intro(void);
void update_scene_game(void);
void update_scene_pause(void);
void update_scene_gameover(void);

int main(void)
{
    if (!open_drivers()) {
        fprintf(stderr, "Try to execute it in sudo mode. Or Try to check if devices have been loaded.\n");

        goto lb_exit;
    }

    // clear all drivers' bits data
    clear_drivers();

    // register SIGINT handler
    (void)signal(SIGINT, signal_exit);

    // generate random seed
    srandom((unsigned int)time(NULL));

    // set timespec
    struct timespec ts_sleep = {
        .tv_sec = 0,
        .tv_nsec = DELAY_NANOSEC_PER_FRAME
    };

    set_lcd_text(MESSAGE_INTRO[0], MESSAGE_INTRO[1]);

    s_now_block = generate_random_block();

    while (s_is_game_running) {

        ++s_frame_count;

        switch (s_now_scene) {
            case SCENE_INTRO:
                update_scene_intro();
                break;

            case SCENE_GAME:
                update_scene_game();
                break;

            case SCENE_PAUSE:
                update_scene_pause();
                break;

            case SCENE_GAMEOVER:
                update_scene_gameover();
                break;

            default:
                assert(false && "invalid scene_t");
        }
        
        nanosleep(&ts_sleep, NULL);
    }

lb_exit:
    clear_drivers();
    close_drivers();

    puts("Bye bye ...");

    return 0;
}

void signal_exit(int sig)
{
    printf("\n<Exit Game>\tsig: %d\n", sig);
    s_is_game_running = false;
}

void print_matrix(const uint8_t* screen_buffer)
{
    for (int8_t y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int8_t x = 0; x < SCREEN_WIDTH; ++x) {
            putchar(((1 << x) & screen_buffer[y]) ? '*' : '.');
        }
        putchar('\n');
    }
}

bool render_matrix_to_device(const uint8_t* screen_buffer)
{
    uint8_t reversed_buffer[SCREEN_HEIGHT];
    
    for (int8_t y = 0; y < SCREEN_HEIGHT; ++y) {
        const uint8_t bits = screen_buffer[y];

        reversed_buffer[y] = (bits & 0x1) << 6 | (bits & 0x40) >> 6
                        | (bits & 0x2) << 4 | (bits & 0x20) >> 4
                        | (bits & 0x4) << 2 | (bits & 0x10) >> 2
                        | (bits & 0x8);
    }

    return write(get_driver_file_descriptor(DRIVER_DOT_MATRIX), reversed_buffer, SCREEN_HEIGHT * sizeof(uint8_t)) >= 0;
}

void update_scene_intro(void)
{
    static int s_scene_frame_count = 0;

    write(get_driver_file_descriptor(DRIVER_DOT_MATRIX), scene_intro_data[s_scene_frame_count], SCREEN_HEIGHT * sizeof(uint8_t));
    s_scene_frame_count = (s_scene_frame_count + 1) % SCENE_INTRO_FRAME_COUNT;
    
    {// get switch key state
        if (read(get_driver_file_descriptor(DRIVER_PUSH_SWITCH), g_now_switch_states, sizeof(g_now_switch_states)) < 0) {
            fprintf(stderr, "Failed to read switch key\n");

            return;
        }

        if (is_switch_key_triggered(SWITCH_KEY_OK_OR_ROTATE)) {
            puts("START");
            
            s_now_scene = SCENE_GAME;
            s_frame_count = 0;
            s_scene_frame_count = 0;
            
            s_level = 0;
            {
                char data[LCD_TEXT_DATA_LENGTH];
                memset(data, ' ', LCD_TEXT_DATA_LENGTH);

                snprintf(data, LCD_TEXT_DATA_LENGTH >> 1, MESSAGE_PLAYING[0], s_level);
                snprintf(data + (LCD_TEXT_DATA_LENGTH >> 1), LCD_TEXT_DATA_LENGTH >> 1, MESSAGE_PLAYING[1], "");

                set_lcd_text_one_line(data);
            }
        }

        memcpy(g_old_switch_states, g_now_switch_states, sizeof(g_now_switch_states));
    }
}

void update_scene_game(void)
{
    // level up
    for (int i = MAX_LEVEL; i >= 0; --i) {
        if (s_frame_count >= FRAME_PER_LEVEL_UPS[i] && s_frame_count < FRAME_PER_LEVEL_UPS[i + 1]) {
            if (s_level != i) {
                s_level = i;

                char data[LCD_TEXT_DATA_LENGTH];
                memset(data, ' ', LCD_TEXT_DATA_LENGTH);

                snprintf(data, LCD_TEXT_DATA_LENGTH >> 1, MESSAGE_PLAYING[0], s_level);
                snprintf(data + (LCD_TEXT_DATA_LENGTH >> 1), LCD_TEXT_DATA_LENGTH >> 1, MESSAGE_PLAYING[1], "");
                
                set_lcd_text_one_line(data);
            }

            break;
        }
    }

    bool is_redrawing_needed = false;

    {// get switch key state
        if (read(get_driver_file_descriptor(DRIVER_PUSH_SWITCH), g_now_switch_states, sizeof(g_now_switch_states)) < 0) {
            fprintf(stderr, "Failed to read switch key\n");

            goto lb_exit;
        }

        if (is_switch_key_triggered(SWITCH_KEY_UP)) {
            puts("UP");
        }
        else if (is_switch_key_triggered(SWITCH_KEY_DOWN)) {
            puts("DOWN");
            while (is_passable_down(s_old_screen_buffer, &s_now_block)) {
                ++s_now_block.y;

                is_redrawing_needed = true;
            }
        }
        else if (is_switch_key_triggered(SWITCH_KEY_LEFT)) {
            puts("LEFT");
            if (is_passable_left(s_old_screen_buffer, &s_now_block)) {
                --s_now_block.x;

                is_redrawing_needed = true;
            }
        }
        else if (is_switch_key_triggered(SWITCH_KEY_RIGHT)) {
            puts("RIGHT");
            if (is_passable_right(s_old_screen_buffer, &s_now_block)) {
                ++s_now_block.x;

                is_redrawing_needed = true;
            }
        }
        else if (is_switch_key_triggered(SWITCH_KEY_OK_OR_ROTATE)) {
            puts("ROTATE");
            if (is_rotatable_clockwise(s_old_screen_buffer, &s_now_block)) {
                s_now_block.angle = (s_now_block.angle + 1) % ANGLE_SIZE;

                is_redrawing_needed = true;
            }
        }
        else if (is_switch_key_triggered(SWITCH_KEY_0)
                    || is_switch_key_triggered(SWITCH_KEY_2)
                    || is_switch_key_triggered(SWITCH_KEY_6)
                    || is_switch_key_triggered(SWITCH_KEY_8)) {
            puts("PAUSED");
            s_now_scene = SCENE_PAUSE;

            set_lcd_text(MESSAGE_PAUSE[0], MESSAGE_PAUSE[1]);

            goto lb_exit;
        }

        memcpy(g_old_switch_states, g_now_switch_states, sizeof(g_now_switch_states));
    }

    if (is_redrawing_needed) { // is_redrawing_needed when block moved or rotated
        printf("x: %2d\ty: %2d\n", s_now_block.x, s_now_block.y);

        uint8_t new_screen_buffer[SCREEN_HEIGHT];
        memcpy(new_screen_buffer, s_old_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t));

        const uint8_t* p_block_tiles = s_now_block.tile_of_zero_angle + (s_now_block.angle * BLOCK_WIDTH * BLOCK_HEIGHT);

        // draw block on new_screen_buffer
        for (int y = 0; y < BLOCK_HEIGHT; ++y) {
            if (s_now_block.y + y >= 0 && s_now_block.y + y < SCREEN_HEIGHT) {
                uint8_t line = (p_block_tiles + y * BLOCK_WIDTH)[2] << 2 | (p_block_tiles + y * BLOCK_WIDTH)[1] << 1 | (p_block_tiles + y * BLOCK_WIDTH)[0];
                if (s_now_block.x >= 0) {
                    line <<= s_now_block.x;
                } else {
                    line >>= (-s_now_block.x);
                }
                new_screen_buffer[s_now_block.y + y] |= line;
            }
        }

        // real drawing
        if (!render_matrix_to_device(new_screen_buffer)) {
            fprintf(stderr, "render_matrix_to_device() error\n");

            goto lb_exit;
        }
    }

    // draw new_screen_buffer per a frame
    if (1 == s_frame_count % DELAY_PER_LEVELS[s_level]) {
        update_led_lamp(s_lamp_index);
        s_lamp_index = (s_lamp_index + 1) % LED_LEVEL;

        uint8_t new_screen_buffer[SCREEN_HEIGHT];
        memcpy(new_screen_buffer, s_old_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t));

        const uint8_t* p_block_tiles = s_now_block.tile_of_zero_angle + (s_now_block.angle * BLOCK_WIDTH * BLOCK_HEIGHT);
        //printf("%6d\t%6d\tdelay=%d\tlevel=%d\n", s_frame_count, s_frame_count % DELAY_PER_LEVELS[s_level], DELAY_PER_LEVELS[s_level], s_level);
        printf("block #%d\tx: %2d y: %2d\tangle: %3d\tframe: %6d\tlevel: %d\n",
            (s_now_block.tile_of_zero_angle - BLOCK_TILES[0]) / (BLOCK_HEIGHT * ANGLE_SIZE * BLOCK_WIDTH),
            s_now_block.x,
            s_now_block.y,
            s_now_block.angle * 90,
            s_frame_count,
            s_level);

        // draw block on new_screen_buffer
        for (int y = 0; y < BLOCK_HEIGHT; ++y) {
            if (s_now_block.y + y >= 0 && s_now_block.y + y < SCREEN_HEIGHT) {
                uint8_t line = (p_block_tiles + y * BLOCK_WIDTH)[2] << 2 | (p_block_tiles + y * BLOCK_WIDTH)[1] << 1 | (p_block_tiles + y * BLOCK_WIDTH)[0];
                if (s_now_block.x >= 0) {
                    line <<= s_now_block.x;
                } else {
                    line >>= (-s_now_block.x);
                }
                new_screen_buffer[s_now_block.y + y] |= line;
            }
        }

        // real drawing
        if (!render_matrix_to_device(new_screen_buffer)) {
            fprintf(stderr, "render_matrix_to_device() error\n");

            goto lb_exit;
        }

        if (!is_passable_down(new_screen_buffer, &s_now_block)) {
            printf("collision occured\n");

            if ((new_screen_buffer[0] & 0x7f) > 0) {
                puts("== GAME OVER ==");

                s_now_scene = SCENE_GAMEOVER;
                set_lcd_text(MESSAGE_GAMEOVER[0], MESSAGE_GAMEOVER[1]);

                goto lb_exit;
            }

            int8_t removed_height = 0;
            
            while (true) {
                bool is_line_found = false;

                for (int8_t h = SCREEN_HEIGHT - 1; h >= 0; --h) {
                    if ((new_screen_buffer[h] & 0xff) == 0x7f) {
                        ++removed_height;
                        
                        for (int8_t y = h; y >= 1; --y) {
                            new_screen_buffer[y] = new_screen_buffer[y - 1];
                        }

                        is_line_found = true;

                        break;
                    }
                }

                if (!is_line_found) {
                    break;
                }
            }

            if (removed_height > 0) {
                memset(new_screen_buffer, 0x0, removed_height * sizeof(uint8_t));
                s_score += removed_height * DEFAULT_SCORE;

                update_score_text(s_score);

                // when line is removed, is_redrawing_needed again
                if (!render_matrix_to_device(new_screen_buffer)) {
                    fprintf(stderr, "render_matrix_to_device() error\n");

                    goto lb_exit;
                }
            }

            memcpy(s_old_screen_buffer, new_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t));

            s_now_block = generate_random_block();
        }
        else {
            s_now_block.y++;
        }

        print_matrix(new_screen_buffer);
    }

lb_exit:
    return;
}

void update_scene_pause(void)
{
    static int s_scene_frame_count = 0;

    write(get_driver_file_descriptor(DRIVER_DOT_MATRIX), scene_pause_data[s_scene_frame_count], SCREEN_HEIGHT * sizeof(uint8_t));
    s_scene_frame_count = (s_scene_frame_count + 1) % SCENE_PAUSE_FRAME_COUNT;

    {// get switch key state
        if (read(get_driver_file_descriptor(DRIVER_PUSH_SWITCH), g_now_switch_states, sizeof(g_now_switch_states)) < 0) {
            fprintf(stderr, "Failed to read switch key\n");

            return;
        }

        if (is_switch_key_triggered(SWITCH_KEY_OK_OR_ROTATE)) { // Goto Scene Intro
            puts("RESUME");
            
            s_now_scene = SCENE_INTRO;
            s_frame_count = 0;
            s_scene_frame_count = 0;
            s_lamp_index = 0;
            clear_drivers();

            set_lcd_text(MESSAGE_INTRO[0], MESSAGE_INTRO[1]);
        }
        else if (is_switch_key_triggered(SWITCH_KEY_0)
                    || is_switch_key_triggered(SWITCH_KEY_2)
                    || is_switch_key_triggered(SWITCH_KEY_6)
                    || is_switch_key_triggered(SWITCH_KEY_8)) { // Resume

            s_now_scene = SCENE_GAME;
            s_frame_count = 0;
            s_scene_frame_count = 0;
        }

        memcpy(g_old_switch_states, g_now_switch_states, sizeof(g_now_switch_states));
    }
}

void update_scene_gameover(void)
{
    static int s_scene_frame_count = 0;

    write(get_driver_file_descriptor(DRIVER_DOT_MATRIX), scene_gameover_data[s_scene_frame_count], SCREEN_HEIGHT * sizeof(uint8_t));
    s_scene_frame_count = (s_scene_frame_count + 1) % SCENE_GAMEOVER_FRAME_COUNT;

    {// get switch key state
        if (read(get_driver_file_descriptor(DRIVER_PUSH_SWITCH), g_now_switch_states, sizeof(g_now_switch_states)) < 0) {
            fprintf(stderr, "Failed to read switch key\n");

            return;
        }

        if (is_switch_key_triggered(SWITCH_KEY_OK_OR_ROTATE)) { // Goto Scene Intro
            puts("OK - SCENE_INTRO");
            
            s_now_scene = SCENE_INTRO;
            s_frame_count = 0;
            s_scene_frame_count = 0;
            s_score = 0;
            s_lamp_index = 0;
            
            memset(s_old_screen_buffer, 0x0, SCREEN_HEIGHT * sizeof(uint8_t)); // clear board
            clear_drivers();

            set_lcd_text(MESSAGE_INTRO[0], MESSAGE_INTRO[1]);
        }

        memcpy(g_old_switch_states, g_now_switch_states, sizeof(g_now_switch_states));
    }
}