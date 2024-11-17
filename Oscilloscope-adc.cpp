#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define TOGGLE_PIN_ADC 15
#define TOGGLE_PIN_PRINT 17
#define BUFFER_SIZE 124400
#define BUFFER_LEN BUFFER_SIZE - 1

#define ADC_NUM 1
#define ADC_PIN (26 + ADC_NUM)

#define DEBUG 1

int main()
{
    stdio_init_all();

    // Initialize the ADC
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_NUM); // Select ADC0 (GPIO26) as the input channel

    // Initialize the GPIO for toggling
    gpio_init(TOGGLE_PIN_ADC);
    gpio_set_dir(TOGGLE_PIN_ADC, GPIO_OUT);

    while (true)
    {

        uint16_t adc_value = adc_read();

        // Scale the 12-bit ADC value to 8-bit (0-255)
        uint8_t scaled_value = adc_value >> 4; // Right shift by 4 to reduce 12-bit to 8-bit

        printf("%d\n", scaled_value);

        // Optional: Toggle a GPIO pin (for debugging or status indication)
        gpio_put(TOGGLE_PIN_ADC, !gpio_get(TOGGLE_PIN_ADC));

        sleep_us(10);
    }

    return 0;
}
