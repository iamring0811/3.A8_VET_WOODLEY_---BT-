
//#define USE_EEP

#define RX0_WAIT  250
#define RX0_1st		0
#define RX0_2nd		1
#define RX0_RCV		4
#define RX3_WAIT  RX0_WAIT

// Because S3C44B0X is connected to AM29LV1600B,
// the addr parameter has to be a WORD address, so called in AMD specification.

#define BASE2470	0x81000000

#define _WR(addr,data)	( *((U16 *)(BASE2470+(addr<<1)))=(U16)data )	//the addr should be shifted
#define _RD(addr)	( *((U16 *)(BASE2470+(addr<<1))) )	
#define _RESET()	_WR(0x00,0xf0f0)		
#define BADDR2WADDR(addr)   (addr>>1)			

#define FNT_ROM			0x81050000
#define FNT_BASE		0xA0050000
#define BMP_ROM			0x81080000
#define BMP_RAM			0xA0080000
#define BMP_BASE		BMP_RAM

#define FlashBaseAdr 	0x00010000
#define FontBaseAdr		0x00050000
#define BmpBaseAdr		0x00080000
#define VersionAdr		0x81010000

/***********************************************************/

#define STORE_ROM		0x70000
#define STORE_SIZE		0x05000
#define SYSTEM_RAM		0x0000		//
#define SYSTEM_SIZE		0x0200		//
#define CURVE_RAM		0x1000		//216	0x0D8
#define CURVE_SIZE		216			//108
#define STRIP_RAM		0x2000		//2700, 0xA8C
#define STRIP_SIZE		27*100							//2700, 0xA8C
#define PARA_RAM		0x3000		//20+(17*7*3*4)=20+1428, 앞의 20Bytes는 Weight Data이다.
#define PARA_SIZE		1496		//1496	0x5C4, including weight data
#define WEIGHT_SIZE		20
#define PFORM_RAM		0x4000		//21*8*8*3
#define PFORM_SIZE		4032		//4032	0XFC0

//#define RESULT_ROM		0x070000
//#define RESULT_ROM		0x200000
#define RESULT_ROM		0x040000
#define RESULT_ROM_SIZE	0x010000
//#define RESULT_ROM_SIZE	0x07D000
#define RESULT_PAGE		0x000100
#define RESULT_BLOCK	0x010000

#ifdef	_FLASH_C_
u8  rx0_step, rx3_step, rx0_wait, flag_flash;
u16 rx0_sum, rx3_sum, get_sum;
int	rx0_size, rx0_cnt;
u8  savebuff[0x10000];
volatile U8	downdata[0x40000];	//download data buffer: 1024KB
u8  STORE_RAM[STORE_SIZE];
u8  FLASH_RAM[STORE_SIZE];

int  AM29F1600_ProgFlash(U32 realAddr,U16 data);
void AM29F1600_EraseSector(int targetAddr);
int  AM29F1600_CheckId(void);
int  BlankCheck(int targetAddr,int targetSize);
int  _WAIT(void);
void backup_write(void);
void store_result(u16 tmp_seqno, u8 dno);
int system_store(u8 cmd);
void test_write(void);
void test_read(void);
short flash_write(void);
void prog_decode(void);
void set_sys_curve(void);
void set_sys_para(void);
void set_sys_strip(void);
void move_result(u16 sno);
	
#else
extern u8  rx0_step, rx3_step, rx0_wait, flag_flash;
extern u16 rx0_sum, rx3_sum;
extern int rx0_size, rx0_cnt;
extern u8  savebuff[0x10000];
extern volatile U8	downdata[0x40000];	//download data buffer: 1024KB
extern u8  STORE_RAM[STORE_SIZE];
extern u8  FLASH_RAM[STORE_SIZE];

extern void backup_write(void);
extern void store_result(u16 tmp_seqno, u8 dno);
extern int system_store(u8 cmd);
extern void	test_write(void);
extern void	test_read(void);
extern short flash_write(void);
extern void prog_decode(void);
extern void set_sys_curve(void);
extern void set_sys_para(void);
extern void set_sys_strip(void);
extern void move_result(u16 sno);

#endif

