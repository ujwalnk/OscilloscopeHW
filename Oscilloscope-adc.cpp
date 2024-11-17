#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/multicore.h"

#define TOGGLE_PIN_ADC 15
#define TOGGLE_PIN_PRINT 16
#define BUFFER_SIZE 25
#define BUFFER_LEN BUFFER_SIZE - 1

#define DEBUG 1

uint16_t buffer_1[BUFFER_SIZE];
uint16_t buffer_2[BUFFER_SIZE];

volatile int buffer_index_1 = 0;
volatile int buffer_index_2 = 0;

volatile bool buffer_1_ready_4_print = false;
volatile bool buffer_2_ready_4_print = false;

// Function to be executed on core 1
void print_task()
{
#ifdef DEBUG
    // Initialize the GPIO for toggling
    gpio_init(TOGGLE_PIN_PRINT);
    gpio_set_dir(TOGGLE_PIN_PRINT, GPIO_OUT);
#endif
    while (true)
    {
        if ((buffer_index_1 == BUFFER_LEN) && buffer_1_ready_4_print)
        {
#ifdef DEBUG
            gpio_put(TOGGLE_PIN_PRINT, 1);
#endif
            char buffer_str[BUFFER_SIZE * 4 + 3]; // Adjust size to handle the numbers and commas, +3 for brackets and null terminator
            int offset = 0;

            // Start with the opening bracket
            // buffer_str[offset++] = '[';

            // Concatenate each number and a comma
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                // Add the number to the buffer
                // offset += snprintf(buffer_str + offset, sizeof(buffer_str) - offset, "%d", buffer_1[i]);

                // Add a comma if it's not the last element
                // if (i < BUFFER_LEN)
                // {
                    // buffer_str[offset++] = ',';
                // }
                printf("%d\n", buffer_1[i]);
            }

            // Close with the closing bracket
            // buffer_str[offset++] = ']';
            // buffer_str[offset] = '\0'; // Null-terminate the string

            // Print the concatenated string
            // printf("%s\n", buffer_str);

            // Reset the buffer and flags
            buffer_index_1 = 0;
            buffer_1_ready_4_print = false;
#ifdef DEBUG
            gpio_put(TOGGLE_PIN_PRINT, 0);
#endif
        }

        if ((buffer_index_2 == BUFFER_LEN) && buffer_2_ready_4_print)
        {
            char buffer_str[BUFFER_SIZE * 4 + 3]; // Adjust size to handle the numbers and commas, +3 for brackets and null terminator
            int offset = 0;

            // Start with the opening bracket
            // buffer_str[offset++] = '[';

            // Concatenate each number and a comma
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                printf("%d\n", buffer_1[i]);
                // Add the number to the buffer
                // offset += snprintf(buffer_str + offset, sizeof(buffer_str) - offset, "%d", buffer_2[i]);

                // Add a comma if it's not the last element
                // if (i < BUFFER_LEN)
                // {
                    // buffer_str[offset++] = ',';
                // }
            }

            // Close with the closing bracket
            // buffer_str[offset++] = ']';
            // buffer_str[offset] = '\0'; // Null-terminate the string

            // Print the concatenated string
            // printf("%s\n", buffer_str);

            // Reset the buffer and flags
            buffer_index_2 = 0;
            buffer_2_ready_4_print = false;
        }
    }
}

int main()
{
    stdio_init_all();

    // Initialize the ADC
    adc_init();
    adc_select_input(0); // Select ADC0 (GPIO26) as the input channel

    // Initialize the GPIO for toggling
    gpio_init(TOGGLE_PIN_ADC);
    gpio_set_dir(TOGGLE_PIN_ADC, GPIO_OUT);

    // Launch the secondary core task
    multicore_launch_core1(print_task);

    while (true)
    {
        // Read ADC value
        uint16_t adc_value = adc_read();

        if (!buffer_1_ready_4_print)
        {
#ifdef DEBUG
            gpio_put(TOGGLE_PIN_ADC, 1);
#endif
            buffer_1[buffer_index_1++] = adc_value;
            if (buffer_index_1 >= BUFFER_LEN)
            {
                buffer_1_ready_4_print = true;
            }
        }
        else if (!buffer_2_ready_4_print)
        {
#ifdef DEBUG
            gpio_put(TOGGLE_PIN_ADC, 0);
#endif
            buffer_2[buffer_index_2++] = adc_value;
            if (buffer_index_2 == BUFFER_LEN)
            {
                buffer_2_ready_4_print = true;
            }
        }
    }

    return 0;
}