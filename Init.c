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
	
	//инициализация PB6 как входа RXD
	PORTBInit.PORT_Pin = PORT_Pin_6; 								
	PORTBInit.PORT_OE = PORT_OE_IN;									//вход
	PORTBInit.PORT_PULL_UP = PORT_PULL_UP_ON;				//подтяжка к верхнему резистору
	PORTBInit.PORT_FUNC = PORT_FUNC_ALTER; 					//работа ножек в альтернативном режиме
	PORT_Init(MDR_PORTB, &PORTBInit); 							//инициализация ножки PA6
	
	//инициализация PB5 как выхода TXD
	PORTBInit.PORT_Pin = PORT_Pin_5;
	PORTBInit.PORT_OE = PORT_OE_OUT;								//выход
	PORTBInit.PORT_PULL_UP = PORT_PULL_UP_OFF;			//подтяжка к верхнему резистору отсутствует
	PORTBInit.PORT_FUNC = PORT_FUNC_ALTER; 					//работа ножек в альтернативном режиме
	PORT_Init(MDR_PORTB, &PORTBInit); 							//инициализация ножки PA7
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
	TIMER_CntInitTypeDef TIM3Init;																			//объявление структуры инициализации	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER3, ENABLE);												//включение тактирования 40МГц
	TIMER_BRGInit(MDR_TIMER3, TIMER_HCLKdiv4);													//Настройка делителя тактовой частоты (10 МГц)
	TIMER_CntStructInit(&TIM3Init);																			//Заполнение структуры значениями по умолчанию
	TIM3Init.TIMER_Prescaler = 4;																				//задание предделителя тактовой частоты
	TIM3Init.TIMER_Period = 5;																					//Задание периода таймера 5 мкс
	TIM3Init.TIMER_CounterDirection = TIMER_CntDir_Dn;									//Направление счета вниз
	TIMER_CntInit(MDR_TIMER3, &TIM3Init);																//Инициализация таймера структурой
	TIMER_SetCounter(MDR_TIMER3, 5);																		//Записываем в таймер значение 5 мкс
	NVIC_EnableIRQ(Timer3_IRQn);																				//включение прерываний от таймера 2
	NVIC_SetPriority(Timer3_IRQn, 0);																		//установка приоритета прерывания (0 - 15)
	TIMER_ITConfig(MDR_TIMER3, TIMER_STATUS_CNT_ZERO, ENABLE);					//разрешение прерывания таймера 2 по достижению 0
	TIMER_Cmd(MDR_TIMER3, ENABLE);																			//запускаем таймер
}

void Init_DAC()
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_DAC, ENABLE); 													//Тактование
	DAC2_Init(DAC2_AVCC); 																							//Настройка DAC2 на работу с AVCC
	DAC2_Cmd(ENABLE); 																									//Активация DAC2
}

void Init_Timer2()
{
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;
	TIMER_CntInitTypeDef TIM2Init;																			//объявление структуры инициализации	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2, ENABLE);												//включение тактирования 40МГц
	TIMER_BRGInit(MDR_TIMER2, TIMER_HCLKdiv4);													//Настройка делителя тактовой частоты (10 МГц)
	TIMER_CntStructInit(&TIM2Init);																			//Заполнение структуры значениями по умолчанию
	TIM2Init.TIMER_Prescaler = 5;																				//задание предделителя тактовой частоты
	TIM2Init.TIMER_Period = 100;																				//Задание периода таймера
	TIM2Init.TIMER_CounterMode = TIMER_CntMode_ClkFixedDir;
	TIMER_CntInit(MDR_TIMER2, &TIM2Init);																//Инициализация таймера структурой
	
	TIMER_ChnStructInit(&sTIM_ChnInit);
	sTIM_ChnInit.TIMER_CH_Mode = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_REF_Format = TIMER_CH_REF_Format6;
	// Инициализируем канал 1
	//sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL1;
	//TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);
	// Инициализируем канал 2
	sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL2;
	TIMER_ChnInit(MDR_TIMER2, &sTIM_ChnInit);

	// Устанавливаем длительность импульсов по каждому каналу
	//TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
	TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 0);

	TIMER_ChnOutStructInit(&sTIM_ChnOutInit);
	// Выбор источника сигнала для прямого выхода CHxN - сигнал REF
	sTIM_ChnOutInit.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF;
	// Настройна прямого выхода микроконтроллера CHxN на вывод данных
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output;
	// Выбор источника сигнала для инверсного выхода CHxN - сигнал REF
	sTIM_ChnOutInit.TIMER_CH_NegOut_Source = TIMER_CH_OutSrc_REF;
	// Настройна инверсного выхода микроконтроллера CHxN на вывод данных
	sTIM_ChnOutInit.TIMER_CH_NegOut_Mode = TIMER_CH_OutMode_Output;
	sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity = TIMER_CHOPolarity_NonInverted;
	// Настраиваем выходы канала 1
	//sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL1;
	//TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);
	// Настраиваем выходы канала 2
	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL2;
	TIMER_ChnOutInit(MDR_TIMER2, &sTIM_ChnOutInit);
	
	TIMER_Cmd(MDR_TIMER2, ENABLE);																			//запускаем таймер
}

void Init_ADC()
{
	ADC_InitTypeDef ADC;																								//объявление структуры инициализации АЦП
	ADCx_InitTypeDef ADC1;
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_ADC, ENABLE);		//включение тактирования АЦП
	
	ADC_StructInit(&ADC);																								//Заполнение структуры АЦП значениями по умолчанию
	ADC_Init(&ADC);																											//Инициализация АЦП
		
	ADCx_StructInit(&ADC1);																							//Заполнение структуры АЦП1 значениями по умолчанию
	ADC1.ADC_ChannelNumber = ADC_CH_ADC1;																//Выбор 1 канала АЦП
	ADC1.ADC_ClockSource = ADC_CLOCK_SOURCE_CPU;												//Тактирование от шины CPU
	ADC1.ADC_Prescaler = ADC_CLK_div_4;			 														//Делитель частоты на 4 (10 МГц)
	ADC1.ADC_DelayGo = 7;																								//Задержка перед началом преобразования
	ADC1_Init(&ADC1);																										//Инициализация АЦП1

	NVIC_EnableIRQ(ADC_IRQn);																						//Разрешение прерываний АЦП
	NVIC_SetPriority(ADC_IRQn, 2);																			//Приоритет прерываний (0 - 15)
	ADC1_ITConfig(ADCx_IT_END_OF_CONVERSION, ENABLE);										//Прерывание по завершению преобразования
	ADC1_Cmd(ENABLE);																										//Включение АЦП
	//ADC1_Start();																											//Запуск АЦП
}

void Init_Timer1()
{
	TIMER_CntInitTypeDef sTIM_CntInit;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;		
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);												// Включение тактирования таймера 1
	TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv4);													//Настройка делителя тактовой частоты (10 МГц)
	TIMER_CntStructInit(&sTIM_CntInit);																	//Заполнение структуры значениями по умолчанию	
	sTIM_CntInit.TIMER_Prescaler = 5;																	//задание предделителя тактовой частоты
	sTIM_CntInit.TIMER_Period = 100;																		//Задание периода таймера
	sTIM_CntInit.TIMER_CounterMode = TIMER_CntMode_ClkFixedDir;
	TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);
	
	TIMER_ChnStructInit(&sTIM_ChnInit);
	sTIM_ChnInit.TIMER_CH_Mode = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_REF_Format = TIMER_CH_REF_Format6;
	// Инициализируем канал 1
	sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL1;
	TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);
	// Инициализируем канал 2
	//sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL2;
	//TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

	// Устанавливаем длительность импульсов по каждому каналу
	TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
	//TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL2, 0);

	TIMER_ChnOutStructInit(&sTIM_ChnOutInit);
	// Выбор источника сигнала для прямого выхода CHxN - сигнал REF
	sTIM_ChnOutInit.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF;
	// Настройна прямого выхода микроконтроллера CHxN на вывод данных
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output;
	// Выбор источника сигнала для инверсного выхода CHxN - сигнал REF
	sTIM_ChnOutInit.TIMER_CH_NegOut_Source = TIMER_CH_OutSrc_REF;
	// Настройна инверсного выхода микроконтроллера CHxN на вывод данных
	sTIM_ChnOutInit.TIMER_CH_NegOut_Mode = TIMER_CH_OutMode_Output;
	sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity = TIMER_CHOPolarity_Inverted;
	// Настраиваем выходы канала 1
	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL1;
	TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);
	// Настраиваем выходы канала 2
	//sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL2;
	//TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

	/* После всех настроек разрешаем работу таймера 1 */
	TIMER_Cmd(MDR_TIMER1,ENABLE);
}

void Init_UART1()
{
	UART_InitTypeDef UART1; 																						//определение переменной для инициализации UART1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);												//Разрешение тактирования UART1
	UART_DeInit(MDR_UART1);
	UART_BRGInit(MDR_UART1, UART_HCLKdiv4);															// Инициализация делителя тактовой частоты для UART1
	NVIC_EnableIRQ(UART1_IRQn);																					// Разрешение прерывания для UART1

	UART1.UART_BaudRate = 115200; 																			//тактовая частота передачи данных
	UART1.UART_WordLength = UART_WordLength8b; 													//длина символов 8 бит
	UART1.UART_StopBits = UART_StopBits1; 															//1 стоп бит
	UART1.UART_Parity = UART_Parity_No; 																//дополнение до четности
	UART1.UART_FIFOMode = UART_FIFO_OFF; 																// выключение FIFO буфера
	UART1.UART_HardwareFlowControl = 
									UART_HardwareFlowControl_RXE|
									UART_HardwareFlowControl_TXE;												// Аппаратный контроль за передачей и приемом 
	
	UART_Init (MDR_UART1, &UART1); 																			//Инициализация UART1
	UART_ITConfig (MDR_UART1, UART_IT_RX, ENABLE);											//Разрешение прерывания по приему
	//UART_ITConfig (MDR_UART1, UART_IT_TX, ENABLE);										//Разрешение прерывания по окончани передачи
	
	UART_Cmd(MDR_UART1, ENABLE); 																				//Разрешение работы UART1
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

