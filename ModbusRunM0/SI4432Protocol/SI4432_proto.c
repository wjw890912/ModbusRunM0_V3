// File Name:SI4432_proto.C
// Author:小ARM菜菜&王德壮
// Date: 2012年
 //Q Q:925295580

#include "SI4432_proto.h"
#include "string.h"
#include "SI4432.H"
#include <stdio.h>
#include "M051Series.h"
#include "DrvSYS.h"
#include "DrvGPIO.h"
#include "GPIO.h"
#include "stdint.h"
#include "FMC.H"




uint32_t Authentication_ID=0;
extern uint32_t Authenticated[3];


extern uint32_t UID;
unsigned char Data_Stream_Tx_Buf[5]={0};//定义数据流输出缓存
unsigned char Data_stream_Rx_Buf[5]={0};//接收数据流输入缓存

struct Protocol_Structure RX_si4432;





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
				 uint8_t s=0xa0;
				 uint8_t e=0x0b;
			 //开始吧.......

					

				dst=pbuf->dst;
				dst&=0x0fff;
			
				src=pbuf->src;			//抽取。。。。。
				src&=0x0fff;


				lsb=dst; //低8位
				msb=(dst>>8)&0xf;
			   //源地址后四位在前，高字节
			    lsb1=src;
				msb1=(src>>8)&0x000f;
				msb1=(msb1<<4)&0xf0;

				  temp=s;  //加入头
				  temp+=msb	;//组合数据帧开始头

				*buf=temp;  //a+高4位DST地址
				*(buf+1)=lsb; //dst的低8位
				*(buf+2)=lsb1; //低8位源地址


				 type = pbuf->dev_type;
				 type<<=1;
				  type&=0x0e;

				chnl  =	 pbuf->s.Channel_number;
				chnl &=	0x07;

				chnl>>=2;
				chnl&=0x01;//取出chnl最高位



				 temp=msb1+type+chnl;

				*(buf+3)=temp;//SRC的高4位和类型的分封装

					 //再次计算chanl值
			   	  chnl = pbuf->s.Channel_number;
				chnl &=	0x03;//地两位

				

				state =	pbuf->s.state;
				state &=0x03;		  //两位状态


				 temp=0;
				 temp=(chnl<<6)+(state<<4)+e;
				*(buf+4)=temp; //D4数据

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
				 uint8_t  chnl_t;
				 uint8_t state;
                 uint8_t temp;
				 uint8_t s;
			 uint8_t e;

					s=s;
					e=e;




				lsb=*(buf+1);  //dst低8位目标地址
				temp=*buf; //dst高4位和头的组合

				msb=temp;
				msb&=0x0f;//取出高四位

				s=temp>>4;//取出头

					dst=0;
			  dst=((uint16_t) msb<<8)+lsb;  //求出DST

				   pbuf->dst=dst;

               lsb1=*(buf+2); //低8位源地址
			   temp=*(buf+3); //

				type =temp;
				type&=0x0e;		  //求出type
				 type>>=1;

				 chnl_t=temp;
				 chnl_t&=0x01;//取出通道最高位


				msb1=temp>>4;//源地址的高4位

			   src=((uint16_t)msb1<<8)+lsb1;//求出SRC
				  pbuf->src=src;


				temp=*(buf+4);

					chnl_t<<=2;

				 chnl  = (temp>>6)&0x3;
				 chnl =chnl +chnl_t;
		
				 state = (temp>>4)&0x03;

				  e= temp&0x0f;	//结束标志

				 pbuf->dev_type  =  type ;
				 pbuf->s.Channel_number = chnl;
				 pbuf->s.state = state;

  
  }


void Light_State_Retrn (struct Protocol_Structure *pFram,eChan_Num Channel,uint8_t state)
{		
			
          struct Protocol_Structure TX_STRUCT;	//构造发送数据

			   uint8_t Data[10]={0};
			   uint8_t  TX_STATE=0;
               uint8_t n=0;
 
				

			   	if( Check_si4432(1)==0)return ;	

				  TX_STRUCT.dst=pFram->src;
				  TX_STRUCT.src=UID;//pFram->dst;

				  TX_STRUCT.dev_type=1;

				  TX_STRUCT.s.Channel_number=Channel;
                  TX_STRUCT.s.state=state; 
			
	            Package_Data_Stream(&TX_STRUCT,Data);


				
			
	send:	TX_STATE = tx_data(Data, 5);//发送成功
		    switch(TX_STATE)
			 {

			 	case TX_SUCCESS:{

						   break;	 }

				case TX_FAIL:{n++;if(n>2){return ;}goto send;}	//发送失败
				case TX_RSSI_CHANNLE_OCCUPANCY:{n++;if(n>2){return ;}goto send;}	//信道被占用
				case TX_TIMEOUT: {n++;if(n>2){return ;}goto send;}//超时

			 
			 
			 }




}


void SI4432_Process_86_cmd(eChan_Num channel,uint8_t Cmd)
{
			  static unsigned char a,b,c;

	  switch(channel)
	  {
		case CHAN_1:{	 
					  

						 switch (Cmd)
						 {
						   case LIGHTCTRL_OPEN:{ 
								
							   	 	LED1_OPEN;
									MOS1_OPEN;
									   a=1;
				                 Light_State_Retrn (&RX_si4432,CHAN_1,a);
									break;
									    }
						   case LIGHTCTRL_CLOSE:{ 
						    
							   	 	   a=0;
									LED1_CLOSE;//开LED
								   	MOS1_CLOSE;	  //关灯

								Light_State_Retrn (&RX_si4432,CHAN_1,a);
				
									  break;
									  }
		 	 				 case 2:{ 
						    
								Light_State_Retrn (&RX_si4432,CHAN_1,a);

									  break;
									  }
		                  }

		
					  
		
			
				 break;}

		 case CHAN_2:{
		 

		             

		 			   		 switch (Cmd)
						 {
						   case LIGHTCTRL_OPEN:{ 
							
							   	 	LED2_OPEN;
									MOS2_OPEN;
									b=1;
				               Light_State_Retrn (&RX_si4432,CHAN_2,1);
				
									break;
								    }
						   case LIGHTCTRL_CLOSE:{ 
						    
							   	 	   b=0;
									LED2_CLOSE;//开LED
								   	MOS2_CLOSE;	  //关灯
									Light_State_Retrn (&RX_si4432,CHAN_2,0);
				
									  break;
									  }
		 	 				    case 2:{ 
						    
								
								  Light_State_Retrn (&RX_si4432,CHAN_2,b);
								 
									  break;
									  }
		                  }
				//	   }
		 
		         break;}


		  case CHAN_3:{

					
					
		  						 switch (Cmd)
						 {
						   case LIGHTCTRL_OPEN:{ 
										c=1;
							   	 	 LED3_OPEN;
									MOS3_OPEN;
				
				                    Light_State_Retrn (&RX_si4432,CHAN_3,1);
				
									break;
									    }
						   case LIGHTCTRL_CLOSE:{ 
						    
							   	 		  c=0;
										LED3_CLOSE;//开LED
								   	MOS3_CLOSE;	  //关灯
									Light_State_Retrn (&RX_si4432,CHAN_3,0);
				
									  break;
							  case 2:{ 
						    
								
								  Light_State_Retrn (&RX_si4432,CHAN_3,c);
								 
									  break;
									  }                                                                                                                                                                                                                                 		  }
		 	 
		                  } 
		  			  
		         break;}

			case CHAN_ALL: {
			
			
						 			 switch (Cmd)
						 {
						   case LIGHTCTRL_OPEN:{ 
				
							   	 	LED1_OPEN;
									MOS1_OPEN;
								   	LED2_OPEN;
									MOS2_OPEN;
									LED3_OPEN;
									MOS3_OPEN;
				                    Light_State_Retrn (&RX_si4432,CHAN_ALL,1);
				
									break;
									    }
						   case LIGHTCTRL_CLOSE:{ 
									LED1_CLOSE;//开LED
								   	MOS1_CLOSE;	  //关灯
									LED2_CLOSE;//开LED
								   	MOS2_CLOSE;	  //关灯
									LED3_CLOSE;//开LED
								   	MOS3_CLOSE;	  //关灯
									Light_State_Retrn (&RX_si4432,CHAN_ALL,0);
				
									  break;
									  }
		 	 
		                  }
				   
			
			      break; }



	  }

}
void Si4432_Process_86(uint8_t *pt_data,uint8_t leg)
{
		 
    			uint8_t cmd;
				cmd = *(pt_data+1);
			
			switch(*pt_data)
			{

				   case CHAN_1 : { SI4432_Process_86_cmd(CHAN_1, cmd);   break;}//一通道
				   case CHAN_2 : { SI4432_Process_86_cmd(CHAN_2, cmd);  break;}//二通道
				   case CHAN_3 : { SI4432_Process_86_cmd(CHAN_3, cmd);  break;} //三通道


				   case CHAN_ALL :  { 
				   
				   		    switch (cmd)
						 {
						   case LIGHTCTRL_OPEN:{ 
				
							   	 	LED1_OPEN;
									MOS1_OPEN;
								   	LED2_OPEN;
									MOS2_OPEN;
									LED3_OPEN;
									MOS3_OPEN;

				                    Light_State_Retrn (&RX_si4432,CHAN_ALL,100);
				
									break;
									    }
						   case LIGHTCTRL_CLOSE:{ 
						    		LED1_CLOSE;//开LED
								   	MOS1_CLOSE;	  //关灯
									LED2_CLOSE;//开LED
								   	MOS2_CLOSE;	  //关灯
									LED3_CLOSE;//开LED
								   	MOS3_CLOSE;	  //关灯
							   	 
									Light_State_Retrn (&RX_si4432,CHAN_ALL,0);
				
									  break;
									  }
		 	 
		                  }

				   
				   
				   
				     break;}	//所有通道
				   case CHAN_INVALID : {  break;} //失效
			
			}




}



 extern uint16_t read_HDT11(void);
 extern uint8_t TX_buf[5]; 
/*解析无线协议帧函数*/
unsigned char RecvCmdFromSI4432(uint8_t *pBuf,uint8_t leg)
{		 
	 	 uint32_t ID_RX=0;
		  uint16_t temp;


		if(leg!=5)return;	 //数据包个数不正确
		
		  ID_RX=pBuf[0];
		  ID_RX<<=8;
		  ID_RX+= pBuf[1];

		if(ID_RX!=UID)return;	  //id acess
		
		
				
							if((pBuf[2]==0x03)&&(pBuf[3]==0x00)&&(pBuf[4]==0x01)) //获取温度湿度
							{
								 temp= read_HDT11();
								   TX_buf[0]= UID>>8;
								   TX_buf[1]= UID;
								   TX_buf[2]=0x03; //温湿度类型
								   TX_buf[3]=	 temp;
								   TX_buf[4]=	 temp>>8;	   //数据
								 tx_data(TX_buf, 5);
				
							
							}
							else
							if(pBuf[2]==0x00)
							{
				
						  
						  if(pBuf[4]==1)  //开灯关灯？
						  {
						   TX_buf[0]= UID>>8;
							TX_buf[1]= UID;
						   	TX_buf[2]=0;
							TX_buf[3]=0; 
						    TX_buf[4]=1;
						  	tx_data(TX_buf, 5);
							REALY_UP ;
				
						  }
						  else
						  if(pBuf[4]==0)
						  {
						   TX_buf[0]= UID>>8;
								   TX_buf[1]= UID;
						  TX_buf[4]=0;
						  TX_buf[2]=0;
						  TX_buf[3]=0; 
						  tx_data(TX_buf, 5);
						  REALY_DOWN ;
				
						  }
						  else
						  if(pBuf[4]==2)   //状态
						  {	TX_buf[0]= UID>>8;
							TX_buf[1]= UID;
						  	TX_buf[2]=0;
							TX_buf[3]=0; 
							TX_buf[4]=DrvGPIO_GetBit(E_PORT0, E_PIN3);
						    tx_data(TX_buf, 5);
				
						  }
						
							
							 }
		




		
		



		
}		   


