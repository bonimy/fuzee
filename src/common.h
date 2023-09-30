#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <windows.h>

#ifdef _ORE_SENYOU
//私はいくつかのソースをライブラリ化しているので……
// I'm making some sources into libraries, so...
#ifdef _DEBUG
#pragma comment(lib, "gl_d.lib")
#else
#pragma comment(lib, "gl.lib")
#endif
#endif

#include "alias.h"
#include "fzcd.h"
#include "keycode.h"
#include "mydibobj3.h"
#include "myeinput.h"
#include "mymemorymanage.h"
#include "nnsys.h"
#include "task0702.h"
#include "taskfunction.h"

#ifdef _ORE_SENYOU
#include "HT/SNESsys.h"
#include "SFCEMU/_65816m.h"
#else
#include "SNESsys.h"
#include "_65816m.h"
#endif
//アプリケーションのウインドウへのドロップを許可するときはコメントアウトを外す
// Uncomment out to allow dropping onto the application window
#define MY_DROP_ACCEPT
//２重起動を（簡易）抑制するときはコメントアウトを外す
// Uncomment out to suppress double startup (simple)
#define ENABLE_MULTIPLE_RUN

#define COMMON_CONST_WINDOW_CLASS_NAME L"FZE_WCN"
#define COMMON_CONST_DEFAULT_WINDOW_TEXT L"ＦＺＥ"

#define WINWIDTH 780
#define WINHEIGHT 600

#define MAXSFC 10
#define MAXTASK 200

#define SFC_BACK 0
#define SFC_BG 1
#define SFC_MINIMAP 2


#define BASE_ROM_NAME L"fzero.smc"
#define DEFAULT_ROM_SIZE (1024 * 512)
#define EHASH_OF_AUTHOR (0x7b35aa)

#ifdef _DEBUG

//#define		MOSAIC_ENABLED

#endif

#define KAGEMOJI(sfc, str, x, y, h, w)                            \
    ;                                                             \
    {                                                             \
        mdo.Text(sfc, str, (x) + 1, (y) + 1, h, w, RGB(0, 0, 0)); \
        mdo.Text(sfc, str, (x), (y), h, w, RGB(255, 255, 255));   \
    } /**/


#define ADR_TILE_DEFINITION 0x0CC380


extern HWND hWnd;
extern HINSTANCE hInstance;


extern int AppPrepare(HWND hwnd, HINSTANCE hinstance);
extern int AppRelease();
extern void MainRoutine(int fps);

extern void RequestRedraw();
extern void RequestFlip();


extern MyDIBObj3 mdo;
extern Task0702Manager TM;

extern BYTE* Prom;
extern int romsize;

#define ROMADR2OFFSET(adr) \
    ((unsigned int)((((adr) >> 16) * 0x8000) | ((adr)&0x7FFF)) % romsize)
#define ROMADR2OFFSET2(adr) \
    ((unsigned int)((((adr) >> 16) * 0x8000) | ((adr)&0x7FFF)) % (romsize * 2))

#define ROM(adr) (Prom[ROMADR2OFFSET(adr)])
#define GET_ADR_BY_ROM(adr) \
    (ROM(adr) | (ROM(adr + 1) << 8) | (ROM(adr + 2) << 16) | (ROM(adr + 3) << 24))

// 8はなんとなく……
// 8 is somehow...
#define SETROM8(adr, val) \
    { ROM(adr) = (BYTE)(val); }
#define SETROM16(adr, val)              \
    {                                   \
        SETROM8(adr, val);              \
        SETROM8((adr) + 1, (val) >> 8); \
    }
#define SETROM24(adr, val)               \
    {                                    \
        SETROM16(adr, val);              \
        SETROM8((adr) + 2, (val) >> 16); \
    }
#define SETROM32(adr, val)                \
    {                                     \
        SETROM16(adr, val);               \
        SETROM16((adr) + 2, (val) >> 16); \
    }

#define GETROM8(adr) (ROM(adr))
#define GETROM16(adr) (ROM(adr) | (ROM((adr) + 1) << 8))
#define GETROM24(adr) (ROM(adr) | (ROM((adr) + 1) << 8) | (ROM((adr) + 2) << 16))
#define GETROM32(adr) \
    (ROM(adr) | (ROM((adr) + 1) << 8) | (ROM((adr) + 2) << 16) | (ROM((adr) + 3) << 24))


#define ROME(ptr, size, adr) \
    ((ptr)[(unsigned int)((((adr) >> 16) * 0x8000) | ((adr)&0x7FFF)) % (size)])

#define SETROME8(ptr, size, adr, val) \
    { ROME(ptr, size, adr) = (BYTE)(val); }
#define SETROME16(ptr, size, adr, val)              \
    {                                               \
        SETROME8(ptr, size, adr, val);              \
        SETROME8(ptr, size, (adr) + 1, (val) >> 8); \
    }
#define SETROME24(ptr, size, adr, val)               \
    {                                                \
        SETROME16(ptr, size, adr, val);              \
        SETROME8(ptr, size, (adr) + 2, (val) >> 16); \
    }
#define SETROME32(ptr, size, adr, val)                \
    {                                                 \
        SETROME16(ptr, size, adr, val);               \
        SETROME16(ptr, size, (adr) + 2, (val) >> 16); \
    }

#define GETROME8(ptr, size, adr) (ROME(ptr, size, adr))
#define GETROME16(ptr, size, adr) \
    (ROME(ptr, size, adr) | (ROME(ptr, size, (adr) + 1) << 8))
#define GETROME24(ptr, size, adr)		(ROME(ptr,size,adr)|(ROME(ptr,size,(adr)+1)<<8)|ROME(ptr,size,(adr)+2)<<16))
#define GETROME32(ptr, size, adr)		(ROME(ptr,size,adr)|(ROME(ptr,size,(adr)+1)<<8)|ROME(ptr,size,(adr)+2)<<16)|(ROME(ptr,size,(adr)+3)<<24))


extern int multi;

#define MAP_TILE_SIZE_D 8
#define MAP_TILE_SIZE ((int)(MAP_TILE_SIZE_D * multi / 10))
#define TRUE_MULTI ((double)MAP_TILE_SIZE / MAP_TILE_SIZE_D)


extern FZCD working;
extern int editingre;
extern int editingcn;


extern void LoadFZEECLIP(wchar_t* filename);
extern void CWT_common(wchar_t* Pdest);