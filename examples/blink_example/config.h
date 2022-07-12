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
                        (gpio_t){.pin=10,.port=GPIOB}, \
                        (gpio_t){.pin=11,.port=GPIOB}, \
                        (gpio_t){.pin=12,.port=GPIOB}, \
                        (gpio_t){.pin=13,.port=GPIOB}, \
                        (gpio_t){.pin=14,.port=GPIOB}, \
                        (gpio_t){.pin=15,.port=GPIOB}, \
                        (gpio_t){.pin=16,.port=GPIOB}, \
                        (gpio_t){.pin=17,.port=GPIOB}, \
                        (gpio_t){.pin=18,.port=GPIOB}, \
                        (gpio_t){.pin=19,.port=GPIOB}}

// Task Config
#define CONFIG_TASK_MAX 8

//Device Config
#define CONFIG_DEVICE_NAME_LENGTH 16

#endif
