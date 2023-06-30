/** @file lcd_spi.h
 *
 * @brief SPI Interfaces library
 * See: https://docs.silabs.com/gecko-platform/4.0/driver/api/group-spidrv
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

#ifndef DRIVER_WRAPPERS_INC_LCD_SPI_H_
#define DRIVER_WRAPPERS_INC_LCD_SPI_H_

#include "base_sercomm_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief  SPI init.
 *
 * @param[in/out] dev - Pointer to base_serial comm driver structure.
 */
void lcd_spi_init(struct base_sercomm_driver *dev);

#ifdef __cplusplus
}
#endif

#endif /* HAL_INC_DISPLAY_SPI_H_ */
