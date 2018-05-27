

#ifndef	REALYCTRL_H
#define REALYCTRL_H

#include "M051Series.h"
#include "port.h"
#include "user_mb_app.h"
/*The realy channel numbers that maxmum is 24 channel*/
typedef enum Channel
{


	   CH1=0,
	   CH2,
	   CH3,
	   CH4,
	   CH5,
	   CH6,
	   CH7,
	   CH8,
	   CH9,
	   CH10,
	   CH11,
	   CH12,
	   CH13,
	   CH14,
	   CH15,
	   CH16,
	   CH17,
	   CH18,
	   CH19,
	   CH20,
	   CH21,
	   CH22,
	   CH23,
	   CH24
}Channel_Type;

typedef enum Status_Realy
{

	  OPEN=1,
	  CLOSE
	  
}Status_Type;










/*
继电器的路数
支持 4路、8路、12路、16路、24路
目前只做4路
*/
#define REALY_NUM    				8

/*8路继电器，以后就按照这个做，4-8路通用2018.3.20*/
#define CO2_1_8MODBUS-RTU  


void HWRealyInit(void);
void RealyCtrl(Channel_Type Channel,Status_Type status);
void RealyPoll(void);
void KeyPool(void);
uint8_t GetHwAddrIint(void);





/* Dever */
#ifdef  CO2_1_8MODBUS-RTU
#define  CH1_OPEN  			      \
do{P10=1;}while(0) 
#define  CH1_CLOSE 				  \
do{P10=0;}while(0)
#define  CH2_OPEN  				  \
do{P11=1;}while(0)
#define  CH2_CLOSE 				  \
do{P11=0;}while(0)
#define  CH3_OPEN  				  \
do{P14=1;}while(0)
#define  CH3_CLOSE 				  \
do{P14=0;}while(0)
#define  CH4_OPEN  				  \
do{P42=1;}while(0)
#define  CH4_CLOSE 				  \
do{P42=0;}while(0)
#define  CH5_OPEN  				  \
do{P03=1;}while(0)
#define  CH5_CLOSE 				  \
do{P03=0;}while(0)
#define  CH6_OPEN  				  \
do{P02=1;}while(0)
#define  CH6_CLOSE 				  \
do{P02=0;}while(0)
#define  CH7_OPEN  				  \
do{P01=1;}while(0)
#define  CH7_CLOSE 				  \
do{P01=0;}while(0)
#define  CH8_OPEN  				  \
do{P00=1;}while(0)
#define  CH8_CLOSE 				  \
do{P00=0;}while(0)

#endif







#endif
