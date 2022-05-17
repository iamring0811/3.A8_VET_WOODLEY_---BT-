#define	_COM_C_
//0x58 시간설정 12Bytes
//0x59 Version  12Bytes
//#include "c:\ads\include\string.h"
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
#include ".\inc\com.h"
#include ".\inc\flash.h"
#include ".\inc\sysadj.h"
#include ".\inc\dsp.h"

void com_msg(char *str);
u16 hexasc2bin4(u32 asc);

void store_uid(void){
u32 i;
	printf0("\n store_uid with seq_ no");
	com_msg("Change User ID");
	i = cmdbuff[0];
	i = (i<<8) + cmdbuff[1];
	i = (i<<8) + cmdbuff[2];
	i = (i<<8) + cmdbuff[3];
	SEQ_NO = bcd2bin(i);
	write24w(SEQNO_ADR, SEQ_NO);
	for(i=0;i<MAX_USERID;i++) USER_ID[i] = cmdbuff[i+4];
	USER_ID[i] = 0x00;
	write_userid();
	write_id_chksum();
}

void store_sid(void){
u8 i;	
	printf0("\n store_sid");
	com_msg("Change Machine ID");
	for(i=0;i<11;i++) SYS_ID[i] = cmdbuff[i];
	SYS_ID[i] = 0x00;
	write_sysid();
	write_id_chksum();
//	system_backup(ID55AA_ADR, ID_CHKSUM+2);
}

void store_strip_no(void){
u32 i;	
	i = 0x30303030;
	i = (i<<8) + cmdbuff[0];	
	i = (i<<8) + cmdbuff[1];	
	printf0("%4x",i);
	STRIPNO = hexasc2bin4(i);
	printf0("\n store_strip_no:%d",STRIPNO);
	com_msg("Change Strip NO");

	write24w(STRIPNO_ADR, STRIPNO);
	write_id_chksum();
//	system_backup(ID55AA_ADR, ID_CHKSUM+2);
	printf0(" -%2d",STRIPNO);
	get_max_stripno();
	move_strip_info();
}

/*********************************************************************/
void store_curve(void){
u16 i,j;	
	printf0("\n store_curve");
	com_msg("Change curve" );
	j=0;
	for(i=0;i<CURVE_SIZE;i++){
		STORE_RAM[CURVE_RAM+i] = cmdbuff[i];
		j += cmdbuff[i];
	}
	j = ~j;
	STORE_RAM[CURVE_RAM+i] = j>>8;
	STORE_RAM[CURVE_RAM+i+1] = j;
	system_write(CURVE_RAM,CURVE_SIZE+2);
	move_sys_curve();
}

/*********************************************************************/
void store_strip(void){
u16 i,j;	
	printf0("\n store_Strip");
	com_msg("Change Strip" );
	j=0;
	for(i=0;i<STRIP_SIZE;i++){
		STORE_RAM[STRIP_RAM+i] = cmdbuff[i];
		j += cmdbuff[i];
	}
	j = ~j;
	STORE_RAM[STRIP_RAM+i] = j>>8;
	STORE_RAM[STRIP_RAM+i+1] = j;
	system_write(STRIP_RAM,STRIP_SIZE+2);

	get_max_stripno();
	move_strip_info();
}

/*********************************************************************/
void store_para(void){
u16 i,j;	
	printf0("\n store_para & Weight");
	com_msg("Change para & Weight" );
	j=0;
	for(i=0;i<PARA_SIZE;i++){
		STORE_RAM[PARA_RAM+i] = cmdbuff[i];
		j += cmdbuff[i];
	}
	j = ~j;
	STORE_RAM[PARA_RAM+i] = j>>8;
	STORE_RAM[PARA_RAM+i+1] = j;

	system_write(PARA_RAM,PARA_SIZE+2);
	move_parameter();
}
/*********************************************************************/
void system_decode(u8 dat){
	com3dat = dat;
	rx3_wait=RX3_WAIT;				//100[ms]
//savcmd0(com3dat);
//printf0("%2x",com3dat);
	if(!rx3_cnt){
		if(com3dat=='*'){ 
			rx3_cnt=1;  
//printf0("\n Start#%2x",com3dat);
		}else if((rx0_step==RX0_1st)&&(com3dat=='$')){ 
			rx0_cnt=0;
			downdata[rx0_cnt++] = com3dat;
			rx0_step = RX0_2nd;
			rx3_wait=RX3_WAIT;
			flag_flash=3; 
			printf03("\n Get1 $");
		}
	}else if(rx3_cnt==1){
//printf0("\n 1'st #%2x",com3dat);
		switch(com3dat){
			case COM_SYS_INFO:		                  
			case COM_ID_SET:		                  
			case COM_MEAS_START:	                  
			case COM_MEAS_RESULT:	                  
			case COM_UID_SET:		                  
			case COM_CURVE_READ:	                  
			case COM_CURVE_WRITE:	                  
			case COM_PARA_READ:		                  
			case COM_PARA_WRITE:	                  
			case COM_STRIP_READ:	                  
			case COM_STRIP_WRITE:	                  
			case COM_STRIP_NO_RD:	                  
			case COM_STRIP_NO_WR:	                  
			case COM_FULL_RAW_DATA:	                  
			case COM_SPD_READ:		                  
			case COM_SPD_WRITE:		                  
			case COM_TIME_WRITE:	                  
			case COM_VERSION_READ:	                  
			case COM_TIME_READ:		                  
			case COM_DOUBLE_READ:	                  
			case COM_DOUBLE_WRITE:	                  
			case COM_CLEAN_LOAD:	                  
			case COM_DATA_COUNT:	                  
			case COM_CALIBRATION:		              
			case COM_SYS_CHECK:	                      
			case COM_DATA_NOSTORE:	                  
			case COM_DATA_ERASE:                      
			case COM_ALL_RESULT:                      
			case COM_PFORM_READ:
			case COM_PFORM_WRITE:
				COMCMD = com3dat;                     
				rx3_cnt=2;   
			break;                          
			default: write_step=rx3_cnt=rx3_wait=0; break;
		}
	}else{
//printf0("\n 2'nd #%2x,%2d",com3dat,rx3_cnt);
		switch(COMCMD){
			case COM_SYS_INFO:		com_sys_info();		break;
			case COM_ID_SET:		com_id_set();		break;
			case COM_MEAS_START:	com_meas_start();	break;
			case COM_MEAS_RESULT:	com_meas_result();	break;
			case COM_UID_SET:		com_uid_set();		break;
			case COM_CURVE_READ:	com_curve_read();	break;
			case COM_CURVE_WRITE:	com_curve_write();	break;
			case COM_PARA_READ:		com_para_read();	break;
			case COM_PARA_WRITE:	com_para_write();	break;
			case COM_STRIP_READ:	com_strip_read();	break;
			case COM_STRIP_WRITE:	com_strip_write();	break;
			case COM_STRIP_NO_RD:	com_strip_no_rd();	break;
			case COM_STRIP_NO_WR:	com_strip_no_wr();	break;
			case COM_FULL_RAW_DATA:	com_full_raw_data();	break;
			case COM_SPD_READ:		com_spd_read();		break;
			case COM_SPD_WRITE:		com_spd_write();	break;
			case COM_TIME_READ:		com_time_read();	break;
			case COM_TIME_WRITE:	com_time_write();	break;
			case COM_VERSION_READ:	com_version_read();	break;
			case COM_DOUBLE_READ:	com_double_read();	break;
			case COM_DOUBLE_WRITE:	com_double_write();	break;
			case COM_PFORM_READ:	com_pform_read();	break;
			case COM_PFORM_WRITE:	com_pform_write();	break;
			default: write_step=rx3_cnt=rx3_wait=0; break;
		}
	}
}

void com_sys_info(void){		
u32 i;
	if((rx3_cnt==2)&&(com3dat==URLF)){
		savcmd3('*');
		savcmd3(0x51);
		for(i=0;i<11;i++) savcmd3(SYS_ID[i]);
		savcmd3(URLF);
		printf0("\n Send sys id.");
		key_buzzer();
	}
	rx3_cnt=0;
}

void com_id_set(void){
u32 i, j;
	if(rx3_cnt<(2+11)){
		if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
		rx3_cnt++;
	}else if((com3dat==URLF)&&(rx3_cnt==13)){
		savcmd3('*');
		savcmd3(0x52);
		for(i=0;i<11;i++) savcmd3(cmdbuff[i]);
		savcmd3(URLF);		
		//뭘넘겨주는지모르겠다.
		j = STORE_RAM[PARA_RAM+20+(2820/2)-4+1];			
		j = (j<<8) + STORE_RAM[PARA_RAM+20+(2820/2)-4+0];
		i = bin2hexasc4(j);
		savcmd3(i>>24); 
		savcmd3(i>>16); 
		savcmd3(i>>8); 
		savcmd3(i>>0); 
		savcmd3(URLF);
		rx3_cnt=0;
		key_buzzer();
		store_sid();
	}else rx3_cnt=0;
}

void com_meas_start(void){
	if((rx3_cnt==2)&&(com3dat==URLF)){
		if((state==M_STANDBY)||(state==M_MEASURE)){
			send_meas_step('0');
//			key_buzzer();
			if(state==M_MEASURE) dsp_standby();
			tsw_start();
		}
	}
	rx3_cnt=0;
}
void send_meas_step(u8 step){
	savcmd3('*');				//STEP1,2,3,4,5 ????
	savcmd3(0x53);
	savcmd3(step);
	savcmd3(URLF);
}
/********************************************/
void com_meas_result(void){
u16 i;
	if((rx3_cnt==2)&&(com3dat==URLF)){
printf0("\n Send Meas_result");		
		//StoreResult시에 cmdbuff에 data를 정리해 두었다.
		for(i=0;i<(RESULT_SIZE-1);i++){ savcmd3(cmdbuff[i]); }
		if((buffRESULT[0]=='*')&&(buffRESULT[1]==0x54)){
			for(i=0;i<RESULT_SIZE;i++){ savcmd3(buffRESULT[i]); }
		}else savcmd3(URLF);
		wait_tx3();
		key_buzzer();
	}
	rx3_cnt=0;
}

void com_uid_set(void){
	if(rx3_cnt<(2+14)){
		if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
		rx3_cnt++;
	}else if((com3dat==URLF)&&(rx3_cnt==16)){
		savcmd3('*');
		savcmd3(0x55);
		savcmd3(URLF);
		key_buzzer();
		store_uid();
		rx3_cnt=0;
	}else rx3_cnt=0;
}
/********************************************/
void com_curve_read(void){
printf0("\n CURVE_READ : %d", rx3_cnt);
	if((rx3_cnt==2)&&(com3dat==URLF)){
		send_curve();	
		key_buzzer();
	}
	rx3_cnt=0;
}
/********************************************/
void com_curve_write(void){
	if(!write_step){
		if((rx3_cnt==2)&&(com3dat==URLF)){
			savcmd3('*');
			savcmd3(0x62);
			savcmd3('1');
			savcmd3(URLF);
			write_step=1;
		}
		rx3_cnt=0;
	}else{
		if(rx3_cnt<(2+216)){
			if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
			rx3_cnt++;
			rx3_wait=RX3_WAIT;
		}else if((com3dat==URLF)&&(rx3_cnt==218)){
			savcmd3('*');	savcmd3(0x62);	savcmd3('2');	savcmd3(URLF);
			store_curve();
			savcmd3('*');	savcmd3(0x62);	savcmd3('3');	savcmd3(URLF);
			rx3_cnt=0;
			write_step=0;
			key_buzzer();
		}else rx3_cnt=0;		
	}
}
/********************************************/
void com_spd_read(void){
printf0("\n SPEED_READ : %d", rx3_cnt);
	if((rx3_cnt==2)&&(com3dat==URLF)){
//		send_speed();	
printf0("\n SEND SPEED_READ : %d", rx3_cnt);
		savcmd3('*');
		savcmd3(COM_SPD_READ);
		if(!SPEED_FLAG) savcmd3('0'); else savcmd3('1'); 
		savcmd3(URLF);	
		rx3_cnt=0;
		key_buzzer();
	}else rx3_cnt=0;
}

void com_spd_write(void){
	if(rx3_cnt<3){
		cmdbuff[0]=com3dat;
		rx3_cnt++;
	}else if((com3dat==URLF)&&(rx3_cnt==3)){
		SPEED_FLAG = cmdbuff[0]-'0'; 
		savcmd3('*');	savcmd3(0x57);	
		if(!SPEED_FLAG) savcmd3('0'); else savcmd3('1'); 
		savcmd3(URLF);
		rx3_cnt=0;
		write_step=0;
		write24(SPEED_ADR, SPEED_FLAG);
		write_id_chksum();
		key_buzzer();
//		system_backup(ID55AA_ADR, ID_CHKSUM+2);
	}else rx3_cnt=0;		
}

void com_double_read(void){
printf0("\n DOUBLE_READ : %d", rx3_cnt);
	if((rx3_cnt==2)&&(com3dat==URLF)){
//		send_speed();	
printf0("\n SEND DOUBLE_READ : %d", rx3_cnt);
		savcmd3('*');
		savcmd3(COM_DOUBLE_READ);
		if(DOUBLE_FLAG==1) savcmd3('1'); else savcmd3('0'); 
		savcmd3(URLF);	
		rx3_cnt=0;
		key_buzzer();
	}else rx3_cnt=0;
}

void com_double_write(void){
	if(rx3_cnt<3){
		cmdbuff[0]=com3dat;
		rx3_cnt++;
	}else if((com3dat==URLF)&&(rx3_cnt==3)){
		DOUBLE_FLAG = cmdbuff[0]-'0'; 
		savcmd3('*');	savcmd3(COM_DOUBLE_WRITE);	
		if(DOUBLE_FLAG==1) savcmd3('1'); else savcmd3('0'); 
		savcmd3(URLF);
		rx3_cnt=0;
		write_step=0;
		write24(DOUBLE_ADR, DOUBLE_FLAG);
		write_id_chksum();
		key_buzzer();
//		system_backup(ID55AA_ADR, ID_CHKSUM+2);
	}else rx3_cnt=0;		
}
/********************************************/
void com_pform_read(void){
u16 i;	
	if((rx3_cnt==2)&&(com3dat==URLF)){
printf0("\n SEND PFORM_READ : %d", rx3_cnt);
		savcmd3('*');
		savcmd3(COM_PFORM_READ);
		for(i=0;i<PFORM_SIZE;i++){
			savcmd3( STORE_RAM[PFORM_RAM+i] );		
//			printf0("%c", STORE_RAM[PFORM_RAM+i] );		
		}
		savcmd3(URLF);	
		key_buzzer();
		wait_tx3();
		rx3_cnt=0;
	}else rx3_cnt=0;
}
/********************************************/
void com_pform_write(void){
u16 i,j;
	if(!write_step){
		if((rx3_cnt==2)&&(com3dat==URLF)){
			savcmd3('*');
			savcmd3(COM_PFORM_WRITE);
			savcmd3('1');
			savcmd3(URLF);
			write_step=1;
			rx3_wait=RX3_WAIT*6;
			printf0("\n Com set meas");
		}else rx3_wait=0;
		rx3_cnt=0;
	}else{
		if(rx3_cnt<(2+PFORM_SIZE)){
			if(rx3_cnt>=2) cmdbuff[rx3_cnt-2] = com3dat;
			rx3_cnt++;
			rx3_wait=RX3_WAIT;
//printf0("%2x",com3dat);				//4044ea
		}else if(com3dat==URLF){
			printf0("\n Com meas write");
			savcmd3('*');	savcmd3(0x72);	savcmd3('2');	savcmd3(URLF);
			j=0;
			for(i=0;i<PFORM_SIZE;i++){
				STORE_RAM[PFORM_RAM+i] = cmdbuff[i];
				j += cmdbuff[i];
			}
			j = ~j;
			STORE_RAM[PFORM_RAM+i+0] = j>>8;
			STORE_RAM[PFORM_RAM+i+1] = j;
			printf0("\t SumB:%4x", j);
			system_write(PFORM_RAM,PFORM_SIZE+2);
			savcmd3('*');	savcmd3(0x72);	savcmd3('3');	savcmd3(URLF);
			rx3_wait=rx3_cnt=0;
			write_step=0;
			key_buzzer();
			move_pform();
		}else rx3_cnt=0;
	}
}
void com_time_write(void){
	if(rx3_cnt<14){
		cmdbuff[rx3_cnt]=com3dat;
		rx3_cnt++;
	}else if((com3dat==URLF)&&(rx3_cnt==14)){
		setyear = cmdbuff[2]-'0';
		setyear = (setyear*10) + (cmdbuff[3]-'0');
		setmonth= cmdbuff[4]-'0';
		setmonth= (setmonth*10) + (cmdbuff[5]-'0');
		setday  = cmdbuff[6]-'0';
		setday  = (setday*10) + (cmdbuff[7]-'0');
		sethour = cmdbuff[8]-'0';
		sethour = (sethour*10) + (cmdbuff[9]-'0');
		setmin  = cmdbuff[10]-'0';
		setmin  = (setmin*10) + (cmdbuff[11]-'0');
		setsec  = cmdbuff[12]-'0';
		setsec  = (setsec*10) + (cmdbuff[13]-'0');
		sendAll_RTC();		
		savcmd3('*');	
		savcmd3(COM_TIME_WRITE);	
		savcmd3(URLF);
		key_buzzer();
		rx3_cnt=0;
		write_step=0;
	}else rx3_cnt=0;		
}

/********************************************/
//                01234567890123456789
u8 Version[22] = "Version:000 13-01-01";
void com_version_read(void){
u16 i;
printf0("\n Version_READ : %d", rx3_cnt);
	if((rx3_cnt==2)&&(com3dat==URLF)){
printf0("\n SEND Version : %d", rx3_cnt);
		Version[8]  = (*(U8 *)(VersionAdr+7));
		Version[9]  = (*(U8 *)(VersionAdr+8));
		Version[10] = (*(U8 *)(VersionAdr+9));
		Version[11] = ' ';
		Version[12] = (*(U8 *)(VersionAdr+24));
		Version[13] = (*(U8 *)(VersionAdr+25));
		Version[14] = '-';
		Version[15] = (*(U8 *)(VersionAdr+27));
		Version[16] = (*(U8 *)(VersionAdr+28));
		Version[17] = '-';
		Version[18] = (*(U8 *)(VersionAdr+30));
		Version[19] = (*(U8 *)(VersionAdr+31));
		savcmd3('*');
		savcmd3(COM_VERSION_READ);
		for(i=0;i<12;i++) savcmd3(Version[i+8]);
		savcmd3(URLF);
		key_buzzer();
		rx3_cnt=0;
	}else rx3_cnt=0;
}

/********************************************/
void com_time_read(void){
u32 i;
printf0("\n Time_READ : %d", rx3_cnt);
	if((rx3_cnt==2)&&(com3dat==URLF)){
printf0("\n SEND Time : %d", rx3_cnt);
		savcmd3('*');
		savcmd3(COM_TIME_READ);
		i = bin2bcdasc4(year);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(month);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(day);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(hour);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(min);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(sec);	savcmd3(i>>8);	savcmd3(i);
		savcmd3(URLF);
		key_buzzer();
		rx3_cnt=0;
	}else rx3_cnt=0;
}

/********************************************/
void com_para_read(void){
	if((rx3_cnt==2)&&(com3dat==URLF)){
		send_para();
		key_buzzer();
	}
	rx3_cnt=0;
}
/********************************************/
void com_para_write(void){
	if(!write_step){
		if((rx3_cnt==2)&&(com3dat==URLF)){
			savcmd3('*');
			savcmd3(0x64);
			savcmd3('1');
			savcmd3(URLF);
			write_step=1;
			rx3_wait=RX3_WAIT*6;
	printf0("\n set para write2");
		}else rx3_wait=0;
		rx3_cnt=0;
	}else{
		if(rx3_cnt<(2+PARA_SIZE)){
			if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
			rx3_cnt++;
			rx3_wait=RX3_WAIT;
//printf0("%2x",com3dat);				//4044ea
//		}else if((com3dat==URLF)&&(rx3_cnt==(2+WEIGHT_SIZE+PARA_SIZE))){
		}else if(com3dat==URLF){
	printf0("\n Com para write2");
//for(i=0;i<1496;i++){	printf0("%2x",cmdbuff[i+2]);	delay_ms(10); }
			savcmd3('*');	savcmd3(0x64);	savcmd3('2');	savcmd3(URLF);
			store_para();
			savcmd3('*');	savcmd3(0x64);	savcmd3('3');	savcmd3(URLF);
			rx3_wait=rx3_cnt=0;
			write_step=0;
			key_buzzer();

		}else rx3_cnt=0;		
	}
}
/********************************************/
void com_strip_read(void){
	if((rx3_cnt==2)&&(com3dat==URLF)){
		send_strip();	
		key_buzzer();
	}
	rx3_cnt=0;
}
/********************************************/
void com_strip_write(void){
	if(!write_step){
		if((rx3_cnt==2)&&(com3dat==URLF)){
			savcmd3('*');
			savcmd3(0x66);
			savcmd3('1');
			savcmd3(URLF);
			write_step=1;
			rx3_wait=RX3_WAIT*6;
printf0("\n Set_strip_write");
		}else rx3_wait=0;
		rx3_cnt=0;
	}else{
		if(rx3_cnt<(2+STRIP_SIZE)){
			if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
			rx3_cnt++;
			rx3_wait=RX3_WAIT;
		}else if(com3dat==URLF){
printf0("\n Com_strip_write");
			savcmd3('*');	savcmd3(0x66);	savcmd3('2');	savcmd3(URLF);
			store_strip();
			savcmd3('*');	savcmd3(0x66);	savcmd3('3');	savcmd3(URLF);
			rx3_wait=rx3_cnt=0;
			write_step=0;
			key_buzzer();
		}else{
			rx3_wait=rx3_cnt=0;		
		}
	}
}
/********************************************/
void com_strip_no_rd(void){
u32 i;	
	if((rx3_cnt==2)&&(com3dat==URLF)){
		savcmd3('*');
		savcmd3(0x67);
		i = bin2hexasc4(STRIPNO);
		savcmd3(i>>8);
		savcmd3(i>>0);
		savcmd3(URLF);
		key_buzzer();
	}
	rx3_cnt=0;
}
/********************************************/
void com_strip_no_wr(void){
	if(!write_step){
		if((rx3_cnt==2)&&(com3dat==URLF)){
printf0("\n StripNO_write");
			savcmd3('*');
			savcmd3(0x68);
			savcmd3('1');
			savcmd3(URLF);
			write_step=1;
			rx3_wait=RX3_WAIT*6;
		}
		rx3_cnt=0;
	}else{
		if(rx3_cnt<(2+2)){
			if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
			rx3_cnt++;
		}else if((com3dat==URLF)&&(rx3_cnt==4)){
			savcmd3('*');
			savcmd3(0x68);
			savcmd3('2');
			savcmd3(URLF);
			rx3_wait=rx3_cnt=0;
			write_step=0;
			store_strip_no();
//			key_home();
			key_buzzer();
		}else rx3_cnt=0;		
	}
}
/********************************************/
void com_full_raw_data(void){		//X3하여 평균낸 데이타를 보낸다.
u32 i, j;
	if((rx3_cnt==2)&&(com3dat==URLF)){
		key_buzzer();
		savcmd3('*');
		savcmd3(0x69);
		for(i=0;i<MaxRawData;i++){
			j = bin2hexasc4(spanRED[i]);
			savcmd3(j>>24);	savcmd3(j>>16);	savcmd3(j>>8); savcmd3(j>>0);
			j = bin2hexasc4(spanGRN[i]);
			savcmd3(j>>24);	savcmd3(j>>16);	savcmd3(j>>8); savcmd3(j>>0);
			j = bin2hexasc4(spanBLU[i]);
			savcmd3(j>>24);	savcmd3(j>>16);	savcmd3(j>>8); savcmd3(j>>0);
			wait_tx3();
		}		
		if(DOUBLE_FLAG){
			for(i=0;i<MaxRawData;i++){
				j = bin2hexasc4(spanRED[i]);
				savcmd3(j>>24);	savcmd3(j>>16);	savcmd3(j>>8); savcmd3(j>>0);
				j = bin2hexasc4(spanGRN[i]);
				savcmd3(j>>24);	savcmd3(j>>16);	savcmd3(j>>8); savcmd3(j>>0);
				j = bin2hexasc4(spanBLU[i]);
				savcmd3(j>>24);	savcmd3(j>>16);	savcmd3(j>>8); savcmd3(j>>0);
				wait_tx3();
			}		
		}
		savcmd3(URLF);
		key_buzzer();
	}
	rx3_cnt=0;
}
/********************************************/
void send_curve(void){
u16 k;
printf0("\n SEND CURVE_READ : %d", rx3_cnt);
	savcmd3('*');
	savcmd3(COM_CURVE_READ);
	for(k=0;k<CURVE_SIZE;k++){ 
		savcmd3(STORE_RAM[CURVE_RAM+k]); 
	}
	savcmd3(URLF);	
	wait_tx3();
	rx3_cnt=0;
}
/********************************************/
void send_para(void){
u16 j;
printf0("\n Send Para");
	savcmd3('*');
	savcmd3(COM_PARA_READ);
	for(j=0;j<(PARA_SIZE+WEIGHT_SIZE);j++){
		savcmd3(STORE_RAM[PARA_RAM+j]); 
		wait_tx3();
	}
	savcmd3(URLF);	
	rx3_cnt=0;
}
/********************************************/
void send_strip(void){
u16 i, k;
printf0("\n SendStrip");
	savcmd3('*');
	savcmd3(COM_STRIP_READ);
	k = STRIP_RAM; 
	for(i=0;i<STRIP_SIZE;i++){ 
		savcmd3(STORE_RAM[k++]); 
	}
	savcmd3(URLF);
	wait_tx3();
	rx3_cnt=0;
}
/********************************************/
void com_msg(char *str){
}
/********************************************/
//=========================================================================================
u16 hexasc2bin4(u32 asc){
u16 i, j;
	j = ((asc>>24)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = j;
	j = ((asc>>16)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = (i<<4) + j;
	j = ((asc>>8)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = (i<<4) + j;
	j = ((asc>>0)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = (i<<4) + j;
	return(i);
}
/********************************************/
/********************************************/
/********************************************/
/********************************************/
/********************************************/
