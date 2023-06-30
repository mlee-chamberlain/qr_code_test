/** @file command_broker.c
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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "command_broker.h"
#include "debug_log.h"

/********************************************* COMMAND BROKER MACROS *************************************************/

#define CB_REQUEST_BUFF_SIZE (3)                 ///< Max Rx buffer size.
#define CB_BITS_IN_A_BYTE (8)                    ///< Bits in a byte.
#define CB_BITS_IN_A_NIBBLE (4)                  ///< Bits in a nibble.
#define CB_NIBBLES_IN_A_BYTE (2)                 ///< Nibbles in a byte.
#define CB_INVALID_ASCCIHEX_TO_BIN_NIBBLE (0xFF) ///< Invalid ASCII HEX to bin byte.

/********************************************* COMMON PROTOCOL MACROS *************************************************/

#define CB_FRAME_BYTE_STX ((uint8_t)(0x02))  ///< Start of Transmission Character.
#define CB_FRAME_BYTE_ETX ((uint8_t)(0x03))  ///< End of Transmission Character
#define CB_FRAME_BYTE_NULL ((uint8_t)(0x00)) ///< Null character sent by the display.

#define CB_BYTES_IN_FRAME (2)         ///< Expected frame bytes in binary format (<STX>, <ETX>).
#define CB_BYTES_IN_PACKET_NUMBER (1) ///< Packet number max binary data size.
#define CB_BYTES_IN_CMD_ID (1)        ///< Packet number max binary data size.
#define CB_BYTES_IN_CRC16_ARC (2)     ///< CRC16 ARC binary data size.
#define CB_BYTES_IN_FRAME (2)         ///< Expected frame bytes in binary format (<STX>, <ETX>).
#define CB_BYTES_IN_PACKET_NUMBER (1) ///< Packet number max binary data size.
#define CB_BYTES_IN_CMD_ID (1)        ///< Packet number max binary data size.
#define CB_BYTES_IN_CRC16_ARC (2)     ///< CRC16 ARC binary data size.

#define CB_RX_BYTES_IN_READ_KEYS_DATA (1) ///< Read keys request command max binary data size.
#define CB_RX_BYTES_IN_WRITE_LINE_DATA \
    (CB_BYTES_IN_WRITE_LINE_DATA)            ///< Write line request command max binary data size.
#define CB_RX_BYTES_IN_SET_BGLIGHT_DATA (1)  ///< Set backlight request command max binary data size.
#define CB_RX_BYTES_IN_CLEAR_DATA (0)        ///< Clear request command max binary data size.
#define CB_RX_BYTES_IN_SET_LANGUAGE_DATA (1) ///< Set language request command max binary data size.
#define CB_RX_BYTES_IN_GET_VERSION_DATA (0)  ///< Get version request command max binary data size.
#define CB_RX_BYTES_IN_BUZ_PARAM_DATA (2)    ///< Buz param request command max binary data size.
#define CB_RX_BYTES_IN_BUZ_CTRL_DATA (3)     ///< Buz ctrl request command max binary data size.

#define CB_TX_BYTES_IN_READ_KEYS_DATA (1)    ///< Read keys response command max binary data size.
#define CB_TX_BYTES_IN_WRITE_LINE_DATA (0)   ///< Write line response command max binary data size.
#define CB_TX_BYTES_IN_SET_BGLIGHT_DATA (0)  ///< Set backlight response command max binary data size.
#define CB_TX_BYTES_IN_CLEAR_DATA (0)        ///< Clear response max command binary data size.
#define CB_TX_BYTES_IN_SET_LANGUAGE_DATA (0) ///< Set language response command max binary data size.
#define CB_TX_BYTES_IN_GET_VERSION_DATA (2)  ///< Get version response command max binary data size.
#define CB_TX_BYTES_IN_BUZ_PARAM_DATA (2)    ///< Buz param response command max binary data size.
#define CB_TX_BYTES_IN_BUZ_CTRL_DATA (0)     ///< Buz ctrl response command max binary data size.
/********************************************** COMMAND BROKER ENUMS *************************************************/

/**
 * @brief Acknowledge states.
 */
typedef enum
{
    CB_ACK_NOT_READY = 0x00, ///< There is no data in the current request (RX) buffer to respond to the main board.
    CB_ACK_TO_BE_SEND,       ///< Enough data to create a response command to the main board.
    CB_ACK_SENDING,          ///< The TX callback (ISR) is consuming the response buffer.
    CB_ACK_SENT,             ///< The ETX byte is ready to be sent.
    CB_ACK_MAX,              ///<

} cbroker_ack_status_e;

/**
 * @brief States of request Command Broker finite state machine.
 * These states are called every time there is an rxByte on the sercomm.
 */
typedef enum
{
    CB_RX_IDLE_STATE,                   ///< Idle_state.
    CB_RX_VALIDATE_FRAME_STATE,         ///< Validate frame.
    CB_RX_VALIDATE_PACKET_NUMBER_STATE, ///< Validate packet number.
    CB_RX_VALIDATE_CMD_ID_STATE,        ///< Validate cmd id.
    CB_RX_VALIDATE_PAYLOAD_STATE,       ///< Validate payload.
    CB_RX_VALIDATE_CRC_STATE,           ///< Validate crc.
    CB_RX_MAX_STATE                     ///<
} cbroker_rx_system_state_e;

/***************************************  REQUEST (RX) STRUCTS AND UNIONS ******************************************/

/**
 * @brief Binary representation of request command.
 */
typedef struct cbroker_request_msg_format
{
    uint8_t                 stx;            ///< Start of Transmission Character.
    uint8_t                 packet_number;  ///< Packet Number (increments 00 to FF).
    cbroker_cmd_id_format_t cmd;            ///< Command ID.
    cbroker_request_data_t  data;           ///< Buffer for each request command's data.
    uint16_t                crc16_received; ///< CRC16 (over Packet Number to end of Data)
    uint8_t                 etx;            ///< End of Transmission Character.

} cbroker_request_msg_format_t;

/*************************************** COMMAND BROKER STRUCTS AND UNIONS ******************************************/

/**
 * @brief Data used by Command Broker to link the crc16 and ACK status for each request (Rx).
 */
typedef struct cbroker_rx_data
{
    uint16_t                     crc16_calc; ///< CRC16 (over Packet Number to end of Data).
    cbroker_request_msg_format_t buff;       ///< Binary representation of incoming request command.
    cbroker_ack_status_e         ack_status; ///< ACK status.
} cbroker_rx_data_t;

/**
 * @brief Data used by Command Broker to handle request (Rx) commands.
 */
typedef struct cbroker_request
{
    uint8_t                    index;      ///< Current request buffer index.
    cbroker_rx_system_state_e  next_state; ///< Next state for request state machine.
    uint8_t                    rxbyte;     ///< Rxbyte from serial communication driver.
    cbroker_request_callback_t callback;   ///<  Function pointer called in valid request commands.
    cbroker_rx_data_t          data[CB_REQUEST_BUFF_SIZE];
} cbroker_request_t;

/**
 * @brief Response states.
 */
typedef struct cbroker_tx_state_machine
{
    bool is_transmiting;

} cbroker_tx_state_machine_t;

/**
 * @brief Data used by Command Broker to handle response (Tx) commands.
 */
typedef struct cbroker_response
{
    uint8_t                    index;         ///< Current responding buffer index.
    cbroker_tx_state_machine_t state_machine; ///< Response states.
    uint8_t                    txByte;        ///< Txbyte for serial communication driver.
    cbroker_response_data_t    bin_data;      ///< Reserved bytes for each response command's data.
    uint16_t                   crc16_calc;    ///< CRC16 (over Packet Number to end of Data)
    // clang-format off
    uint8_t buff[
                           (CB_BYTES_IN_FRAME) +
                           (CB_NIBBLES_IN_A_BYTE * CB_BYTES_IN_PACKET_NUMBER) +
                           (CB_NIBBLES_IN_A_BYTE * CB_BYTES_IN_CMD_ID ) +
                           (CB_NIBBLES_IN_A_BYTE * CB_TX_BYTES_IN_BUZ_PARAM_DATA ) + /*Max response data size element.*/
                           (CB_NIBBLES_IN_A_BYTE * CB_BYTES_IN_CRC16_ARC) 
                        ] ;
} cbroker_response_t;

/**
 * @brief Pointer function for rx request event handler.
 */
typedef cbroker_rx_system_state_e (*cbroker_rx_system_event_handler)(const uint8_t * const rxByte);

/**
 * @brief Structure of state with event handler.
 */
typedef struct
{
    cbroker_rx_system_state_e       state; ///< States updated every time there is an rxByte on the sercomm.
    cbroker_rx_system_event_handler handler; ///< States called every time there is an rxByte on the sercomm.
} cbroker_rx_state_machine_t;

/**
 * @brief Main Command Broker Struct.
 */
typedef struct cbroker
{
    cbroker_request_t  request; ///< Data used by Command Broker to handle request (Rx) commands.
    cbroker_response_t response; ///< Data used by Command Broker to handle response (Tx) commands.
    base_driver        *sercomm; ///<Pointer to structure represents, base serial communication driver.
} cbroker_t;

/************************************************* LOCAL VARIABLES ***************************************************/

static cbroker_t cb = {.request.next_state = CB_RX_IDLE_STATE, .request.index = 0};

/************************************************* COMMON FUNCTIONS **************************************************/
static uint16_t cborker_calc_crc16arc(uint16_t crc, void const *mem, size_t len)
{
    uint8_t const *data = mem;
    if(data == NULL)
    {
        return 0;
    }
    for(size_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for(unsigned k = 0; k < 8; k++)
        {
            crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
        }
    }
    return crc;
}

/*********************************************** RESPONSE FUNCTIONS (TX) *********************************************/
uint8_t cbroker_tx_fill_buff(uint8_t *buff, const uint8_t index)
{
   /**
    * @brief Response command ID to data size table.
    */
    static const uint8_t response_cmd_id_to_data_size_table[DISP_CMD_ID_MAX] = {
    0, // The first command id starts at 0x01.
    CB_TX_BYTES_IN_READ_KEYS_DATA,
    CB_TX_BYTES_IN_WRITE_LINE_DATA,
    CB_TX_BYTES_IN_SET_BGLIGHT_DATA,
    CB_TX_BYTES_IN_CLEAR_DATA,
    CB_TX_BYTES_IN_SET_LANGUAGE_DATA,
    CB_TX_BYTES_IN_GET_VERSION_DATA,
    CB_TX_BYTES_IN_BUZ_PARAM_DATA,
    CB_TX_BYTES_IN_BUZ_CTRL_DATA,
    };
    /**
     * @brief Bin to ASCCIHEX table.
     */
    static const uint8_t bin_to_asciihex_tbl[0x0F + 1] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                                                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    // clang-format on

    uint8_t  cmd_id            = (CB_CMD_ID_BITS_MASK & cb.request.data[index].buff.cmd.id_with_status);
    uint8_t  status            = (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[index].buff.cmd.id_with_status);
    uint8_t  read_keys_data    = 0;
    uint16_t get_version_data  = 0;
    uint8_t  cmd_id_data_size  = 0;
    uint8_t  bytes_to_transmit = 0;
    uint8_t  offset            = 0;

    // STX - START TRANSMISSION
    buff[0] = CB_FRAME_BYTE_STX;
    // PACKET NUMBER
    buff[1] = bin_to_asciihex_tbl[((0xF0 & cb.request.data[index].buff.packet_number) >> CB_BITS_IN_A_NIBBLE)];
    buff[2] = bin_to_asciihex_tbl[0x0F & cb.request.data[index].buff.packet_number];

    // COMMAND ID
    buff[3] = bin_to_asciihex_tbl[((0xF0 & cb.request.data[index].buff.cmd.id_with_status) >> CB_BITS_IN_A_NIBBLE)];
    buff[4] = bin_to_asciihex_tbl[0x0F & cb.request.data[index].buff.cmd.id_with_status];

    // PAYLOAD response
    if(CB_CMD_ID_STATUS_BIT_NO_ERR == status)
    {
        cmd_id_data_size = response_cmd_id_to_data_size_table[cmd_id];
        if(DISP_READ_KEYS == cmd_id)
        {
            if(cb.request.callback)
            {
                cb.request.callback(cmd_id, &cb.request.data[index].buff.data, &cb.response.bin_data);
                read_keys_data = cb.response.bin_data.read_keys;
            }
            buff[5] = bin_to_asciihex_tbl[((0xF0 & read_keys_data) >> CB_BITS_IN_A_NIBBLE)];
            buff[6] = bin_to_asciihex_tbl[0x0F & read_keys_data];
        }
        else if(DISP_GET_VERSION == cmd_id)
        {
            if(cb.request.callback)
            {
                cb.request.callback(cmd_id, &cb.request.data[index].buff.data, &cb.response.bin_data);
                get_version_data = cb.response.bin_data.version;
            }
            buff[5] = bin_to_asciihex_tbl[((0xF000 & get_version_data) >> CB_BITS_IN_A_NIBBLE * 3)];
            buff[6] = bin_to_asciihex_tbl[((0x0F00 & get_version_data) >> CB_BITS_IN_A_NIBBLE * 2)];
            buff[7] = bin_to_asciihex_tbl[((0x00F0 & get_version_data) >> CB_BITS_IN_A_NIBBLE * 1)];
            buff[8] = bin_to_asciihex_tbl[((0x000F & get_version_data))];
        }
        else if(DISP_BUZZER_PARAM)
        {
            buff[5] =
                bin_to_asciihex_tbl[((0xF0 & cb.request.data[index].buff.data.buz_param.type) >> CB_BITS_IN_A_NIBBLE)];
            buff[6] = bin_to_asciihex_tbl[((0x0F & cb.request.data[index].buff.data.buz_param.type))];
            buff[7] = bin_to_asciihex_tbl[((0xF0 & cb.request.data[index].buff.data.buz_param.value.freq_and_duty_cycle) >>
                                           CB_BITS_IN_A_NIBBLE)];
            buff[8] = bin_to_asciihex_tbl[((0x0F & cb.request.data[index].buff.data.buz_param.value.freq_and_duty_cycle))];
        }
        else
        {
            /*Nothing to do.*/
        }
    }

    // CRC CALC
    cb.response.crc16_calc = 0;
    // clang-format off
    cb.response.crc16_calc =cborker_calc_crc16arc(cb.response.crc16_calc, &buff[1],
                                        (CB_NIBBLES_IN_A_BYTE * CB_BYTES_IN_PACKET_NUMBER) +
                                        (CB_NIBBLES_IN_A_BYTE * CB_BYTES_IN_CMD_ID) +
                                        (CB_NIBBLES_IN_A_BYTE * cmd_id_data_size) 
                                        );
    // clang-format on

    // CRC FILL
    offset             = CB_NIBBLES_IN_A_BYTE * cmd_id_data_size;
    buff[(5 + offset)] = bin_to_asciihex_tbl[((0xF000 & cb.response.crc16_calc) >> CB_BITS_IN_A_NIBBLE * 3)];
    buff[(6 + offset)] = bin_to_asciihex_tbl[((0x0F00 & cb.response.crc16_calc) >> CB_BITS_IN_A_NIBBLE * 2)];
    buff[(7 + offset)] = bin_to_asciihex_tbl[((0x00F0 & cb.response.crc16_calc) >> CB_BITS_IN_A_NIBBLE * 1)];
    buff[(8 + offset)] = bin_to_asciihex_tbl[((0x000F & cb.response.crc16_calc))];

    // ETX - END TRANSMISSION
    buff[(9 + offset)] = CB_FRAME_BYTE_ETX;

    // NULL - Requested by the protocol´s base document.
    buff[(10 + offset)] = CB_FRAME_BYTE_NULL;

    // clang-format off
   bytes_to_transmit = (CB_NIBBLES_IN_A_BYTE * CB_BYTES_IN_PACKET_NUMBER) +
                       (CB_NIBBLES_IN_A_BYTE * CB_BYTES_IN_CMD_ID ) +
                       (CB_NIBBLES_IN_A_BYTE * cmd_id_data_size ) +
                       (CB_NIBBLES_IN_A_BYTE * CB_BYTES_IN_CRC16_ARC);
    // clang-format on

    CB_PRINTF("CB - [Tx]: PN=0x%.4X, ST=0x%.2X, ID=0x%.2X, CRC=0x%.4X\r\n", cb.request.data[index].buff.packet_number,
              status, cmd_id, cb.response.crc16_calc);

    return bytes_to_transmit;
}

uint8_t cbroker_tx_set_next_byte(uint8_t *const txByte)
{
    static uint8_t transmited_bytes  = 0;
    uint8_t        bytes_to_transmit = 1;

    uint8_t next_index                       = 0;
    cb.response.state_machine.is_transmiting = true;

    // Verifying if the current Request command (Rx) is ready to Response (Tx).
    if(CB_ACK_NOT_READY == cb.request.data[cb.response.index].ack_status ||
       CB_ACK_SENT == cb.request.data[cb.response.index].ack_status)
    {
        next_index = cb.response.index;

        // If current response index is CB_ACK_NOT_READY or CB_ACK_SENT, verify the next index.
        if(CB_REQUEST_BUFF_SIZE - 1 > next_index)
        {
            next_index++;
        }
        else
        {
            next_index = 0;
        }
        // If next index is CB_ACK_TO_BE_SEND, update the current index.
        if(CB_ACK_TO_BE_SEND == cb.request.data[next_index].ack_status)
        {
            cb.response.index = next_index;
            transmited_bytes  = 0;
        }
        else
        {
            // There is no more bytes to transmit.
            bytes_to_transmit                        = 0;
            cb.response.state_machine.is_transmiting = false;
            return bytes_to_transmit;
        }
    }

    if(CB_ACK_TO_BE_SEND == cb.request.data[cb.response.index].ack_status)
    {
        transmited_bytes                              = 0;
        cb.request.data[cb.response.index].ack_status = CB_ACK_SENDING;
        (*txByte)                                     = CB_FRAME_BYTE_STX;
    }
    else if(CB_ACK_SENDING == cb.request.data[cb.response.index].ack_status)
    {
        if(1 == transmited_bytes)
        {
            // The buffer is filled after STX is sent, to spend time in the tx_callback instead of rx_callback.
            cbroker_tx_fill_buff(cb.response.buff, cb.response.index);
        }

        // Filling the txByte to be transmitted.
        (*txByte) = cb.response.buff[transmited_bytes];

        if(CB_FRAME_BYTE_NULL == (*txByte))
        {
            cb.request.data[cb.response.index].ack_status = CB_ACK_SENT;
        }
    }

    transmited_bytes++;
    return bytes_to_transmit;
}

uint8_t cbroker_tx_cb(uint8_t status, size_t size)
{
    (void)size;
    uint8_t bytes_to_transmit = 0;

    bytes_to_transmit = cbroker_tx_set_next_byte(&cb.response.txByte);
    if(bytes_to_transmit)
    {
        cb.sercomm->write_non_blocking(cb.sercomm->handle, &cb.response.txByte, 1, cbroker_tx_cb);
    }
    return status;
}

void cbroker_tx_send_response(void)
{
    uint8_t bytes_to_transmit;

    // If the response state machine is already transmitting this response will be sent in FIFO order.
    if(false == cb.response.state_machine.is_transmiting)
    {
        // Setting STX byte.
        bytes_to_transmit = cbroker_tx_set_next_byte(&cb.response.txByte);
        if(bytes_to_transmit)
        {
            // Transmit STX byte.
            cb.sercomm->write_non_blocking(cb.sercomm->handle, &cb.response.txByte, 1, cbroker_tx_cb);
        }
    }
}

/**********************************************  REQUEST FUNCTIONS (RX) **********************************************/
static void cbroker_rx_set_status_bit(cbroker_cmd_id_status_bits_e flag)
{
    if(CB_CMD_ID_STATUS_BIT_ERR == flag)
    {
        cb.request.data[cb.request.index].buff.cmd.status &= (~CB_CMD_ID_STATUS_BIT_NO_ERR);
        cb.request.data[cb.request.index].buff.cmd.status |= CB_CMD_ID_STATUS_BIT_ERR;
    }
    else
    {
        cb.request.data[cb.request.index].buff.cmd.status &= (~CB_CMD_ID_STATUS_BIT_ERR);
        cb.request.data[cb.request.index].buff.cmd.status |= CB_CMD_ID_STATUS_BIT_NO_ERR;
    }
}

static void cbroker_rx_asciihex_to_bin(uint8_t *const pRxByte)
{
    /**
     * @brief ASCIIHEX to bin table.
     */
    static const uint8_t cbroker_rx_asciihex_to_bin_table['F' + 1] = {
        [0 ... '0' - 1] = CB_INVALID_ASCCIHEX_TO_BIN_NIBBLE,
        0x00,
        0x01,
        0x02,
        0x03,
        0x04,
        0x05,
        0x06,
        0x07,
        0x08,
        0x09,
        ['9' + 1 ... 'A' - 1] = CB_INVALID_ASCCIHEX_TO_BIN_NIBBLE,
        0x0A,
        0x0B,
        0x0C,
        0x0D,
        0x0E,
        0x0F};

    // Edge validation.
    if('F' >= (*pRxByte))
    {
        (*pRxByte) = cbroker_rx_asciihex_to_bin_table[(*pRxByte)];
    }

    if(CB_INVALID_ASCCIHEX_TO_BIN_NIBBLE == (*pRxByte))
    {
        // Later, some functions reset their static variables based on this flag.
        CB_PRINTF("CB - [Rx]: PN=0x%.4X, ST=0x%.2X, ID=0x%.2X, Err=Asciihex to bin invalid (0x%.2X)\r\n",
                  cb.request.data[cb.request.index].buff.packet_number,
                  (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id_with_status),
                  (CB_CMD_ID_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id_with_status), (*pRxByte));
        cbroker_rx_set_status_bit(CB_CMD_ID_STATUS_BIT_ERR);
    }
}

static cbroker_rx_system_state_e cbroker_rx_validate_frame(const uint8_t *const pRxByte)
{
    static uint8_t               remaining_frame_bytes = CB_BYTES_IN_FRAME;
    cbroker_rx_system_state_e    next_state            = CB_RX_IDLE_STATE;
    cbroker_cmd_id_status_bits_e status_bits =
        (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.status);

    // If cbroker_rx_validate_frame() function/state is called when error flag is set, means that the state machine
    // should wait for STX byte (new requet).
    if(CB_CMD_ID_STATUS_BIT_ERR == status_bits)
    {
        remaining_frame_bytes = CB_BYTES_IN_FRAME;
    }

    if(CB_FRAME_BYTE_STX == (*pRxByte) && CB_BYTES_IN_FRAME == remaining_frame_bytes)
    {
        // If the current ACK state is CB_ACK_NOT_READY, the Request (Rx) index should not be updated to allow the
        // Response (Tx) state machine to consume the index in consecutive order.
        if(CB_ACK_NOT_READY != cb.request.data[cb.request.index].ack_status)
        {
            if((CB_REQUEST_BUFF_SIZE - 1) > cb.request.index)
            {
                cb.request.index++;
            }
            else
            {
                cb.request.index = 0;
            }
        }

        // Clean the index buffer to used in this iteration.
        memset(&cb.request.data[cb.request.index], 0x00, sizeof(cbroker_rx_data_t));
        // Set status bits to NO Error flag.
        cbroker_rx_set_status_bit(CB_CMD_ID_STATUS_BIT_NO_ERR);
        // Saving STX byte.
        cb.request.data[cb.request.index].buff.stx = (*pRxByte);
        // STX byte received.
        remaining_frame_bytes--;
        next_state = CB_RX_VALIDATE_PACKET_NUMBER_STATE;
    }
    else if(CB_FRAME_BYTE_ETX == (*pRxByte) && (CB_BYTES_IN_FRAME - 1) == remaining_frame_bytes)
    {
        next_state = CB_RX_IDLE_STATE;
        // Restart remaining_frame_bytes for next command.
        remaining_frame_bytes = CB_BYTES_IN_FRAME;
        // Saving ETX byte.
        cb.request.data[cb.request.index].buff.etx = (*pRxByte);

        if(CB_CMD_ID_STATUS_BIT_NO_ERR == status_bits)
        {
            if(cb.request.callback)
            {
                cbroker_cmd_id_e cmd_id = (CB_CMD_ID_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id);
                cb.request.callback(cmd_id, &cb.request.data[cb.request.index].buff.data, &cb.response.bin_data);
                CB_PRINTF("CB - [Rx]: PN=0x%.4X, ST=0x%.2X, ID=0x%.2X, CRC=0x%.4X\r\n",
                          cb.request.data[cb.request.index].buff.packet_number,
                          (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id_with_status),
                          cmd_id, cb.request.data[cb.request.index].buff.crc16_received);
            }
        }
        // As soon ETX byte is received, the command response can starts.
        cbroker_tx_send_response();
    }
    else
    {
        remaining_frame_bytes = CB_BYTES_IN_FRAME;
    }

    return next_state;
}

static cbroker_rx_system_state_e cbroker_rx_validate_packet_number(const uint8_t *const pRxByte)
{
    static uint8_t               remaining_nibbles = (CB_BYTES_IN_PACKET_NUMBER * CB_NIBBLES_IN_A_BYTE);
    cbroker_rx_system_state_e    next_state        = CB_RX_VALIDATE_PACKET_NUMBER_STATE;
    cbroker_cmd_id_status_bits_e status_bits =
        (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.status);

    // This flag indicates that an invalid rxByte was detected during AsciiHex to Bin conversion.
    if(CB_CMD_ID_STATUS_BIT_ERR == status_bits)
    {
        remaining_nibbles                                    = (CB_BYTES_IN_PACKET_NUMBER * CB_NIBBLES_IN_A_BYTE);
        cb.request.data[cb.request.index].buff.packet_number = 0;
        next_state                                           = CB_RX_IDLE_STATE;
        return next_state;
    }

    remaining_nibbles--;
    // Saving the rxByte in the corresponding nibble.
    cb.request.data[cb.request.index].buff.packet_number = cb.request.data[cb.request.index].buff.packet_number |
                                                           ((*pRxByte) << (CB_BITS_IN_A_NIBBLE * remaining_nibbles));

    if(0 == remaining_nibbles)
    {
        remaining_nibbles = (CB_BYTES_IN_PACKET_NUMBER * CB_NIBBLES_IN_A_BYTE);
        next_state        = CB_RX_VALIDATE_CMD_ID_STATE;
    }

    return next_state;
}

static cbroker_rx_system_state_e cbroker_rx_validate_cmd_id(const uint8_t *const pRxByte)
{
    static uint8_t               remaining_nibbles = (CB_BYTES_IN_CMD_ID * CB_NIBBLES_IN_A_BYTE);
    static cbroker_cmd_id_e      cmd_id_bits       = 0;
    cbroker_rx_system_state_e    next_state        = CB_RX_VALIDATE_CMD_ID_STATE;
    cbroker_cmd_id_status_bits_e status_bits =
        (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.status);

    // This flag indicates that an invalid rxByte was detected during AsciiHex to Bin conversion.
    if(CB_CMD_ID_STATUS_BIT_ERR == status_bits)
    {
        remaining_nibbles = (CB_BYTES_IN_CMD_ID * CB_NIBBLES_IN_A_BYTE);
        cmd_id_bits       = 0;
        next_state        = CB_RX_IDLE_STATE;
        return next_state;
    }

    remaining_nibbles--;
    // Saving the rxByte in the corresponding nibble.
    cmd_id_bits = cmd_id_bits | ((*pRxByte) << (CB_BITS_IN_A_NIBBLE * remaining_nibbles));

    if(0 == remaining_nibbles)
    {
        next_state = CB_RX_IDLE_STATE;

        // Restart remaining_nibbles for the next request command.
        remaining_nibbles = (CB_BYTES_IN_CMD_ID * CB_NIBBLES_IN_A_BYTE);
        if(DISP_CMD_ID_UNUSED < cmd_id_bits && DISP_CMD_ID_MAX > cmd_id_bits)
        {
            // Adding the status bits to the Rx command id.
            cb.request.data[cb.request.index].buff.cmd.id = (cmd_id_bits | status_bits);

            if((DISP_CLEAR == cmd_id_bits) || (DISP_GET_VERSION == cmd_id_bits))
            {
                // There is no data payload expected in CB_CMD_ID_CLEAR and CB_CMD_ID_GET_VERSION commands.
                next_state = CB_RX_VALIDATE_CRC_STATE;
            }
            else
            {
                next_state = CB_RX_VALIDATE_PAYLOAD_STATE;
            }

            // As soon as we have the command id, we can start sending the response message.
            cb.request.data[cb.request.index].ack_status = CB_ACK_TO_BE_SEND;
        }

        cmd_id_bits = 0;
    }

    return next_state;
}

static cbroker_rx_system_state_e cbroker_rx_validate_read_keys_data(void)
{
    cbroker_rx_system_state_e next_state = CB_RX_VALIDATE_CRC_STATE;
    if(CB_READ_KEYS_DATA_MAX <= cb.request.data[cb.request.index].buff.data.read_keys)
    {
        next_state = CB_RX_IDLE_STATE;
    }
    return next_state;
}

static cbroker_rx_system_state_e cbroker_rx_validate_set_bglight_data(void)
{
    cbroker_rx_system_state_e next_state = CB_RX_VALIDATE_CRC_STATE;
    if(CB_SET_BGLIGHT_DATA_MAX <= cb.request.data[cb.request.index].buff.data.set_bglight)
    {
        next_state = CB_RX_IDLE_STATE;
    }
    return next_state;
}

static cbroker_rx_system_state_e cbroker_rx_validate_set_language_data(void)
{
    cbroker_rx_system_state_e next_state = CB_RX_VALIDATE_CRC_STATE;
    if(CB_SET_LANGUAGE_DATA_MAX <= cb.request.data[cb.request.index].buff.data.set_language)
    {
        next_state = CB_RX_IDLE_STATE;
    }
    return next_state;
}

static cbroker_rx_system_state_e cbroker_rx_validate_buz_param_data(void)
{
    cbroker_rx_system_state_e next_state = CB_RX_IDLE_STATE;
    if(CB_BUZ_PARAM_DATA0_FREQ == cb.request.data[cb.request.index].buff.data.buz_param.type)
    {
        if(CB_BUZ_PARAM_DATA1_FREQ_LOWER_LIMIT <= cb.request.data[cb.request.index].buff.data.buz_param.value.freq &&
           CB_BUZ_PARAM_DATA1_FREQ_UPPER_LIMIT >= cb.request.data[cb.request.index].buff.data.buz_param.value.freq)
        {
            next_state = CB_RX_VALIDATE_CRC_STATE;
        }
    }
    else if(CB_BUZ_PARAM_DATA0_DUTY_CYCLE == cb.request.data[cb.request.index].buff.data.buz_param.type)
    {
        if(CB_BUZ_PARAM_DATA1_DUTY_CYCLE_LOWER_LIMIT <= cb.request.data[cb.request.index].buff.data.buz_param.value.duty_cycle &&
           CB_BUZ_PARAM_DATA1_DUTY_CYCLE_UPPER_LIMIT >= cb.request.data[cb.request.index].buff.data.buz_param.value.duty_cycle)
        {
            next_state = CB_RX_VALIDATE_CRC_STATE;
        }
    }
    return next_state;
}

static cbroker_rx_system_state_e cbroker_rx_validate_buz_ctrl_data(void)
{
    cbroker_rx_system_state_e            next_state = CB_RX_IDLE_STATE;
    cbroker_buz_ctrl_data0_action_bits_e action_bits =
        (CB_BUZ_CTRL_DATA0_ACTION_BITS_MASK & cb.request.data[cb.request.index].buff.data.buz_ctrl.data0.action);
    cbroker_buz_ctrl_data0_num_cycles_bits_e num_cycles =
        (CB_BUZ_CTRL_DATA0_CYCLES_BITS_MASK & cb.request.data[cb.request.index].buff.data.buz_ctrl.data0.cycles);

    if(CB_BUZ_CTRL_DATA0_ACTION_OFF == action_bits || CB_BUZ_CTRL_DATA0_ACTION_ON == action_bits)
    {
        if(CB_BUZ_CTRL_DATA0_NUM_CYCLES_LOWER_LIMIT <= num_cycles &&
           CB_BUZ_CTRL_DATA0_NUM_CYCLES_UPPER_LIMIT >= num_cycles)
        {
            next_state = CB_RX_VALIDATE_CRC_STATE;
        }
    }
    else if(CB_BUZ_CTRL_DATA0_ACTION_BEEP == action_bits)
    {
        if(CB_BUZ_CTRL_DATA1_BEEPER_ON_LOWER_LIMIT <= cb.request.data[cb.request.index].buff.data.buz_ctrl.beeper_on &&
           CB_BUZ_CTRL_DATA1_BEEPER_ON_UPPER_LIMIT >= cb.request.data[cb.request.index].buff.data.buz_ctrl.beeper_on)
        {
            if(CB_BUZ_CTRL_DATA2_BEEPER_OFF_LOWER_LIMIT <=
                   cb.request.data[cb.request.index].buff.data.buz_ctrl.beeper_off &&
               CB_BUZ_CTRL_DATA2_BEEPER_OFF_UPPER_LIMIT >=
                   cb.request.data[cb.request.index].buff.data.buz_ctrl.beeper_off)
            {
                next_state = CB_RX_VALIDATE_CRC_STATE;
            }
        }
    }
    return next_state;
}

static uint8_t cbroker_rx_fill_data_buffer(const uint8_t *const pRxByte)
{
    /**
     * @brief Command ID to data size table.
     */
    static const uint8_t request_cmd_id_to_data_size_tbl[DISP_CMD_ID_MAX] = {
        0, // The first command id starts at 0x01.
        CB_RX_BYTES_IN_READ_KEYS_DATA,
        CB_RX_BYTES_IN_WRITE_LINE_DATA,
        CB_RX_BYTES_IN_SET_BGLIGHT_DATA,
        CB_RX_BYTES_IN_CLEAR_DATA,
        CB_RX_BYTES_IN_SET_LANGUAGE_DATA,
        CB_RX_BYTES_IN_GET_VERSION_DATA,
        CB_RX_BYTES_IN_BUZ_PARAM_DATA,
        CB_RX_BYTES_IN_BUZ_CTRL_DATA,
    };
    static uint8_t               nibbles_to_shiff = 0;
    static uint8_t               saved_bytes_cnt  = 0;
    uint8_t                      remaining_bytes  = 0;
    cbroker_cmd_id_e             cmd_id_bits = (CB_CMD_ID_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id);
    cbroker_cmd_id_status_bits_e status_bits =
        (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.status);

    // This flag indicates that an invalid rxByte was detected during AsciiHex to Bin conversion.
    if(CB_CMD_ID_STATUS_BIT_ERR == status_bits)
    {
        nibbles_to_shiff = 0;
        saved_bytes_cnt  = 0;
        return remaining_bytes;
    }

    if(DISP_CMD_ID_UNUSED < cmd_id_bits && DISP_CMD_ID_MAX > cmd_id_bits)
    {
        remaining_bytes = request_cmd_id_to_data_size_tbl[cmd_id_bits];
    }

    // Shifting from/to Most Significant Nibble to Less Significant Nibble.
    nibbles_to_shiff ^= ((uint8_t)0x01);

    // Saving the rxByte in to the request buffer data union.
    cb.request.data[cb.request.index].buff.data.raw[saved_bytes_cnt] =
        cb.request.data[cb.request.index].buff.data.raw[saved_bytes_cnt] |
        ((*pRxByte) << (CB_BITS_IN_A_NIBBLE * nibbles_to_shiff));

    if(0 == nibbles_to_shiff)
    {
        saved_bytes_cnt++;
        remaining_bytes = remaining_bytes - saved_bytes_cnt;
        if(0 == remaining_bytes)
        {
            saved_bytes_cnt = 0;
        }
    }
    return remaining_bytes;
}

static cbroker_rx_system_state_e cbroker_rx_validate_payload(const uint8_t *const pRxByte)
{
    typedef cbroker_rx_system_state_e (*cbroker_payload_validation_handler)(void);
    // clang-format off
    static const cbroker_payload_validation_handler validate_data_functions_table[DISP_CMD_ID_MAX] = {
        NULL, // CB_CMD_ID_UNUSED:       
        cbroker_rx_validate_read_keys_data,    // CB_CMD_ID_READ_KEYS
        NULL, // CB_CMD_ID_WRITE_LINE:   // There are no defined parameters to validate this command's data.
        cbroker_rx_validate_set_bglight_data,  // CB_CMD_ID_SET_BGLIGHT
        NULL, // CB_CMD_ID_CLEAR:        // There is no data expected in this command.
        cbroker_rx_validate_set_language_data, // CB_CMD_ID_SET_LANGUAGE
        NULL, // CB_CMD_ID_GET_VERSION:  // There is no data expected in this command.
        cbroker_rx_validate_buz_param_data,    // CB_CMD_ID_BUZ_PARAM
        cbroker_rx_validate_buz_ctrl_data      // CB_CMD_ID_BUZ_CTRL

    };
    // clang-format on

    uint8_t                      remaining_bytes = 0;
    cbroker_rx_system_state_e    next_state      = CB_RX_VALIDATE_PAYLOAD_STATE;
    cbroker_cmd_id_status_bits_e status_bits =
        (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.status);
    cbroker_cmd_id_e cmd_id_bits = (CB_CMD_ID_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id);

    remaining_bytes = cbroker_rx_fill_data_buffer(pRxByte);

    if(CB_CMD_ID_STATUS_BIT_NO_ERR == status_bits)
    {
        if(0 == remaining_bytes)
        {
            next_state = CB_RX_VALIDATE_CRC_STATE;

            if(validate_data_functions_table[cmd_id_bits] != NULL)
            {
                // Call the data validation function assigned to the current command id.
                next_state = ((*validate_data_functions_table[cmd_id_bits]))();
            }
        }
    }
    else
    {
        next_state = CB_RX_IDLE_STATE;
    }
    if(CB_RX_IDLE_STATE == next_state)
    {
        // Set error flag to unexpected payload data.
        cbroker_rx_set_status_bit(CB_CMD_ID_STATUS_BIT_ERR);
        CB_PRINTF("CB - [Rx]: PN=0x%.4X, ST=0x%.2X, ID=0x%.2X, Err=Unexpected payload data\r\n",
                  cb.request.data[cb.request.index].buff.packet_number,
                  (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id_with_status),
                  (CB_CMD_ID_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id));
        // Queue ACK response with error bit flag.
        cbroker_tx_send_response();
    }
    return next_state;
}

static cbroker_rx_system_state_e cbroker_rx_validate_crc16(const uint8_t *const pRxByte)
{
    static uint8_t            nibbles_cnt = (CB_BYTES_IN_CRC16_ARC * CB_NIBBLES_IN_A_BYTE);
    cbroker_rx_system_state_e next_state  = CB_RX_VALIDATE_CRC_STATE;

    nibbles_cnt--;

    // Adding every single nibble to crc16 received.
    cb.request.data[cb.request.index].buff.crc16_received =
        cb.request.data[cb.request.index].buff.crc16_received | ((*pRxByte) << (CB_BITS_IN_A_NIBBLE * nibbles_cnt));

    if(0 == nibbles_cnt)
    {
        // Restart nibbles_cnt for the next request message.
        nibbles_cnt = (CB_BYTES_IN_CRC16_ARC * CB_NIBBLES_IN_A_BYTE);

        if(cb.request.data[cb.request.index].buff.crc16_received == cb.request.data[cb.request.index].crc16_calc)
        {
            // Next state waits for ETX byte.
            next_state = CB_RX_VALIDATE_FRAME_STATE;
        }
        else
        {
            // Set error flag to CRC mismatching.
            cbroker_rx_set_status_bit(CB_CMD_ID_STATUS_BIT_ERR);
            CB_PRINTF(
                "CB - [Rx]: PN=0x%.4X, ST=0x%.2X,  ID=0x%.2X, Err=CRC mismatching (Rec:0x%.4X vs Calc: 0x%.4X)\r\n",
                cb.request.data[cb.request.index].buff.packet_number,
                (CB_CMD_ID_STATUS_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id_with_status),
                (CB_CMD_ID_BITS_MASK & cb.request.data[cb.request.index].buff.cmd.id),
                cb.request.data[cb.request.index].buff.crc16_received, cb.request.data[cb.request.index].crc16_calc);
            // Queue ACK response with error bit flag.
            cbroker_tx_send_response();
            next_state = CB_RX_IDLE_STATE;
        }
    }

    return next_state;
}

static void cbroker_rx_byte(uint8_t rxByte)
{
    /**
     * @brief Initialize array of structure with states with proper handler.
     */
    static const cbroker_rx_state_machine_t state_machine[] = {
        {CB_RX_IDLE_STATE, cbroker_rx_validate_frame},
        {CB_RX_VALIDATE_FRAME_STATE, cbroker_rx_validate_frame},
        {CB_RX_VALIDATE_PACKET_NUMBER_STATE, cbroker_rx_validate_packet_number},
        {CB_RX_VALIDATE_CMD_ID_STATE, cbroker_rx_validate_cmd_id},
        {CB_RX_VALIDATE_PAYLOAD_STATE, cbroker_rx_validate_payload},
        {CB_RX_VALIDATE_CRC_STATE, cbroker_rx_validate_crc16}};

    if((CB_RX_MAX_STATE > cb.request.next_state) && (state_machine[cb.request.next_state].handler != NULL))
    {
        // Command frame bytes are already in binary format.
        if(state_machine[cb.request.next_state].handler != cbroker_rx_validate_frame)
        {
            // CRC is calculated over Packet Number to end of Data.
            if(state_machine[cb.request.next_state].handler != cbroker_rx_validate_crc16)
            {
                // Compute incoming byte CRC.
                cb.request.data[cb.request.index].crc16_calc =
                    cborker_calc_crc16arc(cb.request.data[cb.request.index].crc16_calc, &rxByte, 1);
            }

            // Casting the rxByte from AsciiHex format to binary format. (Only from Packet Number to end of CRC)
            cbroker_rx_asciihex_to_bin(&rxByte);
        }

        // Function call as per the state and event and return the next state of the finite state machine.
        cb.request.next_state = ((*state_machine[cb.request.next_state].handler)(&rxByte));
    }
    else
    {
        // Invalid
    }
}

/******************************************** SERIAL COMMUNICATION FUNCTIONS *****************************************/

static uint8_t cbroker_sercom_rx_callback(uint8_t status, uint8_t *data, size_t size)
{
    (void)size;
    cbroker_rx_byte(data[0]);
    cb.sercomm->read_non_blocking(cb.sercomm->handle, &cb.request.rxbyte, 1, cbroker_sercom_rx_callback);
    return status;
}

uint8_t cbroker_init(base_driver *sercomm, cbroker_request_callback_t callback)
{
    uint8_t err         = 0;
    cb.sercomm          = sercomm;
    cb.request.callback = callback;

    if(NULL != cb.sercomm->handle && NULL != cb.request.callback)
    {
        cb.sercomm->read_non_blocking(cb.sercomm->handle, &cb.request.rxbyte, 1, cbroker_sercom_rx_callback);
    }
    else
    {
        err = 1;
    }

    return err;
}
