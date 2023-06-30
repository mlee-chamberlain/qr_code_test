/** @file gpio_lcd.c
 *
 * @brief GPIO Interfaces module
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

#include "sl_simple_led_instances.h"

void led_d10_on(void)
{
    sl_led_turn_on(&sl_led_led_d10);
}

void led_d10_off(void)
{
    sl_led_turn_off(&sl_led_led_d10);
}
