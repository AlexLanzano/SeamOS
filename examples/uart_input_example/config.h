#ifndef CONFIG_H
#define CONFIG_H

// Clock Config
#define CONFIG_CLOCK_SOURCE RCC_SYSTEM_CLOCK_SOURCE_PLL
#define CONFIG_CLOCK_FREQ 64000000

// Peripheral Enable
#define CONFIG_ENABLE_GPIO_A
#define CONFIG_ENABLE_GPIO_B
#define CONFIG_ENABLE_LPUART1

// LPUART1 Config
#define CONFIG_LPUART1_BAUD 115200
#define CONFIG_PIN_LPUART1_TX 2
#define CONFIG_PIN_LPUART1_RX 3
#define CONFIG_LPUART1_WORD_LENGTH UART_WORD_LENGTH_8

// Pin Map
#define CONFIG_PIN_MAP {(gpio_t){.pin=0,.port=GPIOA},  \
                        (gpio_t){.pin=1,.port=GPIOA},  \
                        (gpio_t){.pin=2,.port=GPIOA},  \
                        (gpio_t){.pin=3,.port=GPIOA},  \
                        (gpio_t){.pin=4,.port=GPIOA},  \
                        (gpio_t){.pin=5,.port=GPIOA},  \
                        (gpio_t){.pin=6,.port=GPIOA},  \
                        (gpio_t){.pin=7,.port=GPIOA},  \
                        (gpio_t){.pin=8,.port=GPIOA},  \
                        (gpio_t){.pin=9,.port=GPIOA},  \
                        (gpio_t){.pin=15,.port=GPIOA}, \
                        (gpio_t){.pin=0,.port=GPIOB}, \
                        (gpio_t){.pin=1,.port=GPIOB}, \
                        (gpio_t){.pin=10,.port=GPIOA}, \
                        (gpio_t){.pin=4,.port=GPIOB}, \
                        (gpio_t){.pin=5,.port=GPIOB}, \
                         (gpio_t){.pin=6,.port=GPIOB}, \
                        (gpio_t){.pin=7,.port=GPIOB}, \
                        (gpio_t){.pin=8,.port=GPIOB}, \
                        (gpio_t){.pin=9,.port=GPIOB}}

// GPIO Config
#define CONFIG_GPIO_SPEED GPIO_OUTPUT_SPEED_FAST

// Task Config
#define CONFIG_TASK_MAX 8

//Device Config
#define CONFIG_DEVICE_NAME_LENGTH 16

#endif
