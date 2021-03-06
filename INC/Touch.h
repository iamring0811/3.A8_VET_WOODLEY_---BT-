#define MAX_BACK	12000
#define KEY_POWER 0x01
#define KEY_START 0x02
#define KEY_TEST  0x04

#define KDLY 5
#define TDLY 5
#define MIN_TCHY	55

#define M_INIT		0x00	//초기화작업중
#define M_STANDBY	0x10	//검사대기
#define M_STRIP		0x20	//스트립변경
#define M_SEQNO		0x30	//SEQ번호 설정
#define M_IDNO		0x40	//ID번호 설정
#define M_COLOR		0x50	//SEQ번호 설정
#define M_TURBI		0x60	//SEQ번호 설정
#define M_MEASURE	0x70	//검사대기
#define M_MENU		0x80	//결과/시스템관리
#define M_M_DATE	0x81	//날자,시간 설정
#define M_M_CHK		0x82	//시스템체크
#define M_M_MEM		0x83	//메모리관리 84:view,85:send,86:delete
#define M_M_SET		0x84	//시스템세팅 88:format,89:sysset,8A:change
#define M_M_INFO	0x85	//시스템정보
#define M_CLEAN	 	0x90	//Load Out
#define M_CLEND	 	0xA90	//Load In

#define M_M_VIEWNO	0xA0	//
#define M_M_VIEW	0xA1	//
#define M_M_NODATA	0xA2	//
#define M_M_SENDNO	0xA4	//
#define M_M_DELNO	0xA8	//

#define M_M_FORMAT	0xB0	//
#define M_M_SYSSET	0xB4	//
#define M_M_CHANGE	0xB8	//
#define M_M_C_SET	0xB9	//
#define M_M_C_DGT	0xBA	//


#define M_VIEW	 	0x3f	//검사결과표시
#define M_RESULT	0x90	//결과관리
#define M_R_VIEW	0x91	//결과보기
#define M_R_VIEW2	0x99	//결과보기2
#define M_R_SEND	0x92	//결과전송
#define M_R_PRINT	0x93	//결과출력
#define M_R_DEL		0x94	//결과삭제
#define M_SYSTEM	0xA0	//시스템관리
#define M_S_CHECK	0xA1	//장비점검
#define M_S_CHECK2	0xA2	//장비점검
#define M_S_ADJ		0xA3	//장비설정
#define M_S_ADJ2	0xA4	//장비설정
#define M_OFF		0xF0	//전원끄짐
#define M_M_PASS	0xA5	//장비설정

#ifdef _TOUCH_C_

u16 key_code, keypush, keypull;
u16 tchpush, tchpull;
u16 tchx, tchy;
u8  state, cursor, blink_dur;
u16 xs_cursor, xe_cursor, ys_cursor, ye_cursor;
u16 tmp_seqno, end_seqno;
u8  tmp_idno[MAX_USERID+2];
u8  cursor_buff[2048];
u16 view_seqno;
u16	mem_srt, mem_end;
u16 flag_clean;
u16	pr_no, pr_sub, pr_dgt, pr_ref;
u16 sel_strip;

void key_scan(void);
void key_decode(void);
void key_clear(void);
void tsw_poff(void);
void tsw_start(void);
void key_test(void);
void power_on(void);
void ser_flush(void);
char ser_getchar(void);
char ser_getchar_nowait(void);
void key_error(void);

u16 tch_check(u16 *tb, u8 tno);
void tch_standby(void);
void tsw_menu(void);
void tsw_seqno(void);
void tsw_idno(void);
void tsw_strip(void);

void tch_measure(void);
void tch_strip(void);
void tch_seqno(void);
void tch_menu(void);
void tch_date(void);
void tch_syschk(void);
void tch_memory(void);
void tch_viewno(void);
void tch_view(void);
void tch_nodata(void);
void tch_sendno(void);
void tch_delno(void);
void tch_setting(void);
void tch_format(void);
void tch_sysset(void);
void tch_change(void);
void tch_info(void);
void tch_pass(void);



void tch_idno(void);
void tch_system(void);
void print_sound(void);

#else

extern u16 key_code, keypush, keypull;
extern u16 tchpush, tchpull;
extern u16 tchx, tchy;
extern u8  state, cursor, blink_dur;
extern u16 xs_cursor, xe_cursor, ys_cursor, ye_cursor;
extern u16 tmp_seqno, end_seqno;
extern u8  tmp_idno[MAX_USERID+2];
extern u8  cursor_buff[2048];
extern u16 view_seqno;
extern u16	mem_srt, mem_end;
extern u16 flag_clean;
extern u16	pr_no, pr_sub, pr_dgt, pr_ref;
extern u16 sel_strip;

extern void key_scan(void);
extern void key_decode(void);
extern void key_clear(void);
extern void tsw_poff(void);
extern void tsw_start(void);
extern void key_test(void);
extern void power_on(void);
extern void ser_flush(void);
extern char ser_getchar(void);
extern char ser_getchar_nowait(void);
extern void key_error(void);

extern void tch_standby(void);
extern void tsw_menu(void);
extern void tsw_seqno(void);
extern void tsw_idno(void);
extern void tsw_strip(void);
extern void tch_measure(void);
extern void tch_view(void);
extern void tch_nodata(void);
extern void tch_strip(void);
extern void tch_date(void);
extern void tch_seqno(void);
extern void tch_idno(void);
extern void tch_menu(void);
extern void tch_system(void);
extern void tch_s_adj(void);
extern void print_sound(void);

#endif 

#ifdef _TOUCH_C_
/*   xs, xe, ys, ye */
#define tch_standby_no	9
const u16 tch_standby_tb[4*tch_standby_no]={
	185,245, 70,757,	//strip
	250,305, 70,757,	//seq
	310,376, 70,757,	//idno
	380,441, 70,757,	//color
	450,510, 70,757,	//turbi
	530,873, 70,380,	//start
	530,631,445,770,	//menu
	650,750,445,770,	//clean
	784,887,445,770		//poff
};
#define tch_strip_no	11
const u16 tch_strip_tb[4*tch_strip_no]={
	214,303,671,790,	//Up
	634,732,671,790,	//Dn
	226,285, 90,582,	//line1
	286,345, 90,582,	//line2
	346,405, 90,582,	//line3
	406,463, 90,582,	//line4
	464,523, 90,582,	//line5
	524,583, 90,582,	//line6
	584,641, 90,582,	//line7
	642,703, 90,582,	//line8
	792,887, 90,769 	//Test
};
// 223 - 702 = 53
//  93 - 498 
#define tch_color_no	10
const u16 tch_color_tb[4*tch_color_no]={
	223,276, 93,450,	//'0'
	277,329, 93,450,	//'1'
	330,383, 93,450,	//'2'
	384,437, 93,450,	//'3'
	438,490, 93,450,	//'4'
	491,543, 93,450,	//'5'
	544,596, 93,450,	//'6'
	597,650, 93,450,	//'7'
	651,703, 93,450,	//'8'
	792,887, 90,769 	//Test
};
#define tch_digit_no	13
const u16 tch_digit_tb[4*tch_digit_no]={
	627,707,326,500,	//'0'
	288,374,106,276,	//'1'
	288,374,326,500,	//'2'
	288,374,545,714,	//'3'
	400,484,106,276,	//'4'
	400,484,326,500,	//'5'
	400,484,545,714,	//'6'
	518,598,106,276,	//'7'
	518,598,326,500,	//'8'
	518,598,545,714,	//'9'
	627,707,106,276,	//Dn - L
	627,707,545,714,	//Up - R
	793,873, 90,730 	//Test
};
#define tch_digitv_no	14
const u16 tch_digitv_tb[4*tch_digitv_no]={
	627,707,326,500,	//'0'
	288,374,106,276,	//'1'
	288,374,326,500,	//'2'
	288,374,545,714,	//'3'
	400,484,106,276,	//'4'
	400,484,326,500,	//'5'
	400,484,545,714,	//'6'
	518,598,106,276,	//'7'
	518,598,326,500,	//'8'
	518,598,545,714,	//'9'
	627,707,106,276,	//Dn - L
	627,707,545,714,	//Up - R
	806,883, 70,390, 	//Menu
	806,883,435,778 	//Test
};
#define XD	52
#define tch_digit2_no	16
const u16 tch_digit2_tb[4*tch_digit2_no]={
	627+XD,707+XD,326,500,	//'0'
	288+XD,374+XD,106,276,	//'1'
	288+XD,374+XD,326,500,	//'2'
	288+XD,374+XD,545,714,	//'3'
	400+XD,484+XD,106,276,	//'4'
	400+XD,484+XD,326,500,	//'5'
	400+XD,484+XD,545,714,	//'6'
	518+XD,598+XD,106,276,	//'7'
	518+XD,598+XD,326,500,	//'8'
	518+XD,598+XD,545,714,	//'9'
	627+XD,707+XD,106,276,	//Dn - L
	627+XD,707+XD,545,714,	//Up - R
	186,241, 70,765,	//Date
	248,309, 70,765,	//Time
	806,883, 70,390, 	//Menu
	806,883,435,778 	//Test
};
#define tch_digit3_no	16
#define tch_digit3_tb	tch_digit2_tb

#define tch_measure_no	1
const u16 tch_measure_tb[4*tch_measure_no]={
	793,876, 90,750 	//Test
};
#define tch_menu_no	6
const u16 tch_menu_tb[4*tch_menu_no]={
	210,380,138,423,	//date/time
	210,380,482,784,	//sys check
	410,580,138,423,	//memory
	410,580,482,784,	//settings
	610,780,138,423,	//sys info
	793,876, 90,750 	//Test
};
#define tch_syschk_no	3
const u16 tch_syschk_tb[4*tch_syschk_no]={
	183,261, 90,737,	//start
	806,883, 70,390, 	//Menu
	806,883,435,778 	//Test
};
#define tch_memory_no	5
const u16 tch_memory_tb[4*tch_memory_no]={
	209,278, 90,735,	//view
	297,381, 90,735,	//send
	399,476, 90,735,	//delete
	806,883, 70,390, 	//Menu
	806,883,435,778 	//Test
};

#define tch_view_no	3
const u16 tch_view_tb[4*tch_view_no]={		//수정해야됨
	815,880, 65,282,	//memory
	815,880,318,526,	//send
	815,880,563,786 	//delete
};

#define tch_setting_no	tch_memory_no
#define tch_setting_tb  tch_memory_tb

#define tch_format_no	5
const u16 tch_format_tb[4*tch_format_no]={
	204,308,100,736,	//unit
	327,442,100,736,	//plus
	459,576,100,736,	//mark
	806,883, 70,390, 	//Setting
	806,883,435,778 	//Test
};

#define tch_info_no	2
const u16 tch_info_tb[4*tch_info_no]={
	806,883, 70,390, 	//Menu
	806,883,435,778 	//Test
};
// 786 - 207 = 579/6 = 96.5
// 750 -  87 = 663/3 = 221
#define tch_change_no	19
const u16 tch_change_tb[4*tch_change_no]={
	217,293, 97,298, 	//11
	217,293,318,519, 	//12
	217,293,539,739, 	//13
	314,389, 97,298, 	//21
	314,389,318,519, 	//22
	314,389,539,739, 	//23
	414,488, 97,298, 	//31
	414,488,318,519, 	//32
	414,488,539,739, 	//33
	510,585, 97,298, 	//41
	510,585,318,519, 	//42
	510,585,539,739, 	//43
	606,681, 97,298, 	//51
	606,681,318,519, 	//52
	606,681,539,739, 	//53
	700,780, 97,298, 	//61
	700,780,318,519, 	//62
            
	806,883, 70,390, 	//Setting
	806,883,435,778 	//Test
};

// 815-337=478/7=68.28
// 789-456
#define tch_prset_no	11
const u16 tch_prset_tb[4*tch_prset_no]={
	347,395,470,789, 	//1
	415,463,470,789, 	//2
	483,531,470,789, 	//3
	552,600,470,789, 	//4
	620,668,470,789, 	//5
	688,736,470,789, 	//6
	756,805,470,789, 	//7

	200,260, 60,175, 	//L
	200,260,658,773, 	//R
            
	806,883, 70,390, 	//Setting
	806,883,435,778 	//Test
};

#define tch_prdgt_no	3
const u16 tch_prdgt_tb[4*tch_prdgt_no]={
	660,715, 93,200, 	//dn
	660,715,627,736, 	//up
	765,851, 98,736 	//ok
};

#define tch_nodata_no tch_info_no
#define tch_nodata_tb tch_info_tb

/*   xs, xe, ys, ye */
//	x = 268 - 742 = 59.25
//	y =  66 - 773 = 141.4
#define tch_idset_no	41
const u16 tch_idset_tb[4*tch_idset_no]={
	328,385,653,793, 	//'0'
	268,327, 86,227, 	//'1'
	268,327,228,368, 	//'2'
	268,327,369,510, 	//'3'
	268,327,511,652, 	//'4'
	268,327,653,793, 	//'5'
	328,385, 86,227, 	//'6'
	328,385,228,368, 	//'7'
	328,385,369,510, 	//'8'
	328,385,511,652, 	//'9'
	386,445, 86,227, 	//'A'
	386,445,228,368, 	//'B'
	386,445,369,510, 	//'C'
	386,445,511,652, 	//'D'
	386,445,653,793, 	//'E'
	446,504, 86,227, 	//'F'
	446,504,228,368, 	//'G'
	446,504,369,510, 	//'H'
	446,504,511,652, 	//'I'
	446,504,653,793, 	//'J'
	506,564, 86,227, 	//'K'
	506,564,228,368, 	//'L'
	506,564,369,510, 	//'M'
	506,564,511,652, 	//'N'
	506,564,653,793, 	//'O'
	565,623, 86,227, 	//'P'
	565,623,228,368, 	//'Q'
	565,623,369,510, 	//'R'
	565,623,511,652, 	//'S'
	565,623,653,793, 	//'T'
	624,683, 86,227, 	//'U'
	624,683,228,368, 	//'V'
	624,683,369,510, 	//'W'
	624,683,511,652, 	//'X'
	624,683,653,793, 	//'Y'
	684,742, 86,227, 	//<==
	684,742,228,368, 	//'Z'
	684,742,369,510, 	//'.'
	684,742,511,652, 	//'-'
	684,742,653,793, 	//CLR
	792,887, 90,769 	//Test
};

#endif 
