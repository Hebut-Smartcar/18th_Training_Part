/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			串口的printf功能
					打开串口助手，接收文本 涛哥无敌
 ********************************************************************************************************************/

#include "headfile.h"


void main()
{
	DisableGlobalIRQ();		// 关闭总中断
	board_init();			// 全局初始化
	//uart_init(DEBUG_UART,DEBUG_UART_RX_PIN,DEBUG_UART_TX_PIN,DEBUG_UART_BAUD);
	//UART1已在board_init 中初始化，波特率115200

	EnableGlobalIRQ();		// 开启总中断
    while(1)
	{
		printf("涛哥无敌\r\n");
    }
}











