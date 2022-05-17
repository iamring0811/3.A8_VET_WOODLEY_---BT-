#ifdef _PARA_H_

u8 ini_weight[20];
u8 ini_strip[]; 			//264=24*11
u8 ini_curve[];
u8 ini_para[];
u8 *ar_item_new[21][8];
u8 *co_item_new[21][8];
u8 *si_item_new[21][8];
u8 ar_item_ram[21][8][8];
u8 co_item_ram[21][8][8];
u8 si_item_ram[21][8][8];

#else

extern u8 ini_weight[20];
extern u8 ini_strip[]; 			//264=24*11
extern u8 ini_curve[];
extern u8 ini_para[];
extern u8 *ar_item_new[21][8];
extern u8 *co_item_new[21][8];
extern u8 *si_item_new[21][8];
extern u8 ar_item_ram[21][8][8];
extern u8 co_item_ram[21][8][8];
extern u8 si_item_ram[21][8][8];

#endif

