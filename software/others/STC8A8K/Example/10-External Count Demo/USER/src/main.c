/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			外部计数
 * 					使用在线调试，查看dat的数值，可以得到外部计数数据
					跟编码器重复了，可跳过
 ********************************************************************************************************************/

#include "headfile.h"

int16 dat = 0;

void main()
{
	DisableGlobalIRQ();	//关闭总中断
	board_init();	
	ctimer_count_init(CTIM0_P34);
	EnableGlobalIRQ();	//开启总中断
    while(1)
	{
		//使用在线调试，查看dat的数值，可以得到外部计数数据
		dat = ctimer_count_read(CTIM0_P34);
		ctimer_count_clean(CTIM0_P34);
		pca_delay_ms(100);
    }
}











