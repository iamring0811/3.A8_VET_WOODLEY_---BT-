
#define UART0 0   // arm uart0
#define UART1 1   // arm uart1
#define UART2 2   // external uart0 A
#define UART3 3   // external uart1 B

#define SCRX0_BUF_SIZE  0x10000 /* Serial0 Receive Buffer Size */
#define	SCRX1_BUF_SIZE	0x10000	/* Serial1 Receive Buffer Size */
#define	SCRX2_BUF_SIZE	0x10000	/* Serial2 Receive Buffer Size */
#define	SCRX3_BUF_SIZE	0x10000	/* Serial3 Receive Buffer Size */

#define SCTX0_BUF_SIZE  0x10000 /* Serial0 Send Buffer Size */
#define	SCTX1_BUF_SIZE	0x10000	/* Serial1 Send Buffer Size */
#define	SCTX2_BUF_SIZE	0x10000	/* Serial2 Send Buffer Size */
#define	SCTX3_BUF_SIZE	0x10000	/* Serial3 Send Buffer Size */

#define PR0(a) Uart_Printf(0,a)
#define PR1(a) Uart_Printf(1,a)
#define PR2(a) Uart_Printf(2,a)
#define PR3(a) Uart_Printf(3,a)

#ifdef	_UART_C_

u8	SCRX0_Buf[SCRX0_BUF_SIZE];	/* Serial0���� ���� ����Ÿ�� �����ϴ� ���� */
u8	SCTX0_Buf[SCTX0_BUF_SIZE];	/* Seiral0�� ���� ����Ÿ�� �����ϴ� ���� */
u16	rxw0, rxr0,  txw0, txr0;

u8	SCRX1_Buf[SCRX1_BUF_SIZE];	/* Serial1���� ���� ����Ÿ�� �����ϴ� ���� */
u8	SCTX1_Buf[SCTX1_BUF_SIZE];	/* Seiral1�� ���� ����Ÿ�� �����ϴ� ���� */
u16   rxw1, rxr1,  txw1, txr1;

u8	SCRX2_Buf[SCRX2_BUF_SIZE];	/* Serial2���� ���� ����Ÿ�� �����ϴ� ���� */
u8	SCTX2_Buf[SCTX2_BUF_SIZE];	/* Seiral2�� ���� ����Ÿ�� �����ϴ� ���� */
u16   rxw2, rxr2,  txw2, txr2;

u8	SCRX3_Buf[SCRX3_BUF_SIZE];	/* Serial3���� ���� ����Ÿ�� �����ϴ� ���� */
u8	SCTX3_Buf[SCTX3_BUF_SIZE];	/* Seiral3�� ���� ����Ÿ�� �����ϴ� ���� */
u16   rxw3, rxr3,  txw3, txr3;

unsigned char testChar;
unsigned uart3_dly;

void Uart_Init_All(void);
void Uart_Init(int baud,int whichUart);
void wait_tx0(void);
void wait_tx1(void);
void wait_tx2(void);
void wait_tx3(void);
void com_clear(void);
void Uart_SendChar(int data,int whichUart);
void Uart_Sendu8(int data,int whichUart);
void Uart_SendString(char *pt,int whichUart);
int  Uart_getch(int whichUart);
void Uart_Printf(int whichUart,char *fmt,...);
void printf0(char *fmt,...);
void printf1(char *fmt,...);
void printf2(char *fmt,...);
void printf03(char *fmt,...);
void printfx(char *fmt,...);
void uartTest(void);

void savcmd0(u8 cmd);
void savcmd1(u8 cmd);
void savcmd2(u8 cmd);
void savcmd3(u8 cmd);
u8 getcmd0(void);
u8 getcmd1(void);
u8 getcmd2(void);
u8 getcmd3(void);

u8 bin2hex2(u8 b);
u8 asc2hex(u8 asc);
void set_baud3(void);
short blu_dual(void);
void init_blu(void);
void send_blu(char *str);
short ans_blu(u8 dly);
short blu_baud(void);
short set_blu(void);
short flag_blu;
short set_blu_ble(void);

#else

extern u8	SCRX0_Buf[SCRX0_BUF_SIZE];	/* Serial���� ���� ����Ÿ�� �����ϴ� ���� */
extern u8	SCTX0_Buf[SCTX0_BUF_SIZE];	/* Seiral�� ���� ����Ÿ�� �����ϴ� ���� */
extern u16   rxw0, rxr0,  txw0, txr0;

extern u8	SCRX1_Buf[SCRX1_BUF_SIZE];	/* Serial���� ���� ����Ÿ�� �����ϴ� ���� */
extern u8	SCTX1_Buf[SCTX1_BUF_SIZE];	/* Seiral�� ���� ����Ÿ�� �����ϴ� ���� */
extern u16   rxw1, rxr1,  txw1, txr1;

extern u8	SCRX2_Buf[SCRX2_BUF_SIZE];	/* Serial���� ���� ����Ÿ�� �����ϴ� ���� */
extern u8	SCTX2_Buf[SCTX2_BUF_SIZE];	/* Seiral�� ���� ����Ÿ�� �����ϴ� ���� */
extern u16  rxw2, rxr2,  txw2, txr2;

extern u8	SCRX3_Buf[SCRX3_BUF_SIZE];	/* Serial���� ���� ����Ÿ�� �����ϴ� ���� */
extern u8	SCTX3_Buf[SCTX3_BUF_SIZE];	/* Seiral�� ���� ����Ÿ�� �����ϴ� ���� */
extern u16   rxw3, rxr3,  txw3, txr3;

extern unsigned uart3_dly;

extern void Uart_Init_All(void);
extern void Uart_Init(int baud,int whichUart);
extern void wait_tx0(void);
extern void wait_tx1(void);
extern void wait_tx2(void);
extern void wait_tx3(void);
extern void com_clear(void);
extern void Uart_SendChar(int data,int whichUart);
extern void Uart_Sendu8(int data,int whichUart);
extern void Uart_SendString(char *pt,int whichUart);
extern  int Uart_getch(int whichUart);
extern void Uart_Printf(int whichUart,char *fmt,...);
extern void printf0(char *fmt,...);
extern void printf1(char *fmt,...);
extern void printf2(char *fmt,...);
extern void printf03(char *fmt,...);
extern void printfx(char *fmt,...);
extern void uartTest(void);

extern void savcmd0(u8 cmd);
extern void savcmd1(u8 cmd);
extern void savcmd2(u8 cmd);
extern void savcmd3(u8 cmd);
extern u8 getcmd0(void);
extern u8 getcmd1(void);
extern u8 getcmd2(void);
extern u8 getcmd3(void);

extern u8 bin2hex(u8 b);
extern u8 asc2hex(u8 asc);
extern void set_baud3(void);
                 
extern void set_baud3(void);
extern void init_blu(void);
extern short flag_blu;
#endif
