#include "stm32f4xx.h" 
#include<stdio.h>
#include<string.h>
#include <stdint.h>
#include <limits.h>


typedef struct message{
	int len;
	uint32_t msg;
	int valid;

}message;	


//FIFO implementation for reciever

typedef struct FIFO {
    int top;
    unsigned capacity;
    message* array;
} FIFO;


typedef struct TX_mailbox{
	message * tx_msg;
	int state;
}TX_mailbox;


typedef struct RX_mailbox{
		int state;
		FIFO * rx_box;
}RX_mailbox;
	
	typedef struct
{
		uint32_t ID;
		uint32_t DLC;
		uint32_t CAN_MCR;
		uint32_t CAN_MSR;
		uint32_t CAN_TSR;
		uint32_t CAN_RF0R;
		uint32_t CAN_RF1R;
		uint32_t CAN_IER;
		uint32_t CAN_ESR;
		uint32_t CAN_BTR;
		uint32_t CAN_TI0R;
		TX_mailbox outbox[3];
		RX_mailbox inbox[2];
}CAN_ref;

