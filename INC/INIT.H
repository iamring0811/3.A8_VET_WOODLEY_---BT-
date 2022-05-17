//#define	DEBUG
#define	ERR_MSG

/******	 Measure Length  ************/
#define LPC40M
#ifdef	LPC40M
	#define MAX_TICK 	1150
#else
	#define MAX_TICK	1900
#endif
#define MAX_TICK_1		1900

#define MAX_USERID		16

//#define TEST_PRINT
#ifdef TEST_PRINT
	#define print_thermal 	printf0
	#define wait_printing()	while(txr0!=txw0)
#else	
	#define print_thermal 	printf2
	#define wait_printing()	while(txr2!=txw2)
#endif
//#define TEST_RESULT

/*************************************************************************/

#define LongToBin(n) (((n >> 21) & 0x80) | \
                      ((n >> 18) & 0x40) | \
                      ((n >> 15) & 0x20) | \
                      ((n >> 12) & 0x10) | \
                      ((n >>  9) & 0x08) | \
                      ((n >>  6) & 0x04) | \
                      ((n >>  3) & 0x02) | \
                      ((n      ) & 0x01))

#define __BIN(n) LongToBin(0x##n##l)

#define BIN8(n)                       __BIN(n)
#define BIN(n)                        __BIN(n)
#define BIN16(b1,b2)        ((        __BIN(b1)  <<  8UL) + \
                                      __BIN(b2))
#define BIN32(b1,b2,b3,b4) ((((U32)__BIN(b1)) << 24UL) + \
                            (((U32)__BIN(b2)) << 16UL) + \
                            (((U32)__BIN(b3)) <<  8UL) + \
                              (U32)__BIN(b4))
//  PINSEL5 &= BIN32(11110000,11111100,11111100,11000000);

#define Fosc	12000000
/* Fcck = 72Mhz, Fosc = 288Mhz, and USB 48Mhz */
#define PLL_MValue			48
#define PLL_NValue			3
#define CCLKDivValue		3
/* System configuration: Fosc, Fcclk, Fcco, Fpclk must be defined */
/* PLL input Crystal frequence range 4KHz~20MHz. */
/* System frequence,should be less than 80MHz. */
#define Fcclk	 72000000
#define Fcco	288000000

#define Fpclk	(Fcclk / 2)

// Add IntDisable() and IntEnable() Functions
// changed by Chenxibing
__swi(0x00) void SwiHandle1(int Handle);

#define IRQDisable() SwiHandle1(0)
#define IRQEnable() SwiHandle1(1)

/*************************************************************************/

#define XS_OFFSET	12
#define ADD10B		10

#define	PORTE_DARD	0x0FEA0F		//b0~b2:RGB, b9:ADCin READ,b11:sens,b13:wp24B,b16:SW START,b17:SW TEST

#define _ISR_STARTADDRESS		0x407fbf00
#define pISR_IRQ (*(unsigned *)(_ISR_STARTADDRESS+0))

#define PWM_COUNTER_ENABLE     0x01
#define PWM_COUNTER_RESET      0x02
#define PWM_OUTPUT_ENABLE      0x04
#define PWM_OUTPUT_WAVE_INVERT 0x08
#define PWM_SOURCE_CLOCK       0x10

#define MSEC500		2242		//500000[us]/222.98[us]
#define size_RGB 	2048
#define data_RGB 	16

#define SET_ADC0		0x00212001		//500[KHz]
#define SET_ADC6		0x00212040		//500[KHz]
#define SET_ADC7		0x00212080		//500[KHz]

#ifdef	_INIT_C_
int  t0cnt;
u16  dur_PWM;						//부저 켜져있는 시간
u32  ProgVer;
u8   test_no;
u16  evenodd, smscan;
u16  time_ref, tm_10ms, tm_500ms;
u16  buffRED1[size_RGB], buffGRN1[size_RGB], buffBLU1[size_RGB];
u16  buffRED2[data_RGB], buffGRN2[data_RGB], buffBLU2[data_RGB];
u16  spanRED[size_RGB], spanGRN[size_RGB], spanBLU[size_RGB];
u16  pnt_adc, flag_adc, rgb_adc, tst_cnt, flag_dac; //20150519
u8   div_adc;
char char_buff[256];
u16	adc_cnt, adc_scan, touch_flag;
u16	ad6_pr1, ad7_pr1, ad6_pr2, ad7_pr2; 
u16 ad6_dat, ad6_sum, ad7_dat, ad7_sum;
u32 t0dly;
char ver_str[3];//20150519
void GpioInit(void);
void MemInit(void);
void LED_GRN_BAR(void);
void LED_RED_BAR(void);
void Timer0Init(void);
void On_Interrupt(void);
void delay_ms(int dly);
void delay_set(int dly);
void delay_us(int dly);
void key_buzzer(void);
void move_font(void);
void move_picture(void);
U32 get_ProgVer(void);
void RGB_GRN_ON(void);
void RGB_RED_ON(void);
void RGB_BLU_ON(void);
void RGB_ALL_OFF(void);//20150519
void enable_watchdog(void);
void disable_watchdog(void);
void init_adc(void);				/* ADC */
void adc_read(void);

#else
//extern void RGB_ALL_OFF(void);

extern int  t0cnt;
extern u16  dur_PWM;						//부저 켜져있는 시간
extern u32 	ProgVer;
extern u8  	test_no;
extern u16  evenodd, smscan;
extern u16  time_ref, tm_10ms, tm_500ms;
extern u16  buffRED1[size_RGB], buffGRN1[size_RGB], buffBLU1[size_RGB];
extern u16  buffRED2[data_RGB], buffGRN2[data_RGB], buffBLU2[data_RGB];
extern u16  spanRED[size_RGB], spanGRN[size_RGB], spanBLU[size_RGB];
extern u16  pnt_adc, flag_adc, rgb_adc, tst_cnt, flag_dac; //20150519
extern u8   div_adc;
extern char char_buff[256];
extern u16	adc_cnt, adc_scan, touch_flag;
extern u16	ad6_pr1, ad7_pr1, ad6_pr2, ad7_pr2; 
extern u16 ad6_dat, ad6_sum, ad7_dat, ad7_sum;
extern u32 t0dly;
extern char ver_str[3];//2015-06-09 오전 8:23:02
extern void GpioInit(void);
extern void MemInit(void);
extern void LED_GRN_BAR(void);
extern void LED_RED_BAR(void);
extern void Timer0Init(void);
extern void On_Interrupt(void);
extern void delay_ms(int dly);
extern void delay_set(int dly);
extern void delay_us(int dly);
extern void key_buzzer(void);
extern void move_font(void);
extern void move_picture(void);
extern U32 get_ProgVer(void);
extern void RGB_GRN_ON(void);
extern void RGB_RED_ON(void);
extern void RGB_BLU_ON(void);
extern void RGB_ALL_OFF(void);//20150519
extern void enable_watchdog(void);
extern void disable_watchdog(void);
extern void init_adc(void);				/* ADC */
extern void adc_read(void);

#endif

