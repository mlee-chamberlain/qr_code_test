/** @file buzzer_pwm.h
 *
 * @brief Buzzer Interfaces module
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

#ifndef HAL_BUZZER_PWM_H
#define HAL_BUZZER_PWM_H

#include "base_pwm_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Buzzer Initialize function
 *
 * @param[in/out] dev - Pointer to base PWM device handler struct
 *                      interface driver to initialize
 */
void buzzer_init(base_pwm_driver_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* HAL_BUZZER_PWM_H */
