cmake_minimum_required(VERSION 3.13)

project(  driver_wrappers
    VERSION 0.1
    DESCRIPTION "driver_wrappers"
    LANGUAGES
        C
)

add_library(${PROJECT_NAME}
    src/eeprom_gpio.c
    src/gpio_led.c
    src/lcd_gpio.c
    src/buzzer_pwm.c
    src/watchdog.c
    src/powered_uart.c
    src/button_gpio.c
    src/debug_uart.c
	src/lcd_spi.c
)

target_include_directories (${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)



target_link_libraries( ${PROJECT_NAME}
    PRIVATE
    gecko-sdk
)
