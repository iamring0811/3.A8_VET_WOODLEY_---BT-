//Spider	rom	81000000	00000000
//			ram	A0000000	007FFFFF
#include <stdio.h>
#include <stdlib.h>
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

void chk_rxs(void);
void blink_loop(void);

void Main(void){
int xx, i;
	VICIntEnable=0;				//
	EMCControl= 0x00000001;		/*Disable Address mirror*/
	GpioInit();
	LBLEN_ENABLE();				//Power 33V
	LCD_BK_ON();				//BackLight
	MemInit();
	init_adc();
	stepM_init();
	RGB_ALL_OFF();
	Uart_Init_All();
	Timer0Init();
	initRTC();
	chk_auto_balance();	//20150519
	On_Interrupt();				//	INTC_ISR=0x00;	sti();		//Interrupt ON
	printf0("\n Start Program at SDRAM");
//	enable_watchdog();			//while(1){ if(tm_10ms){ tm_10ms=0;	WDT_CNT=0x00; } }

	ProgVer = get_ProgVer();
//	init_lcd();
	move_font();
	init_fonts();						// wiat_lcd();
	move_picture();
	init_bmps();						// wiat_lcd();
//	copy_lcdbase();						// wiat_lcd();
	dsp_first();
	LED_GRN_OFF(); LED_RED_ON();
    BUZZ_HI();	dur_PWM = 5;			// key_buzzer();

	for(xx=0;xx<3;xx++) if(read_jedec()==OK) break;
//	test_flash(3);
	EN_BLUE();
	initEEP();
	power_on();
	for(i=0;i<MAX_USERID;i++){
		tmp_idno[i]=USER_ID[i]=' ';
	}
	dsp_standby();
//while(1){ key_decode();	prog_decode(); }

	flag_flash=0;
	tm_500ms=0;
//	if(SYS_ID[0]=='B'){
		init_blu();
//	}


//view_result_store(4); while(1);

//while(1){ delay_ms(10); BUZZ_HI(); delay_ms(10); BUZZ_LO(); }
	if(BALANCE_AUTO) read_balance(); //20150519
	for(;;){
		if(tm_10ms){
			tm_10ms=0;
//			readTime();
			chk_rxs();
			blink_loop();
		}else if(tm_500ms>50){
//			tm_500ms-=50;
			tm_500ms=0;
			readTime();
//			LED_GRN_BAR();  
			LED_RED_BAR();
//printf0(" *%2x", xx++);
		}else if(!flag_flash){
			key_decode();
//			if(rxw1!=rxr1) savcmd0(getcmd1());
		}
		prog_decode();
	}
}

//extern void uslave7202_interrupt(void);

/***************************************************************************/
void blink_loop(void){
u8 b1;
	if(flag_clean==0x55){
		blink_dur++;
		if(blink_dur==30){
//			erase_box(140,340,140+120,340+65);			
		}else if(blink_dur>=60){
			blink_dur = 0;
			LCDbmpxx(b_long, 140,88+(36*5)+4+(68*1));			
			LCDbmpxx(b_short, 140,88+(36*5)+4);			
//			LCDbmpxx(b_clean, 140,88+(36*5)+4+(68*1));			
		}
	}else{
		if((state==M_M_SENDNO)||(state==M_M_DELNO)){
			blink_dur++;
			if(blink_dur==30){
				if(!cursor){					//start
					LCDbmpxx(b_start3, 20,88);					
				}else{							//end
					LCDbmpxx(b_end, 20,88+36);					
				}
			}else if(blink_dur>=60){
				blink_dur = 0;
				if(!cursor){					//start
					b1 = put_seqno(mem_srt);
					msg20(130+b1,92,BLK,top_str);	
				}else{							//end
					b1 = put_seqno(mem_end);
					msg20(130+b1,92+36,BLK,top_str);
				}
			}
		}
	}
}

/***************************************************************************/
void chk_rxs(void){
	if(rx3_wait){ 
		if(!(--rx3_wait)){ 
			rx3_cnt=0; rx3_step=0; 
			write_step=0; 
			if(flag_flash==3) flag_flash=0;
		} 
	}
	if(rx0_wait){
		if((--rx0_wait)==0){
			rx0_cnt=0;	rx0_step=0; 
			if(flag_flash==1) flag_flash=0;
		}
	}
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************
1. send data 	start ==> 1  																					ok
2. del. data 	start ==> 1  																					ok
3. srt�� end���� ũ�� msgǥ���� end�� srt�� ����																ok
4. �Է¹�ȣ�� ��ϵ���Ÿ���� ũ�� ����ǥ���� ������ ��ȣ ǥ��													ok
5. send�� delete�� �����ȣ ǥ��																				ok
6. ��Ʈ��,Į��,�ͺ� ���ý� ���õ� ��ȣ�� ǥ�� ==> �׽�Ʈ�� ���� ����											ok
7. ���嵥������ �������̵� �ڿ� 10ĭ ���� 3�ڸ��� IDȮ��, 5���� ����, 1���� Į��, 1���� �ͺ�� ���.
8. Ŭ���� ��Ʈ�������� ������ ��� �����̰� �ٸ�Ű�� �����ʴ´�.  												ok
9. seq�� 2000 ������ ����ǥ���� 2000ǥ��.																		ok
10. ��Ʈ������ ������ ����ǥ��																					ok
11. ���ÿ��� �������� ��ü																						ok
12. %R���� si/conv print formǥ��
13. User ID��ȣ�ǿ��� ����ȭ��ǥ ==> 'C'�� ����																	ok

1.���ͼӵ�
2.����
3.������

***************************************************************************/
/***************************************************************************/
