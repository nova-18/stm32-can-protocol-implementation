#include "stm32f4xx.h"
#include "CAN_setup.h"
#define ARM_MATH_CM4


FIFO* createStack(unsigned capacity){
    FIFO* stack = (FIFO*)malloc(sizeof(FIFO));
    //stack->capacity = capacity;
    stack->top = -1;
    stack->array = (message*)malloc(stack->capacity * sizeof(message));
    return stack;
}

// Stack is full when top is equal to the last index
int isFull(FIFO* stack){
    return stack->top == stack->capacity - 1;
}

// Stack is empty when top is equal to -1
int isEmpty(FIFO* stack){
    return stack->top == -1;
}

// Function to add an item to stack. It increases top by 1
void push(FIFO* stack, message item){
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
    //printf("%d pushed to stack\n", item);
}

// Function to remove an item from stack. It decreases top by 1
message pop(FIFO* stack){
    if (isEmpty(stack)){
        message* overflow = (message*)malloc(sizeof(message));
        return *overflow;
		}			// You might want to handle this differently
    return stack->array[stack->top--];
}

// Function to return the top from stack without removing it
message peek(FIFO* stack){
    if (isEmpty(stack)){
				message* overflow = (message*)malloc(sizeof(message));
        return *overflow;
		}			// You might want to handle this differently
    return stack->array[stack->top];
}


// Function to allocate memory and setup transmit mailboxes
void transmit_mailbox_setup(CAN_ref * CAN){
	for (int i = 0;i<3;i++){
		CAN->outbox[i] = *(TX_mailbox*)malloc(sizeof(TX_mailbox));
		CAN->outbox[i].state = 0;
	}
}

//Function to allocate memory and setup recieving Mailboxes and FIFO's
void recieve_mailbox_setup(CAN_ref * CAN){
	for (int i = 0;i<3;i++){
		CAN->inbox[i] = *(RX_mailbox*)malloc(sizeof(RX_mailbox));
		CAN->inbox[i].state = 0;
	}
	
}


void Transmit_protocol(message * m){
 // Transmit protocol specific to Transiver
	
}
message RX(CAN_ref * CAN){
	// Reciever protocol specific to Transiver
}


//CAN initialization
void CAN_init(CAN_ref * CAN){
		CAN->ID = 0x3; //specified as per policy
		//CAN->DLC =
		CAN->CAN_MCR = 0X0;
		CAN->CAN_MSR = (1<<11) | (1<<10) | (1<<1);
		CAN->CAN_TSR = (7<<26);
		CAN->CAN_RF0R = 0x0;
		CAN->CAN_RF1R = 0x0;
		CAN->CAN_IER = 0x0;
		CAN->CAN_ESR = 0x0;
		CAN->CAN_BTR = (1<<21) | (3<<16);
		CAN->CAN_TI0R = 0x0;
		transmit_mailbox_setup(CAN);
		recieve_mailbox_setup(CAN);
}

// Function for restting CAN 
void CAN_reset(CAN_ref * CAN){
		if(CAN->CAN_MCR && 1<<15){
			CAN->CAN_MCR = 0X0;
			CAN->CAN_MSR = (1<<11) | (1<<10) | (1<<1);
			CAN->CAN_TSR = (7<<26);
			CAN->CAN_RF0R = 0x0;
			CAN->CAN_RF1R = 0x0;
			CAN->CAN_IER = 0x0;
			CAN->CAN_ESR = 0x0;
			CAN->CAN_BTR = (1<<21) | (3<<16);
			CAN->CAN_TI0R = 0x0;
		}
}

// Transmission mailbox protocol as per the state diagram proposed in bxCAN specifiction 
void transmit_push(CAN_ref * CAN,message *msg){

	if(CAN->CAN_TSR && !(1<<0)){
		CAN->outbox[0].tx_msg = msg;
		CAN->outbox[0].state = 1;
		CAN->outbox[0].state = 2;
		while(!(CAN->CAN_TSR && 1<<7) && !(CAN->CAN_TSR && 1<<1));
		Transmit_protocol(CAN->outbox[0].tx_msg);
		CAN->CAN_TSR |= (1<<0);
		CAN->outbox[0].state = 0;
	}
	else if(CAN->CAN_TSR && (1<<8)){
		CAN->outbox[1].tx_msg = msg;
		CAN->outbox[1].state = 1;
		while(!(CAN->CAN_TSR && (1<<0)));
		CAN->outbox[1].state = 2;
		while(!(CAN->CAN_TSR && 1<<15) && !(CAN->CAN_TSR && 1<<9));
		Transmit_protocol(CAN->outbox[1].tx_msg);
		CAN->CAN_TSR |= (1<<8);
		CAN->outbox[1].state = 0;
	}
	else if(CAN->CAN_TSR && (1<<16)){
		CAN->outbox[2].tx_msg = msg;
		CAN->outbox[2].state = 1;
		while(!(CAN->CAN_TSR && (1<<0)) && (!(CAN->CAN_TSR && (1<<8)));
		CAN->outbox[2].state = 2;
		while(!(CAN->CAN_TSR && 1<<23) && !(CAN->CAN_TSR && 1<<17));
		Transmit_protocol(CAN->outbox[2].tx_msg);
		CAN->CAN_TSR |= (1<<16);
		CAN->outbox[2].state = 0;
	}
}


// Reciever mailbox protocol as per the state diagram proposed in bxCAN specifiction for mailbox 0
void activate_recieve_message_box0(CAN_ref * CAN){
	while(1){
		message msg = RX(CAN);
		if(CAN->CAN_RF0R && (1<<4) && !isFull(CAN->inbox[0].rx_box)){
			if(CAN->inbox[0].rx_box->top == 1){
				CAN->CAN_RF0R |= 0x1;
				CAN->CAN_RF0R &= !(0x8);
			}
			else{
				CAN->CAN_RF0R &= !(0xB);
			}
		}
		if(msg.valid && !(CAN->CAN_RF0R && (1<<4))){
			push(CAN->inbox[0].rx_box,msg);
			CAN->inbox[0].state += 1;
			CAN->CAN_RF0R += 1;
		}
		if(CAN->CAN_RF0R && (1<<5)){
			pop(CAN->inbox[0].rx_box);
			CAN->inbox[0].state -= 1;
			CAN->CAN_RF0R += 1;
		}
	}
}

// Reciever mailbox protocol as per the state diagram proposed in bxCAN specifiction for mailbox 1
void activate_recieve_message_box1(CAN_ref * CAN){
	while(1){
		message msg = RX(CAN);
		if(CAN->CAN_RF0R && (1<<4) && !isFull(CAN->inbox[1].rx_box)){
			if(CAN->inbox[1].rx_box->top == 1){
				CAN->CAN_RF0R |= 0x1;
				CAN->CAN_RF0R &= !(0x8);
			}
			else{
				CAN->CAN_RF0R &= !(0xB);
			}
		}
		if(msg.valid && !(CAN->CAN_RF0R && (1<<4))){
			push(CAN->inbox[1].rx_box,msg);
			CAN->inbox[1].state += 1;
			CAN->CAN_RF0R += 1;
		}
		if(CAN->CAN_RF0R && (1<<5)){
			pop(CAN->inbox[1].rx_box);
			CAN->inbox[1].state -= 1;
			CAN->CAN_RF0R += 1;
		}
	}
}


int main(){
		CAN_ref * CAN = (CAN_ref*)malloc(sizeof(CAN_ref));
		CAN_init(CAN);
		transmit_mailbox_setup(CAN);
		recieve_mailbox_setup(CAN);
		while(1){
	
	
		}
}