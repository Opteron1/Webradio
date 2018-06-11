#ifndef _SYSTEM_INIT_H_
#define _SYSTEM_INIT_H_		1U

#include "stm32f2xx.h"
#include "fht.h"
#include "debug.h"
#include "eth.h"//temporary
/*
#include "misc.h"
#include "stm32f2xx_adc.h"
#include "stm32f2xx_dma.h"
#include "stm32f2xx_exti.h"
#include "stm32f2xx_flash.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_i2c.h"
#include "stm32f2xx_iwdg.h"
#include "stm32f2xx_pwr.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_rtc.h"
#include "stm32f2xx_spi.h"
#include "stm32f2xx_syscfg.h"
#include "stm32f2xx_tim.h"
#include "stm32f2xx_usart.h"
#include "stm32f2xx_wwdg.h"
*/

#define SYSTICK_PERIOD		100UL

/* ETHERNET + PHY */
#define PHY_ADDR			0x0001U
#define PHY_HCLK_DIV_42		0x000U
#define PHY_HCLK_DIV_62		0x001U
#define PHY_HCLK_DIV_16		0x010U
#define PHY_HCLK_DIV_26		0x011U


void xv_system_init(void);						/* Initialize CPU and all periphery */

static void v_system_init_rcc_init(void);		/* Initialize RCC module */
static void v_system_init_gpio_init(void);		/* Initialize GPIO module */
static void v_system_init_spi_init(void);		/* Initialize SPI module */
static void v_system_init_i2c_init(void);		/* Initialize I2C module */
static void v_system_init_usart_init(void);		/* Initialize USART module */
static void v_system_init_adc_init(void);		/* Initialize ADC module */
static void v_system_init_dma_init(void);		/* Initialize DMA module */
static void v_system_init_exti_init(void);		/* Initialize EXTI module */
static void v_system_init_flash_init(void);		/* Initialize FLASH module */
static void v_system_init_iwdg_init(void);		/* Initialize IWDG module */
static void v_system_init_pwr_init(void);		/* Initialize PWR module */
static void v_system_init_rtc_init(void);		/* Initialize RTC module */
static void v_system_init_syscfg_init(void);	/* Initialize SYSTEM config */
static void v_system_init_tim_init(void);		/* Initialize TIM module */
static void v_system_init_wwdg_init(void);		/* Initialize WWDG module */
static void v_system_init_nvic_init(void);		/* Initialize NVIC module */



#endif	/* _SYSTEM_INIT_H_ */
