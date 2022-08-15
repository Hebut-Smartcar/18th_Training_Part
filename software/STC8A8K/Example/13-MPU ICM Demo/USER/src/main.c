/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		main.c
 * @author     		智能车实验室
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			六轴传感器的使用
 * 					本例程使用软件模拟IIC协议读取传感器数据
 * 					ICM20602还支持软件、硬件SPI通信协议，可以自行尝试，详情查看 SEEKFREE_ICM20602.c 文件
 * 
 * 
 * 本例程软件IIC接线：
 * 		SCL			P25
 * 		SDA			P23
 
 //由于屏幕管脚的P2.5和P2.3使用的硬件SPI，如果P2.5和P2.3引脚被初始化为硬件spi，
//则P2.3 和 P2.5引脚不能作为普通IO口使用，这样MPU6050的模拟IIC通信不能用P2.3和P2.5引脚
//建议使用P1.1和P1.0作为MPU6050的SDA和SCL引脚使用。
 ********************************************************************************************************************/

#include "headfile.h"

//#define USE_MPU		// 使用MPU6050传感器
#define USE_ICM	// 使用ICM20602传感器

extern int16 mpu_gyro_x,mpu_gyro_y,mpu_gyro_z;	// MPU三轴陀螺仪数据
extern int16 mpu_acc_x,mpu_acc_y,mpu_acc_z;		// MPU三轴加速度数据
extern int16 icm_gyro_x,icm_gyro_y,icm_gyro_z;	// ICM三轴陀螺仪数据
extern int16 icm_acc_x,icm_acc_y,icm_acc_z;		// ICM三轴加速度数据

void main()
{
	DisableGlobalIRQ();		// 关闭总中断
	board_init();			// 全局初始化
	simiic_init();			// 模拟IIC初始化

#ifdef USE_MPU				// 使用MPU
	mpu6050_init();			// MPU初始化
#endif

#ifdef USE_ICM				// 使用ICM
	icm20602_init_simiic();	// ICM初始化
#endif
	//lcd_init();				// 屏幕初始化

	EnableGlobalIRQ();		// 开启总中断
    while(1)
	{
	#ifdef USE_MPU
		mpu6050_get_accdata();				// 获取加速度数据
		mpu6050_get_gyro();					// 获取陀螺仪数据
		/*
		lcd_showint16(0, 0, mpu_gyro_x);	// 屏幕显示
		lcd_showint16(0, 1, mpu_gyro_y);
		lcd_showint16(0, 2, mpu_gyro_z);
		lcd_showint16(0, 3, mpu_acc_x);
		lcd_showint16(0, 4, mpu_acc_y);
		lcd_showint16(0, 5, mpu_acc_z);
		*/
	#endif

	#ifdef USE_ICM
		get_icm20602_accdata_simiic();		// 获取加速度数据
		get_icm20602_gyro_simiic();			// 获取陀螺仪数据
		/*
		lcd_showint16(0, 0, icm_gyro_x);	// 屏幕显示
		lcd_showint16(0, 1, icm_gyro_y);
		lcd_showint16(0, 2, icm_gyro_z);
		lcd_showint16(0, 3, icm_acc_x);
		lcd_showint16(0, 4, icm_acc_y);
		lcd_showint16(0, 5, icm_acc_z);		
		*/
	#endif

	pca_delay_ms(500);
    }
}












