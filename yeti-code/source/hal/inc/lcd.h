/**
 * @file lcd_driver.h
 *
 * @brief LCD Display driver for 5 predefined lines
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

#ifndef HAL_LCD_DRIVER_
#define HAL_LCD_DRIVER_

#include "stdbool.h"

#include "base_sercomm_driver.h"
#include "lcd_font_4_22.h"

#define LCD_CHAR_RESERVED_FOR_LINE_NUMBER (1)
#define LCD_CHAR_NUM (20 - LCD_CHAR_RESERVED_FOR_LINE_NUMBER)
#define LCD_LINE_NUM (4)
#define LCD_LINE_PIXEL_HEIGHT (12) // LCD line width bits width

// QR Code specs
#define QR_CODE_NUM_COL (45)
#define QR_CODE_NUM_ROW (6)

typedef struct lcd_line
{
    size_t upper_indent;
    size_t height;
} lcd_line_t;

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Display Init function
 *
 * @param[in] sercomm_instance - pointer to base serial communication
 *                           driver model instance
 *
 */
void lcd_init(base_driver *sercomm_instance, const lcd_line_t *lcd_layout);

/**
 * @brief Display update function
 *
 * @param[in] lcd_status - Ported from old the project flag,
 *                         should be set to true - should be be removed
 *                         in near future
 *
 * @return true - If the screen was updated completely
 *         false - otherwise
 */
bool lcd_update(bool lcd_status);

/**
 * @brief Display set line function, filling specificed buffer line with
 *        provided array of ascii symbols
 *
 * @param[in] str - pointer to provided array of symbols
 *
 * @param[in] size - size of provided array of symbols
 *
 * @param[in] line - specific line to update number (0 - 4) is valid pointer to provided array of symbols
 *
 * @param[in] language - specific language to display text
 *
 * @return true - line were successfully updated
 *         false - otherwise
 */
bool lcd_put_line(const uint8_t *str, const size_t size, const uint8_t line, language_e language);

/**
 * @brief Display set big number function, filling all buffer lines with
 *        provided array of ascii symbols
 *
 * @param[in] num - value of big number digit
 *
 * @param[in] offset - offset of line buffer string
 *
 * @param[in] index - big number array index
 *
 * @param[in] contrast - contrast value
 *
 * @return true - buffer data was successfully updated
 *         false - otherwise
 */
//bool lcd_put_big_number(uint16_t num, uint8_t offset, uint8_t index, uint8_t contrast);

/**
 * @brief Display set raw data function, filling specificed buffer line with
 *        raw array data;
 *
 * @param[in] data - value of raw data
 *
 * @param[in] line - specific line to update number (0 - 4)
 *
 * @param[in] offset - offset of line buffer string
 *
 * @return true - line were successfully updated
 *         false - otherwise
 */
bool lcd_put_raw_data(uint8_t data, uint8_t line, uint8_t offset);

/**
 * @brief Display clear function
 *
 */
void lcd_clear();

/**
 * @brief Display turn on backlight function
 *
 */
void lcd_backlight_on();

/**
 * @brief Display turn off backlight function
 *
 *
 */
void lcd_backlight_off();

/**
 * @brief Display adjust contrast function
 *
 * @param[in] value - desired contrast value (0 - 255)
 *
 */
void lcd_adjust_contrast(uint8_t value);

/**
 * @brief Display turns all pixel on function
 *
 */
void lcd_all_pixels_on();

/**
 * @brief Display turns all pixel off function
 *
 */
void lcd_all_pixels_off();


bool lcd_put_qr_code(uint8_t qr_version_number, uint16_t num, uint8_t offset, uint8_t index, uint8_t contrast);


#ifdef __cplusplus
}
#endif

#endif
