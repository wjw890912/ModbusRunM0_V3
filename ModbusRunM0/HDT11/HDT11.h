#ifndef _HDT11_H
#define _HDT11_H

#define get_bit(a,b)   a&(1<<b)
#define set_bit(a,b)   a|=(1<<b)
#define clr_bit(a,b)   a&=~(1<<b)
uint8_t Read_8byte_data(void);
void DHT11EventPoll(void);



#endif
