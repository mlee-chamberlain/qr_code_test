#ifndef BASE_GPIO_DRIVER
#define BASE_GPIO_DRIVER

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief  Button generic callback.
 *
 * @param [out] status - Status of button.
 */
typedef void (*callback_button_t)(uint8_t * status);

/**
 * @brief  Structure represents base gpio interface driver.
 */
typedef struct base_gpio_driver
{
    uint8_t (*get)(void *self, uint8_t *status); ///< Pointer to Get Gpio status function.
    int (*enable)(void *self);                   ///< Pointer to Enable Button function  //// This isn't generic
    void *handle;                                ///< Handler

} base_gpio_drv;

#ifdef __cplusplus
}
#endif

#endif // BASE_BUTTON_DRIVER
