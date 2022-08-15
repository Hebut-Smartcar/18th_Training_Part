/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			编码器解码
 * 					使用在线调试，查看dat的数值，可以得到编码器数据
 ********************************************************************************************************************/

#include "headfile.h"

#define DIR P35
int16 dat = 0;

void main()
{
	DisableGlobalIRQ();				// 关闭总中断
	board_init();					// 全局初始化
	ctimer_count_init(CTIM0_P34);	// 定时器初始化作为外部计数，用于编码器数据读取
	EnableGlobalIRQ();				// 开启总中断
    while(1)
	{
		if(DIR == 1)					// 获取方向
		{
			dat = ctimer_count_read(CTIM0_P34);
		}
		else
		{
			dat = -ctimer_count_read(CTIM0_P34);
		}
		ctimer_count_clean(CTIM0_P34);	// 计数清零
		pca_delay_ms(100);
    }
}











