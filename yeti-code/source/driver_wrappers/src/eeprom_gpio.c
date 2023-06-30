/** @file eeprom_gpio.c
 *
 * @brief EEPROM GPIO Interfaces module
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
#include "em_gpio.h"

/* Local Defines */
#define PG22_EEPROM_PIN   0
#define PG22_EEPROM_PORT  gpioPortC

void gpio_eeprom_enable(void)
{
    GPIO_PortOutSet(PG22_EEPROM_PORT, PG22_EEPROM_PIN);
}

void gpio_eeprom_disable(void)
{
    GPIO_PortOutClear(PG22_EEPROM_PORT, PG22_EEPROM_PIN);
}
