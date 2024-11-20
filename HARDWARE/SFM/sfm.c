#include "includes.h"
//USART���ڽ��ճ����Լ���־λ
volatile uint8_t USART2_STA  = 0;

//����ָ��ģ�鴥���ж��¼�
volatile uint32_t g_fpm_touch_event=0;

static volatile uint8_t  g_usart2_buf[64];
static volatile uint32_t g_usart2_cnt=0;
static volatile uint32_t g_usart2_event=0;

static GPIO_InitTypeDef   GPIO_InitStructure;
static EXTI_InitTypeDef   EXTI_InitStructure;
static NVIC_InitTypeDef   NVIC_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

//ģ��LED�ƿ���Э��
static const uint8_t fpm_led_blue[16]  = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x03,0x01,0x01,0x00,0x00,0x49};
static const uint8_t fpm_led_red[16]   = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x04,0x04,0x02,0x00,0x50};
static const uint8_t fpm_led_green[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x02,0x02,0x02,0x00,0x4C};

static void tim3_init(void)
{
	//ʹ��tim3��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//����tim3�ķ�Ƶֵ������ֵ
	//tim3Ӳ��ʱ��=84MHz/8400=10000Hz�����ǽ���10000�μ���������1��ʱ��ĵ���

	TIM_TimeBaseStructure.TIM_Period = 10000/1000-1; //����ֵ0 -> 999����1����ʱ��ĵ���
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;	//Ԥ��Ƶֵ8400
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//ʱ�ӷ�Ƶ����ǰ��û�еģ�����Ҫ��������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//����tim3���ж�
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE);
}


static uint16_t check_sum(uint8_t *buf,uint32_t len)
{
	uint16_t sum=0;
	
	uint8_t *p=buf;
	
	while(len--)
		sum+=*p++;
	
	return sum;
}	


static void usart2_printf_recv_buf(void)
{
	uint32_t i;
	
	printf("usart2 recv buf:");
	
	for(i=0;i<USART2_STA;i++)
	{
		printf("%02X ",g_usart2_buf[i]);
	
	}
	
	printf("\r\n");
}


/**
	* @brief	USART2���ڷ��ͺ���
	* @param 	length: �������鳤��
	* @param	FPM383C_Databuffer[]: ��Ҫ���͵Ĺ���Э�����飬���������ж���
	* @return None
	*/
void fpm_send_data(int32_t length,uint8_t FPM383C_Databuffer[])
{
	int32_t i;
	
	for(i = 0;i<length;i++)
	{
		USART_SendData(USART2,FPM383C_Databuffer[i]);
		
		while(!USART_GetFlagStatus(USART2,USART_FLAG_TXE));
	}
	
	g_usart2_event = 0;
	g_usart2_cnt=0;
}


/**
	* @brief	����˯��ָ�ֻ�з���˯��ָ���TOUCHOUT�Ż��ɵ͵�ƽ
	* @param 	None
	* @return None
	*/
void fpm_sleep(void)
{
	//ģ��˯��Э��
	uint8_t buf[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x33,0x00,0x37};
	
	fpm_send_data(12,buf);
}



uint8_t fpm_ctrl_led(uint8_t color)
{

	if(color == FPM_LED_RED)
		fpm_send_data(16,(uint8_t *)fpm_led_red);
	
	if(color == FPM_LED_GREEN)
		fpm_send_data(16,(uint8_t *)fpm_led_green);	
	
	if(color == FPM_LED_BLUE)
		fpm_send_data(16,(uint8_t *)fpm_led_blue);	
	
	return 0;
}


/* ���ָ�� */
int32_t fpm_empty(void)
{
    uint32_t timeout=4000;	
	

	uint8_t buf[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};	
	
	fpm_send_data(12,buf);
	
	while(!g_usart2_event && (--timeout))
	{
		delay_ms(1);
	}
	
	usart2_printf_recv_buf();	
	
	
	if(g_usart2_buf[0]!=0xEF && g_usart2_buf[1]!=0x01)
	{
		printf("����Ӳ������\r\n");

		return -1;
	}	
	
	if(!timeout)
	{
		printf("���ָ��:��ʱ\r\n");
		
		return -1;
	}
	
	/*  ��ӡ��ָ��ģ�鹤������ԭ�� */
	if(g_usart2_buf[9] != 0x00)
	{
	
		printf("���ָ�ƣ�ʧ�� %02X\r\n",g_usart2_buf[9] );
		printf("�벻Ҫ��ѹָ��ģ��\r\n");
		return -1;
		
	}	

	return 0;
	
}

/* �Զ�ע��ָ�� */
int32_t fpm_enroll_auto(uint16_t id)
{
    uint8_t buf[17];
	uint16_t cs=0;
    uint32_t timeout=4000;
	
    /* ��ͷ��2�ֽ� */
    buf[0]=0xEF;buf[1]=0x01;
    
    /* �豸��ַ��4�ֽ� */    
    buf[2]=0xFF;buf[3]=0xFF;buf[4]=0xFF;buf[5]=0xFF;
	
    /* ����ʶ��1�ֽ� */ 
    buf[6]=0x01;
    
    /* �����ȣ�2�ֽ� */
    buf[7]=0x00;buf[8]=0x08;
    
    /* ָ���룺1�ֽ� */
    buf[9]=0x31;
    
    /* ID���ֽڣ�2�ֽ� */
    buf[10]=(id>>8)&0xFF;buf[11]=id&0xFF;
    
    /* ¼�������1�ֽ� */
    buf[12]=1;
    
    /* ������2�ֽ� */
    buf[13]=0x00;
    
    buf[14]=0x3F;
    
    cs=check_sum(&buf[6],9);
    
    buf[15]=(cs>>8)&0xFF;
    buf[16]=(cs)&0xFF;   
	
	
	fpm_send_data(17,buf);
	
	
	while(!g_usart2_event && (--timeout))
	{
		delay_ms(1);
	}
	
	usart2_printf_recv_buf();
	
	if(g_usart2_buf[0]!=0xEF && g_usart2_buf[1]!=0x01)
	{
		printf("�Զ�ע��ָ��:���ݰ��쳣��\r\n");
		printf("1.���Ӳ������\r\n");
		printf("2.����Ƿ��а�ѹָ��ģ��\r\n");		
		return -1;
	}	
	
	if(!timeout)
	{
		printf("�Զ�ע��ָ��:��ʱ\r\n");
		
		return -1;
	}
	
	/*  ��ӡ��ָ��ģ�鹤������ԭ�� */
	if(g_usart2_buf[9] != 0x00)
	{
	
		printf("�Զ�ע��ָ�ƣ�ʧ�� %02X\r\n",g_usart2_buf[9] );
		
		return -1;
		
	}	
	
	
	/* ��ȷ���� */ 	
	return 0;
}


/* �Զ���ָ֤�� */
int32_t fpm_idenify_auto(uint16_t *id)
{
    uint8_t buf[17];
	uint16_t cs=0;
    uint32_t timeout=4000;
    
    /* ��ͷ��2�ֽ� */
    buf[0]=0xEF;buf[1]=0x01;
    
    /* �豸��ַ��4�ֽ� */    
    buf[2]=0xFF;buf[3]=0xFF;buf[4]=0xFF;buf[5]=0xFF;
	
    /* ����ʶ��1�ֽ� */ 
    buf[6]=0x01;
    
    /* �����ȣ�2�ֽ� */
    buf[7]=0x00;buf[8]=0x08;
    
    /* ָ���룺1�ֽ� */
    buf[9]=0x32;
    
    /* �����ȼ���1�ֽ� */
    buf[10]=80;
    
    /* ID���ֽڣ�2�ֽ� */
    buf[11]=(*id>>8)&0xFF;buf[12]=*id&0xFF;
    
    /* ������2�ֽ� */
    buf[13]=0x00;
    
    buf[14]=0x07;
    
    cs=check_sum(&buf[6],9);
    
    buf[15]=(cs>>8)&0xFF;
    buf[16]=(cs)&0xFF;    
    

	fpm_send_data(17,buf);
	
	
	while(!g_usart2_event && (--timeout))
	{
		delay_ms(1);
	}
	
	usart2_printf_recv_buf();
	
	if(g_usart2_buf[0]!=0xEF && g_usart2_buf[1]!=0x01)
	{
		printf("�Զ���ָ֤��:���ݰ��쳣��\r\n");
		printf("1.���Ӳ������\r\n");
		printf("2.����Ƿ��а�ѹָ��ģ��\r\n");		
		return -1;
	}	
	
	if(!timeout)
	{
		printf("�Զ���ָ֤��:��ʱ\r\n");
		return -1;
	}
	
	/*  ��ӡ��ָ��ģ�鹤������ԭ�� */
	if(g_usart2_buf[9] != 0x00)
	{
	
		printf("�Զ���ָ֤�ƣ�ʧ�� %02X\r\n",g_usart2_buf[9] );
		
		return -1;
		
	}
	
	/* ����ֵ�쳣 */
	if(((g_usart2_buf[13]<<8)|g_usart2_buf[14])==0xFFFF)
	{
		printf("�Զ���ָ֤�ƣ�����ֵ�쳣������û�д��ڸ�ָ��\r\n");
		
		return -1;		
	
	}		
	
	/* �ɹ�ƥ�䵽ָ�Ƶ�IDֵ */
	*id = (g_usart2_buf[11]<<8)|g_usart2_buf[12];
	

	
	/* ��ȷ���� */ 	
	return 0;
}

/* ��ȡ��Чָ������ */
int32_t fpm_id_total(uint16_t *total)
{

    uint32_t timeout=4000;	
	

	uint8_t buf[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x1D,0x00,0x21};	
	
	fpm_send_data(12,buf);
	
	while(!g_usart2_event && (--timeout))
	{
		delay_ms(1);
	}
	
	usart2_printf_recv_buf();	
	
	
	if(g_usart2_buf[0]!=0xEF && g_usart2_buf[1]!=0x01)
	{
		printf("����Ӳ������\r\n");

		return -1;
	}	
	
	if(!timeout)
	{
		printf("��ȡ��Чָ������:��ʱ\r\n");
		
		return -1;
	}
	
	/*  ��ӡ��ָ��ģ�鹤������ԭ�� */
	if(g_usart2_buf[9] != 0x00)
	{
	
		printf("��ȡ��Чָ��������ʧ�� %02X\r\n",g_usart2_buf[9] );
		printf("�벻Ҫ��ѹָ��ģ��\r\n");
		return -1;
		
	}	
	
	*total=(g_usart2_buf[10]<<8)|g_usart2_buf[11];

	return 0;

}


/**
	* @brief  �ⲿ�ж����ų�ʼ������
	* @param  None
	* @return None
	*/
void fpm_touch_init()
{
	/* �򿪶˿�E F��Ӳ��ʱ�ӣ����Ƕ�Ӳ�����磩��Ĭ��״̬�£�����ʱ�Ӷ��ǹر� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	
	/* ��ϵͳ����syscfgʱ�ӣ����Ƕ�Ӳ�����磩��Ĭ��״̬�£�����ʱ�Ӷ��ǹر� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	


	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//ָ��0������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//���Ź���������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;//�ٶ�Խ�ߣ���Ӧʱ��Խ�̣����ǹ��ľ�Խ�ߣ���Ÿ���ҲԽ��
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;//ʹ����������
	GPIO_Init(GPIOE,&GPIO_InitStructure);

	/* ���ⲿ�ж����ӵ�ָ�������ţ��ر�˵�������ű�ž�����ʹ���ĸ��ⲿ�ж� */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource6);


	/* �����ⲿ�ж�0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6; //ָ���ⲿ�ж�6
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�������ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����صĴ���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�����ⲿ�ж�6����
	EXTI_Init(&EXTI_InitStructure);	
	
	
	NVIC_InitStructure.NVIC_IRQChannel =EXTI9_5_IRQn ;//�жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//�жϴ�
	NVIC_Init(&NVIC_InitStructure);
		
}



/**
	* @brief	��main��������ô˺����Գ�ʼ��FPM383Cָ��ģ��
	* @param 	None
	* @return None
	*/
void fpm_init(void)
{
	tim3_init();
	usart2_init(57600);
	//fpm_touch_init();
	fpm_sleep();
}


const char *fpm_error_code_auto_enroll(uint8_t error_code)
{
	const char *p;
	
	switch(error_code)
	{
		case 0x00:p="�Զ�ע��ɹ�";
		break;
		
		case 0x01:p="�Զ�ִ��ʧ��";
		break;	

		case 0x07:p="��������ʧ��";
		break;		

		case 0x0A:p="�ϲ�ģ��";
		break;		

		case 0x0B:p="ID�ų�����Χ";
		break;	
		
		case 0x1F:p="ָ�ƿ�����";
		break;		
	
		case 0x22:p="ָ��ģ��ǿ�";
		break;	
		
		case 0x25:p="¼��������ô���";
		break;	

		case 0x26:p="��ʱ";
		break;	

		case 0x27:p="ָ���Ѵ���";
		break;	
		
		case 0x31:p="��������ܵȼ���ƥ��";
		break;

		default :
			p="ģ�鷵��ȷ��������";break;
	}

	return p;
}

/**
	* @brief	�ⲿ�жϷ�����
	* @param 	None
	* @return None
	*/
void EXTI9_5_IRQHandler(void)
{
	
	if (EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		g_fpm_touch_event=1;
		
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
}


/**
	* @brief	����2�жϷ�����
	* @param 	None
	* @return None
	*/
void USART2_IRQHandler(void)
{
	static uint8_t d=0;
	
	//����Ƿ���յ�����
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		d=USART_ReceiveData(USART2);
		
		if(g_usart2_cnt< sizeof(g_usart2_buf))
			g_usart2_buf[g_usart2_cnt++]=d;
		
		//��ձ�־λ��������Ӧ�µ��ж�����
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
void TIM3_IRQHandler(void)
{
	static uint32_t cnt=0;

	//����־λ
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		if(cnt!=g_usart2_cnt)
		{
			cnt=g_usart2_cnt;
		}
		else if(cnt && (cnt == g_usart2_cnt))
		{
			g_usart2_event=1;
			cnt=0;
		}
		
		//��ձ�־λ
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}

