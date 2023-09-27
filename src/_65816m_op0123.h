//BRK #**現実逃避だな
__forceinline static bool OP00( DataSet* Pd )
{
	assert(0);
	return false;
}

//ORA ($**,X)
__forceinline static bool OP01( DataSet* Pd )
{
	ORA( Pd , READA( Pd , DirectIndirectX( Pd ) ) );
	return true;
}

//COP 現実逃避
__forceinline static bool OP02( DataSet* Pd )
{
	assert(0);
	return false;
}

//OR ,S　わからん
__forceinline static bool OP03( DataSet* Pd )
{
	assert(0);
	return false;
}

//TSB $**
__forceinline static bool OP04( DataSet* Pd )
{
	TSB( Pd , Zeropage( Pd ) ) ;
	return true;
}

//ORA $**
__forceinline static bool OP05( DataSet* Pd )
{
	ORA( Pd , READA( Pd , Zeropage( Pd ) ) ) ;
	return true;
}

//ASL $**
__forceinline static bool OP06( DataSet* Pd )
{
	ASLm( Pd , GetMemoryPointer(Pd , Zeropage( Pd ) ));
	return true;
}

//ORA [$**]
__forceinline static bool OP07( DataSet* Pd )
{
	ORA( Pd , READA ( Pd , DirectIndirectLong( Pd ) ) ) ;
	return true;
}


//PHP
__forceinline static bool OP08( DataSet* Pd )
{
	Pd->Pstack[Pd->reg.s] = Pd->reg.p;
	Pd->reg.s++;
	return true;
}

//ORA #$**(**)
__forceinline static bool OP09( DataSet* Pd )
{
	ORA( Pd , ImmediateA(Pd) );
	return true;
}

//ASL A
__forceinline static bool OP0A( DataSet* Pd )
{
	CLR_F(FLAG_C);
	if( IS8A )
	{
		if( Pd->reg.a.b.l & 0x80 )SET_F(FLAG_C);
		Pd->reg.a.b.l <<= 1;
	}
	else
	{
		if( Pd->reg.a.w & 0x8000 )SET_F(FLAG_C);
		Pd->reg.a.w <<= 1;
	}
	TESTA();
	return true;
}

//PHD
__forceinline static bool OP0B( DataSet* Pd )
{
	Pd->Pstack[Pd->reg.s  ] = Pd->reg.d&0xFF;
	Pd->Pstack[Pd->reg.s+1] = Pd->reg.d>>8;
	Pd->reg.s+=2;
	return true;
}

//TSB
__forceinline static bool OP0C( DataSet* Pd )
{
	TSB( Pd , AbsoluteAddress( Pd ) ) ;
	return true;
}

//ORA $****
__forceinline static bool OP0D( DataSet* Pd )
{
	ORA( Pd , READA( Pd , AbsoluteAddress( Pd ) ) ) ;
	return true;
}

//ASL $****
__forceinline static bool OP0E( DataSet* Pd )
{
	ASLm( Pd , GetMemoryPointer( Pd , AbsoluteAddress( Pd ) ) ) ;
	return true;
}

//ORA $******
__forceinline static bool OP0F( DataSet* Pd )
{
	ORA( Pd , AbsoluteLongAddress( Pd ) ) ;
	return true;
}


//BPL
__forceinline static bool OP10( DataSet* Pd )
{
BYTE tb=Immediate8(Pd);
	if( Pd->reg.p & FLAG_N )return true;
	Pd->reg.pc += (char)tb;
	return true;
}

//ORA ($**),y
__forceinline static bool OP11( DataSet* Pd )
{
	ORA( Pd , READA( Pd , DirectIndirectY( Pd ) ) ) ;
	return true;
}

//ORA ($**)
__forceinline static bool OP12( DataSet* Pd )
{
	ORA( Pd , READA( Pd , DirectIndirect( Pd ) ) ) ;
	return true;
}

//ORA (S),y
__forceinline static bool OP13( DataSet* Pd )
{
	assert(0);
	return false;
}

//trb $**
__forceinline static bool OP14( DataSet* Pd )
{
	TRB( Pd , Zeropage( Pd ) ) ;
	return true;
}

//ORA $**,x
__forceinline static bool OP15( DataSet* Pd )
{
	ORA( Pd , ZeropageIndexX( Pd ) ) ;
	return true;
}

//ASL $**,x
__forceinline static bool OP16( DataSet* Pd )
{
	ASLm( Pd , GetMemoryPointer( Pd , ZeropageIndexX( Pd ) ) ) ;
	return true;
}

//ORA [$**],y
__forceinline static bool OP17( DataSet* Pd )
{
	ORA( Pd , READA( Pd , DirectIndirectLongY( Pd ) ) ) ;
	return true;
}

//CLC
__forceinline static bool OP18( DataSet* Pd )
{
	CLR_F( FLAG_C );
	return true;
}

//ORA &****,y
__forceinline static bool OP19( DataSet* Pd )
{
	ORA( Pd , READA( Pd , AbsoluteAddressIndexY( Pd ) ) ) ;
	return true;
}

//INC A
__forceinline static bool OP1A( DataSet* Pd )
{
	INCA( Pd );
	return true;
}

//TCS            c->s?
__forceinline static bool OP1B( DataSet* Pd )
{
	assert(0);
	return false;
}

//TRB $****
__forceinline static bool OP1C( DataSet* Pd )
{
	TRB( Pd , AbsoluteAddress( Pd ) ) ;
	return true;
}

//ORA $****,x
__forceinline static bool OP1D( DataSet* Pd )
{
	ORA( Pd , READA( Pd , AbsoluteAddressIndexX( Pd ) ) ) ;
	return true;
}

//ASL $****,x
__forceinline static bool OP1E( DataSet* Pd )
{
	ASLm( Pd , GetMemoryPointer( Pd , AbsoluteAddressIndexX( Pd ) ) ) ;
	return true;
}

//ORA $******,x
__forceinline static bool OP1F( DataSet* Pd )
{
	ORA( Pd , READA( Pd , AbsoluteLongAddressIndexX( Pd ) ) ) ;
	return true;
}

//JSR $nn****
__forceinline static bool OP20( DataSet* Pd )
{
	JSR2( Pd , Immediate16( Pd )+(Pd->reg.pb<<16));
	return true;
}

//AND ($**,x)
__forceinline static bool OP21( DataSet* Pd )
{
	AND( Pd , READA( Pd , DirectIndirectX( Pd ) ) ) ;
	return true;
}

__forceinline static bool OP22( DataSet* Pd )
{
WORD tw=Immediate16( Pd );
	JSR3( Pd , tw+(Immediate8(Pd)<<16));
	return true;
}

//AND ,S
__forceinline static bool OP23( DataSet* Pd )
{
	assert(0);
	return false;
}

//BIT $**
__forceinline static bool OP24( DataSet* Pd )
{
	BIT( Pd , READA( Pd , Zeropage( Pd ) ) ) ;
	return true;
}

//AND $**
__forceinline static bool OP25( DataSet* Pd )
{
	AND( Pd , READA( Pd , Zeropage( Pd ) ) ) ;
	return true;
}

//ROL $**
__forceinline static bool OP26( DataSet* Pd )
{
	ROL( Pd , GetMemoryPointer( Pd , Zeropage( Pd ) ) );
	return true;
}

//AND [$**]
__forceinline static bool OP27( DataSet* Pd )
{
	AND( Pd , DirectIndirectLong( Pd ) ) ;
	return true;
}

//PLP
__forceinline static bool OP28( DataSet* Pd )
{
	Pd->reg.p = Pd->Pstack[Pd->reg.s-1];
	Pd->reg.s--;
	return true;
}

//AND #**(**)
__forceinline static bool OP29( DataSet* Pd )
{
	AND( Pd , ImmediateA( Pd ) );
	return true;
}

//ROL A
__forceinline static bool OP2A( DataSet* Pd )
{
	ROL( Pd , (BYTE*)&(Pd->reg.a.w) );
	return true;
}


//PLD
__forceinline static bool OP2B( DataSet* Pd )
{
	Pd->reg.d = (Pd->Pstack[Pd->reg.s-1]<<8) + Pd->Pstack[Pd->reg.s-2];
	Pd->reg.s-=2;
	return true;
}

//CMP $****
__forceinline static bool OP2C( DataSet* Pd )
{
	CMP( Pd , READA( Pd , AbsoluteAddress( Pd ) ) );
	return true;
}

//AND $****
__forceinline static bool OP2D( DataSet* Pd )
{
	AND( Pd , AbsoluteAddress( Pd ) ) ;
	return true;
}

//ROL $****
__forceinline static bool OP2E( DataSet* Pd )
{
	ROL( Pd , GetMemoryPointer( Pd , AbsoluteAddress( Pd ) ) ) ;
	return true;
}

//AND $******
__forceinline static bool OP2F( DataSet* Pd )
{
	AND( Pd , AbsoluteLongAddress( Pd ) ) ;
	return true;
}


//BMI
__forceinline static bool OP30( DataSet* Pd )
{
BYTE tb=Immediate8(Pd);
	if( Pd->reg.p & FLAG_N )
		Pd->reg.pc += (char)tb;
	return true;
}

//AND ($**),y
__forceinline static bool OP31( DataSet* Pd )
{
	AND( Pd , READA( Pd , DirectIndirectY( Pd ) ) ) ;
	return true;
}

//AND ($**)
__forceinline static bool OP32( DataSet* Pd )
{
	AND( Pd , READA( Pd , DirectIndirect( Pd ) ) ) ;
	return true;
}

//AND (s),y
__forceinline static bool OP33( DataSet* Pd )
{
	assert(0);
	return false;
}

//BIT $**,x
__forceinline static bool OP34( DataSet* Pd )
{
	BIT( Pd , READA(  Pd , ZeropageIndexX( Pd ) ) ) ;
	return true;
}

//AND $**,x
__forceinline static bool OP35( DataSet* Pd )
{
	AND( Pd , READA( Pd , ZeropageIndexX( Pd ) ) ) ;
	return true;
}

//ROL $**,x
__forceinline static bool OP36( DataSet* Pd )
{
	ROL( Pd , GetMemoryPointer( Pd , ZeropageIndexX( Pd ) ) ) ;
	return true;
}

//AND [$**],y
__forceinline static bool OP37( DataSet* Pd )
{
	AND( Pd , READA( Pd , DirectIndirectLongY( Pd ) ) ) ;
	return true;
}

//SEC
__forceinline static bool OP38( DataSet* Pd )
{
	Pd->reg.p |= FLAG_C;
	return true;
}

//AND $****,y
__forceinline static bool OP39( DataSet* Pd )
{
	AND( Pd , READA( Pd , AbsoluteAddressIndexY( Pd ) ) ) ;
	return true;
}

//DEC A
__forceinline static bool OP3A( DataSet* Pd )
{
	if( IS8A )
	{
		Pd->reg.a.b.l --;
		TESTA8();
	}
	else
	{
		Pd->reg.a.w --;
		TESTA16();
	}
	return true;
}

//TSC
__forceinline static bool OP3B( DataSet* Pd )
{
	assert(0);
	return false;
}

//BIT $****,x
__forceinline static bool OP3C( DataSet* Pd )
{
	BIT( Pd , READA(  Pd , AbsoluteAddressIndexX( Pd ) ) ) ;
	return true;
}

//AND $****,x
__forceinline static bool OP3D( DataSet* Pd )
{
	AND( Pd , READA( Pd , AbsoluteAddressIndexX( Pd ) ) ) ;
	return true;
}

//ROL $****,x
__forceinline static bool OP3E( DataSet* Pd )
{
	ROL( Pd , GetMemoryPointer( Pd , AbsoluteAddressIndexX( Pd ) ) ) ;
	return true;
}

//AND $******,x
__forceinline static bool OP3F( DataSet* Pd )
{
	AND( Pd , READA( Pd , AbsoluteLongAddressIndexX( Pd ) ) ) ;
	return true;
}
