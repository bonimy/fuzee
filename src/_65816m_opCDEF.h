// CPY #$**(**)
__forceinline static bool OPC0(DataSet* Pd) {
    if (IS8Y) {
        CPY(Pd, Immediate8(Pd));
    } else {
        CPY(Pd, Immediate16(Pd));
    }
    return true;
}

// CMP ($**,x)
__forceinline static bool OPC1(DataSet* Pd) {
    CMP(Pd, READA(Pd, DirectIndirectX(Pd)));
    return true;
}
// REP #$**
__forceinline static bool OPC2(DataSet* Pd) {
    REP(Pd, Immediate8(Pd));
    return true;
}

// CMP ,s
__forceinline static bool OPC3(DataSet* Pd) {
    assert(0);
    return false;
}
// CPY $**
__forceinline static bool OPC4(DataSet* Pd) {
    CPY(Pd, READY(Pd, Zeropage(Pd)));
    return true;
}
// CMP $**
__forceinline static bool OPC5(DataSet* Pd) {
    CMP(Pd, READA(Pd, Immediate8(Pd)));
    return true;
}

// DEC $**
__forceinline static bool OPC6(DataSet* Pd) {
    DEC(Pd, Immediate8(Pd));
    return true;
}

// CMP [$**]
__forceinline static bool OPC7(DataSet* Pd) {
    CMP(Pd, READA(Pd, DirectIndirectLong(Pd)));
    return true;
}
// INY
__forceinline static bool OPC8(DataSet* Pd) {
    INY(Pd);
    return true;
}

// CMP #$**
__forceinline static bool OPC9(DataSet* Pd) {
    CMP(Pd, ImmediateA(Pd));
    return true;
}

// DEX
__forceinline static bool OPCA(DataSet* Pd) {
    DEX(Pd);
    return true;
}

// WAI
__forceinline static bool OPCB(DataSet* Pd) {
    assert(0);
    return false;
}
// CPY $****
__forceinline static bool OPCC(DataSet* Pd) {
    CPY(Pd, READY(Pd, AbsoluteAddress(Pd)));
    return true;
}
// CMP $****
__forceinline static bool OPCD(DataSet* Pd) {
    CMP(Pd, READA(Pd, AbsoluteAddress(Pd)));
    return true;
}

// DEC $****
__forceinline static bool OPCE(DataSet* Pd) {
    DEC(Pd, AbsoluteAddress(Pd));
    return true;
}
// CMP $******
__forceinline static bool OPCF(DataSet* Pd) {
    CMP(Pd, READA(Pd, AbsoluteLongAddress(Pd)));
    return true;
}
// BNE
__forceinline static bool OPD0(DataSet* Pd) {
    BYTE tb = Immediate8(Pd);
    if (Pd->reg.p & FLAG_Z) return true;
    Pd->reg.pc += (char)tb;
    return true;
}
// CMP ($**),y
__forceinline static bool OPD1(DataSet* Pd) {
    CMP(Pd, READA(Pd, DirectIndirectY(Pd)));
    return true;
}
// CMP ($**)
__forceinline static bool OPD2(DataSet* Pd) {
    CMP(Pd, READA(Pd, DirectIndirect(Pd)));
    return true;
}
// JML
__forceinline static bool OPD3(DataSet* Pd) {
    WORD tw;
    int ti;
    tw = Immediate16(Pd);
    ti = (READB(Pd, tw + 2) << 16) | READW(Pd, tw);
    return JMP(Pd, ti);
}
// PEI ($**)  使わないでしょ (do not use)
__forceinline static bool OPD4(DataSet* Pd) { return false; }

// CMP $**,X
__forceinline static bool OPD5(DataSet* Pd) {
    CMP(Pd, READA(Pd, ZeropageIndexX(Pd)));
    return true;
}

// DEC $**,x
__forceinline static bool OPD6(DataSet* Pd) {
    DEC(Pd, ZeropageIndexX(Pd));
    return true;
}
// CMP [$**],y
__forceinline static bool OPD7(DataSet* Pd) {
    CMP(Pd, DirectIndirectLongY(Pd));
    return true;
}
// CLD
__forceinline static bool OPD8(DataSet* Pd) {
    CLR_F(FLAG_D);
    return true;
}

// CMP $****,y
__forceinline static bool OPD9(DataSet* Pd) {
    CMP(Pd, READA(Pd, AbsoluteAddressIndexY(Pd)));
    return true;
}

// PHX
__forceinline static bool OPDA(DataSet* Pd) {
    if (IS8X) {
        Pd->Pstack[Pd->reg.s] = Pd->reg.x.b.l;
        Pd->reg.s++;
    } else {
        Pd->Pstack[Pd->reg.s] = Pd->reg.x.b.l;
        Pd->Pstack[Pd->reg.s + 1] = Pd->reg.x.b.h;
        Pd->reg.s += 2;
    }
    return true;
}
// STP
__forceinline static bool OPDB(DataSet* Pd) {
    assert(0);
    return false;
}

// JML ($****)
__forceinline static bool OPDC(DataSet* Pd) {
    WORD tw;
    int ti;
    tw = Immediate16(Pd);
    ti = READW(Pd, tw);
    ti |= (READB(Pd, tw + 2) << 16);
    return JMP(Pd, ti);
}
// CMP $****,x
__forceinline static bool OPDD(DataSet* Pd) {
    CMP(Pd, READA(Pd, AbsoluteAddressIndexX(Pd)));
    return true;
}

// DEC $****,x
__forceinline static bool OPDE(DataSet* Pd) {
    DEC(Pd, AbsoluteAddressIndexX(Pd));
    return true;
}
// CMP $******,x
__forceinline static bool OPDF(DataSet* Pd) {
    CMP(Pd, READA(Pd, AbsoluteLongAddressIndexX(Pd)));
    return true;
}

// CPX #$**(**)
__forceinline static bool OPE0(DataSet* Pd) {
    if (IS8X)
        CPX(Pd, Immediate8(Pd));
    else
        CPX(Pd, Immediate16(Pd));
    return true;
}
// SBC ($**,x)
__forceinline static bool OPE1(DataSet* Pd) {
    SBC(Pd, READA(Pd, DirectIndirectX(Pd)));
    return true;
}

// SEP
__forceinline static bool OPE2(DataSet* Pd) {
    SEP(Pd, Immediate8(Pd));
    return true;
}
// SBC ,s
__forceinline static bool OPE3(DataSet* Pd) {
    assert(0);
    return false;
}

// CPX $**
__forceinline static bool OPE4(DataSet* Pd) {
    CPX(Pd, READX(Pd, Zeropage(Pd)));
    return true;
}

// SBC $**
__forceinline static bool OPE5(DataSet* Pd) {
    SBC(Pd, READA(Pd, Zeropage(Pd)));
    return true;
}

// INC $**
__forceinline static bool OPE6(DataSet* Pd) {
    INC(Pd, Immediate8(Pd));
    return true;
}
// SBC [$**]
__forceinline static bool OPE7(DataSet* Pd) {
    SBC(Pd, READA(Pd, DirectIndirectLong(Pd)));
    return true;
}
// INX
__forceinline static bool OPE8(DataSet* Pd) {
    INX(Pd);
    return true;
}

// SBC #$**
__forceinline static bool OPE9(DataSet* Pd) {
    SBC(Pd, ImmediateA(Pd));
    TESTA();
    return true;
}

__forceinline static bool OPEA(DataSet* Pd) {
    //大好きな、NOP
    // My favorite, NOP
    return true;
}

// XBA
__forceinline static bool OPEB(DataSet* Pd) {
    XBA(Pd);
    return true;
}
// CPX $****
__forceinline static bool OPEC(DataSet* Pd) {
    CPX(Pd, READX(Pd, AbsoluteAddress(Pd)));
    return true;
}
// SBC $****
__forceinline static bool OPED(DataSet* Pd) {
    SBC(Pd, READA(Pd, AbsoluteAddress(Pd)));
    return true;
}
// INC $****
__forceinline static bool OPEE(DataSet* Pd) {
    INC(Pd, AbsoluteAddress(Pd));
    return true;
}
// SBC $******
__forceinline static bool OPEF(DataSet* Pd) {
    SBC(Pd, READA(Pd, AbsoluteLongAddress(Pd)));
    return true;
}
// BEQ
__forceinline static bool OPF0(DataSet* Pd) {
    BYTE tb = Immediate8(Pd);
    if (Pd->reg.p & FLAG_Z) Pd->reg.pc += (char)tb;
    return true;
}
// SBC ($**),y
__forceinline static bool OPF1(DataSet* Pd) {
    SBC(Pd, READA(Pd, DirectIndirectY(Pd)));
    return true;
}
// SBC ($**)
__forceinline static bool OPF2(DataSet* Pd) {
    SBC(Pd, READA(Pd, DirectIndirect(Pd)));
    return true;
}
// SBC (s),y
__forceinline static bool OPF3(DataSet* Pd) {
    assert(0);
    return false;
}
// PEA $****          そのままプッシュするだけらしいよ
// PEA $**** It seems like you just need to push it.
__forceinline static bool OPF4(DataSet* Pd) {
    Pd->Pstack[Pd->reg.s + 1] = Immediate8(Pd);
    Pd->Pstack[Pd->reg.s] = Immediate8(Pd);
    Pd->reg.s += 2;
    return true;
}

// SBC $**,x
__forceinline static bool OPF5(DataSet* Pd) {
    SBC(Pd, READA(Pd, ZeropageIndexX(Pd)));
    return true;
}


// INC $**,x
__forceinline static bool OPF6(DataSet* Pd) {
    INC(Pd, ZeropageIndexX(Pd));
    return true;
}
// SBC [$**],y
__forceinline static bool OPF7(DataSet* Pd) {
    SBC(Pd, READA(Pd, DirectIndirectLongY(Pd)));
    return true;
}
// SED
__forceinline static bool OPF8(DataSet* Pd) {
    SET_F(FLAG_D);
    return true;
}
// SBC $****,y
__forceinline static bool OPF9(DataSet* Pd) {
    SBC(Pd, READA(Pd, AbsoluteAddressIndexY(Pd)));
    return true;
}
// PLX
__forceinline static bool OPFA(DataSet* Pd) {
    if (IS8X) {
        Pd->reg.x.b.l = Pd->Pstack[Pd->reg.s - 1];
        Pd->reg.s--;
    } else {
        Pd->reg.x.b.l = Pd->Pstack[Pd->reg.s - 2];
        Pd->reg.x.b.h = Pd->Pstack[Pd->reg.s - 1];
        Pd->reg.s -= 2;
    }
    TESTX();
    return true;
}


// XCE 使わないよなぁ
// I don't use XCE
__forceinline static bool OPFB(DataSet* Pd) {
    assert(0);
    return false;
}
// JSR ($******,x)  よーわからん
// JSR ($******,x) I don't know
__forceinline static bool OPFC(DataSet* Pd) {
    assert(0);
    return false;
}

// SBC $****,x
__forceinline static bool OPFD(DataSet* Pd) {
    SBC(Pd, READA(Pd, AbsoluteAddressIndexX(Pd)));
    return true;
}
// INC $****,x
__forceinline static bool OPFE(DataSet* Pd) {
    INC(Pd, AbsoluteAddressIndexX(Pd));
    return true;
}

// SBC $******,x
__forceinline static bool OPFF(DataSet* Pd) {
    SBC(Pd, READA(Pd, AbsoluteLongAddressIndexX(Pd)));
    return true;
}
