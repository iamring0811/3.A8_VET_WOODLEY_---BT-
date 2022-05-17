
#define	_TOUCH_C_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\init.h"
#include ".\inc\uart.h"
#include ".\inc\touch.h"
#include ".\inc\resm.h"
#include ".\inc\LCD.h"
#include ".\inc\EEP.h"
#include ".\inc\Meas.h"
#include ".\inc\flash.h"
#include ".\inc\com.h"
#include ".\inc\Sysadj.h"
#include ".\inc\dsp.h"

extern void com_msg(char *str);		//com.c

void tsw_color(void);
void tsw_turbi(void);
void tch_color(void);
void tch_turbi(void);
void tsw_clean(void);
void start_clean(void);		//Load가 있을 때 밀어낸다
void end_clean(void);		//Load가 없을 때 끌어들인다.
void tch_pr_set(void);
void tch_pr_dgt(void);
void blink_clean(void);
void blink_clean_1(void);
void backup_para(void);

u16 tcode;
u16	blk_pol, blk_dly;

/*****************************************************************************/
/*****************************************************************************/
#define  XT_OFFSET	30
u16 tch_check(u16 *tb, u8 tno){
u16 a, b, c;
	for(a=0;a<tno;a++){
		b = tb[(a*4)+0];
		c = tb[(a*4)+1];
		if((tchx>b)&&(tchx<c)){
			b = tb[(a*4)+2] - XT_OFFSET;
			c = tb[(a*4)+3] - XT_OFFSET;
			if((tchy>b)&&(tchy<c)) break;
		}
	}
	return(a);	
}
/********************** SW READY *********************/
void tch_standby(void){
	tcode=tch_check(tch_standby_tb, tch_standby_no);
//printf0("\n Ready Code:%2d", tcode); tcode=99;
	switch(tcode){
		case 0: tsw_strip(); 	break;
		case 1: tsw_seqno(); 	break;
		case 2: tsw_idno(); 	break;
		case 3: tsw_color(); 	break;
		case 4: tsw_turbi(); 	break;
		case 5: tsw_start();	break;
		case 6: tsw_menu(); 	break;
		case 7: tsw_clean(); 	break;
		case 8: tsw_poff();		break;
	}
}

/**************************************************************/
void tch_clean(void){
	tcode=tch_check(tch_standby_tb, tch_standby_no);
	if(tcode==6){
		DOUBLE_FLAG=1;
		write24(DOUBLE_ADR, DOUBLE_FLAG);
		write_id_chksum();
		key_buzzer();
		end_clean();
	}
	if(tcode==7){
		DOUBLE_FLAG=0;
		write24(DOUBLE_ADR, DOUBLE_FLAG);
		write_id_chksum();
		key_buzzer();
		end_clean();
	}
}
/**************************************************************/
void tsw_strip(void){
	key_buzzer();
	dsp_strip_sel();
	state=M_STRIP;
	if(STRIPNO>7){
		tmp_strip = STRIPNO-7;
	}else{
		tmp_strip = 0;
	}
	sel_strip = STRIPNO;
	cursor = 0;
	dsp_strip8line();
}


/*******************************

/**************************************************************/
void tsw_seqno(void){
u8 b1;
	key_buzzer();
	dsp_seq_set();
	tmp_seqno=SEQ_NO;
	state=M_SEQNO;
	b1 = put_seqno(SEQ_NO);
	msg20(128+b1,92,BLK,top_str);	
	cursor = 0;
}
/**************************************************************/
void tsw_idno(void){
int i;
	key_buzzer();
	dsp_idno_set();
	for(i=0;i<MAX_USERID;i++) tmp_idno[i] = USER_ID[i];
	tmp_idno[i] = 0x00;
	state=M_IDNO;
//	msg20(128,92,BLK,tmp_idno);	
	msg16(128-6,92+5,BLK,tmp_idno);	
	cursor = 0;
//	cursor_20(128, 92, cursor, tmp_idno); 
	cursor_16(128-6, 92, cursor, tmp_idno); 
}
/**************************************************************/
void tsw_color(void){
	key_buzzer();
	dsp_color_sel();
	state=M_COLOR;
}
/**************************************************************/
void tsw_turbi(void){
	key_buzzer();
	dsp_turbi_sel();
	state=M_TURBI;
}
/**************************************************************/
void tsw_start(void){
	key_buzzer();
//Uart_Init(9600,UART1);		//블루투스
//printf0("\n 9600 ");
//init_blu();
//return;	
	move_strip_info();
	state=M_MEASURE;
	LED_GRN_OFF();
	LED_RED_ON();
	dsp_measure();
	run_measure();
//	view_result();
	printf0("\n End Measure %2x", state);
}
/**************************************************************/
void tsw_menu(void){		//메뉴화면표시
	key_buzzer();
//Uart_Init(38400,UART1);		//블루투스
//printf0("\n 38400 ");
//init_blu();
//return;	
	state=M_MENU;
	dsp_menu_home();
}

/******************************************************************************/
/********************** SW Strip *********************/
void tch_strip(void){
u32 i;		
	tcode=tch_check(tch_strip_tb, tch_strip_no);
//printf0("\n tcode:%d", tcode);	tcode=99;
	if(tcode<tch_strip_no) key_buzzer();
	if(tcode==0){
		if(tmp_strip>0){ tmp_strip--; dsp_strip8line(); }
	}else if(tcode==1){
		if(tmp_strip<(MAX_STRIPNO-8)){ tmp_strip++; dsp_strip8line(); }
	}else if(tcode<10){
		if(tmp_strip + tcode-2 >= MAX_STRIPNO){
			blink_strip(MAX_STRIPNO-tmp_strip-1);
			sel_strip = MAX_STRIPNO-1;
			dsp_strip8line();
		}else{
			blink_strip(tcode-2);
			sel_strip = tmp_strip+(tcode-2);
			dsp_strip8line();
		}
	}else if(tcode==10){
		STRIPNO = sel_strip;
		state=M_STANDBY;
		dsp_standby();
		write24w(STRIPNO_ADR, STRIPNO); 
		write_id_chksum();
		printf0("\n STRIPNO:%d",STRIPNO);
		move_strip_info();
		savcmd3('*');
		savcmd3(0x67);
		i = bin2hexasc4(STRIPNO);
		savcmd3(i>>8);
		savcmd3(i>>0);
		savcmd3(URLF);
		key_buzzer();
	}
}
/**************************************************************/
void tch_seqno(void){
u8 b1;	
	tcode=tch_check(tch_digit_tb, tch_digit_no);
//printf0("\n tcode:%d", tcode);	tcode=99;
	if(tcode<tch_digit_no) key_buzzer();
	if(tcode<10){						//'0'-'9'
		if(tmp_seqno>=MAX_RECORD) tmp_seqno=0;
		tmp_seqno = (tmp_seqno*10) + tcode;
		if(tmp_seqno>MAX_RECORD){
			blink_seqno();
//			tmp_seqno = tcode;
			tmp_seqno = MAX_RECORD;
		}
//		msg20(130,92,BLK,"            ");	
		erase_box0(130-2,92-1,130+100,92+30);
		b1 = put_seqno(tmp_seqno);
		msg20(130+b1,92,BLK,top_str);	
	}else if(tcode==10){				//Down==>back space
		tmp_seqno /= 10;
		b1 = put_seqno(tmp_seqno);
//		msg20(130,92,BLK,"            ");	
		erase_box0(130-2,92-1,130+100,92+30);
		msg20(130+b1,92,BLK,top_str);	
	}else if(tcode==11){				//Up==>clear
		tmp_seqno = 0;
		b1 = put_seqno(tmp_seqno);
//		msg20(130,92,BLK,"            ");	
		erase_box0(130-2,92-1,130+100,92+30);
		msg20(130+b1,92,BLK,top_str);	
	}else if(tcode==12){				//Test
		if(tmp_seqno>MAX_RECORD) tmp_seqno = MAX_RECORD;
		if(tmp_seqno>0){
			SEQ_NO = tmp_seqno;
			blink_seqno();
			write24w(SEQNO_ADR, SEQ_NO);
			write_id_chksum();
		}else{
			tmp_seqno =	SEQ_NO;
			blink_seqno();
		}
		state=M_STANDBY;
		dsp_standby();
	}
}
/**************************************************************/
void tch_idno(void){
int i;	
	tcode=tch_check(tch_idset_tb, tch_idset_no);
	if(tcode<tch_idset_no) key_buzzer();
	if(tcode<10){						//'0'-'9'
		tmp_idno[cursor] = tcode+'0';
		if(cursor<15) cursor++;
		erase_box0(128-7,92-1,130+120,92+30);
//		msg20(128,92,BLK,tmp_idno);	
		msg16(128-6,92+5,BLK,tmp_idno);	
//		cursor_20(128, 92, cursor, tmp_idno);
		cursor_16(128-6, 92, cursor, tmp_idno);
	}else if(tcode<35){		//'A'-'Y'
		tmp_idno[cursor] = (tcode-10)+'A';
		if(cursor<15) cursor++;
		erase_box0(128-7,92-1,130+120,92+30);
//		msg20(128,92,BLK,tmp_idno);	
		msg16(128-6,92+5,BLK,tmp_idno);	
//		cursor_20(128, 92, cursor, tmp_idno);
		cursor_16(128-6, 92, cursor, tmp_idno);
	}else if((tcode==36)||(tcode==37)||(tcode==38)){		//'Z','.','-'
		if(tcode==36) tmp_idno[cursor] = 'Z';
		else if(tcode==37) tmp_idno[cursor] = '.';
		else tmp_idno[cursor] = '-';
		if(cursor<15) cursor++;
		erase_box0(128-7,92-1,130+120,92+30);
//		msg20(128,92,BLK,tmp_idno);	
		msg16(128-6,92+5,BLK,tmp_idno);	
//		cursor_20(128, 92, cursor, tmp_idno);
		cursor_16(128-6, 92, cursor, tmp_idno);
	}else if(tcode==35){				//Down==>back space
		if(cursor){ 
			if((cursor==15)&&(tmp_idno[15]!=' ')){
				tmp_idno[cursor]=' '; 
			}else{				 
				cursor--; 
				tmp_idno[cursor]=' '; 
			}
			erase_box0(128-7,92-1,130+120,92+30);
//			msg20(128,92,BLK,tmp_idno);	
			msg16(128-6,92+5,BLK,tmp_idno);	
//			cursor_20(128, 92, cursor, tmp_idno); 
			cursor_16(128-6, 92, cursor, tmp_idno);
		}
	}else if(tcode==39){				//Up==>Clear 
		for(i=0;i<MAX_USERID;i++)tmp_idno[i]=' ';
		tmp_idno[i]=0x00;
		cursor=0;
		erase_box0(128-7,92-1,130+120,92+30);
//		msg20(128,92,BLK,tmp_idno);	
		msg16(128-6,92+5,BLK,tmp_idno);	
//		cursor_20(128, 92, cursor, tmp_idno); 
		cursor_16(128-6, 92, cursor, tmp_idno);
	}else if(tcode==40){				//Test
		blink_idno();
		for(i=0;i<MAX_USERID;i++) USER_ID[i] = tmp_idno[i];
		state=M_STANDBY;
		dsp_standby();
	}
}
/**************************************************************/
void tch_color(void){
	tcode=tch_check(tch_color_tb, tch_color_no);
	if(tcode<tch_color_no) key_buzzer();
//printf0("\n color:%d", tcode); tcode=99;
	if(tcode<9){
		COLOR_NO = tcode;
		blink_color();
		dsp_color_no();
	}else if(tcode==9){
		state=M_STANDBY;
		dsp_standby();
//		write24w(COLOR_ADR, COLOR_NO);
//		write_id_chksum();
	}
}
/**************************************************************/
void tch_turbi(void){
	tcode=tch_check(tch_color_tb, tch_color_no);
printf0("\n turbi:%d", tcode);
	if(tcode<6){
		key_buzzer();
		TURBI_NO = tcode;
		blink_turbi();
		dsp_turbi_no();
	}else if(tcode==9){
		key_buzzer();
		state=M_STANDBY;
		dsp_standby();
//		write24w(TURBI_ADR, TURBI_NO);
//		write_id_chksum();
	}
}
/********************** SW MEASURE *********************/
void tch_measure(void){
	tcode=tch_check(tch_measure_tb, tch_measure_no);
	if(tcode==0){
		key_buzzer();
		state=M_STANDBY;
		dsp_standby();
	}
}
/**************************************************************/
void tch_menu(void){
	tcode=tch_check(tch_menu_tb, tch_menu_no);
	if(tcode<tch_digit_no) key_buzzer();
	switch(tcode){
		case 0:	
			setyear =year ;	setmonth=month;	setday  =day  ;
			sethour =hour ;	setmin  =min  ;	setsec  =sec  ;
			state=M_M_DATE;	 cursor=0; 
			dsp_date_time(); out_date_time();
		break;
		case 1:	state=M_M_CHK;	dsp_sys_chk(); 	 break;
		case 2:	state=M_M_MEM;	dsp_memory();	 break;
		case 3:	state=M_M_SET;	dsp_setting();	 break;
		case 4:	state=M_M_INFO;	dsp_sys_info();	 break;
		case 5: state=M_STANDBY; dsp_standby();  break;
	}
}
/******************************************************************************/
/********************** SW DATE *********************/
void tch_date(void){
int i;	
	tcode=tch_check(tch_digit2_tb, tch_digit2_no);
	if(tcode<tch_digit2_no) key_buzzer();
//printf0("\n tch_date:%d", tcode); tcode=99;
	if(tcode<10){						//'0'-'9'
		key_buzzer();
		switch(cursor){
			case 0:  setyear =(tcode*10)+(setyear%10);  break;
			case 1:  setyear =((setyear/10)*10)+tcode;  break;
			case 2:  setmonth=(tcode*10)+(setmonth%10); break;
			case 3:  setmonth=((setmonth/10)*10)+tcode; break;
			case 4:  setday  =(tcode*10)+(setday%10);   break;
			case 5:  setday  =((setday/10)*10)+tcode;   break;
			case 6:  sethour =(tcode*10)+(sethour%10);  break;
			case 7:  sethour =((sethour/10)*10)+tcode;  break;
			case 8:  setmin  =(tcode*10)+(setmin%10);   break;
			case 9:  setmin  =((setmin/10)*10)+tcode;   break;
			case 10: setsec  =(tcode*10)+(setsec%10);   break;
			case 11: setsec  =((setsec/10)*10)+tcode;   break;
		}
		if(cursor<11) cursor++;
		LCDbmpxx(b_date3, 20,88);					
		LCDbmpxx(b_time,  20,88+(36*1));					
		out_date_time();
	}else if(tcode==10){				//Down
		if(cursor){ 
			cursor--; 
//printf0("\t tch_left:%d", cursor);
			LCDbmpxx(b_date3, 20,88);					
			LCDbmpxx(b_time,  20,88+(36*1));					
			out_date_time();
		}
	}else if(tcode==11){				//Up
		if(cursor<11){ 
			cursor++;	
//printf0("\t tch_right:%d", cursor);
			LCDbmpxx(b_date3, 20,88);					
			LCDbmpxx(b_time,  20,88+(36*1));					
			out_date_time();
		}
	}else if(tcode==12){				//Date
		cursor=0;
		LCDbmpxx(b_date3, 20,88);					
		LCDbmpxx(b_time,  20,88+(36*1));					
		out_date_time();
	}else if(tcode==13){				//Time
		cursor=6;
		LCDbmpxx(b_date3, 20,88);					
		LCDbmpxx(b_time,  20,88+(36*1));					
		out_date_time();
	}else if(tcode==14){				//Menu
//		for(i=0;i<MAX_USERID;i++) USER_ID[i] = tmp_idno[i];
		if(setmonth==0)setmonth=1;
		if(setday==0)setday=1;
		if(setyear>99)  setyear=99;
		if(setmonth>12)	setmonth=12;
		dayInMonth(setyear,setmonth);
		if(setday>dayCount)	setday=dayCount;
		if(sethour>23)	sethour=23;
		if(setmin>59)	setmin=59;
		if(setsec>59)	setsec=59;
		sendAll_RTC();
		state=M_MENU;
		dsp_menu_home();
	}else if(tcode==15){				//Test
//		for(i=0;i<MAX_USERID;i++) USER_ID[i] = tmp_idno[i];
		if(setmonth==0)setmonth=1;
		if(setday==0)setday=1;
		if(setyear>99)  setyear=99;
		if(setmonth>12)	setmonth=12;
		dayInMonth(setyear,setmonth);
		if(setday>dayCount)	setday=dayCount;
		if(sethour>23)	sethour=23;
		if(setmin>59)	setmin=59;
		if(setsec>59)	setsec=59;
		sendAll_RTC();
		state=M_STANDBY;
		dsp_standby();
	}
}
/******************************************************************************/
void tch_syschk(void){
	tcode=tch_check(tch_syschk_tb, tch_syschk_no);
	if(tcode<tch_syschk_no) key_buzzer();
//printf0("\n tch_date:%d", tcode); tcode=99;
	if(tcode==0){
		system_check();
	}else if(tcode==1){
		state=M_MENU;
		dsp_menu_home();
	}else if(tcode==2){
		state=M_STANDBY;
		dsp_standby();
	}
}
/******************************************************************************/
void tch_memory(void){
u16 i;
	tcode=tch_check(tch_memory_tb, tch_memory_no);
	if(tcode<tch_memory_no) key_buzzer();
//printf0("\n tch_date:%d", tcode); tcode=99;
	if(SEQ_NO>1) i=SEQ_NO-1;
	else i=MAX_RECORD;
	if(tcode==0){
		state=M_M_VIEWNO;
		tmp_seqno = i;
		dsp_view_no();
	}else if(tcode==1){
		cursor=0;
		mem_srt=1;
		mem_end=i;
		dsp_send_no();
		state=M_M_SENDNO;
		blink_dur=0;
	}else if(tcode==2){
		mem_srt=1;
		mem_end=i;
		dsp_delete_no();
		state=M_M_DELNO;
		blink_dur=0;
	}else if(tcode==3){
		state=M_MENU;
		dsp_menu_home();
	}else if(tcode==4){
		state=M_STANDBY;
		dsp_standby();
	}
}
/******************************************************************************/
void tch_viewno(void){
u8 b1;	
	tcode=tch_check(tch_digitv_tb, tch_digitv_no);
	if(tcode<tch_digitv_no) key_buzzer();
	if(tcode<10){						//'0'-'9'
		tmp_seqno = (tmp_seqno*10) + tcode;
//		if(tmp_seqno>MAX_RECORD) tmp_seqno = tcode;
		if(tmp_seqno>3000) tmp_seqno = tcode;
		LCDbmpxx(b_seq   ,20,88);					
		b1 = put_seqno(tmp_seqno);
		msg20(130+b1,92,BLK,top_str);	
	}else if(tcode==10){				//Down==> back space
//		if(tmp_seqno) tmp_seqno--;
//		else tmp_seqno = MAX_RECORD;
//		b1 = put_seqno(tmp_seqno);
//		msg20(128+b1,92,BLK,top_str);	
		LCDbmpxx(b_seq   ,20,88);					
		tmp_seqno /= 10;
		b1 = put_seqno(tmp_seqno);
		msg20(128+b1,92,BLK,top_str);	
		blink_dur=0;
	}else if(tcode==11){				//Up==> clear
//		if(tmp_seqno<MAX_RECORD) tmp_seqno++;
//		else tmp_seqno = 1;
//		b1 = put_seqno(tmp_seqno);
//		msg20(128+b1,92,BLK,top_str);	
		LCDbmpxx(b_seq   ,20,88);					
		tmp_seqno = 0;
		b1 = put_seqno(tmp_seqno);
		msg20(128+b1,92,BLK,top_str);	
		blink_dur=0;
	}else if(tcode==12){				//memory		//vvvvvvvvvvvvvvvvvvvv
		state=M_M_MEM;
		dsp_memory();
	}else if(tcode==13){				//view data
		view_seqno = tmp_seqno;
		if(view_result_store(view_seqno)==OK){
			state=M_M_VIEW;
		}else{
			state=M_M_NODATA;
		}
	}
}
/********************** SW Display *********************/
void tch_view(void){
	tcode=tch_check(tch_view_tb, tch_view_no);
	if(tcode<tch_view_no) key_buzzer();
	if(tcode==0){
		state=M_M_MEM;
		dsp_memory();
	}else if(tcode==1){
		send_result_one(view_seqno);
		print_result(view_seqno);
		state=M_M_VIEWNO;
//		tmp_seqno=0;
		dsp_view_no();
	}else if(tcode==2){
		delete_result_one(view_seqno);
		state=M_M_VIEWNO;
//		if(SEQ_NO<MAX_RECORD) tmp_seqno=SEQ_NO+1;
		//if(tmp_seqno<MAX_RECORD) tmp_seqno += 1;
		//else tmp_seqno = 1;
		dsp_view_no();
	}
}
/******************************************************************************/
void tch_nodata(void){
	tcode=tch_check(tch_nodata_tb, tch_nodata_no);
	if(tcode<tch_nodata_no) key_buzzer();
	if(tcode==0){
		state=M_M_MEM;
		dsp_memory();
	}else if(tcode==1){
		state=M_M_VIEWNO;
//		tmp_seqno=0;
		dsp_view_no();
	}
}
/******************************************************************************/
void tch_sendno(void){
u8 b1;	
	tcode=tch_check(tch_digit3_tb, tch_digit3_no);
	if(tcode<tch_digit3_no) key_buzzer();
//printf0("\n tch_date:%d", tcode);
	if(tcode<10){						//'0'-'9'
		key_buzzer();
//	send_result_store(memory_srt, memory_end);
		if(!cursor){					//start
			if(mem_srt>=MAX_RECORD) mem_srt = 0;
			mem_srt = (mem_srt*10)+tcode;
			if(mem_srt>MAX_RECORD){
				blink_srtno(mem_srt);
				mem_srt = MAX_RECORD;
			}
			LCDbmpxx(b_start3, 20,88);					
			b1 = put_seqno(mem_srt);
			msg20(130+b1,92,BLK,top_str);	
			if(mem_srt>mem_end){
				blink_endno(mem_end);
				mem_end=mem_srt;
				LCDbmpxx(b_end,  20,88+(36*1));					
				b1 = put_seqno(mem_end);
				msg20(130+b1,92+36,BLK,top_str);
			}
		}else{							//end
			if(mem_end>=MAX_RECORD) mem_end = 0;
			mem_end = (mem_end*10)+tcode;
			if(mem_end>MAX_RECORD){
				blink_endno(mem_end);
				mem_end = MAX_RECORD;
			}
			LCDbmpxx(b_end,  20,88+(36*1));					
			b1 = put_seqno(mem_end);
			msg20(130+b1,92+36,BLK,top_str);
		}
		blink_dur=0;
	}else if(tcode==10){				//Down==>back space
		if(!cursor){ 
			mem_srt /= 10;
			LCDbmpxx(b_start3, 20,88);					
			b1 = put_seqno(mem_srt);
			msg20(130+b1,92,BLK,top_str);	
		}else{
			mem_end /= 10;
			LCDbmpxx(b_end, 20,88+36);					
			b1 = put_seqno(mem_end);
			msg20(130+b1,92+36,BLK,top_str);
		}
		blink_dur=0;
	}else if(tcode==11){				//Up==> clear
		if(!cursor){ 
			mem_srt = 0;
			LCDbmpxx(b_start3, 20,88);					
			b1 = put_seqno(mem_srt);
			msg20(130+b1,92,BLK,top_str);	
		}else{
			mem_end = 0;
			LCDbmpxx(b_end, 20,88+36);					
			b1 = put_seqno(mem_end);
			msg20(130+b1,92+36,BLK,top_str);
		}
		blink_dur=0;
	}else if(tcode==12){				//Start
		LCDbmpxx(b_start3, 20,88);					
		b1 = put_seqno(mem_srt);
		msg20(130+b1,92,BLK,top_str);	
		LCDbmpxx(b_end, 20,88+36);					
		b1 = put_seqno(mem_end);
		msg20(130+b1,92+36,BLK,top_str);
		blink_dur=0;
		cursor=0;
	}else if(tcode==13){				//End
		LCDbmpxx(b_start3, 20,88);					
		b1 = put_seqno(mem_srt);
		msg20(130+b1,92,BLK,top_str);	
		LCDbmpxx(b_end, 20,88+36);					
		b1 = put_seqno(mem_end);
		msg20(130+b1,92+36,BLK,top_str);
		blink_dur=0;
		cursor=1;
	}else if(tcode==14){				//Memory
		state=M_M_MEM;
		dsp_memory();
	}else if(tcode==15){				//Send
		LCDbmpxx(b_end, 20,88+36);					
		b1 = put_seqno(mem_end);
		msg20(130+b1,92+36,BLK,top_str);
		b1=send_result_store(mem_srt,mem_end);
//		mem_srt=mem_end=0;
		LCDbmpxx(b_start3, 20,88);					
		b1 = put_seqno(mem_srt);
		msg20(130+b1,92,BLK,top_str);	
	}
}
/******************************************************************************/
void tch_delno(void){
u8 b1;	
	tcode=tch_check(tch_digit3_tb, tch_digit3_no);
	if(tcode<tch_digit3_no) key_buzzer();
//printf0("\n tch_date:%d", tcode);
	if(tcode<10){						//'0'-'9'
		key_buzzer();
//	send_result_store(memory_srt, memory_end);
		if(!cursor){					//start
			if(mem_srt>=MAX_RECORD) mem_srt = 0;
			mem_srt = (mem_srt*10)+tcode;
			if(mem_srt>MAX_RECORD){
				blink_srtno(mem_srt);
				mem_srt = MAX_RECORD;
			}
			LCDbmpxx(b_start3, 20,88);					
			b1 = put_seqno(mem_srt);
			msg20(130+b1,92,BLK,top_str);	
			if(mem_srt>mem_end){
				blink_endno(mem_end);
				mem_end=mem_srt;
				LCDbmpxx(b_end,  20,88+(36*1));					
				b1 = put_seqno(mem_end);
				msg20(130+b1,92+36,BLK,top_str);
			}
		}else{							//end
			if(mem_end>=MAX_RECORD) mem_end = 0;
			mem_end = (mem_end*10)+tcode;
			if(mem_end>MAX_RECORD){
				blink_endno(mem_end);
				mem_end = MAX_RECORD;
			}
			LCDbmpxx(b_end,  20,88+(36*1));					
			b1 = put_seqno(mem_end);
			msg20(130+b1,92+36,BLK,top_str);
		}
		blink_dur=0;
	}else if(tcode==10){				//Down==>back space
		if(!cursor){ 
			mem_srt /= 10;
			LCDbmpxx(b_start3, 20,88);					
			b1 = put_seqno(mem_srt);
			msg20(130+b1,92,BLK,top_str);	
		}else{
			mem_end /= 10;
			LCDbmpxx(b_end, 20,88+36);					
			b1 = put_seqno(mem_end);
			msg20(130+b1,92+36,BLK,top_str);
		}
		blink_dur=0;
	}else if(tcode==11){				//Up==>clear
		if(!cursor){ 
			mem_srt = 0;
			LCDbmpxx(b_start3, 20,88);					
			b1 = put_seqno(mem_srt);
			msg20(130+b1,92,BLK,top_str);	
		}else{
			mem_end = 0;
			LCDbmpxx(b_end, 20,88+36);					
			b1 = put_seqno(mem_end);
			msg20(130+b1,92+36,BLK,top_str);
		}
		blink_dur=0;
	}else if(tcode==12){				//Start
		LCDbmpxx(b_start3, 20,88);					
		b1 = put_seqno(mem_srt);
		msg20(130+b1,92,BLK,top_str);	
		LCDbmpxx(b_end, 20,88+36);					
		b1 = put_seqno(mem_end);
		msg20(130+b1,92+36,BLK,top_str);
		cursor=0;
		blink_dur=0;
	}else if(tcode==13){				//End
		LCDbmpxx(b_start3, 20,88);					
		b1 = put_seqno(mem_srt);
		msg20(130+b1,92,BLK,top_str);	
		LCDbmpxx(b_end, 20,88+36);					
		b1 = put_seqno(mem_end);
		msg20(130+b1,92+36,BLK,top_str);
		cursor=1;
		blink_dur=0;
	}else if(tcode==14){				//Memory
		state=M_M_MEM;
		dsp_memory();
	}else if(tcode==15){				//Send
		LCDbmpxx(b_end, 20,88+36);					
		b1 = put_seqno(mem_end);
		msg20(130+b1,92+36,BLK,top_str);
		delete_result_store(mem_srt, mem_end);
		LCDbmpxx(b_start3, 20,88);					
		b1 = put_seqno(mem_srt);
		msg20(130+b1,92,BLK,top_str);	
	}
}
/******************************************************************************/
void tch_setting(void){
	tcode=tch_check(tch_setting_tb, tch_setting_no);
	if(tcode<tch_setting_no) key_buzzer();
//printf0("\n tch_date:%d", tcode);
	if(tcode==0){
		dsp_format();
		state=M_M_FORMAT;
	}else if(tcode==1){
		dsp_system();
		state=M_M_SYSSET;
	}else if(tcode==2){
		dsp_pass();
		state=M_M_PASS;
//		dsp_change1();
//		state=M_M_CHANGE;
	}else if(tcode==3){
		state=M_MENU;
		dsp_menu_home();
	}else if(tcode==4){
		state=M_STANDBY;
		dsp_standby();
	}
}
/******************************************************************************/
void tch_format(void){
	tcode=tch_check(tch_format_tb, tch_format_no);
	if(tcode<tch_format_no) key_buzzer();
//printf0("\n tch_date:%d", tcode);
	if(tcode==0){
		if(UNIT_FLAG==UNIT_CONV){
			UNIT_FLAG=UNIT_SI;
			LCDbmpxx(b_si,   30,96+28);
		}else{
			UNIT_FLAG=UNIT_CONV;
			LCDbmpxx(b_conv,   30,96+28);
		}
		write24w(UNIT_ADR, UNIT_FLAG);
		write_id_chksum();
	}else if(tcode==1){
		if(PLUS_FLAG){
			PLUS_FLAG=0;
			LCDbmpxx(b_disab,   30,96+28+72);					
		}else{
			PLUS_FLAG=1;
			LCDbmpxx(b_enab,   30,96+28+72);					
		}
		write24w(PLUS_ADR, PLUS_FLAG);
		write_id_chksum();
	}else if(tcode==2){
		if(MARK_FLAG){
			MARK_FLAG=0;
			LCDbmpxx(b_no,    30,96+28+72+72);					
		}else{
			MARK_FLAG=1;
			LCDbmpxx(b_yes,    30,96+28+72+72);					
		}
		write24w(MARK_ADR, MARK_FLAG);
		write_id_chksum();
	}else if(tcode==3){
//		state=M_MENU;
//		dsp_menu_home();
		state=M_M_SET;	
		dsp_setting();
	}else if(tcode==4){
		state=M_STANDBY;
		dsp_standby();
	}
}
void tch_sysset(void){
	tcode=tch_check(tch_format_tb, tch_format_no);
	if(tcode<tch_format_no) key_buzzer();
//printf0("\n tch_date:%d", tcode);
	if(tcode==0){
		if(SPEED_FLAG){
			SPEED_FLAG=0;
			LCDbmpxx(b_normal,   30,96+28);					
		}else{
			SPEED_FLAG=1;
			LCDbmpxx(b_fast,   30,96+28);					
		}
		write24(SPEED_ADR, SPEED_FLAG);
		write_id_chksum();
	}else if(tcode==1){
		if(SOUND_FLAG){
			SOUND_FLAG=0;
			LCDbmpxx(b_disab,   30,96+28+72);					
		}else{
			SOUND_FLAG=1;
			LCDbmpxx(b_enab,   30,96+28+72);					
			key_buzzer();
		}
		write24w(SOUND_ADR, SOUND_FLAG);
		write_id_chksum();
	}else if(tcode==2){
		if(BAUD_FLAG==0){
			BAUD_FLAG=1;
			LCDbmpxx(b_19200,    30,96+28+72+72);					
		}else if(BAUD_FLAG==1){
			BAUD_FLAG=2;
			LCDbmpxx(b_9600,    30,96+28+72+72);					
		}else{
			BAUD_FLAG=0;
			LCDbmpxx(b_38400,    30,96+28+72+72);					
		}
		write24w(BAUD_ADR, BAUD_FLAG);
		write_id_chksum();
		set_baud3();
	}else if(tcode==3){
//		state=M_MENU;
//		dsp_menu_home();
		state=M_M_SET;	
		dsp_setting();
	}else if(tcode==4){
		state=M_STANDBY;
		dsp_standby();
	}
}
void tch_change(void){
	tcode=tch_check(tch_change_tb, tch_change_no);
	if(tcode<tch_change_no) key_buzzer();
//printf0("\n tch_date:%d", tcode); tcode=99;
	if(tcode<17){
		state=M_M_C_SET;
		pr_no=tcode;
		dsp_change2();
		dsp_pr_set();
	}else if(tcode==17){
//		state=M_MENU;
//		dsp_menu_home();
		state=M_M_SET;	
		dsp_setting();
	}else if(tcode==18){
		state=M_STANDBY;
		dsp_standby();
	}
}
/******************************************************************************/
void tch_info(void){
	tcode=tch_check(tch_info_tb, tch_info_no);
	if(tcode<tch_info_no) key_buzzer();
//printf0("\n tch_date:%d", tcode);
	if(tcode==0){
		state=M_MENU;
		dsp_menu_home();
	}else if(tcode==1){
		state=M_STANDBY;
		dsp_standby();
	}
}

/*****************************************************************************/
/* KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK */
/*****************************************************************************/
u16 mmm;
void key_scan(void){
u8 kcode;
u16 xx,yy;

	if(!SW_TEST)  kcode=KEY_TEST;
	else kcode=0;
	
	if(keypush){
		if(!kcode) keypush--;
		else keypush=keypull=KDLY;
	}else{
		if(kcode){
			if(!(--keypull)){
				keypull=keypush=KDLY;
				key_code = kcode | 0x80;
printf0("\n Key Push:%2x", key_code);
			}
		}else keypull=KDLY;
	}

	if(!touch_flag) return;
	touch_flag = 0;
//2015-06-08 오후 4:44:27 Start
	if(!BALANCE_AUTO){
		xx = ad7_dat/4;			//Original
		yy = ad6_dat/4;			//Original
	}else{
		yy = ad7_dat/4;
		xx = ad6_dat/4;
	}
	//2015-06-08 오후 4:44:30 End
	
//if((++mmm)>25){	mmm=0;	printf0("\n y:%d,x:%d,a0:%d", yy,xx,ad0_dat/4); }
	if(tchpush){
		if(yy<MIN_TCHY){
			if(!(--tchpush)) tchpull=TDLY;
		}else tchpush=TDLY*2;
	}else{
		if(yy>=MIN_TCHY){
			if(!(--tchpull)){
				tchpush=TDLY;
				tchx = xx;
				tchy = yy;
			}
		}else tchpull=TDLY;
	}
}

/**************************************************************/
void key_decode(void){
	if(key_code&0x80){
		switch(key_code&0x7F){
			case KEY_TEST:  key_test();		break;
		}
		key_code=0;
	}

	if(tchy>MIN_TCHY){
printf0("\n Touch Code: x:%3d, y:%3d", tchx, tchy);
		if(flag_clean!=0x55){
			switch(state){
				case M_STANDBY:	tch_standby();	break;
				case M_STRIP:	tch_strip();	break;
				case M_SEQNO:	tch_seqno();	break;
				case M_IDNO:	tch_idno();		break;
				case M_COLOR:	tch_color();	break;
				case M_TURBI:	tch_turbi();	break;
				case M_MEASURE:	tch_measure();	break;
				case M_MENU:	tch_menu();		break;
				case M_M_DATE:	tch_date();		break;
				case M_M_CHK:	tch_syschk();	break;
				case M_M_MEM:	tch_memory();	break;
				case M_M_VIEWNO: tch_viewno();	break;
				case M_M_VIEW:	tch_view();		break;
				case M_M_NODATA: tch_nodata();	break;
				case M_M_SENDNO: tch_sendno();	break;
				case M_M_DELNO:	tch_delno();	break;
				case M_M_SET:	tch_setting();	break;
				case M_M_FORMAT: tch_format();	break;
				case M_M_SYSSET: tch_sysset();	break;
				case M_M_CHANGE: tch_change();	break;
				case M_M_PASS:   tch_pass();	break;
				case M_M_C_SET:	tch_pr_set();	break;
				case M_M_C_DGT:	tch_pr_dgt();	break;
				case M_M_INFO:	tch_info();		break;
			}
		}else tch_clean();
		tchy=tchx=0;
	}
}

void key_clear(void){	key_code=0;	tchy=tchx=0; }

/**************************************************************/
void tsw_poff(void){	//Power OFF
	key_buzzer(); 		//
	SENS_OFF();
	LED_GRN_OFF();
	LED_RED_OFF();
	state=M_OFF;
	dsp_poff();
	home_tray();

	while(1){
		LCDbmpxx(b_off, 74,328);					// 
		delay_ms(500);
		LCDbmpxx(b_shut,32,216);					// 
		delay_ms(500);
	}
}

/**************************************************************/

void power_on(void){
short a, b;	
	LED_GRN_OFF();
	LED_RED_ON();
	SENS_ON();
	b=0;
	for(a=0;a<10;a++){ delay_ms(1); if(!SENS_BK) b++; }

#ifndef DEBUG
	if(PCB_TEST4){
		if(b>5){
			set_CCW_TICK(MAX_BACK);		//10[sec]
			a=0;
			while(SmDir){
				if(!SENS_BK) a=0; else{ if((++a)>50) break; } 
				delay_ms(1); 
			}
			if(!SmDir)	printf0("\n Machine Error1!!!");	
			stepMotorOff();
			delay_ms(100);
		}
//		init_lcd();
		if(DOUBLE_FLAG==1){
			set_CW_TICK(MAX_TICK);			//10[sec] 전진
		}else{
			set_CW_TICK(MAX_TICK_1);			//10[sec] 전진
		}
		a=0;
		while(SmDir){
			delay_ms(100); 
			if((++a)>(133+50)) break;		//10[sec]
		}
		if(SmDir) printf0("\n Machine Error1!!!");	
//		else printf0("\n Delay %d", a);	
	}
#endif	
	stepMotorOff();
	SENS_OFF();

	move_strip_info();
	move_sys_curve();
	move_parameter();
//	move_pform();
	while(txw0!=txr0);

	state=M_STANDBY;
    rxw0=rxr0=txw0=txr0=0x00;
    rxw3=rxr3=txw3=txr3=0x00;
	LED_GRN_ON();
	LED_RED_OFF();
}

/*****************************************************************************/
/* TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT */
/*****************************************************************************/
#define TMM 100
void key_test(void){
	key_buzzer();
	if(SENS_GO) set_CCW(2000);				//10[sec]
	else set_CW(2000);				//10[sec]
	test_no = (test_no+1)&7;
	switch(test_no){
	}
	msg20(20,28,0xff,"Test%d", test_no); 
}

/**************************************************************/
void key_error(void){
	key_buzzer();
//	BUZZ_LO();
	delay_ms(50);
	key_buzzer();
//	BUZZ_LO();
}


/*****************************************************************************/
/* MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM */
/*****************************************************************************/
void tsw_clean(void){
	key_buzzer();
	if(!flag_clean) start_clean();		//Load가 있을 때 밀어낸다
	else end_clean();					//Load가 없을 때 끌어들인다.
}

#define MES_TICK	1520
#define SM20mm		(MES_TICK+500)/5
#define SM50mm		(MAX_TICK+1000)/2
u16	wait_clean;

void blink_clean(void){
	if(tm_10ms){
		tm_10ms=0;
		blk_dly++;
		if(blk_dly>40){
			blk_dly=0;
			if(!blk_pol){
				blk_pol=1;
				erase_box(140,340,140+120,340+65);			
			}else{
				blk_pol=0;
				LCDbmpxx(b_clean, 140,88+(36*5)+4+(68*1));			
			}
		}
		wait_clean++;
	}
}
void blink_clean_1(void){
	if(tm_10ms){
		tm_10ms=0;
		blk_dly++;
		if(blk_dly>40){
			blk_dly=0;
			if(!blk_pol){
				blk_pol=1;
				if(DOUBLE_FLAG==1){
					erase_box(140,272,140+120,272+65);			
				}else{
					erase_box(140,340,140+120,340+65);			
				}
			}else{
				blk_pol=0;
				if(DOUBLE_FLAG==1){
					LCDbmpxx(b_short, 140,88+(36*5)+4);			
				}else{
					LCDbmpxx(b_long, 140,88+(36*5)+4+(68*1));			
				}
			}
		}
		wait_clean++;
	}
}

//=============================================================
void start_clean(void){			//Load가 있을 때 밀어낸다
u8 b, c;	
u16 len;
	printf0("\n Start Clean");
	SENS_ON();

	if(DOUBLE_FLAG==1){
		set_CW_TICK(MAX_TICK+300);				//전진한다.
		len=(1150+1000)/2;
		b=0;
		do{
			if(SENS_GO){
				if((++b)>5){ if(SmCnt>len) SmCnt=len; }
//				if((++b)>5){ if(SmCnt>SM50mm) SmCnt=SM50mm; }
			}else b=0;
			delay_us(1000);
			blink_clean();
		}while(SmDir);
		
	}else{
		set_CCW_TICK(MES_TICK);				//후진한다.
		b=0; c=0;
		blk_pol=blk_dly=0;
		wait_clean=0;
		len = (1900+600)/2;
		do{
			if(SENS_GO){
				if((++b)>50){ 
					if(SmCnt>SM20mm) SmCnt=SM20mm; 
					wait_clean=0;
				}
			}else{
				b=0;
				if(SENS_BK){
					if((++c)>10){
						stepMotorOff();
						len = MAX_TICK+700;
					}
				}else c=0;
			}
			delay_us(1000);
			blink_clean();
		}while(SmDir);
	
		set_CW_TICK(len);				//전진한다.
		b=0;
		do{
			if(SENS_GO){
//				if((++b)>5){ if(SmCnt>SM50mm) SmCnt=SM50mm; }
				if((++b)>5){ if(SmCnt>len) SmCnt=len; }
			}else b=0;
			delay_us(1000);
			blink_clean();
		}while(SmDir);
		
	
	}
	stepMotorOff();
	flag_clean=0x55;
	write24w(CLEAN_ADR, flag_clean);
	system_write(SYSTEM_RAM, SYSTEM_SIZE);
	key_code=0;
	com_clear();
	LCDbmpxx(b_short, 140,88+(36*5)+4);			
	LCDbmpxx(b_long, 140,88+(36*5)+4+(68*1));			
	SENS_OFF();

}
//=============================================================
void end_clean(void){		//Load가 없을 때 끌어들인다.
u8 b, c;	
u16 len;
u8 flag_load=0;
	printf0("\n Wait Clean");
	if(DOUBLE_FLAG==1){
		set_CCW_TICK(MAX_TICK+200+500+700);		//10[sec]
		SENS_ON();
		b=0; c=0;
		blk_pol=blk_dly=0;
		wait_clean=0;
		len = MAX_TICK;
		do{
			b=0;
			if(SENS_BK){
				if((++c)>10){
					stepMotorOff();
					flag_load=1;
				}
			}else c=0;
			delay_us(1000);
			blink_clean_1();
		}while(SmDir);
		stepMotorOff(); 
		
		if(flag_load){
			set_CW_TICK(len);				//전진한다.
			do{
				delay_us(1000);
				blink_clean_1();
			}while(SmDir);
			flag_clean=0;
			write24w(CLEAN_ADR, flag_clean);
			system_write(SYSTEM_RAM, SYSTEM_SIZE);
			LCDbmpxx(b_clean, 140,88+(36*5)+4+(68*1));		//Tray In(Normal)
		}else{
			key_buzzer2();
			LCDbmpxx(b_clean, 140,88+(36*5)+4+(68*1));		//Tray Out
		}
		stepMotorOff();
		key_code=0;
		com_clear();
		SENS_OFF();
	}else{
		SENS_ON();
		set_CCW_TICK(MAX_TICK);				//후진한다.
		b=0; c=0;
		blk_pol=blk_dly=0;
		wait_clean=0;
		len = MAX_TICK;
		do{
			if(SENS_GO){
				if((++b)>50){ 
					//if(SmCnt>SM20mm) SmCnt=SM20mm; 
					if(SmCnt>SM50mm) SmCnt=SM50mm; 
//					len = 350;
					len = 600;
					flag_load=1;
				}
			}else{
				b=0;
				if(SENS_BK){
					if((++c)>10){
						stepMotorOff();
						flag_load=1;
					}
				}else c=0;
			}
			delay_us(1000);
			blink_clean_1();
		}while(SmDir);
		stepMotorOff(); 
		
		if(flag_load){
			set_CW_TICK(len);				//전진한다.
			do{
				delay_us(1000);
				blink_clean_1();
			}while(SmDir);
			flag_clean=0;
			write24w(CLEAN_ADR, flag_clean);
			system_write(SYSTEM_RAM, SYSTEM_SIZE);
			LCDbmpxx(b_clean, 140,88+(36*5)+4+(68*1));		//Tray In(Normal)
		}else{
			key_buzzer2();
			LCDbmpxx(b_clean, 140,88+(36*5)+4+(68*1));		//Tray Out
		}
		stepMotorOff();
		key_code=0;
		com_clear();
		SENS_OFF();
	}
	LCDbmpxx(b_menu1, 140,88+(36*5)+4);			
	LCDbmpxx(b_clean, 140,88+(36*5)+4+(68*1));			
}
/*****************************************************************************/
void tch_pr_set(void){
	tcode=tch_check(tch_prset_tb, tch_prset_no);
	if(tcode<tch_prset_no) key_buzzer();
//printf0("\n tch_date:%d", tcode); tcode=99;
	if(tcode<7){
//		if((pr_no==4)||(pr_no==10)||(pr_no==14)){
		if((pr_no==4)||(pr_no==10)){
			state=M_M_C_DGT;
			pr_sub=tcode;
			pr_dgt = PARA_NEW[pr_no][pr_sub][2];
			pr_ref = pr_dgt;
			dsp_change3();
		}else if(tcode<6){
			state=M_M_C_DGT;
			pr_sub=tcode;
			pr_dgt = PARA_NEW[pr_no][pr_sub+1][2];
			pr_ref = pr_dgt;
			dsp_change3();
		}else{
			key_buzzer2();
		}
	}else if(tcode==7){
		if(pr_no) pr_no--;
		else pr_no=16;
		dsp_change2();
		dsp_pr_set();
	}else if(tcode==8){
		if(pr_no<16)	pr_no++;
		else pr_no=0;
		dsp_change2();
		dsp_pr_set();
	}else if(tcode==9){
//==		state=M_MENU;
		state=M_M_SET;	
		dsp_setting();
//==		dsp_menu_home();
		while(dur_PWM);
		if(back_parameter()) backup_para();
	}else if(tcode==10){
		state=M_STANDBY;
		dsp_standby();
		while(dur_PWM);
		if(back_parameter()) backup_para();
	}
}
/*********************************************************************/
void backup_para(void){
u16 i,j;	
	printf0("\n store_para & Weight");
	com_msg("Change para & Weight" );
	j=0;
	for(i=0;i<PARA_SIZE;i++) j += STORE_RAM[PARA_RAM+i];
	j = ~j;
	STORE_RAM[PARA_RAM+i] = j>>8;
	STORE_RAM[PARA_RAM+i+1] = j;
	system_write(PARA_RAM, PARA_SIZE+2);
	move_parameter();
}
/*****************************************************************************/
void tch_pr_dgt(void){
	tcode=tch_check(tch_prdgt_tb, tch_prdgt_no);
	if(tcode<tch_prdgt_no) key_buzzer();
//printf0("\n tch_date:%d", tcode); tcode=99;
	if(tcode==0){
		if(pr_dgt) pr_dgt--;
		dsp_change3();
	}else if(tcode==1){
		pr_dgt++;
		dsp_change3();
	}else if(tcode==2){
		blink_pr_dgt();
//		PARA_NEW[pr_no][pr_sub+1][2] = pr_dgt;
// 추가
		if((pr_no==4)||(pr_no==10)){
			PARA_NEW[pr_no][pr_sub][2] = pr_dgt;
			PARA_NEW[pr_no][pr_sub+1][0] = pr_dgt;
		}else{
			PARA_NEW[pr_no][pr_sub][0] = pr_dgt;
			PARA_NEW[pr_no][pr_sub+1][2] = pr_dgt;
		}
// 추가 끝
		state=M_M_C_SET;
		dsp_change2();
		dsp_pr_set();
	}
}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


/******************************************************************************/
void tch_pass(void){
u8 b1;	
	tcode=tch_check(tch_digitv_tb, tch_digitv_no);
	if(tcode<tch_digitv_no) key_buzzer();
	if(tcode<10)
	{						//'0'-'9'
		tmp_seqno = (tmp_seqno*10) + tcode;
		if(tmp_seqno>2000) tmp_seqno = tcode;
		/*LCDbmpxx(b_seq   ,20,88);					
		b1 = put_seqno(tmp_seqno);
		msg20(130+b1,92,BLK,top_str);	
		*/
		draw_box(20,88,20+230,88+32,CYN,RAST1);
		draw_box(20,88,20+230,88+32,CYN,RAST0);
		b1 = put_seqno(tmp_seqno);
		msg20(90+b1,92,BLK,top_str);	
	}else if(tcode==10)
	{				//Down==> back space
		tmp_seqno /= 10;
/*		LCDbmpxx(b_seq   ,20,88);					
		b1 = put_seqno(tmp_seqno);
		msg20(128+b1,92,BLK,top_str);	*/
		draw_box(20,88,20+230,88+32,CYN,RAST1);
		draw_box(20,88,20+230,88+32,CYN,RAST0);
		b1 = put_seqno(tmp_seqno);
		msg20(90+b1,92,BLK,top_str);	
		blink_dur=0;
	}else if(tcode==11)
	{				//Up==> clear
		tmp_seqno = 0;
/*		LCDbmpxx(b_seq,20,88);					
		b1 = put_seqno(tmp_seqno);
		msg20(128+b1,92,BLK,top_str);	*/
		draw_box(20,88,20+230,88+32,CYN,RAST1);
		draw_box(20,88,20+230,88+32,CYN,RAST0);
		b1 = put_seqno(tmp_seqno);
		msg20(90+b1,92,BLK,top_str);	
		blink_dur=0;
	}else if(tcode==12)
	{				//memory		//vvvvvvvvvvvvvvvvvvvv
		state=M_M_SET;	
		dsp_setting();
		tmp_seqno = 0;
	}else if(tcode==13)
	{				//view data
		view_seqno = tmp_seqno;
		tmp_seqno = 0;
		if(view_seqno==1882)
		{
			dsp_change1();
			state=M_M_CHANGE;
		}else
		{
			state=M_M_SET;	
			dsp_setting();
//			dsp_system();
//			state=M_M_SYSSET;
		}
	}
}