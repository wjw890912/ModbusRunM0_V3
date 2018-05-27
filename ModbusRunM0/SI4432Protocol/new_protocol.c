
/*
******************************
*
*
*  新协议C代码实现
*
*	2013年5月11日
*
*	小ARM菜菜
*
*
*******************************
*/
#include "stm32f10x.h"
unsigned char Data_Stream_Tx_Buf[5]={0};//定义数据流输出缓存
unsigned char Data_stream_Rx_Buf[5]={0};//接收数据流输入缓存



 struct switch_struct
 {
 
   uint8_t Channel_number; //通道号
   uint8_t state;  //灯状态
   uint8_t Spare;  //备用
 
 };

struct Protocol_Structure
{

  uint16_t  dst;  //目标地址
  uint16_t  src;  //源地址
  uint8_t  dev_type; //设备类型
  struct switch_struct s;

};



/*
  返回值： 指向数据后别区域的指针
  输入：   结构，发送缓存
  功能：	 一个封装功能的小小纠结一下
  描述：This functionality is encapsulated data stream and intends to send the next step, 
        and returns a pointer to a pointer to the data to be transmitted handle by 小ARM菜菜

*/
uint8_t *Package_Data_Stream(struct Protocol_Structure*pbuf,uint8_t *buf)
{
				 uint16_t dst,src;
				 uint8_t  lsb,msb;
				 uint8_t  lsb1,msb1;
				 uint8_t  type;
				 uint8_t  chnl;
				 uint8_t state;
                 uint8_t temp;
				
			 //开始吧.......
				dst=pbuf->dst;
				dst&=0x0fff;
			
				src=pbuf->src;			//抽取。。。。。
				src&=0x0fff;


				lsb=dst;
				msb=(dst>>8)&0x00ff;
				msb<<=4;   //源地址后四位在前，高字节

			    lsb1=src;
				msb1=(src>>8)&0x00ff;

				temp=msb+msb1;//组合数据

				//在解码时要注意此处的组合逻辑

				*buf=lsb;  //低8位目标地址
				*(buf+1)=temp; //高4位目标和源的组合
				*(buf+2)=lsb1; //低8位源地址



			    type  =	pbuf->dev_type;
				type&=0x07;

				chnl  =	 pbuf->s.Channel_number;
				chnl &=	0x07;

				state =	pbuf->s.state;
				state &=0x03;

				 temp=0;

				 temp= (type<<5)+(chnl<<2)+state;

				*(buf+3)=temp; //D3数据
				*(buf+4)='W'; //D4数据备用
				return buf;

}
  /*
  返回值： 无
  输入：   结构，接收缓存
  功能：	 一个解封装功能的小小纠结一下
  描述：Solution will match the data packet processing, 
        and reported to the application layer data by 小ARM菜菜

*/
void  Unpack_data_stream (struct Protocol_Structure*pbuf,uint8_t *buf)
  {
  
  
				 uint16_t dst,src;
				 uint8_t  lsb,msb;
				 uint8_t  lsb1,msb1;
				 uint8_t  type;
				 uint8_t  chnl;
				 uint8_t state;
                 uint8_t temp;




				lsb=*buf;  //低8位目标地址
				temp=*(buf+1); //高4位目标和源的组合
			    lsb1=*(buf+2); //低8位源地址

			   msb=(temp>>4)&0x0f;
			   msb1=temp&0x0f;

				dst=(uint16_t)msb+lsb;
				src=(uint16_t)msb1+lsb1;

				   pbuf->dst=dst;
			       pbuf->src=src;


				temp=*(buf+3);

				 type  = (temp>>5)&0x7;

				 chnl  = (temp>>2)&0x7;
		
				 state = (temp)&0x3;

				
				 pbuf->dev_type  =  type ;
				 pbuf->s.Channel_number = chnl;
				 pbuf->s.state = state;

  
  }





















