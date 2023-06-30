/** @file gpio_led.h
 *
 * @brief D10 GPIO Interfaces library
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

#ifndef HAL_LED_D10_H_
#define HAL_LED_D10_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Simple D10 Led drv turn on function
 */
void led_d10_on(void);

/**
 * @brief Simple D10 Led drv turn off function
 */
void led_d10_off(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_LED_D10_H_ */
