#include "includes.h"

void MOTOR_Init(void)
{
	GPIO_InitTypeDef 		 GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	//����PF9����
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;;//��9������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;	//���ù���ģʽ�����Ǹ������ܹ�������������п���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;//�����ٶȵͣ�����Ҳ��
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	//ʹ�ܶ�ʱ��14��Ӳ��ʱ�ӣ����Ƕ�������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	//����TIM14�Ĳ������������Ƶ��
	TIM_TimeBaseStructure.TIM_Period = 10000/50-1;//arr   Ƶ��Ϊ100Hz  10000/200 200hz �Ƚ�ֵ50 // 10000/50 50hz �Ƚ�ֵ200  // 10000/100 100hz �Ƚ�ֵ100
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;//psc			168000000(system clk) / arr * psc = pwm
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;//���ʱ�ӷ�Ƶ���Ƕ��η�Ƶ������Ч��TIM3��֧�֡�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	//�����������
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//�����ͨ������PMWģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�����������
	TIM_OCInitStructure.TIM_Pulse = 15;//�Ƚ�ֵΪ5000������ռ�ձ�Ϊ50%
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//�����Ч��״̬Ϊ�ߵ�ƽ
	
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	
	//ʹ��TIM14����
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	//PE13���õ���ʱ��1
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);

}

void motor_corotation_single_pos(void)
{
	TIM_SetCompare3(TIM1, 14);//˳ʱ��
}

//ռ�ձ�0-200��5-25��Ч��5Ϊ��ʱ������ٶȣ�25Ϊ˳ʱ������ٶȣ�15Ϊ�м�λ�����ֹͣ
void motor_corotation_single_rev(void)
{
	TIM_SetCompare3(TIM1, 16);//��ʱ��
}

//ռ�ձ�0-200��5-25��Ч��5Ϊ��ʱ������ٶȣ�25Ϊ˳ʱ������ٶȣ�15Ϊ�м�λ�����ֹͣ
void motor_corotation_double_pos(void)
{
	TIM_SetCompare3(TIM1, 11);//˳ʱ��
	delay_ms(500);
	TIM_SetCompare3(TIM1, 15);
}

//ռ�ձ�0-200��5-25��Ч��5Ϊ��ʱ������ٶȣ�25Ϊ˳ʱ������ٶȣ�15Ϊ�м�λ�����ֹͣ
void motor_corotation_double_rev(void)
{
	TIM_SetCompare3(TIM1, 19);//��ʱ��
	delay_ms(500);
	TIM_SetCompare3(TIM1, 15);
}

//ռ�ձ�0-200��5-25��Ч��5Ϊ��ʱ������ٶȣ�25Ϊ˳ʱ������ٶȣ�15Ϊ�м�λ�����ֹͣ
void motor_corotation_eghit_pos(void)
{
	TIM_SetCompare3(TIM1, 11);
	delay_ms(500);
	TIM_SetCompare3(TIM1, 15);
}
//ռ�ձ�0-200��5-25��Ч��5Ϊ��ʱ������ٶȣ�25Ϊ˳ʱ������ٶȣ�15Ϊ�м�λ�����ֹͣ
void motor_corotation_eghit_rev(void)
{
	TIM_SetCompare3(TIM1, 19);
	delay_ms(500);
	TIM_SetCompare3(TIM1, 15);
}

