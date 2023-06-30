/** @file beeper.c
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
#include <stdio.h>

#include "base_pwm_driver.h"
#include "buzzer_pwm.h"
#include "debug_log.h"
#include "beeper.h"

//"beeper_p" is only for Timer's IRQ usage.
static beeper_t *beeper_p             = NULL;
static uint32_t  time_on_reset_value  = 0;
static uint32_t  time_off_reset_value = 0;

/**
 * @brief  Timer Init: Initialize the timer used by the module.
 *
 * @param [in] timer - Timer to use.
 * @return Zero for no error, otherwise error number.
 */
static uint32_t beeper_timer_int(beeper_timer_t *const timer)
{
    BEEPER_PRINTF("Beeper - %s(addr=0x%p)\r\n", __func__, timer->self);
    uint32_t retval = 0;

    if(NULL != timer)
    {
        CMU_Clock_TypeDef timer_clock = cmuClock_TIMER2;
        // Configure TIMER frequency
        uint32_t top = (CMU_ClockFreqGet(timer_clock) / ((timer->freq))) - 1U;
        // Enable TIMER2 IRQ.
        NVIC_EnableIRQ(TIMER2_IRQn);
        // Enable the clock.
        CMU_ClockEnable(timer_clock, true);
        // Initialize the TIMER compare/capture channel.
        TIMER_InitCC(timer->self, timer->ch, &timer->initCC);
        // Set the top value for timer.
        TIMER_TopSet(timer->self, top);
        // Set the compare value for compare/capture channel.
        TIMER_CompareSet(timer->self, timer->ch, 0U);
        // Initialize timer init struct.
        TIMER_Init_TypeDef timer_init = TIMER_INIT_DEFAULT;
        // Do not start counting when initialization completed
        timer_init.enable = false;
        // Overflow Interrupt Enable.
        timer->self->IEN_SET = TIMER_IEN_OF;
        // Setting up the prescale.
        timer->init.prescale = timerPrescale1;
        // Initialize TIMER with the new parameters.
        TIMER_Init(timer->self, &timer_init);
        // Clear one or more pending TIMER interrupts.
        TIMER_IntClear(timer->self, true);
    }
    else
    {
        retval = 1;
    }

    return retval;
}

/**
 * @brief  Initialize the beeper
 *
 * @param [in] self->pwm.handler - the PWM handler.
 * @param [in] self->cfg.freq - the PWM freq.
 * @param [in] self->cfg.timer - the timer to use.
 * @return Zero for no error, otherwise error number.
 */
static uint32_t beeper_init(beeper_t *const self)
{
    BEEPER_PRINTF("Beeper - %s(freq=%d)\r\n", __func__, self->cfg.freq);
    uint32_t retval = 0;

    // Initialization of beeper pointer only for Timer's IRQ usage.
    beeper_p = self;

    if(NULL != self->pwm.init)
    {
        retval = self->pwm.init(self->pwm.handler, &self->cfg.freq);
        retval += beeper_timer_int(&self->cfg.timer);
    }
    else
    {
        retval = 1;
    }

    return retval;
}

/**
 * @brief  Set the percent or duty cycle of the PWM.
 *
 * @param [in] self->cfg.percent - percent or duty cycle of the PWM.
 * @return Zero for no error, otherwise error number.
 */
static uint32_t beeper_set_percent(const beeper_t *const self)
{
    BEEPER_PRINTF("Beeper - %s(percent=%d)\r\n", __func__, self->cfg.percent);
    uint32_t retval = 0;

    if(NULL != self->pwm.handler)
    {
        retval = self->pwm.set_duty(self->pwm.handler, self->cfg.percent);
    }
    else
    {
        retval = 1;
    }

    return retval;
}

/**
 * @brief  Set the frequency of the PWM.
 *
 * @param [in] self->cfg.percent - frequency of the PWM.
 * @return Zero for no error, otherwise error number.
 */
static uint32_t beeper_set_frequency(beeper_t *const self)
{
    BEEPER_PRINTF("Beeper - %s(freq=%d)\r\n", __func__, self->cfg.freq);
    uint32_t retval = 0;

    if(NULL != self->pwm.init)
    {
        retval = self->pwm.init(self->pwm.handler, &self->cfg.freq);
    }
    else
    {
        retval = 1;
    }

    return retval;
}

/**
 * @brief  Turn ON the beeper.
 *
 * @param [in] self->pwm.handler - PWM to use.
 */
static uint32_t beeper_on(const beeper_t *const self)
{
    BEEPER_PRINTF("Beeper - %s()\r\n", __func__);
    uint32_t retval = 0;

    if(NULL != self->pwm.start)
    {
        self->pwm.start(self->pwm.handler);
    }
    else
    {
        retval = 1;
    }

    return retval;
}

/**
 * @brief  Beeper silence is the silent part of a beep cycle.
 * Used internally for silencing the beep while in a beep cycle sequence.
 *
 * @param [in] self->pwm.handler - PWM to silence.
 * @return Zero for no error, otherwise error number.
 */
static uint32_t beeper_silence(const beeper_t *const self)
{
    BEEPER_PRINTF("Beeper - %s()\r\n", __func__);
    uint32_t retval = 0;

    if(NULL != self->pwm.stop)
    {
        self->pwm.stop(self->pwm.handler);
    }
    else
    {
        retval = 1;
    }

    return retval;
}

/**
 * @brief  Beeper OFF: It allows us to turn off the beeper ON and the Cyclic Beep (It cancels the remaining beeps).
 *
 * @param [in] self->pwm.handler - PWM to turn off.
 * @return Zero for no error, otherwise error number.
 */
static uint32_t beeper_off(const beeper_t *const self)
{
    BEEPER_PRINTF("Beeper - %s()\r\n", __func__);
    uint32_t retval = 0;

    if(NULL != self->pwm.stop)
    {
        self->pwm.stop(self->pwm.handler);
        beeper_p->cfg.num_of_cycles = 0;
        // Clear one or more pending TIMER interrupts.
        TIMER_IntClear(self->cfg.timer.self, true);
        // Stop TIMER.
        TIMER_Enable(self->cfg.timer.self, false);
    }
    else
    {
        retval = 1;
    }

    return retval;
}

/**
 * @brief  Cyclic Beep
 *
 * @param [in] self->cfg.time_on - Time ON for each beep.
 * @param [in] self->cfg.time_off - Time OFF for each beep.
 * @param [in] self->cfg.num_of_cycles - Number of beeps.
 * @return Zero for no error, otherwise error number.
 */
static void beeper_cyclic_beep(const beeper_t *const self)
{
    BEEPER_PRINTF("Beeper - %s(cycles=0x%.2X time_on=%dms, time_off=%dms)\r\n", __func__, self->cfg.num_of_cycles,
                  self->cfg.time_on, self->cfg.time_off);

    // Backing up the time_on to restart it in the Timer's IRQ  every cycle.
    time_on_reset_value = self->cfg.time_on;
    // Backing up the time_off to restart it in the Timer's IRQ every cycle.
    time_off_reset_value = self->cfg.time_off;

    if(self->cfg.num_of_cycles)
    {
        if(time_on_reset_value)
        {
            beeper_on(self);
        }
        else if(time_off_reset_value)
        {
            beeper_off(self);
        }
        // Clear one or more pending TIMER interrupts.
        TIMER_IntClear(self->cfg.timer.self, true);
        // Start TIMER.
        TIMER_Enable(self->cfg.timer.self, true);
    }
}

/**
 * @brief  Beeper init handle initialize the beeper handle.
 *
 * @param [in/out] handle - handle to init.
 * @return Zero for no error, otherwise error number.
 */
uint32_t beeper_init_handle(beeper_t *const handle)
{
    BEEPER_PRINTF("Beeper - %s(addr=0x%p)\r\n", __func__, handle);
    uint32_t retval = 0;

    if(NULL != handle->pwm.init)
    {
        handle->init          = (void *)beeper_init;
        handle->set_frequency = (void *)beeper_set_frequency;
        handle->set_percent   = (void *)beeper_set_percent;
        handle->on            = (void *)beeper_on;
        handle->off           = (void *)beeper_off;
        handle->cyclic_beep   = (void *)beeper_cyclic_beep;
    }
    else
    {
        retval = 1;
    }

    return retval;
}

void TIMER2_IRQHandler(void)
{
    if(beeper_p->cfg.num_of_cycles)
    {
        if(beeper_p->cfg.time_on)
        {
            beeper_p->cfg.time_on--;
            if(!beeper_p->cfg.time_on)
            {
                if(time_off_reset_value)
                {
                    beeper_silence(beeper_p);
                    beeper_p->cfg.time_off = time_off_reset_value;
                }
                else
                {
                    beeper_p->cfg.num_of_cycles--;
                    beeper_p->cfg.time_on = time_on_reset_value;
                    if(beeper_p->cfg.num_of_cycles)
                    {
                        beeper_p->on(beeper_p);
                    }
                    else
                    {
                        beeper_silence(beeper_p);
                    }
                }
            }
        }
        else if(beeper_p->cfg.time_off)
        {
            beeper_p->cfg.time_off--;
            if(!beeper_p->cfg.time_off)
            {
                beeper_p->cfg.num_of_cycles--;
                beeper_p->cfg.time_off = time_off_reset_value;
                if(beeper_p->cfg.num_of_cycles)
                {
                    if(time_on_reset_value)
                    {
                        beeper_p->on(beeper_p);
                        beeper_p->cfg.time_on = time_on_reset_value;
                    }
                    else
                    {
                        beeper_silence(beeper_p);
                    }
                }
            }
        }

        if(!beeper_p->cfg.num_of_cycles)
        {
            // Stop TIMER.
            TIMER_Enable(beeper_p->cfg.timer.self, false);
        }
    }
    // Clear one or more pending TIMER interrupts.
    TIMER_IntClear(beeper_p->cfg.timer.self, true);
}
