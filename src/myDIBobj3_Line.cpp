#include <windows.h>
#include "myDIBobj3.h"
#include "myDIBobj3_p.h"

/*
	線を描画する関数。
	方針は、myDIBobj3_Cls.cppに書いてみた。
*/


//固定小数ForLine
#define UBITSFL			14
#define	D2FFL(num)		((INT32)(num*(1<<UBITSFL)))
#define F2IFL(num)		(num>>UBITSFL)


//範囲を２つずつに分けていって、適合するギリギリの位置を探す。
//汚らしいやり方だが、案外いいかも
//右が常に範囲内
static int DivideCorrectR( int sa , int ea , int cb0 , int cb1 , int b0 , INT32 slope)
{
	//中間位置
int ma = (ea+sa)/2 ;
int mb ;
	//ギリギリ位置である
	if( ma == sa )
	{
		return ea ;//右が範囲内だから
	}
	//中間位置で計算
	mb = b0 + slope * (ma-sa) ;
	if( cb0 <= mb && mb < cb1 )
	{
	//中間位置はＯＫだった
		return DivideCorrectR( sa , ma , cb0 , cb1 , b0 , slope ) ;
	}
	else
	{//中間位置はダメだった
		return DivideCorrectR( ma , ea , cb0 , cb1 , mb , slope ) ;
	}
}
//左が常に範囲内
static int DivideCorrectL( int sa , int ea , int cb0 , int cb1 , int b0 , INT32 slope)
{
	//中間位置
int ma = (ea+sa)/2 ;
int mb ;
	//ギリギリ位置である
	if( ma == sa )
	{
		return sa ;//左が範囲内だから
	}
	//中間位置で計算
	mb = b0 + slope * (ma-sa) ;
	if( cb0 <= mb && mb < cb1 )
	{
	//中間位置はＯＫだった
		return DivideCorrectL( ma , ea , cb0 , cb1 , mb , slope ) ;
	}
	else
	{//中間位置はダメだった
		return DivideCorrectL( sa , ma , cb0 , cb1 , b0 , slope ) ;
	}
}
//両方補正する必要がある。実は、マイナーケースである。
//なぜなら、画面をまたいでしまうときは、大抵逆の座標系が適用されるから。
//……しかし、可能性はある。
//たぶん、中間の位置は範囲内なので（よっぽどおかしな画面比率だと話が違ってくるけど）
//範囲内かどうか調べ、範囲内なら、DivideCorrectR,Lをする。
//範囲外だったら……度外視。
static bool EitherCorrect( int *Psa , int *Pea , INT32 *Ppdb , int cb0 , int cb1 , int b0 , INT32 slope )
{
int ma=((*Pea)+(*Psa))/2 ;
int mb=b0+slope*(ma-(*Psa));
	if( mb < cb0 || cb1 <= mb )return false;
int tmp;
	tmp = DivideCorrectR( (*Psa) , ma , cb0 , cb1 , b0 , slope ) ;
	*Ppdb += ((tmp-*Psa)*slope) ;
	*Psa = tmp ;
	*Pea = DivideCorrectL( ma , (*Pea) , cb0 , cb1 , mb , slope ) ;
	return true;
}


#define RenderLoopForLine(BLTYPE,CKTYPE,SETYPE)							\
	for( na=sa ; na<ea ; na++ )											\
	{																	\
		if( isXY )														\
		{																\
			PixelSet(opt,&P00[ na | (F2IFL(pdb)<<noshift)],color,BLTYPE,CKTYPE,SETYPE);	\
		}																\
		else															\
		{																\
			PixelSet(opt,&P00[ F2IFL(pdb) | (na<<noshift)],color,BLTYPE,CKTYPE,SETYPE);	\
		}																\
		pdb += slope ;													\
	}


inline void MyDIBObj3 :: Line_sub2( bool isXY , double sloped , const MDO3Opt *opt , int isfc , double a1d , double b1d , double a2d , double b2d , BMPD color )
{
//a-b座標系と見る
//両方向に０．５を足し、a方向の小数を切り捨て（つまり四捨五入）、a方向の整数位置を計算する。
	a1d += 0.5;
	a2d += 0.5;
	b1d += 0.5;
	b2d += 0.5;
//傾きを固定小数で計算する
INT32 slope;
	slope  = D2FFL( sloped ) ;
//ループ中、b位置を固定小数で示すもの。present delta b
INT32 pdb;
	pdb    = D2FFL( b1d );
//a方向切捨てに際する移動分（必ず左に移動するため、引く）
	pdb -= D2FFL(sloped*(a1d-(int)a1d));

//サーフェイスのゼロ位置
BMPD *P00=sfc[isfc].data ;
int  noshift=sfc[isfc].width_bits ;
//ループカウンタ
int na;
//開始・終了位置
int sa = (int)a1d ;
int ea = (int)a2d ;
//クリッピング
int ca,cas,cb,cbs;
	if( isXY )
	{
		ca  = sfc[isfc].clipx ;
		cas = ca + sfc[isfc].clipwidth ;
		cb  = sfc[isfc].clipy ;
		cbs = cb + sfc[isfc].clipheight ;
	}
	else
	{
		cb  = sfc[isfc].clipx ;
		cbs = cb + sfc[isfc].clipwidth ;
		ca  = sfc[isfc].clipy ;
		cas = ca + sfc[isfc].clipheight ;
	}
//まずは、a方向（これだけなら楽なのに……）
	if( sa < ca )
	{
		pdb += ((ca-sa)*slope) ;
		sa = ca ;
	}
	if( ea > cas )
	{
		ea = cas ;
	}
	if( sa > ea )return;//不必要か？
//難解なb方向
//クリップ範囲を固定小数にしておこう
	ca  = D2FFL( ca  ) ;
	cas = D2FFL( cas ) ;
	cb  = D2FFL( cb  ) ;
	cbs = D2FFL( cbs ) ;
INT32 expeb;//予期される、最終ｂ位置
	expeb = pdb + slope * (ea-sa) ;
	if( pdb < cb )
	{//始まりの位置で、ｂ負方向にはみ出ている
		if( expeb < cb )
		{//終わりの位置で、ｂ負方向にはみ出ている
			return ;
		}
		else if (expeb >= cbs )
		{//終わりの位置で、ｂ正方向にはみ出ている
			if(!EitherCorrect( &sa , &ea , &pdb , cb , cbs , pdb , slope ))return;
		}
		else
		{//終わりの位置ははみ出していない
int tmp;
			tmp = DivideCorrectR( sa , ea , cb , cbs , pdb , slope ) ;
			pdb += ((tmp-sa)*slope) ;
			sa = tmp ;
		}
	}
	else if (pdb >= cbs )
	{//始まりの位置で、ｂ正方向にはみ出ている
		if( expeb < cb )
		{//終わりの位置で、ｂ負方向にはみ出ている
			if(!EitherCorrect( &sa , &ea , &pdb , cb , cbs , pdb , slope ))return;
		}
		else if (expeb >= cbs )
		{//終わりの位置で、ｂ正方向にはみ出ている
			return ;	
		}
		else
		{//終わりの位置ははみ出していない
int tmp;
			tmp = DivideCorrectR( sa , ea , cb , cbs , pdb , slope ) ;
			pdb += ((tmp-sa)*slope) ;
			sa = tmp ;
		}
	}
	else
	{//始まりの位置ははみ出していない
		if( expeb < cb )
		{//終わりの位置で、ｂ負方向にはみ出ている
			ea = DivideCorrectL( sa , ea , cb , cbs , pdb , slope ) ;
		}
		else if (expeb >= cbs )
		{//終わりの位置で、ｂ正方向にはみ出ている
			ea = DivideCorrectL( sa , ea , cb , cbs , pdb , slope ) ;
		}
		else
		{//終わりの位置ははみ出していない
			//なにもしなくてＯＫ
		}
	}
/*
	//テスト用！！！
#ifdef _DEBUG
	ca  = F2IFL( ca  ) ;
	cas = F2IFL( cas ) ;
	cb  = F2IFL( cb  ) ;
	cbs = F2IFL( cbs ) ;
#endif
	for( na=sa ; na<ea ; na++ )
	{
		//テストルーチン！！！
#ifdef _DEBUG
		if( na < ca || na>= cas || F2IFL(pdb)<cb || F2IFL(pdb)>=cbs)
		{
			MessageBox( hwndp , "暴走しとるよ" , "ぐぶぅ" , MB_OK ) ;
		}
#endif
		if( isXY )
		{
			P00[ na | (F2IFL(pdb)<<noshift) ] = color ;
		}
		else
		{
			P00[ F2IFL(pdb) | (na<<noshift) ] = color ;
		}
		pdb += slope ;
	}
*/
/////////////////////////////////////////////////////////////////////////////////////////////
switch(opt->flag & (MDO3F_BLEND | MDO3F_LIGHT_BLEND | MDO3F_DARK_BLEND | MDO3F_OR_BLEND | MDO3F_AND_BLEND )){
case MDO3F_BLEND:
if((opt->alpha&0xF0)==0)return;
if((opt->alpha&0xF0)!=0xF0){
DWORD alphaselector= ((opt->alpha)&0xF0)<<(-4+16);
BMPD *AT     = &PAlphaTable[ alphaselector ] ;
BMPD *AT_inv = &PAlphaTable[ alphaselector^(0x0F<<16) ] ;
RenderLoopForLine(BL_blend,CK_none,SE_none);break;}
case 0:
RenderLoopForLine(BL_none,CK_none,SE_none);break;
case MDO3F_LIGHT_BLEND:
RenderLoopForLine(BL_lightblend,CK_none,SE_none);break;
case MDO3F_DARK_BLEND:
RenderLoopForLine(BL_darkblend,CK_none,SE_none);break;
case MDO3F_OR_BLEND:
RenderLoopForLine(BL_or,CK_none,SE_none);break;
case MDO3F_AND_BLEND:
RenderLoopForLine(BL_and,CK_none,SE_none);break;
default:return;}
/////////////////////////////////////////////////////////////////////////////////////////////
}
inline void MyDIBObj3 :: Line_sub1( bool isXY , double sloped , const MDO3Opt *opt , int isfc , double a1d , double b1d , double a2d , double b2d , BMPD color )
{
//a-b座標系と見る。a1とa2を比較し、小さいほうから大きいほうに処理するようにする
	if( a1d < a2d ) 
	{
		Line_sub2( isXY , sloped , opt , isfc , a1d , b1d , a2d , b2d , color ) ;
	}
	else
	{
		Line_sub2( isXY , sloped , opt , isfc , a2d , b2d , a1d , b1d , color ) ;
	}
}
void MyDIBObj3 :: Line(const MDO3Opt *opt,int isfc,double x1,double y1,double x2,double y2,BMPD color)
{
	if(!SFCCheck(isfc))return;
	if(opt->flag & MDO3F_USE_WINAPI )
	{
		MoveToEx(sfc[isfc].hdc,(int)x1,(int)y1,NULL);
		LineTo(sfc[isfc].hdc,(int)x2,(int)y2);
		return;
	}
double deltax,deltay;

	deltax = x2 - x1;
	deltay = y2 - y1;
//	if(MYABS(MYABS(deltax) - MYABS(deltay)) < 0.0000001)return;
	if(MYABS(deltax) >= MYABS(deltay))
	{
		Line_sub1(true,deltay/deltax , opt , isfc , x1 , y1 , x2 , y2 , color);
	}
	else
	{
		Line_sub1(false,deltax/deltay , opt , isfc , y1 , x1 , y2 , x2 , color);
	}
}

