/** @file beeper.h
 *
 * @brief Beeper Hardware Abstraction Layer.
 *
 * @author Rene Delgado - rene.delgado@chamberlain.com
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

#ifndef HAL_INC_BEEPER_H_
#define HAL_INC_BEEPER_H_

#include <em_timer.h>
#include <em_cmu.h>

#include "base_pwm_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

// Frequency for 1ms without prescale.
#define BEEPER_TIMER_FREQUENCY_DEFAULT (0x3DC)

/**
 * @brief Beeper timer.
 */
typedef struct beeper_timer
{
    TIMER_TypeDef *      self;
    TIMER_Init_TypeDef   init;
    TIMER_InitCC_TypeDef initCC;
    uint32_t             ch;
    uint32_t             freq;

} beeper_timer_t;

/**
 * @brief Beeper config data.
 */
typedef struct beeper_config
{
    uint32_t          freq;
    uint8_t           percent;
    volatile uint32_t num_of_cycles;
    volatile uint32_t time_on;
    volatile uint32_t time_off;
    beeper_timer_t    timer;

} beeper_config_t;

/**
 * @brief Beeper Handle
 */
typedef struct beeper_handle
{
    beeper_config_t   cfg;
    base_pwm_driver_t pwm;
    uint32_t (*init)(const void *const self);
    uint32_t (*set_frequency)(const void *const self);
    uint32_t (*set_percent)(const void *const self);
    uint32_t (*on)(const void *const self);
    uint32_t (*off)(const void *const self);
    void (*cyclic_beep)(const void *const self);

} beeper_t;

/**
 * @brief  Beeper Init.
 *
 * @param[in] handle
 */
uint32_t beeper_init_handle(beeper_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* HAL_INC_BEEPER_H_ */
