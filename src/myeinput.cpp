#include <windows.h>
#include "myeinput.h"
#include "keycode.h"

/*
	キーボード・マウス入力情報管理ルーチン。
*/

#define		MAXKEY		256
#define		FMN			0x01
#define		FMB			0x02
#define		FMD			0x80
#define		LOGMASK		0x3F


static BYTE LAkey[MAXKEY];

static bool LAmousebutton[3];
static bool LAbmousebutton[3];
static int  Lmousex,Lmousey;
static int  Lnmousex,Lnmousey;
static bool isinited=false;

static bool IsKeyValid=true ;
static bool IsMouseValid=true ;

bool KeyStart()
{
	for(int i=0 ; i<MAXKEY ; i++)
	{
		LAkey[i] = 0;
	}
	for(i=0 ; i<3 ; i++)
	{
		LAmousebutton[i]  = false;
		LAbmousebutton[i] = false;
	}
	isinited = true;
	return true;
}

bool KeyOn(int code)
{
	if(!isinited)KeyStart();
	if( !IsKeyValid )return false ;
	if( LAkey[code]&FMD )return false;
	if(LAkey[code]&FMN)return true;
	return false;
}
bool KeyOff(int code)
{
	if(!isinited)KeyStart();
	if( !IsKeyValid )return false ;
	if( LAkey[code]&FMD )return false;
	if(!LAkey[code]&FMN)return true;
	return false;
}
bool KeyPush(int code)
{
	if(!isinited)KeyStart();
	if( !IsKeyValid )return false ;
	if( LAkey[code]&FMD )return false;
	if(LAkey[code]&FMN && !(LAkey[code]&FMB))return true;
	return false;
}
bool KeyRelease(int code)
{
	if(!isinited)KeyStart();
	if( !IsKeyValid )return false ;
	if( LAkey[code]&FMD )return false;
	if(!(LAkey[code]&FMN) && LAkey[code]&FMB)return true;
	return false;
}

bool MouseOn(mousebuttonidentifyindex wb)
{
	if(!isinited)KeyStart();
	if( !IsMouseValid )return false ;
	if(LAmousebutton[wb])return true;
	return false;
}
bool MouseOff(mousebuttonidentifyindex wb)
{
	if(!isinited)KeyStart();
	if( !IsMouseValid )return false ;
	if(!LAmousebutton[wb])return true;
	return false;
}
bool MousePush(mousebuttonidentifyindex wb)
{
	if(!isinited)KeyStart();
	if( !IsMouseValid )return false ;
	if(LAmousebutton[wb] && !LAbmousebutton[wb])return true;
	return false;
}
bool MouseRelease(mousebuttonidentifyindex wb)
{
	if(!isinited)KeyStart();
	if( !IsMouseValid )return false ;
	if(!LAmousebutton[wb] && LAbmousebutton[wb])return true;
	return false;
}


void KeyMove()
{
	if(!isinited)KeyStart();
	for(int i=0 ; i<MAXKEY ; i++)
	{
		LAkey[i] = ((LAkey[i]<<1)|(LAkey[i]&FMN))&LOGMASK ;
	}
	for(i=0 ; i<3 ; i++)
	{
		LAbmousebutton[i] = LAmousebutton[i];
	}
	Lmousex = Lnmousex;
	Lmousey = Lnmousey;

	IsKeyValid   = true ;
	IsMouseValid = true ;
}
void ForceReleaseKey()
{
	if(!isinited)KeyStart();
	for(int i=0 ; i<MAXKEY ; i++)
	{
		LAkey[i] = 0;
	}
}
void ForceReleaseMouse()
{
	LAmousebutton[0] =
	LAmousebutton[1] =
	LAmousebutton[2] = false ;
}
void ProOn(int wparam)
{
BYTE tmp=wparam;
	if(!isinited)KeyStart();
	LAkey[tmp] |= FMN;
}
void ProOff(int wparam)
{
BYTE tmp=wparam;
	if(!isinited)KeyStart();
	LAkey[tmp] &= ~FMN;
}
void ProMOn(mousebuttonidentifyindex wb)
{
	if(!isinited)KeyStart();
	LAmousebutton[wb] = true;
}
void ProMOff(mousebuttonidentifyindex wb)
{
	if(!isinited)KeyStart();
	LAmousebutton[wb] = false;
}
void ProMMove(LONG lparam)
{
	if(!isinited)KeyStart();
	Lnmousex = (__int16)LOWORD(lparam);
	Lnmousey = (__int16)HIWORD(lparam);
}
int  GetMousePosX()
{
	if(!isinited)KeyStart();
	return Lmousex;
}
int  GetMousePosY()
{
	if(!isinited)KeyStart();
	return Lmousey;
}

int GetNumberKey( bool isHEXenable )
{
	if( !IsKeyValid )return -1 ;
	if( KeyPush(KC_0) || KeyPush(KC_NUM0) )return 0;
	if( KeyPush(KC_1) || KeyPush(KC_NUM1) )return 1;
	if( KeyPush(KC_2) || KeyPush(KC_NUM2) )return 2;
	if( KeyPush(KC_3) || KeyPush(KC_NUM3) )return 3;
	if( KeyPush(KC_4) || KeyPush(KC_NUM4) )return 4;
	if( KeyPush(KC_5) || KeyPush(KC_NUM5) )return 5;
	if( KeyPush(KC_6) || KeyPush(KC_NUM6) )return 6;
	if( KeyPush(KC_7) || KeyPush(KC_NUM7) )return 7;
	if( KeyPush(KC_8) || KeyPush(KC_NUM8) )return 8;
	if( KeyPush(KC_9) || KeyPush(KC_NUM9) )return 9;
	if( !isHEXenable )return -1 ;
	if( KeyPush(KC_A))return 10;
	if( KeyPush(KC_B))return 11;
	if( KeyPush(KC_C))return 12;
	if( KeyPush(KC_D))return 13;
	if( KeyPush(KC_E))return 14;
	if( KeyPush(KC_F))return 15;
	return -1;
}
void InvalidateKey()
{
	IsKeyValid = false ;
}
void InvalidateMouse()
{
	IsMouseValid = false ;
}
void InvalidateKeyCode(int code)
{
	if(!isinited)KeyStart();
	LAkey[code] |= FMD ;
}
bool IsValidKeyCode(int code)
{
	if(!isinited)KeyStart();
	if( LAkey[code]&FMD )return false;
	return true;
}
