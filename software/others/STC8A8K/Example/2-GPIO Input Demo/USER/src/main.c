/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			通过IO输入控制LED亮灭
 * 					将P04连接GND,可以看到LED 灭掉
 ********************************************************************************************************************/

#include "headfile.h"

#define LED P52


void main()
{
	DisableGlobalIRQ();		// 关闭总中断
	board_init();			// 全局初始化
	EnableGlobalIRQ();		// 开启总中断
	
    while(1)
	{
		if(P04 == 1)		// P04输入高电平	
			LED = 0;
		else
			LED = 1;
    }
}












