#include "spi.h"
#include <stdio.h>
#include <stm32f4xx.h>
#include "sys.h"

void w25q128_init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	//使能端口B的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	
	
	//使能SPI1的硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	
	//配置PB3~PB5为复用功能模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;	//指定3、4、5号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//速度越高，功耗就越高，但是响应速度也更快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不需要使能内部上下拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
	//将PB3~PB5连接到SPI1的硬件
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);		
	
	
	//配置PB14为输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	//指定14号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//配置为推挽功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//速度越高，功耗就越高，但是响应速度也更快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不需要使能内部上下拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	//PB14初始电平状态为高电平，因为总线空闲的时候，SS引脚（片选引脚）为高电平
	PBout(14)=1;
	
	
	//SPI1参数的配置
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//每次传输最小单元为字节
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//根据从机的手册来配置，模式3
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//根据从机的手册来配置，模式3
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//由软件代码控制片选引脚
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//根据从机的手册来配置，SPI的硬件时钟=84MHz/6=5.25MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//根据从机的手册来配置
	SPI_Init(SPI1, &SPI_InitStructure);
	
	
	//使能SPI1硬件工作
	SPI_Cmd(SPI1,ENABLE);

}

uint8_t SPI1_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}


void w25q128_read_id(uint8_t *m_id,uint8_t *d_id)
{
	W25Q128_SS=0;

	SPI1_SendByte(0x90);

	SPI1_SendByte(0x00);
	SPI1_SendByte(0x00);
	SPI1_SendByte(0x00);

	
	*m_id=SPI1_SendByte(0xFF);
	*d_id=SPI1_SendByte(0xFF);

	W25Q128_SS=1;	
}

void w25q128_read_data(uint8_t *data,uint32_t addr,uint8_t size) //24位
{
	int i=0;
	W25Q128_SS=0;

	SPI1_SendByte(0x03);

	SPI1_SendByte(addr>>16);
	SPI1_SendByte(addr>>8);
	SPI1_SendByte(addr);

	
	for(i=0;i<size;i++)
	{
		data[i]=SPI1_SendByte(0xFF);
	}

	W25Q128_SS=1;	
}
void w25q128_SectorErase(uint32_t addr) //24位
{
	uint8_t statu=0;
	W25Q128_SS=0;

	SPI1_SendByte(0x06); //写使能

	W25Q128_SS=1;	
	
	W25Q128_SS=0;
	SPI1_SendByte(0x20); //擦除扇区命令
	SPI1_SendByte(addr>>16);
	SPI1_SendByte(addr>>8);
	SPI1_SendByte(addr);
	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x05);//读状态寄存器1	
	while(1)
	{
		statu=SPI1_SendByte(0x00);
		if((statu&0x01)!=0x01)//最低位为1
		{
			break;
		}	
	}
	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x04);//写保护
	W25Q128_SS=1;
	printf("SectorErase ok \r\n");
}

void w25q128_page_write(uint8_t *data,uint32_t addr) //24位
{
	uint8_t i=0;
	uint8_t statu=0;
	W25Q128_SS=0;
	SPI1_SendByte(0x06); //写使能
	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x02); //页写入指令
	SPI1_SendByte(addr>>16);
	SPI1_SendByte(addr>>8);
	SPI1_SendByte(addr);
	
	while(data[i]!='\0')
	{
		SPI1_SendByte(data[i]);
		i++;
		if(i==255) //超过一页
			break;
	}
	

	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x05);//读状态寄存器1	
	while(1)
	{
		statu=SPI1_SendByte(0x00);
		if((statu&0x01)!=0x01)//最低位为1
		{
			break;
		}	
	}
	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x04);//写保护
	W25Q128_SS=1;
}

