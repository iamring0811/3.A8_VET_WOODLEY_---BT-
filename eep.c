#define	_EEP_C_

#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\init.h"
#include ".\inc\uart.h"
#include ".\inc\touch.h"
#include ".\inc\RESM.h"
#include ".\inc\LCD.h"
#include ".\inc\EEP.h"
#include ".\inc\meas.h"
#include ".\inc\flash.h"
#include ".\inc\com.h"
#include ".\inc\para.h"
#include ".\inc\dsp.h"
#include ".\inc\sysadj.h"//2015-06-09 오전 7:58:50
//#define DEBUG_FLASH

u16 init_err;
//u8 lcd_first;
void lcd_init2(void);

short chk_sum_eep(void);
short chk_sum_store(u16 adr, u16 size);
short chk_sum_flash(u16 adr, u16 size);
void backup_flash(u16 adr, u16 size);
void restore_flash(u16 adr, u16 size);

//=======================================================
void initEEP(void){
u16 i;
u8 *ip;
short sum;
//u16 b1;

/*
while(1){
//	CE24_HI(); delay_ms(1);	CE24_LO(); delay_ms(1);
//	WP24_HI(); delay_ms(1);	WP24_LO(); delay_ms(1);
	DI24_HI(); delay_ms(1);	DI24_LO(); delay_ms(1);
	CK24_LO(); delay_ms(1);	CK24_HI(); delay_ms(1);
}
	printf0("\n FR4K ");
	eraseFR(0);
	for(b1=0;b1<0x1000;b1+=2){ 
		flash_buff[b1] = (b1+0x8000)>>8;
		flash_buff[b1+1] = b1+0x8000;
	}
	writeFR4k(0, 0x4000);
	readFR4k(0, 0x4000);
	for(b1=0;b1<0x1000;b1+=2){ 
		printf0(" %2x", flash_read[b1]); 
		printf0("%2x,", flash_read[b1+1]); 
		wait_tx0();
	}
*/
	ip = (u8*)(STORE_ROM+BASE2470);
	for(i=0;i<STORE_SIZE;i++) FLASH_RAM[i] = *(ip++);		//Internal
	
	readFR4k(SYSTEM_RAM, SYSTEM_SIZE+2);
	for(i=0;i<SYSTEM_SIZE+0;i++) STORE_RAM[SYSTEM_RAM+i] = flash_read[i];	
	readFR4k(CURVE_RAM, CURVE_SIZE+2);
	for(i=0;i<CURVE_SIZE+2;i++) STORE_RAM[CURVE_RAM+i] = flash_read[i];	
	readFR4k(STRIP_RAM, STRIP_SIZE+2);
	for(i=0;i<STRIP_SIZE+2;i++) STORE_RAM[STRIP_RAM+i] = flash_read[i];	
	readFR4k(PARA_RAM, PARA_SIZE+2);
	for(i=0;i<PARA_SIZE+2;i++) STORE_RAM[PARA_RAM+i] = flash_read[i];	
	readFR4k(PFORM_RAM, PFORM_SIZE+2);
	for(i=0;i<PFORM_SIZE+2;i++) STORE_RAM[PFORM_RAM+i] = flash_read[i];	

	printf0("\n Check flash memory");
	chk_ini_curve();
	chk_ini_strip();
	chk_ini_para();
	chk_ini_pform();
	if(init_err){
//		init_err = 5 - init_err;
		delay_ms(1000+(init_err*500));
	}

	i = read24w(ID55AA_ADR);
	sum = chk_sum_store(SYSTEM_RAM, ID_CHKSUM);
	
	if((i==ID55AA)&&(sum==OK)){
printf0("\n Correct ID:%4x", i);
		i = FLASH_RAM[0];
		i = (i<<8) + FLASH_RAM[1];
		sum = chk_sum_flash(SYSTEM_RAM, ID_CHKSUM);
		if((i!=ID55AA)||(sum!=OK)){
			printf0("\n Backup System Data.");
			backup_flash(SYSTEM_RAM, SYSTEM_SIZE);
		}
	}else{
		i = FLASH_RAM[0];
		i = (i<<8) + FLASH_RAM[1];
		sum = chk_sum_flash(SYSTEM_RAM, ID_CHKSUM);
		if((i==ID55AA)&&(sum==OK)){
			printf0("\n Restore System Data.");
			restore_flash(SYSTEM_RAM, SYSTEM_SIZE);
		}else{
			printf0("\n Initialize System");
			key_buzzer3(); 
			default_id();
		}
	}
	read_eep_all();
	get_max_stripno();
	flag_clean=read24w(CLEAN_ADR);
}

//=======================================================
short chk_sum_eep(void){
u16 w1, w2;	
	w2 = 0;
	for(w1=ID55AA_ADR;w1<ID_CHKSUM;w1++) w2 += read24(w1);
	w2 = ~w2;
	w1 = read24w(ID_CHKSUM);
	if(w1!=w2){
		printf0("\n Error chksum. SUM:%4x, EEP:%4x", w1, w2);
		return(NG);
	}else return(OK);
}

//=======================================================
void read_eep_all(void){
u16 i;	
printf0("\n Read I.D. All");
	for(i=0;i<11;i++){
		SYS_ID[i]=read24(SYSID_ADR+i);
//		USER_ID[i]=read24(USERID_ADR+i);
	}
	SYS_ID[11]=0;
	USER_ID[11]=0;
printf0("\n SysID:%s", SYS_ID);
	clr_user_id();						
//printf0("\n UserID:%s", USER_ID);

	SEQ_NO=read24w(SEQNO_ADR);
printf0("\n SEQ_NO:%d", SEQ_NO);
	if(SEQ_NO>MAX_RECORD){ 
		printf0("\t Reset SEQ_NO:%d", SEQ_NO);
		SEQ_NO=1; write24w(SEQNO_ADR, SEQ_NO); 
		write_id_chksum();
//		system_backup(ID55AA_ADR, ID_CHKSUM+2);
	}
	
	STRIPNO=read24w(STRIPNO_ADR);
printf0("\n StripNo:%d", STRIPNO);
	if(STRIPNO>99){ 
		STRIPNO=0; write24w(STRIPNO_ADR, STRIPNO); 
		printf0("\t Reset StripNo:%2d", STRIPNO);
		write_id_chksum();
//		system_backup(ID55AA_ADR, ID_CHKSUM+2);
	}

	SPEED_FLAG=read24(SPEED_ADR);
if(SPEED_FLAG==SPEED_HI) printf0("\n SPEED Hi:%4x", SPEED_FLAG);
else printf0("\n SPEED Lo:%4x", SPEED_FLAG);

	DOUBLE_FLAG=read24(DOUBLE_ADR);
	//DOUBLE_FLAG=1;
if(DOUBLE_FLAG==1) printf0("\n Double Read");
else printf0("\n Single Read");

	PRINT_FLAG=read24w(PRINT_ADR);				//Auto Print
	PRINT_FLAG=PRINT_DISABLE;					//Allways Off Printoff

	UNIT_FLAG=read24w(UNIT_ADR);
if(UNIT_FLAG==UNIT_CONV) printf0("\n unit:Conv.");
else if(UNIT_FLAG==UNIT_SI) printf0("\n unit:SI.");
else printf0("\n unit:SI.");

	PLUS_FLAG=read24w(PLUS_ADR);
if(SOUND_FLAG) printf0("\n Plus Enable");
else printf0("\n Plus Disable");

	MARK_FLAG=read24w(MARK_ADR);
if(SOUND_FLAG) printf0("\n Mark Yes");
else printf0("\n Mark No");

	SOUND_FLAG=read24w(SOUND_ADR);
if(SOUND_FLAG) printf0("\n Sound Enable");
else printf0("\n Sound Disable");

	BAUD_FLAG=read24w(BAUD_ADR);
	set_baud3();
if(BAUD_FLAG==1) printf0("\n Baudrate:9600");
else if(BAUD_FLAG==2) printf0("\n Baudrate:19200");
else printf0("\n Baudrate:38400");

//	COLOR_NO = read24w(COLOR_ADR);
//printf0("\n Color NO.:%d", COLOR_NO);
//	TURBI_NO = read24w(TURBI_ADR);
//printf0("\n Turbi NO.:%d", TURBI_NO);
	COLOR_NO = 0;
	TURBI_NO = 0;

	DATA_FLAG =DATA_AUTO;
	printf0("\n Data Auto");

	result_mode=read24w(RESMOD_ADR);
if(result_mode==MULTI) printf0("\n result:Multistix");
else if(result_mode==COMBO) printf0("\n result:Combo");
else printf0("\n result:Normal");
	
	printf0("\n SYS.ID:%s", SYS_ID);

}
//=======================================================
void default_id(void){
u16 i;	
	key_buzzer3(); 
	for(i=0;i<SYSTEM_SIZE;i++) STORE_RAM[SYSTEM_RAM+i] = 0;
	printf0("\n ***** Set default I.D. *****");
	write24w(ID55AA_ADR, ID55AA);
	SEQ_NO=1;
	write24w(SEQNO_ADR, SEQ_NO);
	UNIT_FLAG =UNIT_CONV;
	write24w(UNIT_ADR, UNIT_FLAG);
	STRIPNO = 0;
	write24w(STRIPNO_ADR, STRIPNO);
	SPEED_FLAG=SPEED_LO;
	write24(SPEED_ADR, SPEED_FLAG);
	DOUBLE_FLAG=0;		//single
	write24(DOUBLE_ADR, DOUBLE_FLAG);
	PRINT_FLAG=PRINT_ENABLE;	
	write24w(PRINT_ADR, PRINT_FLAG);
//	DATA_FLAG =DATA_AUTO;
//	write24w(DATA_ADR, DATA_FLAG);
	result_mode = 0;		//Normal
	write24w(RESMOD_ADR, result_mode);
	SOUND_FLAG=1;
	write24w(SOUND_ADR, SOUND_FLAG);
	BAUD_FLAG=0;
	write24w(BAUD_ADR, BAUD_FLAG);
	PLUS_FLAG=0;
	write24w(PLUS_ADR, PLUS_FLAG);
	MARK_FLAG=0;
	write24w(MARK_ADR, MARK_FLAG);
	COLOR_NO=0;
	write24w(COLOR_ADR, COLOR_NO);
	TURBI_NO=0;
	write24w(TURBI_ADR, TURBI_NO);
//20150519 start
	write24w(DAC_ADR, 0X7777);
	set_default_RGB();
//20150519 end
	for(i=0;i<11;i++) SYS_ID[i] ='0'; 	SYS_ID[11]=0;
	write_sysid();
	clr_user_id(); 
	write_userid();
	write_id_chksum();
}

void clr_user_id(void){
u8 i;
	for(i=0;i<MAX_USERID;i++) {
		tmp_idno[i]=' ';
		USER_ID[i]=' '; 	
	}
	USER_ID[MAX_USERID]=0; user_cnt=0;
	COLOR_NO = 0;
	TURBI_NO = 0;
}

//=======================================================
void write_id_chksum(void){
u16 i, sum;
	sum=0;
	for(i=0;i<ID_CHKSUM;i++) sum += read24(i);
	write24w(ID_CHKSUM, ~sum);
	system_write(SYSTEM_RAM, SYSTEM_SIZE);
}

//=======================================================
#define OK_INIT	0x55AA
short chk_sum_store(u16 addr, u16 size){
u16 i, j, sum;	
	printf0(" add:%4x, size:%d", addr, size);
	sum=0;
	for(i=addr;i<(addr+size);i++) sum += STORE_RAM[i];
	j = STORE_RAM[i++];
	j = (j<<8) + STORE_RAM[i++];
	printf0("  cs:%4x,%4x", sum,j);
//j=0;
	if(j==sum) return(OK_INIT);
	sum = ~sum;
	if(j==sum) return(OK);
	else return(NG);
}

//=======================================================
short chk_sum_flash(u16 addr, u16 size){
u16 i, j, sum;	
	printf0(" add:%4x, size:%d", addr, size);
	sum=0;
	for(i=addr;i<(addr+size);i++) sum += FLASH_RAM[i];
	j = FLASH_RAM[i++];
	j = (j<<8) + FLASH_RAM[i++];
	printf0("  fcs:%4x,%4x", sum,j);
//j=0;
	sum = ~sum;
	if(j==sum) return(OK);
	else return(NG);
}

//=======================================================
#define XS_OFF	34
#define YS_OFF	390
#define SS_WAIT	100

void chk_ini_curve(void){
u16 i, j;	
short sum;
	init_err=0;
	printf0("\n chk_ini_curve");
//	for(i=0;i<(CURVE_SIZE+2);i++) printf0(" %2x,",STORE_RAM[CURVE_RAM+i]);
	sum = chk_sum_store(CURVE_RAM, CURVE_SIZE);
//sum=NG;	
	if(sum==OK){
		printf0("\n Valid Curve Data.");
		sum = chk_sum_flash(CURVE_RAM, CURVE_SIZE);
		if(sum!=OK){
			printf0("\n Backup Curve Data.");
			backup_flash(CURVE_RAM, CURVE_SIZE);
		}		
	}else{
		printf0(" (No ext.)");
		sum = chk_sum_flash(CURVE_RAM, CURVE_SIZE);		//cmp with Internal
		if(sum==OK){
			printf0("\n Restore Curve Data.");
//	for(i=0;i<(CURVE_SIZE+2);i++) printf0(" %2x,",FLASH_RAM[CURVE_RAM+i]);
			restore_flash(CURVE_RAM, CURVE_SIZE);
		}else{
			printf0("\n ***** Please, set up curve data!!! *****");
			sum = chk_sum_store(CURVE_RAM, CURVE_SIZE);
			if(sum!=OK_INIT){
				j=0;
				for(i=0;i<CURVE_SIZE;i++){
					STORE_RAM[CURVE_RAM+i] = ini_curve[i];
					j += ini_curve[i];
				}
				STORE_RAM[CURVE_RAM+i] = j>>8;
				STORE_RAM[CURVE_RAM+i+1] = j;
				printf0("\t Init.Curve[SumB:%4x]", j);
				system_write(CURVE_RAM, CURVE_SIZE+2);
#ifdef ERR_MSG
				msg16(XS_OFF,YS_OFF,BLU,"Initialize curve!!!");
#endif
				key_buzzer2(); 
			}else{
#ifdef ERR_MSG
				msg16(XS_OFF,YS_OFF,BLU,"Please, set up curve!!!");
#endif
				key_buzzer(); 
			}
			init_err++;
		}
	}
	move_sys_curve();
}
//=======================================================
void chk_ini_strip(void){
u16 i, j;	
short sum;
	printf0("\n chk_ini_strip");
	sum = chk_sum_store(STRIP_RAM, STRIP_SIZE);
	if(sum==OK){
		printf0("\n Valid Strip Data.");
		sum = chk_sum_flash(STRIP_RAM, STRIP_SIZE);
		if(sum!=OK){
			printf0("\n Backup Strip Data.");
			backup_flash(STRIP_RAM, STRIP_SIZE);
		}		
	}else{
		printf0(" (No ext.)");
		sum = chk_sum_flash(STRIP_RAM, STRIP_SIZE);
		if(sum==OK){
			printf0("\n Restore Strip Data.");
			restore_flash(STRIP_RAM, STRIP_SIZE);
		}else{
			printf0("\n ***** Please, set up Strip data!!! *****");
			sum = chk_sum_store(STRIP_RAM, STRIP_SIZE);
			if(sum!=OK_INIT){
				j=0;
				for(i=0;i<STRIP_SIZE;i++){
					STORE_RAM[STRIP_RAM+i] = ini_strip[i];
					j += ini_strip[i];
				}
				STORE_RAM[STRIP_RAM+i] = j>>8;
				STORE_RAM[STRIP_RAM+i+1] = j;
				printf0("\t Init.Strip[SumB:%4x]", j);
				system_write(STRIP_RAM, STRIP_SIZE+2);
#ifdef ERR_MSG
				msg16(XS_OFF,YS_OFF+18,BLU,"Initialize strip!!!");
#endif
				key_buzzer2(); 
			}else{
#ifdef ERR_MSG
				msg16(XS_OFF,YS_OFF+18,BLU,"Please, set up strip!!!");
#endif
				key_buzzer(); 
			}
			init_err++;
		}
	}
	move_strip_info();
}
//=======================================================
void chk_ini_para(void){
u16 i, j;	
short sum;
	printf0("\n chk_ini_para");
	sum = chk_sum_store(PARA_RAM, PARA_SIZE);
	if(sum==OK){
		printf0("\n Valid Para Data.");
		sum = chk_sum_flash(PARA_RAM, PARA_SIZE);
		if(sum!=OK){
			printf0("\n Backup Para Data.");
			backup_flash(PARA_RAM, PARA_SIZE);
		}		
	}else{
		printf0(" (No ext.)");
		sum = chk_sum_flash(PARA_RAM, PARA_SIZE);
		if(sum==OK){
			printf0("\n Restore Para Data.");
			restore_flash(PARA_RAM, PARA_SIZE);
		}else{
			printf0("\n ***** Please, set up Para data!!! *****");
			sum = chk_sum_store(PARA_RAM, PARA_SIZE);
			if(sum!=OK_INIT){
				j=0;
				for(i=0;i<WEIGHT_SIZE;i++){
					STORE_RAM[PARA_RAM+i] = ini_weight[i];
					j += ini_weight[i];
				}
				for(i=WEIGHT_SIZE;i<PARA_SIZE;i++){
					STORE_RAM[PARA_RAM+i] = ini_para[i-WEIGHT_SIZE];
					j += ini_para[i-WEIGHT_SIZE];
				}
				STORE_RAM[PARA_RAM+i] = j>>8;
				STORE_RAM[PARA_RAM+i+1] = j;
				printf0("\t Init.Para[SumB:%4x]", j);
				system_write(PARA_RAM, PARA_SIZE+2);
#ifdef ERR_MSG
				msg16(XS_OFF,YS_OFF+36,BLU,"Initialize Para!!!");
#endif
				key_buzzer2(); 
			}else{
#ifdef ERR_MSG
				msg16(XS_OFF,YS_OFF+36,BLU,"Please, set up Para!!!");
#endif
				key_buzzer(); 
			}
			init_err++;
		}
	}
	move_parameter();
}

//====================================================================
void chk_ini_pform(void){
u16 i, j, k, m;
short sum;
	printf0("\n chk PrintForm");
	sum = chk_sum_store(PFORM_RAM, PFORM_SIZE);
//sum=NG;	
	if(sum==OK){
		printf0("\n Valid printform");
		sum = chk_sum_flash(PFORM_RAM, PFORM_SIZE);
		if(sum!=OK){
			printf0("\n Backup printform Data.");
			backup_flash(PFORM_RAM, PFORM_SIZE);
		}		
	}else{
		printf0(" (No ext.)");
		sum = chk_sum_flash(PFORM_RAM, PFORM_SIZE);
		if(sum==OK){
			printf0("\n Restore printform Data.");
			restore_flash(PFORM_RAM, PFORM_SIZE);
		}else{
			printf0("\n ***** Please, set up printform data!!! *****");
			sum = chk_sum_store(PFORM_RAM, PFORM_SIZE);
//sum=NG;
			if(sum!=OK_INIT){
				m=PFORM_RAM;
				for(i=0;i<21;i++){ 
					for(j=0;j<8;j++){ 
						for(k=0;k<8;k++)  STORE_RAM[m++] = ar_item_new[i][j][k]; 
					} 
				}
				for(i=0;i<21;i++){ 
					for(j=0;j<8;j++){ 
						for(k=0;k<8;k++) STORE_RAM[m++] = si_item_new[i][j][k]; 
					} 
				}
				for(i=0;i<21;i++){ 
					for(j=0;j<8;j++){ 
						for(k=0;k<8;k++) STORE_RAM[m++] = co_item_new[i][j][k]; 
					} 
				}

				j=0;
				for(i=0;i<PFORM_SIZE;i++) j += STORE_RAM[PFORM_RAM+i];
				STORE_RAM[PFORM_RAM+i] = j>>8;
				STORE_RAM[PFORM_RAM+i+1] = j;
				printf0("\t Init.printform[SumB:%4x]", j);
				system_write(PFORM_RAM, PFORM_SIZE+2);
#ifdef ERR_MSG
				msg16(XS_OFF,YS_OFF+54,BLU,"Initialize printform!!!");
#endif
				key_buzzer2(); 
			}else{
#ifdef ERR_MSG
				msg16(XS_OFF,YS_OFF+54,BLU,"Please, set up printform!!!");
#endif
				key_buzzer(); 
//				for(i=0;i<PFORM_SIZE;i++) printf0("%c", STORE_RAM[PFORM_RAM+i]);
			}
			init_err++;
		}
	}
	move_pform();
}
/*********************************************************************
//====================================================================
//====================================================================
//====================================================================
*********************************************************************/
//====================================================================
void write_sysid(void){
int i;
//	enwrite();
	for(i=0;i<11;i++){
		write24(SYSID_ADR+i, SYS_ID[i]);
	}	
	write24(SYSID_ADR+i, 0);
//	dnwrite();
}

//====================================================================
void write_userid(void){
//int i;
//	enwrite();
//	for(i=0;i<10;i++){	write24(USERID_ADR+i, USER_ID[i]);	}	
//	write24(USERID_ADR+i, 0);
//	dnwrite();
}

//=======================================================
void get_max_stripno(void){
u16 j;
u8 b1;
	for(j=0;j<MAX_STRIP;j++){
		b1 = STORE_RAM[STRIP_RAM+(j*StripOffset)];
		if((b1<'0')||(b1>'9')) break;
	}
	MAX_STRIPNO = j;
printf0("\n MAX_STRIPNO:%d", MAX_STRIPNO);	
}
//====================================================================
void key_buzzer2(void){
	key_buzzer();	delay_ms(150);
	key_buzzer();	delay_ms(150);
}

void key_buzzer3(void){
	key_buzzer();	delay_ms(150);
	key_buzzer();	delay_ms(150);
	key_buzzer();	delay_ms(150);
}


/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
u16 read_ram4(u16 pnt){
u16 dat4;
	dat4 = asc2hex(STORE_RAM[pnt+0]);
	dat4 = (dat4<<4) + asc2hex(STORE_RAM[pnt+1]);
	dat4 = (dat4<<4) + asc2hex(STORE_RAM[pnt+2]);
	dat4 = (dat4<<4) + asc2hex(STORE_RAM[pnt+3]);
	return(dat4);
}
/*******************************************************************************/
void move_strip_info(void){
int j, i;
	printf0("\n move strip....");
	if(STRIPNO>=MAX_STRIPNO) STRIPNO = MAX_STRIPNO - 1;
	for(j=0;j<StripOffset;j++) STRIP_INFO[j] = 0;
	i = STRIP_RAM+(STRIPNO*StripOffset)+2;
	for(j=0;j<10;j++) strip_name[j] = STORE_RAM[i++]; 
	strip_name[j] = 0;
	i = STRIP_RAM+(STRIPNO*StripOffset)+2+10;
	for(j=0;j<12;j++) STRIP_INFO[j] = asc2hex(STORE_RAM[i++]);
	if(STRIP_INFO[0]>MAX_PAD_NO) STRIP_INFO[0] = MAX_PAD_NO;
	wait_time = STORE_RAM[i++] - '0'; 
	wait_time = (wait_time*10) + STORE_RAM[i++] - '0'; 
	wait_time = (wait_time*10) + STORE_RAM[i++] - '0'; 
	printf0(" WaitTime : %d", wait_time);	
}

/*******************************************************************************/
void move_sys_curve(void){
u16 j, k;
	printf0("\n move curve....");
	k=CURVE_RAM;							//0=Position Data
	for(j=0;j<12;j++){
		StripPosition[j] = read_ram4(k); k+=4;
	}
	GIULGI = StripPosition[0];
	k = CURVE_RAM+24;						//48/2
	for(j=0;j<14;j++){
		SYS_CURVE_RED[j] = read_ram4(k); k+=4;
		SYS_CURVE_GRN[j] = read_ram4(k); k+=4;
		SYS_CURVE_BLU[j] = read_ram4(k); k+=4;
	}			
//	printf0(" ********\n");
//	for(j=0;j<CURVE_SIZE;j++){
//		if((j%24)==0) printf0("//");
//		printf0("%c",STORE_RAM[CURVE_RAM+j]);
//	}
//	printf0("\n ********");
}

/*******************************************************************************/
//3*7*11
void move_parameter(void){		//including Weight Data
u16 i;
u8  j, k, m;

	printf0("\n move weight....");
	for(j=0;j<WEIGHT_SIZE;j++) weight[j] = STORE_RAM[PARA_RAM+j]-'0'; 
	
	printf0("\t move para....");
	i = PARA_RAM+WEIGHT_SIZE;			//초기 20Bytes는 Weight Data.		
	for(j=0;j<17;j++){			//was 13, was 11
		for(k=0;k<7;k++){
			for(m=0;m<3;m++){
//				printf0(" %d,", read_ram4(i));
				PARA_NEW[j][k][m] = read_ram4(i); i+=4;
			}
//		print_thermal("\n%5d %5d %5d %5d %5d", j, k, PARA_NEW[j][k][0],PARA_NEW[j][k][1],PARA_NEW[j][k][2]);	
		}
	}		
}

/*******************************************************************************/
u16 back_parameter(void){				//touch.c에서 사용
u32 ww;
u16 i;
u8  j, k, m, change, b1;
	printf0("\n back para....");
	change=0;
	i = PARA_RAM+WEIGHT_SIZE;			//초기 20Bytes는 Weight Data.		
	for(j=0;j<17;j++){					//was 13, was 11
		for(k=0;k<7;k++){
			for(m=0;m<3;m++){
//				printf0(" %d,", read_ram4(i));
				ww = PARA_NEW[j][k][m];
				ww = bin2hexasc4(ww);
//printf0(" %lx", ww);
				b1=(ww>>24)&0xFF; if(STORE_RAM[i]!=b1){ STORE_RAM[i] = b1; change=1; } i++; 
				b1=(ww>>16)&0xFF; if(STORE_RAM[i]!=b1){ STORE_RAM[i] = b1; change=1; } i++; 
				b1=(ww>> 8)&0xFF; if(STORE_RAM[i]!=b1){ STORE_RAM[i] = b1; change=1; } i++; 
				b1=(ww>> 0)&0xFF; if(STORE_RAM[i]!=b1){ STORE_RAM[i] = b1; change=1; } i++; 
			}
		}
	}		
//	change=0;
	return(change);
}

//====================================================================
void move_pform(void){
u16 i, j, k, m;
	printf0("\n Move Printform");
	m=PFORM_RAM;
	for(i=0;i<21;i++){ 
		for(j=0;j<8;j++){ 
			for(k=0;k<8;k++) ar_item_ram[i][j][k] = STORE_RAM[m++]; 
		} 
	}
	for(i=0;i<21;i++){ 
		for(j=0;j<8;j++){ 
			for(k=0;k<8;k++) si_item_ram[i][j][k] = STORE_RAM[m++]; 
		} 
	}
	for(i=0;i<21;i++){ 
		for(j=0;j<8;j++){ 
			for(k=0;k<8;k++) co_item_ram[i][j][k] = STORE_RAM[m++]; 
		} 
	}
}
//====================================================================
//사용안함.
void dsp_pform(void){
u16 i,j,k,m;
	printf0("\n PrintForm");
	m=PFORM_RAM;
	for(i=0;i<21;i++){ 
		printf0("\n");
		for(j=0;j<8;j++){ 
			for(k=0;k<8;k++) printf0("%c", STORE_RAM[m++]); 
		} 
	}
	for(i=0;i<21;i++){ 
		printf0("\n");
		for(j=0;j<8;j++){ 
			for(k=0;k<8;k++) printf0("%c", STORE_RAM[m++]); 
		} 
	}
	for(i=0;i<21;i++){ 
		printf0("\n");
		for(j=0;j<8;j++){ 
			for(k=0;k<8;k++) printf0("%c", STORE_RAM[m++]); 
		} 
	}
}
//====================================================================
void backup_flash(u16 adr, u16 size){		//Ext==>Int
u16 i;
	for(i=0;i<(size+2);i++) FLASH_RAM[adr+i] = STORE_RAM[adr+i];
	backup_write();
}
//====================================================================
void restore_flash(u16 adr, u16 size){		//Int==>Ext
u16 i;
	for(i=0;i<(size+2);i++) STORE_RAM[adr+i] = FLASH_RAM[adr+i];
//	system_write(adr, size);
	system_write(adr, size+2);
}
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
