#include "i2c.h"
#include <stdio.h>
#include <stm32f4xx.h>
#include "sys.h"


#define SCL_W	PBout(8)

#define SDA_W	PBout(9)
#define SDA_R	PBin(9)


void sda_pin_mode(GPIOMode_TypeDef pin_mode)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//指定9号引脚
	GPIO_InitStructure.GPIO_Mode = pin_mode;//配置为输出/输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//速度越高，功耗就越高，但是响应速度也更快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不需要使能内部上下拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void i2c_start(void)
{
	sda_pin_mode(GPIO_Mode_OUT);

	SCL_W=1;
	SDA_W=1;
	
	delay_us(5);
	
	SDA_W=0;
	delay_us(5);	
	
	SCL_W=0;
	delay_us(5);
}


void i2c_stop(void)
{
	sda_pin_mode(GPIO_Mode_OUT);

	SCL_W=1;
	SDA_W=0;
	delay_us(5);
	
	SDA_W=1;
	delay_us(5);
}


void i2c_send_byte(uint8_t txd)
{
	int32_t i=0;
	sda_pin_mode(GPIO_Mode_OUT);

	SCL_W=0;
	SDA_W=0;
	delay_us(5);

	for(i=7; i>=0; i--)
	{
		if(txd &(1<<i))
			SDA_W=1;
		else
			SDA_W=0;
	
		delay_us(5);
		
		SCL_W=1;
		delay_us(5);
		
		SCL_W=0;
		delay_us(5);		
	
	}
}

uint8_t i2c_read_byte()
{
	int32_t i=0;
	uint8_t rx=0;
	sda_pin_mode(GPIO_Mode_IN);

	SCL_W=0;
	delay_us(5);

	for(i=7; i>=0; i--)
	{
		
		if(SDA_R)
		{
			rx|=1<<i;
		}
		delay_us(5);
		
		SCL_W=1;
		delay_us(5);
		
		SCL_W=0;
		delay_us(5);		
	
	}
	return rx;
}

uint8_t i2c_wait_ack(void)
{
	uint8_t ack;
	
	sda_pin_mode(GPIO_Mode_IN);

	SCL_W=1;
	delay_us(5);

	if(SDA_R)
		ack=1;	//无应答
	else
		ack=0;	//有应答
		

	SCL_W=0;
	delay_us(5);

	
	return ack;

}

void i2c_send_ack(void)
{
	
	SCL_W=0;
	sda_pin_mode(GPIO_Mode_OUT);
	SDA_W=0;
	SCL_W=1;
	delay_us(5);
	SCL_W=0;
	delay_us(5);

}

void i2c_send_noack(void)
{
	sda_pin_mode(GPIO_Mode_OUT);
	SCL_W=0;
	SDA_W=1;
	delay_us(5);
	SCL_W=1;
	delay_us(5);
	SCL_W=0;
	
}

void eeprom_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	

	/*3. 初始化该端口 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;	//指定8、9号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//配置为推挽功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//速度越高，功耗就越高，但是响应速度也更快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不需要使能内部上下拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	PBout(8)=1;
	PBout(9)=1;

}

int32_t eeprom_write(uint8_t addr,uint8_t *buf,uint32_t len)
{
	uint8_t ack;
	
	uint8_t *p=buf;
	
	i2c_start();
	
	i2c_send_byte(0xA0);
	
	ack=i2c_wait_ack();
	
	if(ack)
	{
		printf("device address fail\r\n");
		
		return -1;
	}
	
	i2c_send_byte(addr);
	
	ack=i2c_wait_ack();
	
	if(ack)
	{
		printf("word address fail\r\n");
		
		return -2;
	}	
	
	while(len--)
	{
		i2c_send_byte(*p++);
		
		ack=i2c_wait_ack();
		
		if(ack)
		{
			printf("write data fail\r\n");
			
			return -3;
		}	
	}
	
	i2c_stop();
	
	return 0;
}

int32_t eeprom_read(uint8_t addr,uint8_t *buf,uint32_t len)
{
	uint8_t ack;
	
	uint8_t *p=buf;
	
	i2c_start();
	
	i2c_send_byte(0xA0);

	ack=i2c_wait_ack();
	
	if(ack)
	{
		printf("device address with write fail\r\n");
		
		return -1;
	}
	
	i2c_send_byte(addr);
	
	ack=i2c_wait_ack();
	
	if(ack)
	{
		printf("word address fail\r\n");
		
		return -2;
	}	
	
	
	i2c_start();
	i2c_send_byte(0xA1);
	ack=i2c_wait_ack();
	
	if(ack)
	{
		printf("device address with read fail\r\n");
		
		return -3;
	}	
		
	
	len=len-1;
	
	while(len--)
	{
		*p=i2c_read_byte();
		
		p++;
		
		i2c_send_ack();
	}
	
	*p=i2c_read_byte();
	i2c_send_noack();
	
	i2c_stop();
	
	return 0;
}
