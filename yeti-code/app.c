/** @file app.c
 *
 * @brief
 *
 * @author Rene Delgado
 *         Ilya Mushkirov
 *         Ivan Jauregui
 *
 * @copyright 2022 The Chamberlain Group, LLC. All rights reserved. All
 * information within this file and associated files, including all
 * information and files transmitted with this file are CONFIDENTIAL
 * and the proprietary property of The Chamberlain Group, LLC.
 *
 * In using the Licensed Software, Company shall not use with, combine, or
 * incorporate any viral open source software with or into any of the Licensed
 * Software in a manner that would require any portion of the Licensed software to
 * be (i) disclosed or distributed in source code form; (ii) licensed for the
 * purpose of making derivative works; or (iii) distributable or redistributable
 * at no charge.
 */

#include "sl_sleeptimer.h"

#include "gpio_led.h"
#include "lcd_gpio.h"
#include "command_broker.h"
#include "powered_uart.h"
#include "buttons.h"
#include "lcd_spi.h"
#include "lcd.h"
#include "debug_uart.h"
#include "debug_log.h"
#include "beeper.h"
#include "buzzer_pwm.h"
#include "watchdog.h"
#include "lcd.h"

/*****************************
 * Local Defines
 ******************************/

/*****************************
 * Firmware Version
 ******************************/
#define VERSION_MAJOR (0)
#define VERSION_MINOR (2)
#define VERSION_BUILD (10)


#define BUTTON_CLOSE (1 << 2)
#define BUTTON_STOP (1 << 1)
#define BUTTON_OPEN (1)
#define PIN_LOOPBACK (1 << 3)

#define MAIN_LOOP_DELAY (20)
#define BUTTONS_DEBOUNCER_DELAY (10)
#define BITS_IN_A_BYTE (8)


/* Disp FW Version Masks & shift values */
#define VERSION_MAJOR_MASK  0xF800
#define VERSION_MAJOR_SHIFT 11
#define VERSION_MINOR_MASK  0x07C0
#define VERSION_MINOR_SHIFT 6
#define VERSION_BUILD_MASK  0x003F


base_driver powered_uart;
base_driver debug_uart;
base_driver spi_port;
beeper_t    beeper = {.cfg.freq          = (CB_BUZ_PARAM_DATA1_FREQ_LOWER_LIMIT * CB_BUZ_PARAM_DATA1_FREQ_STEP),
                      .cfg.percent       = CB_BUZ_PARAM_DATA1_DUTY_CYCLE_LOWER_LIMIT,
                      .cfg.num_of_cycles = CB_BUZ_CTRL_DATA0_NUM_CYCLES_LOWER_LIMIT,
                      .cfg.time_on       = CB_BUZ_CTRL_DATA1_BEEPER_ON_UPPER_LIMIT,
                      .cfg.time_off      = CB_BUZ_CTRL_DATA2_BEEPER_OFF_LOWER_LIMIT,
                      .cfg.timer.self    = TIMER2,
                      .cfg.timer.init    = TIMER_INIT_DEFAULT,
                      .cfg.timer.initCC  = TIMER_INITCC_DEFAULT,
                      .cfg.timer.ch      = 0,
                      .cfg.timer.freq    = BEEPER_TIMER_FREQUENCY_DEFAULT};

static uint8_t buttons_overall_status = 0;
static uint8_t cycle_to_next_qr = 0;
//Definiton for lcd alignment
static lcd_line_t lcd_layout[LCD_LINE_NUM] = {{0, LCD_LINE_PIXEL_HEIGHT}, {4, LCD_LINE_PIXEL_HEIGHT}, {1, LCD_LINE_PIXEL_HEIGHT}, {3, LCD_LINE_PIXEL_HEIGHT}};

//Definiton for lcd alignment to display QR Code

// lcd_layout_full_height contains the text line data with full height size without
// any spaces between screen lines
static lcd_line_t lcd_layout_full_height[LCD_LINE_NUM] = {{0, LCD_LINE_PIXEL_HEIGHT}, {0, LCD_LINE_PIXEL_HEIGHT}, {0, LCD_LINE_PIXEL_HEIGHT}, {0, LCD_LINE_PIXEL_HEIGHT}};

struct
{
    uint16_t                    version;
    uint16_t                    main_loop_delay;
    cbroker_set_language_data_e language;
} settings; // just a settings struct to keep a local settings

uint8_t qr_version_to_display = 7;

void cycle_qr()
{
    qr_version_to_display++;
    if(qr_version_to_display == 8)
    {
        qr_version_to_display = 4;        
    }
}

#pragma region buttons_callback

/**
 * @brief stop button callback function
 */
void button_stop_cb(uint8_t *status)
{
    //cycle_to_next_qr = 1;
    if(*status == 1)
    {
        buttons_overall_status |= BUTTON_STOP;
    }
    else
    {
        buttons_overall_status ^= BUTTON_STOP;
    }
}

/**
 * @brief close button callback function
 */
void button_close_cb(uint8_t *status)
{
    //cycle_to_next_qr = 1;
    if(*status == 1)
    {
        buttons_overall_status |= BUTTON_CLOSE;
    }
    else
    {
        buttons_overall_status ^= BUTTON_CLOSE;
    }
}

/**
 * @brief open button callback function
 */
void button_open_cb(uint8_t *status)
{
    //cycle_to_next_qr = 1;
    if(*status == 1)
    {
        buttons_overall_status |= BUTTON_OPEN;
    }
    else
    {
        buttons_overall_status ^= BUTTON_OPEN;
    }
}

/**
 * @brief loopback pin callback function
 */
void pin_loopback_cb(uint8_t *status)
{
    if(*status == 1)
    {
        buttons_overall_status |= PIN_LOOPBACK;
    }
    else
    {
        buttons_overall_status ^= PIN_LOOPBACK;
    }
}

#pragma endregion buttons_callback

/**
 * @brief Main callback function
 */
void main_callback(cbroker_cmd_id_e                    cmd_id,
                   const cbroker_request_data_t *const payload,
                   cbroker_response_data_t *const      output)
{
    switch(cmd_id)
    {
        case DISP_READ_KEYS:
        {
            output->read_keys = buttons_overall_status;
            APP_PRINTF("App - READ_KEYS[0x%.2X]\r\n", output->read_keys);
        }
        break;
        case DISP_WRITE_LINE:
        {
            /*
             Note: The data in "payload" will be valid until the Command Broker
                   circular buffer is full, which means that you can consume the
                   data directly from the payload address without creating a copy.

                   For now, the circular buffer is very small to save memory but
                   can be increased if needed.
             */
            lcd_put_line(payload->write_line.data, sizeof(payload->write_line.data), payload->write_line.line, 0);
            APP_PRINTF("App - WRITE_LINE[Line:0x%.2X, [%s]]\r\n", payload->write_line.line, payload->write_line.data);
        }
        break;
        case DISP_SET_BGLIGHT:
        {
            APP_PRINTF("App - SET_BGLIGHT[0x%.2X]\r\n", payload->set_bglight);
            if(CB_SET_BGLIGHT_DATA_ON == payload->set_bglight)
            {
                lcd_gpio_backlight_on();
            }
            else if(CB_SET_BGLIGHT_DATA_OFF == payload->set_bglight)
            {
                lcd_gpio_backlight_off();
            }
        }
        break;
        case DISP_CLEAR:
        {
            APP_PRINTF("App - CLEAR\r\n");
        }
        break;
        case DISP_SET_LANGUAGE:
        {
            APP_PRINTF("App - SET_LANGUAGE[0x%.2X]\r\n", payload->set_language);
        }
        break;
        case DISP_GET_VERSION:
        {
            output->version = settings.version;
            APP_PRINTF("App - GET_VERSION[0x%.2X]\r\n", output->version);
        }
        break;
        case DISP_BUZZER_PARAM:
        {
            APP_PRINTF("App - BUZZER_PARAM[type=0x%.2X, value=0x%.2X]\r\n", payload->buz_param.type,
                       payload->buz_param.value);
            if(CB_BUZ_PARAM_DATA0_FREQ == payload->buz_param.type)
            {
                beeper.cfg.freq = (payload->buz_param.value.freq * CB_BUZ_PARAM_DATA1_FREQ_STEP);
                beeper.set_frequency(&beeper);
            }
            else if(CB_BUZ_PARAM_DATA0_DUTY_CYCLE)
            {
                beeper.cfg.percent = payload->buz_param.value.duty_cycle;
                beeper.set_percent(&beeper);
            }
            else
            {
                // Nothing to do.
            }
        }
        break;
        case DISP_BUZZER_CTRL:
        {
            APP_PRINTF("App - BUZZER_CTR[data0=0x%.2X, time_on=0x%.2X, time_off=0x%.2X]\r\n", payload->buz_ctrl.data0,
                       payload->buz_ctrl.beeper_on, payload->buz_ctrl.beeper_off);

            if(CB_BUZ_CTRL_DATA0_ACTION_OFF == (CB_BUZ_CTRL_DATA0_ACTION_BITS_MASK & payload->buz_ctrl.data0.action))
            {
                beeper.off(&beeper);
            }
            else if(CB_BUZ_CTRL_DATA0_ACTION_ON ==
                    (CB_BUZ_CTRL_DATA0_ACTION_BITS_MASK & payload->buz_ctrl.data0.action))
            {
                beeper.on(&beeper);
            }
            else if(CB_BUZ_CTRL_DATA0_ACTION_BEEP ==
                    (CB_BUZ_CTRL_DATA0_ACTION_BITS_MASK & payload->buz_ctrl.data0.action))
            {
                beeper.cfg.num_of_cycles = (CB_BUZ_CTRL_DATA0_CYCLES_BITS_MASK & payload->buz_ctrl.data0.cycles);
                beeper.cfg.time_on       = payload->buz_ctrl.beeper_on * CB_BUZ_CTRL_DATA1_BEEPER_ON_STEP;
                beeper.cfg.time_off      = payload->buz_ctrl.beeper_off * CB_BUZ_CTRL_DATA2_BEEPER_OFF_STEP;
                beeper.cyclic_beep(&beeper);
            }
            else
            {
                // Nothing to do.
            }
        }
        break;

        default:
            break;
    }
}

/**
 * @brief Initialize settings function
 */
void init_settings()
{
    settings.main_loop_delay = MAIN_LOOP_DELAY;                                 // main loop delay in ms

    // Major Version = 5 Msb bits
    settings.version = ((uint16_t)((VERSION_MAJOR << VERSION_MAJOR_SHIFT)) & \
                                                     VERSION_MAJOR_MASK);
    // Minor Ver = Next 5 bits
    settings.version |= (((uint16_t)(VERSION_MINOR << VERSION_MINOR_SHIFT)) & \
                                                      VERSION_MINOR_MASK);
    // Build Ver = 6 Lsb bits
    settings.version |= (((uint16_t)(VERSION_BUILD & VERSION_BUILD_MASK)));
}

/**
 * @brief Initialize application function
 */
void app_init(void)
{
    //watchdog_init();

    init_settings();

    debug_uart_init(&debug_uart);
    debug_log_init(&debug_uart);
    APP_PRINTF("App - Debug log initialized\r\n");

    led_d10_on();
    lcd_spi_init(&spi_port);
    lcd_init(&spi_port, lcd_layout_full_height);
    lcd_backlight_on();

    button_init(button_open_cb, button_close_cb, button_stop_cb, pin_loopback_cb, BUTTONS_DEBOUNCER_DELAY);

    buzzer_init(&beeper.pwm);
    beeper_init_handle(&beeper);

    beeper.init(&beeper);
    beeper.set_frequency(&beeper);
    beeper.set_percent(&beeper);

    powered_uart_init(&powered_uart);
    cbroker_init(&powered_uart, main_callback);

}

/**
 * @brief App ticking function
 */
void app_process_action(void)
{

    uint8_t open_status = 0;
    uint8_t close_status = 0;
    uint8_t stop_status = 0;

    button_poll_all();

    if(!button_open(&open_status))
    {
        // Works
        open_status?cycle_to_next_qr = 1:0;
    }
    else{
    }


    if(!button_close(&close_status))
    {
        // Works
    }
    else{
    }

    if(!button_stop(&stop_status))
    {
        // Works
    }
    else{
    }

    if(cycle_to_next_qr == 1)
    {
        cycle_qr();
    }

    lcd_clear();
    lcd_put_qr_code(qr_version_to_display, 0, 0, 0, 0);
    while(!lcd_update(1))
    {

    }
}
