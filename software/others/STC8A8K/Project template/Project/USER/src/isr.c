/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897(已满)  三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		isr
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ790875685)
 * @version    		查看doc内version文件 版本说明
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-4-14
 ********************************************************************************************************************/
#include "headfile.h"





//UART1中断
void UartIsr() interrupt 4
{
    uint8 res;
	static uint8 dwon_count;
    if(UART1_GET_TX_FLAG)
    {
        UART1_CLEAR_TX_FLAG;
        busy[1] = 0;
    }
    if(UART1_GET_RX_FLAG)
    {
        UART1_CLEAR_RX_FLAG;
		//接收数据寄存器为：SBUF
        res = SBUF;
        if(res == 0x7F)
        {
            if(dwon_count++ > 20)
				//代码软复位到ISP监控程序区，实现自动下载
                IAP_CONTR = 0x60;
        }
        else
        {
            dwon_count = 0;
        }
		
    }
}

//UART2中断
void Uart2Isr() interrupt 8
{
    if(UART2_GET_TX_FLAG)
	{
        UART2_CLEAR_TX_FLAG;
		busy[2] = 0;
	}
    if(UART2_GET_RX_FLAG)
	{
        UART2_CLEAR_RX_FLAG;
		//接收数据寄存器为：S2BUF

	}
}


//UART3中断
void Uart3Isr() interrupt 17
{
    if(UART3_GET_TX_FLAG)
	{
        UART3_CLEAR_TX_FLAG;
		busy[3] = 0;
	}
    if(UART3_GET_RX_FLAG)
	{
        UART3_CLEAR_RX_FLAG;
		//接收数据寄存器为：S3BUF

	}
}


//UART4中断

void Uart4Isr() interrupt 18
{
    if(UART4_GET_TX_FLAG)
	{
        UART4_CLEAR_TX_FLAG;
		busy[4] = 0;
	}
    if(UART4_GET_RX_FLAG)
	{
        UART4_CLEAR_RX_FLAG;
		wireless_uart_callback();
		//接收数据寄存器为：S4BUF;
		
		wireless_uart_callback();//无线转串口回调函数
	}
}


//PCA周期定时器中断
void PCA_Isr() interrupt 7
{
	if(PCA0_GET_FLAG)
	{
		//清除中断标志位
		PCA_CLEAR_FLAG(PCA_0);	

		//重载计数器
		pca_reload_counter(PCA_0);
		
		//线性CCD采集数据
		ccd_collect();

		
		
	}
	
	if(PCA1_GET_FLAG)
	{
		//清除中断标志位
		PCA_CLEAR_FLAG(PCA_1);	
		//重载计数器
		pca_reload_counter(PCA_1);

	}
	


	if(PCA2_GET_FLAG)
	{
		//清除中断标志位
		PCA_CLEAR_FLAG(PCA_2);	
		//重载计数器
		pca_reload_counter(PCA_2);

	}
	
//	PCA_3已做延时
	if(PCA3_GET_FLAG)
	{
		//清除中断标志位
		PCA_CLEAR_FLAG(PCA_3);	
		
//		//重载计数器
//		pca_reload_counter(PCA_3);
		 
	}
	
}



void INT0_Isr() interrupt 0
{
	//此标志位，硬件自动清零。
	//INT0_CLEAR_FLAG;

}
void INT1_Isr() interrupt 2
{
	//此标志位，硬件自动清零。
	//INT1_CLEAR_FLAG;

}
void INT2_Isr() interrupt 10
{
	INT2_CLEAR_FLAG;
	
}
void INT3_Isr() interrupt 11
{
	INT3_CLEAR_FLAG;
	
}

void INT4_Isr() interrupt 16
{
	INT4_CLEAR_FLAG;
	
}

void TM0_Isr() interrupt 1
{

}
void TM1_Isr() interrupt 3
{

}
void TM2_Isr() interrupt 12
{
	TIM2_CLEAR_FLAG;  //清除中断标志
	
}
void TM3_Isr() interrupt 19
{
	TIM3_CLEAR_FLAG; //清除中断标志
	
}

void TM4_Isr() interrupt 20
{
	TIM4_CLEAR_FLAG; //清除中断标志

}


//void INT0_Routine(void) interrupt 0
//void TM0_Rountine(void) interrupt 1
//void INT1_Routine(void) interrupt 2
//void TM1_Rountine(void) interrupt 3
//void UART1_Routine(void) interrupt 4
//void ADC_Routine(void) interrupt 5
//void LVD_Routine(void) interrupt 6
//void PCA_Routine(void) interrupt 7
//void UART2_Routine(void) interrupt 8
//void SPI_Routine(void) interrupt 9
//void INT2_Routine(void) interrupt 10
//void INT3_Routine(void) interrupt 11
//void TM2_Routine(void) interrupt 12
//void INT4_Routine(void) interrupt 16
//void UART3_Routine(void) interrupt 17
//void UART4_Routine(void) interrupt 18
//void TM3_Routine(void) interrupt 19
//void TM4_Routine(void) interrupt 20
//void CMP_Routine(void) interrupt 21
//void PWM_Routine(void) interrupt 22
//void PWMFD_Routine(void) interrupt 23
//void I2C_Routine(void) interrupt 24
