/** @file debug_log.c
 *
 * @brief Serial Debug Logger - (Non Blocking)
 *
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
#include <stdarg.h>
#include "debug_log.h"

#define DEBUG_LOG_BUFF_SIZE ((size_t)1024)
/**
 * @brief Main Debug Logger Struct.
 */
typedef struct debug_log
{
    int8_t       buff[DEBUG_LOG_BUFF_SIZE];
    int32_t      start;           ///<  Read index.
    int32_t      end;             ///<  Write index.
    uint8_t      is_transmitting; ///< Flag to indicate if sercomm is transmitting.
    base_driver *sercomm;         ///< Pointer to structure represents, base serial communication driver.
} debug_log_t;

#if DEBUG_LOG_ENABLE == true

static debug_log_t logger = {.start = 0, .end = 0, .is_transmitting = false};

static int8_t *debug_log_get_char()
{
    int8_t *pItem = &logger.buff[logger.start++];
    logger.start %= DEBUG_LOG_BUFF_SIZE;
    return pItem;
}

static void debug_log_put_char(int8_t item)
{
    logger.buff[logger.end++] = item;
    logger.end %= DEBUG_LOG_BUFF_SIZE;
}

static uint8_t debug_log_tx_callback(uint8_t status, size_t size)
{
    (void)size;
    if(logger.start != logger.end)
    {
        logger.sercomm->write_non_blocking(logger.sercomm->handle, debug_log_get_char(), 1, debug_log_tx_callback);
    }
    else
    {
        logger.is_transmitting = false;
    }

    return status;
}

static void debug_log_start_transmission(void)
{
    // If the response state machine is already transmitting this response will be sent in FIFO order.
    if(false == logger.is_transmitting)
    {
        if(logger.start != logger.end)
        {
            logger.is_transmitting = true;
            logger.sercomm->write_non_blocking(logger.sercomm->handle, debug_log_get_char(), 1, debug_log_tx_callback);
        }
    }
}

static void debug_log_queue_msg(const int8_t *str)
{
    uint32_t c = 0;
    while(str[c])
    {
        debug_log_put_char(str[c++]);
        debug_log_start_transmission();
    }
}
#endif /* DEBUG_LOG_ENABLE */

void debug_log_print(const int8_t *format, ...)
{
#if DEBUG_LOG_ENABLE == true
    if(NULL != logger.sercomm)
    {
        int8_t  str[DEBUG_LOG_BUFF_SIZE];
        va_list args;
        va_start(args, format);
        vsnprintf((char *)str, DEBUG_LOG_BUFF_SIZE, (const char *)format, args);
        va_end(args);
        debug_log_queue_msg(str);
    }
#else
    (void)format;
#endif /* DEBUG_LOG_ENABLE */
}

uint8_t debug_log_init(base_driver *sercomm)
{
    uint8_t err = 1;
#if DEBUG_LOG_ENABLE == true
    logger.sercomm = sercomm;
    if(NULL != logger.sercomm)
    {
        err = 0;
    }
#else
    (void)sercomm;
#endif /* DEBUG_LOG_ENABLE */

    return err;
}
