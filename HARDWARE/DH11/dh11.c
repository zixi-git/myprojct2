#include "dh11.h"
#include <stdio.h>
#include <stm32f4xx.h>
#include "sys.h"
#include "delay.h"

void dht11_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//ָ��9������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�ٶ�Խ�ߣ����ľ�Խ�ߣ�������Ӧ�ٶ�Ҳ����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����Ҫʹ���ڲ�����������
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	//��ʱ��ͼ����ʼ��ƽ״̬Ϊ�ߵ�ƽ
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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//ָ��9������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�ٶ�Խ�ߣ����ľ�Խ�ߣ�������Ӧ�ٶ�Ҳ����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����Ҫʹ���ڲ�����������
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	PGout(9)=0;
	delay_ms(20);
	PGout(9)=1;	
	delay_us(30);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//ָ��9������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����Ϊ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�ٶ�Խ�ߣ����ľ�Խ�ߣ�������Ӧ�ٶ�Ҳ����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����Ҫʹ���ڲ�����������
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	
	//�ȴ��͵�ƽ����
	t=0;
	while(PGin(9))
	{
		delay_us(1);
		t++;
		
		if(t >=4000)
			return -1;
	}
	
	//���͵�ƽ����Ч��
	t=0;
	while(PGin(9)==0)
	{
		delay_us(1);
		t++;
		
		if(t >=100)
			return -2;
	}	
	
	
	//���ߵ�ƽ����Ч��
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
			//�ɹ��ж�һ������0������1
			
			//�ȴ�ǰ�õ�50us��ƽ�������
			t=0;
			while(PGin(9)==0)
			{
				delay_us(1);
				t++;
				
				if(t >=100)
					return -4;
			}

			//��ʱһ�ᣬ��ʱ�೤ʱ��ź�����(30us~70us)
			delay_us(40);
			
			if(PGin(9))
			{
				d|=1<<i;
				
				//�ȴ�ʣ�µĸߵ�ƽ�������
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
	
	//ͨ��У��ͼ�����ݵ���Ч��
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(check_sum !=p[4])
		return -6;
	
	return 0;
}
