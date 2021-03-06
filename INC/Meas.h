
#define StripOffset		27		//OLD
//#define StripOffset		24		//NEW
//Total   : 98mm 	992dot
//Start1  : 8.6mm	87dot
//Start2  : 4.0mm	40dot
//PAD GAP : 7.5mm 	76dot
#define StartOffset		40
#define PadGapOffset	70
#define MAX_PAD_NO		11

#define MES_TIME		7900
#define SLOW_TIME 		60000 + MES_TIME
//#define FAST_TIME 	10000 + MES_TIME
#define FAST_TIME 		500 + MES_TIME
#define ANAL_TIME 		7200

#define MaxRawData		960		//840		//was 960
#define MULTI			1
#define COMBO			2
#define RESULT_SIZE		198+10

#define ACpnt			18
#define PCpnt			19
#define CCpnt			20


#ifdef	_MEAS_C_
u16 dataRED1[data_RGB], dataGRN1[data_RGB], dataBLU1[data_RGB];
u16 dataRED2[data_RGB], dataGRN2[data_RGB], dataBLU2[data_RGB];
u8 STRIP_INFO[StripOffset];
u16	SYS_CURVE_RED[14], SYS_CURVE_GRN[14], SYS_CURVE_BLU[14];
u16	CURVE_RED[12], CURVE_GRN[12], CURVE_BLU[12];
u16	store_COLOR_NO, store_TURBI_NO;

u16	PARA_NEW[17][7][3];
u8  m_final_results[12];   //???? ???? (0- ff)
u16 StripPosition[16];
u16 start_strip_point;
u16	m_para_min,	m_para_mean, m_para_max;
u8  GIULGI;
u8  MEASyear,MEASmonth,MEASday,MEAShour,MEASmin,MEASsec;
u16 END_RESULT;
u8  min_run, sec_run;
u8  result_mode;
u8  charbuff[64];
s8  datPRO, datALB, datCRE, datCAL,datMDA,datLDH,datBHB,datPCR;
u8  weight[22];
u16 wait_time;
u8  buffRESULT[1024];	
u16 cnt100;
int meas_time;

void span_curve(void);
void get_start_strip_point(void);
void get_pad_data(void);
void run_measure(void);
void xchg_rgb(void);
void send_result(void);
void wait_meas(u16 wsec);
void conv_result(void);
void pass_curve(void);
void calc_ratio1(void);
void calc_ratio2(void);
void print_result(u16 meas_no);
void print_sound(void);
int  print_result_store(u16 ssno, u16 eeno);
int  send_result_store(u16 ssno, u16 eeno);
void memory_erase(u16 ssno, u16 eeno);
short view_result_store(u16 view_seqno);
void send_result_one(u16 sdno);
void delete_result_one(u16 sdno);
void delete_result_store(u16 ssno, u16 eeno);

//const char *pt_size = "001001111000011111111111111111111111111111";
                     // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
const char pt_size[] = {0,0,1,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1};

#else 
extern u16 dataRED1[data_RGB], dataGRN1[data_RGB], dataBLU1[data_RGB];
extern u16 dataRED2[data_RGB], dataGRN2[data_RGB], dataBLU2[data_RGB];
extern u8 STRIP_INFO[StripOffset];
extern u16	SYS_CURVE_RED[14], SYS_CURVE_GRN[14], SYS_CURVE_BLU[14];
extern u16	CURVE_RED[12], CURVE_GRN[12], CURVE_BLU[12];
extern u16	store_COLOR_NO, store_TURBI_NO;

extern u16	PARA_NEW[17][7][3];
extern u8  m_final_results[12];   //???? ???? (0- ff)
extern u16 StripPosition[16];
extern u16 start_strip_point;
extern u16	m_para_min,	m_para_mean, m_para_max;
extern u8  GIULGI;
extern u8  MEASyear,MEASmonth,MEASday,MEAShour,MEASmin,MEASsec;
extern u16 END_RESULT;
extern u8  min_run, sec_run;
extern u8  result_mode;
extern u8  charbuff[64];
extern s8  datPRO, datALB, datCRE, datCAL,datMDA,datLDH,datBHB;
extern u8  weight[22];
extern u16 wait_time;
extern u8  buffRESULT[1024];	
extern u16 cnt100;
extern int meas_time;

extern void span_curve(void);
extern void get_start_strip_point(void);
extern void get_pad_data(void);
extern void run_measure(void);
extern void xchg_rgb(void);
extern void send_result(void);
extern void wait_meas(u16 wsec);
extern void conv_result(void);
extern void pass_curve(void);
extern void calc_ratio1(void);
extern void calc_ratio2(void);
extern void print_result(u16 meas_no);
extern void print_sound(void);
extern int  print_result_store(u16 ssno, u16 eeno);
extern int  send_result_store(u16 ssno, u16 eeno);
extern void memory_erase(u16 ssno, u16 eeno);
extern short view_result_store(u16 view_seqno);
extern void send_result_one(u16 sdno);
extern void delete_result_one(u16 sdno);
extern void delete_result_store(u16 ssno, u16 eeno);

#endif

#ifdef	_MEAS_C_
/*                           01234567890123456789012345678901 */
//const char *PrintLine1  = "= = = = SEQ# : 0000  = = = = = =";
//const char *PrintLine2  = " 2000/01/01          AM 00:00:00";
//const char *PrintLine3  = "                ";
//const char *PrintLine4  = " ID:            ";
//const char *PrintLine6  = "= = = = = = = = = = = = = = = = ";

//=================================================================================================
const char alb_item[5][5]= {	
//	{0,3,3,3,0},	//0
	{4,3,3,3,0},	//0
	{1,2,2,3,0},	//1
	{1,2,2,2,0},	//2
	{1,1,2,2,0},	//3
	{1,1,1,2,0}		//4
};
/***************************************************************************************/
const char pro_item[6][5]= {	
	{1,1,1,1,1},	//0
	{3,1,1,1,1},	//1
	{3,2,1,1,1},	//2
	{3,3,2,2,1},	//3
	{3,3,3,3,2},	//4
	{3,3,3,3,3}		//5
};
/***************************************************************************************/
const char albumin_tb[5][32]= {	
//    01234567890123456789012345678901	
//	{"     Re-analysis                "},	//0
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	//0
	{' ',' ',' ',' ',' ',' ',' ',' ','N','o','r','m','a','l',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	//1
	{' ',' ',' ',' ',' ',' ',' ',' ','A','b','n','o','r','m','a','l',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	//2
	{' ',' ',' ',' ',' ',' ',' ',' ','H','i','g','h',' ',' ',' ',' ',' ','a','b','n','o','r','m','a','l',' ',' ',' ',' ',' ',' ',' '},	//3
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}	//4
//---?߰? ??
};

const char alb_tb_text_1[5][5][8]= {	
//    0123456789012345678901234567890	
	{{"Retest  "},	{"33.3    "},	{"88.9    "},	{"166.7   "},	{"mg/mmol "}},//0
	{{"2.3     "},	{"6.8     "},	{"18.2    "},	{"34.1    "},	{"mg/mmol "}},//1
	{{"1.1     "},	{"3.4     "},	{"9.1     "},	{"17.0    "},	{"mg/mmol "}},//2
	{{"0.6     "},	{"1.7     "},	{"4.5     "},	{"8.5     "},	{"mg/mmol "}},//3
	{{"0.4     "},	{"1.1     "},	{"3.0     "},	{"5.7     "},	{"mg/mmol "}},//4
};
const char albumin_tb_1[5][13]= {	
	//0123456789012
	{"             "},	//0
	{"Normal       "},	//1
	{"Abnormal     "},	//2
	{"High abnormal"},	//3
	{"             "}	//4
};

const char alb_tb_text_c[4][8][8]= {	
	{{"Retest  "},	{"20.0    "},	{"10.0    "},	{"5.0     "},	{"3.3     "},	{"        "},	{"        "},	{"mg/g    "}},//0
	{{"300.0   "},	{"60.0    "},	{"30.0    "},	{"15.0    "},	{"10.0    "},	{"        "},	{"        "},	{"mg/g    "}},//1
	{{"800.0   "},	{"160.0   "},	{"80.0    "},	{"40.0    "},	{"26.7    "},	{"        "},	{"        "},	{"mg/g    "}},//2
	{{"1500.0  "},	{"300.0   "},	{"150.0   "},	{"75.0    "},	{"50.0    "},	{"        "},	{"        "},	{"mg/g    "}} //3
};

const char pro_tb_text_c[6][8][8]= {	
	{{"0.0     "},	{"0.0     "},	{"0.0     "},	{"0.0     "},	{"0.0     "},	{"        "},	{"        "},	{"mg/g    "}},
	{{"1500.0  "},	{"300.0   "},	{"150.0   "},	{"75.0    "},	{"50.0    "},	{"        "},	{"        "},	{"mg/g    "}},     
	{{"3000.0  "},	{"600.0   "},	{"300.0   "},	{"150.0   "},	{"100.0   "},	{"        "},	{"        "},	{"mg/g    "}},   
	{{"10000.0 "},	{"2000.0  "},	{"1000.0  "},	{"500.0   "},	{"333.3   "},	{"        "},	{"        "},	{"mg/g    "}},     
	{{"30000.0 "},	{"6000.0  "},	{"3000.0  "},	{"1500.0  "},	{"1000.0  "},	{"        "},	{"        "},	{"mg/g    "}},     
	{{"100000.0"},	{"20000.0 "},	{"10000.0 "},	{"5000.0  "},	{"3333.3  "},	{"        "},	{"        "},	{"mg/g    "}} 
};

const char pcr_tb_text_c[8][8][8]= {	
	{{"100.0   "},	{"20.0    "},	{"10.0    "},	{"5.0     "},	{"3.3     "},	{"        "},	{"        "},	{"mg/g    "}}, 
	{{"300.0   "},	{"60.0    "},	{"30.0    "},	{"15.0    "},	{"10.0    "},	{"        "},	{"        "},	{"mg/g    "}}, 
	{{"800.0   "},	{"160.0   "},	{"80.0    "},	{"40.0    "},	{"26.7    "},	{"        "},	{"        "},	{"mg/g    "}}, 
	{{"1500.0  "},	{"300.0   "},	{"150.0   "},	{"75.0    "},	{"50.0    "},	{"        "},	{"        "},	{"mg/g    "}}, 
	{{"3000.0  "},	{"600.0   "},	{"300.0   "},	{"150.0   "},	{"100.0   "},	{"        "},	{"        "},	{"mg/g    "}}, 
	{{"10000.0 "},	{"2000.0  "},	{"1000.0  "},	{"500.0   "},	{"333.3   "},	{"        "},	{"        "},	{"mg/g    "}}, 
	{{"30000.0 "},	{"6000.0  "},	{"3000.0  "},	{"1500.0  "},	{"1000.0  "},	{"        "},	{"        "},	{"mg/g    "}},
	{{"100000.0"},	{"20000.0 "},	{"10000.0 "},	{"5000.0  "},	{"3333.3  "},	{"        "},	{"        "},	{"mg/g    "}}  
};
const char alb_tb_text[5][5][8]= {	
//    0123456789012345678901234567890	
	{{"Retest  "},	{"300     "},	{"800     "},	{"1500    "},	{"mg/g    "}},//0
	{{"20      "},	{"60      "},	{"160     "},	{"300     "},	{"mg/g    "}},//1
	{{"10      "},	{"30      "},	{"80      "},	{"150     "},	{"mg/g    "}},//2
	{{"5       "},	{"15      "},	{"40      "},	{"75      "},	{"mg/g    "}},//3
	{{"3.3     "},	{"10      "},	{"26.7    "},	{"50      "},	{"mg/g    "}},//4
};

const char pro_tb_text[6][8][8]= {	
	{{"0.00    "},	{"0.00    "},	{"0.00    "},	{"0.00    "},	{"0.00    "},	{"        "},	{"        "},	{"mg/mmol "}},
	{{"166.67  "},	{"34.09   "},	{"17.05   "},	{"8.47    "},	{"5.66    "},	{"        "},	{"        "},	{"mg/mmol "}}, 
	{{"333.33  "},	{"68.18   "},	{"34.09   "},	{"16.95   "},	{"11.32   "},	{"        "},	{"        "},	{"mg/mmol "}},     
	{{"1111.11 "},	{"227.27  "},	{"113.64  "},	{"56.50   "},	{"37.74   "},	{"        "},	{"        "},	{"mg/mmol "}},     
	{{"3333.33 "},	{"681.82  "},	{"340.91  "},	{"169.49  "},	{"113.21  "},	{"        "},	{"        "},	{"mg/mmol "}},     
	{{"11111.11"},	{"2272.73 "},	{"1136.36 "},	{"564.97  "},	{"377.36  "},	{"        "},	{"        "},	{"mg/mmol "}}	 
};


const char pcr_tb_text[8][8][8]= {	
	{{"11.11   "},	{"2.27    "},	{"1.14    "},	{"0.56    "},	{"0.38    "},	{"        "},	{"        "},	{"mg/mmol "}},
	{{"33.33   "},	{"6.82    "},	{"3.41    "},	{"1.69    "},	{"1.13    "},	{"        "},	{"        "},	{"mg/mmol "}},
	{{"88.89   "},	{"18.18   "},	{"9.09    "},	{"4.52    "},	{"3.02    "},	{"        "},	{"        "},	{"mg/mmol "}},
	{{"166.67  "},	{"34.09   "},	{"17.05   "},	{"8.47    "},	{"5.66    "},	{"        "},	{"        "},	{"mg/mmol "}}, 
	{{"333.33  "},	{"68.18   "},	{"34.09   "},	{"16.95   "},	{"11.32   "},	{"        "},	{"        "},	{"mg/mmol "}},     
	{{"1111.11 "},	{"227.27  "},	{"113.64  "},	{"56.50   "},	{"37.74   "},	{"        "},	{"        "},	{"mg/mmol "}},     
	{{"3333.33 "},	{"681.82  "},	{"340.91  "},	{"169.49  "},	{"113.21  "},	{"        "},	{"        "},	{"mg/mmol "}},     
	{{"11111.11"},	{"2272.73 "},	{"1136.36 "},	{"564.97  "},	{"377.36  "},	{"        "},	{"        "},	{"mg/mmol "}}	 
};

/***************************************************************************************/
//const char *text_AC = " A:C ";
const char text_AC[] = {' ','A',':','C',' '};
//const char *text_PC = " P:C ";
const char text_PC[] = {' ','P',':','C',' '};
//const char *text_CC = " C:C ";
const char text_CC[] = {' ','C',':','C',' '};
/***************************************************************************************/
const u8 maxitem[11]={7,7,7,7,7,7,7,7,7,7,7}; // ph->7degrees,  OBD->6degrees
/***************************************************************************************/
const u8 normal_range[12][8]={	//nega reason
// min, max
	{ 0,0,0,0,0,0,0,1 },	//{ 1,7 },//color
	{ 0,0,1,1,1,1,1,1 },	//{ 1,2 },//uro
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//glu
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//bil
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//ket
	{ 0,0,0,0,0,0,0,1 },	//{ 1,7 },//sg
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//bld
	{ 0,0,0,0,0,1,1,1 },	//{ 1,5 },//ph
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//pro
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//nit
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//leu
	{ 0,1,1,1,1,1,1,1 }		//{ 1,1 } //asa
};
/***************************************************************************************/
const u8 item_string[21][8] = {
	{' ',' ',' ',' ',' ',' ',' ',' '},	//0
	{' ',' ',' ',' ','U','R','O',' '},	//1
	{' ',' ',' ',' ','G','L','U',' '},	//2
	{' ',' ',' ',' ','B','I','L',' '},	//3
	{' ',' ',' ',' ','K','E','T',' '},	//4
	{' ',' ',' ',' ','S','G',' ',' '},	//5
	{' ',' ',' ',' ','B','L','D',' '},	//6
	{' ',' ',' ',' ','p','H',' ',' '},	//7
	{' ',' ',' ',' ','P','R','O',' '},	//8
	{' ',' ',' ',' ','N','I','T',' '},	//9
	{' ',' ',' ',' ','L','E','U',' '},	//A	10
	{' ',' ',' ',' ','A','S','A',' '},	//B	11
	{' ',' ',' ',' ','A','L','B',' '},	//C	12
	{' ',' ',' ',' ','C','R','E',' '},	//D	13
	{' ',' ',' ',' ','M','D','A',' '},	//E	14
	{' ',' ',' ',' ','B','H','B',' '},	//F	15
	{' ',' ',' ',' ','C','A','L',' '},	//G	16
	{' ',' ',' ',' ','L','D','H',' '},	//H	17
	{' ',' ',' ',' ','A','/','C',' '},	//I 18
	{' ',' ',' ',' ','P','/','C',' '},	//J 19
	{' ',' ',' ',' ','C','/','C',' '}	//K 20
};      

const u8 item_string1[21][8] = {	//positive
	{'=','>',' ',' ',' ',' ',' ',' '},
	{'=','>',' ',' ','U','R','O',' '},
	{'=','>',' ',' ','G','L','U',' '},
	{'=','>',' ',' ','B','I','L',' '},
	{'=','>',' ',' ','K','E','T',' '},
	{'=','>',' ',' ','S','G',' ',' '},
	{'=','>',' ',' ','B','L','D',' '},
	{'=','>',' ',' ','p','H',' ',' '},
	{'=','>',' ',' ','P','R','O',' '},
	{'=','>',' ',' ','N','I','T',' '},
	{'=','>',' ',' ','L','E','U',' '},
	{'=','>',' ',' ','A','S','A',' '},
	{' ',' ',' ',' ','A','L','B',' '},	//C
	{' ',' ',' ',' ','C','R','E',' '},	//D
	{' ',' ',' ',' ','M','D','A',' '},	//E
	{' ',' ',' ',' ','B','H','B',' '},	//F
	{' ',' ',' ',' ','C','A','L',' '},	//G
	{' ',' ',' ',' ','L','D','H',' '},	//H
	{' ',' ',' ',' ','A','/','C',' '},	//I
	{' ',' ',' ',' ','P','/','C',' '},	//J
	{' ',' ',' ',' ','C','/','C',' '} 	//K
};      

#else

extern char *PrintLine2;
extern const u8 item_string[21][8];
extern const char alb_item[5][5];
extern const char albumin_tb[5][32];
extern const char albumin_tb_1[5][13];

#endif


