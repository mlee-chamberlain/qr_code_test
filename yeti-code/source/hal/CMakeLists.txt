cmake_minimum_required(VERSION 3.13)

project(  hal
    VERSION 0.1
    DESCRIPTION "hal"
    LANGUAGES
        C
)

add_library(${PROJECT_NAME}
    src/debug_log.c
    src/buttons.c
    src/command_broker.c
    src/lcd.c
    src/lcd_font_4_22.c
    src/beeper.c
)

target_include_directories (${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)



target_link_libraries( ${PROJECT_NAME}
    PRIVATE
    gecko-sdk
    driver_wrappers
)
