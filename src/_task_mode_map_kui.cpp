#include "common.h"


#define		ALIAS_MM_KUI(ptr,str);	;{\
/*	unsigned __int8 *&str##Pkui   = ptr->Pv[0].PB ; */\
	__int32 &str##penX         = ptr->Pv[1].i ; \
	__int32 &str##penY         = ptr->Pv[2].i ; \
	__int32 &str##penXb        = ptr->Pv[3].i ; \
	__int32 &str##penYb        = ptr->Pv[4].i ; \
	__int32 &str##state        = ptr->Pv[5].i ; \
	__int32 &str##sel          = ptr->Pv[6].i ; \
	__int32 *&str##Pkuilog     = ptr->Pv[7].Pi ; \
	__int32 &str##reqsearch    = ptr->Pv[8].i ; \
	__int32 &str##arcargx      = ptr->Pv[10].i ; \
	__int32 &str##arcargy      = ptr->Pv[11].i ; \
	TCB *&str##Pkoho           = ptr->Pv[15].PTCB ; \
	;

#define		LOG_TIMES		3

static char kuipos[8][2] =
{
	{0,0},
	{1,0},
	{2,0},
	{3,0},
	{0,1},
	{0,2},
	{2,2},
	{0,3},
} ;

static char kuidata[256][4] =
{
	{4,4,99,99},
	{0,4,99,99},
	{4,0,99,99},
	{0,0,99,99},
	{5,4,99,99},
	{1,4,99,99},
	{5,0,99,99},
	{1,0,99,99},
	{2,4,99,99},
	{2,0,99,99},
	{3,4,99,99},
	{-1,4,99,99},
	{3,0,99,99},
	{-1,0,99,99},
	{4,5,99,99},
	{0,5,99,99},

	{4,1,99,99},
	{0,1,99,99},
	{4,2,99,99},
	{0,2,99,99},
	{4,3,99,99},
	{0,3,99,99},
	{4,-1,99,99},
	{0,-1,99,99},
	//複合型
	{4,4,0,-2},
	{0,4,4,-2},
	{4,0,0,6},
	{0,0,4,6},
	{4,4,-2,0},
	{0,4,6,0},
	{4,0,-2,4},
	{0,0,6,4},
	{4,4,-3, 0},
	{0,4, 7, 0},
	{4,0,-3, 4},
	{0,0, 7, 4},
	{5,4,-2,0},
	{-1,4,6,0},
	{5,0,-2,4},
	{-1,0,6,0},
	{4,4, 0,-3},
	{0,4, 4,-3},
	{4,0, 0, 7},
	{0,0, 4, 7},
	{4,5,0,-2},
	{0,5,4,-2},
	{4,-1,0,6},
	{0,-1,4,6},
	{4,4,-2,-2},
	{0,4, 6,-2},
	{4,0,-2, 6},
	{0,0, 6, 6},

	//壁付き
	{4,4,99,99},
	{0,4,99,99},
	{4,0,99,99},
	{0,0,99,99},
	{5,4,99,99},
	{1,4,99,99},
	{5,0,99,99},
	{1,0,99,99},
	{2,4,99,99},
	{2,0,99,99},
	{3,4,99,99},
	{-1,4,99,99},
	{3,0,99,99},
	{-1,0,99,99},
	{4,5,99,99},
	{0,5,99,99},

	{4,1,99,99},
	{0,1,99,99},
	{4,2,99,99},
	{0,2,99,99},
	{4,3,99,99},
	{0,3,99,99},
	{4,-1,99,99},
	{0,-1,99,99},
	//複合型
	{4,4,0,-2},
	{0,4,4,-2},
	{4,0,0,6},
	{0,0,4,6},
	{4,4,-2,0},
	{0,4,6,0},
	{4,0,-2,4},
	{0,0,6,4},
	{4,4,-3, 0},
	{0,4, 7, 0},
	{4,0,-3, 4},
	{0,0, 7, 4},
	{5,4,-2,0},
	{-1,4,6,0},
	{5,0,-2,4},
	{-1,0,6,0},
	{4,4, 0,-3},
	{0,4, 4,-3},
	{4,0, 0, 7},
	{0,0, 4, 7},
	{4,5,0,-2},
	{0,5,4,-2},
	{4,-1,0,6},
	{0,-1,4,6},
	{4,4,-2,-2},
	{0,4, 6,-2},
	{4,0,-2, 6},
	{0,0, 6, 6},

	//ど真ん中
	{2,2,99,99},
	//真っ黒
	{99,99,99,99},

	//黒、端っこだけ
	{-3,4,99,99},
	{-3,0,99,99},
	{6,4,99,99},
	{-2,4,99,99},
	{6,0,99,99},
	{-2,0,99,99},
	{7,4,99,99},
	{7,0,99,99},
	{4,-3,99,99},
	{0,-3,99,99},
	{4,6,99,99},
	{0,6,99,99},
	{4,-2,99,99},
	{0,-2,99,99},

	{4,7,99,99},
	{0,7,99,99},
	{6,6,99,99},
	{2,6,99,99},
	{-2,6,99,99},
	{6,2,99,99},
	{-2,2,99,99},
	{6,-2,99,99},
	{2,-2,99,99},
	{-2,-2,99,99},

	//白、端っこだけ
	{-3,4,99,99},
	{-3,0,99,99},
	{6,4,99,99},
	{-2,4,99,99},
	{6,0,99,99},
	{-2,0,99,99},
	{7,4,99,99},
	{7,0,99,99},
	{4,-3,99,99},
	{0,-3,99,99},
	{4,6,99,99},
	{0,6,99,99},
	{4,-2,99,99},
	{0,-2,99,99},

	{4,7,99,99},
	{0,7,99,99},
	{6,6,99,99},
	{2,6,99,99},
	{-2,6,99,99},
	{6,2,99,99},
	{-2,2,99,99},
	{6,-2,99,99},
	{2,-2,99,99},
	{-2,-2,99,99},




	//後はダミー
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//

	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
	{99,99,99,99},//
} ;

//α
static int ChainTable0[][2] =
{
	{ 8, 0,},
	{ 8, 1,},
	{ 8, 2,},
	{ 8, 3,},
	{ 8, 4,},
	{ 6, 4,},
	{ 6, 6,},
	{ 4, 6,},
	{ 4, 8,},
	{ 3, 8,},
	{ 2, 8,},
	{ 1, 8,},
	{ 0, 8,},

	{-8, 0,},
	{-8, 1,},
	{-8, 2,},
	{-8, 3,},
	{-8, 4,},
	{-6, 4,},
	{-6, 6,},
	{-4, 6,},
	{-4, 8,},
	{-3, 8,},
	{-2, 8,},
	{-1, 8,},
//	{-0, 8,},

//	{ 8,-0,},
	{ 8,-1,},
	{ 8,-2,},
	{ 8,-3,},
	{ 8,-4,},
	{ 6,-4,},
	{ 6,-6,},
	{ 4,-6,},
	{ 4,-8,},
	{ 3,-8,},
	{ 2,-8,},
	{ 1,-8,},
	{ 0,-8,},

//	{-8,-0,},
	{-8,-1,},
	{-8,-2,},
	{-8,-3,},
	{-8,-4,},
	{-6,-4,},
	{-6,-6,},
	{-4,-6,},
	{-4,-8,},
	{-3,-8,},
	{-2,-8,},
	{-1,-8,},
//	{-0,-8,},
	{0xFFFF},
};

//β　Ｘ方向に４の倍数
static int ChainTable1[][2] =
{
	{ 8, 0},
	{ 8, 1},
	{ 8, 2},
	{ 8, 3},
	{ 8, 4},
	{ 4, 6},
	{ 4, 7},
//	{ 8,-0},
	{ 8,-1},
	{ 8,-2},
	{ 8,-3},
	{ 8,-4},
	{ 4,-6},
	{ 4,-7},

	{-8, 0},
	{-8, 1},
	{-8, 2},
	{-8, 3},
	{-8, 4},
	{-4, 6},
	{-4, 7},
//	{-8,-0},
	{-8,-1},
	{-8,-2},
	{-8,-3},
	{-8,-4},
	{-4,-6},
	{-4,-7},
	{0xFFFF},
};

//γ　x=4m　y=4n+1
//左右対称だが上下対称ではない
static int ChainTable2[][2] =
{
	{ 4,-7},
	{ 8,-4},
	{ 8,-3},
	{ 8,-2},
	{ 8,-1},
	{ 8, 0},
	{ 8, 1},
	{ 8, 2},
	{ 8, 3},
	{ 8, 4},

	{-4,-7},
	{-8,-4},
	{-8,-3},
	{-8,-2},
	{-8,-1},
	{-8, 0},
	{-8, 1},
	{-8, 2},
	{-8, 3},
	{-8, 4},
	{0xFFFF},
};

//ε
static int ChainTable3[][2] =
{
	{ 6, 6},
	{-6, 6},
	{ 6,-6},
	{-6,-6},
	{0xFFFF},
};

static void GetChainTable( int x , int y , int (**PPout)[2] , bool *isYmirror , bool *isXYswap )
{
	RotateCorrect( &x , 4 ) ;
	RotateCorrect( &y , 4 ) ;
	if( x==0 && y==0 )
	{//α
		(*PPout) = ChainTable0 ;
		(*isYmirror) = false ;
		(*isXYswap) = false ;
		return ;
	}
	if( x==2 && y==2 )
	{//ε
		(*PPout) = ChainTable3 ;
		(*isYmirror) = false ;
		(*isXYswap) = false ;
		return ;
	}
	if( x==0 && y==2 )
	{//β
		(*PPout) = ChainTable1 ;
		(*isYmirror) = false ;
		(*isXYswap) = false ;
		return ;
	}
	if( x==2 && y==0 )
	{//β
		(*PPout) = ChainTable1 ;
		(*isYmirror) = false ;
		(*isXYswap) = true ;
		return ;
	}
	if( x==0 && y==1 )
	{//γ
		(*PPout) = ChainTable2 ;
		(*isYmirror) = false ;
		(*isXYswap) = false ;
		return ;
	}
	if( x==0 && y==3 )
	{//γ
		(*PPout) = ChainTable2 ;
		(*isYmirror) = true ;
		(*isXYswap) = false ;
		return ;
	}
	if( x==1 && y==0 )
	{//γ
		(*PPout) = ChainTable2 ;
		(*isYmirror) = false ;
		(*isXYswap) = true ;
		return ;
	}
	if( x==3 && y==0 )
	{//γ
		(*PPout) = ChainTable2 ;
		(*isYmirror) = true ;
		(*isXYswap) = true ;
		return ;
	}

	assert(0) ;
	(*PPout) = ChainTable0 ;
	(*isYmirror) = false ;
	(*isXYswap) = false ;
	return ;
}
static int GetXatChainTable( int *Pct , bool isYmirror , bool isXYswap )
{
	return Pct[ isXYswap]*(!(isYmirror& isXYswap)*2-1) ;
}
static int GetYatChainTable( int *Pct , bool isYmirror , bool isXYswap )
{
	return Pct[!isXYswap]*(!(isYmirror&!isXYswap)*2-1) ;
}

static void CrossToken( int x , int y , BMPD color )
{
	mdo.Cls( MDO3normal , SFC_BACK , x-1 , y-1-2 , 3 , 7 , color ) ;
	mdo.Cls( MDO3normal , SFC_BACK , x-1-2 , y-1 , 7 , 3 , color ) ;
}

static void KuiSearch( TCB *caller )
{
	ALIAS_MAIN(caller->PmotherTCB->PmotherTCB,main) ;
	ALIAS_MM(caller->PmotherTCB , mm ) ;
	ALIAS_MM_KUI(caller , mmkui ) ;
int x0,x1,y0,y1 ;
		x0 = mmkuiPkuilog[0]/8 -1 ;
		y0 = mmkuiPkuilog[1]/8 -1 ;
		x1 = x0+1+(mmkuiPkuilog[0]%8!=0) ;
		y1 = y0+1+(mmkuiPkuilog[1]%8!=0) ;
		for( int iy=y0 ; iy<=y1 ; iy++ )
		for( int ix=x0 ; ix<=x1 ; ix++ )
		{
//チップでループ
			BYTE koholist[256][4] ;
			int  kohopos[4] ;
			kohopos[0] = kohopos[1] = kohopos[2] = kohopos[3] = 0 ;

			for( int il=0 ; il<4 ; il++ )
			{//タイルでループ
				int nokui ;
				char kuilist[2][2] ;
				nokui = 0 ;
					/*
				for( int q=0 ; q<9 ; q++ )
				{//杭有効圏内の杭を探す
					static BYTE kuimask[9] =
					{
						0x40 , 0xF0 , 0xF0 ,
						0x4E , 0xFF , 0xFF ,
						0x4E , 0xFF , 0xFF ,
					} ;
					int kx,ky ;
					kx = ix*2+il%2+ ((q%3)-1) ;
					ky = iy*2+il/2+ ((q/3)-1) ;
					if( kx<0 || ky<0 )continue ;
					int tk ;
					tk = mmkuiPkui[ ky*(FZCD_TILE_P_MAP_W+1)+kx ] ;
					tk &= kuimask[q] ;

					switch( tk )
					{
					case 0x01 :tk = 0 ;break ;
					case 0x02 :tk = 1 ;break ;
					case 0x04 :tk = 2 ;break ;
					case 0x08 :tk = 3 ;break ;
					case 0x10 :tk = 4 ;break ;
					case 0x20 :tk = 5 ;break ;
					case 0x40 :tk = 6 ;break ;
					case 0x80 :tk = 7 ;break ;
					default :
						tk = -1 ;
					}
					if( tk==-1 )continue ;
					kx = kuipos[tk][0]+((q%3)-1)*4 ;
					ky = kuipos[tk][1]+((q/3)-1)*4 ;

					kuilist[nokui][0] = kx ;
					kuilist[nokui][1] = ky ;
					nokui ++ ;
					if( nokui==2 )break ;
				}
*/

				
				{
					int kx,ky ;
					kx = ix*8+il%2*4+2 ;
					ky = iy*8+il/2*4+2 ;

					for( int it=0 ; it<LOG_TIMES ; it++ )
					{
						if( mmkuiPkuilog[it*2+0] == -1 )continue ;

						int tx,ty ;
						tx = mmkuiPkuilog[it*2+0] - kx ;
						ty = mmkuiPkuilog[it*2+1] - ky ;
						if( tx>-6 && ty>-6 && tx<6 && ty<6 )
						{
							kuilist[nokui][0] = tx+2 ;
							kuilist[nokui][1] = ty+2 ;
							nokui ++ ;
							if( nokui==2 )
								break ;
						}
					}
					
				}
				
				if( !nokui ) continue ;
				if( nokui==1 )kuilist[1][0] = kuilist[1][1] = 99 ;

				for( int k=0 ; k<256 ; k++ )
				{
					if( 
						(
							kuidata[k][0] == kuilist[0][0] &&
							kuidata[k][1] == kuilist[0][1] &&
							kuidata[k][2] == kuilist[1][0] &&
							kuidata[k][3] == kuilist[1][1]
						)
						||
						(
							kuidata[k][0] == kuilist[1][0] &&
							kuidata[k][1] == kuilist[1][1] &&
							kuidata[k][2] == kuilist[0][0] &&
							kuidata[k][3] == kuilist[0][1]
						)
					)
					{
						koholist[kohopos[il]][il] = k ;
						kohopos[il]++ ;
					}
				}
			}
			if( !kohopos[0] && !kohopos[1] && !kohopos[2] && !kohopos[3] )continue ;

			for( int ia=0xCC380 ; ia<=0xCFFFC ; ia++ )
			{
#if _DEBUG
//				if( ia-0xCC380==0xBD1 )
//					ia = ia ;
#endif
                int il=0 ;
				for( ; il<4 ; il++ )
				{
					int tmp ;
					int ik ;
					int table[4] = {0,2,1,3} ;
					tmp = GETROM8(ia+table[il]) ;
					if( kohopos[il] )
					{
						for( ik=0 ; ik<kohopos[il] ; ik++ )
						{
							if( koholist[ik][il] == tmp )
							{
								ik=-1 ;
								break ;
							}
						}
					}
					else
					{
						switch( tmp )
						{
						case 0x9A:
//						case 0x9C:
//						case 0xA0:
//						case 0xC0:
//						case 0xC1:
//						case 0xC2:
							ik = -1 ;
						break ;
						default:
							ik = 0 ;
						}
					}
					if( ik!=-1 )
					{
						il=-1 ;
						break ;
					}
				}
				if( il!=-1 )
				{
					BYTE *Ptmp ;
					Ptmp = &working.exptile[editingcn][(ix+iy*FZCD_CHIP_P_MAP_W)*FZCD_SOE_CHIP] ;
					int wv ;
					wv = ia-0xCC380 ;
					Ptmp[0] = wv ;
					Ptmp[1] = wv>>8 ;
					RequestRedraw() ;
					mmrequestculc = 1 ;
					break ;
				}
			}
		}

	ALIAS_END() ;
	ALIAS_END() ;
	ALIAS_END() ;
}

void TV_mm_kui_token( TCB *caller )
{
	ALIAS_MAIN(caller->PmotherTCB->PmotherTCB->PmotherTCB->PmotherTCB,main) ;
	ALIAS_MM(caller->PmotherTCB->PmotherTCB->PmotherTCB , mm ) ;
	ALIAS_MM_KUI(caller->PmotherTCB->PmotherTCB , mmkui ) ;

int tx,ty;
		tx = (mainx)*MAP_TILE_SIZE ;
		ty = (mainy)*MAP_TILE_SIZE ;
		tx += (int)(caller->x*TRUE_MULTI*2) ;
		ty += (int)(caller->y*TRUE_MULTI*2) ;
		CrossToken( tx , ty , myRGB(0,31,0) ) ;
	ALIAS_END() ;
	ALIAS_END() ;
	ALIAS_END() ;
}

void TD_mm_kui(TCB *caller)
{
	ALIAS_MM_KUI(caller , mmkui ) ;
//	free( mmkuiPkui ) ;
	free( mmkuiPkuilog ) ;
	ALIAS_END() ;
}
bool TF_mm_kui(TCB *caller)
{
	ALIAS_MAIN(caller->PmotherTCB->PmotherTCB,main) ;
	ALIAS_MM(caller->PmotherTCB , mm ) ;
	ALIAS_MM_KUI(caller , mmkui ) ;
	if( !caller->calltiming )
	{
//		mmkuiPkui = (unsigned __int8*) malloc( (FZCD_TILE_P_MAP_W+1)*(FZCD_TILE_P_MAP_H+1) ) ;
//		memset( mmkuiPkui , 0 , (FZCD_TILE_P_MAP_W+1)*(FZCD_TILE_P_MAP_H+1) ) ;
		mmkuiPkuilog = (__int32*)malloc( sizeof(__int32) * LOG_TIMES * 2 ) ;
		{
			for( int i=0 ; i<LOG_TIMES*2 ; i++ )mmkuiPkuilog[i] = -1 ;
		}
		mmkuiPkoho = TM.AddTask( caller , TP_DUMMY , NULL , NULL , NULL ) ;
		if( !mmkuiPkoho )return true ;
		mmkuipenX = -1 ;
		mmkuistate = 0 ;
		mmkuireqsearch = 0 ;
	}

	switch( mmkuistate )
	{
	case 0 :
		{
		int gmx,gmy ;
		gmx  = GetMousePosX() ;
		gmy  = GetMousePosY() ;
		gmx /= MAP_TILE_SIZE ;
		gmy /= MAP_TILE_SIZE ;
		gmx -= mainx ;
		gmy -= mainy ;

		int lmx,lmy ;
		lmx  = GetMousePosX() ;
		lmy  = GetMousePosY() ;
		lmx %= MAP_TILE_SIZE ;
		lmy %= MAP_TILE_SIZE ;
		lmx = (int)((double)lmx/TRUE_MULTI) ;
		lmy = (int)((double)lmy/TRUE_MULTI) ;
		lmx /= 2 ;
		lmy /= 2 ;

		if( gmx>=0 && gmx<FZCD_TILE_P_MAP_W &&
			gmy>=0 && gmy<FZCD_TILE_P_MAP_H )
		{
            int i=0 ;
			for( ; i<8 ; i++ )
			{
				if( lmx==kuipos[i][0] && lmy==kuipos[i][1] )
				{
					mmkuipenX = gmx*4+lmx ;
					mmkuipenY = gmy*4+lmy ;
					if( MousePush( MB_L ) )
					{
						mmkuistate = 1 ;
						mmkuiPkuilog[2*0+0] = mmkuipenX ;
						mmkuiPkuilog[2*0+1] = mmkuipenY ;
						mmkuireqsearch = 1 ;
						RequestRedraw() ;
						break ;
					}
					break ;
				}
			}
			if( i==8 )mmkuipenX = - 1;

			if( !MousePush( MB_L ) && MousePush( MB_R ) )
			{//同じ配置の別チップを探す
				BYTE *Ptmp ;
				Ptmp = &working.exptile[editingcn][(gmx/2+gmy/2*FZCD_CHIP_P_MAP_W)*FZCD_SOE_CHIP] ;
				int cmptile[4] ;
				for( int q=0 ; q<4 ; q++ )
					cmptile[q] = GETROM8((Ptmp[0]|(Ptmp[1]<<8))+q+0xCC380) ;
				int noloop = 0xCFFFC-0xCC380+1-1 ;
				int ia0 = Ptmp[0]|(Ptmp[1]<<8) ;
				for( int ia=ia0+1 ; ia<ia0+noloop ; ia++ )
				{
					int ex = ia % noloop ;
                    int it=0 ;
					for( ; it<4 ; it++ )
					{
						if( memcmp( kuidata[cmptile[it]] ,
							        kuidata[GETROM8(ex+it+0xCC380)] , 4 ) )break ;
					}
					if( it==4 )
					{
						if( KeyOn( KC_CTRL ) )
						{
                            int ill=0 ;
							for( ; ill<4 ; ill++ )
							{
								int tmp ;
								tmp = GETROM8(ex+0xCC380+ill) ;
								if( tmp < 0x9B )continue ;
								switch( tmp )
								{
								case 0xC2://色違い
								break ;
								case 0xA5://B壁
								case 0xC0://M路面
								case 0xC1://B路面
									if( KeyOn( KC_SHIFT ) )break ;
								case 0x9C://<<
								case 0xA0://>>
								default:
									ill = -1 ;
								}
								if( ill==-1 )break ;
							}
							if( ill==-1 )continue ;
						}
						Ptmp[0] = (BYTE)ex ;
						Ptmp[1] = ex>>8 ;
						RequestRedraw() ;
						break ;
					}
				}
				RequestRedraw() ;
			}
		}
		else
		{
			mmkuipenX = -1 ;
		}
#if 0
		{
			if( KeyPush( KC_1 ) )
			{
				int tx,ty ;
				tx  = GetMousePosX() ;
				ty  = GetMousePosY() ;
				tx  = (int)(tx*TRUE_MULTI) ;
				ty  = (int)(ty*TRUE_MULTI) ;
				tx -= mainx*MAP_TILE_SIZE_D*2 ;
				ty -= mainy*MAP_TILE_SIZE_D*2 ;
				mmkuiarcargx = tx ;
				mmkuiarcargy = ty ;
			}
			if( KeyRelease( KC_1 ) )
			{
				int tx,ty ;
				tx  = GetMousePosX() ;
				ty  = GetMousePosY() ;
				tx  = (int)(tx*TRUE_MULTI) ;
				ty  = (int)(ty*TRUE_MULTI) ;
				tx -= mainx*MAP_TILE_SIZE_D*2 ;
				ty -= mainy*MAP_TILE_SIZE_D*2 ;

				mmkuiarcargy = ty ;//ログの意味無し

				int rad ;
				rad = MYABS( mmkuiarcargx - tx ) ;
				if( rad < 32 )
				{
					MessageBox( hWnd , "半径をもう少し大きく取ってください" , "エラー" , MB_OK ) ;
				}
				else
				{
					int ttx,tty ;
					ttx = (mmkuiarcargx-rad)/8*8+4 ;
					tty = ty/MAP_TILE_SIZE_D/2*8+4 ;

					for( int i=0 ; i<LOG_TIMES*2 ; i++ )mmkuiPkuilog[i] = -1 ;

#define		MOVE_KUI(xv,yv)					;\
					if( (xv)>=0 && (xv)<FZCD_TILE_P_MAP_W*4 && \
					    (yv)>=0 && (yv)<FZCD_TILE_P_MAP_W*4 ) \
					{											\
						mmkuiPkuilog[2*0+0] = (xv) ;			\
						mmkuiPkuilog[2*0+1] = (yv) ;			\
						KuiSearch( caller ) ;					\
					}

					MOVE_KUI( ttx , tty ) ;

					int log[512] ;
					/*static */int movetable[][16] =
					{/*最初の２つは合計値　それ以降は移動値*/
						{0,8,0,8,-1},
						{4,32,1,8,1,8,1,8,1,8,-1},
						{4,24,1,8,1,8,2,8,-1},
						{4,16,2,8,2,8,-1},
						{4,8,4,8,-1},
						{8,12,4,6,4,6,-1},
						{12,12,6,6,6,6,-1}
					} ;
#define	SO_MT		(sizeof(movetable)/sizeof(int [16]))
					for( ;; )
					{
						double dist2_koho ;
						int    koho ;
						koho = -1 ;
						dist2_koho = 1000000000 ;
						for( int i=0 ; i<SO_MT ; i++ )
						{
							double distance2 ;
							distance2  = pow( (ttx+movetable[i][0])*2-mmkuiarcargx , 2 ) ;
							distance2 += pow( (tty+movetable[i][1])*2-mmkuiarcargy , 2 ) ;
							distance2 = MYABS( distance2 - rad*rad ) ;
							if( dist2_koho > distance2 )
							{
								koho = i ;
								dist2_koho = distance2 ;
							}
						}
						for( int q=2 ; ; q+=2 )
						{
							if( movetable[koho][q] == -1 )break ;
							ttx += movetable[koho][q  ] ;
							tty += movetable[koho][q+1] ;
							MOVE_KUI( ttx , tty ) ;
						}
						if( ttx>mmkuiarcargx )break ;

					}
//					mmkuiPkuilog[2*0+0] = ttx ;
//					mmkuiPkuilog[2*0+1] = tty ;
//					KuiSearch( caller ) ;


			//				memmove( mmkuiPkuilog+2 , mmkuiPkuilog+0 , sizeof(__int32)*(LOG_TIMES-1)*2 ) ;
#undef SO_MT
#undef KUI_MOVE

				}

			}
		}
#endif
		}
	break ;
	case 1:
		{
			mmkuistate = 2 ;
			TM.KillP( mmkuiPkoho , TP_HIGHEST , TP_LOWEST ) ;
			{
				int (*Pct)[2] ;
				bool isYmirror , isXYswap ;
				GetChainTable( mmkuipenX , mmkuipenY , &Pct , &isYmirror , &isXYswap ) ;
				for( int i=0 ; Pct[i][0] !=0xFFFF ; i++ )
				{
					TCB *Ptcb ;
					Ptcb = TM.AddTask( mmkuiPkoho , TP_DUMMY , NULL , NULL , TV_mm_kui_token ) ;
					if( !Ptcb )return true ;
					Ptcb->x = mmkuipenX+GetXatChainTable(Pct[i],isYmirror,isXYswap) ;
					Ptcb->y = mmkuipenY+GetYatChainTable(Pct[i],isYmirror,isXYswap) ;
				}
			}
			mmkuisel = 0 ;
			RequestRedraw() ;
		}
	//通過
	case 2:
		if( MousePush( MB_R ) )
		{
			mmkuistate = 0 ;
			TM.KillP( mmkuiPkoho , TP_HIGHEST , TP_LOWEST ) ;
			{
				for( int i=0 ; i<LOG_TIMES*2 ; i++ )mmkuiPkuilog[i] = -1 ;
			}
			RequestRedraw() ;
		}
		if( MousePush( MB_L ) )
		{
			int (*Pct)[2] ;
			bool isYmirror , isXYswap ;
			GetChainTable( mmkuipenX , mmkuipenY , &Pct , &isYmirror , &isXYswap ) ;

			int tx,ty ;
			int lx,ly ;
			lx = GetXatChainTable(Pct[mmkuisel],isYmirror,isXYswap) ;
			ly = GetYatChainTable(Pct[mmkuisel],isYmirror,isXYswap) ;
			tx = mmkuipenX + lx ;
			ty = mmkuipenY + ly ;

			for( int i=0 ; i<8 ; i++ )
			{
				if( (tx%4)==kuipos[i][0] && (ty%4)==kuipos[i][1] )
				{
//					mmkuiPkui[ (ty/4)*(FZCD_TILE_P_MAP_W+1)+(tx/4) ] ^= (1<<i) ;
					memmove( mmkuiPkuilog+2 , mmkuiPkuilog+0 , sizeof(__int32)*(LOG_TIMES-1)*2 ) ;
					mmkuiPkuilog[2*0+0] = tx ;
					mmkuiPkuilog[2*0+1] = ty ;
					break ;
				}
			}
			mmkuistate = 1 ;
			mmkuipenX = tx ;
			mmkuipenY = ty ;
			mmkuireqsearch = 1 ;
			RequestRedraw() ;
		}
		{
			int (*Pct)[2] ;
			bool isYmirror , isXYswap ;
			GetChainTable( mmkuipenX , mmkuipenY , &Pct , &isYmirror , &isXYswap ) ;
			double mangle ;
			{
int tx,ty;
				tx = (mainx)*MAP_TILE_SIZE ;
				ty = (mainy)*MAP_TILE_SIZE ;
				tx += (int)(mmkuipenX*TRUE_MULTI*2) ;
				ty += (int)(mmkuipenY*TRUE_MULTI*2) ;
				tx *= -1 ;
				ty *= -1 ;
				tx += GetMousePosX() ;
				ty += GetMousePosY() ;
				mangle = atan2( ty , tx ) ;
			}
			double minangle=2*PI ;
			int koho = 0 ;
			for( int i=0 ; Pct[i][0] !=0xFFFF ; i++ )
			{
				double angle ;
				angle = atan2( GetYatChainTable(Pct[i],isYmirror,isXYswap) ,
								GetXatChainTable(Pct[i],isYmirror,isXYswap) ) ;
				angle -= mangle ;
				AngleFixEven( &angle );
				if( MYABS(angle) < minangle )
				{
					koho = i ;
					minangle = MYABS(angle) ;
				}
			}
			if( koho!=mmkuisel )
			{
				mmkuisel = koho ;
				RequestRedraw() ;
			}
		}

	break ;
	}

	if( mmkuipenX != mmkuipenXb ||
		mmkuipenY != mmkuipenYb )
	{
		mmkuipenXb = mmkuipenX ;
		mmkuipenYb = mmkuipenY ;
		RequestRedraw() ;
	}


	if( mmkuireqsearch )
	{
		mmkuireqsearch = 0 ;
		RequestRedraw() ;
		KuiSearch( caller ) ;
	}
	ALIAS_END() ;
	ALIAS_END() ;
	ALIAS_END() ;
	return false ;	
}
void TV_mm_kui( TCB *caller )
{
	ALIAS_MAIN(caller->PmotherTCB->PmotherTCB,main) ;
	ALIAS_MM(caller->PmotherTCB , mm ) ;
	ALIAS_MM_KUI(caller , mmkui ) ;

	if( mmkuipenX != -1 )
	{
int tx,ty;
			tx = (mainx)*MAP_TILE_SIZE ;
			ty = (mainy)*MAP_TILE_SIZE ;
			tx += (int)(mmkuipenX*TRUE_MULTI*2) ;
			ty += (int)(mmkuipenY*TRUE_MULTI*2) ;
			BMPD ct[2] = {myRGB(0,31,0),myRGB(0,31,31)} ;
			CrossToken( tx , ty , ct[mmkuistate!=0] ) ;
	}
	if( mmkuistate==2 )
	{
		{
			int (*Pct)[2] ;
			bool isYmirror , isXYswap ;
			GetChainTable( mmkuipenX , mmkuipenY , &Pct , &isYmirror , &isXYswap ) ;
			int tx,ty ;
			tx = mmkuipenX+GetXatChainTable(Pct[mmkuisel],isYmirror,isXYswap) ;
			ty = mmkuipenY+GetYatChainTable(Pct[mmkuisel],isYmirror,isXYswap) ;
			tx = (int)(tx*TRUE_MULTI*2) ;
			ty = (int)(ty*TRUE_MULTI*2) ;
			tx += (mainx)*MAP_TILE_SIZE ;
			ty += (mainy)*MAP_TILE_SIZE ;
			CrossToken( tx , ty , myRGB(31,0,0) ) ;
		}
	}
	/*
	for( int iy=0 ; iy< FZCD_TILE_P_MAP_H    ; iy++ )
	for( int ix=0 ; ix<(FZCD_TILE_P_MAP_W+1) ; ix++ )
	for( int il=0 ; il<8 ; il++ )
	{
		if( mmkuiPkui[ iy*(FZCD_TILE_P_MAP_W+1)+ix ] & (1<<il) )
		{
int tx,ty;
			tx = (mainx+ix)*MAP_TILE_SIZE ;
			ty = (mainy+iy)*MAP_TILE_SIZE ;
			tx += (int)(kuipos[il][0]*TRUE_MULTI*2) ;
			ty += (int)(kuipos[il][1]*TRUE_MULTI*2) ;
			tx-- ;
			ty-- ;
			mdo.Cls( MDO3normal , SFC_BACK , tx , ty , 3 , 3 , myRGB(31,0,31) ) ;
		}
	}
	*/
	{
		for( int i=0 ; i<LOG_TIMES ; i++ )
		{
			if( mmkuiPkuilog[i*2] == -1 )continue ;
int tx,ty;
			tx = (mainx)*MAP_TILE_SIZE ;
			ty = (mainy)*MAP_TILE_SIZE ;
			tx += (int)(mmkuiPkuilog[i*2+0]*TRUE_MULTI*2) ;
			ty += (int)(mmkuiPkuilog[i*2+1]*TRUE_MULTI*2) ;
			CrossToken( tx , ty , myRGB(31,31,0) ) ;

		}
	}
	ALIAS_END() ;
	ALIAS_END() ;
	ALIAS_END() ;
	
}
