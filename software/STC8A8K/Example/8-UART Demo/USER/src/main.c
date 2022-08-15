/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			UART串口使用
					打开串口助手，每0.1s接收到一个十六进制数0x1E
 ********************************************************************************************************************/

#include "headfile.h"

void main()
{
	DisableGlobalIRQ();		// 关闭总中断
	board_init();			// 全局初始化	
	//uart_init(DEBUG_UART,DEBUG_UART_BAUD,DEBUG_UART_RX_TX_PIN);
	//UART1已在board_init 中初始化，波特率115200

	EnableGlobalIRQ();		// 开启总中断
    while(1)
	{
		uart_putchar(DEBUG_UART,0x1E);	// 串口发送数据
		pca_delay_ms(100);
    }
}











