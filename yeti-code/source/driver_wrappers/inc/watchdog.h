/** @file watchdog.h
 *
 * @brief Watchdog Interfaces library
 *
 * @author Ilya Mushkirov
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

#ifndef WDG_WRAPPER_H_
#define WDG_WRAPPER_H_

#ifdef  __cplusplus
extern "C"
{
#endif


extern void watchdog_off(void);
extern void watchdog_init(void);
extern void watchdog_reset_processor(void);
extern void watchdog_reset_count(void);


#ifdef  __cplusplus
}
#endif

#endif /* WDG_WRAPPER_H_ */
