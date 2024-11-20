#include "includes.h"

void MOTOR_Init(void)
{
	GPIO_InitTypeDef 		 GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	//配置PF9引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;;//第9根引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;	//复用功能模式，就是该引脚能够被其他外设进行控制
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;//工作速度低，功耗也低
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	//使能定时器14的硬件时钟，就是对它供电
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	//配置TIM14的参数：配置输出频率
	TIM_TimeBaseStructure.TIM_Period = 10000/50-1;//arr   频率为100Hz  10000/200 200hz 比较值50 // 10000/50 50hz 比较值200  // 10000/100 100hz 比较值100
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;//psc			168000000(system clk) / arr * psc = pwm
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;//这个时钟分频就是二次分频，不生效，TIM3不支持。
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	//设置输出脉宽
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//让输出通道工作PMW模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//允许输出脉冲
	TIM_OCInitStructure.TIM_Pulse = 15;//比较值为5000，就是占空比为50%
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出有效的状态为高电平
	
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	
	//使能TIM14工作
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	//PE13复用到定时器1
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);

}

void motor_corotation_single_pos(void)
{
	TIM_SetCompare3(TIM1, 14);//顺时针
}

//占空比0-200，5-25有效，5为逆时针最快速度，25为顺时针最快速度，15为中间位，舵机停止
void motor_corotation_single_rev(void)
{
	TIM_SetCompare3(TIM1, 16);//逆时针
}

//占空比0-200，5-25有效，5为逆时针最快速度，25为顺时针最快速度，15为中间位，舵机停止
void motor_corotation_double_pos(void)
{
	TIM_SetCompare3(TIM1, 11);//顺时针
	delay_ms(500);
	TIM_SetCompare3(TIM1, 15);
}

//占空比0-200，5-25有效，5为逆时针最快速度，25为顺时针最快速度，15为中间位，舵机停止
void motor_corotation_double_rev(void)
{
	TIM_SetCompare3(TIM1, 19);//逆时针
	delay_ms(500);
	TIM_SetCompare3(TIM1, 15);
}

//占空比0-200，5-25有效，5为逆时针最快速度，25为顺时针最快速度，15为中间位，舵机停止
void motor_corotation_eghit_pos(void)
{
	TIM_SetCompare3(TIM1, 11);
	delay_ms(500);
	TIM_SetCompare3(TIM1, 15);
}
//占空比0-200，5-25有效，5为逆时针最快速度，25为顺时针最快速度，15为中间位，舵机停止
void motor_corotation_eghit_rev(void)
{
	TIM_SetCompare3(TIM1, 19);
	delay_ms(500);
	TIM_SetCompare3(TIM1, 15);
}

