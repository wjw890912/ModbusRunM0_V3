#ifndef _SI4432_H
#define _SI4432_H




#define SI4432_PWRSTATE_READY		1		              // ģ�� Ready ״̬����
#define SI4432_PWRSTATE_TX		0x09		              // ģ�� ����״̬����
#define SI4432_PWRSTATE_RX		5		                 // ģ�� ����״̬����
#define SI4432_PACKET_SENT_INTERRUPT	4		        // ģ�� ��������ж�
#define SI4432_Rx_packet_received_interrupt   0x02      // ģ�� �յ����ݰ��ж�

#define  TX1_RX0	spi_rw(0x0e|0x80, 0x01)		// ����״̬�����߿��ض���
#define  TX0_RX1	spi_rw(0x0e|0x80, 0x02)		// ����״̬�����߿��ض���
#define  TX0_RX0	spi_rw(0x0e|0x80, 0x00)         // �Ƿ��䣬����״̬�����߿��ض���



//tx��
#define  TX_SUCCESS  1
#define  TX_TIMEOUT  2
#define  TX_RSSI_CHANNLE_OCCUPANCY 3
#define  TX_FAIL  0

//RX��
#define RX_NO_RECIVER  0



void initsi4432(void) ;
unsigned char spi_rw(unsigned char addr, unsigned char data);
unsigned char spi_byte(unsigned char data);
void SI4432_init(void);
void rx_data(void);
void clr_interruput_si4432(void);
unsigned char tx_data(unsigned char *pt,unsigned char leg);
 signed char save_rec_data(unsigned char *recbuf_pt);
unsigned char RE_RSSI(void);
 unsigned char Check_si4432(unsigned char t);
 #define SI4432BUFNUM 8
  #define SI4432MSGLEN	8

typedef struct
{
	unsigned char  b_Used;
	unsigned char msglen;
	unsigned char  sendbuf[SI4432MSGLEN];
}SI4432_buf;
#endif