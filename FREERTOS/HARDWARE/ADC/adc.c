#include "adc.h"
#include <stdio.h>
#include <stm32f4xx.h>
#include "sys.h"
void adc1_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	//使能端口A的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	
	//使能ADC的硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	
	//配置端口A为AN模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	//配置ADC相关的参数
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//只需要一个ADC独立工作
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//预分频值为2，ADC的硬件时钟=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//关闭DMA
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样点的间隔=5*(1/42MHz)
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位的分辨率(0~4095)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//只扫描一个通道
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换
	//ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换得到后的12位数据进行右对齐存储
	ADC_InitStructure.ADC_NbrOfConversion = 1;//转换1个通道
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//配置ADC1的通道5，扫描序号为1，采样时间为3个ADC时钟周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);
	
	
	//使能ADC工作
	ADC_Cmd(ADC1, ENABLE);
}
void adc3_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	//使能端口A的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	
	//使能ADC的硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	
	//配置端口A为AN模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	
	
	//配置ADC相关的参数
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//只需要一个ADC独立工作
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//预分频值为2，ADC的硬件时钟=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//关闭DMA
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样点的间隔=5*(1/42MHz)
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位的分辨率(0~4095)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//只扫描一个通道
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换
	//ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换得到后的12位数据进行右对齐存储
	ADC_InitStructure.ADC_NbrOfConversion = 1;//转换1个通道
	ADC_Init(ADC3, &ADC_InitStructure);
	
	//配置ADC3的通道5，扫描序号为1，采样时间为3个ADC时钟周期
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);
	
	
	//使能ADC工作
	ADC_Cmd(ADC3, ENABLE);
}

