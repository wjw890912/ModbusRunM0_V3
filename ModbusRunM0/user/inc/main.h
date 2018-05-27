#ifndef __main_h
#define __main_h



/*使用温湿度传感器则定义，反之*/
#define USE_DHT11 

/*
使用GPRS模块则定义，反之
使用CO2模块MH-14A模块则定义，反之
GPRS 和CO2 同时使用串口1，所以他们目前是互斥的
GPRS 定义了则CO2不进行定义，否则会定义CO2,默认使用CO2
*/




//#define USE_GPRS 




#ifndef USE_GPRS

//使用PM25模块则开启，他和CO2只能开一个
#define USE_PM25
 //使用CO2模块则开启 他和PM25只能开一个
//#define USE_CO2
 
#endif


/*使用KEY输入则定义，反之*/
#define USE_KEY 
/*使用RELAY输入则定义，反之*/
#define USE_RELAY 


/*
定义这个表示使用软件配置地址，这个板子是CO2+DHT11+光照Modbus-RTU
#define USE_CO2DHT11_SW_ADDR_BOARD 
定义这个表示使用硬件薄码配置地址，这个板子是CO2+1~8路Modbus-RTU
#define USE_CO2DHT11RELAY8_HW_ADDR_BOARD
*/

//#define USE_CO2DHT11_SW_ADDR_BOARD

#define USE_CO2DHT11RELAY8_HW_ADDR_BOARD


/*使用光照传感器 */
#ifdef USE_CO2DHT11_SW_ADDR_BOARD
#define USE_LUX
#endif


#ifdef   USE_LUX
#define  GET_SW_ADDR   //软件从机地址
#else
#define  GET_HW_ADDR   //硬件薄码开关
#endif



#endif