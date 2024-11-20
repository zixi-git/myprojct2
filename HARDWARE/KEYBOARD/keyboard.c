#include "includes.h"

//S1  - S4      1/2/3/A
//S5  - S8		4/5/6/B
//S9  - S12		7/8/9/C
//S13 - S16		*/0/#/D


/*
		  P8模块		 矩阵键盘		TFTLCD模块
DCMI_SCL	PD6		4	*	R1	 *	PD0		8		FSMC_D2
DCMI_SDA	PD7		6	*	R2	 *	PE7		10		FSMC_D4
DCMI_D0		PC6		8	*	R3	 *	PE9		12		FSMC_D6
DCMI_D2		PC8		10	*	R4	 *	PE11	14		FSMC_D8
				↑OUT 				↓IN
DCMI_D4		PC11	12	*	C1	 *	PD15	7		FSMC_D1
DCMI_D6		PE5		14	*	C2	 *	PD1		9		FSMC_D3
DCMI_PCLK	PA6		16	*	C3	 *	PE8		11		FSMC_D5
DCMI_PWDN	PG9		18	*	C4	 *	PE10	13		FSMC_D7
*/

void Matrix_Keyboard_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//使能端口C的硬件时钟，就是对端口C供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    //使能端口E的硬件时钟，就是对端口E供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 
	
    //PD6 PD7 PC6 PC8      //PC11 PE5 PA6 PG9
	//使用GPIO_Init来配置引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;        //输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;       //推挽输出模式,默认的
	GPIO_InitStructure.GPIO_Pin=  GPIO_Pin_1 | GPIO_Pin_15; //指定第6 7根引脚2 1
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;     //高速，但是功耗是最高
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  //无需上下拉（亦可使能下拉电阻）
    GPIO_Init(GPIOD,&GPIO_InitStructure);              //C口
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8 | GPIO_Pin_10;            //指定第8根引脚 3 4
    GPIO_Init(GPIOE,&GPIO_InitStructure);              //F口
    
	/* 矩阵键盘输入模式必须使能内部上拉电阻，引脚悬空的时候为固定的高电平 */
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	   //上拉
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;         //输入模式
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 ;            //指定第5根引脚1
    GPIO_Init(GPIOD,&GPIO_InitStructure);              //E口
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7| GPIO_Pin_9| GPIO_Pin_11;            //指定第6根引脚234
    GPIO_Init(GPIOE,&GPIO_InitStructure);              //G口
    
}

//
char Get_Key_Borad(void)
{
	//PD6 PD7 PC6 PC8      
    PDout(15) = 0;
	PDout(1)  = 1;
	PEout(8)  = 1;
	PEout(10) = 1;
    delay_ms(2);
	
    //PC11 PE5 PA6 PG9
    if( PDin(0) == 0 ) return '1';
    else if( PEin(7) == 0 ) return '4';
    else if( PEin(9) == 0 ) return '7';
    else if( PEin(11) == 0 ) return '*';
    
	PDout(15) = 1;
	PDout(1)  = 0;
	PEout(8)  = 1;
	PEout(10) = 1;
    delay_ms(2);

    //PC11 PE5 PA6 PG9
    if( PDin(0) == 0 ) return '2'; 
    else if( PEin(7) == 0 )  return '5';
    else if( PEin(9) == 0 )  return '8'; 
    else if( PEin(11) == 0 ) return '0';
    
	PDout(15) = 1;
	PDout(1)  = 1;
	PEout(8)  = 0;
	PEout(10) = 1;
    delay_ms(2);
    //PC11 PE5 PA6 PG9
    if( PDin(0) == 0 ) return '3'; 
    else if( PEin(7) == 0 )  return '6'; 
    else if( PEin(9) == 0 )  return '9'; 
    else if( PEin(11) == 0 ) return '#';
    
	PDout(15) = 1;
	PDout(1)  = 1;
	PEout(8)  = 1;
	PEout(10) = 0;
    delay_ms(2);
    //PC11 PE5 PA6 PG9
    if( PDin(0) == 0 ) return 'A'; 
    else if( PEin(7) == 0 )  return 'B'; 
    else if( PEin(9) == 0 )  return 'C'; 
    else if( PEin(11) == 0 ) return 'D';
	return 'N';
}
//


char Confirm_Key_Borad(void)
{
static 	char key_sta=0;
static 	char key_old='N';	
		char key_val='N';
		char key_cur='N';
	
    /* 使用状态机思想得到按键的状态 */
    switch(key_sta)
    {
        case 0://获取按下的按键
        {
            key_cur = Get_Key_Borad();	

            if(key_cur != 'N')
            {
                key_old = key_cur;
                key_sta=1;
            }	
        }break;
        
        
        case 1://确认按下的按键
        {
			
            key_cur = Get_Key_Borad();	
            
            if((key_cur != 'N') && (key_cur == key_old))
            {					
                key_sta=2;				
            }	
            
        }break;
    
        case 2://获取释放的按键
        {
            
            key_cur = Get_Key_Borad();	
                
            if(key_cur == 'N')
            {
				key_val = key_old;			
                key_sta=0;					
                key_old =  'N';
 
            }
        }break;
        
        default:break;
    }

	 return key_val;
}






//




