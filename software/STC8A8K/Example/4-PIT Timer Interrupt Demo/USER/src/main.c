/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			定时中断控制LED
					LED每隔一秒闪烁一次
 ********************************************************************************************************************/

#include "headfile.h"

void main()
{
	DisableGlobalIRQ();					// 关闭总中断
	board_init();						// 全局初始化
	pca_init_interrupt_ms(PCA_0, 1);	// 使用PCA_0作为周期中断，时间1ms一次
	EnableGlobalIRQ();					// 开启总中断
	
    while(1)
	{

    }
}












