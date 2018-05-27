 
#ifndef __BLUX_V30B_H
#define __BLUX_V30B_H


#include "M051Series.h"

typedef unsigned          char uint8;
typedef unsigned short     int uint16;
typedef unsigned          int uint32;
typedef          float fp32;
/*-----------------------------�ṹ�嶨��---------------------------------*/

/*-----------------------------�궨��---------------------------------*/
#define         B_LUX_SCL0      (0x01<<2)
#define         B_LUX_SDA0      (0x01<<3)

//���Ŷ���
#define B_LUX_SCL0_O            //GPIOB9 �������
#define B_LUX_SCL0_H    	  P44=1
#define B_LUX_SCL0_L    	  P44=0

#define B_LUX_SDA0_O        	//GPIOB10 �������
#define B_LUX_SDA0_H   		  P45=1
#define B_LUX_SDA0_L   		  P45=0

#define B_LUX_SDA0_I         //��������
#define B_LUX_SDA0_DAT        P45

#define	B_LUX_SlaveAddress	  0x94                                          //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�

/*-----------------------------��������-------------------------------*/
void B_LUX_delay_nms(uint16 k);
void B_LUX_Init(void);

void B_LUX_Single_Write(uint8 REG_Address);                    //����д������
uint8 B_LUX_Single_Read(uint8 REG_Address);                     //������ȡ�ڲ��Ĵ�������
void B_LUX_Multiple_read(void);                               //�����Ķ�ȡ�ڲ��Ĵ�������
//------------------------------------
void B_LUX_Delay5us(void);
void B_LUX_Delay5ms(void);
void B_LUX_Start(void);                          //��ʼ�ź�
void B_LUX_Stop(void);                           //ֹͣ�ź�
void B_LUX_SendACK(uint8 ack);                  //Ӧ��ACK
uint8  B_LUX_RecvACK(void);                     //��ack
void B_LUX_SendByte(uint8 dat);                 //IIC�����ֽ�д
uint8 B_LUX_RecvByte(void);                     //IIC�����ֽڶ�

void B_LUX_GetLux(uint32 *vLux);

void Blux_v3bPoll(void);





#endif