#ifndef PARAMS_H
#define PARAMS_H

uint8_t adr = 0x11; 					//Адрес устройства

double distance = 50000;			//Расстояние от края до края в оборотах

int MaxVelocity = 20;					// Угловая скорость об/сек

float MaxI = 3.0;							// Ограничение по току (А)

int DeltaLeft = 45;					// Смещение угла для движения влево

int DeltaRight = 15;					// Смещение угла для движения вправо

int phase = 19;								// Сдвиг фазы датчика (число от 0 до 25)

#endif 
