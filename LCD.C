#define  _LCD_C_
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include ".\inc\LPC24xx.h"
#include ".\inc\pin.h"
#include ".\inc\Init.h"
#include ".\inc\UART.h"
#include ".\inc\lcd.h"
#include ".\inc\flash.h"
#include ".\inc\RESM.h"
#include ".\inc\touch.h"
#include ".\inc\EEP.h"
#include ".\inc\Meas.h"
#include ".\inc\para.h"
#include ".\inc\dsp.h"

const u8 jaum_tb[21];
const u8 moum_tb[30];
const u8 bach_tb[30];
const u8 jm_tb[33];
const u8 jmb_tb[33];
const u8 bb_tb[33];

void delay_Ns (int idly){
    int i;
    for(; idly > 0; idly--) {
        for (i = 0; i < 5000; i++);
    }
}           

/********************************************************************/
void init_lcd(void){															//사용안함
    PCONP     |= (1 << 20);     /*  enable LCD controller power control bit                 */    
//	PCONP |= 0x00100000;        /* Power Control for CLCDC */ 
    delay_Ns(20);

    PINSEL10 = 0x00000000;		//ETM Off ==> select LCD
    
    PINSEL3  = 0xF5555500;		//LCD port
    PINSEL4  = 0x050FFFFF;		//LCD port
    PINSEL5  = 0x05050555;		//SDRAM
    PINSEL6  = 0x55555555;		//d0~d15
    PINSEL7 |= 0x00030000;		//pwm1
    PINSEL8  = 0x55555555;		/* A1~A15  	        */
    PINSEL9  = 0x50050555;      /* A16~A21,P4.22,P4.23,OE,WE,P4.26..29CS0,CS1*/
    PINSEL11 = (int)(0x00000000)      /*  Reserved,should not write 1 */
//  PINSEL11 = (int)(0xfffffff0)      /*  Reserved,should write 1 */
               | (5 << 1)             /*  TFT 16-bit (5:6:5 mode)     */ 
               | (1 << 0);            /*  lcd port is enable          */   
//	PINSEL11|= BIN32(00000000,00000000,00000000,00001011);

    delay_Ns(15);

    LCD_UPBASE = (unsigned int)LcdBaseAdr1;                             /*  set buffer's base address   */
    
    LCD_CTRL   = 0;                                                     /*  power disable               */
    delay_Ns(20);
               
//  LCD_CFG    = 0x02;                                                  /*  pixel clock 9MHZ            */ 
//  LCD_CFG    = 0x05;   //8.3MHz                                       /*  pixel clock 9MHZ            */ 
    LCD_CFG    = 0x08;   //5.3MHz                                       /*  pixel clock 9MHZ            */ 
//  LCD_CFG    = 0;                                                     /*  pixel clock 9MHZ            */ 

    LCD_TIMH   = (45 << 24) |                           //45            /*  set horizontal timing       */
                 (8 << 16) |                            //45              
                 (41 << 8)  |							//45
                 (PPL << 2);							//30-1  (480/16)-1
    LCD_TIMV   = (12 << 24) |                           //16            /*  set vertical timing         */
                 (4 << 16) |							//16
                 (10 << 10) |							//16
                 (LPP << 0);							//272-1
    LCD_POL    = (1 << 26)   |                          /*  bypass pixel color driver   */
                 (CPL << 16) |         //479            /*  240 clock per line          */
                 (0 << 14)   |                          /*  LCDENAB output pin is active HIGH in TFT mode  */
//               (1 << 13)   |                          /*  Data is driven on the LCD on the rising edge of LCDDCLK  */
                 (0 << 13)   |                          /*  Data is driven on the LCD on the falling edge of LCDDCLK  */
                 (1 << 12)   |                          /*  HSYNC is active low         */                                           
//               (1 << 11)   |                          /*  VSYNC is active low         */
                 (0 << 5)	|
                 5;                                     /*  select HCLK                 */
    LCD_CTRL   = (LcdTFT << 5) |                        /*  select TFT LCD type         */
//               (LcdBpp << 1) |      //5:6:5           /*  select 16bpp                */
                 (3 << 1) |           //5:6:5           /*  select 8bpp                */
                 (0 << 8);			  //RGB

    LCD_INTMSK = 0;                                                     /*  disable LCD interrupt       */    
    delay_Ns(20);
    LCD_CTRL  |= 1;                                        				/*  LCD enable         */
    delay_Ns(20);
    LCD_CTRL  |= (1 << 11);                                             /*  Power enable                */
    delay_Ns(20);
	LBLEN_ENABLE();
	LCD_BK_ON();
}

//---------------------------------------------------------------------------------------------
void copy_lcdbase(void){
register u16 *pos, *dst;
register u32 i;
    pos = (u16*)(LcdBaseAdr1);
    dst = (u16*)(LcdBaseAdr0);
    for(i=0;i<(BMP_BLOCK_SIZE/2);i++) *(dst++) = *(pos++);
}
//---------------------------------------------------------------------------------------------
/********************************************************************/
void test_lcd(void){
u8 color=0xff;
	while(1)
	{
//  printf0("**%2x\n",color);
  color=0xff;
//		LCDbmp01();	
		out_line_32(0, 50,"한글시험중-!@#$%^&*", color, 0);
		out_line_32(0, 100,"ABCDEFGHIJKLMNOPQRSTUVW", 0xee, 0);
		out_line_32(0, 150,"큰글자:32X32 1234567890", color, 0);
		out_line_32(0, 200,"abcdefghijklmnopqrstuvwxyz", 0xcc, 0);
  color=0x04;
//		LCDbmp02();	
		out_line_24(0, 50,"[INGBIO] 우린 가고구그 1234567890", color, 1);
		out_line_24(0, 100,"작은글자 : 24X24 이어도", 0x80, 1);
		out_line_24(0, 150,"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0xd0, 1);
		out_line_24(0, 200,"abcdefghijklmnopqrstuvwxyz", 0x80, 1);
	}
}

/********************************************************************/
void msg24(u16 x, u16 y, u8 color, char *fmt,...)
{
va_list ap;
char string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	out_line_24(x, y, string, color, RASTNO);
	va_end(ap);
}
/********************************************************************/
void msg20(u16 x, u16 y, u8 color, char *fmt,...)
{
va_list ap;
char string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	out_line_20(x, y, string, color, RASTNO);
	va_end(ap);
}
/********************************************************************/
void cursor_24(u16 xs, u16 ys, u16 cr, char *str){
u16 x, y;
u8 *pos;
	y=0;
	xs -= XS_OFFSET;
	for(x=0;x<cr;x++) y += size24[str[x]];
	xs=271-(xs+12+y);
	ys += 22;
	for(x=xs;x<(xs+12);x++){
	    pos = (u8*)LcdBaseAdr1 + (x*480) + ys;
		for(y=ys;y<(ys+2);y++)	*(pos++) = BLK;
	}	
}

/********************************************************************/
void cursor_20(u16 xs, u16 ys, u16 cr, char *str){
u16 x, y;
u8 *pos;
	y=0;
	xs -= XS_OFFSET;
	for(x=0;x<cr;x++) y += 11;
	xs=271-(xs+8+y);
	ys += 22;
	for(x=xs;x<(xs+9);x++){
	    pos = (u8*)LcdBaseAdr1 + (x*480) + ys;
		for(y=ys;y<(ys+2);y++)	*(pos++) = BLK;
	}	
}

/********************************************************************/
void cursor_16(u16 xs, u16 ys, u16 cr, char *str){
u16 x, y;
u8 *pos;
	y=0;
	xs -= XS_OFFSET;
	for(x=0;x<cr;x++) y += 8;
	xs=271-(xs+8+y);
	ys += 22;
	for(x=xs;x<(xs+7);x++){
	    pos = (u8*)LcdBaseAdr1 + (x*480) + ys;
		for(y=ys;y<(ys+2);y++)	*(pos++) = BLK;
	}	
}

/********************************************************************/
void msg32(u16 x, u16 y, u8 color, char *fmt,...)
{
va_list ap;
char string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	out_line_32(x, y, string, color, RASTNO);
	va_end(ap);
}

/********************************************************************/
void msg16(u16 x, u16 y, u8 color, char *fmt,...)
{
va_list ap;
char string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	out_line_16(x, y, string, color, RASTNO);
	va_end(ap);
}

/********************************************************************/
//	LCDbmpxx(2       ,    400,    200);					
//	LCDbmpxx(그림번호,Xoffset,Yoffset);					
void LCDbmpxx(u32 bmpnt, u16 xp, u16 yp){
int x, y;
u32 *pos_s, *pos_d, *pos_b;
u32 bmpadr, bd;
int xsize, ysize, bmpsize;

	if(xp<XS_OFFSET) xp=0;
	else xp -= XS_OFFSET;
	x = xp;	y = yp;
	xp = y;	yp = x;
	bmpnt = (bmpnt*8)+1;
	if(bmp_head[bmpnt-1]!=0X4B) return;
	bmpadr=bmp_head[bmpnt++];
	bmpadr=(bmpadr<<8)+bmp_head[bmpnt++];
	bmpadr=(bmpadr<<8)+bmp_head[bmpnt++];
	xsize=bmp_head[bmpnt++];
	xsize=(xsize<<8)+bmp_head[bmpnt++];
	ysize=bmp_head[bmpnt++];
	ysize=(ysize<<8)+bmp_head[bmpnt++];
	yp += ysize;
	if(yp<272) yp = 271-yp;
	else yp=0;
	bmpsize = xsize * ysize;

    pos_s = (u32*)(BMP_BASE+bmpadr);	

	for(y=0; y<ysize; y++){ 
	    bd = ((y+yp)*480)+xp;
    	pos_d = (u32*)(LcdBaseAdr1+bd);
	    pos_b = (u32*)(LcdBaseAdr0+bd);
		for(x=0; x<(xsize/4); x++){ 
			bd = *(pos_s++);
			*(pos_d++) = bd;
			*(pos_b++) = bd;
		}
	}
}
//=====================================================================
void LCDbmpxb(u32 bmpnt, u16 xp, u16 yp){
int x, y;
u32 *pos_s, *pos_d, *pos_b, *pos_e;
u32 bmpadr, bd;
int xsize, ysize, bmpsize;

//bmpnt=4;xp=20;yp=10;
//	if(bmpnt) bmpnt-=1;
	x = xp;	y = yp;
	xp = y;	yp = x;
	bmpnt = (bmpnt*8)+1;
	if(bmp_head[bmpnt-1]!=0X4B) return;
	bmpadr=bmp_head[bmpnt++];
	bmpadr=(bmpadr<<8)+bmp_head[bmpnt++];
	bmpadr=(bmpadr<<8)+bmp_head[bmpnt++];
	xsize=bmp_head[bmpnt++];
	xsize=(xsize<<8)+bmp_head[bmpnt++];
	ysize=bmp_head[bmpnt++];
	ysize=(ysize<<8)+bmp_head[bmpnt++];
	yp += ysize;
	if(yp<272) yp = 271-yp;
	else yp=0;
	bmpsize = xsize * ysize;

    pos_s = (u32*)(BMP_BASE+bmpadr);	

	for(y=0; y<ysize; y++){ 
	    bd = ((y+yp)*480)+xp;
    	pos_b = (u32*)(LcdBaseAdr0+bd);
    	pos_d = (u32*)(LcdBaseAdr1+bd);
	    pos_e = (u32*)(LcdBaseAdr2+bd);		//for erease
		for(x=0; x<(xsize/4); x++){ 
			bd = *(pos_s++);
			*(pos_d++) = bd;
			*(pos_b++) = bd;
			*(pos_e++) = bd;
		}
	}
}
//=====================================================================


/********************************************************************/
#define FSIZE 32
void out_figure(int x_pos, int y_pos, int cod, u8 color, u8 lno){
unsigned short i, a;
unsigned int k;
unsigned char *pos_s, *pos_d, *pos_b;
	pos_s = (unsigned char*) FIG32ADR+(cod*32*4);
    if(lno==RAST1) pos_d = (unsigned char*)(LcdBaseAdr1) + x_pos + (y_pos*480);
    else pos_d = (unsigned char*)(LcdBaseAdr2) + x_pos + (y_pos*480);
    pos_b = (unsigned char*)(LcdBaseAdr0) + x_pos + (y_pos*480);
//  printf0("**%2x\n",color);
	for(i=0;i<32;i++){
		k = *(pos_s++);
		k = (k<<8) + *(pos_s++);
		k = (k<<8) + *(pos_s++);
		k = (k<<8) + *(pos_s++);
		for(a=0;a<FSIZE;a++){
			if(k&0x40000000) *(pos_d) = color;
			else *(pos_d) = *pos_b;
			k = k<<1;
			pos_d++;
			pos_b++;
		}
		pos_d += 480 - FSIZE;
		pos_b += 480 - FSIZE;
	}
}

/******************************************************************************/

const u8 jaum_tb[21]={
    /*0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20*/
     32,32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18
};
const u8 moum_tb[30]={
    /*0  1  2  3  4  5  6  7       10 11 12 13 14 15       18 19 20*/
     32,32,32, 0, 1, 2, 3, 4,32,32, 5, 6, 7, 8, 9,10,32,32,11,12,13,
    /*  21 22 23       26 27 28 29*/
        14,15,16,32,32,17,18,19,20
};
const u8 bach_tb[30]={
    /*0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17    19 20*/
     32,32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,32,16,17,
    /*  21 22 23 24 25 26 27 28 29*/
        18,19,20,21,22,23,24,25,26
};
const u8 jm_tb[33]={
/*  0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2 */
//  0,0,0,0,0,0,0,0,1,3,3,3,1,2,4,4,4,2,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0
    0,0,0,0,0,0,0,0,1,3,3,3,1,2,3,3,3,2,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0
};
const u8 jmb_tb[33]={
/*  0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2 */
//  5,5,5,5,5,5,5,5,6,7,7,7,6,6,7,7,7,6,6,7,5,5,5,5,5,5,5,5,5,5,5,5,5
    4,4,4,4,4,4,4,4,5,6,6,6,5,5,6,6,6,5,5,6,4,4,4,4,4,4,4,4,4,4,4,4,4
};
const u8 bb_tb[33]={
/*  0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2 */
//  0,2,0,2,1,2,1,2,3,0,2,1,3,3,1,2,1,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0
    0,1,0,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/********************************************************************/
void init_fonts(void){
u8 *fp;
short i;
	fp = (u8*) (FNT_BASE+0x20);
	ENG16ADR = 0;	for(i=0;i<4;i++) ENG16ADR = (ENG16ADR<<8) + *(fp++);
	ENG24ADR = 0;	for(i=0;i<4;i++) ENG24ADR = (ENG24ADR<<8) + *(fp++);
	ENG32ADR = 0;	for(i=0;i<4;i++) ENG32ADR = (ENG32ADR<<8) + *(fp++);
	ENG20ADR = 0;	for(i=0;i<4;i++) ENG20ADR = (ENG20ADR<<8) + *(fp++);
	HAN16ADR = 0;	for(i=0;i<4;i++) HAN16ADR = (HAN16ADR<<8) + *(fp++);
	HAN24ADR = 0;	for(i=0;i<4;i++) HAN24ADR = (HAN24ADR<<8) + *(fp++);
	HAN32ADR = 0;	for(i=0;i<4;i++) HAN32ADR = (HAN32ADR<<8) + *(fp++);
	FIG32ADR = 0;	for(i=0;i<4;i++) FIG32ADR = (FIG32ADR<<8) + *(fp++);
//	printf0("\n E16:%lx,E24:%lx,E32:%lx,E20:%lx",ENG16ADR,ENG24ADR,ENG32ADR,ENG20ADR);
	ENG16ADR += FNT_BASE;
	ENG24ADR += FNT_BASE;
	ENG32ADR += FNT_BASE;
	ENG20ADR += FNT_BASE;
	HAN16ADR += FNT_BASE;
	HAN24ADR += FNT_BASE;
	HAN32ADR += FNT_BASE;
	FIG32ADR += FNT_BASE;
	set_size24();
	set_size32();
}

/********************************************************************/
void init_bmps(void){
u8 *fp;
short i;
	fp = (u8*) (BMP_BASE+0);
	for(i=0;i<0x600;i++) bmp_head[i]= *(fp++);
}

/********************************************************************/
#define MIN_SIZE24	12
#define MIN_SIZE32	14
//24X24 영문 Font를 읽어서 가로폭을 알아둔다.
void set_size24(void){
unsigned short i, j, a, b;
unsigned int k;
unsigned char *fp;

	for(j=0;j<96;j++){
		fp = (unsigned char*) ENG24ADR+(j*24*3);
		b=0;
		for(i=0;i<24;i++){
			k = *(fp++);
			k = (k<<8) + *(fp++);
			k = (k<<8) + *(fp++);
			for(a=0;a<24;a++){
				if(k&0x00800000){ if(a>b) b=a; }
				k = k<<1;
			}
		}
		b += 2;
		if(b>=MIN_SIZE24) size24[j+0x20]=b;
		else size24[j+0x20]=MIN_SIZE24;
	}
}

/********************************************************************/
//32X32 영문 Font를 읽어서 가로폭을 알아둔다.
void set_size32(void){
unsigned short i, j, a, b;
unsigned int k;
unsigned char *fp;

	for(j=0;j<96;j++){
		fp = (unsigned char*) ENG32ADR+(j*32*4);
		b=0;
		for(i=0;i<32;i++){
			k = *(fp++);
			k = (k<<8) + *(fp++);
			k = (k<<8) + *(fp++);
			k = (k<<8) + *(fp++);
			for(a=0;a<32;a++){
				if(k&0x80000000){ if(a>b) b=a; }
				k = k<<1;
			}
		}
		b += 2;
		if(b>=MIN_SIZE32) size32[j+0x20]=b;
		else size32[j+0x20]=MIN_SIZE32;
	}
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

//현재사용하지 않는 버퍼를 채운다.
void LCDfill(u8 lno, u32 dat)
{
	int i;
    u32 *pos;
   	pos = (u32*)(LcdBaseAdr1 + (lno*BMP_BLOCK_SIZE));
	for(i=0;i<(BMP_SIZE/4);i++) *(pos++) = dat;
}

/********************************************************************/
void set_LCDpalette256(void){
int i;	
u32 *pLcdPalette;
u32 ww;
	pLcdPalette  =	(u32 *)(LCD_BASE_ADDR + 0x200);	// LCD Palette registers
    for (i=0; i < 0x80; i++){ 
    	ww = PaletteData[(i*2)+1]; 
    	ww = (ww<<16) + (PaletteData[(i*2)+0]&0xFFFF); 
    	*pLcdPalette = ww; 
    	pLcdPalette++; 
    }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void make_UIDNO(u8 pnt){
u32 i;
	for(i=0;i<MAX_USERID;i++) top_str[i+pnt] = tmp_userid[i];		//USER_ID[i];
	top_str[i+pnt]= 0;
}

/********************************************************************/
void draw_box(u16 xs, u16 ys, u16 xe, u16 ye, u8 color, u8 lno){
u16 x, y;
u8 *pos;
	xs -= XS_OFFSET;
	xe -= XS_OFFSET;
	x=271-xs; y=271-xe;	xs=y; xe=x; 
	for(x=xs;x<xe;x++){
	    if(lno==RAST1) pos = (u8*)LcdBaseAdr1 + (x*480) + ys;
    	else pos = (u8*)LcdBaseAdr0 + (x*480) + ys;
		for(y=ys;y<ye;y++){
			*(pos++) = color;
		}
	}	
}

/********************************************************************/
void erase_box(u16 xs, u16 ys, u16 xe, u16 ye){
u16 x, y;
u8 *pos, *pos_b;
	xs -= XS_OFFSET;
	xe -= XS_OFFSET;
	x=271-xs; y=271-xe;	xs=y; xe=x; 
	for(x=xs;x<xe;x++){
    	pos = (u8*)LcdBaseAdr1 + (x*480) + ys;
//    	pos0 = (u8*)LcdBaseAdr0 + (x*480) + ys;
    	pos_b = (u8*)LcdBaseAdr2 + (x*480) + ys;
		for(y=ys;y<ye;y++){
//			*(pos0++) = *(pos_b);
			*(pos++) = *(pos_b++);
		}
	}	
}
/********************************************************************/
void erase_box0(u16 xs, u16 ys, u16 xe, u16 ye){
u16 x, y;
u8 *pos, *pos_b;
	xs -= XS_OFFSET;
	xe -= XS_OFFSET;
	x=271-xs; y=271-xe;	xs=y; xe=x; 
	for(x=xs;x<xe;x++){
    	pos = (u8*)LcdBaseAdr1 + (x*480) + ys;
//    	pos0 = (u8*)LcdBaseAdr0 + (x*480) + ys;
    	pos_b = (u8*)LcdBaseAdr0 + (x*480) + ys;
		for(y=ys;y<ye;y++){
//			*(pos0++) = *(pos_b);
			*(pos++) = *(pos_b++);
		}
	}	
}
/********************************************************************/
//			x-pos	y-pos	length	width	color	rast no
void line_x(u16 xs, u16 ys, u16 ln, u16 wd, u8 color, u8 lno){
u16 x, y;
u8 *pos;
	xs -= XS_OFFSET;
	xs=271-(xs+ln);
	for(x=xs;x<(xs+ln);x++){
	    if(lno==RAST1) pos = (u8*)LcdBaseAdr1 + (x*480) + ys;
    	else pos = (u8*)LcdBaseAdr2 + (x*480) + ys;
		for(y=ys;y<(ys+wd);y++)	*(pos++) = color;
	}	
}

/********************************************************************/
//			x-pos	y-pos	length	width	color	rast no
void line_y(u16 xs, u16 ys, u16 ln, u16 wd, u8 color, u8 lno){
u16 x, y;
u8 *pos;
	xs -= XS_OFFSET;
	xs=271-(xs+wd);
	for(x=xs;x<(xs+wd);x++){
	    if(lno==RAST1) pos = (u8*)LcdBaseAdr1 + (x*480) + ys;
    	else pos = (u8*)LcdBaseAdr2 + (x*480) + ys;
		for(y=ys;y<(ys+ln);y++)	*(pos++) = color;
	}	
}

/********************************************************************/
u32 bin2bcd(u32 bin){
u32 bcd;
	bin %= 1000000;
	bcd = (bin/100000);			bin %= 100000;
	bcd = (bcd<<4)+(bin/10000);	bin %= 10000;
	bcd = (bcd<<4)+(bin/1000);	bin %= 1000;
	bcd = (bcd<<4)+(bin/100);	bin %= 100;
	bcd = (bcd<<4)+(bin/10);	bin %= 10;
	bcd = (bcd<<4)+ bin;
	return(bcd);
}
/********************************************************************/
u8 bcdasc2bin_1(u8 bcd);
u8 bcdasc2bin_1(u8 bcd){
	if((bcd>='0')&&(bcd<='9')) bcd -= '0';
	else if((bcd>='A')&&(bcd<'F')) bcd=(bcd-'A')+10;
	else bcd=0;
	return(bcd);
}

/********************************************************************/
u16 bcdasc2bin(u32 bcdasc){
u16 i;
	i = bcdasc2bin_1(((bcdasc>>24)&0xff)); 
	i = (i*10) + bcdasc2bin_1(((bcdasc>>16)&0xff));
	i = (i*10) + bcdasc2bin_1(((bcdasc>>8)&0xff));
	i = (i*10) + bcdasc2bin_1(((bcdasc>>0)&0xff));
	return(i);
}

/********************************************************************/
u16 hexasc2bin(u32 asc){
u16 i, j;
	j = ((asc>>24)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = j;
	j = ((asc>>16)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = (i<<4) + j;
	j = ((asc>>8)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = (i<<4) + j;
	j = ((asc>>0)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = (i<<4) + j;
	return(i);
}

/********************************************************************/
u16 bcd2bin(u32 bcd){
u16 i;
	i = ((bcd>>24)&0xff);
	i = (i*10) + ((bcd>>16)&0xff);
	i = (i*10) + ((bcd>>8)&0xff);
	i = (i*10) + ((bcd>>0)&0xff);
	return(i);
}

/********************************************************************/
u32 bin2bcdasc(u32 bin){
u16 i;
	i = bin2bcd(bin);
	bin = ((i>>12)&15)+'0'; 
	bin = (bin<<8) + ((i>>8)&15)+'0'; 
	bin = (bin<<8) + ((i>>4)&15)+'0'; 
	bin = (bin<<8) + ((i>>0)&15)+'0';
	return(bin); 	
}

/********************************************************************/
u8 hex2hexasc(u8 hex){
	if(hex>9) return((hex-10)+'A');
	else return(hex+'0');
}	

/********************************************************************/
u32 bin2hexasc4(u32 bin){
u16 i;
	i = bin;
	bin = hex2hexasc((i>>12)&15); 
	bin = (bin<<8) + hex2hexasc((i>>8)&15); 
	bin = (bin<<8) + hex2hexasc((i>>4)&15); 
	bin = (bin<<8) + hex2hexasc((i>>0)&15);
	return(bin);
}
/********************************************************************/
u16 bcdasc2bin4(u32 bcdasc){
u16 i;
	i = bcdasc2bin_1(((bcdasc>>24)&0xff)); 
	i = (i*10) + bcdasc2bin_1(((bcdasc>>16)&0xff));
	i = (i*10) + bcdasc2bin_1(((bcdasc>>8)&0xff));
	i = (i*10) + bcdasc2bin_1(((bcdasc>>0)&0xff));
	return(i);
}
//=============================================================
u32 bin2bcd4(u32 bin){
u32 bcd;
	bin %= 1000000;
	bcd = (bin/100000);			bin %= 100000;
	bcd = (bcd<<4)+(bin/10000);	bin %= 10000;
	bcd = (bcd<<4)+(bin/1000);	bin %= 1000;
	bcd = (bcd<<4)+(bin/100);	bin %= 100;
	bcd = (bcd<<4)+(bin/10);	bin %= 10;
	bcd = (bcd<<4)+ bin;
	return(bcd);
}
//=============================================================
u32 bin2bcdasc4(u32 bin){
u16 i;
	i = bin2bcd4(bin);
	bin = ((i>>12)&15)+'0'; 
	bin = (bin<<8) + ((i>>8)&15)+'0'; 
	bin = (bin<<8) + ((i>>4)&15)+'0'; 
	bin = (bin<<8) + ((i>>0)&15)+'0';
	return(bin); 	
}
/******************************************************************************/
u8 hexasc2bin_1(u8 hexasc){
	if(hexasc<='9') return(hexasc-'0');
	else return((hexasc-'A')+10);
}
/********************************************************************/
void time_copy(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec){
int i;//, j;	
//year=88; month=8; day=28; hour=8; min=58; sec=58;
	i = bin2bcdasc(year);
	top_str[0]='2';
	top_str[1]='0';
	top_str[2]=i>>8;
	top_str[3]=i&0xff;
	i = bin2bcdasc(month);
	top_str[4]='/';
	top_str[5]=i>>8;
	top_str[6]=i&0xff;
	i = bin2bcdasc(day);
	top_str[7]='/';
	top_str[8]=i>>8;
	top_str[9]=i&0xff;
	top_str[10]=' ';
	if(hour<12){
//		msg24(20,28,0xff,"20%d/%d/%d AM %d:%d:%d", year, month, day, hour, min, sec);
		top_str[11]='A';
		if(!hour) hour=12;
	}else{
//		msg24(20,28,0xff,"20%d/%d/%d PM %d:%d:%d", year, month, day, hour-11, min, sec);
		top_str[11]='P';
		if(hour>12)	hour -= 12;
	}
	i = bin2bcdasc(hour);
	top_str[12]='M';
	top_str[13]=' ';
	top_str[14]=i>>8;
	top_str[15]=i&0xff;
	i = bin2bcdasc(min);
	top_str[16]=':';
	top_str[17]=i>>8;
	top_str[18]=i&0xff;
	i = bin2bcdasc(sec);
	top_str[19]=':';
	top_str[20]=i>>8;
	top_str[21]=i&0xff;
	top_str[22]=' ';
	top_str[23]=0;
//printf0("\n Aa-%d", get_xpos24(top_str, 22));
//printf0("\n Aa-%d", get_xpos32(top_str, 22));
}
/********************************************************************/
void time_copy_adj(u8 hour, u8 min, u8 sec){
int i;//, j;	
//year=88; month=8; day=28; hour=8; min=58; sec=58;
	i = bin2bcdasc(hour);
	top_str[0]=i>>8;
	top_str[1]=i&0xff;
	i = bin2bcdasc(min);
	top_str[2]=':';
	top_str[3]=i>>8;
	top_str[4]=i&0xff;
	i = bin2bcdasc(sec);
	top_str[5]=':';
	top_str[6]=i>>8;
	top_str[7]=i&0xff;
	top_str[8]=0;
}

/********************************************************************/
void date_copy_adj(u8 year, u8 month, u8 day){
int i;//, j;	
//year=88; month=8; day=28; hour=8; min=58; sec=58;
	i = bin2bcdasc(year);
//	top_str[0]='2';
//	top_str[1]='0';
	top_str[0]=i>>8;
	top_str[1]=i&0xff;
	i = bin2bcdasc(month);
	top_str[2]='/';
	top_str[3]=i>>8;
	top_str[4]=i&0xff;
	i = bin2bcdasc(day);
	top_str[5]='/';
	top_str[6]=i>>8;
	top_str[7]=i&0xff;
	top_str[8]=0;
}

/********************************************************************/
void time_copy24(void){
int i, j;
	j = 23;
	i = 315 - get_xpos24(top_str, 22);
	while(i>0){
		top_str[j++] = ' ';
		i -= size24[' '];
	}
	top_str[j]=0;
}

/********************************************************************/
u16 get_xpos24(char *str, u16 xn){
u16 i, j;
	j=0;
	for(i=0;i<xn;i++){
		if(str[i]<0x80){
			j += size24[str[i]];
		}else j += 12;					//한글인경우
	}
	return(j);
}

/********************************************************************/
u16 get_xpos32(char *str, u16 xn){
u16 i, j;
	j=0;
	for(i=0;i<xn;i++){
		if(str[i]<0x80){
			j += size32[str[i]];
		}else j += 16;
	}
	return(j);
}

/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
#define BLUWW	0x03030303
void LCDbmpBlue(void){
	int i;
    u32 *pos;
printf0("\t Blue-");
   	pos = (u32*)(LcdBaseAdr1);
	for(i=0;i<(BMP_SIZE/4);i++) *(pos++) = BLUWW;
}

//############################################################################################################
//############################################################################################################
//############################################################################################################

//============================ 32X32 =================================

#define HSIZE32 30

void out_line_32(int x_pos, int y_pos, char *str, u8 color, u8 lno){
u16 i=0;
int j;
	x_pos -= XS_OFFSET;
	x_pos = 272 - x_pos;
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_32e(x_pos, y_pos, j, color, lno);
		}else{
			j = ((j-0xB0)*96) + (str[i++]-0xA0);
			out_text_32h(x_pos, y_pos, j, color, lno);
			x_pos -= HSIZE32;
		}		
	}	
}

/********************************************************************/
int out_text_32e(int x_pos, int y_pos, int cod, u8 color, u8 lno){
u16 i, a, HS;
u32 j,k,m,n;
u8 *pos_s, *pos_d, *pos_b;
u32 buff32[32], save32[32];

	HS = size32[cod];
	cod -= 0x20;
	pos_s = (unsigned char*) ENG32ADR+(cod*32*4);
//  printf0("**%2x\n",color);
	for(i=0;i<32;i++){
		k = *(pos_s++);
		k = (k<<8) + *(pos_s++);
		k = (k<<8) + *(pos_s++);
		k = (k<<8) + *(pos_s++);
		save32[i]=k;
	}
	n = 0x80000000;
	for(a=0;a<32;a++){
		j=0;
		m=0x80000000;
		for(i=0;i<32;i++){
			if(save32[i]&n) j |= m;
			m >>= 1;
		}
		n >>= 1;
		buff32[a] = j;	
	}
	
    if(lno==RAST1) pos_d = (unsigned char*)(LcdBaseAdr1) + y_pos + (x_pos*480);
    else pos_d = (unsigned char*)(LcdBaseAdr2) + y_pos + (x_pos*480);
    pos_b = (unsigned char*)(LcdBaseAdr0) + y_pos + (x_pos*480);
	for(i=0;i<32;i++){
		k = buff32[i];
		for(a=0;a<HSIZE32;a++){
			if(k&0x80000000) *(pos_d) = color;
			else *(pos_d) = *pos_b;
			k = k<<1;
			pos_d++;
			pos_b++;
		}
		pos_d -= 480 + HSIZE32;
		pos_b -= 480 + HSIZE32;
	}
	return(x_pos-HS);
}

/********************************************************************/
void out_text_32h(int x_pos, int y_pos, int cod, u8 color, u8 lno){
u16 i, a;
u32 k, j, m, n;
u8 *pos_s, *pos_d, *pos_b;
u32 buff32[32], save32[32];
	pos_s = (unsigned char*) HAN32ADR+(cod*32*4);
//	printf0("\n***** %4x",cod);
	for(i=0;i<32;i++){
		k = *(pos_s++);
		k = (k<<8) + *(pos_s++);
		k = (k<<8) + *(pos_s++);
		k = (k<<8) + *(pos_s++);
		save32[i]=k;
	}
	n = 0x80000000;
	for(a=0;a<32;a++){
		j=0;
		m=0x80000000;
		for(i=0;i<32;i++){
			if(save32[i]&n) j |= m;
			m >>= 1;
		}
		n >>= 1;
		buff32[a] = j;	
	}
	
    if(lno==RAST1) pos_d = (unsigned char*)(LcdBaseAdr1) + y_pos + (x_pos*480);
    else pos_d = (unsigned char*)(LcdBaseAdr2) + y_pos + (x_pos*480);
    pos_b = (unsigned char*)(LcdBaseAdr0) + y_pos + (x_pos*480);
	for(i=0;i<32;i++){
		k = buff32[i];
		for(a=0;a<HSIZE32;a++){
			if(k&0x40000000) *(pos_d) = color;
			else *(pos_d) = *pos_b;
			k = k<<1;
			pos_d++;
			pos_b++;
		}
		pos_d -= 480 + HSIZE32;
		pos_b -= 480 + HSIZE32;
	}
}

//============================ 24X24 =================================

void out_line_24(int x_pos, int y_pos, char *str, u8 color, u8 lno){
u16 i=0;
int j;
	x_pos -= XS_OFFSET;
	x_pos = 272 - x_pos;
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_24e(x_pos, y_pos, j, color, lno);
		}else{
			j = (((j-0xB0)*94) + (str[i++]-0xA1)) * 2;
			out_text_24h(x_pos, y_pos, j, color, lno);
			x_pos -= 24;
		}		
	}	
}

//============================ 20X20 =================================

void out_line_20(int x_pos, int y_pos, char *str, u8 color, u8 lno){
u16 i=0;
int j;
	x_pos -= XS_OFFSET;
	x_pos = 272 - x_pos;
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_20e(x_pos, y_pos, j, color, lno);
		}else{
			x_pos += 10;
		}		
	}	
}

/********************************************************************/
int out_text_24e(int x_pos, int y_pos, int cod, u8 color, u8 lno){
u16 i, a, HS;
u32 j,k,m,n;
u8 *pos_s, *pos_d, *pos_b;
u32 buff24[24], save24[24];
	
	HS = size24[cod];
	cod -= 0x20;
	pos_s = (u8*) ENG24ADR+(cod*24*3);
//  printf0("**%2x\n",color);
	for(i=0;i<24;i++){
		k = *(pos_s++);
		k = (k<<8) + *(pos_s++);
		k = (k<<8) + *(pos_s++);
		save24[i]=k;
	}
	n = 0x800000;
	for(a=0;a<24;a++){
		j=0;
		m=0x800000;
		for(i=0;i<24;i++){ if(save24[i]&n) j |= m;	m >>= 1; }
		n >>= 1;
		buff24[a] = j;	
	}
    if(lno==RAST1) pos_d = (u8*)(LcdBaseAdr1) + y_pos + (x_pos*480);
    else pos_d = (u8*)(LcdBaseAdr2) + y_pos + (x_pos*480);
    pos_b = (u8*)(LcdBaseAdr0) + y_pos + (x_pos*480);
//  printf0("**%2x\n",color);
	for(i=0;i<24;i++){
		k = buff24[i];
		for(a=0;a<24;a++){
			if(k&0x800000) *(pos_d) = color;
			else *(pos_d) = *(pos_b);
			k = k<<1;
			pos_d++;
			pos_b++;
		}
		pos_d -= 480 + 24;
		pos_b -= 480 + 24;
	}
	return(x_pos-HS);
}

/********************************************************************/
int out_text_20e(int x_pos, int y_pos, int cod, u8 color, u8 lno){
u16 i, a, HS;
u32 j,k,m,n;
u8 *pos_s, *pos_d, *pos_b;
u32 buff20[24], save20[24];
	
//	HS = size24[cod];
	HS = 11;
	cod -= 0x20;
	pos_s = (u8*) ENG20ADR+(cod*24*3);
//  printf0("**%2x\n",color);
	for(i=0;i<24;i++){
		k = *(pos_s++);
		k = (k<<8) + *(pos_s++);
		k = (k<<8) + *(pos_s++);
		save20[i]=k;
	}
	n = 0x800000;
	for(a=0;a<24;a++){
		j=0;
		m=0x800000;
		for(i=0;i<24;i++){ if(save20[i]&n) j |= m;	m >>= 1; }
		n >>= 1;
		buff20[a] = j;	
	}
	y_pos += 4;
    if(lno==RAST1) pos_d = (u8*)(LcdBaseAdr1) + y_pos + (x_pos*480);
    else pos_d = (u8*)(LcdBaseAdr2) + y_pos + (x_pos*480);
    pos_b = (u8*)(LcdBaseAdr0) + y_pos + (x_pos*480);
//  printf0("**%2x\n",color);
#define W20S	21
	for(i=0;i<10;i++){
		k = buff20[i];
		for(a=0;a<W20S;a++){
			if(k&0x800000) *(pos_d) = color;
			else *(pos_d) = *(pos_b);
			k = k<<1;
			pos_d++;
			pos_b++;
		}
		pos_d -= 480 + W20S;
		pos_b -= 480 + W20S;
	}
	return(x_pos-HS);
}

/******************************************************************************/
#define BLANK 	 ((19*7*72)+(21*2*72)+(27*2*72))
#define max_jaum 20
#define max_moum 29
#define max_bach 29
#define offsetM  (19*7*72)
#define offsetMB (offsetM+(21*1*72))		//받침이 있는 모음
#define offsetB  (offsetM+(21*2*72))
/******************************************************************************/

void out_text_24h(int x_pos, int y_pos, int cod, u8 color, u8 lno){
u16 i, a;
u8 hi, lo;
u16 j1, m1, b1;
u16 jp, mp, bp;
u32 j,k,m,n;
u8 *fp1, *fp2, *fp3, *pos_d;
u8 linebuff[72];
u8  *pos_b;
u32 buff24[24], save24[24];

    if(cod>=4700) return;
    hi=kssm[cod]; lo=kssm[cod+1];

    j1 = ((hi>>2)&0x1f);
    if(j1>max_jaum) j1=32; else j1 = jaum_tb[j1];
    m1 = (((hi<<3)&0x18) | (lo>>5));
    if(m1>max_moum) m1=32; else m1 = moum_tb[m1];
    b1 = lo&0x1f;

    if(b1<2){
        if(j1>max_jaum) jp=BLANK;
        else{ jp = (jm_tb[m1]*72*19) + (j1*72); }
        if(m1>max_moum) mp=BLANK;
        else{ mp = (m1*72) + offsetM; }
//printf0("\n 2-jp:%d,mp:%d",jp,mp);
		fp1 = (u8*) HAN24ADR+jp;
		fp2 = (u8*) HAN24ADR+mp;
        for(m1=0;m1<72;m1++){ linebuff[m1] = *(fp1++) | *(fp2++); }
    }else{
        if(b1>max_bach) b1=0;
        b1 = bach_tb[b1];
        if(b1>max_bach) bp=BLANK;
        else{ bp = (bb_tb[m1]*72*27) + offsetB + b1*72; }

        if(j1>max_jaum) jp=BLANK;
        else{ jp = (jmb_tb[m1]*72*19) + (j1*72); }

        if(m1>max_moum) mp=BLANK;
        else{ mp = (m1*72) + offsetMB; }
        
//printf0("\n 3-jp:%d,mp:%d,bp:%d",jp,mp,bp);
		fp1 = (u8*) HAN24ADR+jp;
		fp2 = (u8*) HAN24ADR+mp;
		fp3 = (u8*) HAN24ADR+bp;
        for(m1=0;m1<72;m1++){ linebuff[m1] = *(fp1++) | *(fp2++) | *(fp3++); }
    }

	j1=0;
	for(i=0;i<24;i++){
		k = linebuff[j1++];
		k = (k<<8) + linebuff[j1++];
		k = (k<<8) + linebuff[j1++];
		save24[i]=k;
	}
	n = 0x800000;
	for(a=0;a<24;a++){
		j=0;
		m=0x800000;
		for(i=0;i<24;i++){
			if(save24[i]&n) j |= m;
			m >>= 1;
		}
		n >>= 1;
		buff24[a] = j;	
	}

    if(lno==RAST1) pos_d = (unsigned char*)(LcdBaseAdr1) + y_pos + (x_pos*480);
    else pos_d = (unsigned char*)(LcdBaseAdr2) + y_pos + (x_pos*480);
    pos_b = (u8*)(LcdBaseAdr0) + y_pos + (x_pos*480);
//  printf0("**%2x\n",color);
	for(j1=0;j1<24;j1++ ){
		k = buff24[j1];
		for(b1=0;b1<24;b1++){
			if(k&0x00800000) *(pos_d) = color;
			else *(pos_d) = *(pos_b);
			k = k<<1;
			pos_d++;
			pos_b++;
		}
		pos_d -= 480 + 24;
		pos_b -= 480 + 24;
	}
}

//============================== 16X16 ======================================

void out_line_16(int x_pos, int y_pos, char *str, u8 color, u8 lno){
u16 i=0;
int j;
	x_pos -= XS_OFFSET;
	x_pos = 272 - x_pos;
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_16e(x_pos, y_pos, j, color, lno);
		}else{
			j = ((j-0xB0)*96) + (str[i++]-0xA0);
			out_text_16h(x_pos, y_pos, j, color, lno);
			x_pos -= 16;
		}		
	}	
}

/********************************************************************/
int out_text_16e(int x_pos, int y_pos, int cod, u8 color, u8 lno){
u16 i, a;
u16 k, j, m, n;
u8 *pos_s, *pos_d, *pos_b;
u16 buff16[16], save16[16];
	cod -= 0x20;
	pos_s = (unsigned char*) ENG16ADR+(cod*16*2);
//	printf0("\n***** %4x",cod);
	for(i=0;i<16;i++){
		k = *(pos_s++);
		k = (k<<8) + *(pos_s++);
		save16[i]=k;
	}
	n = 0x8000;
	for(a=0;a<16;a++){
		j=0;
		m=0x8000;
		for(i=0;i<16;i++){
			if(save16[i]&n) j |= m;
			m >>= 1;
		}
		n >>= 1;
		buff16[a] = j;	
	}
	
    if(lno==RAST1) pos_d = (unsigned char*)(LcdBaseAdr1) + y_pos + (x_pos*480);
    else pos_d = (unsigned char*)(LcdBaseAdr2) + y_pos + (x_pos*480);
    pos_b = (unsigned char*)(LcdBaseAdr0) + y_pos + (x_pos*480);
//  printf0("**%2x\n",color);
	for(i=0;i<16;i++){
		k = buff16[i];
		for(a=0;a<16;a++){
			if(k&0x8000) *(pos_d) = color;
			else *(pos_d) = *pos_b;
			k = k<<1;
			pos_d++;
			pos_b++;
		}
		pos_d -= 480 + 16;
		pos_b -= 480 + 16;
	}
	return(x_pos-8);
}

/********************************************************************/
void out_text_16h(int x_pos, int y_pos, int cod, u8 color, u8 lno){
unsigned short i, a;
u16 k, j, m, n;
u8 *pos_s, *pos_d, *pos_b;
u16 buff16[16], save16[16];
	pos_s = (unsigned char*) HAN16ADR+(cod*16*2);
//	printf0("\n***** %4x",cod);
	for(i=0;i<16;i++){
		k = *(pos_s++);
		k = (k<<8) + *(pos_s++);
		save16[i]=k;
	}
	n = 0x8000;
	for(a=0;a<16;a++){
		j=0;
		m=0x8000;
		for(i=0;i<16;i++){
			if(save16[i]&n) j |= m;
			m >>= 1;
		}
		n >>= 1;
		buff16[a] = j;	
	}
	
    if(lno==RAST1) pos_d = (unsigned char*)(LcdBaseAdr1) + y_pos + (x_pos*480);
    else pos_d = (unsigned char*)(LcdBaseAdr2) + y_pos + (x_pos*480);
    pos_b = (unsigned char*)(LcdBaseAdr0) + y_pos + (x_pos*480);
//  printf0("**%2x\n",color);
	for(i=0;i<16;i++){
		k = buff16[i];
		for(a=0;a<16;a++){
			if(k&0x8000) *(pos_d) = color;
			else *(pos_d) = *pos_b;
			k = k<<1;
			pos_d++;
			pos_b++;
		}
		pos_d -= 480 + 16;
		pos_b -= 480 + 16;
	}
}
//############################################################################################################
//############################################################################################################
//############################################################################################################
//############################################################################################################

/********************************************************************/
void put_strip_name(void){
u8 b1;
	for(b1=0;b1<10;b1++) top_str[b1]=STORE_RAM[STRIP_RAM+(STRIPNO*StripOffset)+2+b1];
	top_str[b1]=0;
}
/********************************************************************/
u16 put_seqno(u16 sno){
u16 b1;	
	sprintf(top_str, "%d", sno);
	for(b1=0;b1<10;b1++) if(top_str[b1]==NULL) break;
	b1 = (96-(b1*8))/2;
	return(b1);
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
