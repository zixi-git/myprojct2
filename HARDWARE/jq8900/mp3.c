#include "includes.h"

void mp3_init(void)
{
	GPIO_InitTypeDef 		 GPIO_InitStructure;
	
		//配置PF9引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7 | GPIO_Pin_9;;//第9根引脚 DCMI_D1  DCMI_D3
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;	//复用功能模式，就是该引脚能够被其他外设进行控制
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_25MHz;//工作速度低，功耗也低
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;;//第9根引脚  DCMI_D5
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//第9根引脚  DCMI_D7
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;//第9根引脚  DCMI__XCL
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	PCout(7) = 1;
	PCout(9) = 1;
	PBout(6) = 1;
	PEout(6) = 1;
	PAout(8) = 1;
}
