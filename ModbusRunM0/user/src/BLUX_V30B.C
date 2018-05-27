#include <stdio.h>
#include "M051Series.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "main.h"
#include "user_mb_app.h"
#include "BLUX_V30B.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint8_t g_u8DeviceAddr;
volatile uint8_t g_au8TxData[3];
volatile uint8_t g_u8RxData;
volatile uint8_t g_u8DataLen;
volatile uint8_t g_u8EndFlag = 0;

typedef void (*I2C_FUNC)(uint32_t u32Status);

static I2C_FUNC s_I2C1HandlerFn = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;

    u32Status = I2C_GET_STATUS(I2C1);
    if(I2C_GET_TIMEOUT_FLAG(I2C1))
    {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    }
    else
    {
        if(s_I2C1HandlerFn != NULL)
            s_I2C1HandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    if(u32Status == 0x08)                       /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C1, (g_u8DeviceAddr << 1));    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_STOP(I2C1);
        I2C_START(I2C1);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
        if(g_u8DataLen != 2)
        {
            I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STA_SI);
        }
    }
    else if(u32Status == 0x10)                  /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C_SET_DATA(I2C1, ((g_u8DeviceAddr << 1) | 0x01));   /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x40)                  /* SLA+R has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x58)                  /* DATA has been received and NACK has been returned */
    {
        g_u8RxData = (unsigned char) I2C_GET_DATA(I2C1);
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STO_SI);
        g_u8EndFlag = 1;
    }
    else
    {
        /* TO DO */
       // printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if(u32Status == 0x08)                       /* START has been transmitted */
    {
        I2C_SET_DATA(I2C1, g_u8DeviceAddr << 1);    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_STOP(I2C1);
        I2C_START(I2C1);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
        if(g_u8DataLen != 3)
        {
            I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STO_SI);
            g_u8EndFlag = 1;
        }
    }
    else
    {
        /* TO DO */
       // printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

void I2C0_Close(void)
{
    /* Disable I2C0 interrupt and clear corresponding NVIC bit */
    I2C_DisableInt(I2C0);
    NVIC_DisableIRQ(I2C0_IRQn);

    /* Disable I2C0 and close I2C0 clock */
    I2C_Close(I2C0);
    CLK_DisableModuleClock(I2C0_MODULE);

}
void InitBLUX_V30(void)
{

/* Enable  I2C1 module clock */
//CLK_EnableModuleClock(I2C1_MODULE);

/* Configure the SDA1 & SCL1 of I2C1 pins */
//SYS->P4_MFP &= ~(SYS_MFP_P44_Msk | SYS_MFP_P45_Msk);
//SYS->P4_MFP |= SYS_MFP_P44_SCL1  | SYS_MFP_P45_SDA1;

GPIO_SetMode(P2, BIT7,GPIO_PMD_OUTPUT);//EN
GPIO_SetMode(P4, BIT4|BIT5,GPIO_PMD_QUASI );//EN

P27=1;

   return ;
  /* Open I2C module and set bus clock */
    I2C_Open(I2C1, 100000);

  /* Set I2C 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, 0x15, 0);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C1, 1, 0x35, 0);   /* Slave Address : 0x35 */
    I2C_SetSlaveAddr(I2C1, 2, 0x55, 0);   /* Slave Address : 0x55 */
    I2C_SetSlaveAddr(I2C1, 3, 0x75, 0);   /* Slave Address : 0x75 */

    /* Enable I2C interrupt */
    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);




}
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS] ;
extern unsigned long  Systick;
uint32_t BLUX_V3BTime;
void Blux_v3bPoll(void)
{	
    uint32 m_Lux = 0x00;
  	if(Systick-BLUX_V3BTime>=(500))
	{
	     BLUX_V3BTime=Systick;

	   B_LUX_GetLux(&m_Lux);

	  usSRegHoldBuf[17]=m_Lux>>16 ; //高16bit
   	  usSRegHoldBuf[18]=m_Lux ;	  //低16bit

	   //temp=((float)m_Lux/1000);

	  
	}
   




   return ;
   // g_u8DeviceAddr = 0x50;

   //  g_au8TxData[0] = ;
   //  g_au8TxData[1] = 0;
    // g_au8TxData[2] = 0;

     //   g_u8DataLen = 0;
     //   g_u8EndFlag = 0;

        /* I2C function to write data to slave */
     //   s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterTx;

        /* I2C as master sends START signal */
     //   I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STA);

        /* Wait I2C Tx Finish */
      //  while(g_u8EndFlag == 0);
         g_u8EndFlag = 0;

        /* I2C function to read data from slave */
        s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterRx;

        g_u8DataLen = 0;
        g_u8DeviceAddr = 0x95; //读取光照地址

		 g_au8TxData[0] =0 ;//从地址0开始读取
       //g_au8TxData[1] = 0;
      //g_au8TxData[2] = 0;

        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STA);

        /* Wait I2C Rx Finish */
        while(g_u8EndFlag == 0);

 
        g_u8RxData = g_au8TxData[2];
   

    /* Close I2C0 */
    I2C0_Close();

    while(1);
}















extern void DrvSYS_Delay(uint32_t us);

uint8    BUF_0[129];                       //接收数据缓存区      	
uint32   dis_data_0;                     //变量

/*---------------------------------------------------------------------
 功能描述: 延时纳秒 不同的工作环境,需要调整此函数
 参数说明: 无	
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_delay_nms(uint16 k)	
{						
  uint16 i,j;				
  for(i=0;i<k;i++)
  {			
   	DrvSYS_Delay(1000);
  }						
}					

/*---------------------------------------------------------------------
 功能描述: 延时5微秒  不同的工作环境,需要调整此函数
 参数说明: 无	
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_Delay5us()
{
  DrvSYS_Delay(5);
}

/*---------------------------------------------------------------------
 功能描述: 延时5毫秒  不同的工作环境,需要调整此函数
 参数说明: 无	
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_Delay5ms()
{
  uint16 n = 20000;
  
  while (n--);
}

/*---------------------------------------------------------------------
 功能描述: 起始信号
 参数说明: 无	
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_Start()
{
  B_LUX_SDA0_H;                         //拉高数据线
  B_LUX_SCL0_H;                         //拉高时钟线
  B_LUX_Delay5us();                     //延时
  B_LUX_SDA0_L;                         //产生下降沿
  B_LUX_Delay5us();                     //延时
  B_LUX_SCL0_L;                         //拉低时钟线
}

/*---------------------------------------------------------------------
 功能描述: 停止信号
 参数说明: 无	
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_Stop()
{
  B_LUX_SDA0_L;                         //拉低数据线
  B_LUX_SCL0_H;                         //拉高时钟线
  B_LUX_Delay5us();                     //延时
  B_LUX_SDA0_H;                         //产生上升沿
  B_LUX_Delay5us();                     //延时
  B_LUX_SCL0_L;
  B_LUX_Delay5us();
  
  //B_LUX_delay_nms(1000);                         //延时180ms
}

/*---------------------------------------------------------------------
 功能描述: 发送应答信号
 参数说明: ack - 应答信号(0:ACK 1:NAK)
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_SendACK(uint8 ack)
{
  if (ack&0x01)	B_LUX_SDA0_H;		//写应答信号
  else	B_LUX_SDA0_L;
  
  B_LUX_SCL0_H;                         //拉高时钟线
  B_LUX_Delay5us();                     //延时
  B_LUX_SCL0_L;                         //拉低时钟线
  B_LUX_SDA0_H;
  B_LUX_Delay5us();                     //延时
}

/*---------------------------------------------------------------------
 功能描述: 接收应答信号
 参数说明: 无
 函数返回: 返回应答信号
 ---------------------------------------------------------------------*/
uint8 B_LUX_RecvACK()
{
  uint8 CY = 0x00;
  B_LUX_SDA0_H;
  
  B_LUX_SDA0_I;
  
  B_LUX_SCL0_H;                         //拉高时钟线
  B_LUX_Delay5us();                     //延时
  
  
  CY |= B_LUX_SDA0_DAT;                 //读应答信号
  
  B_LUX_Delay5us();                     //延时
  
  B_LUX_SCL0_L;                         //拉低时钟线
  
  B_LUX_SDA0_O;
  
  return CY;
}

/*---------------------------------------------------------------------
 功能描述: 向IIC总线发送一个字节数据
 参数说明: dat - 写字节
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_SendByte(uint8 dat)
{
  uint8 i;
  
  for (i=0; i<8; i++)         			//8位计数器
  {
    if (dat&0x80)	B_LUX_SDA0_H;
    else	B_LUX_SDA0_L;                   //送数据口
    
    B_LUX_Delay5us();             		//延时
    B_LUX_SCL0_H;                		//拉高时钟线
    B_LUX_Delay5us();             		//延时
    B_LUX_SCL0_L;                		//拉低时钟线
    B_LUX_Delay5us();             		//延时
    dat <<= 1;              			//移出数据的最高位
  }
  
  B_LUX_RecvACK();
}

/*---------------------------------------------------------------------
 功能描述: 从IIC总线接收一个字节数据
 参数说明: 无
 函数返回: 接收字节
 ---------------------------------------------------------------------*/
uint8 B_LUX_RecvByte()
{
  uint8 i;
  uint8 dat = 0;
  B_LUX_SDA0_I;
  
  B_LUX_SDA0_H;                         //使能内部上拉,准备读取数据,
  for (i=0; i<8; i++)         	        //8位计数器
  {
        dat <<= 1;
    B_LUX_SCL0_H;                       //拉高时钟线
    B_LUX_Delay5us();             	//延时
    dat |= B_LUX_SDA0_DAT;              //读数据               
    B_LUX_SCL0_L;                       //拉低时钟线
    B_LUX_Delay5us();             	//延时
    
	
  }
  B_LUX_SDA0_O;
  
  return dat;
}

/*---------------------------------------------------------------------
 功能描述: 写BH1750
 参数说明: REG_Address - 寄存器地址
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_Single_Write(uint8 REG_Address)
{
  /*
  B_LUX_Start();                                //起始信号
  B_LUX_SendByte(B_LUX_SlaveAddress);           //发送设备地址+写信号
  B_LUX_SendByte(REG_Address);                  //内部寄存器地址，请参考中文pdf22页 
  //  BH1750_SendByte(REG_data);                //内部寄存器数据，请参考中文pdf22页 
  B_LUX_Stop();                                 //发送停止信号
  */
  
}

/*---------------------------------------------------------------------
 功能描述: 连续读出BH1750内部数据
 参数说明: 无
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_Multiple_read(void)
{  
  /*
  uint8 i;	
  B_LUX_Start();                                //起始信号
  B_LUX_SendByte(B_LUX_SlaveAddress+1);         //发送设备地址+读信号
  
  for (i=0; i<3; i++)                           //连续读取6个地址数据，存储中BUF
  {
    BUF_0[i] = B_LUX_RecvByte();                //BUF[0]存储0x32地址中的数据
    if (i == 0x02)
    {
      
      B_LUX_SendACK(1);                         //最后一个数据需要回NOACK
    }
    else
    {		
      B_LUX_SendACK(0);                         //回应ACK
    }
  }
  
  B_LUX_Stop();                                 //停止信号
  B_LUX_Delay5ms();
  */
}

/*---------------------------------------------------------------------
 功能描述: 初始化光照传感器
 参数说明: 无
 函数返回: 无
 ---------------------------------------------------------------------*/
void B_LUX_Init()
{
	
  B_LUX_SCL0_O;
  B_LUX_SDA0_O;
  
  //B_LUX_delay_nms(100);	                        //延时100ms
  
  //B_LUX_Single_Write(0x01); 
  
}

/*---------------------------------------------------------------------
 功能描述: 光照读取函数
 参数说明: 无
 函数返回: 返回光照值
 ---------------------------------------------------------------------*/
uint32 val32 = 0;
void B_LUX_GetLux(uint32 *vLux)
{  
  
  fp32 temp;
  uint8 i = 0;
  

  //---------------------------读程序
  B_LUX_Start();                                //起始信号
  B_LUX_SendByte(B_LUX_SlaveAddress+0);         //发送设备地址+读信号
  B_LUX_SendByte(0x00);
 
  B_LUX_Start();                                //起始信号
  B_LUX_SendByte(B_LUX_SlaveAddress+1);         //发送设备地址+读信号
  B_LUX_delay_nms(1);                         //延时180ms
  for (i=0; i<=128; i++)                           //连续读取6个地址数据，存储中BUF
  {
    BUF_0[i] = B_LUX_RecvByte();                //BUF[0]存储0x32地址中的数据
    if (i == 128)
    {
      
      B_LUX_SendACK(1);                         //最后一个数据需要回NOACK
    }
    else
    {		
      B_LUX_SendACK(0);                         //回应ACK
    }
  }
  B_LUX_Stop();                                 //停止信号
  
  //---------------------------
  
  
  /*
   //---------------------------写程序
  B_LUX_Start();                                //起始信号
  B_LUX_SendByte(B_LUX_SlaveAddress+0);         //发送设备地址+读信号
  B_LUX_SendByte(0x10);
  for (i=0; i<=112; i++)                           //连续读取6个地址数据，存储中BUF
  {
    B_LUX_SendByte(0x30+i); 
  }
  //B_LUX_SendByte(0x00); 
  B_LUX_Stop();                                 //停止信号
  //---------------------------
*/
  
  
  val32   = BUF_0[3];
  val32 <<= 8;
  val32  |= BUF_0[2];
  val32 <<= 8;
  val32  |= BUF_0[1];
  val32 <<= 8;
  val32  |= BUF_0[0];
  
  temp = (fp32)val32*1.4;			//半球透明度矫正值*1.4
  *vLux = (uint32)(temp);
} 














