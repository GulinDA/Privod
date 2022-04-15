#ifndef INTERRUPTHANDLER_H
#define INTERRUPTHANDLER_H



void SysTick_Handler();

void Timer2_IRQHandler();

void Timer3_IRQHandler();

void ADC_IRQHandler();

void Update_Data();

void MoveLeft();

void Update_Data1();

void MoveRight();

void Stop();

void UpdateState();

void UART1_IRQHandler();

void AnalysisReceive();

#endif 
