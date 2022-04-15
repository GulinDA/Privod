#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_timer.h>
#include <MDR32F9Qx_uart.h>
#include <MDR32F9Qx_dac.h>
#include <MDR32F9Qx_adc.h>

#include "Init.h"
#include "Interrupts.h"

#include <stdbool.h>
#include <stdint.h>

extern int State;

void delayTick (int count)
{
	while (count--);
}

int main()
{
 	delayTick(0xFFFF);
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);																								// Включение HSE осциллятора (внешнего кварцевого резонатора) 
	if (RST_CLK_HSEstatus() == SUCCESS)
	{
		/* Если HSE осциллятор включился и прошел текст
			 Выбор HSE осциллятора в качестве источника тактовых импульсов для CPU_PLL
			 и установка умножителя тактовой частоты CPU_PLL равного 5 = 4+1 (40 МГц)*/
		RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, 4);
		RST_CLK_CPU_PLLcmd(ENABLE);																											// Включение схемы PLL
		if (RST_CLK_HSEstatus() == SUCCESS)																							// Если включение CPU_PLL прошло успешно
		{ 
			RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1); 																	// Установка CPU_C3_prescaler = 1
			RST_CLK_CPU_PLLuse(ENABLE); 																									// Установка CPU_C2_SEL от CPU_PLL выхода вместо CPU_C1 такта		 
			RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);																// Выбор CPU_C3 такта на мультиплексоре тактовых импульсов микропроцессора (CPUclock MUX)
		}
		else while(1);																																	// блок CPU_PLL не включился
	}
	else while(1); 																																		// кварцевый резонатор HSE не включился*/
	/* Инициализация портов, SysTick, USART и SPI и таймеров (все функции в файле init.c)*/
	//Init_SysTick();																																		// включение системного таймера, период 1 мс
	Init_PortA();																																			// инициализация порта A
	Init_PortB();																																			// инициализация порта B
	Init_PortC();																																			// инициализация порта C
	Init_PortE();																																			// инициализация порта E
	Init_PortD();																																			// инициализация порта D 
	Init_DAC();																																				// Инициализация ЦАП 
	Init_ADC();																																				//Инициализация АЦП
	Init_Timer2();																																		
	Init_Timer3();
	Init_Timer1();
	Init_UART1();
	
		TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
		TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);
	PORT_ResetBits(MDR_PORTA, PORT_Pin_1);
	PORT_ResetBits(MDR_PORTA, PORT_Pin_3);
	TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);//*cos(3.1416+tempAngle));
	TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);//100 - (MaxPWM*sin(tempAngle)));
	delayTick(0xFFFF);
	Update_Data();
	State = 0;
	while(1)
	{

		Update_Data();
		UpdateState();
				
	}
}