#include "adc.h"
#include <stdio.h>
#include <stm32f4xx.h>
#include "sys.h"
void adc1_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	//ʹ�ܶ˿�A��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	
	//ʹ��ADC��Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	
	//���ö˿�AΪANģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	//����ADC��صĲ���
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//ֻ��Ҫһ��ADC��������
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//Ԥ��ƵֵΪ2��ADC��Ӳ��ʱ��=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//�ر�DMA
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//����������ļ��=5*(1/42MHz)
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λ�ķֱ���(0~4095)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//ֻɨ��һ��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��
	//ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ת���õ����12λ���ݽ����Ҷ���洢
	ADC_InitStructure.ADC_NbrOfConversion = 1;//ת��1��ͨ��
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//����ADC1��ͨ��5��ɨ�����Ϊ1������ʱ��Ϊ3��ADCʱ������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);
	
	
	//ʹ��ADC����
	ADC_Cmd(ADC1, ENABLE);
}
void adc3_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	//ʹ�ܶ˿�A��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	
	//ʹ��ADC��Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	
	//���ö˿�AΪANģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	
	
	//����ADC��صĲ���
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//ֻ��Ҫһ��ADC��������
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//Ԥ��ƵֵΪ2��ADC��Ӳ��ʱ��=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//�ر�DMA
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//����������ļ��=5*(1/42MHz)
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λ�ķֱ���(0~4095)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//ֻɨ��һ��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��
	//ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ת���õ����12λ���ݽ����Ҷ���洢
	ADC_InitStructure.ADC_NbrOfConversion = 1;//ת��1��ͨ��
	ADC_Init(ADC3, &ADC_InitStructure);
	
	//����ADC3��ͨ��5��ɨ�����Ϊ1������ʱ��Ϊ3��ADCʱ������
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);
	
	
	//ʹ��ADC����
	ADC_Cmd(ADC3, ENABLE);
}

