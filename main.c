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
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);																								// ��������� HSE ����������� (�������� ���������� ����������) 
	if (RST_CLK_HSEstatus() == SUCCESS)
	{
		/* ���� HSE ���������� ��������� � ������ �����
			 ����� HSE ����������� � �������� ��������� �������� ��������� ��� CPU_PLL
			 � ��������� ���������� �������� ������� CPU_PLL ������� 5 = 4+1 (40 ���)*/
		RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, 4);
		RST_CLK_CPU_PLLcmd(ENABLE);																											// ��������� ����� PLL
		if (RST_CLK_HSEstatus() == SUCCESS)																							// ���� ��������� CPU_PLL ������ �������
		{ 
			RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1); 																	// ��������� CPU_C3_prescaler = 1
			RST_CLK_CPU_PLLuse(ENABLE); 																									// ��������� CPU_C2_SEL �� CPU_PLL ������ ������ CPU_C1 �����		 
			RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);																// ����� CPU_C3 ����� �� �������������� �������� ��������� ��������������� (CPUclock MUX)
		}
		else while(1);																																	// ���� CPU_PLL �� ���������
	}
	else while(1); 																																		// ��������� ��������� HSE �� ���������*/
	/* ������������� ������, SysTick, USART � SPI � �������� (��� ������� � ����� init.c)*/
	//Init_SysTick();																																		// ��������� ���������� �������, ������ 1 ��
	Init_PortA();																																			// ������������� ����� A
	Init_PortB();																																			// ������������� ����� B
	Init_PortC();																																			// ������������� ����� C
	Init_PortE();																																			// ������������� ����� E
	Init_PortD();																																			// ������������� ����� D 
	Init_DAC();																																				// ������������� ��� 
	Init_ADC();																																				//������������� ���
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