/** @file button_gpio.c
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
#include "base_gpio_driver.h"
#include "sl_simple_button_instances.h"

#include "sl_sleeptimer.h"

/* Common gpio driver */
base_gpio_drv button_drv;

static callback_button_t _open_button_callback;
static callback_button_t _close_button_callback;
static callback_button_t _stop_button_callback;
static callback_button_t _loopback_pin_callback;

static sl_sleeptimer_timer_handle_t open_button_timer_handle;
static sl_sleeptimer_timer_handle_t close_button_timer_handle;
static sl_sleeptimer_timer_handle_t stop_button_timer_handle;

// Local timeout param
static uint32_t timeout = 0;

static void button_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
    if(&open_button_timer_handle == handle)
    {
        _open_button_callback((uint8_t *)data);
    }
    else if(&close_button_timer_handle == handle)
    {
        _close_button_callback((uint8_t *)data);
    }
    else if(&stop_button_timer_handle == handle)
    {
        _stop_button_callback((uint8_t *)data);
    }
};

void process_button_task(sl_sleeptimer_timer_handle_t *handle, sl_button_state_t *state)
{
    if(*state == SL_SIMPLE_BUTTON_RELEASED)
    {
        sl_sleeptimer_start_timer_ms(handle, timeout, button_timer_callback, state, 0, 0);
    }
    else
    {
        button_timer_callback(handle, state);
    }
}

uint8_t button_get(uint8_t *self, uint8_t *ptrState)
{
    bool retval = 0;

    sl_button_state_t state = sl_simple_button_get_state((const sl_button_t *)self);

    if(SL_SIMPLE_BUTTON_PRESSED == state)
    {
        *ptrState = 1; // Button Pressed
    }
    else if(SL_SIMPLE_BUTTON_RELEASED == state)
    {
        *ptrState = 0; // Button Released
    }
    else
    {
        /* Error, probably Button disabled */
        retval = 1;
    }

    return retval;
}

void button_open_init(struct base_gpio_driver *hdlr, callback_button_t button_callback)
{
    hdlr->get    = (void *)button_get;
    hdlr->handle = (void *)&sl_button_3pbs_open;
    hdlr->enable = (void *)sl_simple_button_enable;

    _open_button_callback = button_callback;
}

void button_close_init(struct base_gpio_driver *hdlr, callback_button_t button_callback)
{
    hdlr->get    = (void *)button_get;
    hdlr->handle = (void *)&sl_button_3pbs_close;
    hdlr->enable = (void *)sl_simple_button_enable;

    _close_button_callback = button_callback;
}

void button_stop_init(struct base_gpio_driver *hdlr, callback_button_t button_callback)
{
    hdlr->get    = (void *)button_get;
    hdlr->handle = (void *)&sl_button_3pbs_stop;
    hdlr->enable = (void *)sl_simple_button_enable;

    _stop_button_callback = button_callback;
}

void button_loopback_init(struct base_gpio_driver *hdlr, callback_button_t button_callback)
{
    hdlr->get    = (void *)button_get;
    hdlr->handle = (void *)&sl_button_3pbs_loop;
    hdlr->enable = (void *)sl_simple_button_enable;

    _loopback_pin_callback = button_callback;
}

void button_poll(void *self)
{
    // poll and debonuce
    sl_simple_button_poll_step((const sl_button_t *)self);
}

void button_debounce_set(uint32_t time)
{
    timeout = time;
}

void sl_button_on_change(const sl_button_t *handle)
{
    sl_button_state_t state = sl_simple_button_get_state(handle);

    // Button pressed.

    if(&sl_button_3pbs_open == handle)
    {
        if(_open_button_callback != 0)
        {
            process_button_task(&open_button_timer_handle, &state);
        }
    }
    else

        if(&sl_button_3pbs_close == handle)
    {
        if(_close_button_callback != 0)
        {
            process_button_task(&close_button_timer_handle, &state);
        }
    }
    else if(&sl_button_3pbs_stop == handle)
    {
        if(_stop_button_callback != 0)
        {
            process_button_task(&stop_button_timer_handle, &state);
        }
    }
    else if(&sl_button_3pbs_loop == handle)
    {
        if(_loopback_pin_callback != 0)
        {
            _loopback_pin_callback(&state);
        }
    }
}
