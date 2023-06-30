/** @file watchdog.c
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

#include "em_wdog.h"
#include "em_cmu.h"
#include "em_chip.h"

//Please refer to these docs
//https://docs.silabs.com/gecko-platform/3.0/emlib/api/efr32xg22/group-wdog#ga3de9748141ca1e172ec40a77b8e86433

void watchdog_off(void)
{
  WDOGn_Enable(DEFAULT_WDOG, false);
}

//using default WDOG0 with 8 sec timeout
void watchdog_init(void)
{
  //WDOG0 clock enable
  CMU_ClockEnable(cmuClock_WDOG0, true);

  // Watchdog Initialize settings
  WDOG_Init_TypeDef wdogInit = WDOG_INIT_DEFAULT;
  CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_ULFRCO); /* ULFRCO as clock source */
  wdogInit.debugRun = false;      // Stop when mcu is halted by debugger to allow debugging
  wdogInit.em3Run = true;
  wdogInit.perSel = wdogPeriod_1k; // Timeout Formula = [ 2 ^(PERSEL+3) + 1] / F
                                   // PERSEL = Chosen Value above
                                   // F = Clock Source Freq. In this case WDOG Clck = 1 Khz. Per cmuSelect_ULFRCO
                                   // Source: EFM32PG22 Gecko Reference Manual: 26.3.1 Clock Source

  // Initializing watchdog with chosen settings
  WDOGn_Init(DEFAULT_WDOG, &wdogInit);
}


void watchdog_reset_processor(void)
{
  //Please refer to these docs
  //https://docs.silabs.com/gecko-platform/3.2/emlib/api/efm32xg22/group-chip#ga6f18b7b780fcf12fff2b4983f2821c7d
  CHIP_Reset();
}


void watchdog_feed(void)
{
  WDOGn_Feed(DEFAULT_WDOG);
}
