 
#ifndef __BLUX_V30B_H
#define __BLUX_V30B_H


#include "M051Series.h"

typedef unsigned          char uint8;
typedef unsigned short     int uint16;
typedef unsigned          int uint32;
typedef          float fp32;
/*-----------------------------结构体定义---------------------------------*/

/*-----------------------------宏定义---------------------------------*/
#define         B_LUX_SCL0      (0x01<<2)
#define         B_LUX_SDA0      (0x01<<3)

//引脚定义
#define B_LUX_SCL0_O            //GPIOB9 推挽输出
#define B_LUX_SCL0_H    	  P44=1
#define B_LUX_SCL0_L    	  P44=0

#define B_LUX_SDA0_O        	//GPIOB10 推挽输出
#define B_LUX_SDA0_H   		  P45=1
#define B_LUX_SDA0_L   		  P45=0

#define B_LUX_SDA0_I         //浮空输入
#define B_LUX_SDA0_DAT        P45

#define	B_LUX_SlaveAddress	  0x94                                          //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改

/*-----------------------------函数声明-------------------------------*/
void B_LUX_delay_nms(uint16 k);
void B_LUX_Init(void);

void B_LUX_Single_Write(uint8 REG_Address);                    //单个写入数据
uint8 B_LUX_Single_Read(uint8 REG_Address);                     //单个读取内部寄存器数据
void B_LUX_Multiple_read(void);                               //连续的读取内部寄存器数据
//------------------------------------
void B_LUX_Delay5us(void);
void B_LUX_Delay5ms(void);
void B_LUX_Start(void);                          //起始信号
void B_LUX_Stop(void);                           //停止信号
void B_LUX_SendACK(uint8 ack);                  //应答ACK
uint8  B_LUX_RecvACK(void);                     //读ack
void B_LUX_SendByte(uint8 dat);                 //IIC单个字节写
uint8 B_LUX_RecvByte(void);                     //IIC单个字节读

void B_LUX_GetLux(uint32 *vLux);

void Blux_v3bPoll(void);





#endif