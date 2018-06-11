#include "vs.h"
#include "io.h"


volatile s16 sw_pressed = 0;

volatile s16 ir_data = 0;
u32 ir_address = 0x01C0U, ir_status = IR_DETECT, ir_1us = 0U;
u16 ir_keypower=57, ir_keyup=6, ir_keydown=12, ir_keyok=1, ir_keyvolp=3, ir_keyvolm=9;

void ethernet_setmac(uint64_t mac)
{
	u8 address[6];
	
	address[0] = (u8)(mac & 0x0000000000FF);
	address[1] = (u8)((mac & 0x00000000FF00) >> 8);
	address[2] = (u8)((mac & 0x000000FF0000) >> 16);
	address[3] = (u8)((mac & 0x0000FF000000) >> 24);
	address[4] = (u8)((mac & 0x00FF00000000) >> 32);
	address[5] = (u8)((mac & 0xFF0000000000) >> 40);
	ETH_MACAddressConfig(ETH_MAC_Address0, address);

  return;
}

u16 ethernet_link(void)
{
  return (ETH_ReadPHYRegister(PHY_ADDR, PHY_SR) & 0x01) ? 1 : 0;
}

void ethernet_put(u8 *pkt, u16 len)
{
  if(!ETH_HandleTxPkt(pkt, len))
  {
    DEBUGOUT("Eth: Tx put err\r\n");
  }
}

__irq void ETH_IRQHandler(void)
{
	{
		if(ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R))	//NI->R
		{
			while(eth_rxput());
			ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS | ETH_DMA_IT_R);
		}
	}
	return;
}

__irq void ETH_WKUP_IRQHandler(void)
{

}

__irq void EXTI15_10_IRQHandler(void)
{
	/************* Rotary encoder push button PORTE.10 *************/
	if(EXTI_GetFlagStatus(EXTI_Line10))
	{
		EXTI_ClearITPendingBit(EXTI_Line10);
		return;
	}
	/************* Touchscreen interrupt PORTE.15 *************/
	if(EXTI_GetFlagStatus(EXTI_Line15))
	{
		EXTI_ClearITPendingBit(EXTI_Line15);
		return;
	}
}

s16 ir_cmd(void)
{
  s16 data, cmd=0;
  static s16 last_data=0;

  data    = ir_data;
  ir_data = 0;

  if(data != 0)
  {
    DEBUGOUT("IR: %i\r\n", (data&0x3F));

    if((data&0x3F) == ir_keypower)
    {
      if((last_data^(1<<11)) == data)
      {
        cmd  = SW_POWER;
        data = 0;
      }
    }
    else if((data&0x3F) == ir_keyup)
    {
      cmd = SW_UP;
    }
    else if((data&0x3F) == ir_keydown)
    {
      cmd = SW_DOWN;
    }
    else if((data&0x3F) == ir_keyok)
    {
      cmd = SW_ENTER;
    }
    else if((data&0x3F) == ir_keyvolp)
    {
      cmd = SW_VOLP;
    }
    else if((data&0x3F) == ir_keyvolm)
    {
      cmd = SW_VOLM;
    }

    last_data = data;
  }

  return cmd;
}

s16 ir_rawdata(void)
{
  s16 data;

  data = ir_data;
  ir_data = 0;

  return data;
}

__irq void EXTI9_5_IRQHandler(void)
{
	u32 time, delta;
	EXTI_InitTypeDef EXTI_InitStructure;
	/************* VS1053_DREQ GPIOE.5 *************/
	if(EXTI_GetFlagStatus(EXTI_Line5))
	{
		vs_handler();
		//EXTI_ClearITPendingBit(EXTI_Line5);
		return;
	}
	/************* SD_CARD_DETECT GPIOA.6 *************/
	if(EXTI_GetFlagStatus(EXTI_Line6))
	{
		EXTI_ClearITPendingBit(EXTI_Line6);
		return;
	}
	
	/************* Remote control IR RC5 PORTE.7 *************/
	if(EXTI_GetFlagStatus(EXTI_Line7))
	{
		EXTI_ClearITPendingBit(EXTI_Line7);
		time = TIM_GetCounter(TIM7);

		switch(ir_status)
		{
			case IR_DETECT: //high for 2x bit time before start bit
				if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) == 0) //low
				{
					ir_status = IR_STARTBIT1;
				}
				TIM_SetCounter(TIM7, 0x0000U);
				break;

			case IR_STARTBIT1:
				if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)) //high
				{
					delta = time/ir_1us;
					if((delta >= ((IR_BITTIME/2)-IR_MAXERR)) && 
						 (delta <= ((IR_BITTIME/2)+IR_MAXERR)))
					{
						ir_status = IR_STARTBIT2;
						TIM_SetCounter(TIM7, 0x0000U);
						break;
					}
				}
				ir_status = IR_DETECT;
				break;

			case IR_STARTBIT2:
				if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) == 0) //low
				{
					delta = time/ir_1us;
					if((delta > ((IR_BITTIME/2)-IR_MAXERR)) && 
						 (delta < ((IR_BITTIME/2)+IR_MAXERR)))
					{
						ir_status = IR_DATABIT;
					
						TIM_SetCounter(TIM7, 0x0000U);
						TIM_SetAutoreload(TIM7, (IR_BITTIME+(IR_BITTIME/4))*ir_1us);
						TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
						EXTI_InitStructure.EXTI_Line = EXTI_Line7;
						EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
						EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
						EXTI_InitStructure.EXTI_LineCmd = DISABLE;
						EXTI_Init(&EXTI_InitStructure);
						TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
						break;
					}
				}
				ir_status = IR_DETECT;
				break;

			case IR_DATABIT:
				ir_status = IR_DETECT;
				break;
		}
		return;
	}
	/************* ESP_IRQ GPIOC.8 *************/
	if(EXTI_GetFlagStatus(EXTI_Line8))
	{
		EXTI_ClearITPendingBit(EXTI_Line8);
		return;
	}
}

__irq void EXTI3_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line3))
	{		
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

__irq void TIM7_IRQHandler(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;	
  static u16 bit=0, data=0, last_data=0;

  TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	
  if(bit == 0)
  {
    TIM_SetAutoreload(TIM7, IR_BITTIME * ir_1us);
  }

  if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) == 0) //1 bit
  {
    data <<= 1;
    data  |= 1;
  }
  else //0 bit
  {
    data <<= 1;
    //data  |= 0;
  }

  if(++bit == 12)
  {
    bit = 0;
    if(((data&0x07C0) == ir_address) || (ir_address == (IR_ALLADDR<<6)))
    {
      if(data != last_data)
      {
        last_data = data;
        if(ir_data == 0)
        {
          ir_data = data|0x8000;
        }
      }
      else
      {
        last_data = 0; //if button hold down get every 2nd cmd
      }
    }
    DEBUGOUT("IR: raw %x\r\n", data);
    data = 0;
    ir_status = IR_DETECT;

    TIM_ITConfig(TIM7, TIM_IT_Update, DISABLE);
    TIM_SetCounter(TIM7, 0x0000U);		/* GPIOE.7 */
    EXTI_ClearITPendingBit(EXTI_Line7);
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
  return;
}

u16 ir_getkeyvolm(void)  { return ir_keyvolm; }
u16 ir_getkeyvolp(void)  { return ir_keyvolp; }
u16 ir_getkeyok(void)    { return ir_keyok; }
u16 ir_getkeydown(void)  { return ir_keydown; }
u16 ir_getkeyup(void)    { return ir_keyup; }
u16 ir_getkeypower(void) { return ir_keypower; }


void ir_setkeyvolm(u16 key)  { ir_keyvolm  = key; return; }
void ir_setkeyvolp(u16 key)  { ir_keyvolp  = key; return; }
void ir_setkeyok(u16 key)    { ir_keyok    = key; return; }
void ir_setkeydown(u16 key)  { ir_keydown  = key; return; }
void ir_setkeyup(u16 key)    { ir_keyup    = key; return; }
void ir_setkeypower(u16 key) { ir_keypower = key; return; }


u16 ir_getaddr(void)
{
  return (ir_address>>6);
}


void ir_setaddr(u16 addr)
{
  if(addr <= 31)
  {
    ir_address = addr<<6;
  }

  return;
}


void ir_init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_ClocksTypeDef RCC_Clocks;
	ir_data    = 0;
	ir_status  = 0;
	RCC_GetClocksFreq(&RCC_Clocks);
	ir_1us     = RCC_Clocks.PCLK1_Frequency / 15000000UL;	/* 2ticks@30MHz */

	EXTI_ClearITPendingBit(EXTI_Line7);
	TIM_ITConfig(TIM7, TIM_IT_Update, DISABLE);
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	TIM_SetCounter(TIM7, 0x0000U);
	EXTI_ClearITPendingBit(EXTI_Line7);
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	return;
}

s16 keys_steps(void)
{
 	s16 i;

	i = TIM_GetCounter(TIM1);
	if(i)
	{
		TIM_SetCounter(TIM1, i&0x01);
		return (i>>1);
	}

	return 0;
}


s16 keys_sw(void)
{
	s16 sw;

	sw = sw_pressed;
	sw_pressed = 0;

	return sw;
}


void keys_timerservice(void) //100 Hz
{
	static u16 state=0;

	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10))
	{
		if(state > SW_LONGTIME) //1.0 s
		{
			if(sw_pressed == 0)
			{
				sw_pressed = SW_PRESSEDLONG;
			}
		}
		else if(state > SW_SHORTTIME)
		{
			if(sw_pressed == 0)
			{
				sw_pressed = SW_PRESSED;
			}
		}
		state = 0;
	}
	else
	{
		state++;
	}

	return;
}

void cpu_speed(u16 low_speed)
{
	USART_InitTypeDef USART_InitStructure;
	
  delay_ms(10);

  IntMasterDisable();
  xv_TFT_SetBacklight(0);

  if(low_speed)
  {
//#if 0
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);						// Select HSI as system clock source 25MHz
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);			// Wait till HSI is ready
    RCC_PLLCmd(DISABLE);										// Disable PLL 120MHz
    while(RCC_GetSYSCLKSource() != 0x00);						// Wait till HSI is used as system clock source
    SystemCoreClockUpdate();									// Update variable with current main frequency
    SysTickDisable();
    RCC_PCLK1Config(RCC_HCLK_Div1);								// PCLK1 = HCLK 25 MHz
    TIM7->PSC = 0x0018U;										// TIM7 0.5 us = 25MHz/(24+1)
    SysTick_Config(SystemCoreClock/SYSTICK_PERIOD); //100 Hz
    SysTickEnable();
//#endif
    xv_TFT_SetBacklight(LCD_PWMSTANDBY);
#warning COMMENT
    //ssi_speed(0);
    //vs_ssi_speed(0);
  }
  else
  {
//#if 0
  	RCC_PCLK1Config(RCC_HCLK_Div4);							// PCLK1 = HCLK/4 30 MHz
  	TIM7->PSC = 0x001DU;									// TIM7 0.5 us = 30MHz/(29+1)
    RCC_PLLCmd(ENABLE);										// Speed up, enable PLL 120MHz
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);		// Wait till PLL is ready
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				// Select PLL as system clock source
    while(RCC_GetSYSCLKSource() != 0x08);					// Wait till PLL is used as system clock source
    SystemCoreClockUpdate();								// Update variable with current main frequency
    SysTickDisable();
    SysTick_Config(SystemCoreClock/SYSTICK_PERIOD); //100 Hz
    SysTickEnable();
//#endif
    xv_TFT_SetBacklight(100);
#warning COMMENT
    //ssi_speed(0);
    //vs_ssi_speed(0);
  }

  ir_init();
  IntMasterEnable();

  delay_ms(10);

#ifdef DEBUG
  USART_Cmd(USART6, DISABLE);
  /********** USART6 - DEBUG **********/
  USART_InitStructure.USART_BaudRate = 115200UL;		/* 115200 Baud */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART6, &USART_InitStructure);
  USART_Cmd(USART6, ENABLE);
#endif

  return;
}

void cpu_reset(void)
{
	NVIC_SystemReset();
}

void IntMasterDisable(void)
{
	__disable_irq();
}

void IntMasterEnable(void)
{
	__enable_irq();
}

void SysTickIntDisable(void)
{
	SysTick->CTRL &= 0xFFFFFFFD;
}

void SysTickIntEnable(void)
{
	SysTick->CTRL |= 0x00000002;
}

void SysTickDisable(void)
{
	SysTick->CTRL &= 0xFFFFFFFE;
}

void SysTickEnable(void)
{
	SysTick->CTRL |= 0x00000001;
}

#define STM32_CLOCK_HZ 120000000UL
#define STM32_CYCLES_PER_LOOP 6 // This will need tweaking or calculating
void delay_ms(u32 ms)
{
	RCC_ClocksTypeDef RCC_Clocks;

	RCC_GetClocksFreq(&RCC_Clocks);

	u32 tmp = ms * (RCC_Clocks.PCLK1_Frequency/1000);
	/*
    ms *= RCC_Clocks.PCLK1_Frequency / 1000 / STM32_CYCLES_PER_LOOP;

    __ASM volatile(" mov r0, %[ms] \n\t"
             "1: subs r0, #1 \n\t"
             " bhi 1b \n\t"
             :
             : [ms] "r" (ms)
             : "r0");
	*/
	while(tmp--);
}
