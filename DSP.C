#define  _DSP_C_
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\Init.h"
#include ".\inc\UART.h"
#include ".\inc\lcd.h"
#include ".\inc\flash.h"
#include ".\inc\RESM.h"
#include ".\inc\touch.h"
#include ".\inc\EEP.h"
#include ".\inc\Meas.h"
#include ".\inc\para.h"
#include ".\inc\dsp.h"
#include ".\inc\sysadj.h"		//20150519


const char mcr_tb_1[7][7][5]= {
//char *mcr_tb[7][7]= {	
	{{" 1.11"},{" 3.33"},{" 5.56"},{"11.11"},{"16.67"},{"22.22"},{"33.33"}},	//0
	{{" 0.23"},{" 0.68"},{" 1.14"},{" 2.27"},{" 3.41"},{" 4.55"},{" 6.82"}},	//0
	{{" 0.11"},{" 0.34"},{" 0.57"},{" 1.14"},{" 1.70"},{" 2.27"},{" 3.41"}},	//0
	{{" 0.08"},{" 0.23"},{" 0.38"},{" 0.76"},{" 1.14"},{" 1.52"},{" 2.27"}},	//0
	{{" 0.06"},{" 0.17"},{" 0.28"},{" 0.56"},{" 0.85"},{" 1.13"},{" 1.69"}},	//0
	{{" 0.05"},{" 0.14"},{" 0.23"},{" 0.45"},{" 0.68"},{" 0.91"},{" 1.36"}},	//0
	{{" 0.04"},{" 0.11"},{" 0.19"},{" 0.38"},{" 0.57"},{" 0.75"},{" 1.13"}}	//0
};
const char mcr_tb[7][7][6]= {	
	{{" 10.00"},{" 30.00"},{" 50.00"},{"100.00"},{"150.00"},{"200.00"},{"300.00"}},	//0
	{{"  2.00"},{"  6.00"},{" 10.00"},{" 20.00"},{" 30.00"},{" 40.00"},{" 60.00"}},	//0
	{{"  1.00"},{"  3.00"},{"  5.00"},{" 10.00"},{" 15.00"},{" 20.00"},{" 30.00"}},	//0
	{{"  0.67"},{"  2.00"},{"  3.33"},{"  6.67"},{" 10.00"},{" 13.33"},{" 20.00"}},	//0
	{{"  0.50"},{"  1.50"},{"  2.50"},{"  5.00"},{"  7.50"},{" 10.00"},{" 15.00"}},	//0
	{{"  0.40"},{"  1.20"},{"  2.00"},{"  4.00"},{"  6.00"},{"  8.00"},{" 12.00"}},	//0
	{{"  0.33"},{"  1.00"},{"  1.67"},{"  3.33"},{"  5.00"},{"  6.67"},{" 10.00"}}	//0
};

const u32 mcr_val[7][7]= {	
	{ 111, 333, 556,1111,1667,2222,3333},	//0
	{  23,  68, 114, 227, 341, 455, 682},	//0
	{  11,  34,  57, 114, 170, 227, 341},	//0
	{   8,  23,  38,  76, 114, 152, 227},	//0
	{   6,  17,  28,  56,  85, 113, 169},	//0
	{   5,  14,  23,  45,  68,  91, 136},	//0
	{   4,  11,  19,  38,  57,  75, 113}	//0
};

const u32 mcr_val_1[7][7]= {
	{ 1000, 3000, 5000,10000,15000,20000,30000},	//0
	{  200,  600, 1000, 2000, 3000, 4000, 6000},	//0
	{  100,  300,  500, 1000, 1500, 2000, 3000},	//0
	{   67,  200,  333,  667, 1000, 1333, 2000},	//0
	{   50,  150,  250,  500,  750, 1000, 1500},	//0
	{   40,  120,  200,  400,  600,  800, 1200},	//0
	{   33,  100,  167,  333,  500,  667, 1000}	//0
};

const char acr_tb_1[5][4][7]= {	
	{{"       "},{"  300.0"},{"  800.0"},{"1,500.0"}},	//0
	{{"   20.0"},{"   60.0"},{"  160.0"},{"  300.0"}},	//0
	{{"   10.0"},{"   30.0"},{"   80.0"},{"  150.0"}},	//0
//	{{"    6.7"},{"   20.0"},{"   53.3"},{"  100.0"}},	//0
	{{"    5.0"},{"   15.0"},{"   40.0"},{"   75.0"}},	//0
//	{{"    4.0"},{"   12.0"},{"   32.0"},{"   60.0"}},	//0
	{{"    3.3"},{"   10.0"},{"   26.7"},{"   50.0"}}	//0
};

const u32 acr_val_1[5][4]= {
	{     0,  3000,  8000,15000},	//0
	{   200,   600,  1600, 3000},	//0
	{   100,   300,   800, 1500},	//0
//	{    67,   200,   533, 1000},	//0
	{    50,   150,   400,  750},	//0
//	{    40,   120,   320,  600},	//0
	{    33,   100,   267,  500}	//0
};

void out_result(void);
void out_result_1(u16 sno);
void out_result_2(u16 sno);
void out_result_3(u16 sno);
void out_result_4(u16 sno);
void out_no_strip(void);
void out_wrong_strip(void);
u32 cal_point(u32 qq);
u32 cal_point_1(u32 qq);
void make_date_1(u8 year, u8 month, u8 day);
u8 col;

#define MMM	2000
/************************************************************************/
void lcd_test(void){
	delay_ms(1000);	
	dsp_standby();	delay_ms(MMM);
	dsp_seq_set();	delay_ms(MMM);
	dsp_idno_set();	delay_ms(MMM);
	dsp_strip_sel();	delay_ms(MMM);
	dsp_color_sel();	delay_ms(MMM);
	dsp_turbi_sel();	delay_ms(MMM);
	dsp_menu_home();	delay_ms(MMM);
	dsp_date_time();	delay_ms(MMM);
	dsp_sys_chk();	delay_ms(MMM);
	dsp_sys_info();	delay_ms(MMM);
	dsp_memory();	delay_ms(MMM);
	dsp_result_store(1);	delay_ms(MMM);
	dsp_result(1);	delay_ms(MMM);
	dsp_send_no();	delay_ms(MMM);
	dsp_delete_no();	delay_ms(MMM);
	dsp_setting();	delay_ms(MMM);
	dsp_format();	delay_ms(MMM);
	dsp_system();	delay_ms(MMM);
	dsp_change1();	delay_ms(MMM);
	dsp_change2();	delay_ms(MMM);
	dsp_change3();	delay_ms(MMM);
	dsp_down();	delay_ms(MMM);
	dsp_poff();	delay_ms(MMM);
	dsp_measure();	
	while(1);
}

/************************************************************************/
void dsp_first(void){
	RASTNO=RAST1;
	LCDbmpxx(b_base,0,0);					// 
	LCDbmpxx(b_urine,32,100);					// 
	LCDbmpxx(b_init,32,216);					// 
}
/************************************************************************/
#define ST_x	130
#define ST_y	92+(36*3)
void dsp_standby(void){
u8 b1;	
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_test1,20,40);					

	LCDbmpxx(b_strip,20,88);
	put_strip_name();
	msg20(128,92,BLK,top_str);	

	LCDbmpxx(b_seq,  20,88+(36*1));					
	b1 = put_seqno(SEQ_NO);
	msg20(130+b1,92+(36*1),BLK,top_str);	

	LCDbmpxx(b_idno, 20,88+(36*2));					
//	msg20(128,92+(36*2),BLK,USER_ID);	
	msg16(128-6,92+(36*2)+5,BLK,USER_ID);	

	LCDbmpxx(b_color,20,88+(36*3));							
	switch(COLOR_NO){
		case 0:	msg20(ST_x,ST_y,BLK,"NONE");		break; //1	
		case 1:	msg20(ST_x,ST_y,BLK,"YELLOW");		break; //2	
		case 2:	msg20(ST_x,ST_y,BLK,"DK YELLOW");	break; //3	
		case 3:	msg20(ST_x,ST_y,BLK,"STRAW");		break; //4	
		case 4:	msg20(ST_x,ST_y,BLK,"AMBER");		break; //5 
		case 5:	msg20(ST_x,ST_y,BLK,"RED");			break; //6 
		case 6:	msg20(ST_x,ST_y,BLK,"ORANGE");		break; //7 
		case 7:	msg20(ST_x,ST_y,BLK,"GREEN");		break; //8 
		default: msg20(ST_x,ST_y,BLK,"OTHER");		break; //9 
	}
	LCDbmpxx(b_trubi,20,88+(36*4));			
	switch(TURBI_NO){
		case 0:	msg20(ST_x,ST_y+36,BLK,"NONE");		break; //1	
		case 1:	msg20(ST_x,ST_y+36,BLK,"CLEAR");		break; //2	
		case 2:	msg20(ST_x,ST_y+36,BLK,"SL CLOUDY");	break; //3	
		case 3:	msg20(ST_x,ST_y+36,BLK,"CLOUDY");		break; //4	
		case 4:	msg20(ST_x,ST_y+36,BLK,"TURBID");		break; //5 
		default: msg20(ST_x,ST_y+36,BLK,"OTHER");		break; //9 
	}

	LCDbmpxx(b_start1, 20,88+(36*5)+4);			
	LCDbmpxx(b_menu1, 140,88+(36*5)+4);			
	LCDbmpxx(b_clean, 140,88+(36*5)+4+(68*1));			
	LCDbmpxx(b_power, 140,88+(36*5)+4+(68*2));				

//	LCDbmpxx(b_graph2,20,40);					
}

/************************************************************************/
void dsp_seq_set(void){
	LCDbmpxb(b_base,0,0);					
	out_time();

	LCDbmpxx(b_seqset,20,40);					
	LCDbmpxx(b_seq   ,20,88);					

//	LCDbmpxx(b_number,20,88+44);					
	LCDbmpxx(b_num_c,20,88+44);					
	LCDbmpxx(b_test2, 28,420);					
}
/************************************************************************/
void blink_seqno(void){
u8 b1, b2;
	for(b2=0;b2<3;b2++){
		erase_box0(130-2,92-1,124+120,92+30);
		delay_ms(300);
		b1 = put_seqno(tmp_seqno);
		msg20(130+b1,92,BLK,top_str);	
		delay_ms(300);
	}	
}
/************************************************************************/
void dsp_idno_set(void){
	LCDbmpxb(b_base,0,0);					
	out_time();

	LCDbmpxx(b_idset1,20,40);					
	LCDbmpxx(b_idno,  20,88);					

//	LCDbmpxx(b_num_c,20,88+44);					
	LCDbmpxx(b_numapb,20,88+44);					
	LCDbmpxx(b_test2, 28,420);					
}
/************************************************************************/
void blink_idno(void){
u8 b1, b2;
	for(b2=0;b2<3;b2++){
		erase_box0(124-2,92-1,124+124,92+30);
		delay_ms(300);
		b1 = put_seqno(tmp_seqno);
//		msg20(128,92,BLK,tmp_idno);	
		msg16(128-6,92+5,BLK,tmp_idno);	
		delay_ms(300);
	}	
}
/************************************************************************/
//스트립종류화면표시
void dsp_strip_sel(void){
	LCDbmpxb(b_base,0,0);					
	out_time();

	LCDbmpxx(b_strset,20,40);					
	LCDbmpxx(b_box3,  20,88+12);					

	LCDbmpxx(b_test2, 28,420);					
}
/********************************************************************/
void dsp_strip8line(void){
u32 i, j, k, m;	
char str[13];
	printf0("\n StripNo:%d,%d", tmp_strip,MAX_STRIPNO);
	k = tmp_strip;
	for(j=0;j<8;j++){
		m = k + j;
		if(m<MAX_STRIPNO){
			for(i=0;i<12;i++) str[i] = STORE_RAM[STRIP_RAM+(m*StripOffset)+i];
			str[i]=0;
		}else{
			for(i=0;i<12;i++) str[i]=' '; str[i]=0;
		}
		if((tmp_strip+j)==sel_strip) msg20(36,112+(33*j),BLU,str);
		else msg20(36,112+(33*j),BLK,str);
		//msg20(36,112+(33*j),WHT,str);
	}
}
/********************************************************************/
void blink_strip(u8 lno){
u8 b1;
	for(b1=0;b1<3;b1++){
		erase_box0(36-2,112+(33*lno)-1,36+184,112+(33*lno)+30);
		delay_ms(300);
		dsp_strip8line();
		delay_ms(300);
	}
}
/************************************************************************/
#define CS_X	40
#define CS_Y	112
#define CS_T	29
void dsp_color_sel(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_colset,20,40);					
	LCDbmpxx(b_box3,  20,88+12);				
	dsp_color_no();
	LCDbmpxx(b_test2, 28,420);					
}
/************************************************************************/
void dsp_color_no(void){
	if(COLOR_NO==0) msg20(CS_X+0,CS_Y+(CS_T*0),BLU,"NONE");		//1
	else msg20(CS_X+0,CS_Y+(CS_T*0),BLK,"NONE");		//1
	if(COLOR_NO==1) msg20(CS_X+0,CS_Y+(CS_T*1),BLU,"YELLOW");	//2
	else msg20(CS_X+0,CS_Y+(CS_T*1),BLK,"YELLOW");	//2
	if(COLOR_NO==2) msg20(CS_X+0,CS_Y+(CS_T*2),BLU,"DK YELLOW");	//3
	else msg20(CS_X+0,CS_Y+(CS_T*2),BLK,"DK YELLOW");	//3
	if(COLOR_NO==3) msg20(CS_X+0,CS_Y+(CS_T*3),BLU,"STRAW");	//4
	else msg20(CS_X+0,CS_Y+(CS_T*3),BLK,"STRAW");	//4
	if(COLOR_NO==4) msg20(CS_X+0,CS_Y+(CS_T*4),BLU,"AMBER");	//5
	else msg20(CS_X+0,CS_Y+(CS_T*4),BLK,"AMBER");	//5
	if(COLOR_NO==5) msg20(CS_X+0,CS_Y+(CS_T*5),BLU,"RED");		//6
	else msg20(CS_X+0,CS_Y+(CS_T*5),BLK,"RED");		//6
	if(COLOR_NO==6) msg20(CS_X+0,CS_Y+(CS_T*6),BLU,"ORANGE");	//7
	else msg20(CS_X+0,CS_Y+(CS_T*6),BLK,"ORANGE");	//7
	if(COLOR_NO==7) msg20(CS_X+0,CS_Y+(CS_T*7),BLU,"GREEN");	//8
	else msg20(CS_X+0,CS_Y+(CS_T*7),BLK,"GREEN");	//8
	if(COLOR_NO==8) msg20(CS_X+0,CS_Y+(CS_T*8),BLU,"OTHER");	//9
	else msg20(CS_X+0,CS_Y+(CS_T*8),BLK,"OTHER");	//9
}
/************************************************************************/
void blink_color(void){
u8 b1;
	for(b1=0;b1<3;b1++){
		erase_box0(CS_X-2,CS_Y+(CS_T*COLOR_NO)-1,220,CS_Y+(CS_T*COLOR_NO)+30);
		delay_ms(300);
		switch(COLOR_NO){
			case 0:	msg20(CS_X+0,CS_Y+(CS_T*0),BLK,"NONE");		break;
			case 1:	msg20(CS_X+0,CS_Y+(CS_T*1),BLK,"YELLOW");	break;
			case 2:	msg20(CS_X+0,CS_Y+(CS_T*2),BLK,"DK YELLOW"); break;
			case 3:	msg20(CS_X+0,CS_Y+(CS_T*3),BLK,"STRAW");	break;
			case 4:	msg20(CS_X+0,CS_Y+(CS_T*4),BLK,"AMBER");	break;
			case 5:	msg20(CS_X+0,CS_Y+(CS_T*5),BLK,"RED");		break;
			case 6:	msg20(CS_X+0,CS_Y+(CS_T*6),BLK,"ORANGE");	break;
			case 7:	msg20(CS_X+0,CS_Y+(CS_T*7),BLK,"GREEN");	break;
			default: msg20(CS_X+0,CS_Y+(CS_T*8),BLK,"OTHER");	break;
		}
		delay_ms(300);
	}
}
/************************************************************************/
void dsp_turbi_sel(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_turset,20,40);					
	LCDbmpxx(b_box3,  20,88+12);					
	dsp_turbi_no();
	LCDbmpxx(b_test2, 28,420);					
}

/************************************************************************/
void dsp_turbi_no(void){
	if(TURBI_NO==0) msg20(CS_X,CS_Y+(CS_T*0),BLU,"NONE");		
	else msg20(CS_X,CS_Y+(CS_T*0),BLK,"NONE");		
	if(TURBI_NO==1) msg20(CS_X,CS_Y+(CS_T*1),BLU,"CLEAR");		
	else msg20(CS_X,CS_Y+(CS_T*1),BLK,"CLEAR");		
	if(TURBI_NO==2) msg20(CS_X,CS_Y+(CS_T*2),BLU,"SL CLOUDY");	
	else msg20(CS_X,CS_Y+(CS_T*2),BLK,"SL CLOUDY");	
	if(TURBI_NO==3) msg20(CS_X,CS_Y+(CS_T*3),BLU,"CLOUDY");		
	else msg20(CS_X,CS_Y+(CS_T*3),BLK,"CLOUDY");		
	if(TURBI_NO==4) msg20(CS_X,CS_Y+(CS_T*4),BLU,"TURBID");		
	else msg20(CS_X,CS_Y+(CS_T*4),BLK,"TURBID");		
	if(TURBI_NO==5) msg20(CS_X,CS_Y+(CS_T*5),BLU,"OTHER");		
	else msg20(CS_X,CS_Y+(CS_T*5),BLK,"OTHER");		
}
/************************************************************************/
void blink_turbi(void){
u8 b1;
	for(b1=0;b1<3;b1++){
		erase_box0(CS_X-2,CS_Y+(CS_T*TURBI_NO)-1,220,CS_Y+(CS_T*TURBI_NO)+30);
		delay_ms(300);
		switch(TURBI_NO){
			case 0:	msg20(CS_X,CS_Y+(CS_T*0),BLK,"NONE");		break;
			case 1:	msg20(CS_X,CS_Y+(CS_T*1),BLK,"CLEAR");		break;
			case 2:	msg20(CS_X,CS_Y+(CS_T*2),BLK,"SL CLOUDY");	 break;
			case 3:	msg20(CS_X,CS_Y+(CS_T*3),BLK,"CLOUDY");		break;
			case 4:	msg20(CS_X,CS_Y+(CS_T*4),BLK,"TURBID");		break;
			default: msg20(CS_X,CS_Y+(CS_T*5),BLK,"OTHER");		break;
		}
		delay_ms(300);
	}
}

/************************************************************************/
void dsp_menu_home(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_menu2, 20,40);					

	LCDbmpxx(b_date1, 30,92);					
	LCDbmpxx(b_memo1, 30,92+(108*1));					
	LCDbmpxx(b_info1, 30,92+(108*2));					

	LCDbmpxx(b_check1, 138,92);					
	LCDbmpxx(b_sett1,  138,92+(108*1));					

	LCDbmpxx(b_test2, 28,420);					
}

/************************************************************************/
void dsp_date_time(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_date2, 20,40);					

	LCDbmpxx(b_date3, 20,88);					
	LCDbmpxx(b_time,  20,88+(36*1));					

	LCDbmpxx(b_number,20,88+(36*2));					

	LCDbmpxx(b_menu3,  20,428);					
	LCDbmpxx(b_test3, 139,428);					
}

void out_date_time(void){
u8 b1;	
	date_copy_adj(setyear, setmonth, setday);
	msg20(134,92,BLU,top_str);
	if(cursor<6){
		if(cursor>=4) b1 = cursor+2;
		else if(cursor>=2) b1 = cursor+1;
		else b1 = cursor;
		cursor_20(134, 92, b1, top_str);
	}
	time_copy_adj(sethour, setmin, setsec);
	msg20(134,92+36,BLU,top_str);
	if(cursor>5){
		b1 = cursor - 6;
		if(b1>=4) b1 = b1+2;
		else if(b1>=2) b1 = b1+1;
		cursor_20(134, 92+36, b1, top_str);
	}
}

/************************************************************************/
void dsp_sys_chk(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_check2, 20,40);					

	LCDbmpxx(b_start2, 30,88);					
	LCDbmpxx(b_box4,   30,88+48);					
	draw_box(CABXS,CABYS,CABXE,CABYE,WHT,RAST1);
	LCDbmpxx(b_box5,   30,88+48+60);					
	draw_box(SYBXS,SYBYS,SYBXE,SYBYE,WHT,RAST1);
	LCDbmpxx(b_box6,   35,88+48+60+60+4);					

	LCDbmpxx(b_menu3,  20,428);					
	LCDbmpxx(b_test3, 139,428);					
}

/************************************************************************/
#define SI_X	45
#define SO_X	SI_X+65
#define SI_Y	110
#define SO_Y	28//32		2015-08-26 오후 4:25:18  
void dsp_sys_info(void){
	u16 i;//2015-08-26 오후 4:28:51
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_info2, 20,40);					

	LCDbmpxx(b_box7,   35,100);					

	msg20(SI_X,SI_Y+(SO_Y*0),BLK,"Ver.");
	//sprintf(top_str, "- %d.%d%dN",ProgVer/100,(ProgVer%100)/10,ProgVer%10);2015-06-04 오후 2:20:40
	sprintf(top_str, "- %c.%c%cA",ver_str[0], ver_str[1], ver_str[2]);
	msg20(SO_X,SI_Y+(SO_Y*0),BLK,top_str);	
	
	msg20(SI_X,SI_Y+(SO_Y*1),BLK,"S/N.");
	msg20(SO_X,SI_Y+(SO_Y*1),BLK,SYS_ID);	

	msg20(SI_X,SI_Y+(SO_Y*2),BLK,"Strip");
	put_strip_name();
	msg20(SO_X,SI_Y+(SO_Y*2),BLK,top_str);	

	msg20(SI_X,SI_Y+(SO_Y*3),BLK,"Unit");
	if(UNIT_FLAG==UNIT_CONV) msg20(SO_X,SI_Y+(SO_Y*3),BLK,"- CONV.");	
	else msg20(SO_X,SI_Y+(SO_Y*3),BLK,"- SI.");	
	
	msg20(SI_X,SI_Y+(SO_Y*4),BLK,"Plus");
	if(PLUS_FLAG) msg20(SO_X,SI_Y+(SO_Y*4),BLK,"- YES");	
	else msg20(SO_X,SI_Y+(SO_Y*4),BLK,"- NO");	

	msg20(SI_X,SI_Y+(SO_Y*5),BLK,"Pos.");
	if(MARK_FLAG) msg20(SO_X,SI_Y+(SO_Y*5),BLK,"- YES");	
	else msg20(SO_X,SI_Y+(SO_Y*5),BLK,"- NO");	
	
	msg20(SI_X,SI_Y+(SO_Y*6),BLK,"Sound");
	if(SOUND_FLAG) msg20(SO_X,SI_Y+(SO_Y*6),BLK,"- YES");	
	else msg20(SO_X,SI_Y+(SO_Y*6),BLK,"- NO");	

	msg20(SI_X,SI_Y+(SO_Y*7),BLK,"Speed");
	if(SPEED_FLAG) msg20(SO_X,SI_Y+(SO_Y*7),BLK,"- Quick");	
	else msg20(SO_X,SI_Y+(SO_Y*7),BLK,"- General");	

	msg20(SI_X,SI_Y+(SO_Y*8),BLK,"Baud");
	if(BAUD_FLAG==2) msg20(SO_X,SI_Y+(SO_Y*8),BLK,"- 9600");	
	else if(BAUD_FLAG==1) msg20(SO_X,SI_Y+(SO_Y*8),BLK,"- 19200");	
	else msg20(SO_X,SI_Y+(SO_Y*8),BLK,"- 38400");	
		//2015-08-26 오후 4:29:01 Start
	i = PARA_RAM+WEIGHT_SIZE+(12*7*17)+36;
	
	if(StripPosition[10]==StripPosition[11]){
		sprintf(top_str, " ( R-%c%c%c%c-%c%c%c%c )",STORE_RAM[i],STORE_RAM[i+1],STORE_RAM[i+2],STORE_RAM[i+3],ar_item_ram[0][0][0],ar_item_ram[0][0][1],ar_item_ram[0][0][2],ar_item_ram[0][0][3]);
	}else{
		sprintf(top_str, " ( N-%c%c%c%c-%c%c%c%c )",STORE_RAM[i],STORE_RAM[i+1],STORE_RAM[i+2],STORE_RAM[i+3],ar_item_ram[0][0][0],ar_item_ram[0][0][1],ar_item_ram[0][0][2],ar_item_ram[0][0][3]);
	}
	msg20(SI_X,SI_Y+(SO_Y*9)+4,BLK,top_str);
	//2015-08-26 오후 4:29:09 End	
	LCDbmpxx(b_menu3,  20,428);					
	LCDbmpxx(b_test3, 139,428);					
}

/************************************************************************/
void dsp_memory(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_memo2, 20,40);					

	LCDbmpxx(b_view1,   30,100);					
	LCDbmpxx(b_send1,   30,100+52);					
	LCDbmpxx(b_del1,    30,100+52+52);					

	LCDbmpxx(b_menu3,  20,428);					
	LCDbmpxx(b_test3, 139,428);					
}

/************************************************************************/
void dsp_view_no(void){
u8 b1;
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_view4, 20,40);					

	LCDbmpxx(b_seq   ,20,88);					
	b1 = put_seqno(tmp_seqno);
	msg20(130+b1,92,BLK,top_str);	
//	LCDbmpxx(b_number,20,88+44);					
	LCDbmpxx(b_num_c,20,88+44);					

//	LCDbmpxx(b_view1, 30,428);		//??????????????
	LCDbmpxx(b_memo3,  22,428);					
	LCDbmpxx(b_view5, 139,428);					
}
/************************************************************************/
void dsp_result_store(u16 sno){
//u8 i;
u8 i,q;

	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_view4, 20,40);					

	msg16(22,89,BLK, "SEQ:");
	sprintf(top_str, "%4d",sno);
	msg16(22+38,89,BLK, top_str);

	msg16(22+38+38+4,89,BLK, "ID:");				//????????????
	msg16(22+38+38+30,89,BLK, tmp_userid);
	
	msg16(22,89+16,BLK,"Strip:");
	msg16(22+58,89+16,BLK,strip_name);
	
	time_copy(MEASyear, MEASmonth, MEASday, MEAShour, MEASmin, MEASsec);
	msg16(22,89+16+16,BLK,"Date:");
	msg16(22+58,89+16+16,BLK,top_str);

	msg16(22,89+48,BLK, "Color:");
	switch(store_COLOR_NO){
		case 0:	msg16(22+58,89+48,BLK,"NONE");		break; //1	
		case 1:	msg16(22+58,89+48,BLK,"YELLOW");		break; //2	
		case 2:	msg16(22+50,89+48,BLK,"DKYELLOW");	break; //3	
		case 3:	msg16(22+58,89+48,BLK,"STRAW");		break; //4	
		case 4:	msg16(22+58,89+48,BLK,"AMBER");		break; //5 
		case 5:	msg16(22+58,89+48,BLK,"RED");			break; //6 
		case 6:	msg16(22+58,89+48,BLK,"ORANGE");		break; //7 
		case 7:	msg16(22+58,89+48,BLK,"GREEN");		break; //8 
		default: msg16(22+58,89+48,BLK,"OTHER");		break; //9 
	}
	
	msg16(22+116+4,89+16+16+16,BLK, "Turb.:");
	switch(store_TURBI_NO){
		case 0:	msg16(22+116+54,89+48,BLK,"NONE");		break; //1	
		case 1:	msg16(22+116+54,89+48,BLK,"CLEAR");		break; //2	
		case 2:	msg16(22+116+54,89+48,BLK,"SLCLOUDY");	break; //3	
		case 3:	msg16(22+116+54,89+48,BLK,"CLOUDY");		break; //4
		case 4:	msg16(22+116+54,89+48,BLK,"TURBID");		break; //5 
		default: msg16(22+116+54,89+48,BLK,"OTHER");		break; //9 
	}
		
	if(LOAD_NO==1){
		i=0;
		q=STRIP_INFO[0];		
		if(q>5){
			out_wrong_strip();
		}else{
			if(ERROR_NO==0){
				if((datCRE>=0)&&(datALB>=0)){
					out_result();
					//LCDbmpxb(b_base,0,0);					
					//out_time();
					//LCDbmpxx(b_view4, 20,40);					
					//out_result_2(sno);
				}else if((datCRE>=0)&&(datMDA>=0)){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_1(sno);
				}else if(datLDH>=0){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_3(sno);
				}else if(datBHB>=0){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_4(sno);
				}else{
					out_result();
				}
			}else{
				out_no_strip();
			}
		}
	}else{
		if(ERROR_NO==0){
			q=STRIP_INFO[0];		
			if(q<3){
				if((datCRE>=0)&&(datALB>=0)){
					out_result();
					//LCDbmpxb(b_base,0,0);					
					//out_time();
					//LCDbmpxx(b_view4, 20,40);					
					//out_result_2(sno);
				}else if((datCRE>=0)&&(datMDA>=0)){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_1(sno);
				}else if(datLDH>=0){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_3(sno);
				}else if(datBHB>=0){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_4(sno);
				}
			}else{
				out_result();
			}
		}else{
			out_no_strip();
		}
	}
	
//	out_result();

	LCDbmpxx(b_memo4,  22,428);					
	LCDbmpxx(b_send4, 102,428);					
	LCDbmpxx(b_del4,  182,428);					
}

/************************************************************************/
void dsp_result(u16 sno){
u8 i,q;
	LCDbmpxb(b_base,0,0);					
	out_time();

	LCDbmpxx(b_rest5, 20,40);		//?????????????			
	
	msg16(22,89,BLK, "SEQ:");
	sprintf(top_str, "%4d",sno);
	msg16(22+38,89,BLK, top_str);

	msg16(22+38+38+4,89,BLK, "ID:");				//????????????
	msg16(22+38+38+30,89,BLK, USER_ID);

	msg16(22,89+16,BLK,"Strip:");
	msg16(22+58,89+16,BLK,strip_name);

	readAllRTC();
	time_copy(MEASyear, MEASmonth, MEASday, MEAShour, MEASmin, MEASsec);
	msg16(22,89+16+16,BLK,"Date:");
	msg16(22+58,89+16+16,BLK,top_str);

	msg16(22,89+48,BLK, "Color:");
	switch(COLOR_NO){
		case 0:	msg16(22+58,89+48,BLK,"NONE");		break; //1	
		case 1:	msg16(22+58,89+48,BLK,"YELLOW");		break; //2	
		case 2:	msg16(22+50,89+48,BLK,"DKYELLOW");	break; //3	
		case 3:	msg16(22+58,89+48,BLK,"STRAW");		break; //4	
		case 4:	msg16(22+58,89+48,BLK,"AMBER");		break; //5 
		case 5:	msg16(22+58,89+48,BLK,"RED");			break; //6 
		case 6:	msg16(22+58,89+48,BLK,"ORANGE");		break; //7 
		case 7:	msg16(22+58,89+48,BLK,"GREEN");		break; //8 
		default: msg16(22+58,89+48,BLK,"OTHER");		break; //9 
	}
	
	msg16(22+116+4,89+16+16+16,BLK, "Turb.:");
	switch(TURBI_NO){
		case 0:	msg16(22+116+54,89+48,BLK,"NONE");		break; //1	
		case 1:	msg16(22+116+54,89+48,BLK,"CLEAR");		break; //2	
		case 2:	msg16(22+116+54,89+48,BLK,"SLCLOUDY");	break; //3	
		case 3:	msg16(22+116+54,89+48,BLK,"CLOUDY");		break; //4	
		case 4:	msg16(22+116+54,89+48,BLK,"TURBID");		break; //5 
		default: msg16(22+116+54,89+48,BLK,"OTHER");		break; //9 
	}

	if(LOAD_NO==1){
		i=0;
		q=STRIP_INFO[0];		
		if(q>5){
			out_wrong_strip();
		}else{
			if(ERROR_NO==0){
				if((datCRE>=0)&&(datALB>=0)){
					out_result();
					//LCDbmpxb(b_base,0,0);					
					//out_time();
					//LCDbmpxx(b_view4, 20,40);					
					//out_result_2(sno);
				}else if((datCRE>=0)&&(datMDA>=0)){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_1(sno);
				}else if(datLDH>=0){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_3(sno);
				}else if(datBHB>=0){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_4(sno);
				}else{
					out_result();
				}
			}else{
				out_no_strip();
			}
		}
	}else{
		if(ERROR_NO==0){
			q=STRIP_INFO[0];		
			if(q<3){
				if((datCRE>=0)&&(datALB>=0)){
					out_result();
					//LCDbmpxb(b_base,0,0);					
					//out_time();
					//LCDbmpxx(b_view4, 20,40);					
					//out_result_2(sno);
				}else if((datCRE>=0)&&(datMDA>=0)){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_1(sno);
				}else if(datLDH>=0){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_3(sno);
				}else if(datBHB>=0){
					LCDbmpxb(b_base,0,0);					
					out_time();
					LCDbmpxx(b_view4, 20,40);					
					out_result_4(sno);
				}
			}else{
				out_result();
			}
		}else{
			out_no_strip();
		}
	}

	LCDbmpxx(b_test2, 28,430);					
}

/************************************************************************/
void dsp_send_no(void){
u8 b1;
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_send2, 20,40);					

	LCDbmpxx(b_start3, 20,88);					
	b1 = put_seqno(mem_srt);
	msg20(130+b1,92,BLK,top_str);	
	LCDbmpxx(b_end,  20,88+(36*1));					
	b1 = put_seqno(mem_end);
	msg20(130+b1,92+36,BLK,top_str);	

//	LCDbmpxx(b_number,20,88+(36*2));					
	LCDbmpxx(b_num_c,20,88+(36*2));					

	LCDbmpxx(b_memo3,  20,428);					
	LCDbmpxx(b_send3, 139,428);					
}

/************************************************************************/
void dsp_delete_no(void){
u8 b1;
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_del2, 20,40);					

	LCDbmpxx(b_start3, 20,88);					
	b1 = put_seqno(mem_srt);
	msg20(130+b1,92,BLK,top_str);	
	LCDbmpxx(b_end,  20,88+(36*1));					
	b1 = put_seqno(mem_end);
	msg20(130+b1,92+36,BLK,top_str);	

//	LCDbmpxx(b_number,20,88+(36*2));					
	LCDbmpxx(b_num_c,20,88+(36*2));					

	LCDbmpxx(b_memo3,  20,428);					
	LCDbmpxx(b_del3, 139,428);					
}

/************************************************************************/
void dsp_setting(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_inst, 20,40);					

	LCDbmpxx(b_rest1,   30,100);					
	LCDbmpxx(b_sys1,   30,100+52);					
	LCDbmpxx(b_chge1,    30,100+52+52);					

	LCDbmpxx(b_menu3,  20,428);					
	LCDbmpxx(b_test3, 139,428);					
}

/************************************************************************/
void dsp_format(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_rest2, 20,40);					

	LCDbmpxx(b_unit,   30,96);					
	if(UNIT_FLAG==UNIT_CONV) LCDbmpxx(b_conv,   30,96+28);
//	else if(UNIT_FLAG==UNIT_ARBI) LCDbmpxx(b_arbi,   30,96+28);
	else LCDbmpxx(b_si,   30,96+28);

	LCDbmpxx(b_plus,   30,96+72);		
	if(PLUS_FLAG) LCDbmpxx(b_enab,   30,96+28+72);					
	else LCDbmpxx(b_disab,   30,96+28+72);					

	LCDbmpxx(b_mark,   30,96+72+72);					
	if(MARK_FLAG) LCDbmpxx(b_yes,    30,96+28+72+72);					
	else LCDbmpxx(b_no,    30,96+28+72+72);					

//	LCDbmpxx(b_menu3,  20,428);					
	LCDbmpxx(b_sett2,  20,428);					
	LCDbmpxx(b_test3, 139,428);			
}

/************************************************************************/
void dsp_system(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_sys2, 20,40);					

	LCDbmpxx(b_speed,   30,96);					
	if(SPEED_FLAG) LCDbmpxx(b_fast,   30,96+28);					
	else LCDbmpxx(b_normal,   30,96+28);					

	LCDbmpxx(b_sound,   30,96+72);		
	if(SOUND_FLAG) LCDbmpxx(b_enab,   30,96+28+72);					
	else LCDbmpxx(b_disab,   30,96+28+72);					

	LCDbmpxx(b_baud,   30,96+72+72);					
	if(BAUD_FLAG==2) LCDbmpxx(b_9600,    30,96+28+72+72);					
	else if(BAUD_FLAG==1) LCDbmpxx(b_19200,    30,96+28+72+72);					
	else LCDbmpxx(b_38400,    30,96+28+72+72);					

//	LCDbmpxx(b_menu3,  20,428);					
	LCDbmpxx(b_sett2,  20,428);					
	LCDbmpxx(b_test3, 139,428);					
}

/************************************************************************/
#define X_OFS	38
#define X_OFG	68
#define Y_OFS	104
#define Y_OFG	52

void dsp_change1(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_chge2, 20,40);					
	LCDbmpxx(b_box8,  20,88);		
				
	LCDbmpxx(b_uro,   X_OFS+(X_OFG*0),Y_OFS+(Y_OFG*0));			
	LCDbmpxx(b_glu,   X_OFS+(X_OFG*1),Y_OFS+(Y_OFG*0));			
	LCDbmpxx(b_bil,   X_OFS+(X_OFG*2),Y_OFS+(Y_OFG*0));		
	LCDbmpxx(b_ket,   X_OFS+(X_OFG*0),Y_OFS+(Y_OFG*1));			
	LCDbmpxx(b_sg,    X_OFS+(X_OFG*1),Y_OFS+(Y_OFG*1));			
	LCDbmpxx(b_bld,   X_OFS+(X_OFG*2),Y_OFS+(Y_OFG*1));			
	LCDbmpxx(b_ph,    X_OFS+(X_OFG*0),Y_OFS+(Y_OFG*2));			
	LCDbmpxx(b_pro,   X_OFS+(X_OFG*1),Y_OFS+(Y_OFG*2));			
	LCDbmpxx(b_nit,   X_OFS+(X_OFG*2),Y_OFS+(Y_OFG*2));			
	LCDbmpxx(b_leu,   X_OFS+(X_OFG*0),Y_OFS+(Y_OFG*3));			
	LCDbmpxx(b_asa,   X_OFS+(X_OFG*1),Y_OFS+(Y_OFG*3));			
	LCDbmpxx(b_ma,    X_OFS+(X_OFG*2),Y_OFS+(Y_OFG*3));			
	LCDbmpxx(b_cre,   X_OFS+(X_OFG*0),Y_OFS+(Y_OFG*4));			
	LCDbmpxx(b_mda,   X_OFS+(X_OFG*1),Y_OFS+(Y_OFG*4));			
	LCDbmpxx(b_bhb,   X_OFS+(X_OFG*2),Y_OFS+(Y_OFG*4));			
	LCDbmpxx(b_cal,   X_OFS+(X_OFG*0),Y_OFS+(Y_OFG*5));			
	LCDbmpxx(b_nic,   X_OFS+(X_OFG*1),Y_OFS+(Y_OFG*5));			
	LCDbmpxx(b_bbb,   X_OFS+(X_OFG*2),Y_OFS+(Y_OFG*5));			

//	LCDbmpxx(b_menu3,  20,428);					
	LCDbmpxx(b_sett2,  20,428);					
	LCDbmpxx(b_test3, 139,428);					
}

/************************************************************************/
#define X_OFM	149
#define Y_OFM	140
#define Y_OFH	36
void dsp_change2(void){
u16 i;	
	LCDbmpxb(b_base,0,0);
	out_time();
	LCDbmpxx(b_chge2, 20,40);
				
	LCDbmpxx(b_arw_l, 20,96);		
	switch(pr_no){
		case  0:	LCDbmpxx(b_uro,   108,92);	break;
		case  1:	LCDbmpxx(b_glu,   108,92);	break;
		case  2:	LCDbmpxx(b_bil,   108,92);	break;
		case  3:	LCDbmpxx(b_ket,   108,92);	break;
		case  4:	LCDbmpxx(b_sg,    108,92);	break;
		case  5:	LCDbmpxx(b_bld,   108,92);	break;
		case  6:	LCDbmpxx(b_ph,    108,92);	break;
		case  7:	LCDbmpxx(b_pro,   108,92);	break;
		case  8:	LCDbmpxx(b_nit,   108,92);	break;
		case  9:	LCDbmpxx(b_leu,   108,92);	break;
		case 10:	LCDbmpxx(b_asa,   108,92);	break;
		case 11:	LCDbmpxx(b_ma,    108,92);	break;
		case 12:	LCDbmpxx(b_cre,   108,92);	break;
		case 13:	LCDbmpxx(b_mda,   108,92);	break;
		case 14:	LCDbmpxx(b_bhb,   108,92);	break;
		case 15:	LCDbmpxx(b_cal,   108,92);	break;
		case 16:	LCDbmpxx(b_nic,   108,92);	break;
		default:	LCDbmpxx(b_bbb,   108,92);	break;
	}
	LCDbmpxx(b_arw_r, 218,96);			

	LCDbmpxx(b_rest3, 20   ,Y_OFM+(Y_OFH*0));			
	LCDbmpxx(b_rest4, X_OFM,Y_OFM+(Y_OFH*0));			
	for(i=1;i<8;i++){
		LCDbmpxx(b_box9,  20   ,Y_OFM+(Y_OFH*i));			
		LCDbmpxx(b_box10, X_OFM,Y_OFM+(Y_OFH*i));			
	}

//	LCDbmpxx(b_menu3,  20,428);					
	LCDbmpxx(b_sett2,  20,428);					
	LCDbmpxx(b_test3, 139,428);					
}

/************************************************************************/
void dsp_change3(void){
u16 w;
	LCDbmpxx(b_box11, 14,268);
	sprintf(top_str, "%d", pr_ref);
	if(pr_ref<10) w = (11+11+11)/2;
	else if(pr_ref<100) w = (11+11)/2;
	else if(pr_ref<1000) w = 6;
	else w= 0;
	msg20(114+w,303,BLK,top_str);

	sprintf(top_str, "%d", pr_dgt);
	if(pr_dgt<10) w = (16+16+16)/2;
	else if(pr_dgt<100) w = (16+16)/2;
	else if(pr_dgt<1000) w = 16/2;
	else w= 0;
	msg32(102+w,345,BLK,top_str);
}
/************************************************************************/
void blink_pr_dgt(void){
u8 b1;
u16 w;
	for(b1=0;b1<3;b1++){
		erase_box0(102-2,350-1,102+120,350+30);
		delay_ms(300);

		sprintf(top_str, "%d", pr_dgt);
		if(pr_dgt<10) w = (16+16+16)/2;
		else if(pr_dgt<100) w = (16+16)/2;
		else if(pr_dgt<1000) w = 16/2;
		else w= 0;
		msg32(102+w,345,BLK,top_str);
		delay_ms(300);
	}
}
/************************************************************************/
void dsp_down(void){
	LCDbmpxb(b_base,0,0);
	out_time();
	LCDbmpxx(b_urine,32,100);					// 
	LCDbmpxx(b_shut,32,216);					// 
}

/************************************************************************/
void dsp_poff(void){
	LCDbmpxb(b_base,0,0);
//	out_time();
	LCDbmpxx(b_urine,32,100);					// 
	LCDbmpxx(b_shut,32,216);					// 
//	LCDbmpxx(b_off, 74,328);					// 
}

/************************************************************************/
void dsp_measure(void){
//	dsp_standby();
	LCDbmpxx(b_meas, 20,268);
	draw_box(MSBXS,MSBYS,MSBXE,MSBYE,WHT,RAST1);
	dsp_measure_time(0);	
}

/********************************************************************/
void dsp_measure_time(u16 dt){
char str[16];	
short i;
//	erase_box(200, 170, 200+70, 170+24);
	i = bin2bcdasc(dt/60);
	str[0]=i>>8;
	str[1]=i&0xff;
	str[2]=':';
	i=bin2bcdasc(dt%60);
	str[3]=i>>8;
	str[4]=i&0xff;
	str[5]=' ';
	str[6]=0;
	msg32(96,400,WHT,str);	
}

/************************************************************************/
/************************************************************************/

#define X_LN	240
#define W_XY	2
#define Y_FF	30
#define Y_LN	Y_FF*7	
#define Y_FF_1	19
#define Y_FF_2	16
#define Y_LN_1	Y_FF_1*14			//was 14
#define T_ST	23
#define X_ST	16
#define Y_ST	96+30+Y_FF
//#define Y_ST	96+48+Y_FF

#define OFF_A 43		//34

void out_result(void){
u32 i, j, k, m, n, q, o,item_cnt;
char str[32],t;
	o=8;
	item_cnt=STRIP_INFO[0];
	if(datCRE>=0 && datALB>=0 && datPRO >= 0){
		item_cnt=item_cnt+1;	
	}else if(datCRE>=0 && datPRO >= 0){
		item_cnt=item_cnt+2;	
	}else if(datCRE>=0 && datALB >= 0){
		item_cnt=item_cnt+2;	
	}
	
	if(LOAD_NO==1){
		draw_box(X_ST,Y_ST,X_ST+X_LN,Y_ST+Y_LN,CYN,RAST1);
		draw_box(X_ST,Y_ST,X_ST+X_LN,Y_ST+Y_LN,CYN,RAST0);
		for(i=0;i<8;i++) line_x(X_ST,Y_ST+(i*Y_FF),X_LN,W_XY,WHT,RAST1);
	
		msg16( T_ST+30, Y_ST+o,	BLK, "Arb.");
		if(UNIT_FLAG==UNIT_CONV)	msg16( T_ST+30+68, Y_ST+o,	BLK, "Conv.");
		else msg16( T_ST+30+68, Y_ST+o,	BLK, "Si.");
		msg16( T_ST+30+68+68, Y_ST+o,	BLK, "Unit");
		i=0;
		q=0;
		while(1){
			if(buffRESULT[i]==0) break;
			else if(buffRESULT[i]==0x0a) q++;
			i++;
		}
		k=0;
		for(j=0;j<item_cnt;j++){
			k = (j*OFF_A)+4;
			n = buffRESULT[k-3];
			t = buffRESULT[k+2];
			k+=1;
			for(m=0;m<3;m++) str[m]=buffRESULT[k++];	str[m]=0;
			k+=1;
			msg16( T_ST, 			Y_ST+o+(Y_FF*(j+1)),	BLK, str);
			k += 4;
			for(m=0;m<8;m++) str[m]=buffRESULT[k++];	str[m]=0;		//arb
			if(PLUS_FLAG){
				if((n==' ')||(!MARK_FLAG)){
					msg16( T_ST+30, 	Y_ST+o+(Y_FF*(j+1)),	BLU, str);
				}else{
					msg16( T_ST+30, 	Y_ST+o+(Y_FF*(j+1)),	RED, str);
				}
			}else{
				if((t==' ')){
					msg16( T_ST+30, 	Y_ST+o+(Y_FF*(j+1)),	BLU, str);
				}
			}k++;
			for(m=0;m<8;m++) str[m]=buffRESULT[k++];	str[m]=0;		//conv or si
			if((n==' ')||(!MARK_FLAG)){
				msg16( T_ST+30+68, 	Y_ST+o+(Y_FF*(j+1)),	BLU, str);
			}else{
				msg16( T_ST+30+68, 	Y_ST+o+(Y_FF*(j+1)),	RED, str);
			}
			for(m=0;m<8;m++) str[m]=buffRESULT[k++];	str[m]=0;		//unit
			msg16( T_ST+30+68+68,  	Y_ST+o+(Y_FF*(j+1)),	YEL, str);
		}
		line_y(X_ST,Y_ST,Y_LN,W_XY,WHT,RAST1);
		line_y(T_ST+25,Y_ST,Y_LN,1,WHT,RAST1);
		line_y(T_ST+25+68,Y_ST,Y_LN,1,WHT,RAST1);
		line_y(T_ST+25+68+68,Y_ST,Y_LN,1,WHT,RAST1);
		line_y(X_ST+X_LN-2,Y_ST,Y_LN,W_XY,WHT,RAST1);
	}else{
		draw_box(X_ST,Y_ST,X_ST+X_LN,Y_ST+Y_LN_1,CYN,RAST1);
		draw_box(X_ST,Y_ST,X_ST+X_LN,Y_ST+Y_LN_1,CYN,RAST0);
		for(i=0;i<15;i++) line_x(X_ST,Y_ST+(i*Y_FF_1),X_LN,W_XY,WHT,RAST1);
	
		msg16( T_ST+30, Y_ST+3,	BLK, "Arb.");
		if(UNIT_FLAG==UNIT_CONV)	msg16( T_ST+30+68, Y_ST+3,	BLK, "Conv.");
		else msg16( T_ST+30+68, Y_ST+3,	BLK, "Si.");
		msg16( T_ST+30+68+68, Y_ST+3,	BLK, "Unit");
		i=0;
		q=0;
		while(1){
			if(buffRESULT[i]==0) break;
			else if(buffRESULT[i]==0x0a) q++;
			i++;
		}
		k=0;
		for(j=0;j<item_cnt;j++){
			k = (j*OFF_A)+4;
			n = buffRESULT[k-3];
			t = buffRESULT[k+2];
			k+=1;
			for(m=0;m<3;m++) str[m]=buffRESULT[k++];	str[m]=0;
			k+=1;
			msg16( T_ST, 			Y_ST+3+(Y_FF_1*(j+1)),	BLK, str);
			k += 4;
			for(m=0;m<8;m++) str[m]=buffRESULT[k++];	str[m]=0;		//arb
			if(PLUS_FLAG){
				if((n==' ')||(!MARK_FLAG)){
					msg16( T_ST+30, 	Y_ST+3+(Y_FF_1*(j+1)),	BLU, str);
				}else{
					msg16( T_ST+30, 	Y_ST+3+(Y_FF_1*(j+1)),	RED, str);
				}
			}else{
				if((t==' ')){
					msg16( T_ST+30, 	Y_ST+3+(Y_FF_1*(j+1)),	BLU, str);
				}
			}k++;
			for(m=0;m<8;m++) str[m]=buffRESULT[k++];	str[m]=0;		//conv or si
			if((n==' ')||(!MARK_FLAG)){
				msg16( T_ST+30+68, 	Y_ST+3+(Y_FF_1*(j+1)),	BLU, str);
			}else{
				msg16( T_ST+30+68, 	Y_ST+3+(Y_FF_1*(j+1)),	RED, str);
			}
			for(m=0;m<8;m++) str[m]=buffRESULT[k++];	str[m]=0;		//unit
			msg16( T_ST+30+68+68,  	Y_ST+3+(Y_FF_1*(j+1)),	YEL, str);
		}
		line_y(X_ST,Y_ST,Y_LN_1,W_XY,WHT,RAST1);
		line_y(T_ST+25,Y_ST,Y_LN_1,1,WHT,RAST1);
		line_y(T_ST+25+68,Y_ST,Y_LN_1,1,WHT,RAST1);
		line_y(T_ST+25+68+68,Y_ST,Y_LN_1,1,WHT,RAST1);
		line_y(X_ST+X_LN-2,Y_ST,Y_LN_1,W_XY,WHT,RAST1);
	}	
}

void out_result_1(u16 sno){
u32  j, k, m, n, q;
u8 i;
float z;
char str[32];
	LCDbmpxx(b_box8,   	20,85);	
	LCDbmpxx(b_graph2,   	35,192);	
	if(datCRE<1)	datCRE=1;
	if(datMDA<1)	datMDA=1;
	
	//sprintf(top_str, "MCR [%4d]",q);//umol/mmol",q);
	//sprintf(top_str, "MCR [%s]",mcr_tb[datCRE-1][datMDA-1]);//umol/mmol",q);
	sprintf(top_str, "MCR [      ]");
	for(i=0;i<6;i++){
		top_str[i+5]=mcr_tb[datCRE-1][datMDA-1][i];
	}
	msg20(45,160,BLK,top_str);
	msg16(175,165,BLK,"umol/g");

	q=mcr_val_1[datCRE-1][datMDA-1];
	z=q;
	n=cal_point(z);
	
	draw_box(170,n,200,390,col,RAST1);
	
	msg16(45,89+20,BLK, "SEQ.NO:");
	sprintf(top_str, "%4d",sno);
	msg16(45+58,89+20,BLK, top_str);
	make_date_1(MEASyear, MEASmonth, MEASday);
	msg16(45,89+16+20,BLK,"Date:");
	msg16(45+50,89+16+20,BLK,char_buff);
	time_copy_adj(MEAShour, MEASmin, MEASsec);
	msg16(45,89+16+16+20,BLK,"Time:");
	msg16(45+50,89+16+16+20,BLK,top_str);
}

/*void out_result(void){
u32 i, j, k, m, n, q;
char str[32];
	draw_box(X_ST,Y_ST,X_ST+X_LN,Y_ST+Y_LN,CYN,RAST1);
	draw_box(X_ST,Y_ST,X_ST+X_LN,Y_ST+Y_LN,CYN,RAST0);
	for(i=0;i<14;i++) line_x(X_ST,Y_ST+(i*Y_FF),X_LN,W_XY,WHT,RAST1);

	msg16( T_ST+30, Y_ST+3,	BLK, "Arb.");
	if(UNIT_FLAG==UNIT_CONV)	msg16( T_ST+30+68, Y_ST+3,	BLK, "Conv.");
	else msg16( T_ST+30+68, Y_ST+3,	BLK, "Si.");
	msg16( T_ST+30+68+68, Y_ST+3,	BLK, "Unit");
	i=0;
	q=0;
	while(1){
		if(buffRESULT[i]==0) break;
		else if(buffRESULT[i]==0x0a) q++;
		i++;
	}
	k=0;
//	for(j=0;j<STRIP_INFO[0];j++){
	for(j=0;j<q;j++){
		k = (j*OFF_A)+4;
		n = buffRESULT[k-3];
		k+=1;
		for(m=0;m<3;m++) str[m]=buffRESULT[k++];	str[m]=0;
		k+=1;
		msg16( T_ST, 			Y_ST+3+(Y_FF*(j+1)),	BLK, str);
		k += 4;
		for(m=0;m<8;m++) str[m]=buffRESULT[k++];	str[m]=0;		//arb
//printf0("\n PLUS_FLAG : %d", PLUS_FLAG);
		if(PLUS_FLAG){
			if((n==' ')||(!MARK_FLAG)){
				msg16( T_ST+30, 	Y_ST+3+(Y_FF*(j+1)),	BLU, str);
			}else{
				msg16( T_ST+30, 	Y_ST+3+(Y_FF*(j+1)),	RED, str);
			}
		}
		for(m=0;m<8;m++) str[m]=buffRESULT[k++];	str[m]=0;		//conv or si
		if((n==' ')||(!MARK_FLAG)){
			msg16( T_ST+30+68, 	Y_ST+3+(Y_FF*(j+1)),	BLU, str);
		}else{
			msg16( T_ST+30+68, 	Y_ST+3+(Y_FF*(j+1)),	RED, str);
		}
		for(m=0;m<8;m++) str[m]=buffRESULT[k++];	str[m]=0;		//unit
		msg16( T_ST+30+68+68,  	Y_ST+3+(Y_FF*(j+1)),	YEL, str);
	}
	line_y(X_ST,Y_ST,Y_LN,W_XY,WHT,RAST1);
	line_y(T_ST+25,Y_ST,Y_LN,1,WHT,RAST1);
	line_y(T_ST+25+68,Y_ST,Y_LN,1,WHT,RAST1);
	line_y(T_ST+25+68+68,Y_ST,Y_LN,1,WHT,RAST1);
	line_y(X_ST+X_LN-2,Y_ST,Y_LN,W_XY,WHT,RAST1);
}*/

/************************************************************************/
void out_no_strip(void){
	draw_box(X_ST,Y_ST+70,X_ST+X_LN,Y_ST+170,CYN,RAST0);
	draw_box(X_ST,Y_ST+70,X_ST+X_LN,Y_ST+170,CYN,RAST1);
//	draw_box(X_ST,Y_ST,X_ST+X_LN,Y_ST+Y_LN,CYN,RAST0);
	line_x(X_ST,Y_ST+70,X_LN,W_XY,WHT,RAST1);
	line_x(X_ST,Y_ST+170,X_LN,W_XY,WHT,RAST1);
	line_y(X_ST,Y_ST+70,100,W_XY,WHT,RAST1);
	line_y(X_ST+X_LN-2,Y_ST+70,100,W_XY,WHT,RAST1);
	if(ERROR_NO==2){
		msg32( X_ST+40, Y_ST+100, RED, "No Strip.");
	}else{
		msg24( X_ST+30, Y_ST+100, RED, "Loader Error");
	}
	key_buzzer3();
}
/************************************************************************/
void out_wrong_strip(void){
	draw_box(X_ST,Y_ST+70,X_ST+X_LN,Y_ST+170,CYN,RAST0);
	draw_box(X_ST,Y_ST+70,X_ST+X_LN,Y_ST+170,CYN,RAST1);
//	draw_box(X_ST,Y_ST,X_ST+X_LN,Y_ST+Y_LN,CYN,RAST0);
	line_x(X_ST,Y_ST+70,X_LN,W_XY,WHT,RAST1);
	line_x(X_ST,Y_ST+170,X_LN,W_XY,WHT,RAST1);
	line_y(X_ST,Y_ST+70,100,W_XY,WHT,RAST1);
	line_y(X_ST+X_LN-2,Y_ST+70,100,W_XY,WHT,RAST1);
	msg24( X_ST+30, Y_ST+90, RED,  "   Wrong ");
	msg24( X_ST+30, Y_ST+120, RED, "Strip Type");
	key_buzzer3();
}
/************************************************************************/void dsp_novalid(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_rest5, 20,40);		//?????????????			

	msg32( 60, 240, RED, "No data!!!");

	LCDbmpxx(b_memo3,  22,428);					
	LCDbmpxx(b_view5, 139,428);					
}
/************************************************************************/
void blink_send(void){
	erase_box( 102-1, 428, 102+80, 428+40);
	delay_ms(300);
	LCDbmpxx(b_send4, 102,428);						
	delay_ms(300);
}
/************************************************************************/
void blink_send_no(void){
	erase_box( 139-1, 428, 139+100, 428+40);
	delay_ms(300);
	LCDbmpxx(b_send3, 139,428);					
	delay_ms(300);
}
/************************************************************************/
void blink_delete(u16 dly){
	erase_box( 182-1, 428, 182+80, 428+40);
	delay_ms(300);
	LCDbmpxx(b_del4,  182,428);
	delay_ms(dly);
}
/************************************************************************/
/*****************************************************************************/
#define PS_Y	36
void dsp_pr_set(void){
u16 i, k;	
//u8 *ip;
	printf0("\n Para:%d",pr_no);
	for(i=0;i<7;i++){
//		ip = (u8 *)item_string[pr_no+1]+4;
//		for(j=0;j<3;j++) top_str[j] = *ip++;
//		top_str[j++]=' ';
//		top_str[j++]=i+'1';
//		top_str[j++]=0;
		if(UNIT_FLAG==UNIT_CONV){
			for(k=0;k<8;k++) top_str[k] = co_item_ram[pr_no+1][i+1][k];
		}else{
			for(k=0;k<8;k++) top_str[k] = si_item_ram[pr_no+1][i+1][k];
		}
		top_str[k] = 0;
		msg20(40,180+(PS_Y*i),BLK,top_str);	

//		if((pr_no==4)||(pr_no==10)||(pr_no==14)){
		if((pr_no==4)||(pr_no==10)){
			k = PARA_NEW[pr_no][i][2];
		}else{
			if(i<6){
				k = PARA_NEW[pr_no][i+1][2];
			}else{
				k = 0;
			}
		}
		printf0(" [%d:%d]",i, k);
		sprintf(top_str, "%d", k);
		msg20(170,180+(PS_Y*i),BLK,top_str);	
	}
}
/*****************************************************************************/
void blink_endno(u16 eno){
u8 b1, b2;
	for(b2=0;b2<3;b2++){
		LCDbmpxx(b_end, 20,88+36);					
		delay_ms(300);
		b1 = put_seqno(eno);
		msg20(130+b1,92+36,RED,top_str);
		delay_ms(300);
	}
}
/*****************************************************************************/
void blink_srtno(u16 eno){
u8 b1, b2;
	for(b2=0;b2<3;b2++){
		LCDbmpxx(b_start3, 20,88);					
		delay_ms(300);
		b1 = put_seqno(eno);
		msg20(130+b1,92,RED,top_str);	
		delay_ms(300);
	}
}
/************************************************************************/
void dsp_prog_ng(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	msg32(55,220,RED,"Writing NG!");
}
/************************************************************************/
void dsp_prog_ok(void){
	LCDbmpxb(b_base,0,0);					
	out_time();
	msg32(55,220,BLU,"Writing OK!");
}
/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/
void dsp_pass(void){
u8 b1;
	LCDbmpxb(b_base,0,0);					
	out_time();
	LCDbmpxx(b_chge2, 20,40);					

//	LCDbmpxx(b_seq   ,20,88);					
	draw_box(20,88,20+230,88+32,CYN,RAST1);
	draw_box(20,88,20+230,88+32,CYN,RAST0);
/*	line_x(20,    88,   20+230,88,   WHT,RAST1);
	line_x(20,    88,   20    ,88+32,WHT,RAST1);
	line_x(20,    88+32,20+230,88+32,WHT,RAST1);
	line_x(20+230,88   ,20+230,88+32,WHT,RAST1);
*/	
	b1 = put_seqno(tmp_seqno);
	msg20(90+b1,92,BLK,top_str);	
//	LCDbmpxx(b_number,20,88+44);					
	LCDbmpxx(b_num_c,20,88+44);					

//	LCDbmpxx(b_view1, 30,428);		//??????????????
	LCDbmpxx(b_sett2,  22,428);					
	LCDbmpxx(b_view5, 139,428);					
}



u32 cal_point(u32 qq){
	u32 r;
	if(qq>2500){	
		//k=4;
		r=190.0+(50.0-((qq - 2500) * (50.0 / 27500.0)));
		col=RED;
	}else{
		if(qq>999){	
			//k=3;
			r=190.0+(100.0 - ((qq - 1000.0) * (50.0 / 1500.0)));
			col=MGT;
		}else{
			if(qq>200){	
			//	k=2;
				r=190.0+(150.0 - ((qq - 200) * (50.0 / 800.0)));
				col=YEL;
			}else{
			//	k=1;
				r=190.0+(200.0-(qq * 50.0 / 200.0));
				col=BLU;
			}
		}
	}

/*	u32 r;
	if(qq>300){	
		//k=4;
		r=190.0+(50.0-((qq - 300.0) * (50.0 / 3033.0)));
		col=RED;
	}else{
		if(qq>100){	
			//k=3;
			r=190.0+(100.0 - ((qq - 100.0) * (50.0 / 200.0)));
			col=MGT;
		}else{
			if(qq>23){	
			//	k=2;
				r=190.0+(150.0 - ((qq - 23.0) * (50.0 / 77.0)));
				col=YEL;
			}else{
			//	k=1;
				r=190.0+(200.0-(qq * 100.0 / 23.0));
				col=BLU;
			}
		}
	}
*/	return(r);
}


u32 cal_point_1(u32 qq){
	u32 r;
	if(qq>3000){	
		//k=3;
		r=240.0+(50.0 - ((qq - 3000.0) * (50.0 / 12000.0)));
		col=MGT;
	}else{
		if(qq<300){	
		//	k=1;
			r=240.0+(150.0-(qq * 100.0 / 300.0));
			col=BLU;
		}else{
		//	k=2;
			r=240.0+(100.0 - ((qq - 300) * (50.0 / 2700.0)));
			col=YEL;
		}
	}
	return(r);
}


void make_date_1(u8 year, u8 month, u8 day){
	sprintf(charbuff, "%2d%2d%2d",year,month,day);
	sprintf(char_buff, "2000/00/00");
	char_buff[2]=charbuff[0];
	char_buff[3]=charbuff[1];
	char_buff[5]=charbuff[2];
	char_buff[6]=charbuff[3];
	char_buff[8]=charbuff[4];
	char_buff[9]=charbuff[5];
}
void out_result_2(u16 sno){
u32 i, j, o, q, k,n,m,alb, cre,z,g;
char str[32];

//u32  j, k, m, n, q, g;
//u8 i;
//float z;
//char str[32];
	LCDbmpxx(b_box8,   	20,85);	

	msg16(22+50,89,BLK, "SEQ.:");
	sprintf(top_str, "%4d",sno);
	msg16(22+40+50,89,BLK, top_str);

	readAllRTC();

	date_copy_adj(year, month, day);
	msg16(22+50,    89+16,BLK,"Date:");
	msg16(22+58+50, 89+16,BLK,top_str);

	time_copy_adj(hour, min, sec);
	msg16(22+50,    89+16+16,BLK,"Time:");
	msg16(22+58+50, 89+16+16,BLK,top_str);

//	LCDbmpxx(b_graph3,   	35,192+48);	
	if(datCRE<1)	datCRE=1;
	if(datALB<1)	datALB=1;

	o=8;

	i=0;
	q=0;
	while(1){
		if(buffRESULT[i]==0) break;
		else if(buffRESULT[i]==0x0a) q++;
		i++;
	}
	
	if((datCRE>=0)&&(datALB>=0)){
		LCDbmpxx(b_macre, 30,146);		//?????????????			
		alb=((datALB-1)*40)+80;
		cre=((datCRE-1)*32)+178;
		z=alb_item[datCRE-1][datALB-1];
		if(z==1){
			LCDbmpxx(b_r1, alb,cre);		//?????????????			
		}else if(z==2){
			LCDbmpxx(b_r2, alb,cre);	
		}else if(z==3){
			LCDbmpxx(b_r3, alb,cre);	
		}else{
			LCDbmpxx(b_r2, alb,cre);		//?????????????			
		}
		k=0;
		for(j=0;j<3;j++){
			k = (j*OFF_A)+4;
			n = buffRESULT[k+1];
			k+=1;
			for(m=0;m<3;m++) str[m]=buffRESULT[k++];	
			str[m]=' '; 	m++;
			str[m]=':'; 	m++;
			str[m]=' '; 	m++;
			k+=1;
			k += 4;
			for(g=0;g<8;g++) k++;
			for(g=0;g<8;g++){
				str[m]=buffRESULT[k++];	
				m++;
			}
			if(datALB==1&&datCRE==1&&j==2){
			}else{
				for(g=0;g<8;g++){
					str[m]=buffRESULT[k++];
					m++;
				}
			}
			str[m]=0;		//unit
			msg16( 50,  330+(Y_FF_2*(j+1)),	BLK, str);
		}

		for(m=0;m<13;m++) str[m]=albumin_tb_1[z][m];
		str[m]=0;		//arb
		sprintf(top_str, "      %s",str);
		msg16(50,330+(Y_FF_2*(j+1)),BLK, top_str);
	}
	
/*	k=0;
	for(j=0;j<2;j++){
		k = (j*OFF_A)+4;
		n = buffRESULT[k+1];
		k+=1;
		for(m=0;m<3;m++) str[m]=buffRESULT[k++];	
		str[m]=' '; 	m++;
		str[m]=':'; 	m++;
		str[m]=' '; 	m++;
		k+=1;
		k += 4;
		for(g=0;g<8;g++) k++;
		for(g=0;g<8;g++){
			str[m]=buffRESULT[k++];	
			m++;
		}
		if(datALB==1&&datCRE==1&&j==2){
		}else{
			for(g=0;g<8;g++){
				str[m]=buffRESULT[k++];
				m++;
			}
		}
		str[m]=0;		//unit
		msg16( 22+40,  95+16+16+(16*(j+1)),	BLK, str);
	}
	if(datALB==1&&datCRE==1){
		sprintf(top_str, "      Retest");
		msg16(22+40,95+16+16+(16*(j+1)),BLK,top_str);
	}else{
		//sprintf(top_str, "A:C [       ]mg/g");
		if(UNIT_FLAG==UNIT_SI){
			sprintf(top_str, "ACR :         mg/mmol");
		}else{
			sprintf(top_str, "ACR :         mg/g");
		}
		for(i=0;i<7;i++){
			top_str[i+7]=acr_tb_1[datCRE-1][datALB-1][i];
		}
		msg16(22+40,95+16+16+(16*(j+1)),BLK,top_str);
	
		j++;
		
		i=alb_item[datCRE-1][datALB-1];
		for(m=0;m<13;m++) str[m]=albumin_tb_1[i][m];
		str[m]=0;		//arb
		sprintf(top_str, "      %s",str);
		msg16(22+40,95+16+16+(16*(j+1)),BLK, top_str);
	
	
		q=acr_val_1[datCRE-1][datALB-1];
		z=q;
		n=cal_point_1(z);
		
		draw_box(170,n,200,390,col,RAST1);
	}

	msg16(22+40,89,BLK, "SEQ.:");
	sprintf(top_str, "%4d",sno);
	msg16(22+40+48,89,BLK, top_str);

	readAllRTC();

	date_copy_adj(year, month, day);
	msg16(22+40,    89+16,BLK,"Date:");
	msg16(22+40+48, 89+16,BLK,top_str);

	time_copy_adj(hour, min, sec);
	msg16(22+40,    89+16+16,BLK,"Time:");
	msg16(22+40+48, 89+16+16,BLK,top_str);
*/

/*u32 i, j, o, q, k,n,m,alb, cre,z,g;
char str[32];
	LCDbmpxx(b_box8,   	20,85);	
	LCDbmpxx(b_macre, 30,146);		//?????????????			
	alb=((datALB-1)*40)+80;
	cre=((datCRE-1)*32)+178;
	z=alb_item[datCRE-1][datALB-1];
	if(z==1){
		LCDbmpxx(b_r1, alb,cre);		//?????????????			
	}else if(z==2){
		LCDbmpxx(b_r2, alb,cre);	
	}else if(z==3){
		LCDbmpxx(b_r3, alb,cre);	
	}else{
		LCDbmpxx(b_r2, alb,cre);		//?????????????			
	}
	k=0;
	for(j=0;j<3;j++){
		k = (j*OFF_A)+4;
		n = buffRESULT[k+1];
		k+=1;
		for(m=0;m<3;m++) str[m]=buffRESULT[k++];	
		str[m]=' '; 	m++;
		str[m]=':'; 	m++;
		str[m]=' '; 	m++;
		k+=1;
		k += 4;
		for(g=0;g<8;g++) k++;
		for(g=0;g<8;g++){
			str[m]=buffRESULT[k++];	
			m++;
		}
		if(datALB==1&&datCRE==1&&j==2){
		}else{
			for(g=0;g<8;g++){
				str[m]=buffRESULT[k++];
				m++;
			}
		}
		str[m]=0;		//unit
		msg16( 50,  330+(16*(j+1)),	BLK, str);
	}
	for(m=0;m<13;m++) str[m]=albumin_tb_1[z][m];
	str[m]=0;		//arb
	sprintf(top_str, "      %s",str);
	msg16(50,330+(16*(j+1)),BLK, top_str);

*/
}

void out_result_3(u16 sno){
u32  j, k, m, n, q, g, alb;
u8 i;
float z;
char str[32];

	LCDbmpxx(b_box8,   	20,85);	
	LCDbmpxx(b_nicot, 35,205);		//?????????????			
	alb=((4-datLDH)*50)+210;
	LCDbmpxx(b_nic_r, 140,alb);		//?????????????			
	j=1;
	k = (j*OFF_A)+4;
	n = buffRESULT[k+1];
	k+=1;
	for(m=0;m<3;m++) str[m]=buffRESULT[k++];	
	str[m]=' '; 	m++;
	str[m]=':'; 	m++;
	str[m]=' '; 	m++;
	k+=1;
	//msg16(70, 360+(Y_FF*(j+1)),	BLK, str);
	k += 4;
	for(g=0;g<8;g++) k++;
	for(g=0;g<8;g++){
		str[m]=buffRESULT[k++];	
		m++;
	}
	//msg16( T_ST+80,360+(Y_FF*(j+1)), BLU, str);
	for(g=0;g<8;g++){
		str[m]=buffRESULT[k++];
		m++;
	}
	str[m]=0;		//unit
	msg20( 45,  89+16+16+16+30, BLK, str);	

	msg16(45,89+20,BLK, "SEQ.NO:");
	sprintf(top_str, "%4d",sno);
	msg16(45+58,89+20,BLK, top_str);
	make_date_1(MEASyear, MEASmonth, MEASday);
	msg16(45,89+16+20,BLK,"Date:");
	msg16(45+50,89+16+20,BLK,char_buff);
	time_copy_adj(MEAShour, MEASmin, MEASsec);
	msg16(45,89+16+16+20,BLK,"Time:");
	msg16(45+50,89+16+16+20,BLK,top_str);

}

void out_result_4(u16 sno){
u32  j, k, m, n, q, g, alb;
u8 i;
float z;
char str[32];

	LCDbmpxx(b_box8, 20, 85);	
	LCDbmpxx(b_graph4, 35,235);		//?????????????			
	if(datBHB>3){
		datBHB=3;
	}
	alb=((4-datBHB)*50)+195;
	LCDbmpxx(b_bhb_r, 160,alb);		//?????????????			
	j=0;
	k = (j*OFF_A)+4;
	n = buffRESULT[k+1];
	k+=1;
	for(m=0;m<3;m++) str[m]=buffRESULT[k++];	
	str[m]=' '; 	m++;
	str[m]=':'; 	m++;
	str[m]=' '; 	m++;
	k+=1;
	//msg16(70, 360+(Y_FF*(j+1)),	BLK, str);
	k += 4;
	for(g=0;g<12;g++) k++;
	for(g=0;g<4;g++){
		str[m]=buffRESULT[k++];	
		m++;
	}
/*	for(g=0;g<8;g++) k++;
	for(g=0;g<8;g++){
		str[m]=buffRESULT[k++];	
		m++;
	}*/
	//msg16( T_ST+80,360+(Y_FF*(j+1)), BLU, str);
	for(g=0;g<8;g++){
		str[m]=buffRESULT[k++];
		m++;
	}
	str[m]=0;		//unit
	msg20( 45,  89+16+16+16+30, BLK, str);	

	msg16(45,89+20,BLK, "SEQ.NO:");
	sprintf(top_str, "%4d",sno);
	msg16(45+58,89+20,BLK, top_str);
	make_date_1(MEASyear, MEASmonth, MEASday);
	msg16(45,89+16+20,BLK,"Date:");
	msg16(45+50,89+16+20,BLK,char_buff);
	time_copy_adj(MEAShour, MEASmin, MEASsec);
	msg16(45,89+16+16+20,BLK,"Time:");
	msg16(45+50,89+16+16+20,BLK,top_str);
}
