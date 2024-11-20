#include "stm32f4xx.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "esp8266.h"
#include "esp8266_mqtt.h"
#include "includes.h"


const uint8_t g_packet_heart_reply[2] = {0xc0,0x00};


char  g_mqtt_msg[526];

uint32_t g_mqtt_tx_len;

//MQTT��������
void mqtt_send_bytes(uint8_t *buf,uint32_t len)
{
    esp8266_send_bytes(buf,len);
}

//����������
int32_t mqtt_send_heart(void)
{	
	uint8_t buf[2]={0xC0,0x00};
    uint32_t cnt=2;
    uint32_t wait=0;	
	
#if 0	
	mqtt_send_bytes(buf,2);
	return 0;
#else
    while(cnt--)
    {	
		mqtt_send_bytes(buf,2);
		memset((void *)g_esp8266_rx_buf,0,sizeof(g_esp8266_rx_buf));
		g_esp8266_rx_cnt=0;	
		
		wait=3000;//�ȴ�3sʱ��
		
		while(wait--)
		{
			delay_ms(1);

			//���������Ӧ�̶���ͷ
			if((g_esp8266_rx_buf[0]==0xD0) && (g_esp8266_rx_buf[1]==0x00)) 
			{
				dgb_printf_safe("������Ӧȷ�ϳɹ�������������\r\n");
				return 0;
			}
		}
	}
	dgb_printf_safe("������Ӧȷ��ʧ�ܣ�����������\r\n");
	return -1;
#endif	

}

//MQTT�������Ͽ�
void mqtt_disconnect(void)
{
	uint8_t buf[2]={0xe0,0x00};
	
    mqtt_send_bytes(buf,2);
	
	esp8266_disconnect_server();
}


//MQTT���ӷ������Ĵ������
int32_t mqtt_connect(char *client_id,char *user_name,char *password)
{
    uint32_t client_id_len = strlen(client_id);
    uint32_t user_name_len = strlen(user_name);
    uint32_t password_len = strlen(password);
    uint32_t data_len;
    uint32_t cnt=2;
    uint32_t wait=0;
    g_mqtt_tx_len=0;
	
    //�ɱ䱨ͷ+Payload  ÿ���ֶΰ��������ֽڵĳ��ȱ�ʶ
    data_len = 10 + (client_id_len+2) + (user_name_len+2) + (password_len+2);

    //�̶���ͷ
    //���Ʊ�������
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 0x10;		//MQTT Message Type CONNECT
    //ʣ�೤��(�������̶�ͷ��)
    do
    {
        uint8_t encodedByte = data_len % 128;
        data_len = data_len / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( data_len > 0 )
            encodedByte = encodedByte | 128;
        g_esp8266_tx_buf[g_mqtt_tx_len++] = encodedByte;
    } while ( data_len > 0 );

    //�ɱ䱨ͷ
    //Э����
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 0;        	// Protocol Name Length MSB
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 4;        	// Protocol Name Length LSB
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 'M';        // ASCII Code for M
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 'Q';        // ASCII Code for Q
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 'T';        // ASCII Code for T
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 'T';        // ASCII Code for T
    //Э�鼶��
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 4;        	// MQTT Protocol version = 4
    //���ӱ�־
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 0xc2;        // conn flags
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 0;        	// Keep-alive Time Length MSB
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 60;        	// Keep-alive Time Length LSB  60S������

    g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE1(client_id_len);// Client ID length MSB
    g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE0(client_id_len);// Client ID length LSB
    memcpy(&g_esp8266_tx_buf[g_mqtt_tx_len],client_id,client_id_len);
    g_mqtt_tx_len += client_id_len;

    if(user_name_len > 0)
    {
        g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE1(user_name_len);		//user_name length MSB
        g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE0(user_name_len);    	//user_name length LSB
        memcpy(&g_esp8266_tx_buf[g_mqtt_tx_len],user_name,user_name_len);
        g_mqtt_tx_len += user_name_len;
    }

    if(password_len > 0)
    {
        g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE1(password_len);		//password length MSB
        g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE0(password_len);    	//password length LSB
        memcpy(&g_esp8266_tx_buf[g_mqtt_tx_len],password,password_len);
        g_mqtt_tx_len += password_len;
    }

	
    while(cnt--)
    {
        memset((void *)g_esp8266_rx_buf,0,sizeof(g_esp8266_rx_buf));
		g_esp8266_rx_cnt=0;
		
        mqtt_send_bytes(g_esp8266_tx_buf,g_mqtt_tx_len);
		
        wait=3000;//�ȴ�3sʱ��
		
        while(wait--)
        {
			delay_ms(1);

			//�������ȷ�Ϲ̶���ͷ
            if((g_esp8266_rx_buf[0]==0x20) && (g_esp8266_rx_buf[1]==0x02)) 
            {
				if(g_esp8266_rx_buf[3] == 0x00)
				{
					dgb_printf_safe("�����ѱ��������˽��ܣ�����ȷ�ϳɹ�\r\n");
					return 0;//���ӳɹ�
				}
				else
				{
					switch(g_esp8266_rx_buf[3])
					{
						case 1:dgb_printf_safe("�����Ѿܾ�����֧�ֵ�Э��汾\r\n");
						break;
						case 2:dgb_printf_safe("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ��\r\n");
						break;		
						case 3:dgb_printf_safe("�����Ѿܾ�������˲�����\r\n");
						break;		
						case 4:dgb_printf_safe("�����Ѿܾ�����Ч���û�������\r\n");
						break;	
						case 5:dgb_printf_safe("�����Ѿܾ���δ��Ȩ\r\n");
						break;
						default:dgb_printf_safe("δ֪��Ӧ\r\n");
						break;
					}
					return 0;
				} 
            }  
        }
    }
	
    return -1;
}

//MQTT����/ȡ���������ݴ������
//topic       ����
//qos         ��Ϣ�ȼ�
//whether     ����/ȡ�����������
int32_t mqtt_subscribe_topic(char *topic,uint8_t qos,uint8_t whether)
{
    
	
    uint32_t cnt=2;
    uint32_t wait=0;
	
    uint32_t topiclen = strlen(topic);

    uint32_t data_len = 2 + (topiclen+2) + (whether?1:0);//�ɱ䱨ͷ�ĳ��ȣ�2�ֽڣ�������Ч�غɵĳ���
	
	g_mqtt_tx_len=0;
	
    //�̶���ͷ
    //���Ʊ�������
    if(whether) 
		g_esp8266_tx_buf[g_mqtt_tx_len++] = 0x82; //��Ϣ���ͺͱ�־����
    else	
		g_esp8266_tx_buf[g_mqtt_tx_len++] = 0xA2; //ȡ������

    //ʣ�೤��
    do
    {
        uint8_t encodedByte = data_len % 128;
        data_len = data_len / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( data_len > 0 )
            encodedByte = encodedByte | 128;
        g_esp8266_tx_buf[g_mqtt_tx_len++] = encodedByte;
    } while ( data_len > 0 );

    //�ɱ䱨ͷ
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 0;				//��Ϣ��ʶ�� MSB
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 0x01;           //��Ϣ��ʶ�� LSB
	
    //��Ч�غ�
    g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE1(topiclen);//���ⳤ�� MSB
    g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE0(topiclen);//���ⳤ�� LSB
    memcpy(&g_esp8266_tx_buf[g_mqtt_tx_len],topic,topiclen);
	
    g_mqtt_tx_len += topiclen;

    if(whether)
    {
        g_esp8266_tx_buf[g_mqtt_tx_len++] = qos;//QoS����
    }


    while(cnt--)
    {
		g_esp8266_rx_cnt=0;
        memset((void *)g_esp8266_rx_buf,0,sizeof(g_esp8266_rx_buf));
        mqtt_send_bytes(g_esp8266_tx_buf,g_mqtt_tx_len);
		
        wait=3000;//�ȴ�3sʱ��
        while(wait--)
        {
			delay_ms(1);
			
			//��鶩��ȷ�ϱ�ͷ
            if(g_esp8266_rx_buf[0]==0x90)
            {
				dgb_printf_safe("Subscription topic confirmed successfully\r\n");
				
				//��ȡʣ�೤��
				if(g_esp8266_rx_buf[1]==3)
				{
					dgb_printf_safe("Success - Maximum QoS 0 is %02X\r\n",g_esp8266_rx_buf[2]);
					dgb_printf_safe("Success - Maximum QoS 2 is %02X\r\n",g_esp8266_rx_buf[3]);		
					dgb_printf_safe("Failure is %02X\r\n",g_esp8266_rx_buf[4]);	
				}
				//��ȡʣ�೤��
				if(g_esp8266_rx_buf[1]==2)
				{
					dgb_printf_safe("Success - Maximum QoS 0 is %02X\r\n",g_esp8266_rx_buf[2]);
					dgb_printf_safe("Success - Maximum QoS 2 is %02X\r\n",g_esp8266_rx_buf[3]);			
				}				
				
				//��ȡʣ�೤��
				if(g_esp8266_rx_buf[1]==1)
				{
					dgb_printf_safe("Success - Maximum QoS 0 is %02X\r\n",g_esp8266_rx_buf[2]);		
				}	
			
                return 0;//���ĳɹ�
            }
            
        }
    }
	
    if(cnt) 
		return 0;	//���ĳɹ�
	
    return -1;
}

//MQTT�������ݴ������
//topic   ����
//message ��Ϣ
//qos     ��Ϣ�ȼ�
uint32_t mqtt_publish_data(char *topic, char *message, uint8_t qos)
{
static 
	uint16_t id=0;	
    uint32_t topicLength = strlen(topic);
    uint32_t messageLength = strlen(message);

    uint32_t data_len;
	uint8_t encodedByte;

    g_mqtt_tx_len=0;
    //��Ч�غɵĳ����������㣺�ù̶���ͷ�е�ʣ�೤���ֶε�ֵ��ȥ�ɱ䱨ͷ�ĳ���
    //QOSΪ0ʱû�б�ʶ��
    //���ݳ���             ������   ���ı�ʶ��   ��Ч�غ�
    if(qos)	data_len = (2+topicLength) + 2 + messageLength;
    else	data_len = (2+topicLength) + messageLength;

    //�̶���ͷ
    //���Ʊ�������
    g_esp8266_tx_buf[g_mqtt_tx_len++] = 0x30;    // MQTT Message Type PUBLISH

    //ʣ�೤��
    do
    {
        encodedByte = data_len % 128;
        data_len = data_len / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( data_len > 0 )
            encodedByte = encodedByte | 128;
        g_esp8266_tx_buf[g_mqtt_tx_len++] = encodedByte;
    } while ( data_len > 0 );

    g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE1(topicLength);//���ⳤ��MSB
    g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE0(topicLength);//���ⳤ��LSB
	
    memcpy(&g_esp8266_tx_buf[g_mqtt_tx_len],topic,topicLength);//��������
	
    g_mqtt_tx_len += topicLength;

    //���ı�ʶ��
    if(qos)
    {
        g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE1(id);
        g_esp8266_tx_buf[g_mqtt_tx_len++] = BYTE0(id);
        id++;
    }
	
    memcpy(&g_esp8266_tx_buf[g_mqtt_tx_len],message,messageLength);
	
    g_mqtt_tx_len += messageLength;
	

	mqtt_send_bytes(g_esp8266_tx_buf,g_mqtt_tx_len);
	
	
	//���ǵ�Qos�ȼ����õ���00����˰�����������ƽ̨��û�з�����Ӧ��Ϣ��
	
	return g_mqtt_tx_len;
}



//�豸״̬�ϱ�
void mqtt_report_devices_status(uint8_t way)
{
    uint8_t led_1_sta = GPIO_ReadOutputDataBit(GPIOF,GPIO_Pin_9) ? 0:1;

    //�ѿ�������ص�״̬��������sdgb_printf_safe������ŵ�һ��������ٰѸ���������MQTTЭ��������Ϣ����
    //sdgb_printf_safe(str,"a=%d",a);
    //��Ҫ���ġ�temperature���͡�CurrentHumidity��Ϊ��Ӧ��ƽ̨�豸��Ϣ��
    sprintf(g_mqtt_msg,
            "{\"method\":\"thing.service.property.set\",\"id\":\"0001\",\"params\":{\
		\"switch_led_1\":%d,\
		\"locked\":%d,\
		\"way\":%d,\
	},\"version\":\"1.0.0\"}",
 
            led_1_sta,
            lock,
			way
	);

    //�ϱ���Ϣ��ƽ̨������
    mqtt_publish_data(MQTT_PUBLISH_TOPIC,g_mqtt_msg,0);
}

int32_t esp8266_mqtt_init(void)
{
	int32_t rt;
	
	//esp8266��ʼ��
	esp8266_init();


	//�˳�͸��ģʽ����������ATָ��
	rt=esp8266_exit_transparent_transmission();
	
	if(rt)
	{
		dgb_printf_safe("esp8266_exit_transparent_transmission fail\r\n");
		return -1;
	}	
	dgb_printf_safe("esp8266_exit_transparent_transmission success\r\n");
	delay_ms(2000);
	
	//��λģ��
	rt=esp8266_reset();
	if(rt)
	{
		dgb_printf_safe("esp8266_reset fail\r\n");
		return -2;
	}
	dgb_printf_safe("esp8266_reset success\r\n");
	delay_ms(2000);	
	
	//�رջ���
	rt=esp8266_enable_echo(0);
	if(rt)
	{
		dgb_printf_safe("esp8266_enable_echo(0) fail\r\n");
		return -3;
	}	
	dgb_printf_safe("esp8266_enable_echo(0)success\r\n");
	delay_ms(2000);	
		
	//�����ȵ�
	rt = esp8266_connect_ap(WIFI_SSID,WIFI_PASSWORD);
	if(rt)
	{
		dgb_printf_safe("esp8266_connect_ap fail\r\n");
		return -4;
	}	
	dgb_printf_safe("esp8266_connect_ap success\r\n");
	delay_ms(2000);
	
	rt =esp8266_connect_server("TCP",MQTT_BROKERADDRESS,1883);
	if(rt)
	{
		dgb_printf_safe("esp8266_connect_server fail\r\n");
		return -5;
	}	
	dgb_printf_safe("esp8266_connect_server success\r\n");
	delay_ms(2000);
	
	//����͸��ģʽ
	rt =esp8266_entry_transparent_transmission();
	if(rt)
	{
		dgb_printf_safe("esp8266_entry_transparent_transmission fail\r\n");
		return -6;
	}	
	dgb_printf_safe("esp8266_entry_transparent_transmission success\r\n");
	delay_ms(2000);
	
	if(mqtt_connect(MQTT_CLIENTID, MQTT_USARNAME, MQTT_PASSWD))
	{
		dgb_printf_safe("mqtt_connect fail\r\n");
		return -7;	
	
	}
	dgb_printf_safe("mqtt_connect success\r\n");
	delay_ms(2000);		
	
	if(mqtt_subscribe_topic(MQTT_SUBSCRIBE_TOPIC,0,1))
	{
		dgb_printf_safe("mqtt_subscribe_topic fail\r\n");
		return -8;
	}	
	
	dgb_printf_safe("mqtt_subscribe_topic success\r\n");
	
	return 0;
}


