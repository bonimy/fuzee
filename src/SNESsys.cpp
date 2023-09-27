#include <windows.h>

static BYTE looptable[3] = {2,4,8};
//static BYTE datasizetable[3] = {2,4,8};
static BYTE bittable[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
static BYTE bittabler[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

static UINT *Pcolortable4 ;
static UINT *Pcolortable16 ;
static UINT *Pcolortable256 ;
static void (*PCLSFUNK)(int sfc , int x , int y , int w , int h , UINT color );

void SetSNESPalette( UINT *Pict4 , UINT *Pict16 , UINT *Pict256 )
{
	Pcolortable4  = Pict4 ;
	Pcolortable16 = Pict16 ;
	Pcolortable256 = Pict256 ;
}

void SetSNESDrawRoutine( void (*PiCLSFUNK)(int,int,int,int,int,UINT) , UINT *Pict4 , UINT *Pict16 , UINT *Pict256 )
{
	PCLSFUNK      = PiCLSFUNK ;
	SetSNESPalette( Pict4 , Pict16 , Pict256 ) ;
}


void DrawSNESCharacter( BYTE *Pbuf , int sfc , int x , int y , int mode , bool emphasis , int zoom)
{
BYTE tmp[8];
int ix,iy,q;
UINT tcolor;
//	if( mode < 0 || mode >= 3 )return;
	for( iy=0 ; iy<8 ; iy++ )
	{
		for( ix=0 ; ix<8 ; ix++ )tmp[ix] = 0;
		for( ix=0 ; ix<8 ; ix++ )
		{
			if( mode != 2 )
			{
				for( q=0 ; q<looptable[mode] ; q++ )
				{
					if( Pbuf[(q%2)+q/2*16+iy*2]&bittable[ix] )tmp[ix] |= bittabler[q];
				}//q%2+q/2*16+iy*2
			}
			else
			{
					tmp[ix] = Pbuf[1+(iy*8+ix)*2] ;
			}
		}
		for( ix=0 ; ix<8 ; ix++ )
		{
			tcolor = tmp[ix] ;
			if( emphasis )
			{
				switch( mode )
				{
				case 0:
					tcolor = Pcolortable4[tcolor] ;
				break;
				case 1:
					tcolor = Pcolortable16[tcolor] ;
				break;
				case 2:
					tcolor = Pcolortable256[tcolor] ;
				break;
				}
			}
			PCLSFUNK( sfc , x+ix*zoom , y+iy*zoom , zoom , zoom , tcolor ) ;
		}
	}
}