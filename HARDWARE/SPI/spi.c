#include "spi.h"
#include <stdio.h>
#include <stm32f4xx.h>
#include "sys.h"

void w25q128_init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	//ʹ�ܶ˿�B��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	
	
	//ʹ��SPI1��Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	
	//����PB3~PB5Ϊ���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;	//ָ��3��4��5������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//����Ϊ���ù���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�ٶ�Խ�ߣ����ľ�Խ�ߣ�������Ӧ�ٶ�Ҳ����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����Ҫʹ���ڲ�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
	//��PB3~PB5���ӵ�SPI1��Ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);		
	
	
	//����PB14Ϊ���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	//ָ��14������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//����Ϊ���칦��ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�ٶ�Խ�ߣ����ľ�Խ�ߣ�������Ӧ�ٶ�Ҳ����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����Ҫʹ���ڲ�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	//PB14��ʼ��ƽ״̬Ϊ�ߵ�ƽ����Ϊ���߿��е�ʱ��SS���ţ�Ƭѡ���ţ�Ϊ�ߵ�ƽ
	PBout(14)=1;
	
	
	//SPI1����������
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//����ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//ÿ�δ�����С��ԪΪ�ֽ�
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//���ݴӻ����ֲ������ã�ģʽ3
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//���ݴӻ����ֲ������ã�ģʽ3
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//������������Ƭѡ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//���ݴӻ����ֲ������ã�SPI��Ӳ��ʱ��=84MHz/6=5.25MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ݴӻ����ֲ�������
	SPI_Init(SPI1, &SPI_InitStructure);
	
	
	//ʹ��SPI1Ӳ������
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

void w25q128_read_data(uint8_t *data,uint32_t addr,uint8_t size) //24λ
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
void w25q128_SectorErase(uint32_t addr) //24λ
{
	uint8_t statu=0;
	W25Q128_SS=0;

	SPI1_SendByte(0x06); //дʹ��

	W25Q128_SS=1;	
	
	W25Q128_SS=0;
	SPI1_SendByte(0x20); //������������
	SPI1_SendByte(addr>>16);
	SPI1_SendByte(addr>>8);
	SPI1_SendByte(addr);
	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x05);//��״̬�Ĵ���1	
	while(1)
	{
		statu=SPI1_SendByte(0x00);
		if((statu&0x01)!=0x01)//���λΪ1
		{
			break;
		}	
	}
	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x04);//д����
	W25Q128_SS=1;
	printf("SectorErase ok \r\n");
}

void w25q128_page_write(uint8_t *data,uint32_t addr) //24λ
{
	uint8_t i=0;
	uint8_t statu=0;
	W25Q128_SS=0;
	SPI1_SendByte(0x06); //дʹ��
	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x02); //ҳд��ָ��
	SPI1_SendByte(addr>>16);
	SPI1_SendByte(addr>>8);
	SPI1_SendByte(addr);
	
	while(data[i]!='\0')
	{
		SPI1_SendByte(data[i]);
		i++;
		if(i==255) //����һҳ
			break;
	}
	

	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x05);//��״̬�Ĵ���1	
	while(1)
	{
		statu=SPI1_SendByte(0x00);
		if((statu&0x01)!=0x01)//���λΪ1
		{
			break;
		}	
	}
	W25Q128_SS=1;
	
	W25Q128_SS=0;
	SPI1_SendByte(0x04);//д����
	W25Q128_SS=1;
}

