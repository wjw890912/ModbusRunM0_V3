// File Name:SI4432.C
// Author:СARM�˲�
// Date: 2012��
 //Q Q:925295580

#include <stdio.h>
#include "M051Series.h"
#include "DrvSYS.h"
#include "DrvUART.h"
#include "DrvFMC.h"
#include "DrvGPIO.h"
#include "main.h"
#include "USART.h"
#include "GPIO.h"
#include "DrvTIMER.h"
#include"SI4432.H"
#include "WDT.h"
 #define SpiWriteDat8(a,b) spi_rw((a|0x80), b)
extern  uint8_t tx_timeout_en,tx_timeout;

 /*
** ������ : SPI_Write_OneByte
** ����ֵ : None
** ��  �� : u8_writedata--SPIд���һ�ֽ�����
** ��  �� : ������д���ݣ�ÿ��д�� 1 bit
*/
void SPI_Write_OneByte(uint8_t u8_writedata)
{
  uint8_t i;
  
  for(i=0;i<8;i++)
  {
    if(u8_writedata & 0x80)      //�ж����λ�����Ƿ������λ
     SDI_UP;                    //MOSI���1����������׼������1
    else
     SDI_DOWN;                   //MOSI���0����������׼������0
     
   	SCK_UP;                     //����������(SCK��0-->1)�����������ϵ�����д�뵽����
    u8_writedata <<= 1;          //���������Ѿ���������λ
    	SCK_DOWN;                     //����SCK�źţ���ʼ��Ϊ0
  }
}
/*
** ������ : SPI_Read_OneByte
** ����ֵ : temp--SPI��ȡ��һ�ֽ�����
** ��  �� : None
** ��  �� : �½��ض����ݣ�ÿ�ζ�ȡ 1 bit
*/
uint8_t SPI_Read_OneByte(void)
{
  uint8_t i;
  uint8_t temp = 0;
  
  for(i=0;i<8;i++)
  {
   temp <<= 1;       //��ȡMISO 8�������ֵ������temp��֮���Բ����ڡ�SCK = 0�����֮���λ������Ϊ��
                     //��ȡ���1byte�����һλ(��LSB)֮�󣬲�����������
   	SCK_UP;          
   if(GET_SDO)          //��ȡ���λ����������ĩβ��ͨ������λ��ɶ������ֽ�
     temp |= 0x01;
    else
     temp &= ~0x01;
  	SCK_DOWN;          //�½�������(SCK��1-->0)��MISO�ϵ����ݽ������ı䣬�ȶ����ȡ����temp
  }
  
  return temp;
}

/*
** ������: nRF24L01_ReadReg
** ����ֵ: value--��ȡ�Ĵ���ֵ
** �� �� : addr--�Ĵ�����ַ
** ˵ �� : nRF24L01�Ĵ���������
*/
uint8_t nRF24L01_ReadReg(uint8_t addr)
{
    uint8_t value;
    nSEL_DOWN;                   //CSƬѡ����
   SPI_Write_OneByte(addr|0);  //SPIд��ַ����
    value = SPI_Read_OneByte();  //SPI������
    nSEL_UP;                    //CSƬѡ����
    return value;
}

 /*
** ������ : SPI_WriteAndRead_OneByte
** ����ֵ : u8_readdata--SPI��ȡ��һ�ֽ�����
** �� �� : u8_writedata--SPIд���һ�ֽ�����
** �� �� : ������д���½��ض�
*/
uint8_t SPI_WriteAndRead_OneByte(uint8_t u8_writedata)
{
  uint8_t i;
  uint8_t u8_readdata = 0x00;
     
  for(i=0;i<8;i++)
  {
    u8_readdata <<= 1;      //��ȡMISO 8�������ֵ������u8_readdata��
       
    if(u8_writedata & 0x80) //�ж����λ������д���λ��������λ��
      SDI_UP;              //MOSI���1����������׼������1
    else
      SDI_DOWN;             //MOSI���0����������׼������0
    u8_writedata <<= 1;     //���������Ѿ���������λ
       
    	SCK_UP;                //����������(SCK��0-->1)�����������ϵ�����д������
    if(GET_SDO)                //��ȡ���λ����������ĩβ��ͨ������λ��ɶ������ֽ�
      u8_readdata |= 0x01;
    else
      u8_readdata &= ~0x01;
        
    SCK_DOWN;                //�½�������(SCK��1-->0)��MISO�Ͻ������µ����ݣ���ȡ����u8����readdata
   }
  return u8_readdata;
}
unsigned char RE_RSSI(void)
{
	  uint8_t RSSI;
	RSSI = spi_rw(0x26, 0x00);
	return RSSI;


}


 signed char  save_rec_data(unsigned char *recbuf_pt)
{
			 uint8_t leg,i;

	

			 if(!GET_NIRQ)
				{
			  i = RE_RSSI();
             clr_interruput_si4432();
		        leg =spi_rw(0x4b,0);              
				SCK_DOWN;
				nSEL_DOWN; 
				spi_byte(0x7f);		
				for(i=0;i<leg;i++)	
				{
					*(recbuf_pt+i) = spi_byte(0x00);
				}
				nSEL_UP;
				spi_rw(0x07|0x80, SI4432_PWRSTATE_READY);	
				  
			   spi_rw(0x03,0x00);; //read the Interrupt Status1 register
               spi_rw(0x04,0x00);; //read the Interrupt Status2 register
			 rx_data();	
			
		      return leg; 
			  }
			
		  	  return 0;
}
 
void clr_interruput_si4432(void)
{

 spi_rw(0x03,0x00);	
 spi_rw(0x04,0x00);	

}

void initsi4432(void)
{
      DrvSYS_Delay(10000);
	  SDN_DOWN;
 	  DrvSYS_Delay(200000);
	  SI4432_init();  
  	  TX0_RX0;

	
  
}
 

void rx_data(void)
{	
	  uint8_t k;
	 	  k=k;
	spi_rw(0x07|0x80, SI4432_PWRSTATE_READY);
	k=spi_rw(0x07, 0);
	k=spi_rw(0x02, 0);
 	TX0_RX1;	

	spi_rw(0x08|0x80, 0x03);  
	spi_rw(0x08|0x80, 0x00);  
		
	spi_rw(0x07|0x80,SI4432_PWRSTATE_RX );
		k=spi_rw(0x07, 0);   
	spi_rw(0x05|0x80, SI4432_Rx_packet_received_interrupt);  	

		
}		


unsigned char tx_data(unsigned char *pt,unsigned char leg)
{

	unsigned char i,ItStatus,k;
	uint8_t Temp=0,cir=1;
	uint16_t ct=0;
	k=k;
	 
	spi_rw(0x05|0x80, SI4432_PACKET_SENT_INTERRUPT);
 		 
	spi_rw(0x07|0x80, SI4432_PWRSTATE_READY);	// rf ģ�����Ready ģʽ
	k=spi_rw(0x07, 0);
	k=spi_rw(0x02, 0); 	
	TX1_RX0;	
    spi_rw(0x08|0x80, 0x03);   
	spi_rw(0x08|0x80, 0x00); 

	spi_rw(0x34|0x80, 64); // 
	spi_rw(0x3e|0x80, leg); //
  	for (i = 0; i<leg; i++)
	{
		spi_rw(0x7f|0x80,*(pt+i));//
	}
  	 
     spi_rw(0x07|0x80, SI4432_PWRSTATE_TX);
	// spi_rw(0x07|0x80, 9);

	//	k=spi_rw(0x07, 0); 	
	 //	k=spi_rw(0x02, 0); 
	 // tx_timeout_en=0; 
	   
//	ItStatus =  spi_rw(0x03,0x00);  //���жϼĴ���

 while(cir==1)
  {
     Temp=spi_rw(0x03,0x00);
     Temp=Temp&0x04;
     if(Temp!=0){cir=0;}
     DrvSYS_Delay(1000);
     ct++;
     if(ct==650){cir=0;}
  }
  //MyGpioLow(GPIOA,GPIO_Pin_6);
  if(ct==350)
  {
  
 
 	  spi_rw(0x07|0x80,0x01);;
	// DrvSYS_Delay(5000);
	 SI4432_init();
	 rx_data();
    return 0; 	  //fail
  }
  else 
  {
    ItStatus = spi_rw(0x03,0x00);; //read the Interrupt Status1 register
    ItStatus = spi_rw(0x04,0x00);; //read the Interrupt Status2 register
     spi_rw(0x07|0x80,0x01);;
   //DrvSYS_Delay(5000);	   //success
   SI4432_init();
   	 rx_data();
    return 1;
  }


	 
}

void ref_init_si4432() //�ض���SI4432�����ʺ��������
{


	spi_rw(0x1c|0x80, 0x2d); 
	spi_rw(0x1d|0x80, 0x40);  //rx mode 
	spi_rw(0x1e|0x80, 0x0a); 

    spi_rw(0x20|0x80, 0x53);  
	spi_rw(0x21|0x80, 0x01); 
	spi_rw(0x22|0x80, 0x89);  			 
	spi_rw(0x23|0x80, 0x37);  
	spi_rw(0x24|0x80, 0x05);  
	spi_rw(0x25|0x80, 0x4c); //RX mode
	spi_rw(0x2A|0x80, 0x18);  			 
	spi_rw(0x2C|0x80, 0x0);  
	spi_rw(0x2D|0x80, 0x0);  
	spi_rw(0x2E|0x80, 0x0);


	spi_rw(0x30|0x80, 0x8c);  
	spi_rw(0x32|0x80, 0x8c); 
	spi_rw(0x33|0x80, 0x0a);  			 
	spi_rw(0x34|0x80, 0x08);  
	spi_rw(0x35|0x80, 0x40);  
	spi_rw(0x36|0x80, 0x2d); 
	spi_rw(0x37|0x80, 0xd4);  			 
	spi_rw(0x38|0x80, 0x0);  
	spi_rw(0x39|0x80, 0x0);  
	spi_rw(0x3a|0x80, 0x0);
	spi_rw(0x3b|0x80, 0x0); 
	spi_rw(0x3c|0x80, 0x0);  			 
	spi_rw(0x3d|0x80, 0x0);  
	spi_rw(0x3e|0x80, 0x0);  
	spi_rw(0x3f|0x80, 0x0);
	spi_rw(0x40|0x80, 0x0); 
	spi_rw(0x41|0x80, 0x0);  			 
	spi_rw(0x42|0x80, 0x0);  
	spi_rw(0x43|0x80, 0xff);  
	spi_rw(0x44|0x80, 0xff);
	spi_rw(0x45|0x80, 0xff); 
	spi_rw(0x46|0x80, 0xff);  			 
	spi_rw(0x56|0x80, 0x01);  

	
	spi_rw(0x6e|0x80, 0x62);	//tx rate
	spi_rw(0x6f|0x80, 0x4e);


	 spi_rw(0x6d|0x80, 0x00);  // set power

	 
	spi_rw(0x70|0x80, 0x21);
	spi_rw(0x71|0x80, 0x23); 
	spi_rw(0x72|0x80, 0x06);  			 
	 
	spi_rw(0x75|0x80, 0x53); 
	spi_rw(0x76|0x80, 0x4b);  //zhongpin 
	spi_rw(0x77|0x80, 0x00); 
 			 
	
	 

}

//REG����		
void SI4432_init(void)
{
	
	clr_interruput_si4432();

	spi_rw(0x06|0x80, 0x00);  //  �رղ���Ҫ���ж�
	
	spi_rw(0x07|0x80, 1);   // ���� Ready ģʽxton=1
	 


	spi_rw(0x09|0x80, 0x64);  //  ���ص���


	spi_rw(0x0a|0x80, 0x05);	// �رյ�Ƶ���
	spi_rw(0x0b|0x80, 0xea);  // GPIO 0 ������ͨ�����
	spi_rw(0x0c|0x80, 0xea);  //GPIO 1 ������ͨ�����
	spi_rw(0x0d|0x80, 0xf4);  // /GPIO 2 ����յ�������

				 
	spi_rw(0x70|0x80, 0x2c);  	// ��������ø���Silabs ��Excel
	spi_rw(0x1d|0x80, 0x40);  // ʹ�� afc
	// ref_init_si4432();
	//return;	
	
	// 1.2K bps setting start
	spi_rw(0x1c|0x80, 0x1b);	
	spi_rw(0x20|0x80, 0xd0);   
	spi_rw(0x21|0x80, 0x00); //
	spi_rw(0x22|0x80, 0x9d);// 
	spi_rw(0x23|0x80, 0x49); //
	spi_rw(0x24|0x80, 0x00); //
	spi_rw(0x25|0x80, 0x34); //
	spi_rw(0x2a|0x80, 0x50);


	spi_rw(0x6e|0x80, 0x4e);
	spi_rw(0x6f|0x80, 0xa5);
	
		
	//1.2K bps setting end
	
			
		
	spi_rw(0x30|0x80, 0x8c);   // ʹ��PH+ FIFOģʽ����λ��ǰ�棬ʹ��CRCУ��	
			
	spi_rw(0x32|0x80, 0xff);  // byte0, 1,2,3 ��Ϊͷ��
	spi_rw(0x33|0x80, 0x42);//  byte 0,1,2,3 ��ͷ�룬ͬ����3,2 ��ͬ����
	
	spi_rw(0x35|0x80, 0x20);  // ��Ҫ���4��nibble��Preamble  �ֲ��ͼ18   42ҳ

	spi_rw(0x36|0x80, 0x2d);  // ͬ����Ϊ 0x2dd40000
	spi_rw(0x37|0x80, 0xd4);
	spi_rw(0x38|0x80, 0x00);
	spi_rw(0x39|0x80, 0x00);


	spi_rw(0x3a|0x80, 's');  // �����ͷ��Ϊ�� ��sckj"
	spi_rw(0x3b|0x80, 'c');
	spi_rw(0x3c|0x80, 'k');
	spi_rw(0x3d|0x80, 'j');
	spi_rw(0x3e|0x80, 10);  // �ܹ�����10���ֽڵ�����
	spi_rw(0x3f|0x80, 's'); // ��ҪУ���ͷ��Ϊ����sckj"
	spi_rw(0x40|0x80, 'c');
	spi_rw(0x41|0x80, 'k');
	spi_rw(0x42|0x80, 'j');

	spi_rw(0x43|0x80, 0xff);  // ͷ��1,2,3,4 ������λ����ҪУ��
	spi_rw(0x44|0x80, 0xff);  // 
	spi_rw(0x45|0x80, 0xff);  // 
	spi_rw(0x46|0x80, 0xff);  //
	 
	spi_rw(0x6d|0x80, 0x07);  // set power

	spi_rw(0x79|0x80, 0x0);  // ����Ҫ��Ƶ
	spi_rw(0x7a|0x80, 0x0);  // ����Ҫ��Ƶ
	
	
	spi_rw(0x71|0x80, 0x22); // ���䲻��Ҫ CLK��FiFo �� FSKģʽ
    spi_rw(0x72|0x80, 0x30);  // ƵƫΪ 30KHz
	

	spi_rw(0x73|0x80, 0x0);  // û��Ƶ��ƫ��
	spi_rw(0x74|0x80, 0x0);  // û��Ƶ��ƫ��
	
	
		
	spi_rw(0x75|0x80, 0x53);  // Ƶ������ 433.5   //53
	spi_rw(0x76|0x80, 0x57);  // 
	spi_rw(0x77|0x80, 0x80);

		  
}
static  unsigned  char fx=0;
	void su_fo()
	{
		

		if(fx==1)
				{
			  LED_UP;
			  fx=0;
			  }
			  else
			  if(fx==0)
			  {
			  fx=1;	
			  LED_DOWN;
			
			   }
   }

 unsigned char Check_si4432(unsigned char t)
 {
 	 unsigned char state;
	state = spi_rw(0,0);
    if(state!=0x08) 
	{
	
	 if(t){su_fo();}
	
	return 0;}

	 return 1;










  



 
 }



unsigned char spi_byte(unsigned char data)
{

	unsigned char i;
	
	for (i = 0; i < 8; i++)		// ����SCK �� SDI������һ���ֽڵ����ͬ�¶�ȡ1���ֽڵ�����
	{				// û�а���nSEL�Ŀ���
		if (data & 0x80)
			SDI_UP;
		else
			SDI_DOWN;
			
		data <<= 1;
		SCK_UP;
		
		if (GET_SDO)
			data |= 0x01;
		else
			data &= 0xfe;
			
		SCK_DOWN;
	}
	
	return (data);
}

//-------------------------------------------//
unsigned char spi_rw(unsigned char addr, unsigned char data)
{

	unsigned char i;
	
	SCK_DOWN;
	nSEL_DOWN;
	
	for (i = 0; i < 8; i++) 
	{
		if (addr & 0x80)
			SDI_UP;
		else
			SDI_DOWN;
		addr <<= 1;
		SCK_UP;
	
		SCK_DOWN;
	 
	}
	  // 	DrvSYS_Delay(2);

	for (i = 0; i < 8; i++) 
	{
		if (data & 0x80)
			SDI_UP;
		else
			SDI_DOWN;
		data <<= 1;
		SCK_UP;
		if (GET_SDO)
			data |= 0x01;
		else
			data &= 0xfe;
		 
		SCK_DOWN;
	}
	nSEL_UP;
	SCK_UP;
	return data;
}
