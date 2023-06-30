/***************************************************************************//**
 * @file
 * @brief LED Driver Instances
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "sl_simple_led.h"
#include "em_gpio.h"
#include "sl_simple_led_lcd_backlight_config.h"
#include "sl_simple_led_lcd_reset_config.h"
#include "sl_simple_led_led_d10_config.h"

sl_simple_led_context_t simple_lcd_backlight_context = {
  .port = SL_SIMPLE_LED_LCD_BACKLIGHT_PORT,
  .pin = SL_SIMPLE_LED_LCD_BACKLIGHT_PIN,
  .polarity = SL_SIMPLE_LED_LCD_BACKLIGHT_POLARITY,
};

const sl_led_t sl_led_lcd_backlight = {
  .context = &simple_lcd_backlight_context,
  .init = sl_simple_led_init,
  .turn_on = sl_simple_led_turn_on,
  .turn_off = sl_simple_led_turn_off,
  .toggle = sl_simple_led_toggle,
  .get_state = sl_simple_led_get_state,
};
sl_simple_led_context_t simple_lcd_reset_context = {
  .port = SL_SIMPLE_LED_LCD_RESET_PORT,
  .pin = SL_SIMPLE_LED_LCD_RESET_PIN,
  .polarity = SL_SIMPLE_LED_LCD_RESET_POLARITY,
};

const sl_led_t sl_led_lcd_reset = {
  .context = &simple_lcd_reset_context,
  .init = sl_simple_led_init,
  .turn_on = sl_simple_led_turn_on,
  .turn_off = sl_simple_led_turn_off,
  .toggle = sl_simple_led_toggle,
  .get_state = sl_simple_led_get_state,
};
sl_simple_led_context_t simple_led_d10_context = {
  .port = SL_SIMPLE_LED_LED_D10_PORT,
  .pin = SL_SIMPLE_LED_LED_D10_PIN,
  .polarity = SL_SIMPLE_LED_LED_D10_POLARITY,
};

const sl_led_t sl_led_led_d10 = {
  .context = &simple_led_d10_context,
  .init = sl_simple_led_init,
  .turn_on = sl_simple_led_turn_on,
  .turn_off = sl_simple_led_turn_off,
  .toggle = sl_simple_led_toggle,
  .get_state = sl_simple_led_get_state,
};

const sl_led_t *sl_simple_led_array[] = {
  &sl_led_lcd_backlight,
  &sl_led_lcd_reset,
  &sl_led_led_d10
};

void sl_simple_led_init_instances(void)
{
  sl_led_init(&sl_led_lcd_backlight);
  sl_led_init(&sl_led_lcd_reset);
  sl_led_init(&sl_led_led_d10);
}
