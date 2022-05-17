//20150519 Start
#define INI_RGB		0x0777		//R-G-B
//20150519 end
#ifdef	_SYSADJ_C_
u16 blkRED, blkGRN, blkBLU;
u16 whtRED, whtGRN, whtBLU;

//20150519 Start
u16 dac_r, dac_g, dac_b;
u16	BALANCE_AUTO;
short flag_balance;

void out_dac(u8 dat);
char set_balance(u16 yoff);
void set_default_RGB(void);
void read_balance(void);
void chk_auto_balance(void);
//20150519 end

void system_check(void);
void run_meas_adj(u16 YS, u16 YE);
void run_adjust(void);
void out_check(void);

#else
extern u16 blkRED, blkGRN, blkBLU;
extern u16 whtRED, whtGRN, whtBLU;
//20150519 start
extern u16 dac_r, dac_g, dac_b;
extern u16	BALANCE_AUTO;
extern short flag_balance;

extern void out_dac(u8 dat);
extern void set_balance(u16 yoff);
extern void set_default_RGB(void);
extern void read_balance(void);
extern void chk_auto_balance(void);
//20150519 end
extern void system_check(void);
extern void run_adjust(void);

#endif