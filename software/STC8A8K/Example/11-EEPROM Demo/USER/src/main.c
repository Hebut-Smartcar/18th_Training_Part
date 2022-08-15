/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897(已满)  三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ790875685)
 * @version    		查看doc内version文件 版本说明
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-4-14
 ********************************************************************************************************************/

#include "headfile.h"


//board.h文件中FOSC的值设置为0,则程序自动识别系统频率

/*board.h文件中FOSC的值设置不为0，则系统频率为FOSC的值，
在使用stc-isp工具下载程序的时候需要将IRC频率设置为FOSC的值*/


uint8 str[] = "SEEKFREK";
uint8 read_str[8];

uint8 num = 0;
uint8 read_num = 0;

void main()
{
	DisableGlobalIRQ();	//关闭总中断
	board_init();		//初始化内部寄存器
	
	iap_init();			//初始化EEPROM
	
	//将"SEEKFREE"这个字符串到EEPROM中，地址(0x00-0x07)，共8个字符
	//该函数可以实现，对于同一个地址进行多次写入，不擦除扇区写入
	extern_iap_write_bytes(0x00, str, 8);
	
	//将EEPROM中的内容读取到read_str字符串中，地址(0x00-0x07)，共8个字符
	iap_read_bytes(0x00, read_str, 8);
	
	EnableGlobalIRQ();	//开启总中断
	
	//发送刚才读取的字符串
	uart_putstr(UART_1, read_str, 8);
	
	
    while(1)
	{
		num++;
		
		//对于同一个地址进行多次写入，先擦除再写入
		//EEPROM擦除目标地址所在的一页（1扇区/512字节）
		iap_erase_page(0x0A);
		//向0x0A中写入1个字节
		iap_write_bytes(0x0A, (uint8 *)&num, 1);
		//读取0x0A中的数据到read_num
		iap_read_bytes(0x0A, &read_num, 1);
		//发送数据到串口助手
		uart_putchar(UART_1, read_num);
		
		pca_delay_ms(1000);
    }
}
