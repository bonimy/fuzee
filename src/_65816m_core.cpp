#include <stdio.h>
#include <assert.h>
#include "_65816m_local.h"

/*
　_65816から始まるファイルは、少し前に無理やり作ってみたエミュもどき。
　トレイサーとでもいう？
　なんでクラス継承させているのかとか意味不明。
　もう、恐ろしくて中身を見る気もしない。
　あちこちで凄惨なことになっているが、もう気にしない何も見えない聞こえない
*/

static union
{
	WORD w;
	struct
	{
		BYTE l;
		BYTE h;
	}b;
}tmp;


////////////////////////////////////////////////////////////////

inline static void LDA( DataSet *Pd , WORD data )
{
	if( IS8A )
	{
		Pd->reg.a.b.l = (BYTE)data;
		TESTA8();
	}
	else
	{
		Pd->reg.a.w = data;
		TESTA16();
	}
}
inline static void LDX( DataSet *Pd , WORD data )
{
	if( IS8X )
	{
		Pd->reg.x.b.l = (BYTE)data;
		TESTX8();
	}
	else
	{
		Pd->reg.x.w = data;
		TESTX16();
	}
}
inline static void LDY( DataSet *Pd , WORD data )
{
	if( IS8Y )
	{
		Pd->reg.y.b.l = (BYTE)data;
		TESTY8();
	}
	else
	{
		Pd->reg.y.w = data;
		TESTY16();
	}
}
inline static void STA( DataSet *Pd , int adr  )
{
	WRITEA( Pd , adr , Pd->reg.a.w );
}
inline static void STX( DataSet *Pd , int adr  )
{
	WRITEX( Pd , adr , Pd->reg.x.w );
}
inline static void STY( DataSet *Pd , int adr  )
{
	WRITEY( Pd , adr , Pd->reg.y.w );
}
inline static void STZ( DataSet *Pd , int adr )
{
	WRITEA( Pd , adr , 0 );
	//たぶん、アキュムレータのビット数に関係すると思ったのだが……
}
inline static void INC( DataSet *Pd , int adr )
{
WORD tw;
	tw = READA( Pd , adr );
	tw++;
	WRITEA( Pd , adr , tw );
	TESTNUM(tw);
}
inline static void INCA( DataSet *Pd )
{
	if( IS8A )
	{
		Pd->reg.a.b.l ++;
		TESTA();
		if( Pd->reg.a.b.l == 0x00 )SET_F(FLAG_C);//要らないかも
	}
	else
	{
		Pd->reg.a.w ++;
		TESTA();
		if( Pd->reg.a.b.l == 0x0000 )SET_F(FLAG_C);//要らないかも
	}
}

inline static void DEC( DataSet *Pd , int adr )
{
WORD tw;
	tw = READA( Pd , adr );
	tw--;
	WRITEA( Pd , adr , tw );
	if(IS8A)
	{
		TEST8((BYTE)tw);
	}
	else
	{
		TEST16(tw);
	}
}
inline static void INX( DataSet *Pd)
{
	if(IS8X)Pd->reg.x.b.l++;
	else Pd->reg.x.w++;
	TESTX();
}
inline static void INY( DataSet *Pd)
{
	if(IS8X)Pd->reg.y.b.l++;
	else Pd->reg.y.w++;
	TESTY();
}
inline static void DEX( DataSet *Pd)
{
	if(IS8X)Pd->reg.x.b.l--;
	else Pd->reg.x.w--;
	TESTX();
}
inline static void DEY( DataSet *Pd)
{
	if(IS8Y)Pd->reg.y.b.l--;
	else Pd->reg.y.w--;
	TESTY();
}
inline static void CMP( DataSet *Pd , WORD data)
{
	PCLEAR;
	CLR_F( FLAG_C|FLAG_Z|FLAG_N);
int ti;
	if( IS8A )
	{
		ti = Pd->reg.a.b.l - (BYTE)data ;
		if( ti>=0 )SET_F(FLAG_C);
		if( ti==0 )SET_F(FLAG_Z);
		if( (BYTE)ti&0x80 )SET_F(FLAG_N);
	}
	else
	{
		ti = Pd->reg.a.w - data ;
		if( ti>=0 )SET_F(FLAG_C);
		if( ti==0 )SET_F(FLAG_Z);
		if( (WORD)ti&0x8000 )SET_F(FLAG_N);
	}
}
inline static void CPX( DataSet *Pd , WORD data)
{
	PCLEAR;
	CLR_F( FLAG_C );
int ti;
	if( IS8X )
	{
		ti = Pd->reg.x.b.l - (BYTE)data ;
		if( ti>=0 )SET_F(FLAG_C);
		if( ti==0 )SET_F(FLAG_Z);
		if( ti < 0 )SET_F(FLAG_N);
	}
	else
	{
		ti = Pd->reg.x.w - data ;
		if( ti>=0 )SET_F(FLAG_C);
		if( ti==0 )SET_F(FLAG_Z);
		if( ti < 0 )SET_F(FLAG_N);
	}
}
inline static void CPY( DataSet *Pd , WORD data)
{
	PCLEAR;
	CLR_F( FLAG_C );
	int ti;
	if( IS8Y )
	{
		ti = Pd->reg.y.b.l - (BYTE)data ;
		if( ti>=0 )SET_F(FLAG_C);
		if( ti==0 )SET_F(FLAG_Z);
		if( ti < 0 )SET_F(FLAG_N);
	}
	else
	{
		ti = Pd->reg.y.w - data ;
		if( ti>=0 )SET_F(FLAG_C);
		if( ti==0 )SET_F(FLAG_Z);
		if( ti < 0 )SET_F(FLAG_N);
	}
}
inline static void ADC( DataSet *Pd , WORD data )
{
	PCLEAR;
BYTE fl1=0;
long tl;
	if(Pd->reg.p&FLAG_C)fl1=1;
	//ここでキャリーをクリアすべきか？
	CLR_F(FLAG_C);
	if( IS8A )
	{
		tl = (WORD)Pd->reg.a.b.l + data + fl1 ;
		if( tl >= 0x0100)SET_F(FLAG_C);
		if( tl==0 )SET_F(FLAG_Z);
		//オーバーフローは知らない
		Pd->reg.a.b.l = (BYTE)tl;
	}
	else
	{
		tl = (WORD)Pd->reg.a.w + data + fl1 ;
		if( tl >= 0x010000)SET_F(FLAG_C);
		if( tl==0 )SET_F(FLAG_Z);
		//オーバーフローは知らない
		Pd->reg.a.w = (WORD)tl;
	}
}
inline static void SBC( DataSet *Pd , WORD data )
{
BYTE c1;
	c1=1;
	if(Pd->reg.p&FLAG_C)c1=0;
	PCLEAR;
	CLR_F(FLAG_C);
	if( IS8A )
	{
		if(Pd->reg.a.b.l >= data+c1 )SET_F(FLAG_C);
		if(Pd->reg.a.b.l == data+c1 )SET_F(FLAG_Z);
		//オーバーフローは知らない
		Pd->reg.a.b.l-= (data+c1);
	}
	else
	{
		if(Pd->reg.a.w >= data+c1 )SET_F(FLAG_C);
		if(Pd->reg.a.w == data+c1 )SET_F(FLAG_Z);
		//オーバーフローは知らない
		Pd->reg.a.w -= (data+c1);
	}
}
inline static void ASLm( DataSet *Pd , BYTE *Ptr)
{
	CLR_F(FLAG_C);
	if( IS8A )
	{
		if( *Ptr & 0x80 )SET_F(FLAG_C);
		*Ptr <<= 1;
	}
	else
	{
		if( *Ptr & 0x80 )SET_F(FLAG_C);
		*(Ptr+1) <<= 1;
		*(Ptr+1) |= !!(*(Ptr+0) & 0x80);
		*(Ptr+0) <<= 1;
	}
}
inline static void LSRm( DataSet *Pd , BYTE *Ptr)
{
	CLR_F(FLAG_C);
	if( IS8A )
	{
		if( *Ptr & 0x01 )SET_F(FLAG_C);
		*Ptr >>= 1;
	}
	else
	{
		if( *(Ptr+1) & 0x01 )SET_F(FLAG_C);
		*(Ptr+0) >>= 1;
		*(Ptr+0) |= (*(Ptr+1) & 0x01)*0x80;
		*(Ptr+1) >>= 1;
	}
}
inline static void ROL( DataSet *Pd , BYTE *Ptr )
{
BYTE c1;
	c1 = 0;
	if( Pd->reg.p & FLAG_C ) c1 = 1;
	CLR_F(FLAG_C);
	if( IS8A )
	{
		if( *Ptr & 0x80 )SET_F(FLAG_C);
		*Ptr <<= 1;
		*Ptr |= c1;
	}
	else
	{
		if( *Ptr & 0x80 )SET_F(FLAG_C);
		*(Ptr+1) <<= 1;
		*(Ptr+1) |= !!(*(Ptr+0) & 0x80);
		*(Ptr+0) <<= 1;
		*(Ptr+0) |= c1;
	}

}
inline static void RORa( DataSet *Pd )
{
BYTE c1;
	c1 = 0;
	if( Pd->reg.p & FLAG_C ) c1 = 0x80;
	CLR_F(FLAG_C);
	if( IS8A )
	{

		if( Pd->reg.a.b.l & 0x01 )SET_F(FLAG_C);
		Pd->reg.a.b.l >>= 1;
		Pd->reg.a.b.l |= c1;
	}
	else
	{
		if( Pd->reg.a.b.l & 0x01 )SET_F(FLAG_C);
		Pd->reg.a.b.l >>= 1;
		Pd->reg.a.b.l |= (Pd->reg.a.b.h & 0x01)*0x80 ;
		Pd->reg.a.b.h >>= 1;
		Pd->reg.a.b.h |= c1;
	}

}
inline static void ROR( DataSet *Pd , BYTE *Ptr)
{
BYTE c1;
	c1 = 0;
	if( Pd->reg.p & FLAG_C ) c1 = 0x80;
	CLR_F(FLAG_C);
	if( IS8A )
	{

		if( *Ptr & 0x01 )SET_F(FLAG_C);
		*Ptr >>= 1;
		*Ptr |= c1;
	}
	else
	{
		if( *(Ptr+1) & 0x01 )SET_F(FLAG_C);
		*(Ptr+0) >>= 1;
		*(Ptr+0) |= (*(Ptr+1) & 0x01)*0x80;
		*(Ptr+1) >>= 1;
		*(Ptr+1) |= c1;
	}
}
inline static void ORA( DataSet *Pd , WORD data )
{
	if( IS8A )
	{
		Pd->reg.a.b.l |= (BYTE)data;
	}
	else
	{
		Pd->reg.a.w |= (WORD)data;
	}
	TESTA();
}
inline static void AND( DataSet *Pd , WORD data )
{
	if( !IS16A )
	{
		Pd->reg.a.b.l &= (BYTE)data;
	}
	else
	{
		Pd->reg.a.w &= (WORD)data;
	}
	TESTA();
}
inline static void EOR( DataSet *Pd , WORD data )
{
	if( IS8A )
	{
		Pd->reg.a.b.l ^= (BYTE)data;
	}
	else
	{
		Pd->reg.a.w ^= (WORD)data;
	}
	TESTA();
}
inline static void TRB( DataSet *Pd , int adr )
{
WORD tw;
	if( IS8A )
	{
		tw = READA( Pd , adr );
		tw &= ~Pd->reg.a.b.l;
		WRITEA( Pd , adr , (BYTE)tw );
	}
	else
	{
		tw = READA( Pd , adr );
		tw &= ~Pd->reg.a.w;
		WRITEA( Pd , adr , tw );
	}
	TESTA();
}
inline static void BIT( DataSet *Pd , WORD data )
{
	if( IS8A )
	{
		data &= 0x00FF;
		data &= Pd->reg.a.b.l ;
	}
	else
	{
		data &= Pd->reg.a.w;
	}
	TESTNUM(data);
}
inline static void TSB( DataSet *Pd , int adr )
{
WORD tw;
	if( IS8A )
	{
		tw = READA( Pd , adr );
		tw |= Pd->reg.a.b.l;
		WRITEA( Pd , adr , (BYTE)tw );
	}
	else
	{
		tw = READA( Pd , adr );
		tw |= Pd->reg.a.w;
		WRITEA( Pd , adr , tw );
	}
	TESTA();
}

inline static void SEP( DataSet *Pd , BYTE data )
{
	Pd->reg.p |= data;
	if( IS8X )
	{
		Pd->reg.x.b.h = 0;
		Pd->reg.y.b.h = 0;
	}
}
inline static void REP( DataSet *Pd , BYTE data )
{
	Pd->reg.p &= ~data ;
	if( IS8X )
	{
		Pd->reg.x.b.h = 0;
		Pd->reg.y.b.h = 0;
	}
}
inline static void XBA( DataSet *Pd )
{
BYTE tb=Pd->reg.a.b.l;
	Pd->reg.a.b.l = Pd->reg.a.b.h ;
	Pd->reg.a.b.h = tb;
}
inline static void PHA(DataSet *Pd )
{
	if( IS8A )
	{
		Pd->Pstack[Pd->reg.s  ] = Pd->reg.a.b.l;
		Pd->reg.s++;
	}
	else
	{
		Pd->Pstack[Pd->reg.s  ] = Pd->reg.a.b.h;
		Pd->Pstack[Pd->reg.s+1] = Pd->reg.a.b.l;
		Pd->reg.s+=2;
	}
}
inline static void PLA( DataSet *Pd )
{
	if( IS8A )
	{
		Pd->reg.a.b.l = Pd->Pstack[Pd->reg.s-1];
		Pd->reg.s--;
	}
	else
	{
		Pd->reg.a.b.h = Pd->Pstack[Pd->reg.s-2] ;
		Pd->reg.a.b.l = Pd->Pstack[Pd->reg.s-1] ;
		Pd->reg.s-=2;
	}
	TESTA();
}
inline static void JSR2( DataSet *Pd , int adr )
{
//	Pd->Pstack[Pd->reg.s  ] = Pd->reg.pc&0xFF;
//	Pd->Pstack[Pd->reg.s+1] = Pd->reg.pc>>8;
	Pd->reg.pc--;
	Pd->Pstack[Pd->reg.s  ] = Pd->reg.pc>>8;
	Pd->Pstack[Pd->reg.s+1] = Pd->reg.pc&0xFF;
	Pd->reg.s+=2;
	Pd->reg.pc = adr ;
}
inline static void JSR3( DataSet *Pd , int adr )
{
//	Pd->Pstack[Pd->reg.s  ] = Pd->reg.pc&0xFF;
//	Pd->Pstack[Pd->reg.s+1] = Pd->reg.pc>>8;
//	Pd->Pstack[Pd->reg.s+2] = Pd->reg.pb;
	Pd->reg.pc--;
	Pd->Pstack[Pd->reg.s  ] = Pd->reg.pb;
	Pd->Pstack[Pd->reg.s+1] = Pd->reg.pc>>8;
	Pd->Pstack[Pd->reg.s+2] = Pd->reg.pc&0xFF;
	Pd->reg.s+=3;
	Pd->reg.pc = adr &0xFFFF;
	Pd->reg.pb = adr>>16;
}
inline static void RTL( DataSet *Pd )
{
//	Pd->reg.pb = Pd->Pstack[Pd->reg.s-1];
//	Pd->reg.pc = (Pd->Pstack[Pd->reg.s-2] << 8 )| Pd->Pstack[Pd->reg.s-3] ;
	if( Pd-> reg.s <= 2 )
	{
		Pd->reg.pc--;
		return;
	}
	Pd->reg.pb = Pd->Pstack[Pd->reg.s-3];
	Pd->reg.pc = (Pd->Pstack[Pd->reg.s-2] << 8 )| Pd->Pstack[Pd->reg.s-1] ;
	Pd->reg.pc++;
	Pd->reg.s -= 3;
}
inline static void RTS( DataSet *Pd )
{
//	Pd->reg.pc = (Pd->Pstack[Pd->reg.s-1] << 8 )| Pd->Pstack[Pd->reg.s-2] ;
	if( Pd-> reg.s <= 1 )
	{
		Pd->reg.pc--;
		return;
	}
	Pd->reg.pc = (Pd->Pstack[Pd->reg.s-2] << 8 )| Pd->Pstack[Pd->reg.s-1] ;
	Pd->reg.pc++;
	Pd->reg.s -= 2;
}
inline static bool JMP( DataSet *Pd , int adr )
{
	if( (adr&0xFFFF) < 0x8000 )
		return false; 

	Pd->reg.pc = adr &0xFFFF;
	Pd->reg.pb = adr>>16;
	return true;
}

inline static void BlockCopy( DataSet *Pd , BYTE dest , BYTE src , int ID )
{
int destl,srcl;
	destl = dest<<16;
	srcl = src<<16;
	for(;;)
	{
		TESTA();
		if( IS_SET_F( FLAG_N ) )break;
		if( IS8X )
		{
			WRITEB( Pd , 
				destl|Pd->reg.y.b.l , 
				READB( Pd , srcl|Pd->reg.x.b.l ) ) ;
			Pd->reg.x.b.l += ID ;
			Pd->reg.y.b.l += ID ;
		}
		else
		{
			WRITEB( Pd , 
				destl|Pd->reg.y.w , 
				READB( Pd , srcl|Pd->reg.x.w ) ) ;
			Pd->reg.x.w += ID ;
			Pd->reg.y.w += ID ;
		}
		if( IS8A )
			Pd->reg.a.b.l--;
		else
			Pd->reg.a.w --;
	}
}

/*
どう考えてもＰとＮが逆の気がするのだが……
*/
inline static void MVP( DataSet *Pd , BYTE dest , BYTE src )
{
	BlockCopy( Pd , dest , src , -1 );
}
inline static void MVN( DataSet *Pd , BYTE dest , BYTE src )
{
	BlockCopy( Pd , dest , src ,  1 );
}

///////////////////////////////////////////////////////////////////////








//AのサイズのImmediate値
inline static WORD ImmediateA( DataSet *Pd )
{
	if( IS8A )
	{
		tmp.b.l = GetROMinPCinc( Pd );
	}
	else
	{
		tmp.b.l = GetROMinPCinc( Pd );
		tmp.b.h = GetROMinPCinc( Pd );
	}
	return tmp.w;
}
//XのサイズのImmediate値
inline static WORD ImmediateX( DataSet *Pd )
{
	if( IS8X )
	{
		tmp.b.l = GetROMinPCinc( Pd );
	}
	else
	{
		tmp.b.l = GetROMinPCinc( Pd );
		tmp.b.h = GetROMinPCinc( Pd );
	}
	return tmp.w;
}
//YのサイズのImmediate値（Xと同じ）
inline static WORD ImmediateY( DataSet *Pd )
{
	return ImmediateX( Pd );
}
inline static BYTE Immediate8( DataSet *Pd )
{
	return GetROMinPCinc( Pd );
}
inline static WORD Immediate16( DataSet *Pd )
{
WORD tw=GetROMinPCinc( Pd );
	return tw|(GetROMinPCinc( Pd )<<8);
}
inline static int Immediate24( DataSet *Pd )
{
int tw=GetROMinPCinc( Pd );
	tw |= (GetROMinPCinc( Pd )<<8);
	return tw|(GetROMinPCinc( Pd )<<16);
}
inline static int Zeropage( DataSet *Pd )
{
	return GetROMinPCinc( Pd );
}
inline static int ZeropageIndexX( DataSet *Pd )
{
	if(IS8X)
		return GetROMinPCinc( Pd )+Pd->reg.x.b.l;
	else
		return GetROMinPCinc( Pd )+Pd->reg.x.w;
}
inline static int ZeropageIndexY( DataSet *Pd )
{
	if(IS8X)
		return GetROMinPCinc( Pd )+Pd->reg.y.b.l;
	else
		return GetROMinPCinc( Pd )+Pd->reg.y.w;
}
inline static int AbsoluteAddress( DataSet *Pd )
{
int ti;
	ti = GetROMinPCinc( Pd ) | (Pd->reg.db<<16);
	ti |= GetROMinPCinc( Pd ) << 8;
	return ti;
}
inline static int AbsoluteAddressIndexX( DataSet *Pd )
{
int ti;
	ti = GetROMinPCinc( Pd ) | (Pd->reg.db<<16);
	ti |= GetROMinPCinc( Pd ) << 8;
	if(IS8X)
		return ti+Pd->reg.x.b.l;
	else
		return ti+Pd->reg.x.w;
}
inline static int AbsoluteAddressIndexY( DataSet *Pd )
{
int ti;
	ti = GetROMinPCinc( Pd ) | (Pd->reg.db<<16);
	ti |= GetROMinPCinc( Pd ) << 8;
	if(IS8X)
		return ti+Pd->reg.y.b.l;
	else
		return ti+Pd->reg.y.w;
}
inline static int AbsoluteIndirect( DataSet *Pd , int offset=0 )
{
int ti;
	ti = GetROMinPCinc( Pd ) /*| (Pd->reg.pb<<16)*/;
	ti |= GetROMinPCinc( Pd ) << 8;
	return READW( Pd , ti + offset ) | (Pd->reg.pb<<16);//解釈にかなり問題がありそう
}
inline static int AbsoluteIndirectX( DataSet *Pd )
{
	if(IS8X)
		return AbsoluteIndirect(Pd,Pd->reg.x.b.l);
	else
		return AbsoluteIndirect(Pd,Pd->reg.x.w);
}

inline static int AbsoluteInderectLong( DataSet *Pd )
{
int ti,ti2;
	ti = GetROMinPCinc( Pd ) ;
	ti |= GetROMinPCinc( Pd ) << 8;
	ti2 = READW( Pd , ti );
	return ti2|(READB(Pd,ti2+2)<<16);
}
inline static int AbsoluteLongAddress( DataSet *Pd )
{
int ti;
	ti = GetROMinPCinc( Pd );
	ti |= GetROMinPCinc( Pd ) << 8;
	ti |= GetROMinPCinc( Pd ) << 16;
	return ti;
}
inline static int AbsoluteLongAddressIndexX( DataSet *Pd )
{
	if(IS8X)
		return AbsoluteLongAddress(Pd)+Pd->reg.x.b.l;
	else
		return AbsoluteLongAddress(Pd)+Pd->reg.x.w;
}


inline static WORD Direct( DataSet *Pd , int offset=0)
{
	return READW(Pd,GetROMinPCinc( Pd )+Pd->reg.d+offset);
}
inline static WORD DirectIndexX( DataSet *Pd )
{
	if(IS8X)
		return Direct(Pd,Pd->reg.x.b.l);
	else
		return Direct(Pd,Pd->reg.x.w);
}
inline static WORD DirectIndexY( DataSet *Pd )
{
	if(IS8Y)
		return Direct(Pd)+Pd->reg.y.b.l;
	else
		return Direct(Pd)+Pd->reg.y.w;
}
inline static int DirectIndirect( DataSet *Pd , int offset=0)
{
int ti;
	ti=GetROMinPCinc(Pd);
	return READW(Pd,ti+offset+Pd->reg.d) | (Pd->reg.db<<16);
}
inline static int DirectIndirectX( DataSet *Pd )
{
	if(IS8X)
		return DirectIndirect(Pd,+Pd->reg.x.b.l);
	else
		return DirectIndirect(Pd,+Pd->reg.x.w);
}
inline static int DirectIndirectY( DataSet *Pd )
{
	if(IS8Y)
		return DirectIndirect(Pd)+Pd->reg.y.b.l;
	else
		return DirectIndirect(Pd)+Pd->reg.y.w;
}
inline static int DirectIndirectLong( DataSet *Pd )
{
int ti,ti2;
	ti = GetROMinPCinc( Pd )+Pd->reg.d;
	ti2=READW( Pd , ti );
	ti2|=READB( Pd , ti+2)<<16 ;
	return ti2;

}
inline static int DirectIndirectLongY( DataSet *Pd )
{
	if(IS8Y)
		return DirectIndirectLong(Pd)+Pd->reg.y.b.l;
	else
		return DirectIndirectLong(Pd)+Pd->reg.y.w;
}
inline static int StackRelative( DataSet *Pd )
{
	return 0;
}
inline static int StackRelativeIndirectY( DataSet *Pd )
{
	return 0;
}


#include "_65816m_op0123.h"
#include "_65816m_op4567.h"
#include "_65816m_op89AB.h"
#include "_65816m_opCDEF.h"









/*
static bool (*PAFuncPerOp[0x0100])(DataSet* Pd)=
{
	OP00,OP01,OP02,OP03,OP04,OP05,OP06,OP07,OP08,OP09,OP0A,OP0B,OP0C,OP0D,OP0E,OP0F,
	OP10,OP11,OP12,OP13,OP14,OP15,OP16,OP17,OP18,OP19,OP1A,OP1B,OP1C,OP1D,OP1E,OP1F,
	OP20,OP21,OP22,OP23,OP24,OP25,OP26,OP27,OP28,OP29,OP2A,OP2B,OP2C,OP2D,OP2E,OP2F,
	OP30,OP31,OP32,OP33,OP34,OP35,OP36,OP37,OP38,OP39,OP3A,OP3B,OP3C,OP3D,OP3E,OP3F,
	OP40,OP41,OP42,OP43,OP44,OP45,OP46,OP47,OP48,OP49,OP4A,OP4B,OP4C,OP4D,OP4E,OP4F,
	OP50,OP51,OP52,OP53,OP54,OP55,OP56,OP57,OP58,OP59,OP5A,OP5B,OP5C,OP5D,OP5E,OP5F,
	OP60,OP61,OP62,OP63,OP64,OP65,OP66,OP67,OP68,OP69,OP6A,OP6B,OP6C,OP6D,OP6E,OP6F,
	OP70,OP71,OP72,OP73,OP74,OP75,OP76,OP77,OP78,OP79,OP7A,OP7B,OP7C,OP7D,OP7E,OP7F,
	OP80,OP81,OP82,OP83,OP84,OP85,OP86,OP87,OP88,OP89,OP8A,OP8B,OP8C,OP8D,OP8E,OP8F,
	OP90,OP91,OP92,OP93,OP94,OP95,OP96,OP97,OP98,OP99,OP9A,OP9B,OP9C,OP9D,OP9E,OP9F,
	OPA0,OPA1,OPA2,OPA3,OPA4,OPA5,OPA6,OPA7,OPA8,OPA9,OPAA,OPAB,OPAC,OPAD,OPAE,OPAF,
	OPB0,OPB1,OPB2,OPB3,OPB4,OPB5,OPB6,OPB7,OPB8,OPB9,OPBA,OPBB,OPBC,OPBD,OPBE,OPBF,
	OPC0,OPC1,OPC2,OPC3,OPC4,OPC5,OPC6,OPC7,OPC8,OPC9,OPCA,OPCB,OPCC,OPCD,OPCE,OPCF,
	OPD0,OPD1,OPD2,OPD3,OPD4,OPD5,OPD6,OPD7,OPD8,OPD9,OPDA,OPDB,OPDC,OPDD,OPDE,OPDF,
	OPE0,OPE1,OPE2,OPE3,OPE4,OPE5,OPE6,OPE7,OPE8,OPE9,OPEA,OPEB,OPEC,OPED,OPEE,OPEF,
	OPF0,OPF1,OPF2,OPF3,OPF4,OPF5,OPF6,OPF7,OPF8,OPF9,OPFA,OPFB,OPFC,OPFD,OPFE,OPFF,
};
*/


bool EmuCore :: OperateOnce()
{
#ifdef TRACER_ENABLED
	trace_buffer[trace_point] = (d.reg.pb<<16)|d.reg.pc;
	trace_point++;
	trace_point %= TRACE_SIZE ;
#endif

#ifdef _DEBUG
	if( ((d.reg.pb<<16)|d.reg.pc) == 0x0081E6EA )
		d.reg.a=d.reg.a;
#endif


#define		MMACRO1(num)	case 0x##num:return OP##num(&d);
#define		MMACRO2(num)								\
	MMACRO1(num##0);MMACRO1(num##1);MMACRO1(num##2);MMACRO1(num##3);	\
	MMACRO1(num##4);MMACRO1(num##5);MMACRO1(num##6);MMACRO1(num##7);	\
	MMACRO1(num##8);MMACRO1(num##9);MMACRO1(num##A);MMACRO1(num##B);	\
	MMACRO1(num##C);MMACRO1(num##D);MMACRO1(num##E);MMACRO1(num##F);
	switch( GetROMinPCinc(&d) )
	{
	MMACRO2(0);MMACRO2(1);MMACRO2(2);MMACRO2(3);
	MMACRO2(4);MMACRO2(5);MMACRO2(6);MMACRO2(7);
	MMACRO2(8);MMACRO2(9);MMACRO2(A);MMACRO2(B);
	MMACRO2(C);MMACRO2(D);MMACRO2(E);MMACRO2(F);
	}
#undef		MMACRO1
#undef		MMACRO2
	return false ;
}



/*
#define		MMACRO1(num)	case 0x##num:return OP##num(&d);
#define		MMACRO2(num)								\
	MMACRO1(num##0);MMACRO1(num##1);MMACRO1(num##2);MMACRO1(num##3);	\
	MMACRO1(num##4);MMACRO1(num##5);MMACRO1(num##6);MMACRO1(num##7);	\
	MMACRO1(num##8);MMACRO1(num##9);MMACRO1(num##A);MMACRO1(num##B);	\
	MMACRO1(num##C);MMACRO1(num##D);MMACRO1(num##E);MMACRO1(num##F);
	switch( GetROMinPCinc(&d) )
	{
	MMACRO2(0);MMACRO2(1);MMACRO2(2);MMACRO2(3);
	MMACRO2(4);MMACRO2(5);MMACRO2(6);MMACRO2(7);
	MMACRO2(8);MMACRO2(9);MMACRO2(A);MMACRO2(B);
	MMACRO2(C);MMACRO2(D);MMACRO2(E);MMACRO2(F);
	}
#undef		MMACRO1
#undef		MMACRO2
	return false ;
*/
