/** @file lcd_spi.c
 *
 * @brief SPI Interfaces module
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

#include "sl_spidrv_instances.h"
#include "lcd_spi.h"

static callback_transmit_t lcd_spi_tx_callback;
static callback_receive_t  lcd_spi_rx_callback;

static uint8_t **rx_buffer; // Address of the return pointer

void lcd_spi_non_blocking_tx_callback(struct SPIDRV_HandleData *handle, Ecode_t transferStatus, int itemsTransferred)
{
    (void)handle;
    uint8_t status = 0;
    if(transferStatus)
    {
        status = 1; // return 1 if transferStatus is not 0
    }

    if(lcd_spi_tx_callback)
    {
        lcd_spi_tx_callback(status, (uint8_t)itemsTransferred);
    }
}

void lcd_spi_non_blocking_rx_callback(struct SPIDRV_HandleData *handle, Ecode_t transferStatus, int itemsTransferred)
{
    (void)handle;
    uint8_t status = 0;
    if(transferStatus)
    {
        status = 1; // return 1 if transferStatus is not 0
    }

    if(lcd_spi_rx_callback)
    {
        lcd_spi_rx_callback(status, *rx_buffer, (uint8_t)itemsTransferred);
    }
    rx_buffer = 0;
}

uint8_t lcd_spi_non_blocking_tx(void *self, const uint8_t *buff, size_t size, callback_transmit_t callback)
{
    uint8_t retval      = 0;
    lcd_spi_tx_callback = callback;
    uint32_t res        = SPIDRV_MTransmit((SPIDRV_Handle_t)self, (void *)buff, size, lcd_spi_non_blocking_tx_callback);

    if(ECODE_OK != res)
    {
        retval = 1;
    }
    return retval;
}

uint8_t lcd_spi_non_blocking_rx(void *self, uint8_t *buff, size_t size)
{
    if(rx_buffer)
    {
        return 1; // denied nonblocking RX operation if it's already active
    }
    rx_buffer = &buff;

    uint8_t  retval = 0;
    uint32_t res    = SPIDRV_MReceive((SPIDRV_Handle_t)self, (void *)buff, size, lcd_spi_non_blocking_rx_callback);

    if(ECODE_OK != res)
    {
        retval    = 1;
        rx_buffer = 0;
    }
    return retval;
}

uint8_t lcd_spi_blocking_rx(void *self, const uint8_t *buff, size_t size)
{
    uint8_t  retval = 0;
    uint32_t res    = SPIDRV_MReceiveB((SPIDRV_Handle_t)self, (uint8_t *)buff, size);

    if(ECODE_OK != res)
    {
        retval = 1;
    }
    return retval;
}

uint8_t lcd_spi_blocking_tx(void *self, const uint16_t *buff, size_t size)
{
    uint8_t  retval = 0;
    uint32_t res    = SPIDRV_MTransmitB((SPIDRV_Handle_t)self, buff, size);

    if(ECODE_OK != res)
    {
        retval = 1;
    }
    return retval;
}

void lcd_spi_init(struct base_sercomm_driver *dev)
{
    dev->open               = (void *)sl_spidrv_init_instances;
    dev->write_non_blocking = (void *)lcd_spi_non_blocking_tx;
    dev->read_non_blocking  = (void *)lcd_spi_non_blocking_rx;
    dev->write_blocking     = (void *)lcd_spi_blocking_tx;
    dev->read_blocking      = (void *)lcd_spi_blocking_rx;
    dev->handle             = sl_spidrv_lcd_spi_handle;
}
