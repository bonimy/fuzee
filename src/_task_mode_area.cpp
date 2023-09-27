#include "common.h"
#include "_task_mode_area.h"


namespace ModeArea
{
	int selected = -1 ;
} ;

using namespace ModeArea ;

static int submode = 0 ;

static int dragging = 0 ;
static int show_area_scope = 0 ;
#define NO_SUBMODE 2
#define MODE_TITLE			"エリア編集"
#define SUBMODE_TITLE		"配置","パラメータ設定",


#define		ALIAS_ME(ptr,str);	;{\
	;

static void CWT(TCB *caller)
{
const char *Pstr[NO_SUBMODE]=
{
	SUBMODE_TITLE
};
char buf[128] ;
const char *Ptmp ;
	CWT_common( buf ) ;
	sprintf( &buf[strlen(buf)] , MODE_TITLE ) ;
	for( int i=0 ; i<NO_SUBMODE ; i++ )
	{
		Ptmp = "" ;
		if( submode == i )Ptmp = Pstr[i] ;
		sprintf( &buf[strlen(buf)] , "【 %s 】" , Ptmp ) ;
	}
	sprintf( &buf[strlen(buf)] , "<spaceで切り替え" ) ;

	ALIAS_ME( caller , me ) ;
	ALIAS_END() ;
	SetWindowText( hWnd , buf ) ;
}


static bool TF_me_put(TCB *caller)
{
	ALIAS_ME( caller , mm ) ;
	ALIAS_MAIN(caller->PmotherTCB,main) ;


int mx,my ;
double dx,dy ;
	dx = GetMousePosX() ;
	dx /= TRUE_MULTI ;
	mx = (int)dx ;
	dy = GetMousePosY() ;
	dy /= TRUE_MULTI ;
	my = (int)dy ;
	mx -= mainx*MAP_TILE_SIZE_D ;
	my -= mainy*MAP_TILE_SIZE_D ;
	
/*
	{
		char buf[64] ;
		sprintf( buf , "%d,%d" , mx,my ) ;
		SetWindowText( hWnd , buf ) ;
	}
*/
	if( selected != -1 )
	{
		if( KeyPush( KC_DEL ) )
		{
			working.AreaDeleteArea( editingcn , selected ) ;
			if( !working.AreaGetSelectedArea( editingcn , selected )->isexist )
				selected = -1 ;
			RequestRedraw() ;
			return false ;
		}
		if( KeyPush( KC_INS ) )
		{
			if( !working.AreaInsertArea( editingcn , selected ) )
			{
				SetWindowText( hWnd , "エリアはもう配置できません" ) ;
				return false ;
			}
			RequestRedraw() ;
		}
		if( KeyPush( KC_B ) && selected<0x1000)
		{
			working.area_sub_org = selected ;
			RequestRedraw() ;
			return false ;
		}
	}

	if( dragging )
	{
		if( selected == -1 || ( MouseOff(MB_L)&&MouseOff(MB_R) ) )
		{
			dragging = 0 ;
			RequestRedraw() ;
			return false ;
		}
		switch( dragging )
		{
		case 1:
			{
				AREA *Pdest = working.AreaGetSelectedArea( editingcn , selected ) ;
				{
					int tmp ;
					tmp = mx-Pdest->x ;
					if( selected )
						tmp = tmp/8*8 ;
					else
						tmp = tmp/0x100*0x100 ;
					if( tmp )
					{
						Pdest->x += tmp ;
						if( selected )
							Pdest->x = Pdest->x/8*8 ;
						else
							Pdest->x = Pdest->x/0x100*0x100+0x80 ;
						RequestRedraw() ;
					}
				}
				{
					int tmp ;
					tmp = my-Pdest->y ;
//					if( selected )
						tmp = tmp/8*8 ;
//					else
//						tmp = tmp/0x100*0x100 ;
					if( tmp )
					{
						Pdest->y += tmp ;
//						if( selected )
							Pdest->y = Pdest->y/8*8 ;
//						else
//							working.area[editingcn][selected].y = working.area[editingcn][selected].y/0x100*0x100+0x80 ;
						RequestRedraw() ;
					}
				}
			}
			break ;
		case 2:
			{
				AREA *Pdest = working.AreaGetSelectedArea( editingcn , selected ) ;
				{
					double angle ;
					angle = atan2( my-Pdest->y , 
									mx-Pdest->x ) ;
					angle += (PI/8) ;
					AngleFixPlus( &angle ) ;
					int tmp ;
					tmp =(int)(angle/(PI/4)) ;
					tmp = tmp-Pdest->dir ;
					if( tmp )
					{
						Pdest->dir += tmp ;
						RequestRedraw() ;
					}
				}


			}
			break ;
		default:
			assert(0) ;
		}
	}
	else
	{
		if( MousePush( MB_L ) && KeyOn( KC_CTRL ) )
		{
			int index ;
			AREA *Pdest = working.AreaAddArea( editingcn , KeyOn( KC_SHIFT ) , &index ) ;
			if( !Pdest )
			{
				SetWindowText( hWnd , "エリアはもう配置できません" ) ;
				return false ;
			}
			Pdest->isexist = true ;
			Pdest->x = mx ;
			Pdest->y = my ;
			bool isstartline = false ;
			if( index==0 )isstartline = true ;

			if( isstartline )
			{
				Pdest->x = (Pdest->x)/0x100*0x100+0x80 ;
//				working.area[editingcn][i].y = (working.area[editingcn][i].y)/0x100*0x100+0x80 ;
				Pdest->y = (Pdest->y+4)/8*8 ;
			}
			else
			{
				Pdest->x = (Pdest->x+4)/8*8 ;
				Pdest->y = (Pdest->y+4)/8*8 ;
			}
			if( !isstartline )
			{
				double angle ;
				angle = atan2( Pdest->y-(Pdest-1)->y , 
								Pdest->x-(Pdest-1)->x ) ;
				angle += (PI/8) ;
				AngleFixPlus( &angle ) ;
				Pdest->dir =(int)(angle/(PI/4)) ;
			}
			selected = index ;
			dragging = 1 ;
			Pdest->arg[0] = 0x00 ;
			Pdest->arg[1] = 0x32 ;
			Pdest->arg[2] = 0x22 ;
			Pdest->arg[3] = 0x11 ;
			RequestRedraw() ;
		}
		else if( MousePush( MB_L ) || MousePush( MB_R ) )
		{
			if( SelectArea( mainx , mainy ) )
			{
				dragging = 1 ;
				if( MousePush( MB_R ) )
					dragging = 2 ;
				RequestRedraw() ;
				return false ;
			}
			selected = -1 ;
			RequestRedraw() ;
		}
	}
/*
	if( KeyPush( KC_ENTER ) )
	{
		for( int i=0 ; i<FZCD_MAX_AREA ; i++ )
		{
			if( working.area[editingcn][i].isexist )
			{
				if( working.area[editingcn][i].x % 8 )
					MessageBox( hWnd , Int2CharEX( i ) , "hosei" , MB_OK ) ;
				if( working.area[editingcn][i].y % 8 )
					MessageBox( hWnd , Int2CharEX( i ) , "hosei" , MB_OK ) ;
				working.area[editingcn][i].x = working.area[editingcn][i].x/8*8 ;
				working.area[editingcn][i].y = working.area[editingcn][i].y/8*8 ;
			}
		}

	}
*/
	ALIAS_END() ;
	ALIAS_END() ;
	return false ;	
}

bool ModeArea :: SelectArea( int scx , int scy )
{
int mx,my ;
double dx,dy ;
	dx = GetMousePosX() ;
	dx /= TRUE_MULTI ;
	mx = (int)dx ;
	dy = GetMousePosY() ;
	dy /= TRUE_MULTI ;
	my = (int)dy ;
	mx -= scx*MAP_TILE_SIZE_D ;
	my -= scy*MAP_TILE_SIZE_D ;
	for( int i=0 ; i<FZCD_MAX_AREA ; i++ )
	{
		if( working.area[editingcn][i].isexist )
		{
			if( MYABS(MYABS(working.area[editingcn][i].x-mx)+MYABS(working.area[editingcn][i].y-my)) < 20 )
			{
				selected = i ;
				return true ;
			}
		}
		if( working.area_sub[editingcn][i].isexist )
		{
			if( MYABS(MYABS(working.area_sub[editingcn][i].x-mx)+MYABS(working.area_sub[editingcn][i].y-my)) < 20 )
			{
				selected = i+0x1000 ;
				return true ;
			}
		}
	}
	return false ;
}
void ModeArea :: DrawMachineToken( int x , int y , int type )
{
	static BMPD table[3] = { myRGB(4,16,31) , myRGB(24,8,31) , myRGB(0,31,0) , } ;
	BMPD color ;
	color = table[ type%3 ] ;
	mdo.Cls( MDO3normal , SFC_BACK , x-2 , y-2 , 5 , 5 , (color>>1)&0x3DEF ) ;
	mdo.Cls( MDO3normal , SFC_BACK , x-1 , y-1 , 3 , 3 , color ) ;
}

void ModeArea :: DrawArea( int scx , int scy )
{
int bx,by ;
BMPD color ;
	for( int sub=0 ; sub<2 ; sub++ )
	for( int i=0 ; i<FZCD_MAX_AREA ; i++ )
	{
		AREA (*Pdestarea)[3][FZCD_MAX_AREA] ;
		Pdestarea=&working.area ;
		if( sub ) Pdestarea = &working.area_sub ;

		AREA *Pdest ;
		Pdest = &(*Pdestarea)[editingcn][i] ;
		if( !Pdest->isexist )continue ;
		int tx,ty ;
		tx = (int)(Pdest->x*TRUE_MULTI)+scx*MAP_TILE_SIZE ;
		ty = (int)(Pdest->y*TRUE_MULTI)+scy*MAP_TILE_SIZE ;


		color = myRGB(31,0,31) ;
		if( sub ) color = myRGB(0,31,31) ;
		if( selected == i+sub*0x1000 )color = myRGB(31,31,0) ;

		if( i!=0 || sub )
		{
			{
				double angle ;
				angle = Pdest->dir*PI/4 ;
				double x0,x1,y0,y1,x2,y2 ;
				double length = 16*4*TRUE_MULTI ;
				x0 = tx+cos(angle+PI/2)*length ;
				y0 = ty+sin(angle+PI/2)*length ;
				x1 = tx+cos(angle-PI/2)*length ;
				y1 = ty+sin(angle-PI/2)*length ;
				x2 = tx+cos(angle     )*length ;
				y2 = ty+sin(angle     )*length ;
				mdo.Line( MDO3normal , SFC_BACK , x0 , y0 , x1 , y1 , color ) ;
				mdo.Line( MDO3normal , SFC_BACK , tx , ty , x2 , y2 , color ) ;
				if( !sub||i )
				mdo.Line( MDO3normal , SFC_BACK , bx , by , tx , ty , myRGB(31,0,0) ) ;

				if( sub )
				{
					AREA *Pdestm = working.AreaGetSelectedArea(editingcn,i+working.area_sub_org+1) ;
					if( Pdestm->isexist )
					{
						int mtx,mty ;
						mtx = (int)(Pdestm->x*TRUE_MULTI)+scx*MAP_TILE_SIZE ;
						mty = (int)(Pdestm->y*TRUE_MULTI)+scy*MAP_TILE_SIZE ;
						mdo.Line( MDO3normal , SFC_BACK , mtx , mty , tx , ty , myRGB(0,0,31) ) ;
					}
				}

				{
					static int table[4] = { 32 , 16 , 48 , 64 } ;
					for( int q=2 ; q>=0 ; q-- )
					{
						int  tmp = Pdest->arg[1+q] ;
						length = table[(tmp>>2)&3]*TRUE_MULTI ;
						if( !(tmp&0xC0) )length = 0 ;
						else if( tmp&0x80 )length = -length ;
						else length = length ;
						x0 = tx+cos(angle+PI/2)*length ;
						y0 = ty+sin(angle+PI/2)*length ;
						DrawMachineToken( (int)x0 , (int)y0 , q ) ;
					}
				}
			}

		}
		else
		{
			int tty ;
			tty = (int)((Pdest->y/0x100*0x100)*TRUE_MULTI)+scy*MAP_TILE_SIZE ;
			mdo.Cls( MDO3normal , SFC_BACK , tx-1 , tty , 3 , (int)(0x200*TRUE_MULTI) , myRGB(31,0,0) ) ;

			mdo.Cls( MDO3normal , SFC_BACK , tx-3 , ty-3 , 7 , 7 , color ) ;
		}
		bx = tx ;
		by = ty ;
	}
}

static void TV_me_put( TCB *caller )
{
	ALIAS_ME( caller , mm ) ;
	ALIAS_MAIN(caller->PmotherTCB,main) ;

	DrawArea( mainx , mainy ) ;

{
	if( show_area_scope && selected > 0 )
	{
		AREA *Pdest = working.AreaGetSelectedArea( editingcn , selected ) ;
		MDO3Opt topt = *MDO3normal ;
		topt.flag |= MDO3F_BLEND ;
		topt.alpha = 0xA0 ;
		int tx,ty ;
		tx = (int)(Pdest->x*TRUE_MULTI)+mainx*MAP_TILE_SIZE ;
		ty = (int)(Pdest->y*TRUE_MULTI)+mainy*MAP_TILE_SIZE ;


		BMPD color ;
		int  tdir ;
		color = myRGB(14,31,14) ;
		tdir = Pdest->dir ;
		if( show_area_scope == 2 )
		{
			color = myRGB(31,22,0) ;
			tdir = (Pdest->dir+4)%8 ;
		}
		switch( tdir )
		{
		case 0://右
			mdo.Cls( &topt , SFC_BACK , tx , 0 , 0x40000 , WINHEIGHT , color ) ;
		break ;
		case 2://下
			mdo.Cls( &topt , SFC_BACK , 0 , ty , WINWIDTH , 0x40000 , color ) ;
		break ;
		case 4://左
			mdo.Cls( &topt , SFC_BACK , 0, 0 , tx+1 , WINHEIGHT , color ) ;
		break ;
		case 6://上
			mdo.Cls( &topt , SFC_BACK , 0 , 0 , WINWIDTH , ty+1 , color ) ;
		break ;
		default:
			{
				// y-ty = a * (x-tx)
				// y = a * (x-tx) + ty
				// x = a * (y-ty) + tx
				int a ;
				int yGTx ;
				switch( tdir )
				{
				case 1://右下
					a = -1 ;
					yGTx  = 0 ;
				break ;
				case 3://左下
					a = 1 ;
					yGTx  = 1 ;
				break ;
				case 5://左上
					a = -1 ;
					yGTx  = 1 ;
				break ;
				case 7://右上
					a = 1 ;
					yGTx  = 0 ;
				break ;
				}
				for( int iy=0 ; iy<WINHEIGHT ; iy++ )
				{
					int ttx ;
					ttx = a * (iy-ty)+tx ;
					if( yGTx )
					{
						mdo.Cls( &topt , SFC_BACK , 0 , iy , ttx+1 , 1 , color ) ;
					}
					else
					{
						mdo.Cls( &topt , SFC_BACK , ttx , iy , 0x40000 , 1 , color ) ;
					}
				}
			}
		}
//		KAGEMOJI( SFC_BACK , "Tab：エリア範囲表示切り替え" , 0 , WINHEIGHT-14*2 , 14 , 7 ) ;
	}
}

{
	char str[512] = "" ;
	if( selected!=-1 )
	{
		sprintf( &str[strlen(str)] , "Ins:エリア挿入　Del:エリア削除　Tab:エリア範囲表示　" ) ;
		if( selected<0x1000 )
			sprintf( &str[strlen(str)] , "B:分岐開始エリアに設定" ) ;
	}
	KAGEMOJI( SFC_BACK , str , 0 , WINHEIGHT-14*1 , 14 , 7 ) ;
	str[0] = '\0' ;
	sprintf( &str[strlen(str)] , "Ctrl+Click:エリア配置　　Ctrl+Shift+Click:分岐エリア配置" ) ;
	KAGEMOJI( SFC_BACK , str , 0 , WINHEIGHT-14*2 , 14 , 7 ) ;
}
	ALIAS_END() ;
	ALIAS_END() ;
	
}
bool TF_mode_area(TCB *caller)
{
	ALIAS_ME( caller , mm ) ;
bool init = false ;
	if( !caller->calltiming )
	{
		CWT( caller ) ;
		init = true ;
	}

	if( KeyPush( KC_SPACE ) )
	{
		submode ++ ;
		submode %= NO_SUBMODE ;
		CWT(caller) ;
		RequestRedraw() ;
		dragging = 0 ;
		init = true ;

	}

	if( init )
	{
		TM.KillP( caller , TP_HIGHEST , TP_LOWEST ) ;
		switch( submode )
		{
		case 0:
		break ;
		case 1:
			TM.AddTask( caller , TP_MA_PARAM , TF_mode_area_param ) ;
		break ;
		}
	}

	if( !caller->calltiming )return false ;

	//酷いコーディングですが……
	if( KeyPush(KC_F5) )CWT(caller) ;
	if( KeyPush(KC_F6) )CWT(caller) ;


	if( KeyPush( KC_TAB ) )
	{
		show_area_scope ++ ;
		show_area_scope %= 3 ;
		RequestRedraw() ;
	}

	
	caller->Pdraw = NULL ;
	switch( submode )
	{
	case 0:
		TF_me_put( caller ) ;
		caller->Pdraw = TV_me_put ;
	break ;
	case 1:
		caller->Pdraw = NULL ;
	break ;
	default:
		assert( 0 ) ;
	}
	ALIAS_END() ;
	return false ;
}
