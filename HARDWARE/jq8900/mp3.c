#include "includes.h"

void mp3_init(void)
{
	GPIO_InitTypeDef 		 GPIO_InitStructure;
	
		//����PF9����
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7 | GPIO_Pin_9;;//��9������ DCMI_D1  DCMI_D3
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;	//���ù���ģʽ�����Ǹ������ܹ�������������п���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_25MHz;//�����ٶȵͣ�����Ҳ��
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;;//��9������  DCMI_D5
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//��9������  DCMI_D7
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;//��9������  DCMI__XCL
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	PCout(7) = 1;
	PCout(9) = 1;
	PBout(6) = 1;
	PEout(6) = 1;
	PAout(8) = 1;
}
