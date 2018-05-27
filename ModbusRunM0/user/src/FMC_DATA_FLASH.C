#include <stdio.h>
#include "M051Series.h"
#include "FMC_DATA_FLASH.h"
#include "user_mb_app.h"

#define DATA_FLASH_TEST_BASE        (FMC->DFBADR)
#define DATA_FLASH_TEST_END         (FMC->DFBADR + 0x1000)
#define TEST_PATTERN                0x5A5A5A5A

extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS] ;



/*
int32_t FillDataPattern(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t u32Addr;

    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4)
    {
        FMC_Write(u32Addr, u32Pattern);
    }
    return 0;
}


int32_t  VerifyData(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t    u32Addr;
    uint32_t    u32Data;

    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4)
    {
        u32Data = FMC_Read(u32Addr);
        if(u32Data != u32Pattern)
        {
            printf("\nFMC_Read data verify failed at address 0x%x, read=0x%x, expect=0x%x\n", u32Addr, u32Data, u32Pattern);
            return -1;
        }
    }
    return 0;
}


int32_t  FlashTest(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t    u32Addr;

    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += FMC_FLASH_PAGE_SIZE)
    {
        printf("    Flash test address: 0x%x    \r", u32Addr);

        // Erase page
        FMC_Erase(u32Addr);

        // Verify if page contents are all 0xFFFFFFFF
        if(VerifyData(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, 0xFFFFFFFF) < 0)
        {
            printf("\nPage 0x%x erase verify failed!\n", u32Addr);
            return -1;
        }

        // Write test pattern to fill the whole page
        if(FillDataPattern(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, u32Pattern) < 0)
        {
            printf("Failed to write page 0x%x!\n", u32Addr);
            return -1;
        }

        // Verify if page contents are all equal to test pattern
        if(VerifyData(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, u32Pattern) < 0)
        {
            printf("\nData verify failed!\n ");
            return -1;
        }

        FMC_Erase(u32Addr);

        // Verify if page contents are all 0xFFFFFFFF
        if(VerifyData(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, 0xFFFFFFFF) < 0)
        {
            printf("\nPage 0x%x erase verify failed!\n", u32Addr);
            return -1;
        }
    }
    printf("\r    Flash Test Passed.          \n");
    return 0;
}  */
//原子操作
void FMC_WriteData(uint32_t u32Addr,uint32_t u32Data)
{

FMC_Write(u32Addr, u32Data);

}


uint32_t FMC_ReadData(uint32_t u32Addr)
{

 return FMC_Read(u32Addr);

}

void FMC_EraseData(uint32_t u32Addr)
{

	 FMC_Erase(u32Addr);
}


 static uint8_t newid=0,oldid=0;
uint8_t InitSlaveAddr()
{
   uint32_t u32d;
	    /* Enable FMC ISP function */
    FMC_Open();
 u32d = FMC_ReadData(DATA_FLASH_TEST_BASE);

	    /* Disable FMC ISP function */
    FMC_Close();
	 if(u32d==0xffffffff)
	 {
	 	 usSRegHoldBuf[19]=254 ;
		 newid=254;
		 oldid=254;	//同步
	 	 return 254;//默认空的地址为1
	 }
	 else
	 {
	 	  usSRegHoldBuf[19]=u32d ;
		  newid=u32d;
		  oldid=u32d;//同步
	 	 return  u32d;//返回存储的地址
	 }



}



void SlaveAddrPoll()
{
  

     newid=	usSRegHoldBuf[19]; //有效地址1-254 ，

			if (newid!=oldid)
			{
				if((usSRegHoldBuf[19]<255)&&(usSRegHoldBuf[19]>0)) //地址不可以超过254，否者设置无效
				{
			   SaveUserSlaveAddr(usSRegHoldBuf[19]&0x00ff);	//高八位舍弃
			    }
			
				   oldid=newid;
			}
}
void SaveUserSlaveAddr(uint8_t addr)
{
    uint32_t i, u32Data;
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();

  //原子操作
FMC_EraseData(DATA_FLASH_TEST_BASE);
FMC_WriteData(DATA_FLASH_TEST_BASE,(uint32_t) addr);






   // u32Data = FMC_ReadCID();
   // printf("  Company ID ............................ [0x%08x]\n", u32Data);

   // u32Data = FMC_ReadPID();
   // printf("  Product ID ............................ [0x%08x]\n", u32Data);

   /* for(i = 0; i < 3; i++)
    {
        u32Data = FMC_ReadUID(i);
       // printf("  Unique ID %d ........................... [0x%08x]\n", i, u32Data);
    }

    for(i = 0; i < 4; i++)
    {
        u32Data = FMC_ReadUCID(i);
       // printf("  Unique Customer ID %d .................. [0x%08x]\n", i, u32Data);
    }
	 */
    /* Read User Configuration */
   // printf("  User Config 0 ......................... [0x%08x]\n", FMC_Read(FMC_CONFIG_BASE));

    /* Read Data Flash base address */
   // u32Data = FMC_ReadDataFlashBaseAddr();
   // printf("  Data Flash Base Address ............... [0x%08x]\n", u32Data);

   // printf("\n\nLDROM test =>\n");
/*   FMC_EnableLDUpdate();
    if(FlashTest(FMC_LDROM_BASE, FMC_LDROM_BASE + FMC_LDROM_SIZE, TEST_PATTERN) < 0)
    {
        printf("\n\nLDROM test failed!\n");
        goto lexit;
    }
    FMC_DisableLDUpdate();

    printf("\n\nAPROM test =>\n");
    FMC_EnableAPUpdate();
    if(FlashTest(APROM_TEST_BASE, APROM_TEST_END, TEST_PATTERN) < 0)
    {
        printf("\n\nAPROM test failed!\n");
        goto lexit;
    }
    FMC_DisableAPUpdate();
*/



   // printf("\n\nData Flash test =>\n");
   // if(FlashTest(DATA_FLASH_TEST_BASE, DATA_FLASH_TEST_END, TEST_PATTERN) < 0)
    //{
       
  
    //}


    /* Disable FMC ISP function */
    FMC_Close();

   SYS_ResetCPU();


     /* Lock protected registers */
    SYS_LockReg();

   
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
