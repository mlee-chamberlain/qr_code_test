/** @file debug_log.h
 *
 * @brief Serial Debug Logger - (Non Blocking)
 *
 * @author Rene Delgado
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

#ifndef HAL_INC_DEBUG_LOG_H_
#define HAL_INC_DEBUG_LOG_H_
#include <stdio.h>
#include <stdbool.h>
#include "base_sercomm_driver.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define DEBUG_LOG_ENABLE (false)

#if DEBUG_LOG_ENABLE == true
#define APP_PRINTF_ENABLE (false)
#define CB_PRINTF_ENABLE (false)
#define BEEPER_PRINTF_ENABLE (false)
#define BUTTONS_PRINTF_ENABLE (false)
#define LCD_PRINTF_ENABLE (false)
#endif /* DEBUG_LOG_ENABLE */

uint8_t debug_log_init(base_driver *sercomm);
void    debug_log_print(const int8_t *format, ...);

#if DEBUG_LOG_ENABLE == true

#if APP_PRINTF_ENABLE == true
#define APP_PRINTF(fmt, ...) debug_log_print((fmt), ##__VA_ARGS__);
#else
#define APP_PRINTF(fmt, ...)
#endif /* APP_PRINTF_ENABLE */

#if CB_PRINTF_ENABLE == true
#define CB_PRINTF(fmt, ...) debug_log_print((fmt), ##__VA_ARGS__);
#else
#define CB_PRINTF(fmt, ...)
#endif /* CB_PRINTF_ENABLE */

#if BEEPER_PRINTF_ENABLE == true
#define BEEPER_PRINTF(fmt, ...) debug_log_print((fmt), ##__VA_ARGS__);
#else
#define BEEPER_PRINTF(fmt, ...)
#endif /* BEEPER_PRINTF_ENABLE */

#if BUTTONS_PRINTF_ENABLE == true
#define BTN_PRINTF(fmt, ...) debug_log_print((fmt), ##__VA_ARGS__);
#else
#define BTN_PRINTF(fmt, ...)
#endif /* BUTTONS_PRINTF_ENABLE */

#if LCD_PRINTF_ENABLE == true
#define LCD_PRINTF(fmt, ...) debug_log_print((fmt), ##__VA_ARGS__);
#else
#define LCD_PRINTF(fmt, ...)
#endif /* LCD_PRINTF_ENABLE */

#else

#define APP_PRINTF(fmt, ...)
#define CB_PRINTF(fmt, ...)
#define BEEPER_PRINTF(fmt, ...)
#define BUTTONS_PRINTF(fmt, ...)
#define LCD_PRINTF(fmt, ...)

#endif /* DEBUG_LOG_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* HAL_INC_DEBUG_LOG_H_ */
