#include <windows.h>
#include "myDIBobj3.h"
#include "myDIBobj3_p.h"


/*
	ストレッチ転送の関数。
	方針は、myDIBobj3_Cls.cppに書いてみた。
*/

//固定小数ForStretchらしい
#define UBITSFS			14
#define	D2FFS(num)		((INT32)((num)*(1<<UBITSFS)))
#define F2IFS(num)		((num)>>UBITSFS)

//こんくらいになると、ループ展開が果たして速いのかどうか疑わしい。
//たしか、テストしてみたら速かったから、こうなってるんじゃないかな……
#define RenderLoopForSBlt(BLTYPE,CKTYPE,SETYPE)	;\
for( iy=0 ; iy<height ; iy++ )				   \
{                                              \
	psx = prepsx ;                             \
	for( ix=0 ; ix<tw ; ix+=8 )                \
	{                                          \
		PixelSet(opt,&Pwline[ix+0],Prline[F2IFS(psx)],BLTYPE,CKTYPE,SETYPE);	\
		psx += vx ;                            \
		PixelSet(opt,&Pwline[ix+1],Prline[F2IFS(psx)],BLTYPE,CKTYPE,SETYPE);	\
		psx += vx ;                            \
		PixelSet(opt,&Pwline[ix+2],Prline[F2IFS(psx)],BLTYPE,CKTYPE,SETYPE);	\
		psx += vx ;                            \
		PixelSet(opt,&Pwline[ix+3],Prline[F2IFS(psx)],BLTYPE,CKTYPE,SETYPE);	\
		psx += vx ;                            \
		PixelSet(opt,&Pwline[ix+4],Prline[F2IFS(psx)],BLTYPE,CKTYPE,SETYPE);	\
		psx += vx ;                            \
		PixelSet(opt,&Pwline[ix+5],Prline[F2IFS(psx)],BLTYPE,CKTYPE,SETYPE);	\
		psx += vx ;                            \
		PixelSet(opt,&Pwline[ix+6],Prline[F2IFS(psx)],BLTYPE,CKTYPE,SETYPE);	\
		psx += vx ;                            \
		PixelSet(opt,&Pwline[ix+7],Prline[F2IFS(psx)],BLTYPE,CKTYPE,SETYPE);	\
		psx += vx ;                            \
	}                                          \
	for(  ; ix<width ; ix++ )                  \
	{                                          \
		PixelSet(opt,&Pwline[ix],Prline[F2IFS(psx)],BLTYPE,CKTYPE,SETYPE);	\
		psx += vx ;                            \
	}                                          \
	Pwline += prowline ;                       \
	psy += vy ;                                \
	if(psy&((~0)<<UBITSFS))                    \
	{                                          \
		Prline += prorline*(F2IFS(psy)) ;      \
		psy &= ~((~0)<<UBITSFS);               \
	}                                          \
}

void MyDIBObj3 :: SBlt(const MDO3Opt *opt,int isfc,int x,int y,int width,int height,int isrc,int srcx,int srcy,int srcwidth , int srcheight)
{
	if( !SFCCheck( isfc ) )return;
	if( !SFCCheck( isrc ) )return;

//幅と高さのスケール
double scalex,scaley;
	scalex = (double)  srcwidth /  width ;
	scaley = (double) srcheight / height ;
//クリップされた後の、srcの座標を浮動小数で
double srcxd,srcyd;
	//Ｘ方向クリッピング
	srcxd = srcx ;
	if( x < sfc[isfc].clipx )
	{//左にはみ出ている
		width -= (sfc[isfc].clipx-x);
		srcxd += (sfc[isfc].clipx-x)*scalex ;
		x = sfc[isfc].clipx ;
	}
	if( x+width > sfc[isfc].clipx+sfc[isfc].clipwidth )
	{//右にはみ出している
		width = sfc[isfc].clipx+sfc[isfc].clipwidth-x ;
	}
	//Ｘ方向クリッピング
	srcyd = srcy ;
	if( y < sfc[isfc].clipy )
	{//上にはみ出ている
		height -= (sfc[isfc].clipy-y);
		srcyd += (sfc[isfc].clipy-y)*scaley ;
		y = sfc[isfc].clipy ;
	}
	if( y+height > sfc[isfc].clipy+sfc[isfc].clipheight )
	{//下にはみ出している
		height = sfc[isfc].clipy+sfc[isfc].clipheight-y ;
	}
//src位置、ｘ位置の記憶
INT32 psx,psy,prepsx;
	psx = D2FFS(srcxd);
	psy = D2FFS(srcyd);
	prepsx = psx ;
//ベクトルじゃないけどさ……移動量
INT32 vx,vy;
	vx = D2FFS( scalex ) ;
	vy = D2FFS( scaley ) ;
//速くなるのかわからんが、列の読み書き位置先頭のポインタ
BMPD *Pwline,*Prline;
//その、進歩値の退避
int prowline,prorline;
//カウンタ……素朴すぎか？
int ix,iy;
//アンロールに使う、幅の限界値
int tw=width-7;
	Pwline = &sfc[isfc].data[x|(y<<sfc[isfc].width_bits)] ;
	if(opt->flag&MDO3F_Y_MIRROR)
	{//こんな程度で問題ないみたい
		Prline = &sfc[isrc].data[(srcheight-F2IFS(psy)-1)<<sfc[isrc].width_bits];
		psy %= D2FFS(1) ;
		prorline = -sfc[isrc].width ;
	}
	else
	{
		Prline = &sfc[isrc].data[(F2IFS(psy)<<sfc[isrc].width_bits)];
		psy %= D2FFS(1) ;
		prorline = sfc[isrc].width ;
	}
	prowline = sfc[isfc].width ;
	if(opt->flag&MDO3F_X_MIRROR)
	{
		vx *= -1 ;
		prepsx = D2FFS(srcwidth+srcx)-1-(prepsx-D2FFS(srcx));
	}
//////////////////////////////////////////////////////////////////////////////////
switch(opt->flag & (MDO3F_BLEND | MDO3F_LIGHT_BLEND | MDO3F_DARK_BLEND | MDO3F_OR_BLEND | MDO3F_AND_BLEND )){
case MDO3F_BLEND:
if((opt->alpha&0xF0)==0)return;
if((opt->alpha&0xF0)!=0xF0){
DWORD alphaselector= ((opt->alpha)&0xF0)<<(-4+16);
BMPD *AT     = &PAlphaTable[ alphaselector ] ;
BMPD *AT_inv = &PAlphaTable[ alphaselector^(0x0F<<16) ] ;
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_blend,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_blend,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_blend,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_blend,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_blend,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_blend,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_blend,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_blend,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_blend,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_blend,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;}
case 0:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_none,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_none,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_none,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_none,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_none,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_none,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_none,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_none,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_none,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_none,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
case MDO3F_LIGHT_BLEND:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_lightblend,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_lightblend,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_lightblend,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_lightblend,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_lightblend,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_lightblend,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_lightblend,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_lightblend,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_lightblend,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_lightblend,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
case MDO3F_DARK_BLEND:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_darkblend,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_darkblend,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_darkblend,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_darkblend,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_darkblend,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_darkblend,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_darkblend,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_darkblend,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_darkblend,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_darkblend,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
case MDO3F_OR_BLEND:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_or,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_or,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_or,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_or,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_or,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_or,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_or,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_or,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_or,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_or,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
case MDO3F_AND_BLEND:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_and,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_and,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_and,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_and,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_and,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_and,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_and,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_and,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForSBlt(BL_and,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForSBlt(BL_and,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
default:return;}
//////////////////////////////////////////////////////////////////////////////////

	return;
#if 0
//テスト
	for( iy=0 ; iy<height ; iy++ )
	{
		psx = prepsx ;
		for( ix=0 ; ix<tw ; ix+=8 )
		{
			Pwline[ix+0] = Prline[F2IFS(psx)] ;
			psx += vx ;
			Pwline[ix+1] = Prline[F2IFS(psx)] ;
			psx += vx ;
			Pwline[ix+2] = Prline[F2IFS(psx)] ;
			psx += vx ;
			Pwline[ix+3] = Prline[F2IFS(psx)] ;
			psx += vx ;
			Pwline[ix+4] = Prline[F2IFS(psx)] ;
			psx += vx ;
			Pwline[ix+5] = Prline[F2IFS(psx)] ;
			psx += vx ;
			Pwline[ix+6] = Prline[F2IFS(psx)] ;
			psx += vx ;
			Pwline[ix+7] = Prline[F2IFS(psx)] ;
			psx += vx ;
		}
		for(  ; ix<width ; ix++ )										
		{
			Pwline[ix] = Prline[F2IFS(psx)] ;
			psx += vx ;
		}
		Pwline += prowline ;
		psy += vy ;

#if 0
//どう比較したら、値を下げたら早いかをテスト。
//結論は５０歩１００歩。かっこつけて、＆を採用する
//*
		if(psy&D2FFS(1))
/*/
		if(psy>=D2FFS(1))
//*/
		{
/*
			psy &= ~D2FFS(1);
/*/
			psy -= D2FFS(1);
//*/
			Prline += prorline ;
		}
#endif
		if(psy&((~0)<<UBITSFS))
		{
			//最後に、ちょぉぉっと、ポインタ、領域の先を指すんですけど……ＣＰＵが例外出しませんように。
			Prline += prorline*(F2IFS(psy)) ;
			psy &= ~((~0)<<UBITSFS);
		}

	}

#endif
}
