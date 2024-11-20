#include "sys.h"
#include <stdio.h>
#include <stm32f4xx.h>

void delay_ms(uint32_t n)
{
	while(n--)
	{
	
		SysTick->CTRL = 0; // 关闭系统定时器
		SysTick->LOAD = 168000-1; // 168000000/1000 -1，每次延时1ms
		SysTick->VAL = 0; //清空val寄存器 还有清空COUNTFLAG标志位
		SysTick->CTRL = 5; // 使能系统定时器工作，而它的时钟源为处理器时钟168MHz
		while ((SysTick->CTRL & 0x00010000)==0);// 等待系统定时器计数完毕
		SysTick->CTRL = 0; // 关闭系统定时器	
	}
}

void delay_us(uint32_t n)
{

	SysTick->CTRL = 0; // 关闭系统定时器
	SysTick->LOAD = n*168-1; // 168000000/1000000 -1，每次延时1us
	SysTick->VAL = 0; //清空val寄存器 还有清空COUNTFLAG标志位
	SysTick->CTRL = 5; // 使能系统定时器工作，而它的时钟源为处理器时钟168MHz
	while ((SysTick->CTRL & 0x00010000)==0);// 等待系统定时器计数完毕
	SysTick->CTRL = 0; // 关闭系统定时器	
}

void usart1_init(uint32_t baud)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//打开硬件时钟：GPIO端口硬件时钟、USART硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	
	//配置GPIO引脚为复用功能模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;	//指定9、10号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//速度越高，功耗就越高，但是响应速度也更快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不需要使能内部上下拉电阻
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//将对应的GPIO引脚连接到串口
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	//配置串口相关的参数：串口号，波特率，数据位，停止位，流控制
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	
	//配置串口相关的中断
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//使能串口工作
	USART_Cmd(USART1, ENABLE);
}


#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) 
{
	
	USART_SendData(USART1,ch);
		
	//等待发送完毕
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		
	//清空标志位
	USART_ClearFlag(USART1,USART_FLAG_TXE);
	
	//返回成功发送的字符
	return ch;
}

void _sys_exit(int return_code) 
{
	label: goto label; /* endless loop */
}
