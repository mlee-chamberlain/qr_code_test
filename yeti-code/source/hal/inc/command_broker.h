/** @file command_broker.h
 *
 * @brief The Command Broker handles request and response commands from/to the main
 * board through the specified serial communication driver (Powered UART by default).
 *
 * See: 190A2895.docx - 11/30/22 - RD - Adding format table and updating file number.
 *
 * Direct link: https://wiki.chamberlain.com/display/YETI/Display+Comms
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

#ifndef HAL_INC_COMMAND_BROKER_H_
#define HAL_INC_COMMAND_BROKER_H_

#include <stdint.h>
#include "base_sercomm_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif

/********************************************* COMMON PROTOCOL MACROS *************************************************/

#define CB_CMD_ID_STATUS_BITS_POS (6)
#define CB_CMD_ID_STATUS_BITS_MASK (0xC0)
#define CB_CMD_ID_BITS_MASK (~CB_CMD_ID_STATUS_BITS_MASK)

#define CB_BUZ_CTRL_DATA0_ACTION_BITS_POS (6)
#define CB_BUZ_CTRL_DATA0_ACTION_BITS_MASK (0xC0)
#define CB_BUZ_CTRL_DATA0_CYCLES_BITS_MASK (~CB_BUZ_CTRL_DATA0_ACTION_BITS_MASK)

#define CB_BYTES_IN_WRITE_LINE_DATA (20) ///< Write line request command max binary data size.

#define CB_BUZ_PARAM_DATA1_FREQ_STEP (100)      ///< Step: 100 Hz. Buzzer Param data1 frequency step.
#define CB_BUZ_CTRL_DATA1_BEEPER_ON_STEP (128)  ///< Beeper ON in 128 milliseconds counts.
#define CB_BUZ_CTRL_DATA2_BEEPER_OFF_STEP (128) ///< Beeper OFF in 128 milliseconds counts.

/********************************************* COMMON PROTOCOL ENUMS *************************************************/

/**
 * @brief Command ID status bits.
 */
typedef enum
{
    CB_CMD_ID_STATUS_BIT_NO_ERR = ((0x01) << CB_CMD_ID_STATUS_BITS_POS), ///< No error flag.
    CB_CMD_ID_STATUS_BIT_ERR    = ((0x02) << CB_CMD_ID_STATUS_BITS_POS)  ///< Error flag.

} cbroker_cmd_id_status_bits_e;

/**
 * @brief Representation of the binary value of command ID.
 */
typedef enum
{
    DISP_CMD_ID_UNUSED = 0x00, ///< Unused.
    DISP_READ_KEYS     = 0x01, ///< Request input status.
    DISP_WRITE_LINE,           ///< Write to a line in the display.
    DISP_SET_BGLIGHT,          ///< Set backlight state.
    DISP_CLEAR,                ///< Clear the display lines.
    DISP_SET_LANGUAGE,         ///< Selects the current language and loads the character set used by that language.
    DISP_GET_VERSION,          ///< Returns software version of the display board.
    DISP_BUZZER_PARAM,         ///< Buz parameters.
    DISP_BUZZER_CTRL,          ///< Buz control message OFF/ON/BEEP.
    DISP_CMD_ID_MAX,           ///< Enum length.
} cbroker_cmd_id_e;

/**
 * @brief Representation of the binary value of read keys data.
 */
typedef enum
{
    CB_READ_KEYS_DATA_LED_OFF = 0x00, ///< External LED OFF.
    CB_READ_KEYS_DATA_LED_ON,         ///< External LED ON.
    CB_READ_KEYS_DATA_MAX,            ///< Enum length.
} cbroker_read_keys_data_e;

/**
 * @brief Representation of the binary value of write line data 0.
 * (Line 3 and Line 4 are NOT supported in Road Runner and VDrive)
 */
typedef enum
{
    CB_WRITE_LINE_DATA0_LINE_1 = 0x00, ///< Line 1 of display.
    CB_WRITE_LINE_DATA0_LINE_2,        ///< Line 2 of display.
    CB_WRITE_LINE_DATA0_LINE_3,        ///< Line 3 of display.
    CB_WRITE_LINE_DATA0_LINE_4,        ///< Line 4 of display (reserved for icons).
    CB_WRITE_LINE_DATA0_MAX,           ///< Enum length.
} cbroker_write_line_data0_e;

/**
 * @brief Representation of the binary value of set backlight data.
 */
typedef enum
{
    CB_SET_BGLIGHT_DATA_OFF = 0x00, ///< Backlight OFF.
    CB_SET_BGLIGHT_DATA_ON,         ///< Backlight ON.
    CB_SET_BGLIGHT_DATA_MAX,        ///< Enum length.
} cbroker_set_bglight_data_e;

/**
 * @brief Representation of the binary value of set language data.
 */
typedef enum
{
    CB_SET_LANGUAGE_DATA_GERMAN = 0x00, ///< Language to GERMAN.
    CB_SET_LANGUAGE_DATA_DUTCH,         ///< Language to DUTCH.
    CB_SET_LANGUAGE_DATA_ENGLISH,       ///< Language to ENGLISH.
    CB_SET_LANGUAGE_DATA_ITALIAN,       ///< Language to ITALIAN.
    CB_SET_LANGUAGE_DATA_FRENCH,        ///< Language to FRENCH.
    CB_SET_LANGUAGE_DATA_SPANISH,       ///< Language to SPANISH.
    CB_SET_LANGUAGE_DATA_SWEDISH,       ///< Language to SWEDISH.
    CB_SET_LANGUAGE_DATA_MAX,           ///< Enum length.
} cbroker_set_language_data_e;

/**
 * @brief Representation of the binary value of buz parameters data1 for FREQ.
 */
typedef enum
{
    CB_BUZ_PARAM_DATA0_FREQ = 0, ///< Buz parameters data0 FREQ.
    CB_BUZ_PARAM_DATA0_DUTY_CYCLE,      ///< Buz parameters data0 DUTY_CYCLE.
    CB_BUZ_PARAM_DATA0_MAX       ///< Enum length.
} cbroker_buz_param_data0_e;

/**
 * @brief Representation of the binary value of buz parameters data1.
 * Step: 100 Hz.
 * Range: [0-200].
 * Value (DEC): 20 for 2 kHZ.
 */
typedef enum
{
    CB_BUZ_PARAM_DATA1_FREQ_LOWER_LIMIT = 0x00, ///< Buzzer parameter data1 lower limit.
    CB_BUZ_PARAM_DATA1_FREQ_UPPER_LIMIT = 0xC8, ///< Buzzer parameter data1 upper limit.
    CB_BUZ_PARAM_DATA1_FREQ_MAX         = CB_BUZ_PARAM_DATA1_FREQ_UPPER_LIMIT + 1 ///< Enum length.
} cbroker_buz_param_data1_freq_e;

/**
 * @brief Representation of the binary value of buz parameters data1.
 * Step: 1%.
 * Range: [0-100]
 * Value (DEC): 100 for 100%.
 */
typedef enum
{
    CB_BUZ_PARAM_DATA1_DUTY_CYCLE_LOWER_LIMIT = 0x00,                                  ///< Buzzer parameter data1 lower limit.
    CB_BUZ_PARAM_DATA1_DUTY_CYCLE_UPPER_LIMIT = 0x64,                                  ///< Buzzer parameter data1 upper limit.
    CB_BUZ_PARAM_DATA1_DUTY_CYCLE_MAX         = CB_BUZ_PARAM_DATA1_DUTY_CYCLE_UPPER_LIMIT + 1 ///< Enum length.
} cbroker_buz_param_data1_duty_cycle_e;

/**
 * @brief Representation of the binary value of buz control data 0: Action (2 bits) for DUTY_CYCLE.
 * Data 0:  Action (2 bits)
 *          0 – OFF
 *          1 – ON     // Continuous beep
 *          2 – Beep   // Cyclic beep
 *          3 – unused
 * Data 0:  Number of cycle (6 bits)
 *          Number of cycles: 1-63.
 */
typedef enum
{
    CB_BUZ_CTRL_DATA0_ACTION_OFF    = ((0x00) << CB_BUZ_CTRL_DATA0_ACTION_BITS_POS), ///< Action OFF flag.
    CB_BUZ_CTRL_DATA0_ACTION_ON     = ((0x01) << CB_BUZ_CTRL_DATA0_ACTION_BITS_POS), ///< Action ON flag.
    CB_BUZ_CTRL_DATA0_ACTION_BEEP   = ((0x02) << CB_BUZ_CTRL_DATA0_ACTION_BITS_POS), ///< Action Beep flag.
    CB_BUZ_CTRL_DATA0_ACTION_UNUSED = ((0x03) << CB_BUZ_CTRL_DATA0_ACTION_BITS_POS), ///< Unused.
} cbroker_buz_ctrl_data0_action_bits_e;

/**
 * @brief Representation of the binary value of buz control data 0: Action (2 bits) for DUTY_CYCLE.
 * Data 0:  Action (2 bits)
 *          0 – OFF
 *          1 – ON     // Continuous beep
 *          2 – Beep   // Cyclic beep
 *          3 – unused
 * Data 0:  Number of cycle (6 bits)
 *          Number of cycles: 1-63.
 */
typedef enum
{
    CB_BUZ_CTRL_DATA0_NUM_CYCLES_LOWER_LIMIT = 0x00, ///< Buzzer Control Data0 Lower limit.
    CB_BUZ_CTRL_DATA0_NUM_CYCLES_UPPER_LIMIT = 0x3f, ///< Buzzer Control Data0 Upper limit.
    CB_BUZ_CTRL_DATA0_NUM_CYCLES_MAX                 ///< Enum length.
} cbroker_buz_ctrl_data0_num_cycles_bits_e;

/**
 * @brief Beeper ON, data 1 value (in 128 milliseconds counts).
 * Range: 0 to 255 (0x01 to 0xFF)
 *        e.g.
 *        1 = 128 ms
 *        6 = 768 ms
 */
typedef enum
{
    CB_BUZ_CTRL_DATA1_BEEPER_ON_LOWER_LIMIT = 0x00, ///< Buzzer Control Data1 Lower limit.
    CB_BUZ_CTRL_DATA1_BEEPER_ON_UPPER_LIMIT = 0xFF, ///< Buzzer Control Data1 upper limit.
} cbroker_buz_ctrl_data1_e;

/**
 * @brief  Beeper OFF, data 2 value (in 128 milliseconds counts).
 * Range: 0 to 255 (0x01 to 0xFF)
 *        e.g.
 *        1 = 128 ms
 *        6 = 768 ms
 */
typedef enum
{
    CB_BUZ_CTRL_DATA2_BEEPER_OFF_LOWER_LIMIT = 0x01, ///< Buzzer Control Data2 Lower limit.
    CB_BUZ_CTRL_DATA2_BEEPER_OFF_UPPER_LIMIT = 0xFF, ///< Buzzer Control Data2 upper limit.
} cbroker_buz_ctrl_data2_e;

/*************************************** COMMON PROTOCOL STRUCTS AND UNIONS*******************************************/

/**
 * @brief Write line request command data format.
 */
typedef struct cbroker_write_line_data
{
    cbroker_write_line_data0_e line;
    uint8_t                    data[CB_BYTES_IN_WRITE_LINE_DATA - sizeof(cbroker_write_line_data0_e)];

} cbroker_write_line_data_t;

/**
 * @brief Format of buzzer param data1.
 */
typedef union cbroker_buz_param_data1
{
    uint8_t                        freq_and_duty_cycle;
    cbroker_buz_param_data1_freq_e freq;
    cbroker_buz_param_data1_duty_cycle_e  duty_cycle;
} cbroker_buz_param_data1_t;

/**
 * @brief Format of buzzer param data0 and data1.
 */
typedef struct cbroker_buz_param_data
{
    cbroker_buz_param_data0_e type;
    cbroker_buz_param_data1_t value;
} cbroker_buz_param_data_t;

/**
 * @brief Format of buzzer ctrl data0.
 */
typedef union cbroker_buz_ctrl_data0
{
    cbroker_buz_ctrl_data0_action_bits_e     action;
    cbroker_buz_ctrl_data0_num_cycles_bits_e cycles;
} cbroker_buz_ctrl_data0_t;

/**
 * @brief Format buzzer ctrl data0, data1 and data2.
 */
typedef struct cbroker_buz_ctrl_data
{
    cbroker_buz_ctrl_data0_t data0;
    cbroker_buz_ctrl_data1_e beeper_on;
    cbroker_buz_ctrl_data2_e beeper_off;

} cbroker_buz_ctrl_data_t;

/**
 * @brief Representation of the binary value of command id.
 */
typedef union cbroker_cmd_id_format
{
    uint8_t                      id_with_status;
    cbroker_cmd_id_status_bits_e status;
    cbroker_cmd_id_e             id;
} cbroker_cmd_id_format_t;

/**
 * @brief Reserved bytes for each request command's data.
 */
typedef union cbroker_request_data
{
    cbroker_read_keys_data_e   read_keys;   ///< Read keys command max binary data size.
    cbroker_write_line_data_t  write_line;  ///< Write line command max binary data size.
    cbroker_set_bglight_data_e set_bglight; ///< Set backlight command max binary data size.
    // uint8_t                     clear;            ///< There is no data expected for clear command.
    cbroker_set_language_data_e set_language; ///< Set language command max binary data size.
    // uint8_t                     get_version;      ///< There is no data expected for get version command.
    cbroker_buz_param_data_t buz_param;                 ///< Buzzer param command max binary data size.
    cbroker_buz_ctrl_data_t  buz_ctrl;                  ///< Buzzer ctrl command max binary data size.
    uint8_t raw[sizeof(cbroker_write_line_data_t) + 1]; ///< Generic addressing of the largest union´s element.
} cbroker_request_data_t;

/**
 * @brief Reserved bytes for each response command's data.
 */
typedef union cbroker_response_data
{
    uint8_t  read_keys;             ///< Read keys command max binary data size.
    uint16_t version;               ///< Version max binary data size.
    uint8_t  raw[sizeof(uint16_t)]; ///< Generic addressing of the largest union´s element.
} cbroker_response_data_t;

/**
 * @brief  Command Broker request callback.
 *
 * @param [in] command_id - Command id without status bits.
 * @param [in] payload - Data payload from request command.
 * @param [out] output - Data payload for response command.
 */
typedef void (*cbroker_request_callback_t)(cbroker_cmd_id_e                    cmd_id,
                                           const cbroker_request_data_t *const payload,
                                           cbroker_response_data_t *const      output);

/**
 * @brief  Command Brocker init.
 *
 * @param[in] sercomm - Pointer to a structure represents, base serial communication driver.
 * @param[in] callback - Function pointer called in valid request commands.
 */
uint8_t cbroker_init(base_driver *sercomm, cbroker_request_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* HAL_INC_COMMAND_BROKER_H_ */
