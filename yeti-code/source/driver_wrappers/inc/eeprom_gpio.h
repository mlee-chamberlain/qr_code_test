/** @file eeprom_gpio.h
 *
 * @brief EEPROM GPIO Interfaces library
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

#ifndef HAL_INC_EEPROM_GPIO_H_
#define HAL_INC_EEPROM_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief EEPROM Chip enable function
 */
void gpio_eeprom_enable(void);

/**
 * @brief EEPROM Chip disable function
 */
void gpio_eeprom_disable(void);


#ifdef __cplusplus
}
#endif

#endif /* HAL_INC_EEPROM_GPIO_H_ */
