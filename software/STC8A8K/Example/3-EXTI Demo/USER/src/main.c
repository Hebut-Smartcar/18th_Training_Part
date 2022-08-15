/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			外部中断控制LED
					P32接GND后LED点亮
 ********************************************************************************************************************/

#include "headfile.h"

#define LED P52


void main()
{
	DisableGlobalIRQ();			// 关闭总中断
	board_init();				// 全局初始化
	exit_init(INT0_P32, BOTH);	// 外部中断初始化，边沿触发INT0_P32中断
	EnableGlobalIRQ();			// 开启总中断
	
    while(1)
	{

    }
}












