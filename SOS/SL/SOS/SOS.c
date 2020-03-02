
/*
 * SOS.c
 *
 * Created: 1/19/2020 9:21:03 AM
 *  Author: ahmad
 */ 
/******************************************************************************************
*                                                                                         *
*                                        INCLUDES                                         *
*																						  *
*																						  *
*******************************************************************************************/
#include "Timer.h"
#include "interrupt.h"
#include "SOS.h"
#include "CPUSleep.h"
#if DEBUGTEST
#include "DIO.h"
#endif



/******************************************************************************************
*                                                                                         *
*                                        DEFINES                                          *
*																						  *
*																						  *
*******************************************************************************************/

#define SOS_INIT		 0U
#define SOS_UNINIT		 1U

#define SOS_STARTED		 0U
#define SOS_NOT_STARTED	 1U

#define SOS_READY	   	 0U
#define SOS_RUN			 1U
#define SOS_WAIT		 2U

/******************************************************************************************
*                                                                                         *
*                               TYPEDEF                                                   *
*																						  *
*																						  *
*******************************************************************************************/


typedef struct {
	uint16 u16_counter;
	uint16 u16_period;
	SOS_TaskPtr_t SOS_Task;
	uint8 u8_priority;
	uint8 u8_state;
}str_SOS_Task_t;

/******************************************************************************************
*                                                                                         *
*                               STATIC VARIABLES                                          *
*																						  *
*																						  *
*******************************************************************************************/


static uint8 gu8_SOS_Status = SOS_UNINIT;
static uint8 gu8_SOS_TIMER_CH;
static uint16 gu16_SOS_TimerCount;
static uint8 gu8_SOS_DispatcherFlag;
static str_SOS_Task_t gastr_SOS_Buffer[SOS_MAX_BUFFER_SIZE];
static uint8 gu8_bufferCounter;
static uint8 gu8_timerStarted;
/******************************************************************************************
*                                                                                         *
*                                 IMPLEMENTATION                                          *
*																						  *
*																						  *
*******************************************************************************************/


void SOS_TimerCbk(void);
static void SOS_PrioritySort(void);
static void SOS_GoToReadyState(void);
static void SOS_GoToRunState(void);
/**
 * Input: 
 * Output:
 * In/Out:			
 * Return: The error status of the function.			
 * Description: This function initializes the SOS module.
 * 							
 */
ERROR_STATUS 
SOS_Init(void)
{
	uint8 u8_retval = SOS_BASE_ERR + SUCCESS;
	if (gu8_SOS_Status == SOS_UNINIT)
	{
		/* TIMER INIT*/
		Timer_cfg_s Timer_info;
		switch(SOS_Info.u8_SOS_TimerID)
		{
			case SOS_CH0:
				Timer_info.Timer_CH_NO = TIMER_CH0;
				gu8_SOS_TIMER_CH = TIMER_CH0;
				break;
			case SOS_CH1:
				Timer_info.Timer_CH_NO = TIMER_CH1;
				gu8_SOS_TIMER_CH = TIMER_CH1;
				break;
			case SOS_CH2:
				Timer_info.Timer_CH_NO = TIMER_CH2;
				gu8_SOS_TIMER_CH = TIMER_CH2;
				break;
		}
		Timer_info.Timer_Mode = TIMER_MODE;
		Timer_info.Timer_Polling_Or_Interrupt = TIMER_INTERRUPT_MODE;
		Timer_info.Timer_Prescaler = TIMER_PRESCALER_1024;
		Timer_info.Timer_Cbk_ptr = SOS_TimerCbk;
		Timer_Init(&Timer_info);
		/* module variables init*/
		gu16_SOS_TimerCount = (SOS_Info.u8_SOS_SystemTick * 16000U) / 1024U;
		gu8_SOS_DispatcherFlag = 0;
		gu8_bufferCounter = 0;
		gu8_timerStarted = SOS_NOT_STARTED;
		gu8_SOS_Status = SOS_INIT;
	}
	else
	{
		u8_retval = SOS_BASE_ERR + ALREADY_INITIALIZED_ERR;
	}
	return u8_retval;
}


/**
 * Input: 
 * Output:
 * In/Out:			
 * Return: The error status of the function.			
 * Description: This function deinitializes the SOS module.
 * 							
 */
ERROR_STATUS 
SOS_Deinit(void)
{
	uint8 u8_retval = SOS_BASE_ERR + SUCCESS;
	if (gu8_SOS_Status == SOS_INIT)
	{
		/* TIMER DEINIT and module variables returning to unused values*/
		Timer_Stop(gu8_SOS_TIMER_CH);
		gu8_timerStarted = SOS_NOT_STARTED;
		gu8_bufferCounter = 0;
		gu8_SOS_DispatcherFlag = 0;
		gu8_SOS_Status = SOS_UNINIT;
	}
	else
	{
		u8_retval = SOS_BASE_ERR + NOT_INITIALIZED_ERR;
	}

	return u8_retval;	
}


/**
 * Input: 
 *		 FUNC_PTR: pointer to task.
 *		 period: periodicity of the task.
 *		 priority: priority of the task.
 * Output:
 * In/Out:			
 * Return: The error status of the function.			
 * Description: This function adds the function to the ready queue.
 * 							
 */
ERROR_STATUS 
SOS_CreateTask(SOS_TaskPtr_t SOS_Task, uint16 u16_period, uint8 u8_priority)
{
	uint8 u8_retval = SOS_BASE_ERR + SUCCESS;
	if (gu8_SOS_Status == SOS_INIT)
	{
		if (NULL != SOS_Task)
		{
			/* adding func to buffer*/
			str_SOS_Task_t SOS_Request = {0, u16_period, SOS_Task, u8_priority, SOS_WAIT};
			gastr_SOS_Buffer[gu8_bufferCounter] = SOS_Request;
			gu8_bufferCounter++;
			/*sorting buffer by priority*/
			SOS_PrioritySort();
		}
		else
		{
			u8_retval = SOS_BASE_ERR + NULL_PTR_ERR;
		}
/*
		if (gu8_timerStarted == SOS_NOT_STARTED)
		{
			/ *getting the module into starting state "happens first time only"* /
			Timer_Start(gu8_SOS_TIMER_CH, gu16_SOS_TimerCount);
			gu8_timerStarted = SOS_STARTED;
		}
		else
		{
			
		}
*/
	}
	else
	{
		u8_retval = SOS_BASE_ERR + NOT_INITIALIZED_ERR;
	}
	return u8_retval;	
}


/**
 * Input: 
 *		 FUNC_PTR: pointer to task.
 * Output:
 * In/Out:			
 * Return: The error status of the function.			
 * Description: This function removes the function from the running queue.
 * 							
 */
ERROR_STATUS 
SOS_DeleteTask(SOS_TaskPtr_t SOS_Task)
{
	uint8 u8_retval = SOS_BASE_ERR + SUCCESS;
	if (gu8_SOS_Status == SOS_INIT)
	{
		if (NULL != SOS_Task)
		{
			uint8 i;
			for (i = 0; i < gu8_bufferCounter; i++)
			{
				if(gastr_SOS_Buffer[i].SOS_Task == SOS_Task)
				{
					/*removing Task from buffer*/
					gastr_SOS_Buffer[i] = gastr_SOS_Buffer[gu8_bufferCounter];
					gu8_bufferCounter--;
					/*sorting buffer by priority*/
					SOS_PrioritySort();
					break;
				}
				else
				{
					
				}
			}
		}
		else
		{
			u8_retval = SOS_BASE_ERR + NULL_PTR_ERR;
		}
/*
		if (gu8_bufferCounter == 0 && u8_retval == SOS_BASE_ERR + SUCCESS)
		{
			/ * getting module into stop state "happens when buffer is empty only"* /
			Timer_Stop(gu8_SOS_TIMER_CH);
			gu8_timerStarted = SOS_NOT_STARTED;
		}
		else
		{
			
		}
*/
	}
	else
	{
		u8_retval = SOS_BASE_ERR + NOT_INITIALIZED_ERR;
	}
	return u8_retval;	
}


/**
 * Input: 
 * Output:
 * In/Out:			
 * Return: The error status of the function.			
 * Description: This function performs the operation of the OS.
 * 							
 */
ERROR_STATUS 
SOS_Run(void)
{
	uint8 u8_retval = SOS_BASE_ERR + SUCCESS;
	if (SOS_INIT == gu8_SOS_Status)
	{
		Timer_Start(gu8_SOS_TIMER_CH, gu16_SOS_TimerCount);
		gu8_timerStarted = SOS_STARTED;
		SOS_PrioritySort();
		while (1)
		{
			#if DEBUGTEST
			DIO_Toggle(GPIOB, PIN6);
			#endif
			
			if (1 == gu8_SOS_DispatcherFlag)
			{
				/*checking if any task is ready*/
				SOS_GoToReadyState();
				/*running ready tasks*/
				SOS_GoToRunState();
				gu8_SOS_DispatcherFlag = 0;
			} 
			else
			{
			
			}
			#if DEBUGTEST
			DIO_Toggle(GPIOB, PIN6);
			#endif
			#if DEBUGTEST
			DIO_Toggle(GPIOB, PIN7);
			#endif
			/*putting cpu to sleep*/
			CPUSleep_enable();
			#if DEBUGTEST
			DIO_Toggle(GPIOB, PIN7);
			#endif
			
		}
	} 
	else
	{
		u8_retval = SOS_BASE_ERR + NOT_INITIALIZED_ERR;
	}
	return u8_retval;
	
	
}




void SOS_TimerCbk(void)
{
	/* resetting the timer */
	Timer_Start(gu8_SOS_TIMER_CH, gu16_SOS_TimerCount);
	gu8_SOS_DispatcherFlag = 1;	
}

static void SOS_PrioritySort(void)
{
   uint8 i, j;
   for (i = 0; i < gu8_bufferCounter - 1; i++)
   {
	   for (j = 0; j < gu8_bufferCounter - i - 1; j++)
	   {
		   if (gastr_SOS_Buffer[j].u8_priority > gastr_SOS_Buffer[j+1].u8_priority)
		   {
				str_SOS_Task_t str_SOS_TempTask = gastr_SOS_Buffer[j];
				gastr_SOS_Buffer[j] = gastr_SOS_Buffer[j+1];
				gastr_SOS_Buffer[j+1] = str_SOS_TempTask;
		   }
		   else
		   {

		   }
	   }
	   
   }

	
}

static void SOS_GoToReadyState(void)
{
	uint8 u8_index;
	for (u8_index = 0; u8_index < gu8_bufferCounter; u8_index++)
	{
		/* incrementing Task counter */
		gastr_SOS_Buffer[u8_index].u16_counter++;
		if (gastr_SOS_Buffer[u8_index].u16_counter == gastr_SOS_Buffer[u8_index].u16_period)
		{
			/*setting the state for ready tasks*/
			gastr_SOS_Buffer[u8_index].u8_state = SOS_READY;
			gastr_SOS_Buffer[u8_index].u16_counter = 0;
		}
		else
		{
						
		}
	}	
}

static void SOS_GoToRunState(void)
{
	uint8 u8_index;
	for (u8_index = 0; u8_index < gu8_bufferCounter; u8_index++)
	{
		if (gastr_SOS_Buffer[u8_index].u8_state == SOS_READY)
		{
			/* excuting task with highest priority*/
			gastr_SOS_Buffer[u8_index].u8_state = SOS_RUN;
			gastr_SOS_Buffer[u8_index].SOS_Task();
			gastr_SOS_Buffer[u8_index].u8_state = SOS_WAIT;
		}
		else
		{
						
		}
	}
}