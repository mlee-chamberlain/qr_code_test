#ifndef BASE_SERCOMM_DRIVER
#define BASE_SERCOMM_DRIVER

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief  UART tx generic callback.
 *
 * @param [out] status - Status of transmission.
 * @param [out] size - Lenght of transmited data.
 */
typedef uint8_t (*callback_transmit_t)(uint8_t status, size_t size);
/**
 * @brief  UART rx generic callback.
 *
 * @param [out] status - Status of transmission.
 * @param [out] data - Pointer receive data.
 * @param [out] size - Lenght of transmited data.
 */
typedef uint8_t (*callback_receive_t)(uint8_t status, uint8_t *data, size_t size);
/**
 * @brief  Structure represents, base serial communication driver model.
 */
typedef struct base_sercomm_driver
{
    int (*open)(void *self, char *fspec);                                                                  ///< Pointer to init function.
    int (*close)(void *self);                                                                              ///< Pointer to deinit function.
    int (*write_non_blocking)(void *self, const uint8_t *buff, size_t size, callback_transmit_t callback); ///< Pointer to non blocking tx function.
    int (*read_non_blocking)(void *self, const uint8_t *buff, size_t size, callback_receive_t callback);   ///< Pointer to non blocking rx function.
    int (*write_blocking)(void *self, const uint16_t *buff, size_t size);                                   ///< Pointer to blocking tx function.
    int (*read_blocking)(void *self, const uint8_t *buff, size_t size);                                    ///< Pointer to blocking tx function.
    void *handle;                                                                                          ///< Pointer to autogen code handle.
} base_driver;

#ifdef __cplusplus
}
#endif

#endif // BASE_SERCOMM_DRIVER
