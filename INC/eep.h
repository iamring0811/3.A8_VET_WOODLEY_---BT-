#define MAX_RECORD		2000			//128		//256

#define ID55AA	 		0x55A9
#define ID55AA_ADR 		0x00
#define SEQNO_ADR 		0x02		// 2B, 0~299
#define UNIT_ADR 		0x04		// 1B
#define STRIPNO_ADR		0x06		// 1B, 0~99
#define SPEED_ADR 		0x08		// 1bit
#define DOUBLE_ADR 		0x09		// 1bit
#define PRINT_ADR 		0x0A		// 1bit
#define DATA_ADR 		0x0C		// 1bit
#define PLUS_ADR		0x0E		// 1bit
#define MARK_ADR		0x10		// 1bit
#define SOUND_ADR		0x12		// 1bit
#define BAUD_ADR		0x14		// 2bit
#define RESMOD_ADR		0x16		// 1bit
#define COLOR_ADR		0x18		// 1B
#define TURBI_ADR		0x1A		// 1B
#define DAC_ADR			0x1C		// rrrr,gggg,bbbb,sum 	2015-06-09 오전 8:11:03
#define SYSID_ADR 		0x20		//11B
#define ID_CHKSUM 		0x30		// 2B
#define CLEAN_ADR		0x3A		// 1B
#define USERID_ADR 		0x40		//11B

#define SPEED_HI		1
#define SPEED_LO		0
#define PRINT_ENABLE	1
#define PRINT_DISABLE	0
#define DATA_MENUAL		1
#define DATA_AUTO		0

#define UNIT_CONV		0
#define UNIT_ARBI		1
#define UNIT_SI			2

#define MAX_STRIP		100

#ifdef	_EEP_C_
u8  SYS_ID[16];		//
u8  USER_ID[MAX_USERID+1];	//
u8  tmp_userid[MAX_USERID+1];	//
u8  strip_name[16];	//
u16 SEQ_NO;			//0~300					측정할번호
u8  SPEED_FLAG;		//0:60[sec],1:20[sec] 	측정대기시간
u8  DOUBLE_FLAG;	//0:single,1:double
u8  PRINT_FLAG;		//0:On,     1:Off     	측정완료후 프린터 여부 선택
u8  UNIT_FLAG;		//0:Conv.1:SI, 2:Arbitrary
u8  STRIPNO;		//0-99
u8  MAX_STRIPNO;	//0-99
u8  DATA_FLAG;		//0:On(Auto), 1:Off
u8	PLUS_FLAG;		//1:enable
u8	MARK_FLAG;		//1:yes
u8  user_cnt;
u8  cmdbuff[4096];
u8	tmp_strip;
u8  SOUND_FLAG;		//0:off,1:on
u8  BAUD_FLAG;		//0:38400,1:19200,2:9600
u8	LOAD_NO;
u8	ERROR_NO;
u8	COLOR_NO;
u8	TURBI_NO;

void initEEP(void);
//void eep_restore(u16 addr, u16 size);
//char chk_sum_back(u16 addr, u16 size);
void read_eep_all(void);
void default_id(void);
//void system_backup(u16 addr, u16 size);
void write_id_chksum(void);
void clr_user_id(void); 

void chk_ini_curve(void);
void chk_ini_para(void);
void chk_ini_strip(void);
void get_max_stripno(void);
void write_sysid(void);
void write_userid(void);
//void read_para(void);
void chk_ini_pform(void);
void dsp_pform(void);

u16 read_ram4(u16 pnt);
void move_strip_info(void);
void move_sys_curve(void);
void move_parameter(void);
u16 back_parameter(void);
void move_pform(void);
void key_buzzer2(void);
void key_buzzer3(void);

#else
extern u8  SYS_ID[16];		//
extern u8  USER_ID[MAX_USERID+1];	//
extern u8  tmp_userid[MAX_USERID+1];	//
extern u8  strip_name[16];	//
extern u16 SEQ_NO;			//0~300					측정할번호
extern u8  SPEED_FLAG;		//0:60[sec],1:20[sec] 	측정대기시간
extern u8  DOUBLE_FLAG;	//0:single,1:double
extern u8  PRINT_FLAG;		//0:On,     1:Off     	측정완료후 프린터 여부 선택
extern u8  UNIT_FLAG;		//0:Conv.1:SI, 2:Arbitrary
extern u8  STRIPNO;		//0-99
extern u8  MAX_STRIPNO;	//0-99
extern u8  DATA_FLAG;		//0:On(Auto), 1:Off
extern u8	PLUS_FLAG;		//1:enable
extern u8	MARK_FLAG;		//1:yes
extern u8  user_cnt;
extern u8  cmdbuff[4096];
extern u8  tmp_strip;
extern u8  SOUND_FLAG;		//0:off,1:on
extern u8  BAUD_FLAG;		//0:38400,1:19200,2:9600
extern u8	LOAD_NO;
extern u8	ERROR_NO;
extern u8	COLOR_NO;
extern u8	TURBI_NO;

extern void initEEP(void);
//extern void eep_restore(u16 addr, u16 size);
//extern char chk_sum_back(u16 addr, u16 size);
extern void read_eep_all(void);
extern void default_id(void);
//extern void system_backup(u16 addr, u16 size);
extern void write_id_chksum(void);
extern void clr_user_id(void); 

extern void chk_ini_curve(void);
extern void chk_ini_para(void);
extern void chk_ini_strip(void);
extern void get_max_stripno(void);
extern void write_sysid(void);
extern void write_userid(void);
//extern void read_para(void);
extern void chk_ini_pform(void);
extern void dsp_pform(void);

extern u16 read_ram4(u16 pnt);
extern void move_strip_info(void);
extern void move_sys_curve(void);
extern void move_parameter(void);
extern u16 back_parameter(void);
extern void move_pform(void);
extern void key_buzzer2(void);
extern void key_buzzer3(void);

#endif