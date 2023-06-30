#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// $[CMU]
// [CMU]$

// $[LFXO]
// [LFXO]$

// $[PRS.ASYNCH0]
// [PRS.ASYNCH0]$

// $[PRS.ASYNCH1]
// [PRS.ASYNCH1]$

// $[PRS.ASYNCH2]
// [PRS.ASYNCH2]$

// $[PRS.ASYNCH3]
// [PRS.ASYNCH3]$

// $[PRS.ASYNCH4]
// [PRS.ASYNCH4]$

// $[PRS.ASYNCH5]
// [PRS.ASYNCH5]$

// $[PRS.ASYNCH6]
// [PRS.ASYNCH6]$

// $[PRS.ASYNCH7]
// [PRS.ASYNCH7]$

// $[PRS.ASYNCH8]
// [PRS.ASYNCH8]$

// $[PRS.ASYNCH9]
// [PRS.ASYNCH9]$

// $[PRS.ASYNCH10]
// [PRS.ASYNCH10]$

// $[PRS.ASYNCH11]
// [PRS.ASYNCH11]$

// $[PRS.SYNCH0]
// [PRS.SYNCH0]$

// $[PRS.SYNCH1]
// [PRS.SYNCH1]$

// $[PRS.SYNCH2]
// [PRS.SYNCH2]$

// $[PRS.SYNCH3]
// [PRS.SYNCH3]$

// $[GPIO]
// [GPIO]$

// $[TIMER0]
// TIMER0 CC0 on PC04
#define TIMER0_CC0_PORT                          gpioPortC
#define TIMER0_CC0_PIN                           4

// [TIMER0]$

// $[TIMER1]
// [TIMER1]$

// $[TIMER2]
// [TIMER2]$

// $[TIMER3]
// [TIMER3]$

// $[TIMER4]
// [TIMER4]$

// $[USART0]
// USART0 RX on PC05
#define USART0_RX_PORT                           gpioPortC
#define USART0_RX_PIN                            5

// USART0 TX on PC06
#define USART0_TX_PORT                           gpioPortC
#define USART0_TX_PIN                            6

// [USART0]$

// $[USART1]
// USART1 CLK on PA06
#define USART1_CLK_PORT                          gpioPortA
#define USART1_CLK_PIN                           6

// USART1 CS on PA08
#define USART1_CS_PORT                           gpioPortA
#define USART1_CS_PIN                            8

// USART1 RX on PA00
#define USART1_RX_PORT                           gpioPortA
#define USART1_RX_PIN                            0

// USART1 TX on PA05
#define USART1_TX_PORT                           gpioPortA
#define USART1_TX_PIN                            5

// [USART1]$

// $[I2C1]
// [I2C1]$

// $[PDM]
// [PDM]$

// $[LETIMER0]
// [LETIMER0]$

// $[IADC0]
// [IADC0]$

// $[I2C0]
// I2C0 SCL on PC01
#define I2C0_SCL_PORT                            gpioPortC
#define I2C0_SCL_PIN                             1

// I2C0 SDA on PC02
#define I2C0_SDA_PORT                            gpioPortC
#define I2C0_SDA_PIN                             2

// [I2C0]$

// $[EUART0]
// EUART0 RX on PD03
#define EUART0_RX_PORT                           gpioPortD
#define EUART0_RX_PIN                            3

// EUART0 TX on PD02
#define EUART0_TX_PORT                           gpioPortD
#define EUART0_TX_PIN                            2

// [EUART0]$

// $[CUSTOM_PIN_NAME]
#define SPI_EMPTY_RX_PIN_PORT                    gpioPortA
#define SPI_EMPTY_RX_PIN_PIN                     0

#define LCD_BACKLIGHT_PORT                       gpioPortA
#define LCD_BACKLIGHT_PIN                        4

#define 3PBS_LOOP_PORT                           gpioPortB
#define 3PBS_LOOP_PIN                            1

#define 3PBS_STOP_PORT                           gpioPortB
#define 3PBS_STOP_PIN                            2

#define 3PBS_CLOSE_PORT                          gpioPortB
#define 3PBS_CLOSE_PIN                           3

#define 3PBS_OPEN_PORT                           gpioPortB
#define 3PBS_OPEN_PIN                            4

#define EEPROM_SCL_PORT                          gpioPortC
#define EEPROM_SCL_PIN                           1

#define EEPROM_SDA_PORT                          gpioPortC
#define EEPROM_SDA_PIN                           2

#define D10_LED_PORT                             gpioPortC
#define D10_LED_PIN                              3

// [CUSTOM_PIN_NAME]$

#endif // PIN_CONFIG_H

