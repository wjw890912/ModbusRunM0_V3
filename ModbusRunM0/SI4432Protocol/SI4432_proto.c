// File Name:SI4432_proto.C
// Author:СARM�˲�&����׳
// Date: 2012��
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
unsigned char Data_Stream_Tx_Buf[5]={0};//�����������������
unsigned char Data_stream_Rx_Buf[5]={0};//�������������뻺��

struct Protocol_Structure RX_si4432;





/*
  ����ֵ�� ָ�����ݺ�������ָ��
  ���룺   �ṹ�����ͻ���
  ���ܣ�	 һ����װ���ܵ�СС����һ��
  ������This functionality is encapsulated data stream and intends to send the next step, 
        and returns a pointer to a pointer to the data to be transmitted handle by СARM�˲�

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
			 //��ʼ��.......

					

				dst=pbuf->dst;
				dst&=0x0fff;
			
				src=pbuf->src;			//��ȡ����������
				src&=0x0fff;


				lsb=dst; //��8λ
				msb=(dst>>8)&0xf;
			   //Դ��ַ����λ��ǰ�����ֽ�
			    lsb1=src;
				msb1=(src>>8)&0x000f;
				msb1=(msb1<<4)&0xf0;

				  temp=s;  //����ͷ
				  temp+=msb	;//�������֡��ʼͷ

				*buf=temp;  //a+��4λDST��ַ
				*(buf+1)=lsb; //dst�ĵ�8λ
				*(buf+2)=lsb1; //��8λԴ��ַ


				 type = pbuf->dev_type;
				 type<<=1;
				  type&=0x0e;

				chnl  =	 pbuf->s.Channel_number;
				chnl &=	0x07;

				chnl>>=2;
				chnl&=0x01;//ȡ��chnl���λ



				 temp=msb1+type+chnl;

				*(buf+3)=temp;//SRC�ĸ�4λ�����͵ķַ�װ

					 //�ٴμ���chanlֵ
			   	  chnl = pbuf->s.Channel_number;
				chnl &=	0x03;//����λ

				

				state =	pbuf->s.state;
				state &=0x03;		  //��λ״̬


				 temp=0;
				 temp=(chnl<<6)+(state<<4)+e;
				*(buf+4)=temp; //D4����

				return buf;

}
  /*
  ����ֵ�� ��
  ���룺   �ṹ�����ջ���
  ���ܣ�	 һ�����װ���ܵ�СС����һ��
  ������Solution will match the data packet processing, 
        and reported to the application layer data by СARM�˲�

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




				lsb=*(buf+1);  //dst��8λĿ���ַ
				temp=*buf; //dst��4λ��ͷ�����

				msb=temp;
				msb&=0x0f;//ȡ������λ

				s=temp>>4;//ȡ��ͷ

					dst=0;
			  dst=((uint16_t) msb<<8)+lsb;  //���DST

				   pbuf->dst=dst;

               lsb1=*(buf+2); //��8λԴ��ַ
			   temp=*(buf+3); //

				type =temp;
				type&=0x0e;		  //���type
				 type>>=1;

				 chnl_t=temp;
				 chnl_t&=0x01;//ȡ��ͨ�����λ


				msb1=temp>>4;//Դ��ַ�ĸ�4λ

			   src=((uint16_t)msb1<<8)+lsb1;//���SRC
				  pbuf->src=src;


				temp=*(buf+4);

					chnl_t<<=2;

				 chnl  = (temp>>6)&0x3;
				 chnl =chnl +chnl_t;
		
				 state = (temp>>4)&0x03;

				  e= temp&0x0f;	//������־

				 pbuf->dev_type  =  type ;
				 pbuf->s.Channel_number = chnl;
				 pbuf->s.state = state;

  
  }


void Light_State_Retrn (struct Protocol_Structure *pFram,eChan_Num Channel,uint8_t state)
{		
			
          struct Protocol_Structure TX_STRUCT;	//���췢������

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


				
			
	send:	TX_STATE = tx_data(Data, 5);//���ͳɹ�
		    switch(TX_STATE)
			 {

			 	case TX_SUCCESS:{

						   break;	 }

				case TX_FAIL:{n++;if(n>2){return ;}goto send;}	//����ʧ��
				case TX_RSSI_CHANNLE_OCCUPANCY:{n++;if(n>2){return ;}goto send;}	//�ŵ���ռ��
				case TX_TIMEOUT: {n++;if(n>2){return ;}goto send;}//��ʱ

			 
			 
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
									LED1_CLOSE;//��LED
								   	MOS1_CLOSE;	  //�ص�

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
									LED2_CLOSE;//��LED
								   	MOS2_CLOSE;	  //�ص�
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
										LED3_CLOSE;//��LED
								   	MOS3_CLOSE;	  //�ص�
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
									LED1_CLOSE;//��LED
								   	MOS1_CLOSE;	  //�ص�
									LED2_CLOSE;//��LED
								   	MOS2_CLOSE;	  //�ص�
									LED3_CLOSE;//��LED
								   	MOS3_CLOSE;	  //�ص�
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

				   case CHAN_1 : { SI4432_Process_86_cmd(CHAN_1, cmd);   break;}//һͨ��
				   case CHAN_2 : { SI4432_Process_86_cmd(CHAN_2, cmd);  break;}//��ͨ��
				   case CHAN_3 : { SI4432_Process_86_cmd(CHAN_3, cmd);  break;} //��ͨ��


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
						    		LED1_CLOSE;//��LED
								   	MOS1_CLOSE;	  //�ص�
									LED2_CLOSE;//��LED
								   	MOS2_CLOSE;	  //�ص�
									LED3_CLOSE;//��LED
								   	MOS3_CLOSE;	  //�ص�
							   	 
									Light_State_Retrn (&RX_si4432,CHAN_ALL,0);
				
									  break;
									  }
		 	 
		                  }

				   
				   
				   
				     break;}	//����ͨ��
				   case CHAN_INVALID : {  break;} //ʧЧ
			
			}




}



 extern uint16_t read_HDT11(void);
 extern uint8_t TX_buf[5]; 
/*��������Э��֡����*/
unsigned char RecvCmdFromSI4432(uint8_t *pBuf,uint8_t leg)
{		 
	 	 uint32_t ID_RX=0;
		  uint16_t temp;


		if(leg!=5)return;	 //���ݰ���������ȷ
		
		  ID_RX=pBuf[0];
		  ID_RX<<=8;
		  ID_RX+= pBuf[1];

		if(ID_RX!=UID)return;	  //id acess
		
		
				
							if((pBuf[2]==0x03)&&(pBuf[3]==0x00)&&(pBuf[4]==0x01)) //��ȡ�¶�ʪ��
							{
								 temp= read_HDT11();
								   TX_buf[0]= UID>>8;
								   TX_buf[1]= UID;
								   TX_buf[2]=0x03; //��ʪ������
								   TX_buf[3]=	 temp;
								   TX_buf[4]=	 temp>>8;	   //����
								 tx_data(TX_buf, 5);
				
							
							}
							else
							if(pBuf[2]==0x00)
							{
				
						  
						  if(pBuf[4]==1)  //���ƹصƣ�
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
						  if(pBuf[4]==2)   //״̬
						  {	TX_buf[0]= UID>>8;
							TX_buf[1]= UID;
						  	TX_buf[2]=0;
							TX_buf[3]=0; 
							TX_buf[4]=DrvGPIO_GetBit(E_PORT0, E_PIN3);
						    tx_data(TX_buf, 5);
				
						  }
						
							
							 }
		




		
		



		
}		   


