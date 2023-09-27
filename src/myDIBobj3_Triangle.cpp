#if 0

#include <windows.h>
#include "myDIBobj3.h"
#include "myDIBobj3_p.h"
/*
	三角形を描画する関数。
	方針は、myDIBobj3_Cls.cppに書いてみた。

	……２点を同じ座標に指定して、１点だけ違うとき、２つの三角形はぴったり繋がらないといけないのだが、
	その２点が水平に近いとき危ないんだっけな……
	水平に近いなんていうのはザラにあることに思えるがなぁ。
	っていうか、いまこのソース見たが、なにやってんだか全くわからんかった……

	少し弄って、誤魔化して穴は空かないようにした。
	が、代わりに少々広範囲を書くようにしたため、半透明を重ねると……
	それだけじゃなく、なんか変な出っ張りが生じる……
	さらに弄って、縦方向に強制的に１大きくするようにした……
	あまりに顕著……
*/

//固定小数ForTRIangleらしい
#define UBITSFTRI			14
#define	D2FFTRI(num)		((INT32)((num)*(1<<UBITSFTRI)))
#define F2IFTRI(num)		((num)>>UBITSFTRI)










//２点水平時の穴あき防止用ごまかし拡張幅の遊び
//（穴が開くよりは重なった方が……という発想）
#define FILL_WIDTH			0

#define SET_FOR_TRIANGLE(n1,n2,n3)								;\
	leng[0] = (int)y##n1;										\
	leng[1] = (int)y##n2;										\
	leng[2] = (int)y##n3;										\
	if( x##n2 < ELINE_GETX(&line[n2],y##n2) )					\
	{/*左向き*/													\
		/*傾きをセット*/										\
		leftslope[0] = D2FFTRI(line[n3].slope) ;				\
		rightslope[0] = D2FFTRI(line[n2].slope) ;				\
		leftslope[1] = D2FFTRI(line[n1].slope) ;				\
		rightslope[1] = D2FFTRI(line[n2].slope) ;				\
		/*マス目の上はしでの位置に補正しつつセット*/			\
		leftppos[0] = D2FFTRI(x##n1-line[n3].slope*(y##n1-(int)y##n1) ) ;	\
		rightppos[0] = D2FFTRI(x##n1-line[n2].slope*(y##n1-(int)y##n1) ) ;	\
		leftppos[1] = D2FFTRI(x##n2+line[n1].slope*(1-y##n2+(int)y##n2) ) ;	\
		rightconnect = true;									\
	}															\
	else														\
	{/*右向き*/													\
		/*傾きをセット*/										\
		leftslope[0] = D2FFTRI(line[n2].slope) ;				\
		rightslope[0] = D2FFTRI(line[n3].slope) ;				\
		leftslope[1] = D2FFTRI(line[n2].slope) ;				\
		rightslope[1] = D2FFTRI(line[n1].slope) ;				\
		/*マス目の上はしでの位置に補正しつつセット*/			\
		leftppos[0] = D2FFTRI(x##n1-line[n2].slope*(y##n1-(int)y##n1) ) ;	\
		rightppos[0] = D2FFTRI(x##n1-line[n3].slope*(y##n1-(int)y##n1) ) ;	\
		rightppos[1] = D2FFTRI(x##n2+line[n1].slope*(1-y##n2+(int)y##n2) ) ;	\
		leftconnect = true;										\
	}

/*
#define SET_FOR_TRIANGLE(n1,n2,n3)								;\
	fixfillL = 0 ;	fixfillR = 0 ;								\
	leftppos[0] = rightppos[0] = D2FFTRI(x##n1) ;				\
	leftppos[1]  = D2FFTRI(x##n2);								\
	rightppos[1] = D2FFTRI(ELINE_GETX(&line[n2],y##n2));\
	leng[0] = (int)y##n1;										\
	leng[1] = (int)y##n2;										\
	leng[2] = (int)y##n3;										\
	if( leftppos[1] < rightppos[1] )							\
	{															\
		leftslope[0] = D2FFTRI(line[n3].slope) ;				\
		rightslope[0] = D2FFTRI(line[n2].slope) ;				\
		leftslope[1] = D2FFTRI(line[n1].slope) ;				\
		rightslope[1] = D2FFTRI(line[n2].slope) ;				\
		rightconnect = true;									\
	}															\
	else														\
	{															\
		MYSWAP( &leftppos[1] , &rightppos[1] ) ;				\
		leftslope[0] = D2FFTRI(line[n2].slope) ;				\
		rightslope[0] = D2FFTRI(line[n3].slope) ;				\
		leftslope[1] = D2FFTRI(line[n2].slope) ;				\
		rightslope[1] = D2FFTRI(line[n1].slope) ;				\
		leftconnect = true;										\
	}


*/
#define		SET_FOR_TRIANGLE_MINOR(n1,n2,dp)			;\
		if(y##n1 < y##dp )								\
		{												\
			leng[1] = (int)(y##n1);						\
			leng[2] = (int)(y##dp);						\
			if( x##n1 < x##n2 )								\
			{												\
				leftppos[1] = D2FFTRI(x##n1);				\
				rightppos[1] = D2FFTRI(x##n2);				\
				leftslope[1] = D2FFTRI(line[n2].slope);		\
				rightslope[1] = D2FFTRI(line[n1].slope);	\
			}												\
			else											\
			{												\
				leftppos[1] = D2FFTRI(x##n2);				\
				rightppos[1] = D2FFTRI(x##n1);				\
				leftslope[1] = D2FFTRI(line[n1].slope);		\
				rightslope[1] = D2FFTRI(line[n2].slope);	\
			}											\
			fixfillL = -D2FFTRI(FILL_WIDTH) ;	\
			fixfillR = +D2FFTRI(FILL_WIDTH) ;	\
		}												\
		else											\
		{												\
			leng[1] = (int)(y##dp);						\
			leng[2] = (int)(y##n1);						\
			if( x##n1 < x##n2 )								\
			{												\
				leftppos[1] =								\
				rightppos[1] = D2FFTRI(x##dp);				\
				leftslope[1] = D2FFTRI(line[n2].slope);		\
				rightslope[1] = D2FFTRI(line[n1].slope);	\
			}												\
			else											\
			{												\
				leftppos[1] =								\
				rightppos[1] = D2FFTRI(x##dp);				\
				leftslope[1] = D2FFTRI(line[n1].slope);		\
				rightslope[1] = D2FFTRI(line[n2].slope);	\
			}											\
			fixfillL =-D2FFTRI(FILL_WIDTH) ;	\
			fixfillR = +D2FFTRI(FILL_WIDTH) ;	\
		}


#define	RenderLoopForTriangle_sub(clipenabled,BLTYPE,CKTYPE,SETYPE) ;\
			for(iy=0 ;iy<finexp;iy++)								\
			{														\
int linewidth =  F2IFTRI(rightppos[i])-F2IFTRI(leftppos[i]) ;		\
				if(clipenabled)										\
				{													\
int tmpx=F2IFTRI(leftppos[i]);										\
					if(tmpx<sfc[isfc].clipx)									\
					{															\
						linewidth -= (sfc[isfc].clipx-tmpx);					\
						tmpx = sfc[isfc].clipx ;								\
					}															\
					if(tmpx+linewidth>sfc[isfc].clipx+sfc[isfc].clipwidth)		\
					{															\
						linewidth = sfc[isfc].clipx+sfc[isfc].clipwidth-tmpx;	\
					}															\
					Psfcsenv = Psfc + tmpx ;									\
				}													\
				else												\
				{													\
					Psfcsenv = Psfc + F2IFTRI(leftppos[i]) ;		\
				}													\
int tw=linewidth - 7 ;												\
				for(ix= 0 ; ix<tw  ; ix += 8 )						\
				{													\
	PixelSet(opt,&Psfcsenv[ix+0],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+1],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+2],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+3],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+4],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+5],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+6],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+7],color,BLTYPE,CKTYPE,SETYPE);		\
				}													\
				for( ; ix<linewidth  ; ix ++ )						\
				{													\
	PixelSet(opt,&Psfcsenv[ix+0],color,BLTYPE,CKTYPE,SETYPE);		\
				}													\
				Psfc += sfc[isfc].width ;							\
				leftppos[i]  +=  leftslope[i] ;						\
				rightppos[i]  +=  rightslope[i] ;					\
			}

#define	RenderLoopForTriangle(BLTYPE,CKTYPE,SETYPE) ;\
		if(issideclip)												\
		{															\
			RenderLoopForTriangle_sub(1,BLTYPE,CKTYPE,SETYPE) ;		\
		}															\
		else														\
		{															\
			RenderLoopForTriangle_sub(0,BLTYPE,CKTYPE,SETYPE) ;		\
		}


void MyDIBObj3 :: Triangle( const MDO3Opt *opt , int isfc , double x0 , double y0 , double x1 , double y1 , double x2 , double y2 , BMPD color )
{
ELINE line[3];
int sameheight=0;
//マス目のセンターを見る……代わりに、ずらして格子点を見る。
	x0 -= 0.5 ;
	y0 -= 0.5 ;
	x1 -= 0.5 ;
	y1 -= 0.5 ;
	x2 -= 0.5 ;
	y2 -= 0.5 ;

	if( !ELINE_SET( &line[0] , x1 , y1 , x2 , y2 ) )sameheight+=1;	
	if( !ELINE_SET( &line[1] , x2 , y2 , x0 , y0 ) )sameheight+=2;	
	if( !ELINE_SET( &line[2] , x0 , y0 , x1 , y1 ) )sameheight+=4;	
INT32 leftslope[2],rightslope[2];
INT32 leftppos[2],rightppos[2];
bool leftconnect,rightconnect;
int leng[3];
	leftconnect = false;
	rightconnect = false;
	switch( sameheight )
	{
	case 0://一致無し
		//きったねぇやり方……
		if( y0 < y1 )
		{

			if( y2 < y0 )
			{//2 0 1
				SET_FOR_TRIANGLE(2,0,1);
			}
			else if( y2 < y1 )
			{//0 2 1
				SET_FOR_TRIANGLE(0,2,1);
			}
			else
			{//0 1 2
				SET_FOR_TRIANGLE(0,1,2);
			}
		}
		else
		{
			if( y2 < y1 )
			{//2 1 0 
				SET_FOR_TRIANGLE(2,1,0);
			}
			else if( y2 < y0 )
			{//1 2 0
				SET_FOR_TRIANGLE(1,2,0);
			}
			else
			{//1 0 2
				SET_FOR_TRIANGLE(1,0,2);
			}

		}
	break;
/*	case 1://12一致

		SET_FOR_TRIANGLE_MINOR(1,2,0);
	break;
	case 2://20一致
		SET_FOR_TRIANGLE_MINOR(2,0,1);
	break;
	case 4://01一致
		SET_FOR_TRIANGLE_MINOR(0,1,2);
	break;
*/	default:
		return ;
	}
int i,ix,iy;

BMPD *Psfc,*Psfcsenv;


for(i=0 ; i<2 ; i++)
{
	if(i==1 || !sameheight)
	{
//ｙ方向クリッピング
int ry0,ry1;
	ry0 = leng[i] +(i==1);
	ry1 = leng[i+1]+(i==0) ;
//下のクリッピング（範囲を縮めるだけ！）
	ry1 = min( ry1 , sfc[isfc].clipy+sfc[isfc].clipheight ) ;
//上のクリッピング（左右の現在位置を、補正分×傾き足してやらないといけない）
	if( ry0 < sfc[isfc].clipy )
	{
		leftppos[i] += leftslope[i]*( sfc[isfc].clipy-ry0 ) ;
		rightppos[i] += rightslope[i]*( sfc[isfc].clipy-ry0 ) ;
		ry0 = sfc[isfc].clipy ;
	}
//結局の、処理高さ
int finexp=ry1-ry0;
//ごまかし幅拡張
	rightppos[i]+=FILL_WIDTH ;
	leftppos[i]+=FILL_WIDTH ;
//処理を３つに分ける。クリップ必要なし、あり、そして、処理しない、である。
bool issideclip=false;
int tmpx ;
	//左側で、処理終了時の位置
	tmpx = F2IFTRI(leftppos[i]+leftslope[i]*(finexp-1)) ;//１低い位置
	//左側の移動前後の両位置が、クリップより右なら処理しない
	if( min(F2IFTRI(leftppos[i]),tmpx)
		>= sfc[isfc].clipx+sfc[isfc].clipwidth )continue;
	//はみだしがあったら、issideclipをtrueに
	if( 
		F2IFTRI(leftppos[i]) < sfc[isfc].clipx ||
		F2IFTRI(leftppos[i]) >= sfc[isfc].clipx+sfc[isfc].clipwidth ||
		tmpx < sfc[isfc].clipx ||
		tmpx >= sfc[isfc].clipx+sfc[isfc].clipwidth 
		)issideclip=true;
	//右側
	tmpx = F2IFTRI(rightppos[i]+rightslope[i]*(finexp-1)) ;//１低い位置
	//右翼と、右翼の移動後の位置のより右側が、クリップより左なら処理しない
	if( max(F2IFTRI(rightppos[i]),tmpx)
		< sfc[isfc].clipx )continue;
	//はみだしがあったら、issideclipをtrueに
	if( 
		F2IFTRI(rightppos[i]) < sfc[isfc].clipx ||
		F2IFTRI(rightppos[i]) >= sfc[isfc].clipx+sfc[isfc].clipwidth ||
		tmpx < sfc[isfc].clipx ||
		tmpx >= sfc[isfc].clipx+sfc[isfc].clipwidth 
		)issideclip=true;





		Psfc = &sfc[isfc].data[ry0<<sfc[isfc].width_bits]  ;
//*
RenderLoopForTriangle(BL_lightblend,CK_none,SE_none);
/*/
//////////////////////////////////////////////////////////////////////////////////////////
switch(opt->flag & (MDO3F_BLEND | MDO3F_LIGHT_BLEND | MDO3F_DARK_BLEND | MDO3F_OR_BLEND | MDO3F_AND_BLEND )){
case MDO3F_BLEND:
if((opt->alpha&0xF0)==0)return;
if((opt->alpha&0xF0)!=0xF0){
DWORD alphaselector= ((opt->alpha)&0xF0)<<(-4+16);
BMPD *AT     = &PAlphaTable[ alphaselector ] ;
BMPD *AT_inv = &PAlphaTable[ alphaselector^(0x0F<<16) ] ;
RenderLoopForTriangle(BL_blend,CK_none,SE_none);break;}
case 0:
RenderLoopForTriangle(BL_none,CK_none,SE_none);break;
case MDO3F_LIGHT_BLEND:
RenderLoopForTriangle(BL_lightblend,CK_none,SE_none);break;
case MDO3F_DARK_BLEND:
RenderLoopForTriangle(BL_darkblend,CK_none,SE_none);break;
case MDO3F_OR_BLEND:
RenderLoopForTriangle(BL_or,CK_none,SE_none);break;
case MDO3F_AND_BLEND:
RenderLoopForTriangle(BL_and,CK_none,SE_none);break;
default:return;}
//////////////////////////////////////////////////////////////////////////////////////////
//*/
		if( rightconnect )
		{
			rightppos[1] = rightppos[0];
//			assert( !fixfillR ) ;
		}
		if( leftconnect )
		{
			leftppos[1] = leftppos[0];
//			assert( !fixfillL ) ;
		}
	}
}
	return ;
}

#endif








#include <windows.h>
#include "myDIBobj3.h"
#include "myDIBobj3_p.h"
/*
	三角形を描画する関数。
	方針は、myDIBobj3_Cls.cppに書いてみた。

	……２点を同じ座標に指定して、１点だけ違うとき、２つの三角形はぴったり繋がらないといけないのだが、
	その２点が水平に近いとき危ないんだっけな……
	水平に近いなんていうのはザラにあることに思えるがなぁ。
	っていうか、いまこのソース見たが、なにやってんだか全くわからんかった……

	少し弄って、誤魔化して穴は空かないようにした。
	が、代わりに少々広範囲を書くようにしたため、半透明を重ねると……
	それだけじゃなく、なんか変な出っ張りが生じる……
	さらに弄って、縦方向に強制的に１大きくするようにした……
	あまりに顕著……
*/

//固定小数ForTRIangleらしい
#define UBITSFTRI			14
#define	D2FFTRI(num)		((INT32)((num)*(1<<UBITSFTRI)))
#define F2IFTRI(num)		((num)>>UBITSFTRI)










//２点水平時の穴あき防止用ごまかし拡張幅の遊び
//（穴が開くよりは重なった方が……という発想）
#define FILL_WIDTH			0

#define SET_FOR_TRIANGLE(n1,n2,n3)								;\
	leftppos[0] = rightppos[0] = D2FFTRI(x##n1) ;				\
	leftppos[1]  = D2FFTRI(x##n2);								\
	rightppos[1] = D2FFTRI(ELINE_GETX(&line[n2],y##n2));		\
	leng[0] = (int)y##n1;										\
	leng[1] = (int)y##n2;										\
	leng[2] = (int)y##n3;										\
	if( leftppos[1] < rightppos[1] )							\
	{															\
		leftslope[0] = D2FFTRI(line[n3].slope) ;				\
		rightslope[0] = D2FFTRI(line[n2].slope) ;				\
		leftslope[1] = D2FFTRI(line[n1].slope) ;				\
		rightslope[1] = D2FFTRI(line[n2].slope) ;				\
		rightconnect = true;									\
	}															\
	else														\
	{															\
		MYSWAP( &leftppos[1] , &rightppos[1] ) ;				\
		leftslope[0] = D2FFTRI(line[n2].slope) ;				\
		rightslope[0] = D2FFTRI(line[n3].slope) ;				\
		leftslope[1] = D2FFTRI(line[n2].slope) ;				\
		rightslope[1] = D2FFTRI(line[n1].slope) ;				\
		leftconnect = true;										\
	}

#define		SET_FOR_TRIANGLE_MINOR(n1,n2,dp)			;\
		if(y##n1 < y##dp )								\
		{												\
			leng[1] = (int)(y##n1);						\
			leng[2] = (int)(y##dp);						\
			if( x##n1 < x##n2 )								\
			{												\
				leftppos[1] = D2FFTRI(x##n1);				\
				rightppos[1] = D2FFTRI(x##n2);				\
				leftslope[1] = D2FFTRI(line[n2].slope);		\
				rightslope[1] = D2FFTRI(line[n1].slope);	\
			}												\
			else											\
			{												\
				leftppos[1] = D2FFTRI(x##n2);				\
				rightppos[1] = D2FFTRI(x##n1);				\
				leftslope[1] = D2FFTRI(line[n1].slope);		\
				rightslope[1] = D2FFTRI(line[n2].slope);	\
			}											\
		}												\
		else											\
		{												\
			leng[1] = (int)(y##dp);						\
			leng[2] = (int)(y##n1);						\
			if( x##n1 < x##n2 )								\
			{												\
				leftppos[1] =								\
				rightppos[1] = D2FFTRI(x##dp);				\
				leftslope[1] = D2FFTRI(line[n2].slope);		\
				rightslope[1] = D2FFTRI(line[n1].slope);	\
			}												\
			else											\
			{												\
				leftppos[1] =								\
				rightppos[1] = D2FFTRI(x##dp);				\
				leftslope[1] = D2FFTRI(line[n1].slope);		\
				rightslope[1] = D2FFTRI(line[n2].slope);	\
			}											\
		}

/*
#define		SET_FOR_TRIANGLE_MINOR(n1,n2,dp)			;\
		if(y##n1 < y##dp )								\
		{												\
			leng[1] = (int)y##n1-1;						\
			leng[2] = (int)y##dp+1;						\
			if( x##n1 < x##n2 )								\
			{												\
				leftppos[1] = D2FFTRI(x##n1);				\
				rightppos[1] = D2FFTRI(x##n2);				\
				leftslope[1] = D2FFTRI(line[n2].slope);		\
				rightslope[1] = D2FFTRI(line[n1].slope);	\
			}												\
			else											\
			{												\
				leftppos[1] = D2FFTRI(x##n2);				\
				rightppos[1] = D2FFTRI(x##n1);				\
				leftslope[1] = D2FFTRI(line[n1].slope);		\
				rightslope[1] = D2FFTRI(line[n2].slope);	\
			}											\
			fixfillL = -leftslope[1]*2-D2FFTRI(FILL_WIDTH) ;	\
			fixfillR = -rightslope[1]*2+D2FFTRI(FILL_WIDTH) ;	\
		}												\
		else											\
		{												\
			leng[1] = (int)y##dp-1;						\
			leng[2] = (int)y##n1+1;						\
			if( x##n1 < x##n2 )								\
			{												\
				leftppos[1] =								\
				rightppos[1] = D2FFTRI(x##dp);				\
				leftslope[1] = D2FFTRI(line[n2].slope);		\
				rightslope[1] = D2FFTRI(line[n1].slope);	\
			}												\
			else											\
			{												\
				leftppos[1] =								\
				rightppos[1] = D2FFTRI(x##dp);				\
				leftslope[1] = D2FFTRI(line[n1].slope);		\
				rightslope[1] = D2FFTRI(line[n2].slope);	\
			}											\
			fixfillL = leftslope[1]*2-D2FFTRI(FILL_WIDTH) ;	\
			fixfillR = rightslope[1]*2+D2FFTRI(FILL_WIDTH) ;	\
		}
  */
#define	RenderLoopForTriangle_sub(clipenabled,BLTYPE,CKTYPE,SETYPE) ;\
			for(iy=0 ;iy<finexp;iy++)								\
			{														\
int linewidth =  F2IFTRI(rightppos[i])-F2IFTRI(leftppos[i]) ;		\
				if(clipenabled)										\
				{													\
int tmpx=F2IFTRI(leftppos[i]);										\
					if(tmpx<sfc[isfc].clipx)									\
					{															\
						linewidth -= (sfc[isfc].clipx-tmpx);					\
						tmpx = sfc[isfc].clipx ;								\
					}															\
					if(tmpx+linewidth>sfc[isfc].clipx+sfc[isfc].clipwidth)		\
					{															\
						linewidth = sfc[isfc].clipx+sfc[isfc].clipwidth-tmpx;	\
					}															\
					Psfcsenv = Psfc + tmpx ;									\
				}													\
				else												\
				{													\
					Psfcsenv = Psfc + F2IFTRI(leftppos[i]) ;		\
				}													\
int tw=linewidth - 7 ;												\
				for(ix= 0 ; ix<tw  ; ix += 8 )						\
				{													\
	PixelSet(opt,&Psfcsenv[ix+0],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+1],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+2],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+3],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+4],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+5],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+6],color,BLTYPE,CKTYPE,SETYPE);		\
	PixelSet(opt,&Psfcsenv[ix+7],color,BLTYPE,CKTYPE,SETYPE);		\
				}													\
				for( ; ix<linewidth  ; ix ++ )						\
				{													\
	PixelSet(opt,&Psfcsenv[ix+0],color,BLTYPE,CKTYPE,SETYPE);		\
				}													\
				Psfc += sfc[isfc].width ;							\
				leftppos[i]  +=  leftslope[i] ;						\
				rightppos[i]  +=  rightslope[i] ;					\
			}

#define	RenderLoopForTriangle(BLTYPE,CKTYPE,SETYPE) ;\
		if(issideclip)												\
		{															\
			RenderLoopForTriangle_sub(1,BLTYPE,CKTYPE,SETYPE) ;		\
		}															\
		else														\
		{															\
			RenderLoopForTriangle_sub(0,BLTYPE,CKTYPE,SETYPE) ;		\
		}


void MyDIBObj3 :: Triangle( const MDO3Opt *opt , int isfc , double x0 , double y0 , double x1 , double y1 , double x2 , double y2 , BMPD color )
{
ELINE line[3];
int sameheight=0;
/*
//補正したほうが良い？
	x0 += 0.5 ;
	y0 += 0.5 ;
	x1 += 0.5 ;
	y1 += 0.5 ;
	x2 += 0.5 ;
	y2 += 0.5 ;
//*/
	if( !ELINE_SET( &line[0] , x1 , y1 , x2 , y2 ) )sameheight+=1;	
	if( !ELINE_SET( &line[1] , x2 , y2 , x0 , y0 ) )sameheight+=2;	
	if( !ELINE_SET( &line[2] , x0 , y0 , x1 , y1 ) )sameheight+=4;	
INT32 leftslope[2],rightslope[2];
INT32 leftppos[2],rightppos[2];
//INT32 fixfillL,fixfillR;//穴が開かないようにごまかす幅
bool leftconnect,rightconnect;
int leng[3];
	leftconnect = false;
	rightconnect = false;
	switch( sameheight )
	{
	case 0://一致無し
		//きったねぇやり方……
		if( y0 < y1 )
		{

			if( y2 < y0 )
			{//2 0 1
				SET_FOR_TRIANGLE(2,0,1);
			}
			else if( y2 < y1 )
			{//0 2 1
				SET_FOR_TRIANGLE(0,2,1);
			}
			else
			{//0 1 2
				SET_FOR_TRIANGLE(0,1,2);
			}
		}
		else
		{
			if( y2 < y1 )
			{//2 1 0 
				SET_FOR_TRIANGLE(2,1,0);
			}
			else if( y2 < y0 )
			{//1 2 0
				SET_FOR_TRIANGLE(1,2,0);
			}
			else
			{//1 0 2
				SET_FOR_TRIANGLE(1,0,2);
			}

		}
	break;
	case 1://12一致

		SET_FOR_TRIANGLE_MINOR(1,2,0);
	break;
	case 2://20一致
		SET_FOR_TRIANGLE_MINOR(2,0,1);
	break;
	case 4://01一致
		SET_FOR_TRIANGLE_MINOR(0,1,2);
	break;
	default:
		return ;
	}
int i,ix,iy;

BMPD *Psfc,*Psfcsenv;

for(i=0 ; i<2 ; i++)
{
	if(i==1 || !sameheight)
	{
//ｙ方向クリッピング
int ry0,ry1;
	ry0 = leng[i] ;
	ry1 = leng[i+1] ;
//下のクリッピング（範囲を縮めるだけ！）
	ry1 = min( ry1 , sfc[isfc].clipy+sfc[isfc].clipheight ) ;
//上のクリッピング（左右の現在位置を、補正分×傾き足してやらないといけない）
	if( ry0 < sfc[isfc].clipy )
	{
		leftppos[i] += leftslope[i]*( sfc[isfc].clipy-ry0 ) ;
		rightppos[i] += rightslope[i]*( sfc[isfc].clipy-ry0 ) ;
		ry0 = sfc[isfc].clipy ;
	}
//結局の、処理高さ
int finexp=ry1-ry0;
//ごまかし幅拡張
//	rightppos[i]+=fixfillR ;
//	leftppos[i]+=fixfillL ;
//処理を３つに分ける。クリップ必要なし、あり、そして、処理しない、である。
bool issideclip=false;
int tmpx ;
	//左側
	tmpx = F2IFTRI(leftppos[i]+leftslope[i]*(finexp-1)) ;//１低い位置
	//左翼と、左翼の移動後の位置のより左側が、クリップより右なら処理しない
	if( min(F2IFTRI(leftppos[i]),tmpx)
		>= sfc[isfc].clipx+sfc[isfc].clipwidth )continue;
	//はみだしがあったら、issideclipをtrueに
	if( 
		F2IFTRI(leftppos[i]) < sfc[isfc].clipx ||
		F2IFTRI(leftppos[i]) >= sfc[isfc].clipx+sfc[isfc].clipwidth ||
		tmpx < sfc[isfc].clipx ||
		tmpx >= sfc[isfc].clipx+sfc[isfc].clipwidth 
		)issideclip=true;
	//右側
	tmpx = F2IFTRI(rightppos[i]+rightslope[i]*(finexp-1)) ;//１低い位置
	//右翼と、右翼の移動後の位置のより右側が、クリップより左なら処理しない
	if( max(F2IFTRI(rightppos[i]),tmpx)
		< sfc[isfc].clipx )continue;
	//はみだしがあったら、issideclipをtrueに
	if( 
		F2IFTRI(rightppos[i]) < sfc[isfc].clipx ||
		F2IFTRI(rightppos[i]) >= sfc[isfc].clipx+sfc[isfc].clipwidth ||
		tmpx < sfc[isfc].clipx ||
		tmpx >= sfc[isfc].clipx+sfc[isfc].clipwidth 
		)issideclip=true;





		Psfc = &sfc[isfc].data[ry0<<sfc[isfc].width_bits]  ;
//*
RenderLoopForTriangle(BL_none,CK_none,SE_none);
/*/
//////////////////////////////////////////////////////////////////////////////////////////
switch(opt->flag & (MDO3F_BLEND | MDO3F_LIGHT_BLEND | MDO3F_DARK_BLEND | MDO3F_OR_BLEND | MDO3F_AND_BLEND )){
case MDO3F_BLEND:
if((opt->alpha&0xF0)==0)return;
if((opt->alpha&0xF0)!=0xF0){
DWORD alphaselector= ((opt->alpha)&0xF0)<<(-4+16);
BMPD *AT     = &PAlphaTable[ alphaselector ] ;
BMPD *AT_inv = &PAlphaTable[ alphaselector^(0x0F<<16) ] ;
RenderLoopForTriangle(BL_blend,CK_none,SE_none);break;}
case 0:
RenderLoopForTriangle(BL_none,CK_none,SE_none);break;
case MDO3F_LIGHT_BLEND:
RenderLoopForTriangle(BL_lightblend,CK_none,SE_none);break;
case MDO3F_DARK_BLEND:
RenderLoopForTriangle(BL_darkblend,CK_none,SE_none);break;
case MDO3F_OR_BLEND:
RenderLoopForTriangle(BL_or,CK_none,SE_none);break;
case MDO3F_AND_BLEND:
RenderLoopForTriangle(BL_and,CK_none,SE_none);break;
default:return;}
//////////////////////////////////////////////////////////////////////////////////////////
//*/
		if( rightconnect )
		{
			rightppos[1] = rightppos[0];
//			assert( !fixfillR ) ;
		}
		if( leftconnect )
		{
			leftppos[1] = leftppos[0];
//			assert( !fixfillL ) ;
		}
	}
}
	return ;
}

