#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_timer.h>
#include <MDR32F9Qx_uart.h>
#include <MDR32F9Qx_dac.h>
#include <MDR32F9Qx_adc.h>

#include "Init.h"

#include "Interrupts.h"

#include <stdbool.h>

extern int State;
extern double distance;
extern double temp_distance;
extern double current_distance;

extern float I;

bool InitD = 0;
bool InitEnd = 1;

void Init_SysTick()
{
	SysTick->LOAD = Period;
	SysTick->CTRL |= CLKSOURSE|TICKINT|ENABLE_SysTick;
}

void Init_PortE()
{
	PORT_InitTypeDef PORTEInit;
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	PORTEInit.PORT_SPEED = PORT_SPEED_FAST;
	PORTEInit.PORT_Pin = PORT_Pin_0;
	PORTEInit.PORT_OE = PORT_OE_OUT;						
	PORTEInit.PORT_MODE = PORT_MODE_ANALOG; 								
	PORT_Init(MDR_PORTE, &PORTEInit);
	
	PORTEInit.PORT_Pin = PORT_Pin_3;
	PORTEInit.PORT_OE = PORT_OE_OUT; 							
	PORTEInit.PORT_FUNC = PORT_FUNC_PORT; 				
	PORTEInit.PORT_MODE = PORT_MODE_DIGITAL; 
	PORT_Init(MDR_PORTE, &PORTEInit); 							
	PORT_ResetBits(MDR_PORTE, PORT_Pin_3);
}

void Init_PortC()
{
	PORT_InitTypeDef PORTCInit;
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	PORTCInit.PORT_SPEED = PORT_SPEED_FAST;
	PORTCInit.PORT_Pin = PORT_Pin_0;
	PORTCInit.PORT_OE = PORT_OE_OUT; 							
	PORTCInit.PORT_FUNC = PORT_FUNC_PORT; 				
	PORTCInit.PORT_MODE = PORT_MODE_DIGITAL; 
	PORT_Init(MDR_PORTC, &PORTCInit); 							
	PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
}

void Init_PortB()
{
	PORT_InitTypeDef PORTBInit;
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	PORTBInit.PORT_SPEED = PORT_SPEED_FAST;
	
	//������������� PB6 ��� ����� RXD
	PORTBInit.PORT_Pin = PORT_Pin_6; 								
	PORTBInit.PORT_OE = PORT_OE_IN;									//����
	PORTBInit.PORT_PULL_UP = PORT_PULL_UP_ON;				//�������� � �������� ���������
	PORTBInit.PORT_FUNC = PORT_FUNC_ALTER; 					//������ ����� � �������������� ������
	PORT_Init(MDR_PORTB, &PORTBInit); 							//������������� ����� PA6
	
	//������������� PB5 ��� ������ TXD
	PORTBInit.PORT_Pin = PORT_Pin_5;
	PORTBInit.PORT_OE = PORT_OE_OUT;								//�����
	PORTBInit.PORT_PULL_UP = PORT_PULL_UP_OFF;			//�������� � �������� ��������� �����������
	PORTBInit.PORT_FUNC = PORT_FUNC_ALTER; 					//������ ����� � �������������� ������
	PORT_Init(MDR_PORTB, &PORTBInit); 							//������������� ����� PA7
}

void Init_PortA()
{
	PORT_InitTypeDef PORTAInit;										
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE); 
	PORTAInit.PORT_SPEED = PORT_SPEED_FAST;
	PORTAInit.PORT_Pin = PORT_Pin_7| PORT_Pin_1| PORT_Pin_3;
	PORTAInit.PORT_OE = PORT_OE_OUT; 							
	PORTAInit.PORT_FUNC = PORT_FUNC_PORT; 				
	PORTAInit.PORT_MODE = PORT_MODE_DIGITAL; 
	PORT_Init(MDR_PORTA, &PORTAInit); 							
	PORT_ResetBits(MDR_PORTA, PORT_Pin_7);
	
	//PORT_SetBits(MDR_PORTA, PORT_Pin_1);
	//PORT_SetBits(MDR_PORTA, PORT_Pin_3);

	PORTAInit.PORT_Pin = PORT_Pin_2;
	PORTAInit.PORT_OE = PORT_OE_OUT;
	PORTAInit.PORT_FUNC = PORT_FUNC_ALTER;
	PORTAInit.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_Init(MDR_PORTA, &PORTAInit);	
	
	PORTAInit.PORT_Pin = PORT_Pin_3;
	PORTAInit.PORT_OE = PORT_OE_OUT; 							
	PORTAInit.PORT_FUNC = PORT_FUNC_PORT; 				
	PORTAInit.PORT_MODE = PORT_MODE_DIGITAL; 
	PORT_Init(MDR_PORTA, &PORTAInit);
	
	PORTAInit.PORT_Pin = PORT_Pin_4;
	PORTAInit.PORT_OE = PORT_OE_OUT;
	PORTAInit.PORT_FUNC = PORT_FUNC_OVERRID;
	PORTAInit.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_Init(MDR_PORTA, &PORTAInit);
	
	//PORT_SetBits(MDR_PORTA, PORT_Pin_3);
	
	
	
}

void Init_PortD()
{
	PORT_InitTypeDef PORTDInit;										
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE); 
	PORTDInit.PORT_Pin = PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3;
	PORTDInit.PORT_SPEED = PORT_SPEED_FAST;
	PORTDInit.PORT_OE = PORT_OE_IN; 							 				
	PORTDInit.PORT_MODE = PORT_MODE_ANALOG; 
	PORT_Init(MDR_PORTD, &PORTDInit); 										
}

void Init_Timer3()
{
	TIMER_CntInitTypeDef TIM3Init;																			//���������� ��������� �������������	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER3, ENABLE);												//��������� ������������ 40���
	TIMER_BRGInit(MDR_TIMER3, TIMER_HCLKdiv4);													//��������� �������� �������� ������� (10 ���)
	TIMER_CntStructInit(&TIM3Init);																			//���������� ��������� ���������� �� ���������
	TIM3Init.TIMER_Prescaler = 4;																				//������� ������������ �������� �������
	TIM3Init.TIMER_Period = 5;																					//������� ������� ������� 5 ���
	TIM3Init.TIMER_CounterDirection = TIMER_CntDir_Dn;									//����������� ����� ����
	TIMER_CntInit(MDR_TIMER3, &TIM3Init);																//������������� ������� ����������
	TIMER_SetCounter(MDR_TIMER3, 5);																		//���������� � ������ �������� 5 ���
	NVIC_EnableIRQ(Timer3_IRQn);																				//��������� ���������� �� ������� 2
	NVIC_SetPriority(Timer3_IRQn, 0);																		//��������� ���������� ���������� (0 - 15)
	TIMER_ITConfig(MDR_TIMER3, TIMER_STATUS_CNT_ZERO, ENABLE);					//���������� ���������� ������� 2 �� ���������� 0
	TIMER_Cmd(MDR_TIMER3, ENABLE);																			//��������� ������
}

void Init_DAC()
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_DAC, ENABLE); 													//����������
	DAC2_Init(DAC2_AVCC); 																							//��������� DAC2 �� ������ � AVCC
	DAC2_Cmd(ENABLE); 																									//��������� DAC2
}

void Init_Timer2()
{
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;
	TIMER_CntInitTypeDef TIM2Init;																			//���������� ��������� �������������	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2, ENABLE);												//��������� ������������ 40���
	TIMER_BRGInit(MDR_TIMER2, TIMER_HCLKdiv4);													//��������� �������� �������� ������� (10 ���)
	TIMER_CntStructInit(&TIM2Init);																			//���������� ��������� ���������� �� ���������
	TIM2Init.TIMER_Prescaler = 5;																				//������� ������������ �������� �������
	TIM2Init.TIMER_Period = 100;																				//������� ������� �������
	TIM2Init.TIMER_CounterMode = TIMER_CntMode_ClkFixedDir;
	TIMER_CntInit(MDR_TIMER2, &TIM2Init);																//������������� ������� ����������
	
	TIMER_ChnStructInit(&sTIM_ChnInit);
	sTIM_ChnInit.TIMER_CH_Mode = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_REF_Format = TIMER_CH_REF_Format6;
	// �������������� ����� 1
	//sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL1;
	//TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);
	// �������������� ����� 2
	sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL2;
	TIMER_ChnInit(MDR_TIMER2, &sTIM_ChnInit);

	// ������������� ������������ ��������� �� ������� ������
	//TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
	TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 0);

	TIMER_ChnOutStructInit(&sTIM_ChnOutInit);
	// ����� ��������� ������� ��� ������� ������ CHxN - ������ REF
	sTIM_ChnOutInit.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF;
	// ��������� ������� ������ ���������������� CHxN �� ����� ������
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output;
	// ����� ��������� ������� ��� ���������� ������ CHxN - ������ REF
	sTIM_ChnOutInit.TIMER_CH_NegOut_Source = TIMER_CH_OutSrc_REF;
	// ��������� ���������� ������ ���������������� CHxN �� ����� ������
	sTIM_ChnOutInit.TIMER_CH_NegOut_Mode = TIMER_CH_OutMode_Output;
	sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity = TIMER_CHOPolarity_NonInverted;
	// ����������� ������ ������ 1
	//sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL1;
	//TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);
	// ����������� ������ ������ 2
	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL2;
	TIMER_ChnOutInit(MDR_TIMER2, &sTIM_ChnOutInit);
	
	TIMER_Cmd(MDR_TIMER2, ENABLE);																			//��������� ������
}

void Init_ADC()
{
	ADC_InitTypeDef ADC;																								//���������� ��������� ������������� ���
	ADCx_InitTypeDef ADC1;
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_ADC, ENABLE);		//��������� ������������ ���
	
	ADC_StructInit(&ADC);																								//���������� ��������� ��� ���������� �� ���������
	ADC_Init(&ADC);																											//������������� ���
		
	ADCx_StructInit(&ADC1);																							//���������� ��������� ���1 ���������� �� ���������
	ADC1.ADC_ChannelNumber = ADC_CH_ADC1;																//����� 1 ������ ���
	ADC1.ADC_ClockSource = ADC_CLOCK_SOURCE_CPU;												//������������ �� ���� CPU
	ADC1.ADC_Prescaler = ADC_CLK_div_4;			 														//�������� ������� �� 4 (10 ���)
	ADC1.ADC_DelayGo = 7;																								//�������� ����� ������� ��������������
	ADC1_Init(&ADC1);																										//������������� ���1

	NVIC_EnableIRQ(ADC_IRQn);																						//���������� ���������� ���
	NVIC_SetPriority(ADC_IRQn, 2);																			//��������� ���������� (0 - 15)
	ADC1_ITConfig(ADCx_IT_END_OF_CONVERSION, ENABLE);										//���������� �� ���������� ��������������
	ADC1_Cmd(ENABLE);																										//��������� ���
	//ADC1_Start();																											//������ ���
}

void Init_Timer1()
{
	TIMER_CntInitTypeDef sTIM_CntInit;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;		
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);												// ��������� ������������ ������� 1
	TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv4);													//��������� �������� �������� ������� (10 ���)
	TIMER_CntStructInit(&sTIM_CntInit);																	//���������� ��������� ���������� �� ���������	
	sTIM_CntInit.TIMER_Prescaler = 5;																	//������� ������������ �������� �������
	sTIM_CntInit.TIMER_Period = 100;																		//������� ������� �������
	sTIM_CntInit.TIMER_CounterMode = TIMER_CntMode_ClkFixedDir;
	TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);
	
	TIMER_ChnStructInit(&sTIM_ChnInit);
	sTIM_ChnInit.TIMER_CH_Mode = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_REF_Format = TIMER_CH_REF_Format6;
	// �������������� ����� 1
	sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL1;
	TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);
	// �������������� ����� 2
	//sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL2;
	//TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

	// ������������� ������������ ��������� �� ������� ������
	TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
	//TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL2, 0);

	TIMER_ChnOutStructInit(&sTIM_ChnOutInit);
	// ����� ��������� ������� ��� ������� ������ CHxN - ������ REF
	sTIM_ChnOutInit.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF;
	// ��������� ������� ������ ���������������� CHxN �� ����� ������
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output;
	// ����� ��������� ������� ��� ���������� ������ CHxN - ������ REF
	sTIM_ChnOutInit.TIMER_CH_NegOut_Source = TIMER_CH_OutSrc_REF;
	// ��������� ���������� ������ ���������������� CHxN �� ����� ������
	sTIM_ChnOutInit.TIMER_CH_NegOut_Mode = TIMER_CH_OutMode_Output;
	sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity = TIMER_CHOPolarity_Inverted;
	// ����������� ������ ������ 1
	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL1;
	TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);
	// ����������� ������ ������ 2
	//sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL2;
	//TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

	/* ����� ���� �������� ��������� ������ ������� 1 */
	TIMER_Cmd(MDR_TIMER1,ENABLE);
}

void Init_UART1()
{
	UART_InitTypeDef UART1; 																						//����������� ���������� ��� ������������� UART1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);												//���������� ������������ UART1
	UART_DeInit(MDR_UART1);
	UART_BRGInit(MDR_UART1, UART_HCLKdiv4);															// ������������� �������� �������� ������� ��� UART1
	NVIC_EnableIRQ(UART1_IRQn);																					// ���������� ���������� ��� UART1

	UART1.UART_BaudRate = 115200; 																			//�������� ������� �������� ������
	UART1.UART_WordLength = UART_WordLength8b; 													//����� �������� 8 ���
	UART1.UART_StopBits = UART_StopBits1; 															//1 ���� ���
	UART1.UART_Parity = UART_Parity_No; 																//���������� �� ��������
	UART1.UART_FIFOMode = UART_FIFO_OFF; 																// ���������� FIFO ������
	UART1.UART_HardwareFlowControl = 
									UART_HardwareFlowControl_RXE|
									UART_HardwareFlowControl_TXE;												// ���������� �������� �� ��������� � ������� 
	
	UART_Init (MDR_UART1, &UART1); 																			//������������� UART1
	UART_ITConfig (MDR_UART1, UART_IT_RX, ENABLE);											//���������� ���������� �� ������
	//UART_ITConfig (MDR_UART1, UART_IT_TX, ENABLE);										//���������� ���������� �� �������� ��������
	
	UART_Cmd(MDR_UART1, ENABLE); 																				//���������� ������ UART1
}

void InitDriver()
{
	State = 1;
	while (State)
	{
		Update_Data();
		UpdateState();
	}
	while (I > 1)
		Update_Data();
	distance = 0;
	InitD = 1;
	State = 2;
	while (State)
	{
		Update_Data();
		UpdateState();
	}
	InitEnd = 1;
}

