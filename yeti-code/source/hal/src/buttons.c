/** @file buttons.c
 *
 * @brief Button GPIO source file
 *
 * @author Ivan Jauregui
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
#include "buttons.h"
#include "button_gpio.h"

base_gpio_drv open_button;
base_gpio_drv close_button;
base_gpio_drv stop_button;
base_gpio_drv loopback_pin;

void button_init(callback_button_t open_button_callback,
                 callback_button_t close_button_callback,
                 callback_button_t stop_button_callback,
                 callback_button_t loopback_pin_callback, uint32_t timeout)
{
    button_open_init(&open_button, open_button_callback);
    button_close_init(&close_button, close_button_callback);
    button_stop_init(&stop_button, stop_button_callback);
    button_loopback_init(&loopback_pin, loopback_pin_callback);
    button_debounce_set(timeout);
}

void button_poll_all(void)
{
    button_poll(open_button.handle);
    button_poll(close_button.handle);
    button_poll(stop_button.handle);
    button_poll(loopback_pin.handle);
}

uint8_t button_open(uint8_t *is_pressed)
{
    uint8_t retVal = 0;

    if(open_button.get(open_button.handle, is_pressed))
    {
        /* ERROR */
        retVal = 1;
    }

    return retVal;
}

uint8_t button_close(uint8_t *is_pressed)
{
    uint8_t retVal = 0;

    if(close_button.get(close_button.handle, is_pressed))
    {
        /* ERROR */
        retVal = 1;
    }

    return retVal;
}

uint8_t button_stop(uint8_t *is_pressed)
{
    uint8_t retVal = 0;

    if(stop_button.get(stop_button.handle, is_pressed))
    {
        /* ERROR */
        retVal = 1;
    }

    return retVal;
}

uint8_t loopback_state(uint8_t *is_broken)
{
    uint8_t retVal = 0;

    if(stop_button.get(stop_button.handle, is_broken))
    {
        /* ERROR */
        retVal = 1;
    }

    return retVal;
}
