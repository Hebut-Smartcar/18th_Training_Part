/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			ADC采集
 * 					使用在线调试，查看adc_data数组的数值，可以得到AD数据。
 ********************************************************************************************************************/

#include "headfile.h"

uint16 adc_data[3];


void main()
{
	DisableGlobalIRQ();						// 关闭总中断
	board_init();							// 全局初始化
	
	adc_init(ADC_P10, ADC_SYSclk_DIV_2);	// 初始化ADC,P1.0通道 ，ADC时钟频率：SYSclk/2
	adc_init(ADC_P11, ADC_SYSclk_DIV_2);	// 初始化ADC,P1.1通道 ，ADC时钟频率：SYSclk/2
	adc_init(ADC_P12, ADC_SYSclk_DIV_2);	// 初始化ADC,P1.2通道 ，ADC时钟频率：SYSclk/2
	
	EnableGlobalIRQ();						// 开启总中断
	
    while(1)
	{
		adc_data[0] = adc_once(ADC_P10, ADC_10BIT);		// 采集一次ADC，精度10位
		adc_data[1] = adc_once(ADC_P11, ADC_9BIT);		// 采集一次ADC，精度9位
		adc_data[2] = adc_once(ADC_P12, ADC_8BIT);		// 采集一次ADC，精度8位
		pca_delay_ms(100);
    }
}











