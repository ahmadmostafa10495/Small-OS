/*
 * SOS.c
 *
 * Created: 1/19/2020 9:04:39 AM
 *  Author: ahmad
 */ 
#include "DIO.h"
#include "SOS.h"
#include "CPUSleep.h"
#include "interrupt.h"

void LED0(void)
{
	DIO_Toggle(GPIOB, PIN0);
}
void LED1(void)
{
	DIO_Toggle(GPIOB, PIN1);
}
void LED2(void)
{
	DIO_Toggle(GPIOB, PIN2);
}
void LED3(void)
{
	DIO_Toggle(GPIOB, PIN3);
}
void LED4(void)
{
	DIO_Toggle(GPIOB, PIN4);
}
void LED5(void)
{
	DIO_Toggle(GPIOB, PIN5);
}
void LED6(void)
{
	DIO_Toggle(GPIOB, PIN6);
}
void LED7(void)
{
	DIO_Toggle(GPIOB, PIN7);
}

int main(void)
{
	sei();
	DIO_Cfg_s DIO_Info = {GPIOB, FULL_PORT, OUTPUT};
	DIO_init(&DIO_Info);
	DIO_Write(GPIOB, FULL_PORT, LOW);
	SOS_Init();
	SOS_CreateTask(LED4, 5000, 5);
	SOS_CreateTask(LED5, 6000, 6);
	SOS_CreateTask(LED6, 7000, 7);
	SOS_CreateTask(LED7, 8000, 8);
	SOS_CreateTask(LED0, 1000, 1);
	SOS_CreateTask(LED1, 2000, 2);
	SOS_CreateTask(LED2, 3000, 3);
	SOS_CreateTask(LED3, 4000, 4);

	CPUSleep_Init();
	CPUSleep_setMode(CPUSLEEP_IDLE_MODE);
	SOS_Run();
}