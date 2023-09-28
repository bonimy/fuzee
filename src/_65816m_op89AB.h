// BRA
__forceinline static bool OP80(DataSet* Pd) {
    BYTE tb = Immediate8(Pd);
    Pd->reg.pc += (char)tb;
    return true;
}

// STA ($**,x)
__forceinline static bool OP81(DataSet* Pd) {
    STA(Pd, DirectIndirectX(Pd));
    return true;
}
// BRL
__forceinline static bool OP82(DataSet* Pd) {
    WORD tb = Immediate16(Pd);
    Pd->reg.pc += (short)tb;
    return true;
}

// STA ,s しらん
__forceinline static bool OP83(DataSet* Pd) {
    assert(0);
    return false;
}

// STY $**
__forceinline static bool OP84(DataSet* Pd) {
    STY(Pd, Zeropage(Pd));
    return true;
}


// STA $**
__forceinline static bool OP85(DataSet* Pd) {
    STA(Pd, Zeropage(Pd));
    return true;
}

// STX $**
__forceinline static bool OP86(DataSet* Pd) {
    STX(Pd, Zeropage(Pd));
    return true;
}

// STA [$**]
__forceinline static bool OP87(DataSet* Pd) {
    STA(Pd, DirectIndirectLong(Pd));
    return true;
}
// DEY
__forceinline static bool OP88(DataSet* Pd) {
    DEY(Pd);
    return true;
}

// BIT
__forceinline static bool OP89(DataSet* Pd) {
    BIT(Pd, ImmediateA(Pd));
    return false;
}
// TXA
__forceinline static bool OP8A(DataSet* Pd) {
    Pd->reg.a.w = Pd->reg.x.w;
    //	if( IS8A )Pd->reg.a.b.h = 0x00 ;
    TESTA();
    /*
            if(IS8A)
            {
                    Pd->reg.a.b.l = Pd->reg.x.b.l;
            }
            else
            {
                    if(IS8X)
                            Pd->reg.a.w = Pd->reg.x.b.l;
                    else
                            Pd->reg.a.w = Pd->reg.x.w;
            }
    */
    return true;
}

// PHB
__forceinline static bool OP8B(DataSet* Pd) {
    Pd->Pstack[Pd->reg.s] = Pd->reg.db;
    Pd->reg.s++;
    return true;
}

// STY $****
__forceinline static bool OP8C(DataSet* Pd) {
    STY(Pd, AbsoluteAddress(Pd));
    return true;
}


// STA $****
__forceinline static bool OP8D(DataSet* Pd) {
    STA(Pd, AbsoluteAddress(Pd));
    return true;
}

// STX $****
__forceinline static bool OP8E(DataSet* Pd) {
    STX(Pd, AbsoluteAddress(Pd));
    return true;
}

// STA $******
__forceinline static bool OP8F(DataSet* Pd) {
    STA(Pd, AbsoluteLongAddress(Pd));
    return true;
}
// BCC
__forceinline static bool OP90(DataSet* Pd) {
    BYTE tb = Immediate8(Pd);
    if (Pd->reg.p & FLAG_C) return true;
    Pd->reg.pc += (char)tb;
    return true;
}
// STA ($**),y
__forceinline static bool OP91(DataSet* Pd) {
    STA(Pd, DirectIndirectY(Pd));
    return true;
}
// STA ($**)
__forceinline static bool OP92(DataSet* Pd) {
    STA(Pd, DirectIndirectY(Pd));
    return false;
}
// STA (S),y  逃避 (escape)
__forceinline static bool OP93(DataSet* Pd) {
    assert(0);
    return false;
}
// STY $**,x
__forceinline static bool OP94(DataSet* Pd) {
    STY(Pd, ZeropageIndexX(Pd));
    return true;
}
// STA $**,x
__forceinline static bool OP95(DataSet* Pd) {
    STA(Pd, ZeropageIndexX(Pd));
    return true;
}
// STX $**,y
__forceinline static bool OP96(DataSet* Pd) {
    STX(Pd, ZeropageIndexY(Pd));
    return true;
}
// STA [$**],y
__forceinline static bool OP97(DataSet* Pd) {
    STA(Pd, DirectIndirectLongY(Pd));
    return true;
}

// TYA
__forceinline static bool OP98(DataSet* Pd) {
    Pd->reg.a.w = Pd->reg.y.w;
    //	if( IS8A )Pd->reg.a.b.h = 0x00 ;
    TESTA();
    /*	if(IS8A)
            {
                    Pd->reg.a.b.l = Pd->reg.y.b.l;
            }
            else
            {
                    if(IS8Y)
                            Pd->reg.a.w = Pd->reg.y.b.l;
                    else
                            Pd->reg.a.w = Pd->reg.y.w;
            }
    */
    return true;
}

// STA $****,y
__forceinline static bool OP99(DataSet* Pd) {
    STA(Pd, AbsoluteAddressIndexY(Pd));
    return true;
}

// TXS スタックはいい加減なので、動かない可能性が大
// TXS stack is sloppy, so there's a good chance it won't work
__forceinline static bool OP9A(DataSet* Pd) {
    Pd->reg.s = Pd->reg.x.w;
    TESTX16();
    //	assert(0);
    return true;
}
// TXY
__forceinline static bool OP9B(DataSet* Pd) {
    Pd->reg.y.w = Pd->reg.x.w;
    if (IS8X) Pd->reg.y.b.h = 0x00;
    TESTY();
    /*
            if(IS8Y)
            {
                    Pd->reg.y.b.l = Pd->reg.x.b.l;
            }
            else
            {
                    Pd->reg.y.w = Pd->reg.x.w;
            }
    */
    return true;
}


// STZ $****
__forceinline static bool OP9C(DataSet* Pd) {
    STZ(Pd, Immediate16(Pd));
    return true;
}


// STA $****,x
__forceinline static bool OP9D(DataSet* Pd) {
    STA(Pd, AbsoluteAddressIndexX(Pd));
    return true;
}

// STZ $****,x
__forceinline static bool OP9E(DataSet* Pd) {
    STZ(Pd, AbsoluteAddressIndexX(Pd));
    return true;
}

// sta $******,X
__forceinline static bool OP9F(DataSet* Pd) {
    STA(Pd, AbsoluteLongAddressIndexX(Pd));
    return true;
}

// LDY #$**
__forceinline static bool OPA0(DataSet* Pd) {
    if (IS8Y) {
        LDY(Pd, Immediate8(Pd));
    } else {
        LDY(Pd, Immediate16(Pd));
    }
    return true;
}

// LDA ($**,x)
__forceinline static bool OPA1(DataSet* Pd) {
    LDA(Pd, READA(Pd, DirectIndirectX(Pd)));
    return true;
}
// LDX #$**
__forceinline static bool OPA2(DataSet* Pd) {
    LDX(Pd, ImmediateX(Pd));
    return true;
}

// LDA ,s
__forceinline static bool OPA3(DataSet* Pd) {
    assert(0);
    return false;
}

// LDY $**
__forceinline static bool OPA4(DataSet* Pd) {
    LDY(Pd, READY(Pd, Zeropage(Pd)));
    return true;
}


// LDA $**
__forceinline static bool OPA5(DataSet* Pd) {
    LDA(Pd, READA(Pd, Zeropage(Pd)));
    return true;
}

// LDX $**
__forceinline static bool OPA6(DataSet* Pd) {
    LDX(Pd, READX(Pd, Zeropage(Pd)));
    return true;
}

// LDA [$**]
__forceinline static bool OPA7(DataSet* Pd) {
    LDA(Pd, READA(Pd, DirectIndirectLong(Pd)));
    return true;
}

// TAY
__forceinline static bool OPA8(DataSet* Pd) {
    Pd->reg.y.w = Pd->reg.a.w;
    if (IS8Y) Pd->reg.y.b.h = 0x00;
    TESTY();
    /*
            if(IS8Y)
            {
                    Pd->reg.y.b.l = Pd->reg.a.b.l;
            }
            else
            {
                    if(IS8A)
                            Pd->reg.y.w = Pd->reg.a.b.l;
                    else
                            Pd->reg.y.w = Pd->reg.a.w;
            }
    */
    return true;
}

// LDA #**(**)
__forceinline static bool OPA9(DataSet* Pd) {
    LDA(Pd, ImmediateA(Pd));
    return true;
}

// TAX
__forceinline static bool OPAA(DataSet* Pd) {
    Pd->reg.x.w = Pd->reg.a.w;
    if (IS8X) Pd->reg.x.b.h = 0x00;
    TESTX();
    /*
            if(IS8X)
            {
                    Pd->reg.x.b.l = Pd->reg.a.b.l;
            }
            else
            {
                    if(IS8A)
                            Pd->reg.x.w = Pd->reg.a.b.l;
                    else
                            Pd->reg.x.w = Pd->reg.a.w;
            }
    */
    return true;
}

// PLB
__forceinline static bool OPAB(DataSet* Pd) {
    Pd->reg.db = Pd->Pstack[Pd->reg.s - 1];
    Pd->reg.s--;
    return true;
}

// LDY $****
__forceinline static bool OPAC(DataSet* Pd) {
    LDY(Pd, READY(Pd, AbsoluteAddress(Pd)));
    return true;
}

// LDA $****
__forceinline static bool OPAD(DataSet* Pd) {
    LDA(Pd, READA(Pd, AbsoluteAddress(Pd)));
    return true;
}

// LDX $****
__forceinline static bool OPAE(DataSet* Pd) {
    LDX(Pd, READX(Pd, AbsoluteAddress(Pd)));
    return true;
}

// LDA $******
__forceinline static bool OPAF(DataSet* Pd) {
    LDA(Pd, READA(Pd, AbsoluteLongAddress(Pd)));
    return true;
}


// BCS
__forceinline static bool OPB0(DataSet* Pd) {
    BYTE tb = Immediate8(Pd);
    if (Pd->reg.p & FLAG_C) Pd->reg.pc += (char)tb;
    return true;
}

// LDA ($**),y
__forceinline static bool OPB1(DataSet* Pd) {
    LDA(Pd, READA(Pd, DirectIndexY(Pd)));
    return true;
}

// LDA ($**)
__forceinline static bool OPB2(DataSet* Pd) {
    LDA(Pd, READA(Pd, DirectIndirect(Pd)));
    return true;
}

// LDA (s),y
__forceinline static bool OPB3(DataSet* Pd) {
    assert(0);
    return false;
}

// LDY $**,X
__forceinline static bool OPB4(DataSet* Pd) {
    LDY(Pd, READY(Pd, ZeropageIndexX(Pd)));
    return true;
}
// LDA $**,X
__forceinline static bool OPB5(DataSet* Pd) {
    LDA(Pd, READA(Pd, ZeropageIndexX(Pd)));
    return true;
}

// LDX $**,y
__forceinline static bool OPB6(DataSet* Pd) {
    LDX(Pd, READX(Pd, ZeropageIndexY(Pd)));
    return true;
}

// LDA [$**],y
__forceinline static bool OPB7(DataSet* Pd) {
    LDA(Pd, READA(Pd, DirectIndirectLongY(Pd)));
    return true;
}

// CLV
__forceinline static bool OPB8(DataSet* Pd) {
    CLR_F(FLAG_V);
    return true;
}

// LDA $****,y
__forceinline static bool OPB9(DataSet* Pd) {
    LDA(Pd, READA(Pd, AbsoluteAddressIndexY(Pd)));
    return true;
}
// TSX
__forceinline static bool OPBA(DataSet* Pd) {
    Pd->reg.x.w = Pd->reg.s;
    if (IS8X) Pd->reg.x.b.h = 0x00;
    TESTX();
    return true;
}
// TYX
__forceinline static bool OPBB(DataSet* Pd) {
    Pd->reg.x.w = Pd->reg.y.w;
    if (IS8X) Pd->reg.x.b.h = 0x00;
    TESTX();
    /*	if(IS8X)
            {
                    Pd->reg.x.b.l = Pd->reg.y.b.l;
            }
            else
            {
                    Pd->reg.x.w = Pd->reg.y.w;
            }
    */
    return true;
}

// LDY $****,X
__forceinline static bool OPBC(DataSet* Pd) {
    LDY(Pd, READY(Pd, AbsoluteAddressIndexX(Pd)));
    return true;
}
// LDA $****,X
__forceinline static bool OPBD(DataSet* Pd) {
    LDA(Pd, READA(Pd, AbsoluteAddressIndexX(Pd)));
    return true;
}

// LDX $****,y
__forceinline static bool OPBE(DataSet* Pd) {
    LDX(Pd, READX(Pd, AbsoluteAddressIndexY(Pd)));
    return true;
}


// LDA $******,X
__forceinline static bool OPBF(DataSet* Pd) {
    LDA(Pd, READA(Pd, AbsoluteLongAddressIndexX(Pd)));
    return true;
}
