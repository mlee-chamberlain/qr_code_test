/***************************************************************************//**
 * @file
 * @brief Simple Button Driver Instances
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

#include "sl_simple_button.h"
#include "sl_simple_button_3pbs_close_config.h"
#include "sl_simple_button_3pbs_loop_config.h"
#include "sl_simple_button_3pbs_open_config.h"
#include "sl_simple_button_3pbs_stop_config.h"

sl_simple_button_context_t simple_3pbs_close_context = {
  .state = 0,
  .history = 0,
  .port = SL_SIMPLE_BUTTON_3PBS_CLOSE_PORT,
  .pin = SL_SIMPLE_BUTTON_3PBS_CLOSE_PIN,
  .mode = SL_SIMPLE_BUTTON_3PBS_CLOSE_MODE,
};

const sl_button_t sl_button_3pbs_close = {
  .context = &simple_3pbs_close_context,
  .init = sl_simple_button_init,
  .get_state = sl_simple_button_get_state,
  .poll = sl_simple_button_poll_step,
  .enable = sl_simple_button_enable,
  .disable = sl_simple_button_disable,
};
sl_simple_button_context_t simple_3pbs_loop_context = {
  .state = 0,
  .history = 0,
  .port = SL_SIMPLE_BUTTON_3PBS_LOOP_PORT,
  .pin = SL_SIMPLE_BUTTON_3PBS_LOOP_PIN,
  .mode = SL_SIMPLE_BUTTON_3PBS_LOOP_MODE,
};

const sl_button_t sl_button_3pbs_loop = {
  .context = &simple_3pbs_loop_context,
  .init = sl_simple_button_init,
  .get_state = sl_simple_button_get_state,
  .poll = sl_simple_button_poll_step,
  .enable = sl_simple_button_enable,
  .disable = sl_simple_button_disable,
};
sl_simple_button_context_t simple_3pbs_open_context = {
  .state = 0,
  .history = 0,
  .port = SL_SIMPLE_BUTTON_3PBS_OPEN_PORT,
  .pin = SL_SIMPLE_BUTTON_3PBS_OPEN_PIN,
  .mode = SL_SIMPLE_BUTTON_3PBS_OPEN_MODE,
};

const sl_button_t sl_button_3pbs_open = {
  .context = &simple_3pbs_open_context,
  .init = sl_simple_button_init,
  .get_state = sl_simple_button_get_state,
  .poll = sl_simple_button_poll_step,
  .enable = sl_simple_button_enable,
  .disable = sl_simple_button_disable,
};
sl_simple_button_context_t simple_3pbs_stop_context = {
  .state = 0,
  .history = 0,
  .port = SL_SIMPLE_BUTTON_3PBS_STOP_PORT,
  .pin = SL_SIMPLE_BUTTON_3PBS_STOP_PIN,
  .mode = SL_SIMPLE_BUTTON_3PBS_STOP_MODE,
};

const sl_button_t sl_button_3pbs_stop = {
  .context = &simple_3pbs_stop_context,
  .init = sl_simple_button_init,
  .get_state = sl_simple_button_get_state,
  .poll = sl_simple_button_poll_step,
  .enable = sl_simple_button_enable,
  .disable = sl_simple_button_disable,
};

// the table of buttons and button count are generated as a
// convenience for the application
const sl_button_t *sl_simple_button_array[] = {
  &sl_button_3pbs_close, 
  &sl_button_3pbs_loop, 
  &sl_button_3pbs_open, 
  &sl_button_3pbs_stop
};
const uint8_t simple_button_count = 4;

void sl_simple_button_init_instances(void)
{
  sl_button_init(&sl_button_3pbs_close);
  sl_button_init(&sl_button_3pbs_loop);
  sl_button_init(&sl_button_3pbs_open);
  sl_button_init(&sl_button_3pbs_stop);
}

void sl_simple_button_poll_instances(void)
{
  sl_button_poll_step(&sl_button_3pbs_close);
  sl_button_poll_step(&sl_button_3pbs_loop);
  sl_button_poll_step(&sl_button_3pbs_open);
  sl_button_poll_step(&sl_button_3pbs_stop);
}
