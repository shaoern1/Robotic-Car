#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "barcode.h"

// Define states for the state machine
typedef enum {
    BARCODE_IDLE,
    BARCODE_READING,
    BARCODE_COMPLETE
} barcode_state_t;

static barcode_state_t barcode_state = BARCODE_IDLE;

typedef struct {
    float width;
    int state;
} BarInfo;

typedef struct {
    char pattern[BARS_PER_CHAR + 1];
    char character;
} Code39Mapping;

const Code39Mapping CODE39_DICT[] = {
    {"000110100", '0'},
    {"100100001", '1'},
    {"001100001", '2'},
    {"101100000", '3'},
    {"000110001", '4'},
    {"100110000", '5'},
    {"001110000", '6'},
    {"000100101", '7'},
    {"100100100", '8'},
    {"001100100", '9'},
    {"100001001", 'A'},
    {"001001001", 'B'},
    {"101001000", 'C'},
    {"000011001", 'D'},
    {"100011000", 'E'},
    {"001011000", 'F'},
    {"000001101", 'G'},
    {"100001100", 'H'},
    {"001001100", 'I'},
    {"000011100", 'J'},
    {"100000011", 'K'},
    {"001000011", 'L'},
    {"101000010", 'M'},
    {"000010011", 'N'},
    {"100010010", 'O'},
    {"001010010", 'P'},
    {"000000111", 'Q'},
    {"100000110", 'R'},
    {"001000110", 'S'},
    {"000010110", 'T'},
    {"110000001", 'U'},
    {"011000001", 'V'},
    {"111000000", 'W'},
    {"010010001", 'X'},
    {"110010000", 'Y'},
    {"011010000", 'Z'},
    {"010000101", '-'},
    {"110000100", '.'},
    {"011000100", ' '},
    {"010101000", '$'},
    {"010100010", '/'},
    {"010001010", '+'},
    {"000101010", '%'}
};

char decoded_message[MAX_MESSAGE_LENGTH];
int message_length = 0;

void init_barcode() {
    // Initialize GPIO for IR sensor
    gpio_init(IR_SENSOR_PIN);
    gpio_set_dir(IR_SENSOR_PIN, GPIO_IN);
    gpio_init(RESET_BUTTON_PIN);
    gpio_set_dir(RESET_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(RESET_BUTTON_PIN);
}

int get_denoised_state() {
    int sum = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        sum += gpio_get(IR_SENSOR_PIN);
        sleep_ms(1);
    }
    float average = (float)sum / SAMPLE_SIZE;
    return (average > THRESHOLD) ? 1 : 0;
}

void sort_bars(BarInfo* bars, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (bars[j].width < bars[j + 1].width) {
                BarInfo temp = bars[j];
                bars[j] = bars[j + 1];
                bars[j + 1] = temp;
            }
        }
    }
}

int count_wide_bars(const char* pattern) {
    int count = 0;
    for (int i = 0; i < BARS_PER_CHAR; i++) {
        if (pattern[i] == '1') count++;
    }
    return count;
}

bool has_initial_outlier(BarInfo* bars) {
    BarInfo sorted_bars[BARS_PER_CHAR];
    memcpy(sorted_bars, bars, sizeof(BarInfo) * BARS_PER_CHAR);
    sort_bars(sorted_bars, BARS_PER_CHAR);

    if (sorted_bars[0].width > sorted_bars[1].width * OUTLIER_RATIO) {
        for (int i = 0; i < BARS_PER_CHAR; i++) {
            if (bars[i].width == sorted_bars[0].width && i == 0) {
                return true;
            }
        }
    }
    return false;
}

char decode_pattern(const char* pattern) {
    for (size_t i = 0; i < sizeof(CODE39_DICT) / sizeof(CODE39_DICT[0]); i++) {
        if (strcmp(pattern, CODE39_DICT[i].pattern) == 0) {
            return CODE39_DICT[i].character;
        }
    }
    return '?';  // Return ? for unknown patterns
}

void add_to_message(char decoded_char) {
    if (message_length < MAX_MESSAGE_LENGTH - 1) {  // Leave room for null terminator
        // Only add the character if it's not a start/stop character (*)
        if (decoded_char != '*') {
            decoded_message[message_length++] = decoded_char;
            decoded_message[message_length] = '\0';  // Null terminate
            printf("Current decoded message: %s\n", decoded_message);
        }
    }
    else {
        printf("Warning: Message buffer full, cannot add more characters\n");
    }
}

void reset_message() {
    message_length = 0;
    decoded_message[0] = '\0';
    printf("Message buffer reset\n");
}

void process_bars(BarInfo* bars, int character_count) {
    char pattern[BARS_PER_CHAR + 1];
    BarInfo sorted_bars[BARS_PER_CHAR];
    memcpy(sorted_bars, bars, sizeof(BarInfo) * BARS_PER_CHAR);
    sort_bars(sorted_bars, BARS_PER_CHAR);

    float threshold = sorted_bars[2].width - 0.1;

    for (int i = 0; i < BARS_PER_CHAR; i++) {
        pattern[i] = (bars[i].width > threshold) ? '1' : '0';
    }
    pattern[BARS_PER_CHAR] = '\0';

    int wide_count = count_wide_bars(pattern);

    printf("\n=== Character %d Analysis ===\n", character_count);
    printf("Threshold (3rd highest width): %.2f ms\n", threshold);
    printf("Bar widths (sorted): ");
    for (int i = 0; i < BARS_PER_CHAR; i++) {
        printf("%.2f ", sorted_bars[i].width);
    }
    printf("\nPattern: %s\n", pattern);
    printf("Wide bars count: %d ", wide_count);

    if (wide_count == WIDE_BARS_PER_CHAR) {
        printf("(VALID Code 39 pattern)\n");
        char decoded_char = decode_pattern(pattern);
        printf("Decoded character: %c\n", decoded_char);
        add_to_message(decoded_char);
    }
    else {
        printf("(INVALID - Code 39 requires exactly %d wide bars)\n", WIDE_BARS_PER_CHAR);
        printf("Possible issues:\n");
        if (wide_count < WIDE_BARS_PER_CHAR) {
            printf("- Threshold might be too high\n");
            printf("- Bar widths not sufficiently distinct\n");
        }
        else {
            printf("- Threshold might be too low\n");
            printf("- Noise in readings\n");
        }
    }

    printf("========================\n\n");
}

void track_bars() {
    BarInfo bars[BARS_PER_CHAR];
    int current_state = get_denoised_state();
    int bar_count = 0;
    int character_count = 0;
    absolute_time_t bar_start_time = get_absolute_time();
    char pattern[BARS_PER_CHAR + 1];
    bool collecting_extra = false;
    BarInfo temp_bars[BARS_PER_CHAR + 1];
    bool skip_next_white = false;  // Flag to skip the next white bar after character detection

    printf("Starting bar tracking...\n");
    fflush(stdout);  // Flush the output buffer to ensure the message is sent
    printf("Move barcode past sensor...\n");
    fflush(stdout);  // Flush the output buffer to ensure the message is sent
    printf("Press reset button (GP20) to reset counters\n\n");
    fflush(stdout);  // Flush the output buffer to ensure the message is sent

    while (true) {
        if (gpio_get(RESET_BUTTON_PIN) == 0) {
            printf("\n=== RESET TRIGGERED ===\n");
            fflush(stdout);  // Flush the output buffer to ensure the message is sent
            printf("Resetting bar counter and character counter\n\n");
            fflush(stdout);  // Flush the output buffer to ensure the message is sent
            bar_count = 0;
            character_count = 0;
            collecting_extra = false;
            skip_next_white = false;
            reset_message();
            sleep_ms(200);
            continue;
        }

        int new_state = get_denoised_state();

        if (new_state != current_state) {
            absolute_time_t current_time = get_absolute_time();
            float bar_width_ms = absolute_time_diff_us(bar_start_time, current_time) / 1000.0;

            // Skip the white bar if it's the intercharacter gap
            if (skip_next_white && new_state == 1) {
                printf("Skipping intercharacter gap (white bar)\n");
                skip_next_white = false;  // Reset the flag
            }
            else if (!collecting_extra) {
                bars[bar_count].width = bar_width_ms;
                bars[bar_count].state = new_state;

                printf("Character %d - Bar %d: %s, Width: %.2f ms\n",
                    character_count,
                    bar_count,
                    new_state ? "WHITE" : "BLACK",
                    bar_width_ms);

                bar_count++;

                if (bar_count >= BARS_PER_CHAR) {
                    if (has_initial_outlier(bars)) {
                        printf("\nFirst bar appears to be an outlier - collecting one more bar...\n");
                        memcpy(temp_bars, bars, sizeof(bars));
                        collecting_extra = true;
                        bar_count = BARS_PER_CHAR;
                    }
                    else {
                        process_bars(bars, character_count);
                        bar_count = 0;
                        character_count++;
                        skip_next_white = true;  // Set flag to skip next white bar (intercharacter gap)
                    }
                }
            }
            else {
                temp_bars[BARS_PER_CHAR].width = bar_width_ms;
                temp_bars[BARS_PER_CHAR].state = new_state;

                printf("Extra bar: %s, Width: %.2f ms\n",
                    new_state ? "WHITE" : "BLACK",
                    bar_width_ms);

                for (int i = 0; i < BARS_PER_CHAR; i++) {
                    bars[i] = temp_bars[i + 1];
                }

                process_bars(bars, character_count);

                bar_count = 0;
                character_count++;
                collecting_extra = false;
                skip_next_white = true;  // Set flag to skip next white bar (intercharacter gap)
            }

            bar_start_time = current_time;
            current_state = new_state;
        }

        sleep_ms(100);
    }
}

//int main() {
  //  stdio_init_all();
    //init_gpio();

    //sleep_ms(1000);

  //  printf("IR Bar Pattern Analyzer (Code 39)\n");
   // printf("0 = narrow bar, 1 = wide bar\n");
   // printf("Using top 3 widths to determine threshold\n");
   // printf("Validating for exactly %d wide bars per character\n", WIDE_BARS_PER_CHAR);
    //printf("Using %d samples for denoising\n", SAMPLE_SIZE);
   // printf("Reset button on GP20\n\n");

   // track_bars();

   // return 0;
//}