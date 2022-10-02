/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			PWM输出
 * 					P25接P52，LED逐渐点亮后熄灭
 ********************************************************************************************************************/

#include "headfile.h"

uint16 duty = 10000;

void main()
{
	DisableGlobalIRQ();					// 关闭总中断
	board_init();						// 全局初始化
	
	pwm_init(PWM5_P25, 10000, duty);	// 初始化PWM5  使用引脚P2.5  输出PWM频率10000HZ   占空比为百分之 pwm_duty/PWM_DUTY_MAX*100
	
	EnableGlobalIRQ();					// 开启总中断
    while(1)
	{
		pwm_duty(PWM5_P25, duty);		// 更新占空比
		
		if(duty <= 0)		// 占空比限幅
		{
			duty = 10000;
		}
		duty -= 50;
		
		pca_delay_ms(10);
    }
}











