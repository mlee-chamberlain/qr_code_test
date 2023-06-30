/** @file button_gpio.h
 *
 * @brief Button GPIO Interfaces library
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
 * Software in a manner that would require any portion of the Licensed software
 * to be (i) disclosed or distributed in source code form; (ii) licensed for the
 * purpose of making derivative works; or (iii) distributable or redistributable
 * at no charge.
 */

#ifndef BUTTON_GPIO_H_
#define BUTTON_GPIO_H_

#include "base_gpio_driver.h"
#include <stdbool.h>

/**
 * @brief Button Open Init function
 */
void button_open_init(struct base_gpio_driver *hdlr, callback_button_t button_callback);

/**
 * @brief Button Close Init function
 */
void button_close_init(struct base_gpio_driver *hdlr, callback_button_t button_callback);

/**
 * @brief Button Stop Init function
 */
void button_stop_init(struct base_gpio_driver *hdlr, callback_button_t button_callback);

/**
 * @brief Button Loopback Init function
 */
void button_loopback_init(struct base_gpio_driver *hdlr, callback_button_t button_callback);

/**
 * @brief Poll all Buttons
 *
 * This function should be used to update
 * the buttons the state, and needs to be
 * called from a tick function or similar
 * by the user.
 */
void button_poll(void *self);

/**
 * @brief Button debouncer delay set function
 *
 * @param[in/out] time - debouncing delay definition parameter
 *                      
 */
void button_debounce_set(uint32_t time);

#endif /* BUTTON_GPIO_H_*/
