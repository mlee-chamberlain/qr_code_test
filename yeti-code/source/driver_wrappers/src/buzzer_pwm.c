/** @file buzzer_pwm.c
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
#include "sl_pwm_instances.h"
#include "sl_pwm_init_pwm_buzzer_config.h"
#include "base_pwm_driver.h"

/* PWM config structure */
static sl_pwm_config_t pwm_buzzer_cfg = {
    .frequency = 0,
    .polarity  = SL_PWM_PWM_BUZZER_POLARITY, // From project configurator
};

static uint8_t buzzer_pwm_init(void *self, uint32_t *freq)
{
    uint8_t result = 0;

    /* Save the desired frequency */
    pwm_buzzer_cfg.frequency = *freq;

    if(SL_STATUS_OK != sl_pwm_init((sl_pwm_instance_t *)self, &pwm_buzzer_cfg))
    {
        result = 1;
    }

    return result;
}

static void buzzer_pwm_start(void *self)
{
    sl_pwm_start((sl_pwm_instance_t *)self);
}

static void buzzer_pwm_stop(void *self)
{
    sl_pwm_stop((sl_pwm_instance_t *)self);
}

static uint8_t buzzer_pwm_set_dutycycle(void *self, uint8_t duty)
{
    uint8_t result   = 0;
    uint8_t readDuty = 0;

    sl_pwm_set_duty_cycle((sl_pwm_instance_t *)self, duty);

    readDuty = sl_pwm_get_duty_cycle((sl_pwm_instance_t *)self);

    if(!(duty == readDuty))
    {
        result = 1;
    }

    return result;
}

void buzzer_init(base_pwm_driver_t *dev)
{
    dev->init     = (void *)&buzzer_pwm_init;
    dev->start    = (void *)&buzzer_pwm_start;
    dev->stop     = (void *)&buzzer_pwm_stop;
    dev->set_duty = (void *)&buzzer_pwm_set_dutycycle;
    dev->handler  = (void *)&sl_pwm_pwm_buzzer;
}
