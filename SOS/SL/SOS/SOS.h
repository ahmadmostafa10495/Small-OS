
/*
 * SOS.h
 *
 * Created: 1/19/2020 9:21:19 AM
 *  Author: ahmad
 */ 
#ifndef SOS_H_
#define SOS_H_



/******************************************************************************************
*                                                                                         *
*                                        INCLUDES                                         *
*																						  *
*																						  *
*******************************************************************************************/

#include "SOS_Cfg.h"
#include "SOS_LCfg.h"

/******************************************************************************************
*                                                                                         *
*                                        DEFINES                                          *
*																						  *
*																						  *
*******************************************************************************************/
#define		SOS_CH0					0
#define		SOS_CH1					1
#define		SOS_CH2					2


/******************************************************************************************
*                                                                                         *
*                               TYPEDEF                                                   *
*																						  *
*																						  *
*******************************************************************************************/

typedef void(*SOS_TaskPtr_t)(void);


/******************************************************************************************
*                                                                                         *
*                               STATIC VARIABLES                                          *
*																						  *
*																						  *
*******************************************************************************************/





/******************************************************************************************
*                                                                                         *
*                                 PROTOTYPES                                              *
*																						  *
*																						  *
*******************************************************************************************/




/**
 * Input: 
 * Output:
 * In/Out:			
 * Return: The error status of the function.			
 * Description: This function initializes the SOS module.
 * 							
 */
ERROR_STATUS SOS_Init(void);


/**
 * Input: 
 * Output:
 * In/Out:			
 * Return: The error status of the function.			
 * Description: This function deinitializes the SOS module.
 * 							
 */
ERROR_STATUS SOS_Deinit(void);


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
ERROR_STATUS SOS_CreateTask(SOS_TaskPtr_t SOS_Task, uint16 u16_period, uint8 u8_priority);


/**
 * Input: 
 *		 FUNC_PTR: pointer to function.
 * Output:
 * In/Out:			
 * Return: The error status of the function.			
 * Description: This function removes the function from the running queue.
 * 							
 */
ERROR_STATUS SOS_DeleteTask(SOS_TaskPtr_t SOS_Task);


/**
 * Input: 
 * Output:
 * In/Out:			
 * Return: The error status of the function.			
 * Description: This function performs the operation of the OS.
 * 							
 */
ERROR_STATUS SOS_Run(void);
#endif