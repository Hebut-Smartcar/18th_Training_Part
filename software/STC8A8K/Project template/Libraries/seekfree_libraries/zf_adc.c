/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897(����)  ��Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ����������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		adc
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ790875685)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		MDK FOR C51 V9.60
 * @Target core		STC8A8K64S4
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-4-14
 ********************************************************************************************************************/


#include "zf_adc.h"



//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC��ʼ��
//  @param      adcn            ѡ��ADCͨ��
//  @param      speed      		ADCʱ��Ƶ��
//  @return     void
//  Sample usage:               adc_init(ADC_P10,ADC_SYSclk_DIV_2);//��ʼ��P1.0ΪADC����,ADCʱ��Ƶ�ʣ�SYSclk/2
//-------------------------------------------------------------------------------------------------------------------
void adc_init(ADCN_enum adcn,ADC_SPEED_enum speed)
{
	ADC_CONTR |= 1<<7;			//�� ADC ��Դ
	ADC_CONTR &= (0xF0);		//���ADC_CHS[3:0] �� ADC ģ��ͨ��ѡ��λ
	ADC_CONTR |= adcn;
	
	if((adcn >> 3) == 1) 		//P0��
	{
		//IO������Ϊ��������
		P0M0 &= ~(1 << (adcn & 0x07));
		P0M1 |= (1 << (adcn & 0x07));
	}
	else if((adcn >> 3) == 0) 	//P1��
	{
		//IO������Ϊ��������
		P1M0 &= ~(1 << (adcn & 0x07));
	    P1M1 |= (1 << (adcn & 0x07));
	}

	ADCCFG |= speed&0x0F;		//ADCʱ��Ƶ��SYSclk/2/16/speed&0x0F;

	ADCCFG |= 1<<5;				//ת������Ҷ��롣 ADC_RES �������ĸ� 2 λ�� ADC_RESL �������ĵ� 8 λ��
}



//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADCת��һ��
//  @param      adcn            ѡ��ADCͨ��
//  @param      resolution      �ֱ���
//  @return     void
//  Sample usage:               adc_convert(ADC_P10, ADC_10BIT);
//-------------------------------------------------------------------------------------------------------------------
uint16 adc_once(ADCN_enum adcn,ADCRES_enum resolution)
{
	uint16 adc_value;
	
	ADC_CONTR &= (0xF0);			//���ADC_CHS[3:0] �� ADC ģ��ͨ��ѡ��λ
	ADC_CONTR |= adcn;
	
	ADC_CONTR |= 0x40;  			// ���� AD ת��
//	_nop_();
//	_nop_();
	while (!(ADC_CONTR & 0x20));  	// ��ѯ ADC ��ɱ�־
	ADC_CONTR &= ~0x20;  			// ����ɱ�־
	
	
	adc_value = ADC_RES; 		 	//�洢 ADC �� 12 λ����ĸ� 4 λ
	adc_value <<= 8;
	adc_value |= ADC_RESL;  		//�洢 ADC �� 12 λ����ĵ� 4 λ
	
	adc_value = adc_value >> resolution;		//ȡ����λ
	

	return adc_value;
}