

/*第三方串口*/

#include <stdio.h>
#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "user_mb_app.h"

#include "M051Series.h"
#include "Uart3th.h"
#include "main.h"

extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS] ;
static 	USHORT  MBTime2rout50us; //time2 base time tick =50us 20KHZ
unsigned char UartReciveBuf[16*8]; //uart reciver buff 
unsigned int  UartLen=0;		   //uart reciver byte lenth 
unsigned char RecivEvent=0;// 1:recived UART byte to buff 0:empty UART buff
unsigned char Time2handel=0;	//定时器标志
int time2cut=0;//计数器
unsigned long  Systick=0,Co2SentTime=0;



//UART
BOOL x3THPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
		eMBParity eParity)
		{
		
		 
     SYS_ResetModule(UART1_RST);
	CLK_SetModuleClock(UART1_MODULE , CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1));
	CLK_EnableModuleClock(UART1_MODULE);
    //Configure UART0 and set UART0 Baudrate 
    UART_Open(UART1, ulBaudRate);
	ENTER_CRITICAL_SECTION(); //关全局中断

	UART_ENABLE_INT(UART1, UART_IER_RDA_IEN_Msk); //接收中断
	
	NVIC_SetPriority(UART1_IRQn,2);
	NVIC_EnableIRQ(UART1_IRQn);

	EXIT_CRITICAL_SECTION(); //开全局中断
	
	return TRUE;
}

 BOOL x3THPortSerialPutByte(CHAR ucByte)//写一个字节到硬件
{

	UART_WRITE(UART1, ucByte);

	return TRUE;
}

BOOL x3THPortSerialGetByte(CHAR * pucByte) //读一个自己从硬件
{	   
	*pucByte = UART_READ(UART1);
	return TRUE;
}




//TIMER


/* ----------------------- Start implementation -----------------------------*/
BOOL x3THPortTimersInit(USHORT usTim2Timerout50us)
{
   
	 MBTime2rout50us= usTim2Timerout50us;
	   // Initial Timer2 TIMER_PERIODIC_MODE  setting   20kHZ
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 20000);
    TIMER_EnableInt(TIMER2);
	NVIC_SetPriority(TMR2_IRQn, 4);
    //Enable Timer2 NVIC 
    NVIC_EnableIRQ(TMR2_IRQn);
    //Start Timer2 counting 
    TIMER_Start(TIMER2); 

	return TRUE;
}

void x3THPortTimersEnable()
{	  
	 Time2handel=1;//开始计时
	 time2cut=0;

//	TIMER_EnableInt(TIMER2);
    //Enable Timer2 NVIC 
   // NVIC_EnableIRQ(TMR2_IRQn);
    //Start Timer2 counting 
    //TIMER_Start(TIMER2);  
	 
}

void x3THPortTimersDisable()
{
   //TIMER_Stop(TIMER2);
	Time2handel=0;//禁止计时
	time2cut=0;//清空计数 
}

 void TMR2_IRQHandler(void)
{
	static 	unsigned  int t;

	 
	 if(TIMER_GetIntFlag(TIMER2) == 1)
    {
		  	  TIMER_ClearIntFlag(TIMER2);

			 t++;
			 if(t>=20)
			 {
			 t=0;
			 Systick++;	 //1ms
			 }

			if(Time2handel)
			{
				 time2cut++;
			   if(time2cut>= MBTime2rout50us)	//超时事件发生
			   {

				RecivEvent=1;
			   	x3THPortTimersDisable();
			   	 //	收到一帧RTU	
			   }
			   
			
			}
			else
			{
				 time2cut=0;
			
			}
				
   }
}

 
void UART1_IRQHandler(void)
{
    
	   
	if(UART_IS_RX_READY(UART1))
	 {
	 
	  x3THPortSerialGetByte((CHAR *)&UartReciveBuf[UartLen]); //read byte to buff
	  UartLen++; //the reciver byte +1
	  x3THPortTimersEnable(); 
	
     }
	 else
	 if(UART_IS_TX_EMPTY(UART1))
	 
	  {
	 
	   
	 }
	 else
	 {
	 
	   //do nothing ....
	 }	  
	
}


void Co2SendCmd(void)
{	

    #ifdef USE_PM25
	uint8_t CO2SentBuf[9]={0xaa,0x02,0x00,0x00,0x00,0x00,0x01,0x67,0xbb};//查询
	uint8_t CO2SentBuf1[9]={0xaa,0x03,0x00,0x00,0x00,0x00,0x01,0x68,0xbb}; //关机
	#endif

	#ifdef USE_CO2 
    uint8_t CO2SentBuf[9]={0xff,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};//读取
	#endif
//	uint8_t CO2SentBuf2[9]={0xff,0x01,0x79,0x00,0x00,0x00,0x00,0x00,0x86};//关闭零点校准
//	uint8_t CO2SentBuf3[9]={0xff,0x01,0x99,2000/256,2000%256,0x00,0x00,0x00,(0xff-(0x01+0x99+(2000/256)+(2000%256)))+1};
	if(Systick-Co2SentTime>=(1800))
	{
	    Co2SentTime=Systick;
	   #ifdef USE_PM25
	   if(usSRegHoldBuf[22]!=0)
	   {
	 
	    	//发送查询PM25命令
	    UART_Write(UART1, CO2SentBuf, 9);
	   }
	   else 
	   {
	   		  //发送关机命令
	   	UART_Write(UART1, CO2SentBuf1, 9);
	   
	   }
	   #endif


	}

  

}	  
void Co2EventPoll(void)
{	   

		#ifdef USE_CO2
		uint8_t sum;
		#endif

		#ifdef USE_PM25
		 uint16_t sum;
		#endif
	 if(RecivEvent)
	 {


		   #ifdef USE_CO2 


			sum=((UartReciveBuf[0]- (UartReciveBuf[1]+
							 UartReciveBuf[2]+
							 UartReciveBuf[3]+
							 UartReciveBuf[4]+
							 UartReciveBuf[5]+
							 UartReciveBuf[6]+
							 UartReciveBuf[7])
		       )+1);

		if(sum==UartReciveBuf[8])
		{

		usSRegHoldBuf[14]= (UartReciveBuf[2]<<8)+UartReciveBuf[3];//get CO2	MSB and LSB
		usSRegHoldBuf[14]=usSRegHoldBuf[14]+ (int16_t)usSRegHoldBuf[8];//CO2补偿
		}
		//process CO2 reciver byte
		 #endif

		 #ifdef USE_PM25
		  
		sum=(   UartReciveBuf[0]+ 
			    UartReciveBuf[1]+
				UartReciveBuf[2]+
				UartReciveBuf[3]+
				UartReciveBuf[4]+
				UartReciveBuf[5]+
				UartReciveBuf[8]
			 );

			    
		  	if(sum==((uint16_t )(UartReciveBuf[6]<<8)+UartReciveBuf[7])) 
			{
			
		usSRegHoldBuf[20]= (UartReciveBuf[4]<<8)+UartReciveBuf[5];//get PM25	MSB and LSB
		usSRegHoldBuf[21]= (UartReciveBuf[2]<<8)+UartReciveBuf[3];//get PM10	MSB and LSB

			
			}    


		 //process pm2.5+pm10 reciver byte
		 #endif


		UartLen=0;	//clear reciver byte lenth 
		RecivEvent=0;//reset

	 }

}

void Co2Init(void)
{

	#ifdef USE_PM25
uint8_t CO2SentBuf3[9]={0xaa,0x01,0x00,0x00,0x00,0x00,0x01,0x66,0xbb};
	#endif

uint8_t CO2SentBuf2[9]={0xff,0x01,0x79,0x00,0x00,0x00,0x00,0x00,0x86};//关闭零点校准

  #ifdef USE_CO2
uint8_t CO2SentBuf3[9]={0xff,0x01,0x99,2000/256,2000%256,0x00,0x00,0x00,(0xff-(0x01+0x99+(2000/256)+(2000%256)))+1};
   #endif

uint8_t CO2SentBuf4[9]={0xff,0x01,0x88,2000/256,2000%256,0x00,0x00,0x00,(0xff-(0x01+0x88+(2000/256)+(2000%256)))+1};

 x3THPortSerialInit(0, 9600,8,MB_PAR_NONE);
 x3THPortTimersInit(( 7UL * 220000UL ) / ( 2UL * /*ulBaudRate*/9600 ))	;

 	//	UART_Write(UART1, CO2SentBuf2, 9);
UART_Write(UART1, CO2SentBuf3, 9);
	   //	UART_Write(UART1, CO2SentBuf4, 9);
	

}










 /*
串口收到任何一个字符开始计时，\
如果定时值没有溢出再来一个字符中断的话表示字节流正在注入，
此时只管对buf进行写入。并记录深度作为字节流的长度
每次中断重新对定时器的值赋值一次，直到最后一个字节，
后面由于没有字节中断，所以最终定时器会产生超时事件，
事件发生后，读取buf和计数字节的值便得到了接受到的字符和len。

*/
