#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_timer.h>
#include <MDR32F9Qx_uart.h>
#include <MDR32F9Qx_dac.h>
#include <MDR32F9Qx_adc.h>

#include <stdbool.h>
#include <math.h>

#include "Interrupts.h"

#include "Init.h"

#include "params.h"


volatile uint32_t timeDelay; 

uint8_t UART_Receive[7];
int counter_receive = 0;
//uint8_t adr = 0x11;


uint8_t UART_Transmit[8];
int counter_transmit = 0;
uint8_t reg_state = 0x00;
bool trans8 = 0;

int counter_sin = 0;
int counter_adc = 1;
bool ADC_ready = 1;
float sinF[5] = {0, 0, 0, 0, 0};
int counter_sinF = 0;
float cosF[5] = {0, 0, 0, 0, 0};
int counter_cosF = 0;
float IF[5] = {0, 0, 0, 0, 0};
int counter_IF = 0;
double Sin = 0;
double Cos = 0;
float I = 0;
double Angle1 = -180;
double Angle;
double counter_LED = 0;
int velocity = 0;
int temp_velocity = 0;
bool en_v = 0;
//double distance = 500;
double temp_distance;
double current_distance = 0;

int State = 0;
int MaxPWM = 10;

extern bool InitD;
extern bool InitEnd;

float table_sin[25] = {0, 0.0698, 0.1219, 0.1908, 0.2419, 
								 0.3090, 0.3584, 0.4226, 0.4695, 0.5299, 
								 0.5736, 0.6293, 0.6691, 0.7193, 0.7547, 
								 0.7986, 0.8387, 0.8660, 0.8988, 0.9205, 
								 0.9455, 0.9613, 0.9781, 0.9877, 0.9962};
 
void SysTick_Handler()																						//прерывание от системного таймера каждую 1 мс
{
	Update_Data();
}

void Timer3_IRQHandler()
{
	if(TIMER_GetITStatus(MDR_TIMER3, TIMER_STATUS_CNT_ZERO))
	{																							
		TIMER_Cmd(MDR_TIMER3, DISABLE);																//Остановка таймера 2
		if (counter_sin < 25)
			DAC2_SetData(2048+2048*table_sin[counter_sin]);
		if (counter_sin > 24 && counter_sin < 50)
			DAC2_SetData(2048+2048*table_sin[49-counter_sin]);
		if (counter_sin > 49 && counter_sin < 75)
			DAC2_SetData(2048-2048*table_sin[counter_sin-50]);
		if (counter_sin > 74 && counter_sin < 100)
			DAC2_SetData(2048-2048*table_sin[99-counter_sin]);	
		if (counter_sin == (phase*4) && ADC_ready)
		{
			ADC_ready = 0;
			ADC1_Start();
		}
		counter_sin=counter_sin+4;
		if (counter_sin == 100)
			counter_sin = 0;
		counter_LED++;
		if (counter_LED == 40000)
		{
			counter_LED = 0;
			if (PORT_ReadInputDataBit(MDR_PORTA, PORT_Pin_7))
				PORT_ResetBits(MDR_PORTA, PORT_Pin_7);
			else
				PORT_SetBits(MDR_PORTA, PORT_Pin_7);
			velocity = temp_velocity;
			temp_velocity = 0;
				if(velocity<MaxVelocity)
					MaxPWM++;
				if(velocity>MaxVelocity)
					MaxPWM--;
				if (MaxPWM < 10)
					MaxPWM = 10;
		}
	
		TIMER_SetCounter(MDR_TIMER3, 5);															//Установка значения таймера 2
		TIMER_ClearITPendingBit(MDR_TIMER3, TIMER_STATUS_CNT_ZERO);		//сброс флага прерывания таймера		
		TIMER_Cmd(MDR_TIMER3, ENABLE);																//запуск таймера 2
	}
}

void Timer2_IRQHandler()
{
	if(TIMER_GetITStatus(MDR_TIMER2, TIMER_STATUS_CNT_ZERO))
	{																							
		TIMER_Cmd(MDR_TIMER2, DISABLE);																//Остановка таймера 3
		if (PORT_ReadInputDataBit(MDR_PORTA, PORT_Pin_7))
			PORT_ResetBits(MDR_PORTA, PORT_Pin_7);
		else
			PORT_SetBits(MDR_PORTA, PORT_Pin_7);
		TIMER_SetCounter(MDR_TIMER2, 500);														//Установка значения таймера 3
		TIMER_ClearITPendingBit(MDR_TIMER2, TIMER_STATUS_CNT_ZERO);		//сброс флага прерывания таймера		
		TIMER_Cmd(MDR_TIMER2, ENABLE);																//запуск таймера 3
	}
}

void ADC_IRQHandler()
{
	if(ADC_GetITStatus(ADC1_IT_END_OF_CONVERSION))
	{
		int ADCResult;
		ADCResult = ADC1_GetResult();
		ADCResult &= 0x00FFF;
			switch (counter_adc)
			{
				case 1:
					sinF[counter_sinF] = (ADCResult*3.3/4095)-1.85;
					counter_sinF++;
					if (counter_sinF == 5)
						counter_sinF = 0;
					ADC1_SetChannel(ADC_CH_ADC2);
					break;
				case 2:
					cosF[counter_cosF] = (ADCResult*3.3/4095)-1.85;
					counter_cosF++;
					if (counter_cosF == 5)
						counter_cosF = 0;
					ADC1_SetChannel(ADC_CH_ADC0);
					break;
				case 3:
					IF[counter_IF] = (ADCResult*3.3/4095)/0.4;
					counter_IF++;
					if (counter_IF == 5)
						counter_IF = 0;
					ADC1_SetChannel(ADC_CH_ADC1);
					break;
				default:
					ADC1_SetChannel(ADC_CH_ADC1);
					break;
			}
		counter_adc++;
		if (counter_adc == 4)
			counter_adc = 1;
		ADC_ready = 1;
		NVIC_ClearPendingIRQ(ADC_IRQn);																//Сброс флага прерывания АЦП
	}
}

void Update_Data()
{
	int i;
	float tempS = 0;
	float tempC = 0;
	float tempI = 0;
	for (i = 0; i < 5; i++)
	{
		tempS += sinF[i];
		tempC += cosF[i];
		tempI += IF[i];
	}
	Sin = tempS/5;
	Cos = tempC/5;
	I = tempI/5;
	Angle = atan2(Sin, Cos)/3.1416*180;
	if (Angle < -180)
		Angle = 360+Angle;
	if (Angle > 180)
		Angle = -360+Angle;
	if (I > MaxI)
	{
		if (InitD)
		{
			InitD = 0;
			distance = -temp_distance;
			current_distance = 0;
			temp_distance = 0;
		}
		reg_state|=0x02;
		State = 0;
	}
	
	if (State != 0)
	{
		reg_state|=0x08;
		reg_state&=0xFD;
	}
	
	if (InitEnd)
	{
		current_distance = current_distance + temp_distance;
		temp_distance = 0;
		if (State == 1 && current_distance > (distance-1))
			State = 0;
		if (State == 2 && current_distance < 1)
			State = 0;
	}
	if (current_distance <1 || current_distance > (distance-1))
		reg_state|=0x01;
	else
		reg_state&=0xFE;
}

void MoveLeft()
{
	int quarter = 0;
	double tempAngle;
	tempAngle = Angle+DeltaLeft;
	if (tempAngle > 180)
		tempAngle = -360+tempAngle;
	if (tempAngle < -180)
		tempAngle = 360+tempAngle;
	if (tempAngle > 45 && tempAngle <135)
		quarter = 1;
	if (tempAngle > 135 || tempAngle <-135)
		{
		en_v = 1;
		quarter = 2;
		}
	if (tempAngle > -135 && tempAngle <-45)
		quarter = 3;
	if (tempAngle > -45 && tempAngle <45)
		quarter = 4;
	tempAngle = tempAngle*3.1416/180;
	PORT_ResetBits(MDR_PORTE, PORT_Pin_3);
	switch (quarter)
	{
		case 1:
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);
			PORT_SetBits(MDR_PORTA, PORT_Pin_1);
			PORT_SetBits(MDR_PORTA, PORT_Pin_3);
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);//MaxPWM*cos(tempAngle));
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100-MaxPWM);//*sin(tempAngle)));
			break;
		case 2:
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);		
			PORT_SetBits(MDR_PORTA, PORT_Pin_1);
			PORT_ResetBits(MDR_PORTA, PORT_Pin_3);
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, MaxPWM);//*cos(3.1416+tempAngle));
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);//100 - (MaxPWM*sin(tempAngle)));
			break;
		case 3:
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);
			PORT_ResetBits(MDR_PORTA, PORT_Pin_1);
			PORT_ResetBits(MDR_PORTA, PORT_Pin_3);
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);//MaxPWM*cos(3.1416+tempAngle));
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100-MaxPWM);//*sin(3.1416+tempAngle)));
			break;
		case 4:
			if (en_v)
			{
				en_v = 0;
				temp_velocity++;
				temp_distance++;
			}
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);
			PORT_ResetBits(MDR_PORTA, PORT_Pin_1);
			PORT_SetBits(MDR_PORTA, PORT_Pin_3);
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, MaxPWM);//*cos(tempAngle));
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);// - (MaxPWM*sin(3.1416+tempAngle)));
			break;
		default:
			break;
	}
}

void Update_Data1()
{
	Angle1= Angle1+1;
	if (Angle1 > 180)
		Angle1 = -180;
}

void MoveRight()
{
	int quarter = 0;
	double tempAngle;
	tempAngle = Angle+DeltaRight;
	if (tempAngle > 180)
		tempAngle = -360+tempAngle;
	if (tempAngle < -180)
		tempAngle = 360+tempAngle;
	if (tempAngle > -45 && tempAngle <45)
		quarter = 1;
	if (tempAngle > -135 && tempAngle <-45)
		{
		en_v = 1;
		quarter = 2;
		}
	if (tempAngle > 135 || tempAngle <-135)
		quarter = 3;
	if (tempAngle > 45 && tempAngle <135)
		quarter = 4;
	tempAngle = tempAngle*3.1416/180;
	PORT_ResetBits(MDR_PORTE, PORT_Pin_3);
	switch (quarter)
	{
		case 1:
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);
			PORT_SetBits(MDR_PORTA, PORT_Pin_1);
			PORT_SetBits(MDR_PORTA, PORT_Pin_3);
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, MaxPWM);//*cos(tempAngle));
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);//-(MaxPWM*sin(tempAngle)));
			break;
		case 2:
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);	
			PORT_ResetBits(MDR_PORTA, PORT_Pin_1);
			PORT_SetBits(MDR_PORTA, PORT_Pin_3);
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);//MaxPWM*cos(3.1416+tempAngle));
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100-MaxPWM);//*sin(tempAngle)));
			break;
		case 3:
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);
			PORT_ResetBits(MDR_PORTA, PORT_Pin_1);
			PORT_ResetBits(MDR_PORTA, PORT_Pin_3);
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, MaxPWM);//*cos(3.1416+tempAngle));
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);// - (MaxPWM*sin(3.1416+tempAngle)));
			break;
		case 4:
			if (en_v)
			{
				en_v = 0;
				temp_velocity++;
				temp_distance--;
			}
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);
			PORT_SetBits(MDR_PORTA, PORT_Pin_1);
			PORT_ResetBits(MDR_PORTA, PORT_Pin_3);
			TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);//*cos(tempAngle));
			TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100-MaxPWM);// - MaxPWM*sin(3.1416+tempAngle)));
			break;
		default:
			break;
	}
}

void Stop()
{
		uint8_t temp;
		TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, 0);//*cos(tempAngle));
		TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL2, 100);//*sin(3.1416+tempAngle)));
		MaxPWM = 10;
		temp_distance = 0;
		temp = reg_state&0x03;
		reg_state=temp&0x03;
}

void UpdateState()
{
	switch (State)
	{
		case 0:
			Stop();
			break;
		case 1:
			MoveLeft();
			break;
		case 2:
			MoveRight();
			break;
		default:
			Stop();
	}
}

void UART1_IRQHandler()
{
	uint16_t dataUART;
	uint8_t temp_CSN;
	if (UART_GetITStatusMasked(MDR_UART1, UART_IT_RX) == SET)						//проверка установки флага прерывания по окончании приема данных
	{
		dataUART = MDR_UART1->DR;																					//получение принятого байта
		if (dataUART & 0xFF00)																						//определение наличия ошибок
		{
			dataUART = 0x0000;
		}
		UART_Receive[counter_receive] = dataUART;													//Запись в буфер
		counter_receive++;																								//Увеличение счетчика
		switch (counter_receive)
		{
			case 1:
				if (UART_Receive[0] == 0xAA)
					break;
				else
				{
					counter_receive = 0;
					break;
				}
			case 2:
				if (UART_Receive[1] == 0xAA)
					break;
				else
				{
					UART_Receive[0] = 0x00;
					UART_Receive[1] = 0x00;
					counter_receive = 0;
					break;
				}
			case 3:
				if (UART_Receive[2] == adr)
					break;
				else
				{
					UART_Receive[0] = 0x00;
					UART_Receive[1] = 0x00;
					UART_Receive[2] = 0x00;
					counter_receive = 0;
					break;
				}				
			case 4:
				if ((UART_Receive[3] < 0xF0)||(UART_Receive[3] > 0xF3))
				{
					UART_Receive[0] = 0x00;
					UART_Receive[1] = 0x00;
					UART_Receive[2] = 0x00;
					UART_Receive[3] = 0x00;
					counter_receive = 0;
					break;
				}
				else
					break;
			case 5:
				temp_CSN = 0;
				temp_CSN = ~(UART_Receive[2]+UART_Receive[3]);
				if (UART_Receive[4] == temp_CSN)
					break;
				else
				{
					UART_Receive[0] = 0x00;
					UART_Receive[1] = 0x00;
					UART_Receive[2] = 0x00;
					UART_Receive[3] = 0x00;
					UART_Receive[4] = 0x00;
					counter_receive = 0;
					break;
				}
			case 7:
				if ((UART_Receive[5] == 0x55)&&(UART_Receive[6] == 0x55))
					break;
				else
				{
					UART_Receive[0] = 0x00;
					UART_Receive[1] = 0x00;
					counter_receive = 0;
					break;
				}
			default:
				break;
		}
		UART_ClearITPendingBit(MDR_UART1, UART_IT_RX);										//очистка флага прерывания	
		if (counter_receive == 7)
		{
			PORT_SetBits(MDR_PORTC, PORT_Pin_0);
			UART_ITConfig (MDR_UART1, UART_IT_RX, DISABLE);
			UART_ITConfig (MDR_UART1, UART_IT_TX, ENABLE);
			AnalysisReceive();
		}
	}
	if (UART_GetITStatusMasked(MDR_UART1, UART_IT_TX) == SET)						//проверка установки флага прерывания по окончании передачи данных
	{
		counter_transmit++;
		if (counter_transmit > 6)
		{
			if (trans8)
			{
				trans8 = 0;
				UART_SendData(MDR_UART1, UART_Transmit[counter_transmit]);
			}
			else
			{
				while (UART_GetFlagStatus(MDR_UART1, UART_FLAG_BUSY)){}
				counter_receive = 0;
				PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
				UART_ITConfig (MDR_UART1, UART_IT_RX, ENABLE);
				UART_ITConfig (MDR_UART1, UART_IT_TX, DISABLE);
			}
		}
		else
		{
			UART_SendData(MDR_UART1, UART_Transmit[counter_transmit]);
		}
		UART_ClearITPendingBit(MDR_UART1, UART_IT_TX);											//очистка флага прерывания
	}
}

void AnalysisReceive()
{
	int i;
	uint8_t temp_CSN;
	for (i = 0; i < 7; i++)
		UART_Transmit[i] = UART_Receive[i];
	trans8 = 0;
	switch (UART_Receive[3])
	{
		case 0xF0:
			if (State != 1)
				Stop();
			State = 1;
			break;
		case 0xF1:
			if (State != 2)
				Stop();
			State = 2;
			break;
		case 0xF2:
			State = 0;
			break;
		case 0xF3:
			trans8 = 1;
			UART_Transmit[4] = reg_state;
			temp_CSN = 0;
			temp_CSN = ~(UART_Transmit[2]+UART_Transmit[3]+UART_Transmit[4]);
			UART_Transmit[5] = temp_CSN;
			UART_Transmit[6] = 0x55;
			UART_Transmit[7] = 0x55;
			break;
		default:
			break;
	}
	counter_transmit = 0;
	UART_SendData(MDR_UART1, UART_Transmit[0]);
}

