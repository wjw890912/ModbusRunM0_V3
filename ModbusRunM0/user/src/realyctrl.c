
#include <string.h>
#include <stdio.h>
#include "realyctrl.h"
#include  "main.h"
extern UCHAR *Slave_ucSDiscInBuf; //
extern UCHAR *Slave_ucSCoilBuf;//信号寄存器
extern USHORT  *Slave_usSRegInBuf; //输入寄存器
extern USHORT  *Slave_usSRegHoldBuf;//保持寄存器
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS] ;
USHORT old[REALY_NUM]={0},new[REALY_NUM]={0};

#ifdef  CO2_1_8MODBUS-RTU_RELAY
void HWRealyInit(void)
{


	 #ifdef USE_RELAY 

	  GPIO_SetMode(P1, BIT0|BIT1|BIT4, GPIO_PMD_OUTPUT);//realy 
	  GPIO_SetMode(P0, BIT0|BIT1|BIT2|BIT3, GPIO_PMD_OUTPUT);//realy
	  GPIO_SetMode(P4, BIT2, GPIO_PMD_OUTPUT);//realy 
	
	  P10=0;
	  P11=0;
	  P14=0;
	  P00=0;
	  P01=0;
	  P02=0;
	  P03=0;
	  P42=0;

	 #endif

	 #ifdef USE_KEY 
	 GPIO_SetMode(P2, BIT2|BIT3|BIT4, GPIO_PMD_QUASI);//key
	 GPIO_SetMode(P4, BIT0, GPIO_PMD_QUASI);//key

	 #endif
	
	  
} 


void RealyCtrl(Channel_Type Channel,Status_Type status)
{


	  switch (Channel)
	  {

	  	 case CH1:
		 {
		 
		   if(status==OPEN)
		   {
		   
			   CH1_OPEN;
			  

		   }
		   else
		   if(status==CLOSE)
		   {
		   	   CH1_CLOSE;
			 
		   }


		 break;
		 }
	  	 case CH2:
		 {
		 	 if(status==OPEN)
		   {
		   
				 CH2_OPEN;
				
		   }
		   else
		   if(status==CLOSE)
		   {
		   
				  CH2_CLOSE;
				  
		   }
		 
		 break;
		 }
		 case CH3:
		 {
		 	 if(status==OPEN)
		   {
		   
				CH3_OPEN;
			
		   }
		   else
		   if(status==CLOSE)
		   {
		   	   CH3_CLOSE;
			
		   }


		 break;
		 }
		 case CH4:
		 {
		 	  if(status==OPEN)
		   {
		   
				CH4_OPEN;
			

		   }
		   else
		   if(status==CLOSE)
		   {
		   		 CH4_CLOSE;
			

		   }
		 
		 break;
		 }
		 case CH5:
		 {
		     if(status==OPEN)
		   {
		   
				CH5_OPEN;
		

		   }
		   else
		   if(status==CLOSE)
		   {
		   		 CH5_CLOSE;
				

		   }
		 
		 break;
		 }
		 case CH6:
		 {
		 	 if(status==OPEN)
		   {
		   
				CH6_OPEN;
		

		   }
		   else
		   if(status==CLOSE)
		   {
		   		 CH6_CLOSE;
				

		   }
		 
		 break;
		 }
		 case CH7:
		 {
		 	 if(status==OPEN)
		   {
		   
				CH7_OPEN;
		

		   }
		   else
		   if(status==CLOSE)
		   {
		   		 CH7_CLOSE;
				

		   }
		 
		 break;
		 }
		 case CH8:
		 {
		 	  if(status==OPEN)
		   {
		   
				CH8_OPEN;
		

		   }
		   else
		   if(status==CLOSE)
		   {
		   		 CH8_CLOSE;
				

		   }
		 
		 
		 break;
          }

		  /*reserve9-24*/
		 case CH9:{break;}
		 case CH10:{break;}
		 case CH11:{break;}
		 case CH12:{break;}
		 case CH13:{break;}
		 case CH14:{break;}
		 case CH15:{break;}
		 case CH16:{break;}
		 case CH17:{break;}
		 case CH18:{break;}
		 case CH19:{break;}
		 case CH20:{break;}
		 case CH21:{break;}
		 case CH22:{break;}
		 case CH23:{break;}
		 case CH24:{break;}
	  
	  }




}


void RealyPoll(void)
{
	char i;

	for(i=CH1;i<REALY_NUM;i++)
	{	   new[i]=	usSRegHoldBuf[i];

			if (new[i]!=old[i])
			{
				  if(new[i]==0)
				  {
				    RealyCtrl(i,CLOSE);
				  
				  }
				  else
				  if(new[i]==1)
				  {
				  	 RealyCtrl(i,OPEN);
				  }
				   old[i]=new[i];
			}

	}


}



#define K1  0x01
#define K2  0x02
#define K3  0x04
#define K4  0x08


#define GET_BUTTON1    P24
#define GET_BUTTON2    P40
#define GET_BUTTON3    P23
#define GET_BUTTON4    P22
  
unsigned char Trg;
unsigned char Cont;
unsigned char button_data=0xff;

 void KeyRead( void )
 {
	 unsigned char  ReadData=0;

	 if(!GET_BUTTON1)
	 {
	 button_data=0xfe;//生成按键值
	 }
	 else
	 if(!GET_BUTTON2)
	 {
	 button_data=0xfd;
	 }
	 else
	 if(!GET_BUTTON3)
	 {
	 button_data=0xfb;
	 }
	 else
	 if(!GET_BUTTON4)
	 {
	  button_data=0xf7;
	 }
	 else
	 {
	  button_data=0xff;
	 }
     ReadData = button_data^0xff;    
     Trg = ReadData & (ReadData ^ Cont);   
     Cont = ReadData;   
 }
 void KeyProc(void)
 {	

      if (Trg & K1) // 如果按下的是K1
     {
 		 if(usSRegHoldBuf[0])
		 {
		 	usSRegHoldBuf[0]=0;		 
		 }
		 else
		 {
		   usSRegHoldBuf[0]=1;
		 }
				
     }
	  if (Trg & K2) // 如果按下的是K2
     {
          if(usSRegHoldBuf[1])
		 {
		 	usSRegHoldBuf[1]=0;		 
		 }
		 else
		 {
		   usSRegHoldBuf[1]=1;
		 }
     }

	 if (Trg & K3) // 如果按下的是K3
	 {
	 	 if(usSRegHoldBuf[2])
		 {
		 	usSRegHoldBuf[2]=0;		 
		 }
		 else
		 {
		   usSRegHoldBuf[2]=1;
		 }
	 } 
	 if (Trg & K4) // 如果按下的是K4
	 {
	 	   if(usSRegHoldBuf[3])
		 {
		 	usSRegHoldBuf[3]=0;		 
		 }
		 else
		 {
		   usSRegHoldBuf[3]=1;
		 }
	 } 
 
 }




extern unsigned long  Systick;
unsigned long  KeyScanTime=0;
void KeyPool(void)
{

	if(Systick-KeyScanTime>=(50))	//ms
	{
	     KeyScanTime=Systick;


		   KeyRead();
		   KeyProc();

	
	}
		/*sample key data in idle*/
	usSRegHoldBuf[10] = P24;
	usSRegHoldBuf[11] = P40;
	usSRegHoldBuf[12] = P23;
	usSRegHoldBuf[13] = P22;


}
#endif

 /*
 获取从机地址
 Get the addr form hardwave switch for HEX .
 */
uint8_t GetHwAddrIint(void)
{


	  uint8_t addr[4]={0};

	 
   /*The first our init the GPIO mode is INPUT */
 GPIO_SetMode(P2, BIT5|BIT6|BIT7, GPIO_PMD_INPUT);
 GPIO_SetMode(P4, BIT4, GPIO_PMD_INPUT);

   	addr[0]=P25;//get bit of ADDR3
	addr[1]=P26;//get bit of ADDR2
	addr[2]=P27;//get bit of ADDR1
	addr[3]=P44;//get bit of ADDR0

	addr[0]<<=0;//get bit of ADDR3
	addr[1]<<=1;//get bit of ADDR2
	addr[2]<<=2;//get bit of ADDR1
	addr[3]<<=3;//get bit of ADDR0


	return (addr[0]+addr[1]+addr[2]+addr[3]);
}


 
