#include "sys.h"
#include <stdio.h>
#include <stm32f4xx.h>

void delay_ms(uint32_t n)
{
	while(n--)
	{
	
		SysTick->CTRL = 0; // �ر�ϵͳ��ʱ��
		SysTick->LOAD = 168000-1; // 168000000/1000 -1��ÿ����ʱ1ms
		SysTick->VAL = 0; //���val�Ĵ��� �������COUNTFLAG��־λ
		SysTick->CTRL = 5; // ʹ��ϵͳ��ʱ��������������ʱ��ԴΪ������ʱ��168MHz
		while ((SysTick->CTRL & 0x00010000)==0);// �ȴ�ϵͳ��ʱ���������
		SysTick->CTRL = 0; // �ر�ϵͳ��ʱ��	
	}
}

void delay_us(uint32_t n)
{

	SysTick->CTRL = 0; // �ر�ϵͳ��ʱ��
	SysTick->LOAD = n*168-1; // 168000000/1000000 -1��ÿ����ʱ1us
	SysTick->VAL = 0; //���val�Ĵ��� �������COUNTFLAG��־λ
	SysTick->CTRL = 5; // ʹ��ϵͳ��ʱ��������������ʱ��ԴΪ������ʱ��168MHz
	while ((SysTick->CTRL & 0x00010000)==0);// �ȴ�ϵͳ��ʱ���������
	SysTick->CTRL = 0; // �ر�ϵͳ��ʱ��	
}

void usart1_init(uint32_t baud)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//��Ӳ��ʱ�ӣ�GPIO�˿�Ӳ��ʱ�ӡ�USARTӲ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	
	//����GPIO����Ϊ���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;	//ָ��9��10������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//����Ϊ���ù���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�ٶ�Խ�ߣ����ľ�Խ�ߣ�������Ӧ�ٶ�Ҳ����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����Ҫʹ���ڲ�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//����Ӧ��GPIO�������ӵ�����
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	//���ô�����صĲ��������ںţ������ʣ�����λ��ֹͣλ��������
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	
	//���ô�����ص��ж�
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//ʹ�ܴ��ڹ���
	USART_Cmd(USART1, ENABLE);
}


#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) 
{
	
	USART_SendData(USART1,ch);
		
	//�ȴ��������
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		
	//��ձ�־λ
	USART_ClearFlag(USART1,USART_FLAG_TXE);
	
	//���سɹ����͵��ַ�
	return ch;
}

void _sys_exit(int return_code) 
{
	label: goto label; /* endless loop */
}
