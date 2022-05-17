// ������� ��ƾ - ext_uart ����

#define	_UART_C_

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\uart.h"
#include ".\inc\flash.h"
#include ".\inc\init.h"
#include ".\inc\eep.h"

//#define PRT38400

void Uart_Init_All(void)
{
	Uart_Init(38400,UART0);		//Debug
    Uart_Init(38400,UART1);		//��������
#ifdef PRT38400
    Uart_Init(38400,UART2);		//������
#else
    Uart_Init( 9600,UART2);		//������
#endif
//    Uart_Init(38400,UART3);
	set_baud3();				//PC
}

/**************************************************
	UART0 : Output & RomWrite
	UART1 : Printer
	UART2 : Debug  & RomWrite
	UART3 : spare
**************************************************/

void Uart_Init(int baud,int whichUart)
{
	switch (whichUart){
		case UART0:
			PCONP 	|= 1 << 3;						// Power on UART0
			PINSEL0  = (PINSEL0 & 0xFFFFFFCF) | (0x01 << 4);	// ��P0.2 TX0
			PINSEL0  = (PINSEL0 & 0xFFFFFF3F) | (0x01 << 6);	// ��P0.3 RX0
			U0LCR 	 = 0x83;
			U0DLM    = ((Fpclk / 16) / baud) / 256; 
			U0DLL    = ((Fpclk / 16) / baud) % 256; 
			U0LCR    = 0x03;
		   	U0FCR  	 = 0x03;					    // FIFO Trig.1Char
			U0IER    = 0x00;		 				// no INT.
			U0IER    = 0x01;		 				// no INT.
            rxw0=rxr0=txw0=txr0=0x00;
        break;
		case UART1:
			PCONP 	|= 1 << 4;						// Power on UART1
			PINSEL7  = (PINSEL7 & 0xFFFFFFFC) | (0x03 << 0);	// ��P3.16 TXD1
			PINSEL7  = (PINSEL7 & 0xFFFFFFF3) | (0x03 << 2);	// ��P3.17 RXD1
			U1LCR 	 = 0x83;
			U1DLM    = ((Fpclk / 16) / baud) / 256; 
			U1DLL    = ((Fpclk / 16) / baud) % 256; 
			U1LCR    = 0x03;
		   	U1FCR  	 = 0x03;					    // FIFO Trig.1Char
			U1IER    = 0x01;		 				// no INT.
            rxw1=rxr1=txw1=txr1=0x00;
        break;
        case UART2:
			PCONP 	|= 1 << 24;						// Power on UART2
			PINSEL0  = (PINSEL0 & 0xFFCFFFFF) | (0x01 << 20);	// ��P0.10 TX2
			PINSEL0  = (PINSEL0 & 0xFF3FFFFF) | (0x01 << 22);	// ��P0.11 RX2
			U2LCR 	 = 0x83;
			U2DLM    = ((Fpclk / 16) / baud) / 256; 
			U2DLL    = ((Fpclk / 16) / baud) % 256; 
			U2LCR    = 0x03;
		   	U2FCR  	 = 0x03;					    // FIFO Trig.1Char
			U2IER    = 0x01;		 				// no INT.
            rxw2=rxr2=txw2=txr2=0x00;
        break;
		case UART3:
			PCONP 	|= 1 << 25;						// Power on UART3
			PINSEL0  = (PINSEL0 & 0xFFFFFFFC) | (0x02 << 0);	// ��P0.0 TX3
			PINSEL0  = (PINSEL0 & 0xFFFFFFF3) | (0x02 << 2);	// ��P0.1 RX3
			U3LCR 	 = 0x83;
			U3DLM    = ((Fpclk / 16) / baud) / 256; 
			U3DLL    = ((Fpclk / 16) / baud) % 256; 
			U3LCR    = 0x03;
		   	U3FCR  	 = 0x03;					    // FIFO Trig.1Char
			U3IER    = 0x01;		 				// no INT.
            rxw3=rxr3=txw3=txr3=0x00;
        break;
	}
}

void wait_tx0(void){	while(txr0!=txw0); }
void wait_tx1(void){	while(txr1!=txw1); }
void wait_tx2(void){	while(txr2!=txw2); }
void wait_tx3(void){	while(txr3!=txw3); }

void com_clear(void){
	wait_tx0();
	wait_tx1();
	wait_tx2();
	wait_tx3();
	rxw0=rxr0=txw0=txr0=0x00;
	rxw1=rxr1=txw1=txr1=0x00;
	rxw2=rxr2=txw2=txr2=0x00;
	rxw3=rxr3=txw3=txr3=0x00;
}
void Uart_SendChar(int data,int whichUart){
   	switch (whichUart){
   		case UART0:
			SCTX0_Buf[txw0++]=data;
//			if(txw0==SCTX0_BUF_SIZE)txw0=0;
			return;
   		case UART1:
			SCTX1_Buf[txw1++]=data;
//			if(txw1==SCTX1_BUF_SIZE)txw1=0;
			return;
   		case UART2:
			SCTX2_Buf[txw2++]=data;
//			if(txw2==SCTX2_BUF_SIZE)txw2=0;
			return;
   		case UART3:
			SCTX3_Buf[txw3++]=data;
//			if(txw3==SCTX3_BUF_SIZE)txw3=0;
			break;
	}
}

void Uart_SendByte(int data,int whichUart){
	if(data == '\n'){Uart_SendChar('\r',whichUart);}
	Uart_SendChar(data,whichUart);
}		

void Uart_SendString(char *str,int whichUart){
    while(*str)	Uart_SendByte(*str++,whichUart);
}

int Uart_getch(int whichUart){
int b1;
	switch(whichUart){
		case UART0:
			b1=SCRX0_Buf[rxr0++];
//			if(rxr0==SCRX0_BUF_SIZE)rxr0=0;
			return b1;
		case UART1:
			b1=SCRX1_Buf[rxr1++];
//			if(rxr1==SCRX1_BUF_SIZE)rxr1=0;
			return b1;
		case UART2:
			b1=SCRX2_Buf[rxr2++];
//			if(rxr2==SCRX2_BUF_SIZE)rxr2=0;
			return b1;
		case UART3:
			b1=SCRX3_Buf[rxr3++];
//			if(rxr3==SCRX3_BUF_SIZE)rxr3=0;
			return b1;
	}
	return(0);
}
//if you don't use vsprintf(), the code size is reduced very much.
void Uart_Printf(int whichUart,char *fmt,...)
{
    va_list ap;
    char string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	Uart_SendString(string,whichUart);
	va_end(ap);
}

char string[256];

void printf0(char *fmt,...)
{
	va_list ap;
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	Uart_SendString(string,0);
	va_end(ap);
}

void printf1(char *fmt,...)
{
	va_list ap;
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	Uart_SendString(string,1);
	va_end(ap);
}
void printf2(char *fmt,...)
{
	va_list ap;
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	Uart_SendString(string,2);
	va_end(ap);
}
void printf03(char *fmt,...)
{
	va_list ap;
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    if(flag_flash!=3){
		Uart_SendString(string,0);
	}else{
		Uart_SendString(string,3);
	}
	va_end(ap);
}


void printfx(char *fmt,...)
{
va_list ap;
u8 a, b;
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    if(flag_flash!=3){
		for(a=0;a<255;a++){
			if(!string[a]) break;
			while(!(U0LSR&0x20)); for(b=0;b<100;b++);
			delay_ms(2);
			if(string[a]=='\n'){
				U0THR='\r';
				while(!(U0LSR&0x20)); for(b=0;b<100;b++);
				U0THR=0x0a;
			}else{
				U0THR=string[a];
			}
		}
	}else{
		for(a=0;a<255;a++){
			if(!string[a]) break;
			while(!(U3LSR&0x20)); for(b=0;b<100;b++);
			delay_ms(2);
			if(string[a]=='\n'){
				U3THR='\r';
				while(!(U3LSR&0x20)); for(b=0;b<100;b++);
				U3THR=0x0a;
			}else{
				U3THR=string[a];
			}
		}
	}
	va_end(ap);
}


void savcmd0(BYTE cmd){ SCTX0_Buf[txw0++]=cmd; } //sendprog
void savcmd1(BYTE cmd){ SCTX1_Buf[txw1++]=cmd; } //send485 
void savcmd2(BYTE cmd){ SCTX2_Buf[txw2++]=cmd; } //sendkey 
void savcmd3(BYTE cmd){
	 SCTX3_Buf[txw3++]=cmd; 
//	 if(BAUD_FLAG == 1)delay_us(500);
//	 else if(BAUD_FLAG == 2)delay_us(1500);
} //send232 
BYTE getcmd0(void){ return(SCRX0_Buf[rxr0++]); } //readprog
BYTE getcmd1(void){ return(SCRX1_Buf[rxr1++]); } //read485 
BYTE getcmd2(void){ return(SCRX2_Buf[rxr2++]); } //readkey 
BYTE getcmd3(void){ return(SCRX3_Buf[rxr3++]); } //read232 

BYTE bin2hex2(BYTE b){
	b &= 0x0f;
	if(b>9) return((b-10)+'A');
	else return(b+'0');
}

u8 asc2hex(u8 asc){
	if(asc<='9') return(asc - '0');
	else return( (asc-'A') + 10 );
}


void uartTest(void){
unsigned char ch;
	Uart_SendByte(testChar++,UART0);
//	Uart_SendString("-uart0\n",UART0);
//	Uart_SendString("-uart1\n",UART1);
//	Uart_SendString("-uart2\n",UART2);
//	Uart_SendString("-uart3\n",UART3);

	if(rxw0 != rxr0){
		ch=Uart_getch(UART0);
		Uart_SendChar(ch,UART0);
	}
	if(rxw1 != rxr1){
		ch=Uart_getch(UART1);
		Uart_SendChar(ch,UART1);
	}
	if(rxw2 != rxr2){
		ch=Uart_getch(UART2);
		Uart_SendChar(ch,UART2);
	}
	if(rxw3 != rxr3){
		ch=Uart_getch(UART3);
		Uart_SendChar(ch,UART3);
	}
}

/*******************************************************************/
void set_baud3(void){
	if(BAUD_FLAG==0){
	    Uart_Init(38400,UART3);
	}else if(BAUD_FLAG==1){
	    Uart_Init(19200,UART3);
	}else{
	    Uart_Init(9600,UART3);
	}
}
/*******************************************************************/
/*******************************************************************/
/*****************          Bluetooth          *********************/
/*******************************************************************/
/*******************************************************************/
void send_blu(char *str){
	rxw1=rxr1=0;	
    while(*str){
		U1THR = *str++;
//    	delay_ms(2);
//    	delay_ms(1);
    	delay_us(300);
    }
//	U1THR = 0x0d;
   	delay_ms(10);	
}
/*******************************************************************/
short ans_blu(u8 dly){
u8 b1, b2, b3;	
	b2=0;
	for(b1=0;b1<dly;b1++){
		delay_ms(5);
		if(rxw1!=rxr1){
			b3 = getcmd1();
if((b3==0x0d)||(b3==0x0a)) printf0(" "); else printf0("%c", b3); 
			if(!b2){
				if(b3=='O') b2=1; 
			}else{
				if(b3=='K') b1=111;
				else b2=0;
			}
		}
	}
	if(b1>=111) return(OK);
	else return(NG);	
}
/*******************************************************************/
short blu_baud(void){
u8 b1;
//	Uart_Init(9600,UART1);		//��������
	Uart_Init(115200,UART1);		//��������
//	printf0("\n Set baudrate : 38400"); 
	send_blu("AT");	delay_ms(50);
//	send_blu("AT");	delay_ms(50);
//	send_blu("AT");	delay_ms(50);
	for(b1=0;b1<3;b1++){
//		send_blu("AT+UARTCONFIG,38400,N,1,1");
//		send_blu("AT+BAUD?");
//		delay_ms(50);
		send_blu("AT+BAUD4");
		if(ans_blu(100)==OK) break;
	}	
	Uart_Init(38400,UART1);		//��������
	if(b1<3){
//		printf0("  OK!"); 
//		send_blu("ATZ");
		return(OK);
	}else{
//		printf0("  NG!"); 
		return(NG);
	}
}

short blu_dual(void){
u8 b1;
//	Uart_Init(9600,UART1);		//��������
	send_blu("AT");	delay_ms(50);
//	send_blu("AT");	delay_ms(50);
//	send_blu("AT");	delay_ms(50);
	for(b1=0;b1<3;b1++){
//		send_blu("AT+UARTCONFIG,38400,N,1,1");
//		send_blu("AT+DUAL?");
//		delay_ms(50);
		send_blu("AT+DUAL0");
		if(ans_blu(100)==OK) break;
	}	
	if(b1<3){
//		printf0("  OK!"); 
//		send_blu("ATZ");
		return(OK);
	}else{
//		printf0("  NG!"); 
		return(NG);
	}
}

/*******************************************************************/
short set_blu_ble(void){
u8 b1, b2, b3=0;
char nbuf[16], cbuf[16];
char text_NAME[16];
	printf0("\n Get Name : "); 
	for(b1=0;b1<16;b1++)text_NAME[b1]=(char)SYS_ID[b1];
	for(b1=0;b1<3;b1++){
//	   	send_blu("AT+NAMB?");
//	   	delay_ms(10);
	   	for(b2=0;b2<100;b2++){
	   		while(rxw1!=rxr1){
				nbuf[b3++] = getcmd1();
				if(b3>=11) break;
			}
			delay_ms(2);
		}
		nbuf[b3++] = 0;
		printf0("%s", nbuf);
	   	for(b2=0;b2<5;b2++){
	   		if(nbuf[b2+7]!=text_NAME[b2]) break;
	   	}
	   	if(b2<5){
//			printf0("\n Change Namb:R-50S %d", b1+1); 
   			strcpy(cbuf, "AT+NAMB");
   			strcat(cbuf, text_NAME);
  			strcat(cbuf, "_BLE");
//   			send_blu("AT+NAMER-50S");
   			send_blu(cbuf);
			if(ans_blu(50)==OK){ printf0(" [Change Namb]"); break; }
			return(NG);
		}else break;
	}

	return(OK);
}
short set_blu(void){
u8 b1, b2, b3=0;
char nbuf[16], cbuf[16];
char text_NAME[16];
//	printf0("\n Get Name : "); 
	for(b1=0;b1<16;b1++)text_NAME[b1]=(char)SYS_ID[b1];
	for(b1=0;b1<3;b1++){
//	   	send_blu("AT+NAME?");
//	   	delay_ms(10);
	   	for(b2=0;b2<100;b2++){
	   		while(rxw1!=rxr1){
				nbuf[b3++] = getcmd1();
				if(b3>=11) break;
			}
			delay_ms(2);
		}
		nbuf[b3++] = 0;
		printf0("%s", nbuf);
	   	for(b2=0;b2<5;b2++){
	   		if(nbuf[b2+7]!=text_NAME[b2]) break;
	   	}
	   	if(b2<5){
//			printf0("\n Change Name:R-50S %d", b1+1); 
   			strcpy(cbuf, "AT+NAME");
   			strcat(cbuf, text_NAME);
   			strcat(cbuf, "_EDR");
   			
//   			send_blu("AT+NAMER-50S");
   			send_blu(cbuf);
			if(ans_blu(50)==OK){ printf0(" [Change Name]"); break; }
			return(NG);
		}else break;
	}

	return(OK);
}
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
#define TRY_BLU	250		//5
void init_blu(void){
u8 b1;	
char tempString[30];
	flag_blu=1;
//	for(b1=0;b1<TRY_BLU;b1++){
	//send_blu("AT+RENEW");//�ʱ�ȭ
	//send_blu("AT+RESET");//�����
//	delay_ms(1500);

/*	strcat(tempString, "_EDR");	2019-06-28 ���� 9:08:12
	strcpy(tempString, "AT+NAMB");
	strcat(tempString, SYS_ID);
	strcat(tempString, "_BLE");
	send_blu(tempString);//BT NAME_BLE 
	send_blu("AT+DUAL0");//Dual���
	send_blu("AT+HIGH0");//SPEED:Balance���
	send_blu("AT+ATOB");//A to B ���(EDR BLE ��ȣȣȯ)
	send_blu("AT+MARJ0x1234");//Major 0x1234
	send_blu("AT+MINO0xFA01");//Minor 0xFA01
	send_blu("AT+IBEA1");//iBeacon On*///2019-06-28 ���� 9:08:53
	
//	send_blu("AT+ROLB1");//MASTER���
	
	Uart_Init(115200,UART1);
	send_blu("AT+BAUD4");//��żӵ� 2:9600bps, 4:38400bps
	Uart_Init(38400,UART1);		//��żӵ� 38400 ����
//	send_blu("AT+BAUD4");//��żӵ� 2:9600bps, 4:38400bps
	strcpy(tempString, "AT+NAME"); 
	strcat(tempString, SYS_ID);
	send_blu(tempString);//BT NAME_EDR
	send_blu("AT+PINE1882");//EDR PIN��ȣ
	send_blu("AT+SCAN0");//�˻�O
	send_blu("AT+DUAL1");//Dual���
	send_blu("AT+AUTH1");//PIN ����X
	send_blu("AT+RESET");//�����
	delay_ms(1500);
	flag_blu=0;
}
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
