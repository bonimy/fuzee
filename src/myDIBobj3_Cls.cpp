#include <windows.h>
#include "myDIBobj3.h"
#include "myDIBobj3_p.h"

/*
	矩形単色塗り関数。
	どの描画関数もですが、ちょい下を見ればわかるとおり、マクロ酷使な仕様です。
	おかげさまで、ビルドに時間がかかり、さらには実行ファイルのサイズは肥大。
	ＶＣ＋＋がコンパイルに際してヒープを使い果たすなど、酷い惨状。
*/

/*
	レンダリングのループ。想定される全ての条件下で、このマクロを呼ぶ。
	今思いついたのだが、比較は『ある値以下』より、『０と比較』の方が速いだろうか……
	ループを逆向きにしたほうがいい？
	（まぁ、速いかどうかなんて環境依存だけどね……でも一般的に０との比較は速かったような……？）
*/

#define RenderLoopForCls(BLTYPE,CKTYPE,SETYPE)	;\
{																			\
BMPD *Psfc;																	\
int ix,iy;																	\
int tw=width-7;																\
	Psfc = &sfc[isfc].data[(y<<sfc[isfc].width_bits)+x];					\
	for( iy=0 ; iy<height ; iy++)											\
	{																		\
		for( ix=0 ; ix<tw ; ix+=8 )											\
		{																	\
			PixelSet(opt,&Psfc[ix+0],color,BLTYPE,CKTYPE,SETYPE);			\
			PixelSet(opt,&Psfc[ix+1],color,BLTYPE,CKTYPE,SETYPE);			\
			PixelSet(opt,&Psfc[ix+2],color,BLTYPE,CKTYPE,SETYPE);			\
			PixelSet(opt,&Psfc[ix+3],color,BLTYPE,CKTYPE,SETYPE);			\
			PixelSet(opt,&Psfc[ix+4],color,BLTYPE,CKTYPE,SETYPE);			\
			PixelSet(opt,&Psfc[ix+5],color,BLTYPE,CKTYPE,SETYPE);			\
			PixelSet(opt,&Psfc[ix+6],color,BLTYPE,CKTYPE,SETYPE);			\
			PixelSet(opt,&Psfc[ix+7],color,BLTYPE,CKTYPE,SETYPE);			\
		}																	\
		for(  ; ix<width ; ix++ )											\
		{																	\
			PixelSet(opt,&Psfc[ix+0],color,BLTYPE,CKTYPE,SETYPE);			\
		}																	\
		Psfc += sfc[isfc].width;											\
	}																		\
}


void MyDIBObj3 :: Cls(const MDO3Opt *opt,int isfc,int x,int y,int width,int height,BMPD color)
{
	if( !SFCCheck( isfc ) )return;
	if( opt->flag & MDO3F_CLIP )
	{
		if( !Clipping( opt ,  false , &x , &y , &width , &height , NULL , NULL , sfc[isfc].clipx , sfc[isfc].clipy , sfc[isfc].clipwidth , sfc[isfc].clipheight ) )return;
	}
//////////////////////////////////////////////////////////////////////////////////////////
switch(opt->flag & (MDO3F_BLEND | MDO3F_LIGHT_BLEND | MDO3F_DARK_BLEND | MDO3F_OR_BLEND | MDO3F_AND_BLEND )){
case MDO3F_BLEND:
if((opt->alpha&0xF0)==0)return;
if((opt->alpha&0xF0)!=0xF0){
DWORD alphaselector= ((opt->alpha)&0xF0)<<(-4+16);
BMPD *AT     = &PAlphaTable[ alphaselector ] ;
BMPD *AT_inv = &PAlphaTable[ alphaselector^(0x0F<<16) ] ;
RenderLoopForCls(BL_blend,CK_none,SE_none);break;}
case 0:
RenderLoopForCls(BL_none,CK_none,SE_none);break;
case MDO3F_LIGHT_BLEND:
RenderLoopForCls(BL_lightblend,CK_none,SE_none);break;
case MDO3F_DARK_BLEND:
RenderLoopForCls(BL_darkblend,CK_none,SE_none);break;
case MDO3F_OR_BLEND:
RenderLoopForCls(BL_or,CK_none,SE_none);break;
case MDO3F_AND_BLEND:
RenderLoopForCls(BL_and,CK_none,SE_none);break;
default:return;}
//////////////////////////////////////////////////////////////////////////////////////////

}

