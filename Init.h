
#ifndef INIT_H
#define INIT_H

#define CLKSOURSE 		 (1<<2)												//�������� ���������� ������� - ������� ���������
#define TICKINT 			 (1<<1)												//���������� ���������� �� ���������� �������
#define ENABLE_SysTick (1<<0)												//���������� ������ ���������� �������
#define Period				 ((40000000/1000)-1)					//������ ������� - 1 ��

void Init_SysTick();																//������������� ���������� �������
void Init_PortA();																	//������������� ����� A	
void Init_PortD();																	//������������� ����� D	
void Init_PortE();																	//������������� ����� E	
void Init_PortB();																	//������������� ����� B	
void Init_PortC();																	//������������� ����� C
void Init_DAC();																		//������������� ���
void Init_Timer2();																	//������������� Timer2 ��� ����������� ������� �������� ������\�������� �� SPI
void Init_Timer3();
void Init_ADC();
void Init_Timer1();
void Init_UART1();

void InitDriver();																	//������������� ���������� ��������� �������

#endif 
