
/*
 * SOSLCfg.h
 *
 * Created: 1/19/2020 9:23:15 AM
 *  Author: ahmad
 */ 
#ifndef SOS_LCFG_H_
#define SOS_LCFG_H_


/******************************************************************************************
*                                                                                         *
*                                        INCLUDES                                         *
*																						  *
*																						  *
*******************************************************************************************/

#include "common_macros.h"
#include "std_types.h"


/******************************************************************************************
*                                                                                         *
*                                        DEFINES                                          *
*																						  *
*																						  *
*******************************************************************************************/



/******************************************************************************************
*                                                                                         *
*                               TYPEDEF                                                   *
*																						  *
*																						  *
*******************************************************************************************/

typedef struct {
	uint8 u8_SOS_TimerID;
	uint8 u8_SOS_SystemTick;
}str_SOS_Cfg_t;


/******************************************************************************************
*                                                                                         *
*                               EXTERN VARIABLES                                          *
*																						  *
*																						  *
*******************************************************************************************/

extern str_SOS_Cfg_t SOS_Info;



/******************************************************************************************
*                                                                                         *
*                                 PROTOTYPES                                              *
*																						  *
*																						  *
*******************************************************************************************/
#endif