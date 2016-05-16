#ifndef __BSP_PCF8563_H__
#define __BSP_PCF8563_H__

/*----添加需要包含的头文件----*/
//////////////////////////////////////////////////////////////////
#include "hal_iic1.h"
//////////////////////////////////////////////////////////////////

void BSP_PCF8563_Init_Port(void);
void BSP_PCF8563_Default_Port(void);

bool BSP_PCF8563_Check(void);
bool BSP_PCF8563_ReadTimes(u8 *rtc);
bool BSP_PCF8563_SetTimes(u8 year,u8 month,u8 day,u8 hour,u8 min,u8 sec);


#endif
