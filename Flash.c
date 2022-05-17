#define	_FLASH_C_

#include <string.h>
#include <stdio.h>
#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\init.h"
#include ".\inc\uart.h"
#include ".\inc\lcd.h"
#include ".\inc\flash.h"
#include ".\inc\com.h"
#include ".\inc\eep.h"
#include ".\inc\meas.h"
#include ".\inc\resm.h"
#include ".\inc\dsp.h"

U32	FlashWriteStartAdr;		//
#define ROMSELECT	10		//'0':1'st Position

void flash_decode(u8 dat);
void flash_end(char no);
u8 posi;

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int AM29F1600_CheckId(void)
{
	U16 manId,devId;
	_RESET();
	_WR(0x555,0xaa);
	_WR(0x2aa,0x55);
	_WR(0x555,0x90);
	manId=_RD(0x0);
	_WR(0x555,0xaa);
	_WR(0x2aa,0x55);
	_WR(0x555,0x90);
	devId=_RD(0x1);
	_RESET();   
//	printfx("\n Manufacture ID=%4x(0x0001), Device ID(0x2249)=%4x",manId,devId);
	if((manId==0x0001 || manId==0x00AD) && devId==0x2249)
		return 1;
	else 
//		return 0;
		return 1;
}

/**********************************************************************/
void AM29F1600_EraseSector(int targetAddr)
{
//	printf("\n Sector Erase is started!");
	_RESET();   
	_RESET();   
	_RESET();   
	_RESET();   
	_RESET();
	_WR(0x555,0xaaaa);
	_WR(0x2aa,0x5555);
	_WR(0x555,0x8080);
	_WR(0x555,0xaaaa);
	_WR(0x2aa,0x5555);
	_WR(BADDR2WADDR(targetAddr),0x3030);
	_WAIT();
	_RESET();
}

/**********************************************************************/
int BlankCheck(int targetAddr,int targetSize)
{
	int i,j;
	for(i=0; i<targetSize; i+=2)
	{
		j=*((U16 *)(i+targetAddr+BASE2470));
		if( j!=0xffff)
		{
			printfx("\n Blank Check Error:%x=%x",(i+targetAddr+0x0000000),j);
			return 0;
		}
//		if(!(i&0x01ff)) CLREXWDT();
	}
	return 1;
}

/**********************************************************************/
int _WAIT(void) //Check if the bit6 toggle ends.
{
	u16 i;
	volatile U16 flashStatus,old;
	for(i=0;i<10;i++);
	old=*((volatile U16 *)BASE2470);

	while(1){
		flashStatus=*((volatile U16 *)BASE2470);
		if( (old&0x40) == (flashStatus&0x40) ) break;
		
		if( flashStatus&0x20 )	//Time_limit_over check(in case normally end with set DQ5)
		{
			//printfx("\n [DQ5=1:%x]",flashStatus);
			old=*((volatile U16 *)BASE2470);
			flashStatus=*((volatile U16 *)BASE2470);
			if( (old&0x40) == (flashStatus&0x40) ){
				return 1;   //No toggle
			}else{
				printfx("\n Exceed timing error...");
				return 0;   //toggling
			}
		}
		old=flashStatus;
	}
	return 1;
}

/**********************************************************************/
int AM29F1600_ProgFlash(U32 realAddr,U16 data)
{
	volatile U16 *tempPt;
	tempPt=(volatile U16 *) (BASE2470+realAddr);
	_WR(0x555,0xaaaa);
	_WR(0x2aa,0x5555);
	_WR(0x555,0xa0a0);
	*tempPt=data;

	return _WAIT();
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
short flash_write(void)
{
	U32	targetAddr, targetSize;
	U32	sourceAddr;
	int	i;
	U32	ramData, flashData;
	U32 a, b;

	printfx("\n Flash Memory Programming...");
	if((FlashWriteStartAdr<0x10000)||(FlashWriteStartAdr&0x0FFFF)){
		printfx("\n [Error] Writing Point : %lx", FlashWriteStartAdr);
		return(NG);
	}
//	printfx("\n Check Flash ID...");
	if(!AM29F1600_CheckId()){
//		printfx("\n [Error]ID Check Error!!!");
		return(NG);
	}//else printfx(" O.K.");

	b = (rx0_size/0x10000)+1; 
	if(!(rx0_size%0x10000)) b-=1; 
	printfx("\n Block Size:%2x", b);
	if(b>4){ b = 4;	rx0_size=0x3fff0; }	//max 256kbyte
	for(a=0;a<b;a++){
		sourceAddr = (unsigned int)downdata+(a*0x10000);
		targetAddr = FlashWriteStartAdr+(a*0x10000);
		if(rx0_size>=0x10000){
			rx0_size -= 0x10000;
			targetSize = 0x10000;
		}else{
			targetSize = rx0_size;
			rx0_size = 0;
		}
	 	printfx("\n SourceAddr:%XH", sourceAddr);
 		printfx(", TargetAddr:%XH-%xH", targetAddr,(targetAddr+targetSize-1));

		printfx("\n EraseBlock...addr=%XH", targetAddr);
		AM29F1600_EraseSector(targetAddr);
		printfx(" EndErase");
		if(!BlankCheck(targetAddr,targetSize)){
			printfx("\n [Error]Blank Check Error!!!");
			AM29F1600_EraseSector(targetAddr);
			if(!BlankCheck(targetAddr,targetSize)){
				printfx("\n [Error]Blank Check Error! 2'nd");
				AM29F1600_EraseSector(targetAddr);
				if(!BlankCheck(targetAddr,targetSize)){
					printfx("\n [Error]Blank Check Error! 3'rd");
//	    			sti();			//rINTMSK &= ~(BIT_GLOBAL);    // global interrupt enable
					return(NG);
				}else printfx(", Blank Check O.K. 3'rd");
			}else printfx(", Blank Check O.K. 2'nd");
		}else printfx(", Blank Check O.K.");

		printfx("\n BlockWriting:  %2d [", a+1);
		for(i=0;i<targetSize;i+=2){
			if((i%0x1000)==0) printfx(" %x",i/0x1000);

			ramData = *(U16 *)(sourceAddr+i);
			AM29F1600_ProgFlash(i+targetAddr, ramData);
		}
		printfx(" ] End!!!");
		_RESET();

		printfx("\n BlockVerifying:%2d [", a+1);
		for(i=0;i<targetSize;i+=2){
			ramData = *(U16 *)(sourceAddr+i);
			flashData = *(U16 *)(BASE2470+targetAddr+i);
			if(flashData != ramData){
				printfx("\n [Error]verify error at %x\n",i+targetAddr);
				return(NG);
			}
			if((i%0x1000)==0) printfx(" %x",i/0x1000);
		}
		printfx(" ] End!!!");
		if(!rx0_size) break;
	}
	return(OK);
}
/**************************************************************************************/
void flash_end(char no){
//char a=0;	
	if(no<2){
		printfx("\n Program Fail!!![Bank%d]-ENER",no);
		dsp_prog_ng();
		for(;;){
			LED_GRN_ON(); LED_RED_OFF(); delay_ms(50);
			LED_RED_ON(); LED_GRN_OFF(); delay_ms(500);
//			if((++a)>30) enable_watchdog();
		}
	}else{
		printfx("\n Please! Power Off, Now-ENOK");
		dsp_prog_ok();
		for(;;){
			LED_GRN_ON(); LED_RED_OFF(); delay_ms(500);
			LED_RED_ON(); LED_GRN_OFF(); delay_ms(50);
//			if((++a)>3) enable_watchdog();
		}
	}
}

/**************************************************************************************/

void prog_decode(void){
	if(rxw0!=rxr0) flash_decode(SCRX0_Buf[rxr0++]); 

	if(rxw3!=rxr3){
//savcmd0(SCRX3_Buf[rxr3++]); 
		if(flag_flash==3) flash_decode(SCRX3_Buf[rxr3++]); 
		else system_decode(SCRX3_Buf[rxr3++]); 
	}
}

//#define MDTROMW
//		00   : '$'
//		01-09: <롬 파일명>    (ex :  MDT.001 )
//		10   : Data종류(0:code,1:font,2:boot,3:BMP)
//		11   : start address (롬 파일이 쓰여질 시작 번지)
//		12-14: Reserved
//		15-18: BIN data size LSB first
//		19   : RESERVED
//		20-23: checksum ( data를 byte단위로 더한 값, LSB16bit만사용, LSB first )
//		24-31: Date ( 07-04-20 )
//		32-END: Program data(BINary)

/************************** UART0 **************************/
void flash_decode(u8 dat){
u8 b1;
int i, j;
U32 position;
//savcmd0(b1); return;
	switch(rx0_step){
		case RX0_1st:
			if(dat=='$'){
				rx0_cnt=0;
				downdata[rx0_cnt++] = dat;
				rx0_step = RX0_2nd;
				printf03("\n Get1 $");
				flag_flash=1;
			}
			rx0_wait=RX0_WAIT;
		break;
		case RX0_2nd:
			downdata[rx0_cnt++] = dat;
			if(rx0_cnt>=0x20){
				b1=downdata[1];
				if((b1=='D')&&(downdata[2]=='F')){
					b1=downdata[3];
					if(((b1=='I')||(b1=='B')||(b1=='F')||(b1=='S'))&&(downdata[6]=='.')){
						rx0_step = RX0_RCV;
						rx0_size = downdata[17];
						rx0_size = (rx0_size<<8) + downdata[16];
						rx0_size = (rx0_size<<8) + downdata[15];
						rx0_size += 0x20;
						rx0_sum = downdata[21];
						rx0_sum = (rx0_sum<<8) + downdata[20];
						posi = downdata[ROMSELECT];
						if((posi==3)&&(b1=='B')) 	 { i = 0xFFE0;	j=0x008000; }  	//BOOT
						else if((posi==2)&&(b1=='S')){ i = 0x80000;	j=0x010000;	}	//BMP
						else if((posi==1)&&(b1=='F')){ i = 0x1FFF0;	j=0x018000; } 	//Font
						else if((posi==0)&&(b1=='I')&&(downdata[4]=='6')){ i = 0x3FFE0;	j=0x018000; } 	//Prog.
						else i = 0;						//data error
						if((rx0_size>i)||(rx0_size<j)){
							rx0_step=RX0_1st;
							printf0("\n Size Error:%lx,Posi:%2x   ", rx0_size, posi);
							for(i=0;i<0x20;i++) printf0(" %2x", downdata[i]);
						}else{
							rx0_step = RX0_RCV;
////							flag_flash = 1;
						}
						printf03(" *DFI%c-%c",downdata[4], b1);
					}else rx0_step=RX0_1st;
				}else{ 
					rx0_step=RX0_1st;
					delay_ms(100);
				}
				if(rx0_step==RX0_RCV){ 
					printf03(", Size:%lx",rx0_size);
					position = downdata[ROMSELECT+1];				//Position
					position *= 0x20000;
					position += BmpBaseAdr;
					if(posi==2) printf03(" Posi[%lx] ", position);
				}else{ 
					printf03(", No para.");
					for(b1=0;b1<0x20;b1++) savcmd0(downdata[b1]);
					flag_flash=0;
				}
			}
			if(flag_flash==3) rx3_wait=RX3_WAIT;
			else rx0_wait=RX0_WAIT;
		break;
		case RX0_RCV:
			downdata[rx0_cnt++] = dat;
			if(!(rx0_cnt&0xfff)){ i = rx0_cnt/0x1000; printf03(" %x", i); }
			if(flag_flash==3){
			  	while(rxw3!=rxr3){
					downdata[rx0_cnt++] = SCRX3_Buf[rxr3++];
					if(!(rx0_cnt&0xfff)){ i = rx0_cnt/0x1000; printf03(" %x", i); }
		  		}
		  	}else{
			  	while(rxw0!=rxr0){
					downdata[rx0_cnt++] = SCRX0_Buf[rxr0++];
					if(!(rx0_cnt&0xfff)){ i = rx0_cnt/0x1000; printf03(" %x", i); }
			  	}
			}
		  	
			if(rx0_cnt>=rx0_size){
				get_sum=0;
				for(i=0x20;i<rx0_size;i++) get_sum += downdata[i];
				if(rx0_sum==get_sum){ 
					printf03("\n *** Data Receive [OK] ***");
					printf03(" [chksum:%4x]", get_sum);
					while(txw0!=txr0);
					switch(posi){
						case 0:										//program
							printfx("\n Flash Memory Programming[Code]");
							FlashWriteStartAdr = FlashBaseAdr;
							if(flash_write()==OK){ 
								printfx("\n Program Success[Code]");
								flash_end(2);
							}else flash_end(0);
						break;
						case 1:										//Font
							printfx("\n Flash Memory Programming[FONT]");
							rx0_size+=0x20;
//							for(i=0;i<rx0_size;i++) downdata[i]=downdata[i+0x20];
							FlashWriteStartAdr = FontBaseAdr;
							if(flash_write()==OK){ 
								printfx("\n Program Success[FONT]");
								flash_end(2);
							}else flash_end(0);
						break;
						case 2:										//BMP
							rx0_size=0x20000;
							position = downdata[ROMSELECT+1];				//Position
							position *= rx0_size;
							position += BmpBaseAdr;
							for(i=0;i<rx0_size;i++) downdata[i]=downdata[i+0x20];
							FlashWriteStartAdr = position;	//for BMP Write Position
							if(flash_write()==OK){ 
								printfx("\n Program Success[BMP]-%lxENOK", position);
//								flash_end(2);
							}else{
								printfx("\n Program Error[BMP]-%lxENER", position);
//								flash_end(0);
							}
						break;
						case 3:										//Boot
							printfx("\n Flash Memory Programming[Reserved]");
							flash_end(2);
						break;
					}
				    sti();
				}else{ 
					printf03("\n *** Data Receive [NG] ***");
					printf03(" [rcvsum:%4x,getsum:%4x]", rx0_sum, get_sum);
				}
				rx0_step=RX0_1st;
			}
			if(flag_flash==3) rx3_wait=RX3_WAIT;
			else rx0_wait=RX0_WAIT;
		break;
	}
}		

/********************************************************************************/
/**************   SYSTEM CURVE, PARAMETER, STRIP INFO. RESULT DATA    **********/
/********************************************************************************/
void backup_write(void){
u32	i;
	for(i=0;i<STORE_SIZE;i++) savebuff[i] = FLASH_RAM[i];
	wait_tx0();
	for(i=0;i<10;i++){
		if(system_store('V')==OK){
			printfx(" WriteSuccess!!!");
			break;
		}else printfx("\n *** WriteErro:%d ***", i);
	}
}

/********************************************************************************/
/**************   Store RESULT DATA    **********/
/********************************************************************************/

void store_result(u16 tmp_seqno, u8 dno){		//dno는 사용안함.
u32	i, j, k, radr;

	wait_tx0();
	wait_tx3();
	
printf0("\n *store result:%d",tmp_seqno); 		//key_code=0; while(!key_code);
	if((!tmp_seqno)||(tmp_seqno>MAX_RECORD)){
		printf0("\n Result Write Error!!! %d", tmp_seqno);
		return;				//Do not store
	}else printf0(" - ResultWrite");

	i=0;
	cmdbuff[i++] = '*';	cmdbuff[i++] = 0x54;						//0, 1
	j = bin2hexasc4(tmp_seqno);
	cmdbuff[i++]=j>>24; cmdbuff[i++]=j>>16; cmdbuff[i++]=j>>8; cmdbuff[i++]=j>>0;
//	for(j=0;j<MAX_USERID;j++) cmdbuff[i++] = USER_ID[j];					//6, 15
	for(j=0;j<10;j++) cmdbuff[i++] = USER_ID[j];					//6, 15
	if(ADD10B>2){
//		for(j=0;j<ADD10B;j++) cmdbuff[i++] = ' ';						//16~25
		for(j=0;j<(MAX_USERID-10);j++) cmdbuff[i++] = USER_ID[j+10];	//16~21
		for(j=0;j<(ADD10B-(MAX_USERID-10));j++) cmdbuff[i++] = ' ';		//22~25
		cmdbuff[i-4] = LOAD_NO+'0';									//24
		cmdbuff[i-3] = ERROR_NO+'0';									//25
		cmdbuff[i-2] = COLOR_NO+'0';									//24
		cmdbuff[i-1] = TURBI_NO+'0';									//25
	}
	cmdbuff[i++]='2'; cmdbuff[i++]='0';								//26,27
	j = bin2bcdasc(MEASyear); 	cmdbuff[i++]=j>>8; cmdbuff[i++]=j;	//28,29
	j = bin2bcdasc(MEASmonth);	cmdbuff[i++]=j>>8; cmdbuff[i++]=j;	//30,31
	j = bin2bcdasc(MEASday); 	cmdbuff[i++]=j>>8; cmdbuff[i++]=j;	//32,33
	j = bin2bcdasc(MEAShour); 	cmdbuff[i++]=j>>8; cmdbuff[i++]=j;	//34,35
	j = bin2bcdasc(MEASmin); 	cmdbuff[i++]=j>>8; cmdbuff[i++]=j;	//36,37
	j = bin2bcdasc(MEASsec); 	cmdbuff[i++]=j>>8; cmdbuff[i++]=j;	//38,39

	for(j=0;j<24;j++) cmdbuff[i++] = STORE_RAM[STRIP_RAM+(STRIPNO*StripOffset)+j];	//40~63
	for(j=0;j<11;j++) cmdbuff[i++] = m_final_results[j]+'0';		//64~74
	for(j=0;j<11;j++){												//75~
		k = bin2hexasc4(CURVE_RED[j]);
		cmdbuff[i++]=k>>24; cmdbuff[i++]=k>>16;	cmdbuff[i++]=k>>8; cmdbuff[i++]=k>>0;
		k = bin2hexasc4(CURVE_GRN[j]);
		cmdbuff[i++]=k>>24; cmdbuff[i++]=k>>16;	cmdbuff[i++]=k>>8; cmdbuff[i++]=k>>0;
		k = bin2hexasc4(CURVE_BLU[j]);
		cmdbuff[i++]=k>>24; cmdbuff[i++]=k>>16;	cmdbuff[i++]=k>>8; cmdbuff[i++]=k>>0;
	}
	cmdbuff[i++] = URLF;											//207
//	k=0;
//	j = (tmp_seqno-1)*RESULT_PAGE; j &= 0xFFFF;
//printf0("\n SeqNo.%4d, ResultStoreAddr:0x%4x", tmp_seqno, j);
//	for(i=0;i<RESULT_SIZE;i++){
//		savebuff[j++] = cmdbuff[i];	
//		k += cmdbuff[i];
//	}
//	k = ~k;
//	savebuff[j++] = k&0xff;		//LSB
//	savebuff[j++] = k>>8;		//CHKSUM		//199,200
//	system_store('R');

	radr = tmp_seqno-1;
	radr = (radr*RESULT_PAGE) + RESULT_ADR;
	printf0("\n SeqNo.%4d, ResultStoreAddr:0x%lx", tmp_seqno, radr);
	readFR4k(radr, 0x1000);
	j = radr & 0x0FFF;
	k=0;
	for(i=0;i<RESULT_SIZE;i++){
		flash_read[j++] = cmdbuff[i];	
		k += cmdbuff[i];
	}
	k = ~k;
	flash_read[j++] = k&0xff;		//LSB
	flash_read[j++] = k>>8;			//CHKSUM		//199,200

//	printf0("\n Write \n");
//	for(i=0x0c00;i<0x1000;i++){ printf0("%c", flash_read[i]); }

	system_wr4k(radr);				//	system_store('R');

//	readFR4k(radr, 0x1000);
//	printf0("\n Read \n");
//	for(i=0x0c00;i<0x1000;i++){ printf0("%c", flash_read[i]); }
}

/********************************************************************************/
/**************       **********/
/********************************************************************************/
int system_store(u8 cmd){
u32	targetAddr;
u32	sourceAddr;
u32	i, data_size;
u32	ramData, flashData;

	printfx("\n Flash(System) Memory Programming...");
	if(!AM29F1600_CheckId()){
		printfx("\n ID Check Error!!!");
		return(NG);
	}else{
//		printfx(" I.D. Check O.K.");
	}

	sourceAddr = (unsigned int)savebuff;
	if(cmd=='V'){
		targetAddr = STORE_ROM;
		data_size = 0x10000;
	}else{
//		targetAddr = RESULT_ROM;				//사용안함.
//		data_size = 0x10000;
	    return(NG);
	}
 	printfx("\n SourceAddr:%XH", sourceAddr);
	printfx(", TargetAddr:%XH-%xH", targetAddr,(targetAddr+0x10000-1));

	printfx("\n EraseBlock...addr=%XH", targetAddr);
	AM29F1600_EraseSector(targetAddr);
	
//	if(!BlankCheck(targetAddr,0x10000)){
	if(!BlankCheck(targetAddr,data_size)){
		printfx("\n [Error]Blank Check Error!!!");
		return(NG);
	}else{
		printfx(", Blank Check O.K.");
	}

	printfx("\n SystemWriting:  %d [", targetAddr);
	for(i=0;i<data_size;i+=2){
		ramData = *(U16 *)(sourceAddr+i);
		AM29F1600_ProgFlash(targetAddr+i, ramData);
		if((i%0x1000)==0) printfx(" %x",i/0x1000);
	}
	printfx(" ] End!!!");
	_RESET();

	printfx("\n SystemVerifying:%d [", targetAddr);
	for(i=0;i<data_size;i+=2){
		ramData = *(U16 *)(sourceAddr+i);
		flashData = *(U16 *)(BASE2470+targetAddr+i);
		if(flashData != ramData){
			printfx("\n [Error]verify error at %x\n",i+targetAddr);
			return(NG);
		}
		if((i%0x1000)==0) printfx(" %x",i/0x1000);
	}
	printfx(" ] End!!!");

	printfx("\n [ Success Flash Write ]");
    return(OK);
}

/********************************************************************************/
void set_sys_curve(void){
u16 j, k;
	k=CURVE_RAM;						
	for(j=0;j<CURVE_SIZE;j++){
		STORE_RAM[k++] = cmdbuff[j];
	}
}

void set_sys_strip(void){
u16 j, k;
	k=STRIP_RAM;							
	for(j=0;j<STRIP_SIZE;j++){
		STORE_RAM[k++] = cmdbuff[j];
	}
}

void set_sys_para(void){
u16 j, k;
	k=PARA_RAM;							
	for(j=0;j<PARA_SIZE;j++){
		STORE_RAM[k++] = cmdbuff[j];
	}
}
/********************************************************************************/
void move_result(u16 sno){		// extROM ==> RAM
u32 radr;	
	radr = sno;
	radr = (radr*RESULT_PAGE) + RESULT_ADR;
	readFR4k(radr, 0x1000);
}
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/


