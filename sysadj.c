#define	_SYSADJ_C_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\init.h"
#include ".\inc\uart.h"
#include ".\inc\lcd.h"
#include ".\inc\flash.h"
#include ".\inc\touch.h"
#include ".\inc\resm.h"
#include ".\inc\EEP.h"
#include ".\inc\Meas.h"
#include ".\inc\com.h"
#include ".\inc\sysadj.h"
#include ".\inc\dsp.h"

char PrintSystem1[] = "= = = = System Check  = = = = = ";	
char PrintSystem3[] = "= = = = = = = = = = = = = = = = ";	
char PrintSystem4[] = "      System Check Fail         ";
char PrintSystem5[] = "      System Check Success      ";
void set_print_time(void);

#define PadPoint	1		//was 0

//20150519 start
extern void backup_flash(u16 adr, u16 size);

/********************************************************************************************/
void out_dac(u8 dat){
	if(dat&0x01){ FIO2DIR |= DAC0; DAC0_LO(); }else{ FIO2DIR &= ~DAC0; }
	if(dat&0x02){ FIO1DIR |= DAC1; DAC1_LO(); }else{ FIO1DIR &= ~DAC1; }
	if(dat&0x04){ FIO1DIR |= DAC2; DAC2_LO(); }else{ FIO1DIR &= ~DAC2; }
	if(dat&0x08){ FIO3DIR |= DAC3; DAC3_LO(); }else{ FIO3DIR &= ~DAC3; }
}

#define DAC_RGB	2800
u16 dac_abs(u16 dat){
	if(dat>DAC_RGB) return(dat-DAC_RGB);
	else return(DAC_RGB-dat);	
}


char set_balance(u16 yoff){
u16 w1, w2, w3, ldat=0;
u32 buf[3][16];
u8  sav_r,sav_g,sav_b;
	sav_r=dac_r; sav_g=dac_g; sav_b=dac_b;

	/*if((start_strip_point<(srtGIULGI+5))||(start_strip_point>(srtGIULGI+40))){
		printf0("\n Error Start Strip Point!!!");
		sprintf(charbuff, "Error.SSP:%d", start_strip_point );
		if(!yoff) msg24(62,294,RED,charbuff);
		else msg24(62,294+yoff,WHT,charbuff);
		for(w1=0;w1<3;w1++){ BUZZ_HI();	dur_PWM = 40; delay_ms(1000); }
		return(0);
	}*/

	flag_dac=1;
	flag_adc=0;

	for(w1=0;w1<16;w1++){	buf[0][w1] = buf[1][w1] = buf[2][w1] = 0; }	
	for(ldat=0;ldat<256;ldat++){
		out_dac(ldat&15);
		RGB_RED_ON();	delay_ms(2);	w1=mpc3201();	buf[0][ldat&15] += w1;
		RGB_GRN_ON();	delay_ms(2);	w1=mpc3201();	buf[1][ldat&15] += w1;	
		RGB_BLU_ON();	delay_ms(2);	w1=mpc3201();	buf[2][ldat&15] += w1;
	}	
	for(w1=0;w1<16;w1++){ buf[0][w1] /= 16;	buf[1][w1] /= 16; buf[2][w1] /= 16;	}	
	flag_dac=0;
	/* 2015-06-09 오전 8:51:44Start
	for(w1=0;w1<16;w1++){
		printf0("\n %2d, R:%4d", w1, buf[0][w1]);
		printf0(" G:%4d",buf[1][w1]);
		printf0(" B:%4d",buf[2][w1]);
	}2015-06-09 오전 8:51:49 End */
	
	dac_r=dac_g=dac_b=0;
	for(w1=1;w1<16;w1++){
		w2 = dac_abs(buf[0][w1]);
		w3 = dac_abs(buf[0][dac_r]);
		if(w2<w3) dac_r = w1;
		w2 = dac_abs(buf[1][w1]);
		w3 = dac_abs(buf[1][dac_g]);
		if(w2<w3) dac_g = w1;
		w2 = dac_abs(buf[2][w1]);
		w3 = dac_abs(buf[2][dac_b]);
		if(w2<w3) dac_b = w1;
	}
/* 2015-06-09 Start	
	printf0("\n R:%d-%4d", dac_r, buf[0][dac_r]);
	printf0("   G:%d-%4d", dac_g, buf[1][dac_g]);
	printf0("   B:%d-%4d", dac_b, buf[2][dac_b]);	

	sprintf(charbuff, "R:%2d  G:%2d  B:%2d", dac_r, dac_g, dac_b );
	2015-06-09 오전 8:52:44 End */
//	if(!yoff) msg16(62+10,294,BLU,charbuff);
//	else msg16(62+10,294+yoff,WHT,charbuff);
//	for(w1=0;w1<3;w1++){
//	    BUZZ_HI();	dur_PWM = 20;
//		LCDbmpxx(b_balrgb, 30,96+28+72+72+72);	delay_ms(300);
//		msg20(50,345,BLU,charbuff);				delay_ms(400);
//	}

	if((sav_r!=dac_r)||(sav_g!=dac_g)||(sav_b!=dac_b)){
		w1 = ((dac_r<<12)&0xF000) | ((dac_g<<8)&0x0F00) | ((dac_b<<4)&0x00F0) | ((dac_r+dac_g+dac_b)&0x000F);
		write24w(DAC_ADR, w1);
		write_id_chksum();
		backup_flash(SYSTEM_RAM, SYSTEM_SIZE);
		flag_balance=OK;
		return(1);
	}else return(0);

//	delay_ms(1000);
//	BUZZ_HI();	dur_PWM = 5;
}

void set_default_RGB(void){
	dac_r = (INI_RGB>>8)&0x0F;
	dac_g = (INI_RGB>>4)&0x0F;
	dac_b = (INI_RGB>>0)&0x0F;
}
/********************************************************************************************/
void read_balance(void){
u16 w1, w2;
	w1=read24w(DAC_ADR);
	dac_r = (w1>>12)&15;
	dac_g = (w1>> 8)&15;
	dac_b = (w1>> 4)&15;
	if(((dac_r+dac_g+dac_b)&15)==(w1&15)){
		flag_balance=OK;
		//printf0("\n DAC Balance: R-%d, G-%d, B-%d", dac_r, dac_g, dac_b );2015-06-09 오전 8:53:02
	}else{
		flag_balance=NG;
		set_default_RGB();
//		printf0("\n Error. DAC Balance!!! - %4x", w1);2015-06-09 오전 8:53:09
	}
}

/********************************************************************************************/
void chk_auto_balance(void){
u16 w1, w2;	
	PINSEL2 &= ~0x03000C00;		//DAC2,1
	PINSEL4 &= ~0x0000000C;		//DAC0
	PINSEL7 &= ~0x000000C0;		//DAC3
	flag_dac=1;
	flag_adc=0;

	w2=0;
	for(w1=0;w1<10;w1++){
		out_dac(0x0E);		delay_ms(2);
		if(DAC0_READ) w2++;
		DAC3_HI();	DAC2_HI();	DAC1_HI();	delay_ms(2);
		if(!DAC0_READ) w2++;
	}
	if(!w2){
		BALANCE_AUTO = 1;
//		printf0("\n Auto DAC Balance Ready");2015-06-09 오전 8:53:19
	}else{
		BALANCE_AUTO = 0;
//		printf0("\n No Auto DAC Balance!!![%d]", w2);2015-06-09 오전 8:53:23
	}
	RGB_ALL_OFF();
	flag_dac=0;
}
//20150519 end


void system_check(void){
u16 i;	
	draw_box(CABXS,CABYS,CABXE,CABYE,WHT,RAST1);
	draw_box(SYBXS,SYBYS,SYBXE,SYBYE,WHT,RAST1);
	LCDbmpxx(b_box6,   35,88+48+60+60+4);					

	move_strip_info();
	meas_time=0;
	run_meas_adj(CABYS,CABYE);
	span_curve();
	get_start_strip_point();

	if(BALANCE_AUTO){
		i=set_balance(0);
		if(i){
			move_strip_info();
			meas_time=0;
			run_meas_adj(CABYS,CABYE);
			span_curve();
			get_start_strip_point();
		}
	}else delay_ms(500);

	run_adjust();
	for(i=0;i<MAX_PAD_NO;i++){		//	get_pad_data();
		dataRED1[i] = whtRED;
		dataGRN1[i] = whtGRN;
		dataBLU1[i] = whtBLU;
	}
	pass_curve();

	run_meas_adj(SYBYS,SYBYE);
	span_curve();
	get_start_strip_point();

	for(i=0;i<MAX_PAD_NO;i++){		//	get_pad_data();
		dataRED1[i] = whtRED;
		dataGRN1[i] = whtGRN;
		dataBLU1[i] = whtBLU;
	}
	pass_curve();
	delay_ms(500);

	out_check();

}
/***************************************************************/
/***************************************************************/
/***************************************************************/
/***************************************************************/
//#define MEV_DLY	1440		//334;
#define MEV_DLY	1
void run_meas_adj(u16 YS, u16 YE){
int gt, gb, ge, a;
	SENS_ON();
	MEASyear = year;
	MEASmonth = month;
	MEASday = day;
	MEAShour = hour;
	MEASmin = min;
	MEASsec = sec;

	send_meas_step('1');
	a=0;
	if(DOUBLE_FLAG==1){
		
		gb=CABXS;
		ge = CABXE - CABXS;
	//	gt = ge*MEV_DLY;									//334;
		gt = (ge*12)/10;
		delay_set(gt);
		set_CCW_TICK(MAX_TICK+100);							//후진(복귀)한다.
		while(1){
			if(!t0dly){
				delay_set(gt);
				if(ge){	draw_box(gb,YS,gb+1,YE,MSBBC,RASTNO); ge--; }
				gb++;										//	if(gb>=MSBXE) break;
			}else if(tm_10ms){
				tm_10ms=0;
				if(SmDir){
					if(SENS_BK){ if((++a)>10){ stepMotorOff(); } }else a=0;
				}
			}
			if((!ge)&&(!SmDir))	break; 
		}		
		stepMotorOff(); 
		delay_ms(100);
	
		send_meas_step('2');
		gb=CABXS;
		ge = CABXE - CABXS;
	//	gt = (ge*MEV_DLY);
		gt = (ge*12)/10;
		delay_set(gt);
		set_CW_TICK(MAX_TICK);								//전진한다.
		div_adc=0;	pnt_adc=0;
		RGB_GRN_ON();
		div_adc=0;
		flag_adc=1;											//측정을 시작한다.
		while(1){		
			if(!t0dly){
				delay_set(gt);
				if(ge){	draw_box(gb,YS,gb+1,YE,BLU,RASTNO);	ge--; }
				gb++;										//	if(gb>=MSBXE) break;
			}
			if((!ge)&&(!SmDir))	break; 
		}
		flag_adc=0;
		xchg_rgb();
		stepMotorOff(); 
		RGB_ALL_OFF();
		key_clear();
		com_clear();
		SENS_OFF();
		send_meas_step('3');
	}else{
		gb=CABXS;
		ge = CABXE - CABXS;
	//	gt = ge*MEV_DLY;									//334;
		gt = (ge*12)/10;
		delay_set(gt);
		set_CCW_TICK(MAX_TICK_1+100);							//후진(복귀)한다.
		while(1){
			if(!t0dly){
				delay_set(gt);
				if(ge){	draw_box(gb,YS,gb+1,YE,MSBBC,RASTNO); ge--; }
				gb++;										//	if(gb>=MSBXE) break;
			}else if(tm_10ms){
				tm_10ms=0;
				if(SmDir){
					if(SENS_BK){ if((++a)>10){ stepMotorOff(); } }else a=0;
				}
			}
			if((!ge)&&(!SmDir))	break; 
		}		
		stepMotorOff(); 
		delay_ms(100);
	
		send_meas_step('2');
		gb=CABXS;
		ge = CABXE - CABXS;
	//	gt = (ge*MEV_DLY);
		gt = (ge*12)/10;
		delay_set(gt);
		set_CW_TICK(MAX_TICK_1);								//전진한다.
		div_adc=0;	pnt_adc=0;
		RGB_GRN_ON();
		div_adc=0;
		flag_adc=1;											//측정을 시작한다.
		while(1){		
			if(!t0dly){
				delay_set(gt);
				if(ge){	draw_box(gb,YS,gb+1,YE,BLU,RASTNO);	ge--; }
				gb++;										//	if(gb>=MSBXE) break;
			}
			if((!ge)&&(!SmDir))	break; 
		}
		flag_adc=0;
		xchg_rgb();
		stepMotorOff(); 
		RGB_ALL_OFF();
		key_clear();
		com_clear();
		SENS_OFF();
		send_meas_step('3');
	
	}
}
/******************************************************************************/
/******************************************************************************/
void run_adjust(void){
u8  k;
u32 h;
	printf0("\n R:%5d,%5d", whtRED, blkRED);
	printf0("   G:%5d,%5d", whtGRN, blkGRN);
	printf0("   B:%5d,%5d", whtBLU, blkBLU);

	for(k=0;k<14;k++){
		switch(k){
			case 0:
				SYS_CURVE_RED[k]=0;
				SYS_CURVE_GRN[k]=0;
				SYS_CURVE_BLU[k]=0;
			break;
			case 1:
				SYS_CURVE_RED[k]=blkRED/2;
				SYS_CURVE_GRN[k]=blkGRN/2;
				SYS_CURVE_BLU[k]=blkBLU/2;
			break;
			case 2:
				SYS_CURVE_RED[k]=blkRED;
				SYS_CURVE_GRN[k]=blkGRN;
				SYS_CURVE_BLU[k]=blkBLU;
			break;
			case 11:
				SYS_CURVE_RED[k]=whtRED;
				SYS_CURVE_GRN[k]=whtGRN;
				SYS_CURVE_BLU[k]=whtBLU;
			break;
			case 13:
				SYS_CURVE_RED[k]=13000;
				SYS_CURVE_GRN[k]=13000;
				SYS_CURVE_BLU[k]=13000;
			break;
			default:
				h = whtRED - blkRED;
                h = (h * (k-2)) / 9;
				SYS_CURVE_RED[k] = h + blkRED;  

				h = whtGRN - blkGRN;
                h = (h * (k-2)) / 9;
				SYS_CURVE_GRN[k] = h + blkGRN;  

				h = whtBLU - blkBLU;
                h = (h * (k-2)) / 9;
				SYS_CURVE_BLU[k] = h + blkBLU;  
			break;					
		}
	}
}
/******************************************************************************/
/******************************************************************************/
void out_check(void){
short  chk_result;	
u16 i;
u16 R_MIN, R_MAX;
u16 G_MIN, G_MAX;
u16 B_MIN, B_MAX;

	i = PARA_RAM+WEIGHT_SIZE+(12*7*17);
	R_MIN = read_ram4(i);	i+=4;
	R_MAX = read_ram4(i);	i+=8;
	G_MIN = read_ram4(i);	i+=4;
	G_MAX = read_ram4(i);	i+=8;
	B_MIN = read_ram4(i);	i+=4;
	B_MAX = read_ram4(i);	i+=8;
	printf0("\n Rmin:%4d,Rmax:%4d", R_MIN,R_MAX);
	printf0("\t Gmin:%4d,Gmax:%4d", G_MIN,G_MAX);
	printf0("\t Bmin:%4d,Bmax:%4d", B_MIN,B_MAX);

	sprintf(charbuff, "V1:%4d (1080-1120)", CURVE_RED[PadPoint]);		
	msg16(62,300,BLU, charbuff);
	sprintf(charbuff, "V2:%4d (1080-1120)", CURVE_GRN[PadPoint]);
	msg16(62,322,BLU, charbuff);
	sprintf(charbuff, "V3:%4d (1080-1120)", CURVE_BLU[PadPoint]);
	msg16(62,344,BLU, charbuff);
	
//printf0("\n curve R:%4d, G:%4d, B:%4d", CURVE_RED[0], CURVE_GRN[0], CURVE_BLU[0]);
	chk_result=OK;
	if(CURVE_RED[PadPoint]>R_MAX) chk_result=NG;
	if(CURVE_RED[PadPoint]<R_MIN) chk_result=NG;
	if(CURVE_GRN[PadPoint]>G_MAX) chk_result=NG;
	if(CURVE_GRN[PadPoint]<G_MIN) chk_result=NG;
	if(CURVE_BLU[PadPoint]>B_MAX) chk_result=NG;
	if(CURVE_BLU[PadPoint]<B_MIN) chk_result=NG;
	if(chk_result!=OK){
		msg24( 90,370,RED, "  FAIL  ");
	}else{
		msg24( 90,370,BLU, "Success ");
	}
//	msg24(220,190,0xff, "Return");


	strcpy(char_buff,"= = = = System-Check  = = = = = ");
	print_thermal("\n%s", char_buff);		wait_printing();
	MEASyear  = year;
	MEASmonth = month;
	MEASday   = day;
	MEAShour  = hour;
	MEASmin   = min;
	MEASsec   = sec;
	set_print_time();
	print_thermal("\n%s", char_buff);			wait_printing();
	strcpy(char_buff,"= = = = = = = = = = = = = = = = ");
	print_thermal("\n%s", char_buff);		wait_printing();
	sprintf(charbuff, "      V1:%4d (1080-1120)", CURVE_RED[PadPoint]);
	print_thermal("\n%s", charbuff);
	sprintf(charbuff, "      V2:%4d (1080-1120)", CURVE_GRN[PadPoint]);
	print_thermal("\n%s", charbuff);
	sprintf(charbuff, "      V3:%4d (1080-1120)", CURVE_BLU[PadPoint]);
	print_thermal("\n%s", charbuff);
	if(chk_result!=OK){
		strcpy(char_buff,"      System Check Fail         ");
	}else{
		strcpy(char_buff,"      System Check Success      ");
	}
	print_thermal("\n\n%s", char_buff);	wait_printing();
	strcpy(char_buff,"= = = = = = = = = = = = = = = = ");
	print_thermal("\n%s", char_buff);		wait_printing();
	print_thermal("\n S/N: %s",SYS_ID);		//??????????????
	strcpy(char_buff,"= = = = = = = = = = = = = = = = ");
	print_thermal("\n%s", char_buff);		wait_printing();
	print_thermal("\n\n\n\n\n\n\n");		wait_printing();

}

/******************************************************************************/
/**************************************************************/
void set_print_time(void){
u32 w1;
u8  b1;
//printf03("\n H:%2d,M:%2d,S:%2d", hour, min, sec);
	strcpy(char_buff," 2000/01/01         AM 00:00:00");
	w1 = bin2bcdasc4(MEASyear);
	char_buff[3] = w1>>8;
	char_buff[4] = w1>>0;
	w1 = bin2bcdasc4(MEASmonth);
	char_buff[6] = w1>>8;
	char_buff[7] = w1>>0;
	w1 = bin2bcdasc4(MEASday);
	char_buff[9] = w1>>8;
	char_buff[10] = w1>>0;
	if(MEAShour<12){
		if(!MEAShour) b1=12;
		else b1=MEAShour;
	}else{
		char_buff[20]='P'; 
		if(MEAShour==12) b1 = 12;				//오후 12시
		else b1 = MEAShour-12;
	}
	w1 = bin2bcdasc4(b1);
	char_buff[23] = w1>>8;
	char_buff[24] = w1>>0;
	w1 = bin2bcdasc4(MEASmin);
	char_buff[26] = w1>>8;
	char_buff[27] = w1>>0;
	w1 = bin2bcdasc4(MEASsec);
	char_buff[29] = w1>>8;
	char_buff[30] = w1>>0;
}
/******************************************************************************/
