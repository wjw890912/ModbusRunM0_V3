






	  开发日志

 MODBUS RTU 主要针对了基于RS485的 M0上的产品需求而编写。
 此外已经顺利的在 ARM 公司的 M3内核CPU和M4内核CPU运行，
 其中M4内核CPU的MODBUS 是已经用在项目中去了。

1、移植MODBUS1.60到M051系列上。
1）、修改MODBUS协议栈的上层应用线圈和保持寄存器全部改为8个字节
/* -----------------------Slave Defines -------------------------------------*/
#define S_DISCRETE_INPUT_START        1
#define S_DISCRETE_INPUT_NDISCRETES   8
#define S_COIL_START                  1
#define S_COIL_NCOILS                 8
#define S_REG_INPUT_START             1
#define S_REG_INPUT_NREGS             8
#define S_REG_HOLDING_START           1
#define S_REG_HOLDING_NREGS           8
/* -----------------------Master Defines -------------------------------------*/
#define M_DISCRETE_INPUT_START        1
#define M_DISCRETE_INPUT_NDISCRETES   8
#define M_COIL_START                  1
#define M_COIL_NCOILS                 8
#define M_REG_INPUT_START             1
#define M_REG_INPUT_NREGS             8
#define M_REG_HOLDING_START           1
#define M_REG_HOLDING_NREGS           8
2）、修改定时器文件，开启20KHZ的定时器1.
3）、修改串口收发以及发送空中断和接收中断。
4）、预定波特率为9600  NONE校验 数据位8 停止1 
20160902

2、硬件上使用的是在比特电子设计的4路灯控继电器的PCB板。
1）RS485部分是自动的，那是那块电路是姚工原来的设计，有问题。
发送的时候接收可以收到几个字节。这样导致了MODBUS协议栈会上报一个错误的代码。
依次循环，会引发发送风暴。针对这个电路修改了代码的异常处理部分，是强制关闭错误MODBUS代码上报。
值得注意的是，在我自己的设计中应该去掉这个改动。因为我的那个电路是OK的。不修改也可以但是就没有了错误诊断的功能。
修改了
/* If the request was not sent to the broadcast address we
             * return a reply. */
            if( ucRcvAddress != MB_ADDRESS_BROADCAST )
            {
                if( eException != MB_EX_NONE )
                {
                    /* An exception occured. Build an error frame. */
                    usLength = 0;
                    ucMBFrame[usLength++] = ( UCHAR )( ucFunctionCode | MB_FUNC_ERROR );
                    ucMBFrame[usLength++] = eException;
					break;
					/*是为了适应姚工电路（他的发送接收可以收到，并且收不全），
					这个break是为了避免MODBUS接收到错误数据而上报错误状态码。
					我自己的自动收发电路无需修改。
                	*/
				}
                eStatus = peMBFrameSendCur( ucMBAddress, ucMBFrame, usLength );


上面的代码段中加入了一个break，是代码提前结束。:)


2）、使用MODBUS poll 进行读取HOLD寄存器值0-5 OK 测试OK 。
20160902



3、增加磁保持继电器的驱动和通过修改HOLD寄存器控制4路继电器的框架程序文件 realyctrl.c
使用的PCB是为比特开发的GLCS207-ER4GS T1 V1

 usSRegHoldBuf[0] 值是0表示第一路磁保持继电器关闭 1表示第1路继电器开
 usSRegHoldBuf[1] 值是0表示第一路磁保持继电器关闭 1表示第2路继电器打开
 usSRegHoldBuf[2] 值是0表示第一路磁保持继电器关闭 1表示第3路继电器打开
 usSRegHoldBuf[3] 值是0表示第一路磁保持继电器关闭 1表示第4路继电器打开

 20160907

 3、增加从机地址的配置，从硬件的薄码开关读取进来作为地址使用。
 增加2个宏分别支持2个不同的PCB板的驱动
 /*

#define GTLCS207_ER4GS_V1  使用PCB板为比特开发的 GTLCS207-ER4GS MB V1  的4路磁保持继电器驱动

*/
//#define GTLCS207_ER4GS_V1
#define GTRMS107_ER4AS_V10

 20160908

4、增加对按键参数的映射。分别放入寄存器如下。使得读取寄存器就可以获取外部开关量状态

		/*sample key data in idle*/
	usSRegHoldBuf[10] = P17;
	usSRegHoldBuf[11] = P16;
	usSRegHoldBuf[12] = P15;
	usSRegHoldBuf[13] = P14;
20160909

5、修改输出继电器的IO脚位，修改薄码开关的脚位、这个修改是为信为物联继电器板子做的。我们自己的板子，加油！
GTRMS107_ER4AS_V10，定义这个
201707


6、增加C02传感器接口，第三方串口接口，文件Uart3th.c和Uart3th.h

  CO2传感器使用MH-Z14A，兼容MH-Z14，对比发现偏低约200ppm,传感器差异咱不理会

  使用14号寄存器来存储二氧化碳浓度
  usSRegHoldBuf[14]= (UartReciveBuf[2]<<8)+UartReciveBuf[3];//get CO2	MSB and LSB

  (int16_t)usSRegHoldBuf[8];//CO2补偿,

20180318

7、增加DHT11驱动程序。

   使用15\16号寄存器来存储温湿度DHT11
		   usSRegHoldBuf[15]=(Dht11>>8)&0xff; //湿度
		   usSRegHoldBuf[16]=Dht11&0xff;	  //温度
20180320

8、修改8路继电器和薄码开关以及KEY4路按键的硬件驱动地址。适配CO2+1-8路MODBUS-RTU板子
	 usSRegHoldBuf[0] 值是0表示第1路继电器关 1表示第1路继电器开
	 usSRegHoldBuf[1] 值是0表示第2路继电器关 1表示第2路继电器开
	 usSRegHoldBuf[2] 值是0表示第3路继电器关 1表示第3路继电器开
	 usSRegHoldBuf[3] 值是0表示第4路继电器关 1表示第4路继电器开
	 usSRegHoldBuf[4] 值是0表示第5路继电器关 1表示第5路继电器开
	 usSRegHoldBuf[5] 值是0表示第6路继电器关 1表示第6路继电器开
	 usSRegHoldBuf[6] 值是0表示第7路继电器关 1表示第7路继电器开
	 usSRegHoldBuf[7] 值是0表示第8路继电器关 1表示第8路继电器开



1路打开
7165989-Tx:01 10 00 00 00 01 02 00 01 67 90 
7165990-Rx:01 10 00 00 00 01 01 C9 
2路打开
7165991-Tx:01 10 00 01 00 01 02 00 01 66 41 
7165992-Rx:01 10 00 01 00 01 50 09 
3路打开
7165993-Tx:01 10 00 02 00 01 02 00 01 66 72 
7165994-Rx:01 10 00 02 00 01 A0 09 
4路打开
7165995-Tx:01 10 00 03 00 01 02 00 01 67 A3 
7165996-Rx:01 10 00 03 00 01 F1 C9 
5路打开
7165997-Tx:01 10 00 04 00 01 02 00 01 66 14 
7165998-Rx:01 10 00 04 00 01 40 08 
6路打开
7165999-Tx:01 10 00 05 00 01 02 00 01 67 C5 
7166000-Rx:01 10 00 05 00 01 11 C8 
7路打开
7166001-Tx:01 10 00 06 00 01 02 00 01 67 F6 
7166002-Rx:01 10 00 06 00 01 E1 C8 
8路打开
7166003-Tx:01 10 00 07 00 01 02 00 01 66 27 
7166004-Rx:01 10 00 07 00 01 B0 08 

1路关闭
7166005-Tx:01 10 00 00 00 01 02 00 00 A6 50 
7166006-Rx:01 10 00 00 00 01 01 C9 
2路关闭
7166007-Tx:01 10 00 01 00 01 02 00 00 A7 81 
7166008-Rx:01 10 00 01 00 01 50 09 
3路关闭
7166009-Tx:01 10 00 02 00 01 02 00 00 A7 B2 
7166010-Rx:01 10 00 02 00 01 A0 09 
4路关闭
7166011-Tx:01 10 00 03 00 01 02 00 00 A6 63 
7166012-Rx:01 10 00 03 00 01 F1 C9 
5路关闭
7166013-Tx:01 10 00 04 00 01 02 00 00 A7 D4 
7166014-Rx:01 10 00 04 00 01 40 08 
6路关闭
7166015-Tx:01 10 00 05 00 01 02 00 00 A6 05 
7166016-Rx:01 10 00 05 00 01 11 C8 
7路关闭
7166017-Tx:01 10 00 06 00 01 02 00 00 A6 36 
7166018-Rx:01 10 00 06 00 01 E1 C8 
8路关闭
7166019-Tx:01 10 00 07 00 01 02 00 00 A7 E7 
7166020-Rx:01 10 00 07 00 01 B0 08 

全开：
7166021-Tx:01 10 00 00 00 08 10 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 6E 20
7166022-Rx:01 10 00 00 00 08 C1 CF
全关：
7166023-Tx:01 10 00 00 00 08 10 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 19 CD
7166024-Rx:01 10 00 00 00 08 C1 CF


20180320

9、增加GPRS模块，这两天带孩子，所以迟一点弄，目前已经调通配置AT指令和链接到服务器的DEMO，后面还会修改。
20180401
10、增加main.h文件，由于未来用这个程序的模块太多。功能有相关的定制，所以在里面定义一些宏，来操作这些功能选项
必要开启就行，维护一个程序。
20180402
11、修改KEY扫描部分，修改为Key来控制继电器1-4路，其他路数只能通过MODBUS控制
    逻辑是，如果继电器当前是开启，那么按键之后关闭，如果当前是关闭，那么按键之后是开启。
	MCU侧是低电平有效，坑了，getbutton 的时候搞成1了，总是按键被按下，改成！GETBUTTON
20180402
12、增加I2C接口光照传感器
光照值用
	  usSRegHoldBuf[17]=m_Lux>>16 ; //高16bit
   	  usSRegHoldBuf[18]=m_Lux ;	  //低16bit

	  上位机读取到之后需要/1000

来存储
光照的操作需要看main.h里面定义的宏开关，和继电器模块的HW地址设定。
20180403

13、增加FMC操作来存储软件设定从机地址。

如果是没有设置过的地址默认是254
如果需要设置地址直接对 usSRegHoldBuf[19]进行写数据即可，注意有效地址1-254 ，
设置完毕地址后立即自动重启，执行新地址,
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


20180403


14、使用哪一个板子编译宏开关，在main.h中  默认是定义硬件薄码开关的继电器板子

/*
定义这个表示使用软件配置地址，这个板子是CO2+DHT11+光照Modbus-RTU
#define USE_CO2DHT11_SW_ADDR_BOARD 
定义这个表示使用硬件薄码配置地址，这个板子是CO2+1~8路Modbus-RTU
#define USE_CO2DHT11RELAY8_HW_ADDR_BOARD
*/

//#define USE_CO2DHT11_SW_ADDR_BOARD

#define USE_CO2DHT11RELAY8_HW_ADDR_BOARD
20180405
15、调整了一下UART1和UART2的中断优先级，UART1的原来是3调整为2，UART0的原来是2调整为3
为了规避MOdbus高速读取数据时 和UART1的冲突而造成的Modbus丢帧
20180406

16、增加PM2.5  模组

 	usSRegHoldBuf[20]= (UartReciveBuf[4]<<8)+UartReciveBuf[5];//get PM25	MSB and LSB
	usSRegHoldBuf[21]= (UartReciveBuf[2]<<8)+UartReciveBuf[3];//get PM10	MSB and LSB
	用来存储 PM25和PM10值。
	usSRegHoldBuf[22]用来存储传感器是否开机命令，开机写1，关机写0即可。	默认上点是关机的。
20180518


