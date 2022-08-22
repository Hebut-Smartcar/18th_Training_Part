/*********************************************************************************************************************
 * -*-encoding: GB2312
 * 
 * @file       		isr.c
 * @author     		���ܳ�ʵ����
 * @version    		v1.0
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @date       		2020-9-21
 * @note			�ⲿ�жϿ���LED
 * 					��������غ����� void INT0_Isr() interrupt 0
 ********************************************************************************************************************/

#include "headfile.h"

//UART1�ж�
void UartIsr() interrupt 4
{
    uint8 res;
	static uint8 dwon_count;
    if(TI)
    {
        TI = 0;
        busy[1] = 0;
    }
    if (RI)
    {
        RI = 0;
        res = SBUF;
        //�����Զ�����
        if(res == 0x7F || res == 0x80)
        {
            if(dwon_count++ > 20)
                IAP_CONTR = 0x60;
        }
        else
        {
            dwon_count = 0;
        }
    }
}

//UART2�ж�
void Uart2Isr() interrupt 8
{
	if (S2CON & 0x02)
	{
		S2CON &= ~0x02;
		busy[2] = 0;
	}
	if (S2CON & 0x01)
	{
		S2CON &= ~0x01;
		//S2BUF;

	}
}


//UART3�ж�
void Uart3Isr() interrupt 17
{
	if (S3CON & 0x02)
	{
		S3CON &= ~0x02;
		busy[3] = 0;
	}
	if (S3CON & 0x01)
	{
		S3CON &= ~0x01;
		//S3BUF;

	}
}


//UART4�ж�

void Uart4Isr() interrupt 18
{
	if (S4CON & 0x02)
	{
		S4CON &= ~0x02;
		busy[4] = 0;
	}
	if (S4CON & 0x01)
	{
		S4CON &= ~0x01;

		//S4BUF;

	}
}


//PCA���ڶ�ʱ���ж�
void PCA_Isr() interrupt 7
{

	if(CCF0 == 1)
	{
		//����жϱ�־λ
		CCF0 = 0;				
		//���ؼ�����
		CCAP0L = ccp_value[0];
		CCAP0H = ccp_value[0] >> 8;
		ccp_value[0] += pca_time_ms[0];
		

	}
	
	if(CCF1 == 1)
	{
		//����жϱ�־λ
		CCF1 = 0;
		//���ؼ�����
		CCAP1L = ccp_value[1];
		CCAP1H = ccp_value[1] >> 8;
		ccp_value[1] += pca_time_ms[1];
		
	}
	

	
//	PCA_2������ʱ
//	if(CCF2 == 1)
//	{
//		//����жϱ�־λ
//		CCF2 = 0;
//		//���ؼ�����
//		CCAP2L = ccp_value[2];
//		CCAP2H = ccp_value[2] >> 8;
//		ccp_value[2] += pca_time_ms[2];
//	}
	
}


#define LED P52
void INT0_Isr() interrupt 0
{
	LED = 0;	//����LED
}
void INT1_Isr() interrupt 2
{

}
void INT2_Isr() interrupt 10
{

}
void INT3_Isr() interrupt 11
{

}

void INT4_Isr() interrupt 16
{

}


//void TM4_Isr() interrupt 20
