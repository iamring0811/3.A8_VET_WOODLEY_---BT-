
#define COM_SYS_INFO		0x51		//Q
#define COM_ID_SET			0x52		//R
#define COM_MEAS_START		0x53		//S
#define COM_MEAS_RESULT		0x54		//T
#define COM_UID_SET			0x55		//U
#define COM_SPD_READ		0x56		//V		//0:표준, 1:고속
#define COM_SPD_WRITE		0x57		//W
#define COM_TIME_WRITE		0x58		//X
#define COM_VERSION_READ	0x59		//Y
#define COM_TIME_READ		0x5A		//Z
#define COM_CURVE_READ		0x61		//a
#define COM_CURVE_WRITE		0x62		//b
#define COM_PARA_READ		0x63		//c
#define COM_PARA_WRITE		0x64		//d
#define COM_STRIP_READ		0x65		//e
#define COM_STRIP_WRITE		0x66		//f
#define COM_STRIP_NO_RD		0x67		//g
#define COM_STRIP_NO_WR		0x68		//h
#define COM_FULL_RAW_DATA	0x69		//i
#define COM_DOUBLE_READ		0x44		//D
#define COM_DOUBLE_WRITE	0x45		//E
#define COM_PFORM_READ		0x71		//
#define COM_PFORM_WRITE		0x72		//
#define URLF 				0x0A
#define COM_CLEAN_LOAD		0x5B		//[
#define COM_DATA_COUNT		0x6A		//j
//#define COM_MEAS_RESULT		0x6B		//k
#define COM_CALIBRATION		0x6C		//l
#define COM_SYS_CHECK		0x6D		//m
#define COM_DATA_NOSTORE	0x6E		//n		현재측정데이타는 저장안함.
#define COM_DATA_ERASE		0x6F		//o		데이타를 지운다.
#define COM_ALL_RESULT		0x70		//p		저장된 데이타전체를 보낸다.

#define RX0WAIT 		10
#define STORE_CURVE		0xc001
#define STORE_PARA		0xc002
#define STORE_STRIP		0xc003
#define STORE_WEIGHT	0xc004

#define MAX_STRIP		100

#ifdef _COM_C_

u8  COMCMD;
u8  com3dat;
u16 rx3_wait;		//통신에러 감시용.
u8  write_step;
u16 rx3_cnt;

void store_uid(void);
void store_id(void);
void store_strip_no(void);
void store_curve(void);
void store_para(void);
void store_strip(void);

void system_decode(u8 dat);

void com_spd_write(void);
void com_spd_read(void);
void com_time_write(void);
void com_version_read(void);
void com_time_read(void);
void com_double_read(void);
void com_double_write(void);
void com_sys_info(void);
void com_id_set(void);
void com_meas_start(void);	
void send_meas_step(u8 step);
void com_meas_result(void);	
void com_uid_set(void);		
void com_curve_read(void);	
void com_curve_write(void);	
void com_para_read(void);	
void com_para_write(void);	
void com_strip_read(void);	
void com_strip_write(void);	
void com_strip_no_rd(void);	
void com_strip_no_wr(void);	
void com_full_raw_data(void);
void com_pform_read(void);
void com_pform_write(void);

void send_curve(void);
void send_para(void);
void send_strip(void);


#else
extern u8  COMCMD;
extern u8  com3dat;
extern u16 rx3_wait;		//통신에러 감시용.
extern u8  write_step;
extern u16 rx3_cnt;

extern void store_curve(void);
extern void store_para(void);

extern void system_decode(u8 dat);
extern void send_meas_step(u8 step);

extern void send_curve(void);
extern void send_para(void);
extern void send_strip(void);

#endif

