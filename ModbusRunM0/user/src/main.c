#include <stdio.h>
#include "M051Series.h"
#include "realyctrl.h"
#include "Uart3th.h"
#include "HDT11.h"
#include "BLUX_V30B.h"
#include "FMC_DATA_FLASH.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#include "main.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000



void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable IRC22M clock */
    CLK->PWRCON |= CLK_PWRCON_IRC22M_EN_Msk;

    /* Waiting for IRC22M clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_IRC22M_STB_Msk);

    /* Switch HCLK clock source to HIRC */
    CLK->CLKSEL0 = CLK_CLKSEL0_HCLK_S_HIRC;

    /* Set PLL to Power-down mode and PLL_STB bit in CLKSTATUS register will be cleared by hardware.*/
    CLK->PLLCON |= CLK_PLLCON_PD_Msk;

    /* Enable external 12 MHz XTAL */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Enable PLL and Set PLL frequency */
    CLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_PLL_STB_Msk | CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    /* Enable peripheral clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_TMR1_EN_Msk|CLK_APBCLK_UART1_EN_Msk | CLK_APBCLK_TMR2_EN_Msk;
    /* Peripheral clock source */
    CLK->CLKSEL1 = CLK_CLKSEL1_UART_S_PLL | CLK_CLKSEL1_TMR1_S_HCLK| CLK_CLKSEL1_TMR2_S_S_HCLK;
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set P3 multi-function pins for UART0 RXD, TXD and T1 */
    SYS->P3_MFP = SYS_MFP_P30_RXD0 | SYS_MFP_P31_TXD0 /*| SYS_MFP_P35_T1*/;
   /* Set P1 multi-function pins for UART1 RXD, TXD and T1 */
    SYS->P1_MFP = SYS_MFP_P12_RXD1 | SYS_MFP_P13_TXD1 /*| SYS_MFP_P35_T1*/;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}
 void DrvSYS_Delay(uint32_t us);
/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/


 extern void GprsInit(void);
 extern void GprsSentEventPoll(void);
 extern void GprsReciveEventPoll(void);
uint8_t SlaveAddr;
extern int32_t I2C_main(void);
int main(void ){
    volatile uint32_t u32InitCount;
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
	#ifdef USE_LUX
	InitBLUX_V30();
	#endif
	/*Init hardwave realies*/
	HWRealyInit();
	/*init CO2 sensor*/
    
    Co2Init();

	#ifdef USE_GPRS
    GprsInit();
	#endif
	/*Get the Addr from the HW switch */
	#ifdef GET_SW_ADDR
	SlaveAddr = InitSlaveAddr();
	#endif
	#ifdef GET_HW_ADDR
    SlaveAddr = GetHwAddrIint();
	#endif

    /* Lock protected registers */
   SYS_LockReg();
    /*Init Modbus RTU main thread and config USART */
 eMBInit( MB_RTU, SlaveAddr,0,9600, MB_PAR_NONE );
 /* Enable the Modbus Protocol Stack. */
 eMBEnable();
 
  while(1)
  {
  		
  	  (void)eMBPoll();

	   #ifdef USE_RELAY 
	  (void)RealyPoll();
	   #endif

	   #ifdef USE_KEY
	  (void)KeyPool(); 
	   #endif

	   #ifdef USE_GPRS
	   GprsSentEventPoll();
       GprsReciveEventPoll();
	   #endif

	  
	  (void)Co2EventPoll();
	  (void)Co2SendCmd();
	   
	  #ifdef USE_DHT11
	  (void)DHT11EventPoll();
	  #endif

	  #ifdef USE_LUX
	   Blux_v3bPoll();
	  #endif
	  
	  #ifdef GET_SW_ADDR
	   SlaveAddrPoll();
	  #endif
	  					  
  }

   
}

void DrvSYS_Delay(uint32_t us)
{
    SysTick->LOAD = us * CyclesPerUs;
    SysTick->VAL  = (0x00);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    /* Waiting for down-count to zero */
    while((SysTick->CTRL & (1 << 16)) == 0);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/



