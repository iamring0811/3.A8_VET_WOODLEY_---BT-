
#define	_MEAS_C_

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\init.h"
#include ".\inc\uart.h"
#include ".\inc\touch.h"
#include ".\inc\resm.h"
#include ".\inc\LCD.h"
#include ".\inc\EEP.h"
#include ".\inc\Meas.h"
#include ".\inc\COM.h"
#include ".\inc\flash.h"
#include ".\inc\para.h"
#include ".\inc\Sysadj.h"
#include ".\inc\dsp.h"

void make_result(void);

void wait_analysis(u16 wsec);
void meas_double(void);
u16  put_result1(u16 i, u8 x, u8 y);
u16  put_arb(u16 i, u8 x, u8 c);

#define pntPRO		0x8
#define pntLUC		0xA
#define pntALB		0xC
#define pntCRE		0xD
#define pntMDA		0xE		//G
#define pntBHB		0xF		//G
#define pntCAL		0x10		//G
#define pntLDH		0x11		//G

u16 put_albumin(u16 i, u8 x);

#define RGB_DATA_SRT	65
#define STRIP_DATA_SRT	30+12
void chg_rgb_data(void);
u8 check_double(void);

/*******************************************************************************/
void span_curve(void){
u32 i, j, k;
	for(i=0;i<5;i++){
		buffRED1[pnt_adc+i] = buffRED1[pnt_adc-1];
		buffGRN1[pnt_adc+i] = buffGRN1[pnt_adc-1];
		buffBLU1[pnt_adc+i] = buffBLU1[pnt_adc-1];
	}
	for(i=0;i<pnt_adc;i++){
		k=0; for(j=0;j<5;j++) k += buffRED1[i+j]; spanRED[i] = (k*3)/5;
		if(DSW3==0){
			k=0; for(j=0;j<5;j++) k += buffGRN1[i+j]; spanBLU[i] = (k*3)/5;
			k=0; for(j=0;j<5;j++) k += buffBLU1[i+j]; spanGRN[i] = (k*3)/5;
		}else{
			k=0; for(j=0;j<5;j++) k += buffGRN1[i+j]; spanGRN[i] = (k*3)/5;
			k=0; for(j=0;j<5;j++) k += buffBLU1[i+j]; spanBLU[i] = (k*3)/5;
		}
	}
}

/*******************************************************************************/
/* 스트립의 시작점계산 */
#define srtGIULGI	130		//was 140
#define srtGIULGI_1	165		//was 140

void get_start_strip_point(void){
u16 i, j, k, a;
printf0("\n *GIULGI:%d",GIULGI); 
	j = spanRED[srtGIULGI];
	a=0;
	for(i=srtGIULGI;i<pnt_adc;i++){
		k = spanRED[i];
		if(k>(j+GIULGI)){
			if((++a)>2) break;
		}else a=0;
		j = k;
	}
	start_strip_point	= i;
printf0("   StartStripPoint:%d", start_strip_point); 

	if(DOUBLE_FLAG==1){
		blkRED = spanRED[srtGIULGI_1];
		blkGRN = spanGRN[srtGIULGI_1];
		blkBLU = spanBLU[srtGIULGI_1];
	}else{
		blkRED = spanRED[srtGIULGI];
		blkGRN = spanGRN[srtGIULGI];
		blkBLU = spanBLU[srtGIULGI];
	}

	whtRED = spanRED[30];
	whtGRN = spanGRN[30];
	whtBLU = spanBLU[30];

	if((whtRED-blkRED)<2000) ERROR_NO=1;
	if(start_strip_point>220) ERROR_NO=2;

//printf0("\t R:%5d-%5d", blkRED, whtRED); 
//printf0(",  G:%5d-%5d", blkGRN, whtGRN); 
//printf0(",  B:%5d-%5d", blkBLU, whtBLU); 
}
/*******************************************************************************/
void get_pad_data(void){
u16 i, j;
printf0("\n *get pad data_single"); 		//key_code=0; while(!key_code);
	for(i=0;i<MAX_PAD_NO;i++){			//data_RGB = 11
		j = start_strip_point + StripPosition[i+1];
		dataRED1[i] = spanRED[j];
		dataGRN1[i] = spanGRN[j];
		dataBLU1[i] = spanBLU[j];
	}
}
/*******************************************************************************/
/*******************************************************************************/
void pass_curve(void){
u16 i, k, n, m;
u32 j;
printf0("\n *pass curve"); 			//key_code=0; while(!key_code);
	for(i=0;i<MAX_PAD_NO;i++){
		for(k=1;k<14;k++) if(dataRED1[i]<SYS_CURVE_RED[k]) break;
		if(k>=14) n = 1300;
		else{
			j = dataRED1[i] - SYS_CURVE_RED[k-1];
			m = SYS_CURVE_RED[k] - SYS_CURVE_RED[k-1];
			n = ((k-1)*100) + ((j*100) / m);
		}
		CURVE_RED[i] = n;
		for(k=1;k<14;k++) if(dataGRN1[i]<SYS_CURVE_GRN[k]) break;
		if(k>=14) n = 1300;
		else{
			j = dataGRN1[i] - SYS_CURVE_GRN[k-1];
			m = SYS_CURVE_GRN[k] - SYS_CURVE_GRN[k-1];
			n = ((k-1)*100) + ((j*100) / m);
		}
		CURVE_GRN[i] = n;
		for(k=1;k<14;k++) if(dataBLU1[i]<SYS_CURVE_BLU[k]) break;
		if(k>=14) n = 1300;
		else{
			j = dataBLU1[i] - SYS_CURVE_BLU[k-1];
			m = SYS_CURVE_BLU[k] - SYS_CURVE_BLU[k-1];
			n = ((k-1)*100) + ((j*100) / m);
		}
		CURVE_BLU[i] = n;
	}	
}
// ----------------------------------------------------------------------------------------------
// led hitting ratio
//float GaussMember(float nor_in)
float GaussMember(unsigned int temp)
{
	float fuzzyval; //, temp_para=0.0;
	float sig;//, nor_in;
	float nor_in;

	nor_in = temp;
	fuzzyval = 0.0;
	if ( temp <= m_para_mean )
	{
		sig=((float)(m_para_mean - m_para_min)/3.0);
	}
	else 
	{
		sig=((float)(m_para_max - m_para_mean)/3.0);
	}
		
	nor_in -= m_para_mean ;
	nor_in *= nor_in;
	sig *= sig;
	nor_in /= 2.0;
// printf0("\n mi:%4d, ma:%4d, mm:%4d, te:%4d, %x", m_para_min, m_para_mean, m_para_max, temp, sig );
	if(sig==0) 
	{
		fuzzyval = 0.0;
	}
	else
	{
	 	nor_in /= sig;
		nor_in *= -1.0;
		fuzzyval=exp(nor_in);
	}
	return fuzzyval;
}

/******************************************************************************/
void calc_ratio1(void){
float prev_fuzzy, deg_fuzzy;
u8  degree, i;
u8  item, result, result_f;
u16 temp;
	for(i=0; i<MAX_PAD_NO; i++ ) m_final_results[i] = 0;

	for(i=0; i < STRIP_INFO[0] ; i++ ){
		item=STRIP_INFO[i+1]-1;
		deg_fuzzy=0.0;
		result = 0;
		for(degree=0 ; degree < maxitem[item] ; degree++)		//0~6
		{
			prev_fuzzy=3.0;
			m_para_min =PARA_NEW[item][degree][0];
			m_para_mean=PARA_NEW[item][degree][1];
			m_para_max =PARA_NEW[item][degree][2];
			temp = CURVE_RED[i];
			prev_fuzzy = GaussMember(temp);
			
			if(prev_fuzzy == 3.0) prev_fuzzy = 0;
			if(deg_fuzzy<prev_fuzzy){
				deg_fuzzy = prev_fuzzy;
				result_f = (unsigned char) degree;
			}
		}  
		if (deg_fuzzy!=0) result=result_f;
		else result=7;
		m_final_results[i] = result;
	}	
printf0("\n Result: ");
for(item=0;item<STRIP_INFO[0];item++) printf0(" %d", m_final_results[item]);
}
/******************************************************************************/
void calc_ratio2(void){
u8  degree;
u8  item;
u16 temp;
u8  i;
	for(i=0; i<MAX_PAD_NO; i++ ) m_final_results[i] = 0;
	for(i=0; i < STRIP_INFO[0]; i++ ){
		item=STRIP_INFO[i+1]-1;
		for(degree=0 ; degree < 7 ; degree++)		//0~6
		{
			m_para_min =PARA_NEW[item][degree][0];
			m_para_max =PARA_NEW[item][degree][2];
			if(weight[item]==1){					//was 1
				temp = CURVE_RED[i];
			}else if(weight[item]==2){				//was 2
				temp = CURVE_GRN[i];
			}else{
				temp = CURVE_BLU[i];
			}
			if((temp<=m_para_max)&&(temp>=m_para_min)) break;
		}  
		m_final_results[i] = degree;		//result;
	}	
	printf0("\n Result2: ");
	for(item=0;item<STRIP_INFO[0];item++){
		printf0("%2x,", m_final_results[item]);
	}
}
/******************************************************************************/
#define MEV_DLY	1		//334;
void run_measure(void){
int gt, gb, ge, a;
	printf0("\n Start measure");
	SENS_ON();
//	LOAD_NO=0;
	LOAD_NO=DOUBLE_FLAG;
	ERROR_NO=0;
	MEASyear = year;
	MEASmonth = month;
	MEASday = day;
	MEAShour = hour;
	MEASmin = min;
	MEASsec = sec;
	for(a=0;a<MAX_USERID;a++) tmp_userid[a] = USER_ID[a];
	tmp_userid[a]=0x00;

	send_meas_step('1');
	a=0;
	meas_time=0;
	cnt100=0;
	gb=MSBXS;
	ge = MSBXE - MSBXS;		//234-36=198
	gt = ge*MEV_DLY;		//334;
	delay_set(gt);
	if(DOUBLE_FLAG==1){
		set_CCW_TICK(MAX_TICK+100);					//후진(복귀)한다.
	}else{
		set_CCW_TICK(MAX_TICK_1+100);					//후진(복귀)한다.
	}
	while(1){
		if(!t0dly){
			delay_set(gt);
			if(ge){
				draw_box(gb,MSBYS,gb+1,MSBYE,MSBBC,RASTNO);
				ge--;
			}
			gb++;										//	if(gb>=MSBXE) break;
		}else if(tm_10ms){
			if((++cnt100)>=100){ dsp_measure_time(++meas_time); cnt100=0; }
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
		if((!ge)&&(!SmDir))	break; 
	}		
	stepMotorOff(); 
	printf0("\n Stop Motor1");
//	delay_ms(10);
	delay_ms(100);
	if(SPEED_FLAG==SPEED_LO){
		if(wait_time<=26) wait_meas(1);		//??????wait_meas(45);
		else wait_meas(wait_time-26);		//??????wait_meas(45);
//		wait_meas(60-26);		
	}//else xLine(11,40,4,106,0);					//BOX안을 지운다.

	send_meas_step('2');
	a=0;
	cnt100=10;
	gb=MSBXS;
	ge = MSBXE - MSBXS;
	gt = (ge*MEV_DLY);
	delay_set(gt);
	if(DOUBLE_FLAG==1){
		set_CW_TICK(MAX_TICK);					//전진한다.
	}else{
		set_CW_TICK(MAX_TICK_1);					//전진한다.
	}
	div_adc=0;	pnt_adc=0;
	RGB_GRN_ON();
	div_adc=0;
	flag_adc=1;								//측정을 시작한다.
	while(1){		
		if(!t0dly){
			delay_set(gt);
			if(ge){
				draw_box(gb,MSBYS,gb+1,MSBYE,MSBEC,RASTNO);
				ge--;
			}
			gb++;										//	if(gb>=MSBXE) break;
		}else if(tm_10ms){
			if((++cnt100)>=100){ dsp_measure_time(++meas_time); cnt100=0; }
			tm_10ms=0;
		}
		if((!ge)&&(!SmDir))	break; 
	}
	flag_adc=0;
	stepMotorOff(); 
	printf0("\n Stop Motor2");
	RGB_ALL_OFF();
	xchg_rgb();
	send_meas_step('3');

	if(state==M_MEASURE){
//		if(!MMU_ENABLE)	wait_analysis(3);			//3초간 분석그림.
//		else wait_analysis(1);						//1초간 분석그림.
		wait_analysis(3);			//3초간 분석그림.
	}
	send_meas_step('4');
	span_curve();
//printf0("\n *pass span"); wait_tx0();	//key_code=0; while(!key_code);
	get_start_strip_point();
//printf0("\n *pass point"); wait_tx0();	//key_code=0; while(!key_code);
	run_adjust();
//printf0("\n *pass adjust"); wait_tx0();	//key_code=0; while(!key_code);
	get_pad_data();
//printf0("\n *pass pad data"); wait_tx0();	//key_code=0; while(!key_code);
	pass_curve();
//printf0("\n *calc ratio"); wait_tx0();	//key_code=0; while(!key_code);
	calc_ratio2();
	
//printf0("\n *pass ratio"); wait_tx0();	//key_code=0; while(!key_code);
	conv_result();
//printf0("\n *pass result"); wait_tx0();	//key_code=0; while(!key_code);

	send_meas_step('5');
	cursor=0;
	tmp_seqno=SEQ_NO;
	delay_ms(100);

	store_COLOR_NO=COLOR_NO;		// for print
	store_TURBI_NO=TURBI_NO;		// for print

	dsp_result(SEQ_NO);
//printf0("\n *pass dsp"); wait_tx0();	//key_code=0; while(!key_code);

	store_result(SEQ_NO, 1);	//Store Result
//	if(DATA_FLAG==DATA_AUTO) send_result();
//printf0("\n *pass store"); wait_tx0();	//key_code=0; while(!key_code);
	send_result();
//printf0("\n *pass send"); wait_tx0();	//key_code=0; while(!key_code);
//	if(PRINT_FLAG==PRINT_ENABLE) print_result(SEQ_NO);
	print_result(SEQ_NO);
	SEQ_NO++; if(SEQ_NO>MAX_RECORD) SEQ_NO=1;
	write24w(SEQNO_ADR, SEQ_NO);
	write_id_chksum();
//printf0("\n *pass write"); wait_tx0();	//key_code=0; while(!key_code);
	clr_user_id(); 
	key_clear();
	com_clear();
	SENS_OFF();
 	if(SOUND_FLAG) key_buzzer();
 	if(BALANCE_AUTO&&(flag_balance!=OK)) set_balance(100);//2015-06-08 오후 4:33:19
}

/*******************************************************************************/
void xchg_rgb(void){
u16 i, j, k;	
	printf0("\n pnt_adc:%d", pnt_adc);
	if(pnt_adc>MaxRawData) pnt_adc=MaxRawData;
	pnt_adc = (pnt_adc/4) * 4;
	printf0("\n pnt_adc:%d", pnt_adc);
//	for(i=0;i<pnt_adc;i++) printf0("%4x,",buffRED1[i]);
	for(i=0;i<(pnt_adc/2);i++){
		j = buffRED1[i]; k = buffRED1[pnt_adc-(i+1)];
		buffRED1[i] = k; buffRED1[pnt_adc-(i+1)] = j;
		j = buffGRN1[i]; k = buffGRN1[pnt_adc-(i+1)];
		buffGRN1[i] = k; buffGRN1[pnt_adc-(i+1)] = j;
		j = buffBLU1[i]; k = buffBLU1[pnt_adc-(i+1)];
		buffBLU1[i] = k; buffBLU1[pnt_adc-(i+1)] = j;
	}
}
/*******************************************************************************/
void send_result(void){
u8 i;
	//StoreResult시에 cmdbuff에 data를 정리해 두었다.
	printf0("\n Send Result\n");
#ifdef TEST_RESULT
	for(i=0;i<RESULT_SIZE;i++){ 
		if((i%50)==0) printf0("\n");
		savcmd0(cmdbuff[i]); wait_tx0();
	}
#else
	for(i=0;i<RESULT_SIZE;i++){ savcmd3(cmdbuff[i]); wait_tx3(); }
#endif
}	

//시간이 될때 까지 기다린다.
void wait_meas(u16 wsec){
int gt, gb, ge, a;
	printf0("\n Waiting %3d[sec]", wsec);
	a = wsec*100;
	gb=MSBXS;
	ge = MSBXE - MSBXS;
	gt = a / (MSBXE - MSBXS);		
	while(1){
		if(tm_10ms){
			tm_10ms=0;
			if(!gt){
				if(ge){	draw_box(gb,MSBYS,gb+1,MSBYE,CYN,RASTNO); ge--;	}
				gb++;										//	if(gb>=MSBXE) break;
				gt = a / (MSBXE - MSBXS);		
			}else gt--;
			if((++cnt100)>=100){ dsp_measure_time(++meas_time); cnt100=0; }
		}
		if(!ge) break;
	}		
}

/*******************************************************************************/
void wait_analysis(u16 sec){
int gt, gb, ge;
//	erase_box(140,110,140+64,110+32);
//	if(isKorean) msg32(140,110,BLU,"분석중입니다.");	
//	else msg32(140,110,BLU,"Analyzing...");	
	cnt100=10;
	gb=MSBXS;
	ge = MSBXE - MSBXS;
//	gt = (ge*50);
	gt = (ge/10);
	delay_set(gt);
	while(ge){		
		if(!t0dly){
			delay_set(gt);
			if(ge){
				draw_box(gb,MSBYS,gb+1,MSBYE,MSBCC,RASTNO);
				ge--;
			}
			gb++;										//	if(gb>=MSBXE) break;
		}else if(tm_10ms){
			if((++cnt100)>=100){ dsp_measure_time(++meas_time); cnt100=0; }
			tm_10ms=0;
		}
	}
	stepMotorOff(); 
	printf0("\n Stop Motor3");
	RGB_ALL_OFF();
}
/*******************************************************************************/
u16 put_albumin(u16 i, u8 x){
u8 j;	
printf0("\n Put abnormal:%d", x);
	for(j=0;j<32;j++) buffRESULT[i++]=albumin_tb[x][j];
	buffRESULT[i++]=' ';
	buffRESULT[i++]=0x0d;
	buffRESULT[i++]=0x0a;
	return(i);
}

/*******************************************************************************/

void conv_result(void){
u16 i, j, item;	
//u8 *ip, x, y;
u8 x, y;
		
	printf0("\n Final Result(conv result)"); wait_tx0();
	for(item=0;item<12;item++) printf0("  %d", STRIP_INFO[item]); wait_tx0();
	i=0;
	datPRO=datALB=datCRE=datCAL=datMDA=datLDH= -1;
	for(item=0; item < STRIP_INFO[0] ; item++ ){
		x = STRIP_INFO[item+1];
		y = m_final_results[item];
		buffRESULT[i++]=' ';
		if(normal_range[x][y]==0){
			for(j=0;j<8;j++) buffRESULT[i++] = item_string[STRIP_INFO[item+1]][j];
		}else{
			for(j=0;j<8;j++) buffRESULT[i++] = item_string1[STRIP_INFO[item+1]][j];
		}			
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		x = STRIP_INFO[item+1]; y=m_final_results[item]+1;
		i = put_arb(i, x, y);
		buffRESULT[i++]=' ';
		i = put_result1(i, x, y);
		if(x==pntPRO) datPRO = y;
		else if(x==pntALB) datALB = y;
		else if(x==pntCRE) datCRE = y;
		else if(x==pntCAL) datCAL = y;
		else if(x==pntMDA) datMDA = y;
		else if(x==pntLDH) datLDH = y;
		else if(x==pntBHB) datBHB = y;

		if(((m_final_results[item]>0)&&(m_final_results[item]<7))
							||(x==pntALB)||(x==pntCRE)||(x==pntCAL)||(x==pntMDA)){
			x = STRIP_INFO[item+1];
			i = put_result1(i, x, 0);
		}else{
			for(j=0;j<8;j++) buffRESULT[i++]=' ';
		}
		for(j=0;j<3;j++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
	}	
	//printf0("\n C:%d, A:%d, P:%d", datCRE, datALB, datPRO );
	//printf0(" L:%d", datCAL ); wait_tx0();
	if((datCRE>=0)&&(datALB>=0)){
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		for(x=0;x<5;x++) buffRESULT[i++]=text_AC[x];
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		x = alb_item[datCRE-1][datALB-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
//		for(j=0;j<8;j++) buffRESULT[i++]=' ';
//		i = put_arb( i, ACpnt, y/8 );
		i = put_result1( i, ACpnt, y/8 );
		buffRESULT[i++]=' ';
		if(UNIT_FLAG==UNIT_SI){
			for(j=0;j<8;j++) buffRESULT[i++]=alb_tb_text_1[datCRE-1][datALB-1][j];
			for(j=0;j<8;j++) buffRESULT[i++]=alb_tb_text_1[datCRE-1][4][j];
		}else{
			for(j=0;j<8;j++) buffRESULT[i++]=alb_tb_text[datCRE-1][datALB-1][j];
			for(j=0;j<8;j++) buffRESULT[i++]=alb_tb_text[datCRE-1][4][j];
		}
/*		i = put_result1( i, ACpnt, y/8 );
		if(x) i = put_result1( i, ACpnt, 0 );*/
//		if(x) i = put_result1( i, ACpnt, 0 );
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(y=0;y<8;y++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
	}else if((datCRE>=0)&&(datPRO>=0)){
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		for(x=0;x<5;x++) buffRESULT[i++]=text_PC[x];
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		x = pro_item[datPRO-1][datCRE-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, PCpnt, y/8 );
		buffRESULT[i++]=' ';
		i = put_result1( i, PCpnt, y/8 );
		if(x) i = put_result1( i, PCpnt, 0 );
		for(y=0;y<8;y++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
//		for(y=0;y<8;y++) buffRESULT[i++]=' ';
//		for(y=0;y<3;y++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
//		for(j=0;j<13;j++) buffRESULT[i++]=albumin_tb_1[x][j];
//		for(y=0;y<8;y++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
	}
	if((datCRE>=0)&&(datCAL>=0)){
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		for(x=0;x<5;x++) buffRESULT[i++]=text_CC[x];
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		x = alb_item[datCRE-1][datCAL-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, CCpnt, y/8 );
		buffRESULT[i++]=' ';
		i = put_result1( i, CCpnt, y/8 );
		if(x) i = put_result1( i, CCpnt, 0 );
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(y=0;y<8;y++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
	}
//판정은 어떻게하나?	
	END_RESULT = i;
	buffRESULT[i++]=0;
	printf0("\n buffRESULT\n");
	for(i=0;i<END_RESULT;i++) savcmd0(buffRESULT[i]);


}

/*void conv_result(void){
u16 i, j, item;	
//u8 *ip, x, y;
u8 x, y;
	printf0("\n Final Result(conv result)"); wait_tx0();
	for(item=0;item<12;item++) printf0("  %d", STRIP_INFO[item]); wait_tx0();
	i=0;
	datPRO=datALB=datCRE=datCAL= -1;
	for(item=0; item < STRIP_INFO[0] ; item++ ){
		x = STRIP_INFO[item+1];
		y = m_final_results[item];
//		if(normal_range[x][y]==0) ip = (u8 *)item_string[STRIP_INFO[item+1]];
//		else ip = (u8 *)item_string1[STRIP_INFO[item+1]];
		buffRESULT[i++]=' ';
//eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
//eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
//eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
//		for(j=0;j<8;j++) buffRESULT[i++]=*ip++;
		if(normal_range[x][y]==0){
			for(j=0;j<8;j++) buffRESULT[i++] = item_string[STRIP_INFO[item+1]][j];
		}else{
			for(j=0;j<8;j++) buffRESULT[i++] = item_string1[STRIP_INFO[item+1]][j];
		}			
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		x = STRIP_INFO[item+1]; y=m_final_results[item]+1;
		i = put_arb(i, x, y);
		i = put_result1(i, x, y);
		if(x==pntPRO) datPRO = y;
		else if(x==pntALB) datALB = y;
		else if(x==pntCRE) datCRE = y;
		else if(x==pntCAL) datCAL = y;

		if(((m_final_results[item]>0)&&(m_final_results[item]<7))
							||(x==pntALB)||(x==pntCRE)||(x==pntCAL)){
			x = STRIP_INFO[item+1];
			i = put_result1(i, x, 0);
		}else{
			for(j=0;j<8;j++) buffRESULT[i++]=' ';
		}
		for(j=0;j<3;j++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
	}	
printf0("\n C:%d, A:%d, P:%d", datCRE, datALB, datPRO );
printf0(" L:%d", datCAL ); wait_tx0();
	if((datCRE>=0)&&(datALB>=0)){
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		for(x=0;x<5;x++) buffRESULT[i++]=text_AC[x];
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		x = alb_item[datCRE-1][datALB-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, ACpnt, y/8 );
		i = put_result1( i, ACpnt, y/8 );
		if(x) i = put_result1( i, 14, 0 );
//		if(x) i = put_result1( i, ACpnt, 0 );
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(y=0;y<8;y++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
	}else if((datCRE>=0)&&(datPRO>=0)){
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		for(x=0;x<5;x++) buffRESULT[i++]=text_PC[x];
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		x = pro_item[datPRO-1][datCRE-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, PCpnt, y/8 );
		i = put_result1( i, PCpnt, y/8 );
		for(y=0;y<8;y++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(y=0;y<8;y++) buffRESULT[i++]=' ';
		i = put_pro(i, x);		
	}
	if((datCRE>=0)&&(datCAL>=0)){
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		for(x=0;x<5;x++) buffRESULT[i++]=text_CC[x];
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		x = alb_item[datCRE-1][datCAL-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, CCpnt, y/8 );
		i = put_result1( i, CCpnt, y/8 );
		if(x) i = put_result1( i, 14, 0 );
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(y=0;y<8;y++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
	}
//판정은 어떻게하나?	
	END_RESULT = i;
	buffRESULT[i++]=0;
	printf0("\n buffRESULT\n");
	for(i=0;i<END_RESULT;i++) savcmd0(buffRESULT[i]);
}*/

/*******************************************************************************/
u16 put_result1(u16 i, u8 x, u8 y){
u16 j;
	if(UNIT_FLAG==UNIT_SI){
		for(j=0;j<8;j++) buffRESULT[i++]=si_item_ram[x][y][j];
	}else if(UNIT_FLAG==UNIT_CONV){
		for(j=0;j<8;j++) buffRESULT[i++]=co_item_ram[x][y][j];
	}else{
		for(j=0;j<8;j++) buffRESULT[i++]=ar_item_ram[x][y][j];
	}
	return(i);
}
/*******************************************************************************/
u16  put_arb(u16 i, u8 x, u8 y){
u16 j;
	for(j=0;j<8;j++) buffRESULT[i++]=ar_item_ram[x][y][j];
	return(i);
}
/*******************************************************************************/
void print_result(u16 meas_no){
u32 w1;
u8  b1;
	print_sound();
	print_thermal("\n");					//Line0
//	strcpy(char_buff,PrintLine1);
	strcpy(char_buff,"= = = = SEQ# : 0000  = = = = = =");
	w1 = bin2bcdasc(meas_no);				//w1 = bin2bcdasc(SEQ_NO);
	char_buff[15] = w1>>24;
	char_buff[16] = w1>>16;
	char_buff[17] = w1>>8;
	char_buff[18] = w1>>0;
	print_thermal("\n%s", char_buff);
	
//	strcpy(char_buff,PrintLine2);
	strcpy(char_buff," 2000/01/01          AM 00:00:00");
	w1 = bin2bcdasc(MEASyear);
	char_buff[3] = w1>>8;
	char_buff[4] = w1>>0;
	w1 = bin2bcdasc(MEASmonth);
	char_buff[6] = w1>>8;
	char_buff[7] = w1>>0;
	w1 = bin2bcdasc(MEASday);
	char_buff[9] = w1>>8;
	char_buff[10] = w1>>0;
	if(!MEAShour) b1=12;
	else if(MEAShour>=12){
		char_buff[21]='P'; 
		if(MEAShour>12)	b1 = MEAShour - 12;
	}else b1 = MEAShour;
	w1 = bin2bcdasc(b1);
	char_buff[24] = w1>>8;
	char_buff[25] = w1>>0;
	w1 = bin2bcdasc(MEASmin);
	char_buff[27] = w1>>8;
	char_buff[28] = w1>>0;
	w1 = bin2bcdasc(MEASsec);
	char_buff[30] = w1>>8;
	char_buff[31] = w1>>0;
	print_thermal("\n%s", char_buff);	
//	strcpy(char_buff, PrintLine3);
	strcpy(char_buff, "                ");
	for(w1=0;w1<11;w1++) char_buff[w1+1] = strip_name[w1];
	print_thermal("\n%s", char_buff);	
//	strcpy(char_buff, PrintLine4);
	strcpy(char_buff, " ID:            ");
	for(w1=0;w1<MAX_USERID;w1++) char_buff[w1+5] = tmp_userid[w1];
	char_buff[w1+5] = 0;
	print_thermal("\n%s", char_buff);	
	print_thermal("\n");					//Line5
//	print_thermal("\n%s", PrintLine6);	
	print_thermal("\n%s", "= = = = = = = = = = = = = = = = ");	
	print_thermal("\n");					//Start Data
	if(LOAD_NO==1){
		if(END_RESULT/43>5){
			print_thermal("\n%s\n", "    Wrong Strip Type   ");	
		}else{
			if(ERROR_NO==0){
				b1=0;
				for(w1=0;w1<END_RESULT;w1++){
					if(pt_size[b1]==1) savcmd2(buffRESULT[w1]);
					if((++b1)>=43) b1=0;
				}
			}else if(ERROR_NO==1){
				print_thermal("\n%s\n", "     Loader Error   ");	
			}else{
				print_thermal("\n%s\n", "       No Strip     ");	
			}
		}
	}else{
		if(ERROR_NO==0){
				b1=0;
				for(w1=0;w1<END_RESULT;w1++){
					if(pt_size[b1]==1) savcmd2(buffRESULT[w1]);
					if((++b1)>=43) b1=0;
				}
		}else if(ERROR_NO==1){
			print_thermal("\n%s\n", "     Loader Error   ");	
		}else{
			print_thermal("\n%s\n", "       No Strip     ");	
		}
	}
	
/*
#ifdef TEST_PRINT
//	for(w1=0;w1<END_RESULT;w1++) savcmd0(buffRESULT[w1]);
	b1=0;
	for(w1=0;w1<END_RESULT;w1++){
		if(pt_size[b1]==1) savcmd0(buffRESULT[w1]);
		if((++b1)>=42) b1=0;
	}
#else
	b1=0;
	for(w1=0;w1<END_RESULT;w1++){
		if(pt_size[b1]==1) savcmd2(buffRESULT[w1]);
		if((++b1)>=42) b1=0;
	}
#endif

*/
//	print_thermal("%s", PrintLine6);	
	print_thermal("%s", "= = = = = = = = = = = = = = = = ");	
	print_thermal("\n Color : ");
	switch(store_COLOR_NO){
		case 0:	print_thermal("NONE");		break; //1	
		case 1: print_thermal("YELLOW");		break; //2	
		case 2:	print_thermal("DK YELLOW");	break; //3	
		case 3:	print_thermal("STRAW");		break; //4	
		case 4:	print_thermal("AMBER");		break; //5 
		case 5:	print_thermal("RED");			break; //6 
		case 6:	print_thermal("ORANGE");		break; //7 
		case 7:	print_thermal("GREEN");		break; //8 
		default: print_thermal("OTHER");		break; //9 
	}
	
	print_thermal("\n Turb. : ");
	switch(store_TURBI_NO){
		case 0:	print_thermal("NONE");		break; //1	
		case 1:	print_thermal("CLEAR");		break; //2	
		case 2:	print_thermal("SL CLOUDY");	break; //3	
		case 3:	print_thermal("CLOUDY");		break; //4
		case 4:	print_thermal("TURBID");		break; //5 
		default: print_thermal("OTHER");		break; //9 
	}
	
	print_thermal("\n S/N: %s",SYS_ID);		//??????????????
	
//	print_thermal("\n%s", PrintLine6);	
	print_thermal("\n%s", "= = = = = = = = = = = = = = = = ");	
	print_thermal("\n\n\n\n\n");			//Line19
}

/*******************************************************************************/
/*********  현재 사용하지 않음  *****************/
int print_result_store(u16 ssno, u16 eeno){
int i, j, k;
u16 w1, w2;
	if((!ssno)||(ssno>MAX_RECORD)) return(NG);
	if((!eeno)||(eeno>MAX_RECORD)) return(NG);
	if(ssno>eeno) return(NG);
	for(i=(ssno-1);i<eeno;i++){
		move_result(i);
		j = (i*RESULT_PAGE) & 0x0FFF;
		w1=0;
		for(k=0;k<RESULT_SIZE;k++) w1 += flash_read[j++];
		w2 = flash_read[j+1];
		w2 = (w2<<8) + flash_read[j];
		w2=~w2;
		if(w1==w2){
			printf0("\n *Success(PrintResult) No:%4d,Sum:%4x\n", i+1,w1);		
			j = (i*RESULT_PAGE) & 0x0FFF;
			store_COLOR_NO=flash_read[j+24]-'0';
			store_TURBI_NO=flash_read[j+25]-'0';
			k=0x303000+flash_read[j+18+ADD10B]; k=(k<<8)+flash_read[j+19+ADD10B]; MEASyear  = bcdasc2bin(k);
			k=0x303000+flash_read[j+20+ADD10B]; k=(k<<8)+flash_read[j+21+ADD10B]; MEASmonth = bcdasc2bin(k);
			k=0x303000+flash_read[j+22+ADD10B]; k=(k<<8)+flash_read[j+23+ADD10B]; MEASday   = bcdasc2bin(k);
			k=0x303000+flash_read[j+24+ADD10B]; k=(k<<8)+flash_read[j+25+ADD10B]; MEAShour  = bcdasc2bin(k);
			k=0x303000+flash_read[j+26+ADD10B]; k=(k<<8)+flash_read[j+27+ADD10B]; MEASmin   = bcdasc2bin(k);
			k=0x303000+flash_read[j+28+ADD10B]; k=(k<<8)+flash_read[j+29+ADD10B]; MEASsec   = bcdasc2bin(k);
//			for(k=0;k<10;k++) tmp_userid[k] = flash_read[j+6+k+ADD10B];
			for(k=0;k<MAX_USERID;k++) tmp_userid[k] = flash_read[j+6+k+ADD10B];
			tmp_userid[k]=0x00;
			for(k=0;k<10;k++) strip_name[k] = flash_read[j+32+k+ADD10B];	strip_name[k] = 0;
			for(k=0;k<12;k++) STRIP_INFO[k] = hexasc2bin_1(flash_read[j+42+k+ADD10B]);
			if(STRIP_INFO[0]>11) STRIP_INFO[0] = 11;
			for(k=0;k<11;k++){ m_final_results[k]=hexasc2bin_1(flash_read[j+54+k+ADD10B]); }
			conv_result();
			print_result(i+1);
		}else{
			printf0("\n *Error!(PrintResult) No:%4d,Sum:%4x,SAV:%4x", i+1, w1, w2);		
		}
	}
	return(OK);
}

/*******************************************************************************/
void print_sound(void){
u16 i;
	for(i=0;i<20;i++){	
		key_buzzer();
		delay_ms(30);
		key_buzzer();
		delay_ms(20);
	}
	delay_ms(120);
	BUZZ_LO();	
}

/*******************************************************************************/
//=============================================================
//no = 0 ~ MAX_RECORD-1
short view_result_store(u16 view_seqno){
u16 w1, w2, j, k;
	printf0("\n ResultView:%d", view_seqno);
	if((!view_seqno)||(view_seqno>MAX_RECORD)){
		dsp_novalid();
		printf0("\n Error ==> view_seqno");
		return(NG);
	}else{
		move_result(view_seqno-1);
		j = (view_seqno-1)*RESULT_PAGE; j &= 0x0FFF;
		for(k=0;k<RESULT_PAGE;k++) cmdbuff[k] = flash_read[j++];
//printf0("\n"); for(k=0;k<200;k++){ savcmd2(cmdbuff[k]); wait_tx2(); }
		w1=0;
		for(k=0;k<RESULT_SIZE;k++) w1 += cmdbuff[k];
//	for(k=0;k<RESULT_SIZE;k++) printf0("%2x", cmdbuff[k]);
		w1 = ~w1;
		w2 = cmdbuff[RESULT_SIZE+1];
		w2 = (w2<<8) + cmdbuff[RESULT_SIZE];
		if(w1==w2){
			make_result();
			conv_result();
			dsp_result_store(view_seqno);
			return(OK);
		}else{	
printf0("\n SUM:%4x,SAV:%4x", w1, w2);		wait_tx0();
			dsp_novalid();
			return(NG);
		}
	}
}
//=============================================================
void make_result(void){
u16 k;
	for(k=0;k<MAX_USERID;k++) tmp_userid[k] = cmdbuff[6+k];							//6~15
	tmp_userid[k]=0x00;
	LOAD_NO=cmdbuff[22]-'0';
	ERROR_NO=cmdbuff[23]-'0';
	store_COLOR_NO=cmdbuff[24]-'0';
	store_TURBI_NO=cmdbuff[25]-'0';
	k=cmdbuff[18+ADD10B];	k=(k<<8)+cmdbuff[19+ADD10B];	MEASyear  = bcdasc2bin4(k);
	k=cmdbuff[20+ADD10B];	k=(k<<8)+cmdbuff[21+ADD10B];	MEASmonth = bcdasc2bin4(k);
	k=cmdbuff[22+ADD10B];	k=(k<<8)+cmdbuff[23+ADD10B];	MEASday   = bcdasc2bin4(k);
	k=cmdbuff[24+ADD10B];	k=(k<<8)+cmdbuff[25+ADD10B];	MEAShour  = bcdasc2bin4(k);
	k=cmdbuff[26+ADD10B];	k=(k<<8)+cmdbuff[27+ADD10B];	MEASmin   = bcdasc2bin4(k);
	k=cmdbuff[28+ADD10B];	k=(k<<8)+cmdbuff[29+ADD10B];	MEASsec   = bcdasc2bin4(k);
	for(k=0;k<10;k++) strip_name[k] = cmdbuff[32+ADD10B+k]; strip_name[k] = 0;		//32~41
	for(k=0;k<12;k++) STRIP_INFO[k] = hexasc2bin_1(cmdbuff[42+k+ADD10B]);
	if(STRIP_INFO[0]>MAX_PAD_NO) STRIP_INFO[0] = MAX_PAD_NO;
	for(k=0;k<MAX_PAD_NO;k++) m_final_results[k]=hexasc2bin_1(cmdbuff[54+k+ADD10B]);
}
//=============================================================
//no = 0 ~ MAX_RECORD-1
void send_result_one(u16 sdno){
u16 w1, w2, j, k;
	printf0("\n Send data:%d", sdno);
	if((!sdno)||(sdno>MAX_RECORD)){
		dsp_novalid();
		printf0("\t Error ==> send_seqno");
		blink_send();
	}else{
		move_result(sdno-1);
		j = (sdno-1)*RESULT_PAGE; j &= 0x0FFF;
		for(k=0;k<RESULT_PAGE;k++) cmdbuff[k] = flash_read[j++];
		w1=0;
		for(k=0;k<RESULT_SIZE;k++) w1 += cmdbuff[k];
		w1 = ~w1;
		w2 = cmdbuff[k+1];
		w2 = (w2<<8) + cmdbuff[k+0];
		if(w1==w2){
#ifdef TEST_RESULT
			for(k=0;k<RESULT_SIZE;k++) savcmd0(cmdbuff[k]); //wait_tx0();
#else
			for(k=0;k<RESULT_SIZE;k++) savcmd3(cmdbuff[k]);
#endif
			printf0("\t Send result one");
			blink_send();
			key_buzzer();
			blink_send();
			key_buzzer();
			blink_send();
		}else{	
			printf0("\t Error!!! SUM:%4x,SAV:%4x", w1, w2);
			blink_send();
			key_buzzer();
			blink_send();
		}
		delay_ms(500);
	}
}
//=============================================================
//no = 0 ~ MAX_RECORD-1
void delete_result_one(u16 sdno){
u16 j, k;
u32 radr;
	printf0("\n delete data:%d", sdno);
	if((!sdno)||(sdno>MAX_RECORD)){
		dsp_novalid();
		printf0("\t Error ==> seq.no");
		blink_delete(300);
	}else{
		move_result(sdno-1);
		j = (sdno-1)*RESULT_PAGE; j &= 0x0FFF;
		for(k=0;k<RESULT_SIZE;k++) flash_read[j++]=0xff;
		radr = sdno-1;
		radr = (radr*RESULT_PAGE) + RESULT_ADR;
		system_wr4k(radr);			//	system_store('R');	==> Delete
		printf0("\t delete result one");
		blink_delete(300);
		key_buzzer();
		blink_delete(300);
		key_buzzer();
		blink_delete(0);
	}
}
//=============================================================
void delete_result_store(u16 ssno, u16 eeno){
u32 j, k, cnt;
u32 radr;
	if((!ssno)||(ssno>MAX_RECORD)) return;
	if((!eeno)||(eeno>MAX_RECORD)) return;
	if(ssno<eeno){
		cnt = eeno-ssno;
	}else if(ssno>eeno){
		cnt = (eeno+MAX_RECORD)-ssno;
	}else cnt=0;
	cnt += 1;
		
	do{
		erase_box( 139, 428, 139+120, 428+40);
		move_result(ssno-1);
		j = (ssno-1)*RESULT_PAGE;	j &= 0x0FFF;
		for(k=0;k<RESULT_SIZE;k++) flash_read[j++]=0xff;
		radr = ssno-1;
		radr = (radr*RESULT_PAGE) + RESULT_ADR;
		system_wr4k(radr);			//	system_store('R');	==> Delete

		cnt--;
		delay_ms(100);
		LCDbmpxx(b_del3, 139,428);					
		delay_ms(150);
		j = put_seqno(ssno);
		msg20(130+j,92,BLK,top_str);	
		ssno++;
		if(ssno>MAX_RECORD) ssno=1;
	}while(cnt);
}
//=============================================================
//=============================================================
int send_result_store(u16 ssno, u16 eeno){
int j, k, cnt;
u16 w1, w2;
	if((!ssno)||(ssno>MAX_RECORD)) return(NG);
	if((!eeno)||(eeno>MAX_RECORD)) return(NG);
	if(ssno<eeno){
		cnt = (eeno-ssno)+1;
	}else if(ssno==eeno){
		cnt = 1;
	}else{
//		cnt = (eeno+MAX_RECORD)-ssno;
		cnt = 0;
	}
		
	while(cnt){
		erase_box( 139, 428, 139+120, 428+40);
		move_result(ssno-1);
		j = (ssno-1)*RESULT_PAGE; j&= 0x0FFF;
		for(k=0;k<RESULT_PAGE;k++) cmdbuff[k] = flash_read[j++];
		w1=0;
		for(k=0;k<RESULT_SIZE;k++) w1 += cmdbuff[k];
		w2 = cmdbuff[k+1];
		w2 = (w2<<8) + cmdbuff[k];
		w1=~w1;
		if(w1==w2){
			printf0("\n *Success(SendResult) No:%4d,Sum:%4x\n", ssno,w1);		
#ifdef TEST_RESULT
			for(k=0;k<RESULT_SIZE;k++) savcmd0(cmdbuff[k]); //wait_tx0();
#else
			for(k=0;k<RESULT_SIZE;k++){ savcmd3(cmdbuff[k]);	/*wait_tx3();*/ }
#endif
		}else{
			printf0("\n *Error!(SendResult) No:%4d,Sum:%4x,SAV:%4x", ssno, w1, w2);	//wait_tx0();	
#ifdef TEST_RESULT
			printf0("\n");
			for(k=0;k<RESULT_SIZE;k++) printf0("0"); //wait_tx0();
#else
			for(k=0;k<RESULT_SIZE;k++){ savcmd3('0'); /*wait_tx3();*/ }
#endif
		}
		delay_ms(100);
		LCDbmpxx(b_send3, 139,428);					
		wait_tx0();
		wait_tx3();
		delay_ms(150);
		LCDbmpxx(b_start3, 20,88);					
		j = put_seqno(ssno);
		msg20(130+j,92,BLK,top_str);	
		cnt--;
		ssno++;
//		if(ssno>MAX_RECORD) ssno=1;
	}
	delay_ms(1000);
	return(OK);
}
//=============================================================
//=============================================================
//=============================================================
//=============================================================
//=============================================================
