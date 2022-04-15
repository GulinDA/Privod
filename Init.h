
#ifndef INIT_H
#define INIT_H

#define CLKSOURSE 		 (1<<2)												//источник системного таймера - внешний резонатор
#define TICKINT 			 (1<<1)												//разрешение прерывани€ от системного таймера
#define ENABLE_SysTick (1<<0)												//разрешение работы системного таймера
#define Period				 ((40000000/1000)-1)					//период таймера - 1 мс

void Init_SysTick();																//инициализаци€ системного таймера
void Init_PortA();																	//инициализаци€ порта A	
void Init_PortD();																	//инициализаци€ порта D	
void Init_PortE();																	//инициализаци€ порта E	
void Init_PortB();																	//инициализаци€ порта B	
void Init_PortC();																	//инициализаци€ порта C
void Init_DAC();																		//»нициализаци€ ÷јѕ
void Init_Timer2();																	//инициализаци€ Timer2 дл€ ограничени€ времени ожидани€ приема\передачи по SPI
void Init_Timer3();
void Init_ADC();
void Init_Timer1();
void Init_UART1();

void InitDriver();																	//»нициализаци€ начального положени€ привода

#endif 
