#define _INIT_C_
#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\init.h"
#include ".\inc\uart.h"
#include ".\inc\flash.h"
#include ".\inc\RESM.h"
#include ".\inc\LCD.h"
#include ".\inc\Touch.h"
#include ".\inc\com.h"
#include ".\inc\sysadj.h"
#include ".\inc\dsp.h"
#include ".\inc\eep.h"

#define UART0_THR	U0THR
#define UART1_THR	U1THR
#define UART2_THR	U2THR
#define UART3_THR	U3THR

void run_CW(void);
void run_CCW(void);

u8 RGB_FLAG;

void __irq IRQ_Timer0(void)         			//Timer0 interrupt 210[us] ?????
{
register u16 ii;
register u8 b1;

	LED_GRN_BAR();

	switch(smscan&7){
		case 0: 							//1679.6875[us]
//20150519 start
//		  	if(srt_step) srt_step--;
//		  	if(!(srt_step&1)){
				switch(div_adc&0x7){
					case 0: 
						if(!flag_dac) RGB_ALL_OFF();//20150519
						if(flag_adc){ 
							if(BALANCE_AUTO) out_dac(dac_r); RGB_RED_ON(); 
						}
					break;	
					case 2: 
						if(flag_adc){
							buffRED1[pnt_adc]=mpc3201();
							RGB_ALL_OFF();
							if(BALANCE_AUTO) out_dac(dac_g); RGB_GRN_ON();
						}
					break;	
					case 4: 
						if(flag_adc){
							buffGRN1[pnt_adc]=mpc3201();
							RGB_ALL_OFF();
							if(BALANCE_AUTO) out_dac(dac_b); RGB_BLU_ON();
						}
					break;	
					case 6:
						if(flag_adc){
							buffBLU1[pnt_adc]=mpc3201();
							RGB_ALL_OFF();
							pnt_adc++;
						}
					break;	

       		default:   //1,3,5,7
            if(txw0!=txr0) UART0_THR=SCTX0_Buf[txr0++];                //38400bps
            if(txw3!=txr3){ 
              b1=SCTX3_Buf[txr3++];                     
              UART3_THR=b1;                  
              UART1_THR=b1;                  
            }
        break;
        }
        div_adc++;
        adc_read();                                                                                                                                 //420[us] touch    ==> �߰�
 		break;                   
 					
//20150519 end
		case 2:
//			if(UART2_LSR&0x01) SCRX2_Buf[rxw2++]=UART2_RBR;	//9600bps
			while(!(U2IIR&0x01)){
				ii=U2IIR&0x0E;
				if((ii==0x04)||(ii==0x0C)){
					if ((U2LSR&0x01)==0x01)	SCRX2_Buf[rxw0++]=U2RBR;		//9600bps			
				}
			}
			time_ref += 1680;								//210*8
			if(time_ref>=10000){							
				time_ref -= 10000;
				tm_10ms++;
				if(dur_PWM){ if(!(--dur_PWM)) BUZZ_LO(); }	//b6:PWM1
				tm_500ms++;
				key_scan();
//			    WDFEED = 0xAA;	WDFEED = 0x55;   	// WDT clear
			}
		break;
		case 4:
			if(txw2!=txr2) UART2_THR=SCTX2_Buf[txr2++];		//9600bps
			//adc_read();										//840[us] touch		2015051
			if(txw0!=txr0) UART0_THR=SCTX0_Buf[txr0++];                //38400bps
			if(txw3!=txr3){ 
			         b1=SCTX3_Buf[txr3++];                     
			         UART3_THR=b1;                  
			         UART1_THR=b1;                  
			}
		break;
		case 6:
			if(SmDir==SM_CCW) run_CCW();
			else if(SmDir==SM_CW) run_CW();
//			if(UART2_LSR&0x01) SCRX2_Buf[rxw2++]=UART2_RBR;	//9600bps
			while(!(U2IIR&0x01)){
				ii=U2IIR&0x0E;
				if((ii==0x04)||(ii==0x0C)){
					if ((U2LSR&0x01)==0x01)	SCRX2_Buf[rxw0++]=U2RBR;		//9600bps			
				}
			}
		break;
		default:	//1,3,5,7
			adc_read();                                                                                                                                 
		break;		
	}
//	while(UART0_LSR&0x01) SCRX0_Buf[rxw0++]=UART0_RBR;		//38400bps DEBUG
	while(!(U0IIR&0x01)){
		ii=U0IIR&0x0E;
		if((ii==0x04)||(ii==0x0C)){
			if ((U0LSR&0x01)==0x01)	SCRX0_Buf[rxw0++]=U0RBR;		//38400bps			
		}
	}
//	while(UART3_LSR&0x01) SCRX3_Buf[rxw3++]=UART3_RBR;		//38400bps USB
	while(!(U3IIR&0x01)){
		ii = U3IIR&0x0E;
		if((ii==0x04)||(ii==0x0C)){
			if ((U3LSR&0x01)==0x01)	SCRX3_Buf[rxw3++]=U3RBR;
		}
	}
//	while(UART1_LSR&0x01){
//		if(flag_blu) SCRX1_Buf[rxw1++]=UART1_RBR;		//38400bps
//		else SCRX3_Buf[rxw3++]=UART1_RBR;				//38400bps
//	}
	while(!(U1IIR&0x01)){
		ii = U1IIR&0x0E;
		if((ii==0x04)||(ii==0x0C)){
			if ((U1LSR&0x01)==0x01){
				if(flag_blu) SCRX1_Buf[rxw1++]=U1RBR;		//38400bps
				else SCRX3_Buf[rxw3++]=U1RBR;				//38400bps
			}
		}
	}

	t0cnt++;											//T0_interrupt();
	smscan++;
	if(t0dly) t0dly--;
	
	T0IR        = 0x01;					//
	VICVectAddr = 0x00;					//
}
/*******************************************************************************************/

void run_CCW(void){
	switch(SmStep){
		case 0:  SM_CH2_OFF(); SM_CH4_ON(); SmStep=1; break;		//����
		case 1:  SM_CH1_OFF(); SM_CH3_ON(); SmStep=2; break;
		case 2:  SM_CH4_OFF(); SM_CH2_ON(); SmStep=3; break;
		default: SM_CH3_OFF(); SM_CH1_ON(); SmStep=0; 
			if(!(--SmCnt)){SmDir=0;SM_CH1_OFF();SM_CH2_OFF();SM_CH3_OFF();SM_CH4_OFF();}
		break;
	}
}
/*******************************************************************************************/

void run_CW(void){
	switch(SmStep){
		case 0:	 SM_CH3_OFF(); SM_CH1_ON(); SmStep=1; break;		//����
		case 1:  SM_CH4_OFF(); SM_CH2_ON(); SmStep=2; break;
		case 2:  SM_CH1_OFF(); SM_CH3_ON(); SmStep=3; break;
		default: SM_CH2_OFF(); SM_CH4_ON(); SmStep=0; 
			if(!(--SmCnt)){SmDir=0;SM_CH1_OFF();SM_CH2_OFF();SM_CH3_OFF();SM_CH4_OFF();}
		break;
	}
}
/*******************************************************************************************
	//				 10987654 32109876 54321098 76543210
//	PINSELx  = BIN32(00000000,00000000,00000000,00000000);
*******************************************************************************************/
void GpioInit(void){
//	SCS     &= 0xFFFFFFFE;			/* select  IO */
	SCS     |= 0x00000001;			/* select FIO */

//	PINSEL0 &= ~((0xFFF<<8)|(3<<28));
	//				 10987654 32109876 54321098 76543210
	PINSEL0  = BIN32(00001111,01010000,00000000,01011010);	//AD7,6 TRX2,TRX0,TRX3
//	PINSEL1 &= ~((0xFFF<<2)|(0xFF<<22)|0XC0000000);
	//				 10987654 32109876 54321098 76543210
	PINSEL1  = BIN32(01010100,00000000,01000000,00000000);	//USB, AD0
	FIO0DIR &= ~P0OUT;		//
	FIO0DIR |= P0OUT;		//
	FIO0CLR  = (SM_CH4|SENS_PW);
	FIO0SET  = (DSIO|DSCK|DS_EN);

//	PINSEL2  = 0x00000000;
//	PINSEL3  = 0x05555500;
	PINSEL3  = PINSEL3 & 0xFFFFFF00;		//p1.15,16,17,18
	FIO1DIR &= ~P1OUT;	//
	FIO1DIR |= P1OUT;		//
	FIO1CLR  = (SM_CH3|BUZZ|AD_CLK);
	FIO1SET  = (LED_G|AD_EN|RGB_RED|RGB_GRN|RGB_BLU);

//	PINSEL4 &= ~((0xF<<20)|0XC0000000);
//	PINSEL4  = 0x000FFFF3;
//	PINSEL5 &= ~((3<<6)|(3<<14)|(0x3F<<18));
//	PINSEL5  = 0x05010115;			//SDRAM
	FIO2DIR &= ~P2OUT;				//LED(red & green)
	FIO2DIR |= P2OUT;				//LED(red & green)
	FIO2CLR  = (SM_CH2|LCD_BK);
	FIO2SET  = (CE24|DI24);

	PINSEL6  = 0x55555555;			//d0~d15
	PINSEL7  = 0x0000000F;
	FIO3DIR &= ~P3OUT;				//b24:pwm1,b25:velocity
	FIO3DIR |= P3OUT;				//b24:pwm1,b25:velocity
	FIO3CLR  = (SM_CH1);
	FIO3SET  = (WP24|CK24|LED_R);

	PINSEL8  = 0x55555555;			//A1~A15
	PINSEL9  = 0x40050555;			//A16~A21,P4.22,P4.23,OE,WE,P4.26..29CS1
	FIO4DIR &= ~P4OUT;				//
	FIO4DIR |= P4OUT;				//
	FIO4SET  = (LBL_EN|RSTBLU);

	RGB_ALL_OFF(); 
}
/*******************************************************************************************/
void LED_GRN_BAR(void){
	if(FIO1PIN&LED_G){
		LED_GRN_ON();	
	}else{
		LED_GRN_OFF();	
	}
}
/*******************************************************************************************/
void LED_RED_BAR(void){
	if(FIO3PIN&LED_R){
		LED_RED_ON();	
	}else{
		LED_RED_OFF();	
	}
}
/*******************************************************************************************/

void MemInit(void){
	state = M_INIT;
	flag_flash=0;
	tm_10ms=tm_500ms=0;
	rx0_step=0;
	flag_adc=0;
	pnt_adc=0;
	test_no=0;
	rx3_cnt=0;
	write_step=0;
	COMCMD=0;
	tchy=tchx=0;
//	flag_clean=0;
}
/*******************************************************************************************/
void Timer0Init(void){
	T0TC    = 0;						// 0										
	T0PR    = 0;						// 											
	T0MCR   = 0x03;						// T0MR0,T0TC				
//	T0MR0   = Fpclk / 2;				// 0.5[sec]
//	T0MR0   = Fpclk / 4900;				// 200[us]
	T0MR0   = Fpclk / 4700;				// 210[us] = 4761.9
	T0TCR   = 0x01;						// 										
	
	IRQEnable();						// IRQ

	VICIntSelect = 0x00;				// Select IRQ:0, FEQ:1
	VICVectPri4  = 00;					//
	VICVectAddr4 = (u32)IRQ_Timer0;		//
//	VICIntEnable = 1 << 0x04;			//

	t0cnt=0;
}
/*******************************************************************************************/
void On_Interrupt(void){
int i;
u32 *pos, *dst;
    pos = (u32*)0xA0000000;
    dst = (u32*)0x40000000;
	for(i=0;i<40;i++){ *(dst++) = *(pos++); }
	sti();								// VICIntEnable = 1 << 0x04;
}

/*******************************************************************************************/
void delay_ms(int dly){ 
	T1TCR   = 0x02;						// count stop					
	T1TC    = 0;						// 0										
	T1PR    = 0;						// 											
	T1MCR   = 0x03;						// T0MR0,T0TC				
//	T0MR0   = Fpclk / 2;				// 0.5[sec]
//	T0MR0   = Fpclk / 7840;				// 250[us]
//	T1MR0   = Fpclk / 1960;				// 0.5[ms]
	T1MR0   = Fpclk /  980;				// 1.0[ms]
	T1IR 	= 0x01;				
	T1TCR   = 0x01;						// count run after reset
	while(dly){
		while ((T1IR & 0x01) == 0);	
		T1IR = 0x01;				
		dly--;
	}
}
/*******************************************************************************************/
void delay_set(int dly){ 
#ifdef	LPC40M
	dly = (dly*100) / ((65*1900)/1150);
#else
	dly = (dly*100) / 65;
#endif
	t0dly = dly;
	t0dly = dly;
}

/*******************************************************************************************/
void delay_us(int dly){ 
	T1TCR    = 0x02;					// count stop					
	T1TC    = 0;						// 0										
	T1PR    = 0;						// 											
	T1MCR   = 0x03;						// T0MR0,T0TC				
	T1MR0   = Fpclk / 1000000;			// 1.0[us]
	T1IR 	= 0x01;				
	T1TCR   = 0x01;						// count run after reset
	while(dly){
		while ((T1IR & 0x01) == 0);	
		T1IR = 0x01;
		dly--;
	}
}

/*******************************************************************************************/
void key_buzzer(void){
	if(SOUND_FLAG){
	    BUZZ_HI();
		dur_PWM = 5;
	}
}
/*******************************************************************************************/
void move_font(void){
int i;
u32 *pos, *dst;
    pos = (u32*)FNT_ROM;
    dst = (u32*)FNT_BASE;
	for(i=0;i<((0x80000-0x50000)/4);i++){ *(dst++) = *(pos++); }
}

/*******************************************************************************************/
void move_picture(void){
int i;
u32 *pos, *dst;
    pos = (u32*)BMP_ROM;
    dst = (u32*)BMP_RAM;
	for(i=0;i<((0x200000-0x080000)/4);i++){ *(dst++) = *(pos++); }
}

/*******************************************************************************************/
U32 get_ProgVer(void){
u32 PV;	
u8 *pos_s;
    pos_s = (u8*)(VersionAdr);	
	PV = *(pos_s+7)-'0';
	PV = (PV*10) + *(pos_s+8)-'0';
	PV = (PV*10) + *(pos_s+9)-'0';
	//2015-06-09 ���� 4:45:57 Start
	ver_str[0] = *(pos_s+7);
	ver_str[1] = *(pos_s+8);
	ver_str[2] = *(pos_s+9);
	//2015-06-09 ���� 4:46:04 End  
	printf0("\n Ver.%3d",PV);
	return(PV);
}
/*******************************************************************************************/
void init_adc(void){				/* ADC */
u32 i;
	PCONP  |= 1 << 12;						// ADC Power 
	PINSEL0  = (PINSEL0 & 0xF0FFFFFF) | (0x0F << 24);	// P0.12=AD6, P0.13=AD7
	PINSEL1  = (PINSEL1 & 0xFFFF3FFF) | (0x01 << 14);	// P0.23=AD0
	AD0CR = (1 << 4)					 |	// SEL=0x10,ADC0.4
			((Fpclk / 1000000 - 1) << 8) |	// CLKDIV=Fpclk/1000000-1=1MHz
			(1 << 16)					 |	// BURST=1,Burst
			(0 << 17)					 |	// CLKS=0, 11clocks 10bit
			(1 << 21)					 |  // PDN=1,ADC Run
			(0 << 24);						// START=0,No Start
	delay_ms(1);
	i = AD0GDR;								// AD dune
	ATSYP_HI(); ATSYN_LO();	ATSXP_LO();	ATSXN_HI();	AD0CR = SET_ADC0; 
	adc_cnt = adc_scan = touch_flag = 0;
	ad6_pr1 = ad7_pr1 = ad6_pr2 = ad7_pr2 = 0;
	ad6_dat = ad6_sum = ad7_dat = ad7_sum = 0;
}
/*******************************************************************************************/
#define TD	10

void adc_read(void){
register u16 ii;
	adc_scan++;	
	switch(adc_scan){
		case  1: 
			ATSXP_HI(); ATSXN_LO();	ATSYP_LO();	ATSYN_HI();	AD0CR = SET_ADC6; 
		break;
		case  2: 
			AD0CR |= 1 << 24; 			// Start Conversion
		break;						
		case  3: 
			ii = AD0GDR;	ad6_pr1 = ad6_pr2; ad6_pr2 = (ii>>6) & 0x3FF;
			ATSYP_HI(); ATSYN_LO();	ATSXP_LO();	ATSXN_HI();	AD0CR = SET_ADC7; 
		break;
		case  4: 
			AD0CR |= 1 << 24;			// Start Conversion
		break;				
		case  5: 
			ii = AD0GDR;	ad7_pr1 = ad7_pr2; ad7_pr2 = (ii>>6) & 0x3FF;	adc_scan++;	
			ATSYP_HI(); ATSYN_LO();	ATSXP_HI();	ATSXN_LO();
		break;				
		default:
			ii = 0;
			if(ad6_pr1>ad6_pr2){ if(ad6_pr1>(ad6_pr2+TD)) ii=1; } 
			else{ if(ad6_pr2>(ad6_pr1+TD)) ii=1; } 
			if(ad7_pr1>ad7_pr2){ if(ad7_pr1>(ad7_pr2+TD)) ii=1; } 
			else{ if(ad7_pr2>(ad7_pr1+TD)) ii=1; } 
			
			if(!ii){
				ad6_sum += ad6_pr2;
				ad7_sum += ad7_pr2;
				if((++adc_cnt)>=4){						//23[ms]
					ad6_dat = ad6_sum;	ad6_sum = 0;
					ad7_dat = ad7_sum;	ad7_sum = 0;
					touch_flag=1;
					adc_cnt=0;
				}
			}
			adc_scan=0;	
			AD0CR = SET_ADC0; 
		    WDFEED = 0xAA;	WDFEED = 0x55;   	// WDT clear
		break;
	}	
}
/*******************************************************************************************/
//20150519 start
void RGB_GRN_ON(void){
	RGB_ALL_OFF(); 			//GPIO_EDIR |=0x0007;  	// portE.0,1,2
	FIO1DIR |= RGB_GRN; 	//
	FIO1CLR = RGB_GRN;		//
}
void RGB_RED_ON(void){
	RGB_ALL_OFF(); 			//GPIO_EDIR |=0x0007;  	// portE.0,1,2
	FIO1DIR |= RGB_RED; 	//
	FIO1CLR = RGB_RED;	//
}
void RGB_BLU_ON(void){
	RGB_ALL_OFF(); 			//GPIO_EDIR |=0x0007;  	// portE.0,1,2
	FIO1DIR |= RGB_BLU; 	//
	FIO1CLR = RGB_BLU;	//
}
/***************************************************************f****************************/
void RGB_ALL_OFF(void){
	if(BALANCE_AUTO){
		FIO1DIR &= ~(RGB_RED|RGB_GRN|RGB_BLU);	 	//
	}else{
		FIO1SET = (RGB_RED|RGB_GRN|RGB_BLU);	 	//
	}
}
//20150519 end
/*******************************************************************/
void enable_watchdog(void){
	WDMOD 	  = 0x03;                   	// Reset Mode
	WDTC  	  = 4000000 * 3;	        	// 3[sec] at 4MHz
    WDFEED 	  = 0xAA;	WDFEED 	  = 0x55;   // WDT clear
}
void disable_watchdog(void){
    WDFEED 	  = 0xAA;	WDFEED 	  = 0x55;   // WDT clear
	WDTC  	  = 4000000 * 300;	        	// 300[sec] at 4MHz
}
/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/
/*************                           ������                                ***********/
/*************                           ������                                ***********/
/*************                           ������                                ***********/
/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/
void  SDRAMDelayNS(u32  idly)
{  
    u32  i;
    for(; idly>0; idly--){
        for(i=0; i<5000; i++);
//        for(i=0; i<20000; i++);
    }
}

void  SDRAMInit( void )
{  
     u32 i, dummy ;

    PINSEL3  = 0xF5555500;		//LCD port
    PINSEL4  = 0x050FFFFF;		//LCD port
    PINSEL5  = 0x05050555;		//SDRAM
    PINSEL6  = 0x55555555;		//d0~d15
    PINSEL7 |= 0x00030000;		//pwm1
    PINSEL8  = 0x55555555;		/* A1~A15  	        */
    PINSEL9  = 0x50050555;      /* A16~A21,P4.22,P4.23,OE,WE,P4.26..29CS0,CS1*/
    PINSEL10 = 0x00000000;		//ETM Off ==> select LCD
    PINSEL11 = (int)(0x00000000)      /*  Reserved,should not write 1 */
               | (5 << 1)             /*  TFT 16-bit (5:6:5 mode)     */ 
               | (1 << 0);            /*  lcd port is enable          */   
//	PINSEL11|= BIN32(00000000,00000000,00000000,00001011);

	EMCControl= 0x00000001;		/*Disable Address mirror*/
    PCONP   |= 0x00000800;    /* Turn on EMC PCLK */

    EMCDynamictRP   = 1;	//2;        
    EMCDynamictRAS  = 3;		
    EMCDynamictSREX = 1;	//7;		
    EMCDynamictAPR  = 1;	//2;		
    EMCDynamictDAL  = 5;		
    EMCDynamictWR   = 2;	//1;		
    EMCDynamictRC   = 3;	//5;		
    EMCDynamictRFC  = 3;	//5;		
    EMCDynamictXSR  = 5;	//7;		
    EMCDynamictRRD  = 1;		
    EMCDynamictMRD  = 2;		

    EMCDynamicReadConfig =1;		/* Command delayed strategy */

  /* Default setting, RAS latency 3 CCLKs, CAS latenty 3 CCLKs. */
    EMCDynamicRASCAS0  = 0x00000303;
//    EMCDynamicRASCAS1  = 0x00000303;

  /* 256MB, 16Mx16, 4 banks, row=13, column=9 */
	EMCDynamicConfig0 = 0x00000680;
  /* 64MB, 4Mx16, 4 banks, row=12, column=8 */
//    EMCDynamicConfig0 = 0x00000280;
   
    SDRAMDelayNS(200);
  /* Mem clock enable, CLKOUT runs, send command: NOP */
    EMCDynamicControl = 0x00000183;
    SDRAMDelayNS(100);
  /* Send command: PRECHARGE-ALL, shortest possible refresh period */
    EMCDynamicControl = 0x00000103;
    SDRAMDelayNS(100);
  /* set 32 CCLKs between SDRAM refresh cycles */
    EMCDynamicRefresh  = 0x00000002;
    for(i = 0; i < 0x80; i++);	/* wait 128 AHB clock cycles */
    
  /* set 28 x 16CCLKs=448CCLK=7us between SDRAM refresh cycles */
    EMCDynamicRefresh  = 28;
    
  /* To set mode register in SDRAM, enter mode by issue
  MODE command, after finishing, bailout and back to NORMAL mode. */    
  /* Mem clock enable, CLKOUT runs, send command: MODE */
    EMCDynamicControl = 0x00000083;
    dummy=*((volatile u32 *)(0xa0033000));
    EMCDynamicControl = 0x00000000;	  /* Send command: NORMAL */
//    EMCDynamicControl = 0x00000003;	  /* Send command: NORMAL */
//    EMCDynamicControl = 0x00000002;	  /* Send command: NORMAL */

	EMCDynamicConfig0 |= 0x00080000;	  /* Enable buffer */
    SDRAMDelayNS(10);
}

/********************************************************************************/
void ConfigurePLL ( void )
{
	unsigned int MValue, NValue;

	if ( PLLSTAT & (1 << 25) )
    {
		PLLCON = 1;			/* Enable PLL, disconnected */
		PLLFEED = 0xaa;
		PLLFEED = 0x55;
    }

    PLLCON = 0;				/* Disable PLL, disconnected */
    PLLFEED = 0xaa;
    PLLFEED = 0x55;
    
	SCS |= 0x20;			/* Enable main OSC */
    while( !(SCS & 0x40) );	/* Wait until main OSC is usable */

    CLKSRCSEL = 0x1;		/* select main OSC, 12MHz, as the PLL clock source */

    PLLCFG = PLL_MValue | (PLL_NValue << 16);
    PLLFEED = 0xaa;
    PLLFEED = 0x55;
      
    PLLCON = 1;				/* Enable PLL, disconnected */
    PLLFEED = 0xaa;
    PLLFEED = 0x55;

    CCLKCFG = CCLKDivValue;	/* Set clock divider */

    while ( ((PLLSTAT & (1 << 26)) == 0) );	/* Check lock bit status */
    
    MValue = PLLSTAT & 0x00007FFF;
    NValue = (PLLSTAT & 0x00FF0000) >> 16;
    while ((MValue != PLL_MValue) && ( NValue != PLL_NValue) );

    PLLCON = 3;				/* enable and connect */
    PLLFEED = 0xaa;
    PLLFEED = 0x55;
	while ( ((PLLSTAT & (1 << 25)) == 0) );	/* Check connect bit status */
}
/********************************************************************************/
void LpcInit(void)
{
    SDRAMInit();                          
    MEMMAP = 0x02; 

	ConfigurePLL();
	
  /* Set system timers for each component */
#if (Fpclk / (Fcclk / 4)) == 1
    PCLKSEL0 = 0x00000000;	/* PCLK is 1/4 CCLK */
    PCLKSEL1 = 0x00000000;
#endif
#if (Fpclk / (Fcclk / 4)) == 2
    PCLKSEL0 = 0xAAAAAAAA;	/* PCLK is 1/2 CCLK */
    PCLKSEL1 = 0xAAAAAAAA;	 
#endif
#if (Fpclk / (Fcclk / 4)) == 4
    PCLKSEL0 = 0x55515555;	/* PCLK is the same as CCLK */
    PCLKSEL1 = 0x55555555;	
#endif

    /* Set memory accelerater module*/
    MAMCR = 0;
#if Fcclk < 20000000
    MAMTIM = 1;
#else
	#if Fcclk < 40000000
    	MAMTIM = 2;
	#else
		#if Fcclk < 60000000
    		MAMTIM = 3;
		#else
    		MAMTIM = 4;
		#endif
	#endif
#endif

	MAMCR  = 2;					//0=disabled, 1=partly enabled (enabled for code prefetch, but not for data), 2=fully enabled

	//initialize VIC
	VICIntEnable   = 0;
	VICIntEnClr    = 0xFFFFFFFF;           /* Disable ALL interrupts                             */
//	PCONP |= 0x00100000;        			/* Power Control for CLCDC */ 

    return;
}

/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/

