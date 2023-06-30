/** @file button.h
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

#ifndef BUTTON_H_
#define BUTTON_H_

#include "stdint.h"

#include "base_gpio_driver.h"

/**
 * @brief Get Open Button status function.
 *
 * @param[out] is_pressed = 1 if Button is PRESSED
 *                          0 Otherwise
 *
 * @return 0 if no Error
 *         1 otherwise. Button could be disabled
 */
uint8_t button_open(uint8_t *is_pressed);

/**
 * @brief Get Close Button status function.
 *
 * @param[out] is_pressed = 1 if Button is PRESSED
 *                          0 Otherwise
 *
 * @return 0 if no Error
 *         1 otherwise. Button could be disabled
 */
uint8_t button_close(uint8_t *state);

/**
 * @brief Get Stop Button status function.
 *
 * @param[out] is_pressed = 1 if Button is PRESSED
 *                          0 Otherwise
 *
 * @return 0 if no Error
 *         1 otherwise. Button could be disabled
 */
uint8_t button_stop(uint8_t *state);

/**
 * @brief Get Loopback Pin status function.
 *
 * @param[out] is_broken = 1 if Button is PRESSED
 *                         0 Otherwise
 *
 * @return 0 if no Error
 *         1 otherwise. Loopback pin could be disabled
 */
uint8_t loopback_state(uint8_t *is_broken);

/**
 * @brief Poll all Buttons
 *
 * This function should be used to update
 * the buttons the state, and needs to be
 * called from a tick function or similar
 * by the user.
 */
void button_poll_all(void);

/**
 * @brief Initialize / Enable all Buttons & Loopback
 *
 * @param[in] open_button_callback callback function for open button
 *
 * @param[in] close_button_callback callback function for close button
 *
 * @param[in] stop_button_callback callback function for stop button
 *
 * @param[in] loopback_pin_callback callback function for loopback
 * 
 * @param[in] timeout debouncing delay timeout parameter
 *
 * This function should be used to enable
 * all Display Buttons and Loopback Pin
 */
void button_init(callback_button_t open_button_callback,
                 callback_button_t close_button_callback,
                 callback_button_t stop_button_callback,
                 callback_button_t loopback_pin_callback,
                 uint32_t          timeout);

#endif /* BUTTON_H_*/
