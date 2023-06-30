/** @file base_pwm_driver.h
 *
 * @brief Base pwm driver interface for pwm instances.
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

#ifndef BASE_PWM_DRIVER
#define BASE_PWM_DRIVER

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief  Structure represents base pwm interface driver interface.
 *
 * @
 */
typedef struct base_pwm_driver
{
    uint8_t (*init)(void *self, uint32_t *freq);   ///< Pointer to init/enable the PWM Instances
    uint8_t (*set_duty)(void *self, uint8_t duty); ///< Variable to set_duty function
    void (*start)(void *self);                     ///< Pointer to Start function
    void (*stop)(void *self);                      ///< Pointer to Stop function
    void *handler;                                 ///< Pointer to PWM Handler

} base_pwm_driver_t;

#ifdef __cplusplus
}
#endif

#endif // BASE_PWM_DRIVER
