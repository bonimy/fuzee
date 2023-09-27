#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifdef _ORE_SENYOU
//私はいくつかのソースをライブラリ化しているので……
#ifdef _DEBUG
#pragma comment(lib,"gl_d.lib")
#else
#pragma comment(lib,"gl.lib")
#endif
#endif

#include "nnsys.h"
#include "mydibobj3.h"
#include "task0702.h"
#include "taskfunction.h"
#include "alias.h"
#include "myeinput.h"
#include "keycode.h"
#include "mymemorymanage.h"
#include "fzcd.h"

#ifdef _ORE_SENYOU
#include "SFCEMU/_65816m.h"
#include "HT/SNESsys.h"
#else
#include "_65816m.h"
#include "SNESsys.h"
#endif
//アプリケーションのウインドウへのドロップを許可するときはコメントアウトを外す
#define	MY_DROP_ACCEPT
//２重起動を（簡易）抑制するときはコメントアウトを外す
#define	ENABLE_MULTIPLE_RUN

#define		COMMON_CONST_WINDOW_CLASS_NAME		"FZE_WCN"
#define		COMMON_CONST_DEFAULT_WINDOW_TEXT	"ＦＺＥ"

#define		WINWIDTH				780
#define		WINHEIGHT				600

#define		MAXSFC				10
#define		MAXTASK				200

#define		SFC_BACK			0
#define		SFC_BG				1
#define		SFC_MINIMAP			2


#define		BASE_ROM_NAME		"fzero.smc"
#define		DEFAULT_ROM_SIZE	(1024*512)
#define		EHASH_OF_AUTHOR		(0x7b35aa)

#ifdef _DEBUG

//#define		MOSAIC_ENABLED

#endif

#define		KAGEMOJI(sfc,str,x,y,h,w)										;\
	{																		\
		mdo.Text( sfc , str , (x)+1 , (y)+1 , h , w , RGB(0,0,0) ) ;		\
		mdo.Text( sfc , str , (x)   , (y)   , h , w , RGB(255,255,255) ) ;	\
	}																		/**/


#define		ADR_TILE_DEFINITION			0x0CC380


extern HWND hWnd;
extern HINSTANCE hInstance;





extern int AppPrepare(HWND hwnd , HINSTANCE hinstance);
extern int AppRelease();
extern void MainRoutine(int fps);

extern void RequestRedraw();
extern void RequestFlip();





extern MyDIBObj3 mdo;
extern Task0702Manager TM;

extern BYTE *Prom ;
extern int  romsize ;

#define	ROMADR2OFFSET(adr)	((unsigned int)((((adr)>>16)*0x8000)|((adr)&0x7FFF))%romsize)
#define	ROMADR2OFFSET2(adr)	((unsigned int)((((adr)>>16)*0x8000)|((adr)&0x7FFF))%(romsize*2))

#define ROM(adr)			(Prom[ROMADR2OFFSET(adr)])
#define GET_ADR_BY_ROM(adr)	(ROM(adr)|(ROM(adr+1)<<8)|(ROM(adr+2)<<16)|(ROM(adr+3)<<24))

//8はなんとなく……
#define SETROM8(adr,val)	{ROM(adr)=(BYTE)(val);}
#define SETROM16(adr,val)	{SETROM8(adr,val);SETROM8((adr)+1,(val)>>8);}
#define SETROM24(adr,val)	{SETROM16(adr,val);SETROM8((adr)+2,(val)>>16);}
#define SETROM32(adr,val)	{SETROM16(adr,val);SETROM16((adr)+2,(val)>>16);}

#define GETROM8(adr)		(ROM(adr))
#define GETROM16(adr)		(ROM(adr)|(ROM((adr)+1)<<8))
#define GETROM24(adr)		(ROM(adr)|(ROM((adr)+1)<<8)|(ROM((adr)+2)<<16))
#define GETROM32(adr)		(ROM(adr)|(ROM((adr)+1)<<8)|(ROM((adr)+2)<<16)|(ROM((adr)+3)<<24))


#define ROME(ptr,size,adr)	((ptr)[(unsigned int)((((adr)>>16)*0x8000)|((adr)&0x7FFF))%(size)])

#define SETROME8(ptr,size,adr,val)	{ROME(ptr,size,adr)=(BYTE)(val);}
#define SETROME16(ptr,size,adr,val)	{SETROME8(ptr,size,adr,val);SETROME8(ptr,size,(adr)+1,(val)>>8);}
#define SETROME24(ptr,size,adr,val)	{SETROME16(ptr,size,adr,val);SETROME8(ptr,size,(adr)+2,(val)>>16);}
#define SETROME32(ptr,size,adr,val)	{SETROME16(ptr,size,adr,val);SETROME16(ptr,size,(adr)+2,(val)>>16);}

#define GETROME8(ptr,size,adr)		(ROME(ptr,size,adr))
#define GETROME16(ptr,size,adr)		(ROME(ptr,size,adr)|(ROME(ptr,size,(adr)+1)<<8))
#define GETROME24(ptr,size,adr)		(ROME(ptr,size,adr)|(ROME(ptr,size,(adr)+1)<<8)|ROME(ptr,size,(adr)+2)<<16))
#define GETROME32(ptr,size,adr)		(ROME(ptr,size,adr)|(ROME(ptr,size,(adr)+1)<<8)|ROME(ptr,size,(adr)+2)<<16)|(ROME(ptr,size,(adr)+3)<<24))



extern int multi ;

#define		MAP_TILE_SIZE_D		8
#define		MAP_TILE_SIZE		((int)(MAP_TILE_SIZE_D*multi/10))
#define		TRUE_MULTI			((double)MAP_TILE_SIZE/MAP_TILE_SIZE_D)


extern FZCD working ;
extern int editingre ;
extern int editingcn ;


extern void LoadFZEECLIP( char *filename ) ;
extern void CWT_common( char *Pdest ) ;