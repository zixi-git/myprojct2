#include "dh11.h"
#include <stdio.h>
#include <stm32f4xx.h>
#include "sys.h"
#include "delay.h"

void dht11_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//指定9号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//配置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//速度越高，功耗就越高，但是响应速度也更快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不需要使能内部上下拉电阻
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	//看时序图，初始电平状态为高电平
	PGout(9)=1;
}

int32_t dht11_read(uint8_t *buf)
{
	uint8_t *p = buf;
	uint32_t t=0;
	int32_t i,j;
	uint8_t d=0;
	uint8_t check_sum=0;
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//指定9号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//配置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//速度越高，功耗就越高，但是响应速度也更快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不需要使能内部上下拉电阻
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	PGout(9)=0;
	delay_ms(20);
	PGout(9)=1;	
	delay_us(30);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//指定9号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//配置为输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//速度越高，功耗就越高，但是响应速度也更快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不需要使能内部上下拉电阻
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	
	//等待低电平出现
	t=0;
	while(PGin(9))
	{
		delay_us(1);
		t++;
		
		if(t >=4000)
			return -1;
	}
	
	//检测低电平的有效性
	t=0;
	while(PGin(9)==0)
	{
		delay_us(1);
		t++;
		
		if(t >=100)
			return -2;
	}	
	
	
	//检测高电平的有效性
	t=0;
	while(PGin(9))
	{
		delay_us(1);
		t++;
		
		if(t >=100)
			return -3;
	}

	for(j=0; j<5; j++)
	{
		d=0;
		
		for(i=7; i>=0; i--)
		{
			//成功判断一个数据0或数据1
			
			//等待前置的50us电平持续完毕
			t=0;
			while(PGin(9)==0)
			{
				delay_us(1);
				t++;
				
				if(t >=100)
					return -4;
			}

			//延时一会，延时多长时间才合适呢(30us~70us)
			delay_us(40);
			
			if(PGin(9))
			{
				d|=1<<i;
				
				//等待剩下的高电平持续完毕
				t=0;
				while(PGin(9))
				{
					delay_us(1);
					t++;
					
					if(t >=100)
						return -5;
				}				
			}
		
		}
		
		p[j]=d;
		
	}
	
	//通过校验和检测数据的有效性
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(check_sum !=p[4])
		return -6;
	
	return 0;
}
