/** @file debug_uart.c
 *
 * @brief UART Interfaces module
 * See: https://docs.silabs.com/gecko-platform/4.0/driver/api/group-uartdrv
 * @author Rene Delgado
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

#include "debug_uart.h"
#include "sl_uartdrv_instances.h"

static callback_transmit_t debug_uart_tx_callback;
static callback_receive_t  debug_uart_rx_callback;

void debug_uart_non_blocking_tx_callback(struct UARTDRV_HandleData *handle,
                                         Ecode_t                    transferStatus,
                                         uint8_t                   *data,
                                         UARTDRV_Count_t            transferCount)
{
    (void)handle;
    (void)data;
    uint8_t status = 0;

    if(debug_uart_tx_callback)
    {
        if(ECODE_OK != transferStatus)
        {
            status = 1;
        }
        debug_uart_tx_callback(status, transferCount);
    }
}

void debug_uart_non_blocking_callback_rx(struct UARTDRV_HandleData *handle,
                                         Ecode_t                    transferStatus,
                                         uint8_t                   *data,
                                         UARTDRV_Count_t            transferCount)
{
    (void)handle;
    uint8_t status = 0;

    if(debug_uart_rx_callback)
    {
        if(ECODE_OK != transferStatus)
        {
            status = 1;
        }
        debug_uart_rx_callback(status, data, transferCount);
    }
}

uint8_t debug_uart_non_blocking_tx(void *self, const uint8_t *buff, size_t size, callback_transmit_t callback)
{
    Ecode_t ecode          = 0;
    uint8_t retval         = 0;
    debug_uart_tx_callback = callback;
    ecode = UARTDRV_Transmit((UARTDRV_Handle_t)self, (uint8_t *)buff, size, debug_uart_non_blocking_tx_callback);

    if(ECODE_OK != ecode)
    {
        retval = 1;
    }
    return retval;
}

uint8_t debug_uart_non_blocking_rx(void *self, const uint8_t *buff, size_t size, callback_receive_t callback)
{
    Ecode_t ecode          = 0;
    uint8_t retval         = 0;
    debug_uart_rx_callback = callback;
    ecode = UARTDRV_Receive((UARTDRV_Handle_t)self, (uint8_t *)buff, size, debug_uart_non_blocking_callback_rx);

    if(ECODE_OK != ecode)
    {
        retval = 1;
    }
    return retval;
}

uint8_t debug_uart_blocking_rx(void *self, const uint8_t *buff, size_t size)
{
    Ecode_t ecode  = 0;
    uint8_t retval = 0;
    ecode          = UARTDRV_ReceiveB((UARTDRV_Handle_t)self, (uint8_t *)buff, size);

    if(ECODE_OK != ecode)
    {
        retval = 1;
    }
    return retval;
}

uint8_t debug_uart_blocking_tx(void *self, const uint8_t *buff, size_t size)
{
    Ecode_t ecode  = 0;
    uint8_t retval = 0;
    ecode          = UARTDRV_TransmitB((UARTDRV_Handle_t)self, (uint8_t *)buff, size);

    if(ECODE_OK != ecode)
    {
        retval = 1;
    }
    return retval;
}

void debug_uart_init(struct base_sercomm_driver *dev)
{
    dev->open               = (void *)sl_uartdrv_init_instances;
    dev->write_non_blocking = (void *)debug_uart_non_blocking_tx;
    dev->read_non_blocking  = (void *)debug_uart_non_blocking_rx;
    dev->write_blocking     = (void *)debug_uart_blocking_tx;
    dev->read_blocking      = (void *)debug_uart_blocking_rx;
    dev->handle             = sl_uartdrv_usart_debug_uart_handle;
}
