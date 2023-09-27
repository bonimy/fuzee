#include <windows.h>
#include "myDIBobj3.h"
#include "myDIBobj3_p.h"

/*
	矩形転送の関数。
	方針は、myDIBobj3_Cls.cppに書いてみた。
*/
/*
	コンパイルの時間が一番かかるソース。
	このソースの一番下をみれば、どんだけ無茶しているのか明白だけどね。
*/
#define RenderLoopForBlt(opt,x,y,srcx,srcy,lpwidth,lpheight,BLTYPE,CKTYPE,SETYPE)	;\
{																			\
BMPD *Psfc,*Psrc;															\
int tw=lpwidth-7;															\
int ix,iy;																	\
int procelinesrc;															\
int procelinesfc = sfc[isfc].width ;										\
Psfc = &sfc[isfc].data[(y<<sfc[isfc].width_bits)+x];						\
	Psrc = &sfc[isrc].data[(srcy<<sfc[isrc].width_bits)+srcx];				\
	procelinesrc = sfc[isrc].width ;										\
	if(opt->flag & MDO3F_Y_MIRROR )											\
	{																		\
		Psrc += ((lpheight-1)<<sfc[isrc].width_bits);						\
		procelinesrc = -sfc[isrc].width ;									\
	}																		\
	if( (opt->flag & MDO3F_X_MIRROR) == 0)									\
	{																		\
		for( iy=0 ; iy<lpheight ; iy++)										\
		{																	\
			for( ix=0 ; ix<tw ; ix+=8 )										\
			{																\
				PixelSet(opt,&Psfc[ix+0],Psrc[ix+0],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+1],Psrc[ix+1],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+2],Psrc[ix+2],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+3],Psrc[ix+3],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+4],Psrc[ix+4],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+5],Psrc[ix+5],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+6],Psrc[ix+6],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+7],Psrc[ix+7],BLTYPE,CKTYPE,SETYPE);	\
			}																\
			for(  ; ix<lpwidth ; ix++ )										\
			{																\
				PixelSet(opt,&Psfc[ix],Psrc[ix],BLTYPE,CKTYPE,SETYPE);		\
			}																\
			Psfc += procelinesfc;											\
			Psrc += procelinesrc;											\
		}																	\
	}																		\
	else																	\
	{																		\
		Psrc += lpwidth-1;													\
		for( iy=0 ; iy<lpheight ; iy++)										\
		{																	\
			for( ix=0 ; ix<tw ; ix+=8 )										\
			{																\
				PixelSet(opt,&Psfc[ix+0],Psrc[-ix-0],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+1],Psrc[-ix-1],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+2],Psrc[-ix-2],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+3],Psrc[-ix-3],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+4],Psrc[-ix-4],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+5],Psrc[-ix-5],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+6],Psrc[-ix-6],BLTYPE,CKTYPE,SETYPE);	\
				PixelSet(opt,&Psfc[ix+7],Psrc[-ix-7],BLTYPE,CKTYPE,SETYPE);	\
			}																\
			for(  ; ix<lpwidth ; ix++ )										\
			{																\
				PixelSet(opt,&Psfc[ix],Psrc[-ix],BLTYPE,CKTYPE,SETYPE);		\
			}																\
			Psfc += procelinesfc;											\
			Psrc += procelinesrc;											\
		}																	\
	}																		\
}


void MyDIBObj3 :: Blt(const MDO3Opt *opt,int isfc,int x,int y,int width,int height,int isrc,int srcx,int srcy)
{
	if( !SFCCheck( isfc ) )return;
	if( !SFCCheck( isrc ) )return;
	if( opt->flag & MDO3F_CLIP )
	{
		if( !Clipping( opt , true , &x , &y , &width , &height , &srcx , &srcy , sfc[isfc].clipx , sfc[isfc].clipy , sfc[isfc].clipwidth , sfc[isfc].clipheight ) )return;
	}
	else if( opt->flag & MDO3F_SFCLOOP )
	{
		if(width > sfc[isfc].clipwidth || height > sfc[isfc].clipheight)return;
int rx,ry;
		rx = x - sfc[isfc].clipx ;
		ry = y - sfc[isfc].clipy ;
		if(rx < 0)rx = rx % sfc[isfc].clipwidth + sfc[isfc].clipwidth ;
		if(ry < 0)ry = ry % sfc[isfc].clipheight+ sfc[isfc].clipheight;
		if(rx >= sfc[isfc].clipwidth )rx %= sfc[isfc].clipwidth; 
		if(ry >= sfc[isfc].clipheight)ry %= sfc[isfc].clipheight; 
		x = rx + sfc[isfc].clipx ;
		y = ry + sfc[isfc].clipy ;
		if(rx+width > sfc[isfc].clipwidth )
		{
			Blt(opt,isfc,x,y,sfc[isfc].clipwidth-rx,height,isrc,srcx,srcy);
			Blt(opt,isfc,sfc[isfc].clipx,y,rx+width-sfc[isfc].clipwidth,height,isrc,srcx+sfc[isfc].clipwidth-rx,srcy);
			return;
		}
		if(ry+height> sfc[isfc].clipheight)
		{
			Blt(opt,isfc,x,y,width,sfc[isfc].clipheight-ry,isrc,srcx,srcy);
			Blt(opt,isfc,x,sfc[isfc].clipy,width,ry+height-sfc[isfc].clipheight,isrc,srcx,srcy+sfc[isfc].clipheight-ry);
			return;
		}
	}
	else if( opt->flag & MDO3F_SRCLOOP )
	{
		if( !Clipping( opt ,  true , &x , &y , &width , &height , &srcx , &srcy , sfc[isfc].clipx , sfc[isfc].clipy , sfc[isfc].clipwidth , sfc[isfc].clipheight ) )return;
	}
	if( !ReverseClipping( srcx , srcy , width , height , isrc ) )return;

	if( opt->flag & MDO3F_USE_WINAPI )
	{
		BitBlt(sfc[isfc].hdc,x,y,width,height,sfc[isrc].hdc,srcx,srcy,opt->DW);
		return;
	}
//////////////////////////////////////////////////////////////////////////////////////////
switch(opt->flag & (MDO3F_BLEND | MDO3F_LIGHT_BLEND | MDO3F_DARK_BLEND | MDO3F_OR_BLEND | MDO3F_AND_BLEND )){
case MDO3F_BLEND:
if((opt->alpha&0xF0)==0)return;
if((opt->alpha&0xF0)!=0xF0){
DWORD alphaselector= ((opt->alpha)&0xF0)<<(-4+16);
BMPD *AT     = &PAlphaTable[ alphaselector ] ;
BMPD *AT_inv = &PAlphaTable[ alphaselector^(0x0F<<16) ] ;
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_blend,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;}
case 0:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_none,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
case MDO3F_LIGHT_BLEND:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_lightblend,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
case MDO3F_DARK_BLEND:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_darkblend,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
case MDO3F_OR_BLEND:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_or,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
case MDO3F_AND_BLEND:
	switch(opt->flag & (MDO3F_OR_OPERATE | MDO3F_AND_OPERATE | MDO3F_SIMPLE_COLOR | MDO3F_USE_COLOR_TABLE | MDO3F_BRIGHT_CHANGE)){
	case 0:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_none,SE_none);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_colorkey,SE_none);break;default:return;}
	break;case MDO3F_USE_COLOR_TABLE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_none,SE_colortable);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_colorkey,SE_colortable);break;default:return;}
	break;case MDO3F_SIMPLE_COLOR:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_none,SE_simple);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_colorkey,SE_simple);break;default:return;}
	break;case MDO3F_OR_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_none,SE_or);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_colorkey,SE_or);break;default:return;}
	break;case MDO3F_AND_OPERATE:
		switch(opt->flag & (MDO3F_COLORKEY | MDO3F_USE_OTHER_COLORKEY)){case 0:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_none,SE_and);break;case MDO3F_COLORKEY:RenderLoopForBlt(opt,x,y,srcx,srcy,width,height,BL_and,CK_colorkey,SE_and);break;default:return;}
	break;default:return;}
break;
default:return;}
//////////////////////////////////////////////////////////////////////////////////////////
}



