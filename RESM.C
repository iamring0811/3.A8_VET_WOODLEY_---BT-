#define _RTC_C_

#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\init.h"
#include ".\inc\uart.h"
#include ".\inc\Lcd.h"
#include ".\inc\touch.h"
#include ".\inc\resm.h"
#include ".\inc\eep.h"
#include ".\inc\dsp.h"
#include ".\inc\flash.h"
#include ".\inc\meas.h"

u8 ds_fault=0;
u8 buff256[0x100];
u8 Fstat;
u8 bcd2bin2(u8 ch);
u8 bin2bcd2(u8 bin);
/**************************************************************
************************ RTC(ds1302) **************************
***************************************************************/

void Delay1302(unsigned int d1){
	while(d1){
		d1--; d1++;	d1--; d1++;	
		d1--; d1++;	d1--; d1++;	d1--;
	}
}

void send_clk02(void){
//CLK_TEST:
	set_clk02(); Delay1302(25);
	clr_clk02(); Delay1302(15);
//goto CLK_TEST;
}

void sendRTC(u8 cmd){
u8 b1;
	rtc_write();
	for(b1=0;b1<8;b1++){
		if(cmd&0x01) set_dat02();
		else clr_dat02();
		cmd	= cmd >> 1;
		Delay1302(10);
		send_clk02();
	}
	Delay1302(10);
}
u8 dataRTC(void){
u8 b1, b2;
	rtc_read();
	b2=0;
	for(b1=0;b1<8;b1++){
		b2 = b2	>> 1;
		if(read_dat02()) b2	|= 0x80;
//Delay1302(10);
		send_clk02();
	}
	rtc_write();
	return(b2);
}

void writeRTC(u8 add, u8 sd){
u8 b1;
	clr_clk02();			//clk low
	en1302(); Delay1302(10);	//rst pin enable(ds1302	on)
	b1 = (add<<1) |	0x80;
	sendRTC(b1);
	sendRTC(sd);
Delay1302(200);
	dn1302();
Delay1302(200);
}

u8 readRTC(u8 add){
u8 b1;
	clr_clk02();
	en1302(); Delay1302(10);
	b1 = (add<<1) |	0x81;
	sendRTC(b1);
//	set_dat02();
	b1 = dataRTC();
Delay1302(200);
	dn1302();
Delay1302(200);
	return(b1);
}

/********************************************************************/

#define	DS_SLEEP 0x80
#define	DS_1224	 0x80

void initRTC(void){
u8 b1;
	rtc_write();
//while(1){	DS_ENABLE();	delay_ms(10);	DS_DISABLE();	delay_ms(10); };
	
	writeRTC(RTC_DSWP, 0x00);	 writeRTC(RTC_DSWP,	0x00);
//	writeRTC(RTC_TCRV, 0xA5);	 writeRTC(RTC_TCRV,	0xA5);
	writeRTC(RTC_TCRV, 0x00);	 writeRTC(RTC_TCRV,	0x00);

	b1=readRTC(RTC_SECOND);
	b1=readRTC(RTC_SECOND);
//printf0("\n DS_SLEEP:%2x", b1);	
	if((b1&DS_SLEEP)==DS_SLEEP){
		if((bcd2bin2(b1&0x7f))>59) writeRTC(RTC_SECOND,	0x00);
		else{
//			writeRTC(RTC_SECOND, b1&0x7f);
			b1 = b1&0x7f; if(b1<59) b1 += 1;
			writeRTC(RTC_SECOND, b1&0x7f);
		}
	}
	b1=readRTC(RTC_HOUR);
	if((b1&DS_1224)==DS_1224){
		if((bcd2bin2(b1)&0x7f)>23) writeRTC(RTC_HOUR, 0x00);
		else writeRTC(RTC_HOUR,	b1&0x7f);
	}
	b1=bcd2bin2( readRTC(RTC_DAY) );
	if(!b1||(b1>31)) writeRTC(RTC_DAY, 0x01);
	b1=bcd2bin2( readRTC(RTC_MONTH)	);
	if(!b1||(b1>12)) writeRTC(RTC_MONTH, 0x01);

	readAllRTC();
printf0("\n time:%4d-%2d-%2d,%2d-%2d-%2d", year,month,day,hour,min,sec);	
}


void readAllRTC(void){
	year =bcd2bin2(readRTC(RTC_YEAR));				//BCDȮ�ιٶ�.
	month=bcd2bin2(readRTC(RTC_MONTH));
	day	 =bcd2bin2(readRTC(RTC_DAY));
	hour =bcd2bin2(readRTC(RTC_HOUR));
	min	 =bcd2bin2(readRTC(RTC_MINUTE));
	sec	 =bcd2bin2(readRTC(RTC_SECOND));
}

void readTime(void){
//u16 a, b, smh;
u16 a, smh;
	smh = 0;
	a = bcd2bin2(readRTC(RTC_SECOND));
	if(a!=sec){
		time_flag = 1;
		smh = 's';
		sec = a;
		a = bcd2bin2(readRTC(RTC_MINUTE));
		if(a!=min){
			smh = 'm';
			min = a;
			a = bcd2bin2(readRTC(RTC_HOUR));
			if(a!=hour){
				smh = 'h';
				hour = a;
				day	 =bcd2bin2(readRTC(RTC_DAY));
				month=bcd2bin2(readRTC(RTC_MONTH));
				year =bcd2bin2(readRTC(RTC_YEAR));				//BCDȮ�ιٶ�.
			}
		}
		ds_fault=0;
	}else{
		if((a&DS_SLEEP)==DS_SLEEP){
			if((++ds_fault)>4){
//				writeRTC(RTC_SECOND, bin2bcd2(a&0x7F));
				a = bin2bcd2(a&0x7F);	//if(a<59) a += 1;
				writeRTC(RTC_SECOND, a);
				ds_fault=0;
			}
		}else{
			if((++ds_fault)>7){
//				writeRTC(RTC_SECOND, bin2bcd2(a&0x7F));
				a = bin2bcd2(a&0x7F);	if(a<59) a += 1;
				writeRTC(RTC_SECOND, a);
				ds_fault=0;
			}
		}
	}
//	if(smh&&(state==M_STANDBY)){
	if(smh&&(state>=M_STANDBY)){
		readAllRTC();
		time_copy(year, month, day, hour, min, sec);
		msg16(50,14,BLK,top_str);
	}
}


void out_time(void){
	readAllRTC();
	time_copy(year, month, day, hour, min, sec);
	msg16(50,14,BLK,top_str);
}

u8 bcd2bin2(u8 ch){
u8 a;
	a =	ch>>4; ch &= 0xf;
	return((a*10)+ch);
}
//BINARY�� 2�ڸ��� BCD CODE�� �ٲ۴�.
u8 bin2bcd2(u8 bin){
    bin %= 100;
    bin = ((bin/10)<<4) + (bin%10);
    return(bin);
}

///////////////////////////////rtc store
void sendAll_RTC(void){
	year  = setyear;
	month = setmonth;
	day	  =	setday;
	hour  =	sethour;
	min	  =	setmin;
	sec	  =	setsec;
	writeRTC(RTC_SECOND, bin2bcd2(sec));
	writeRTC(RTC_MINUTE, bin2bcd2(min));
	writeRTC(RTC_HOUR,	 bin2bcd2(hour));
	writeRTC(RTC_DAY,	 bin2bcd2(day));
	writeRTC(RTC_MONTH,	 bin2bcd2(month));
	writeRTC(RTC_YEAR,	 bin2bcd2(year));
}
void test_rtc_init(void){
//	initRTC();
	setyear=7;
	setmonth=5;
	setday=23;
	sethour=4;
	setmin=12;
	sendAll_RTC();
}
void test_rtc(void){
	year =readRTC(RTC_YEAR);				//BCDȮ�ιٶ�.
	month=bcd2bin2(readRTC(RTC_MONTH));
	day	 =bcd2bin2(readRTC(RTC_DAY));
	hour =bcd2bin2(readRTC(RTC_HOUR));
	min	 =bcd2bin2(readRTC(RTC_MINUTE));
	sec	 =bcd2bin2(readRTC(RTC_SECOND));

	printf0("\n%d-Year ", year);
	printf0("%d-Month ", month);
	printf0("%d-Dat ", day);
	printf0("%d-Hour ", hour);
	printf0("%d-Min ", min);
	printf0("%d-Sec", sec);
}

/**************************************************************
**************************	stemMotor *************************
***************************************************************/
void stepMotorOff(void){
	SM_CH1_OFF();SM_CH2_OFF();SM_CH3_OFF();SM_CH4_OFF(); SmDir=0;
}

void stepMotor(unsigned int rotate){		// 1 step�� �� 1.45mS~1.5mS�� ����?
	smRotCnt++;
	if(!smDlyDat){
		smDlyDat=SM_DLY;
		if((++SmStep)>3){ SmStep=0; Cycle=1; }
	}else{
		smDlyDat--;
		return;
	}
	switch(SmStep){
		case 0:
			if(rotate==JUNJIN){
				SM_CH1_ON();SM_CH2_OFF();SM_CH3_OFF();SM_CH4_ON();
			}else{
				SM_CH1_ON();SM_CH2_OFF();SM_CH3_OFF();SM_CH4_ON();
			}
			break;
		case 1:
			if(rotate==JUNJIN){
				SM_CH1_ON();SM_CH2_ON();SM_CH3_OFF();SM_CH4_OFF();
			}else{
				SM_CH1_OFF();SM_CH2_OFF();SM_CH3_ON();SM_CH4_ON();
			}
			break;
		case 2:
			if(rotate==JUNJIN){
				SM_CH1_OFF();SM_CH2_ON();SM_CH3_ON();SM_CH4_OFF();
			}else{
				SM_CH1_OFF();SM_CH2_ON();SM_CH3_ON();SM_CH4_OFF();
			}
			break;
		case 3:
			if(rotate==JUNJIN){
				SM_CH1_OFF();SM_CH2_OFF();SM_CH3_ON();SM_CH4_ON();
			}else{
				SM_CH1_ON();SM_CH2_ON();SM_CH3_OFF();SM_CH4_OFF();
			}
			break;
	}
}
void delay_stepM(unsigned int dl){
//unsigned int w1;
	while(dl)dl--;
//	for(w1=0;w1<dl;w1++);
}
void stepM_init(void){
	stepMotorOff();
	SmStep=0;
//	SmDir=0;
	SmCnt=0;
	smDlyDat=0;		//SM_DLY;
	Cycle=0;
}

void home_tray(void){
BYTE a;
	SENS_ON();
	a=0;
//	set_CCW_TICK(MAX_TICK+100);					//����(����)�Ѵ�.
	set_CCW_TICK(MAX_BACK);
	while(1){
		if(tm_10ms){
			tm_10ms=0;
			if(SmDir){
				if(SENS_BK){
					if((++a)>10){
						stepMotorOff(); 
						printf0("\n SmCnt:%d", SmCnt);
					}
				}else a=0;
			}
		}
		if(!SmDir)	break; 
	}		
	stepMotorOff(); 
}

#define TTT 1100

void go_CW(u16 cnt){
	stepM_init();
	while(!SW_TEST){
		SM_CH1_ON();SM_CH2_OFF();SM_CH3_OFF();SM_CH4_ON(); delay_us(TTT);
		SM_CH1_ON();SM_CH2_ON();SM_CH3_OFF();SM_CH4_OFF(); delay_us(TTT);
		SM_CH1_OFF();SM_CH2_ON();SM_CH3_ON();SM_CH4_OFF(); delay_us(TTT);
		SM_CH1_OFF();SM_CH2_OFF();SM_CH3_ON();SM_CH4_ON(); delay_us(TTT);
		cnt++;
	}	
	stepMotorOff();
	printf0("\n CW:%d",cnt);
}

void go_CCW(u16 cnt){
	stepMotor(WHOJIN);
	while(!SW_TEST){
		SM_CH1_ON();SM_CH2_OFF();SM_CH3_OFF();SM_CH4_ON(); delay_us(TTT);
		SM_CH1_OFF();SM_CH2_OFF();SM_CH3_ON();SM_CH4_ON(); delay_us(TTT);
		SM_CH1_OFF();SM_CH2_ON();SM_CH3_ON();SM_CH4_OFF(); delay_us(TTT);
		SM_CH1_ON();SM_CH2_ON();SM_CH3_OFF();SM_CH4_OFF(); delay_us(TTT);
		cnt++;
	}	
	stepMotorOff();
	printf0("\n CCW:%d",cnt);
}

void set_CW_TICK(u32 tick){
	SmStep=0;
	SmCnt=tick;
	printf0("\n StepMotor set_CW, Tick:%d", tick);
	SmDir=SM_CW;
}
void set_CCW_TICK(u32 tick){
	SmStep=0;
	SmCnt=tick;
	printf0("\n StepMotor set_CCW, Tick:%d", tick);
	SmDir=SM_CCW;
}
void set_CW(u32 msec){
	SmStep=0;
	SmCnt=(msec*1000)/(1338*4);
	printf0("\n StepMotor set_CW, Time:%d[msec]", msec);
	SmDir=SM_CW;
}
void set_CCW(u32 msec){
	SmStep=0;
	SmCnt=(msec*1000)/(1338*4);
	printf0("\n StepMotor set_CCW, Time:%d[msec]", msec);
	SmDir=SM_CCW;
}

void step_test(void){
unsigned int w1;

/*
	while(1){
		SmStep=0;
		SmCnt=4000;
		printf0("\n StepMotor run_CW, Time:%d[msec]", (SmCnt*12)/10);
		SmDir=SM_CW;
		while(SmDir);
		stepMotorOff();
		printf0("\n StepMotor Stop, Time:%d[msec]", 1000);
		delay_ms(1000);
		SmStep=0;
		SmCnt=3000;
		printf0("\n StepMotor run_CCW, Time:%d[msec]", (SmCnt*12)/10);
		SmDir=SM_CCW;
		while(SmDir);
		stepMotorOff();
		printf0("\n StepMotor Stop, Time:%d[msec]", 500);
		delay_ms(500);
	}
*/
	while(1){
		printf0("\n StepMotor CW");
		for(w1=0;w1<20000;w1++){stepMotor(JUNJIN);delay_stepM(150);}
		printf0("\n StepMotor Stop");
		for(w1=0;w1<1000;w1++){stepMotorOff();delay_stepM(500);}
		printf0("\n StepMotor CCW");
		for(w1=0;w1<20000;w1++){stepMotor(WHOJIN);delay_stepM(150);}
		printf0("\n StepMotor Stop");
		for(w1=0;w1<1000;w1++){stepMotorOff();delay_stepM(500);}
	}
}

/**************************************************************
**************************	 MPC3201  *************************
***************************************************************/

//#define	AD_DAT_READ		   (rPDATE & 0x800000) 	// portE.23  MPC3201 DATA READ
//#define AD_ENABLE()			rPDATE &=~0x040000 	// portE.18	MPC3201 CS
//#define AD_DISABLE()		rPDATE |=0x040000  	// portE.18

void Delay32(int dl){ int w1; for(w1=0;w1<dl;w1++); }
#define DLYCNT 3
void clock32(void){	AD_CLK_LO(); Delay32(DLYCNT); AD_CLK_HI(); Delay32(DLYCNT); }

u16 mpc3201(void){
u16 w1, w2;
	AD_CLK_LO();
	AD_ENABLE();
	Delay32(DLYCNT);								/* Tsample */
	clock32();
	clock32();
	clock32();		//null bit
	w1 = 0;
	for(w2=0;w2<12;w2++){
//		clock32();								/* Data	Read Colck */
		AD_CLK_LO();
Delay32(DLYCNT);						
		AD_CLK_HI(); 
Delay32(DLYCNT);						
		w1 = w1	<< 1;
		if(AD_DAT_READ) w1 |= 0x0001;
	}
	AD_DISABLE();
	AD_CLK_LO();
	
	return(w1&0x0fff);		//AD��ȯ��
}


/**************************************************************
**************************   F25L16PA   ************************
***************************************************************/

void sendFR(BYTE cmd);
void delay_FR(u16 dly);
void sendWRSR(void);
void writeFR(u32 add, BYTE wd);
void eraseFR(u32 add);
BYTE readFR(u32 add);
short cmpFR4k(u32 adr, u16 size);

void eraseFR4K(u32 add);
short blankFR4K(u32 add);
void read_stat(void);
void writeFR256(u32 add);

//====================================================================
void test_flash(u8 tno){
u32 i;
u16 j;
	printf0("\n Flash Memory Test");
	read_jedec();

	i = SYS_TEST_ADR;
	eraseFR(i);
	if(SFID==F25L){
		for(j=0;j<0x100;j++) buff256[j] = j+0xA0;	
		writeFR256(i);	
	}else{
		for(j=0;j<0x100;j++){ writeFR(i+j, j+0xA0); }	
	}

	for(j=0;j<0x100;j++){ if(readFR(i+j)!=((j+0xA0)&0xFF)) break; }
	if(j<0x100){			
		printf0("\n Flash Memory Error.1     ");
		for(j=0;j<0x100;j++){ 
			printf0(" %2x", readFR(i+j) );
			delay_ms(50);
		}
		for(j=0;j<tno;j++){
			key_buzzer();
			delay_ms(300);
		}
	}else{
		printf0("\t Pass First.");
		eraseFR(i);
		do{ read_stat(); }while(Fstat&0x01);
		if(SFID==F25L){
			for(j=0;j<0x100;j++) buff256[j] = j+0x40;	
			writeFR256(i);	
		}else{
			for(j=0;j<0x100;j++){ writeFR(i+j, j+0x40); }	
		}
		for(j=0;j<0x100;j++){ if(readFR(i+j)!=((j+0x40)&0xFF)) break; }
		if(j<0x100){			
			printf0("\n Flash Memory Error.2   ");
			for(j=0;j<0x100;j++){ 
				printf0(" %2x", readFR(i+j) );
				delay_ms(50);
			}
			for(j=0;j<tno;j++){
				key_buzzer();
				delay_ms(300);
			}
		}else{
			printf0("\t Flash Test OK!!!");	
		}
	}
	
	printf0("\n erase SYSTEM_RAM");	
	eraseFR(SYSTEM_RAM);
	delay_ms(100);
	printf0("\n erase CURVE_RAM");	
	eraseFR(CURVE_RAM);
	delay_ms(100);
	printf0("\n erase STRIP_RAM");	
	eraseFR(STRIP_RAM);
	delay_ms(100);
	printf0("\n erase PARA_RAM");	
	eraseFR(PARA_RAM);
	delay_ms(100);
	printf0("\n erase PFORM_RAM");	
	eraseFR(PFORM_RAM);
}

//====================================================================
void delay_FR(u16 dly){
	dly*=3;
	while(dly) dly--;
}
//====================================================================
void send_ck(void){	CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); }
//====================================================================
void sendFR(BYTE cmd){
	if(cmd&0x80) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x40) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x20) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x10) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x08) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x04) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x02) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x01) DI24_HI(); else DI24_LO(); send_ck();
}

//====================================================================
void sendWRSR(void){
	CE24_HI();
	CK24_LO();
	WP24_HI();
	delay_FR(10);

//	CE24_LO();	
//	sendFR(FWREN);
//	CE24_HI();
//	delay_FR(10);

	CE24_LO();	
	sendFR(FEWSR);
	CE24_HI();
	delay_FR(10);
	
	CE24_LO();
	sendFR(FWRSR);
	sendFR(0x00);
	if(SFID==F25L32) sendFR(0x00);				//FFFFFFFFFFFFFFFFFF
	CE24_HI();
}
//====================================================================
void writeFR(u32 add, BYTE wd){
	CE24_HI();
	CK24_LO();
	WP24_HI();		//WP24=1;
	delay_FR(1);
	CE24_LO();	
	delay_FR(1);
	sendFR(FWREN);
	CE24_HI();
	delay_FR(1);
	CE24_LO();	
	delay_FR(1);
	sendFR(FBTPR);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(add>>0);
	sendFR(wd);
	delay_us(10);
	WP24_LO();		//WP24=0;
	CE24_HI();
}
//====================================================================
void eraseFR(u32 add){
u16 i;
	add &= 0x1FF000;		//BlockSize = 4KB
	for(i=0;i<3;i++){
		eraseFR4K(add);
		if(SFID!=S25V){
			do{ read_stat(); }while(Fstat&0x01);
		}else delay_ms(40);	
		if(blankFR4K(add)==OK) break;
	}
	if(i>=3) printf0("\n Erase NG!!!(%d)", i);
}
//====================================================================
void eraseFR4K(u32 add){
	sendWRSR();
	CE24_HI();
	CK24_LO();
	WP24_HI();		//WP24=1;
	delay_FR(1);
	CE24_LO();	
	delay_FR(1);
	sendFR(FWREN);
	CE24_HI();
	delay_FR(1);
	CE24_LO();
	delay_FR(1);
	sendFR(FERASE);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(add>>0);
	delay_us(10);
	WP24_LO();		//WP24=0;
	CE24_HI();
}

//====================================================================
void writeFR4k(u32 adr, u16 size){
u16 i, j;
	adr &= 0x1FF000;		//BlockSize = 4KB
	eraseFR(adr);
	delay_us(10);
	if(size>0x1000) size = 0x1000;
	for(i=0;i<size; ){
		if(SFID!=S25V){
			for(j=0;j<0x100;j++) buff256[j] = flash_buff[i+j];
			i += 0x100;
			writeFR256(adr);
			adr += 0x100;
		}else{
			writeFR(adr++, flash_buff[i++] );			//교체바랍니다.
		}
		delay_us(10);
	}	
}

//====================================================================
BYTE readFR(u32 add){
u8 e1;	
	CE24_HI();
	CK24_LO();
	WP24_LO();		//WP24=0;
	CE24_LO();	
	sendFR(FREAD);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(add>>0);
	e1=0;
	if(DO24) e1 |= 0x80; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x40; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x20; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x10; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x08; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x04; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x02; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x01; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	CE24_HI();
	return(e1);
}

//====================================================================
void readFR4k(u32 add, u16 size){
u16 j;	
u8 e1;
	add &= 0x1FF000;		//BlockSize = 4KB
	CE24_HI();
	CK24_LO();
	WP24_LO();		//WP24=0;
	CE24_LO();	
	sendFR(FREAD);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(0x00);
	if(size>0x1000) size = 0x1000;
	for(j=0;j<size;j++){
		e1=0;
		if(DO24) e1 |= 0x80; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x40; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x20; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x10; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x08; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x04; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x02; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x01; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		flash_read[j] = e1;
//		WDTR = 0xFF;        				// watchdog timer reflash
	}
	CE24_HI();
}

//====================================================================
short cmpFR4k(u32 adr, u16 size){
u16 i;
//	printf0("\n CMP:%lx,%d",adr,size);
	readFR4k(adr, size);
//	for(i=0;i<size;i++) printf0(" %2x,%2x", flash_buff[i], flash_read[i]);
	for(i=0;i<size;i++) if(flash_buff[i]!=flash_read[i]) return(NG);
	return(OK);
}

//====================================================================
u8 read24(u16 adr){	return(STORE_RAM[adr]); }
u16 read24w(u16 adr){
u16 w;
	w = STORE_RAM[adr+0];
	w = (w<<8) + STORE_RAM[adr+1];
	return(w);
}
//====================================================================
void write24(u16 adr, u8 dat){	STORE_RAM[adr+0] = dat; }
void write24w(u16 adr, u16 dat){
	STORE_RAM[adr+0] = (dat>>8);
	STORE_RAM[adr+1] = (dat&0xff);
}

/********************************************************************************/
/**************   SYSTEM CURVE, PARAMETER, STRIP INFO. RESULT DATA    **********/
/********************************************************************************/

void system_write(u32 adr, u16 size){
u16	i;
	for(i=0;i<size;i++) flash_buff[i] = STORE_RAM[adr+i];
//	printf0("\n SW:");
//	for(i=0;i<size;i++) printf0(" %2x,",flash_buff[i]);
	for(i=0;i<10;i++){
		writeFR4k(adr, size);
		if(cmpFR4k(adr, size)==OK) break;
	}
	if(i<10){ printf0("\n Flash(ext) Write OK!"); }
	else{	  printf0("\n Flash(ext) Write NG!"); }
}
//====================================================================
//#define SIZE4KS	0x1000-0x0020
#define SIZE4KS	0x1000
void system_wr4k(u32 adr){
u16 i;	
	for(i=0;i<0x1000;i++) flash_buff[i] = flash_read[i];
	for(i=0;i<10;i++){
		writeFR4k(adr, SIZE4KS);
		wait_tx0();
		if(cmpFR4k(adr, SIZE4KS)==OK) break;
		wait_tx0();
	}
	if(i<10){ printf0("\n Flash(ext) Wr4k OK![%d]", i); }
	else{	  printf0("\n Flash(ext) Wr4k NG!"); }
}
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
short blankFR4K(u32 add){
u16 i;
	readFR4k(add,0x1000);
	for(i=0;i<0x1000;i++){
		if(flash_read[i]!=0xFF) return(NG);
	}
	return(OK);
}

//====================================================================
void writeFR256(u32 add){
u16 j;	
	add &= 0x1FFF00;		//PageSize = 0x100
	do{ read_stat(); }while(Fstat&0x01);
	CE24_HI();
	CK24_LO();
	WP24_HI();		//WP24=1;
	delay_FR(1);
	CE24_LO();	
	delay_FR(1);
	sendFR(FWREN);
	CE24_HI();
	delay_FR(1);
//	read_stat();	printf0("\n Stat2:%2x", Fstat);
	do{ read_stat(); }while(Fstat&0x01);
//	read_stat();	printf0("\n Stat21:%2x", Fstat);
	CE24_LO();	
	delay_FR(1);
	sendFR(FBTPR);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(add>>0);
//	sendFR(wd);
	for(j=0;j<0x100;j++){
		delay_FR(10);
		sendFR(buff256[j]);
	}
//	read_stat();	printf0("\n Stat3:%2x", Fstat);
	delay_us(10);
	do{ read_stat(); }while(Fstat&0x01);
//	read_stat();	printf0("\n Stat4:%2x", Fstat);
	WP24_LO();		//WP24=0;
	CE24_HI();
}
//====================================================================
short read_jedec(void){
u32 e1,e2;
	CE24_HI();
	CK24_LO();
	WP24_LO();
	delay_FR(10);
	CE24_LO();	
	sendFR(FJDRD);
	delay_FR(1);
	
	e1=0;
	for(e2=0;e2<24;e2++){
		e1 <<= 1;
		if(DO24) e1 |= 0x01; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 		
	}
	CE24_HI();
	printf0("\n Jedec:%lx", e1);
//	sprintf(charbuff, "\nJD:%lx", e1);
//	msg24(25,Y_RDJD,BLU,charbuff);
	if(e1==0x8C2015){
		SFID = F25L;
		printf0(" [F25L]");
//		msg24(X_RDJD,Y_RDJD,BLU,"[F25L]");
		delay_ms(2000);
		return(OK);
	}else if(e1==0xBF2541){
		SFID = S25V;
		printf0(" [S25V]");
//		msg24(X_RDJD,Y_RDJD,BLU,"[S25V]");
		delay_ms(2000);
		return(OK);
	}else if(e1==0x1C7015){
		SFID = EN25;
		printf0(" [EN25]");
//		msg24(X_RDJD,Y_RDJD,BLU,"[EN25]");
		delay_ms(2000);
		return(OK);
	}else if(e1==0x8c4116){
		SFID = F25L32;
		printf0(" [F25L32]");
//		msg24(X_RDJD,Y_RDJD,BLU,"[F25L32]");
		delay_ms(2000);
		return(OK);
	}else{
		SFID = XX25;
		printf0(" [!NG!]");
//		msg24(X_RDJD,Y_RDJD,RED,"[!NG!]");
		for(e1=0;e1<5;e1++){
			key_buzzer(); delay_ms(400);
		}
		return(NG);
	}
}

//====================================================================
void read_stat(void){
u32 e1,e2;
	CE24_HI();
	CK24_LO();
//	WP24_LO();
	delay_FR(10);
	CE24_LO();	
	sendFR(FRDSR);
	delay_FR(1);
	
	e1=0;
	for(e2=0;e2<8;e2++){
		e1 <<= 1;
		if(DO24) e1 |= 0x01; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 		
	}
	CE24_HI();
//	printf0("\n Stat:%lx", e1);
	Fstat=e1;
}

//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
///////////////2019-04-04 ���� 1:33:15 Start
void dayInMonth(u16 year,u8 month){
	year+=2000;
	switch(month){
		case 1: case 3: case 5: case 7: case 8: case 10: case 12: 
			dayCount = 31;
		break;
		case 4: case 6: case 9: case 11:
			dayCount = 30;
		break;
		case 2:
			if (!(year % 400))dayCount = 29;
			else if (!(year % 100))dayCount = 28;
			else if (!(year % 4))dayCount = 29;
			else dayCount = 28;
		break;
	}
}
/////////////////2019-04-04 ���� 1:33:23End