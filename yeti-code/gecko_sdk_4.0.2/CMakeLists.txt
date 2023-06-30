cmake_minimum_required(VERSION 3.13)

project(gecko-sdk
    VERSION 0.1
    DESCRIPTION "gecko sdk yeti display"
    LANGUAGES
        C
)

add_library(${PROJECT_NAME}
   OBJECT   # - IJ For creating object library not static. This fixed Display CMake/Docker binary
   platform/Device/SiliconLabs/EFM32PG22/Source/GCC/startup_efm32pg22.c  
   platform/Device/SiliconLabs/EFM32PG22/Source/system_efm32pg22.c  
   platform/common/toolchain/src/sl_memory.c   
   platform/driver/i2cspm/src/sl_i2cspm.c  
   platform/driver/leddrv/src/sl_led.c  
   platform/driver/leddrv/src/sl_simple_led.c   
   platform/driver/pwm/src/sl_pwm.c   
   platform/emlib/src/em_assert.c
   platform/emlib/src/em_burtc.c   
   platform/emlib/src/em_cmu.c   
   platform/emlib/src/em_core.c   
   platform/emlib/src/em_emu.c   
   platform/emlib/src/em_gpio.c   
   platform/emlib/src/em_i2c.c
   platform/emlib/src/em_rtcc.c  
   platform/emlib/src/em_system.c   
   platform/emlib/src/em_timer.c
   platform/service/device_init/src/sl_device_init_dcdc_s2.c   
   platform/service/device_init/src/sl_device_init_emu_s2.c   
   platform/service/device_init/src/sl_device_init_nvic.c  
   platform/service/system/src/sl_system_init.c   
   platform/service/system/src/sl_system_process_action.c  
   platform/service/udelay/src/sl_udelay.c  
   platform/emlib/src/em_wdog.c
   platform/emdrv/gpiointerrupt/src/gpiointerrupt.c
   platform/emdrv/uartdrv/src/uartdrv.c
   platform/emdrv/dmadrv/src/dmadrv.c
   platform/emlib/src/em_eusart.c
   platform/emlib/src/em_usart.c
   platform/emlib/src/em_ldma.c
   platform/service/udelay/src/sl_udelay_armv6m_gcc.S  
   platform/driver/button/src/sl_simple_button.c
   platform/driver/button/src/sl_button.c
   platform/emdrv/spidrv/src/spidrv.c
   platform/service/sleeptimer/src/sl_sleeptimer_hal_burtc.c
   platform/service/sleeptimer/src/sl_sleeptimer.c
   platform/service/sleeptimer/src/sl_sleeptimer_hal_rtcc.c
   platform/common/src/sl_slist.c

   # Adding here Autogen folder Sources
   ../autogen/sl_device_init_clocks.c
   ../autogen/sl_event_handler.c
   ../autogen/sl_i2cspm_init.c
   ../autogen/sl_pwm_init.c
   ../autogen/sl_simple_button_instances.c
   ../autogen/sl_simple_led_instances.c
   ../autogen/sl_spidrv_init.c
   ../autogen/sl_uartdrv_init.c
)

target_include_directories (${PROJECT_NAME}
    PUBLIC
   platform/Device/SiliconLabs/EFM32PG22/Include  
   platform/common/toolchain/inc   
   platform/driver/i2cspm/inc  
   platform/driver/leddrv/inc   
   platform/driver/pwm/inc   
   platform/emlib/inc   
   platform/service/device_init/inc   
   platform/service/system/inc   
   platform/service/udelay/inc
   platform/service/sleeptimer/inc
   platform/common/inc   
   platform/emlib/init/gpio_simple   
   platform/CMSIS/Include

   # Adding here Autogen folder libs
   ../config
   ../autogen
   platform/emdrv/gpiointerrupt/inc
   platform/emdrv/uartdrv/inc
   platform/emdrv/common/inc
   platform/emdrv/dmadrv/inc
   platform/emdrv/dmadrv/inc
   platform/driver/button/inc
   platform/emdrv/spidrv/inc
   platform/common/spidrv/inc
   platform/emlib/inc/
   platform/service/sleeptimer/inc
)


# this is a good place for this declaration.
# turn this off when you don't need it.
set(CMAKE_VERBOSE_MAKEFILE       OFF)
