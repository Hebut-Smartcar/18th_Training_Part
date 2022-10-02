/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			屏幕的使用
 * 
 * 
 * 1.8寸TFT屏幕：	
	模块管脚            单片机管脚
	SCL                 P25     硬件SPI引脚不可随意切换
	SDA                 P23     硬件SPI引脚不可随意切换
	RES                 P20    
	DC                  P21  
	CS                  P22     
						
	电源引脚
	BL  3.3V电源（背光控制引脚，也可以接PWM来控制亮度）
	VCC 3.3V电源
	GND 电源地
	最大分辨率128*160 

 * 0.96寸OLED屏幕：
 	OLED液晶    单片机                        
    D0          P25
    D1          P23            
    RES         P20    
    DC          P21
	CS          P22	
 ********************************************************************************************************************/

#include "headfile.h"

//#define USE_LCD		// 如果使用1.8寸液晶屏需要此行代码
#define USE_OLED	// 如果使用0.96寸OLED屏需要此行代码

void main()
{
	uint16 test_u16 = 255;

	DisableGlobalIRQ();		// 关闭总中断
	board_init();			// 全局初始化

#ifdef USE_LCD							// 使用1.8寸液晶屏
	lcd_init();							// 液晶初始化
	lcd_showstr(0, 0, "SmartCar");		// 显示字符串
	lcd_showuint8(0, 1, uint8(test_u8));		// 显示uint8类型数字
#endif

#ifdef USE_OLED							// 使用0.96OLED屏幕
	oled_init();						// OLED初始化
	oled_p6x8str(0, 0, "SmartCar");		// 显示字符串
	oled_uint16(0, 1, test_u16);	// 显示uint16类型数字
#endif

	EnableGlobalIRQ();		// 开启总中断
    while(1)
	{
		
    }
}












