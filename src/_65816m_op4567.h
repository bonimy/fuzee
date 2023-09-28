// RTI とりあえず、RTL呼んで置く
// RTI For now, call RTL
__forceinline static bool OP40(DataSet* Pd) {
    RTL(Pd);
    return true;
}

// EOR ($**,x)
__forceinline static bool OP41(DataSet* Pd) {
    EOR(Pd, READA(Pd, DirectIndirectX(Pd)));
    return true;
}

// WDM??
__forceinline static bool OP42(DataSet* Pd) {
    Immediate8(Pd);
    return true;
}

// EOR ,s  わからん
// EOR ,s I don't know
__forceinline static bool OP43(DataSet* Pd) {
    assert(0);
    return false;
}

// MVP　ブロック転送ポジティブ
// MVP block transfer positive
__forceinline static bool OP44(DataSet* Pd) {
    BYTE tb = Immediate8(Pd);
    MVP(Pd, tb, Immediate8(Pd));
    return true;
}

// EOR $**
__forceinline static bool OP45(DataSet* Pd) {
    EOR(Pd, READA(Pd, Zeropage(Pd)));
    return true;
}

// LSR $**
__forceinline static bool OP46(DataSet* Pd) {
    LSRm(Pd, GetMemoryPointer(Pd, Zeropage(Pd)));
    return true;
}

// EOR [$**]
__forceinline static bool OP47(DataSet* Pd) {
    EOR(Pd, READA(Pd, DirectIndirectLong(Pd)));
    return true;
}
// PHA
__forceinline static bool OP48(DataSet* Pd) {
    PHA(Pd);
    return true;
}

// EOR #$**(**)
__forceinline static bool OP49(DataSet* Pd) {
    EOR(Pd, ImmediateA(Pd));
    return true;
}

// LSR A
__forceinline static bool OP4A(DataSet* Pd) {
    CLR_F(FLAG_C);
    if (IS8A) {
        if (Pd->reg.a.b.l & 0x01) SET_F(FLAG_C);
        Pd->reg.a.b.l >>= 1;
    } else {
        if (Pd->reg.a.w & 0x0001) SET_F(FLAG_C);
        Pd->reg.a.w >>= 1;
    }
    TESTA();
    return true;
}

// PHK
__forceinline static bool OP4B(DataSet* Pd) {
    Pd->Pstack[Pd->reg.s] = Pd->reg.pb;
    Pd->reg.s++;
    return true;
}

// JMP
__forceinline static bool OP4C(DataSet* Pd) {
    return JMP(Pd, Immediate16(Pd) | (Pd->reg.pb << 16));
}

// EOR $****
__forceinline static bool OP4D(DataSet* Pd) {
    EOR(Pd, READA(Pd, AbsoluteAddress(Pd)));
    return true;
}

// LSR $****
__forceinline static bool OP4E(DataSet* Pd) {
    LSRm(Pd, GetMemoryPointer(Pd, AbsoluteAddress(Pd)));
    return true;
}
// EOR $******
__forceinline static bool OP4F(DataSet* Pd) {
    EOR(Pd, AbsoluteLongAddress(Pd));
    return true;
}

// bvc
__forceinline static bool OP50(DataSet* Pd) {
    BYTE tb = Immediate8(Pd);
    if (Pd->reg.p & FLAG_V) return true;
    Pd->reg.pc += (char)tb;
    return true;
}

// EOR ($**),y
__forceinline static bool OP51(DataSet* Pd) {
    EOR(Pd, READA(Pd, DirectIndirectY(Pd)));
    return true;
}

// EOR ($**)
__forceinline static bool OP52(DataSet* Pd) {
    EOR(Pd, READA(Pd, DirectIndirect(Pd)));
    return true;
}

// EOR (s),y 放置
__forceinline static bool OP53(DataSet* Pd) {
    assert(0);
    return false;
}

// MVN:ブロック転送ネガティブ
// MVN: Block transfer negative
__forceinline static bool OP54(DataSet* Pd) {
    BYTE dest = Immediate8(Pd);
    MVN(Pd, dest, Immediate8(Pd));
    return true;
}

// EOR $**,x
__forceinline static bool OP55(DataSet* Pd) {
    EOR(Pd, READA(Pd, ZeropageIndexX(Pd)));
    return true;
}

// LSR $**,x
__forceinline static bool OP56(DataSet* Pd) {
    LSRm(Pd, GetMemoryPointer(Pd, ZeropageIndexX(Pd)));
    return true;
}

// EOR [$**],y
__forceinline static bool OP57(DataSet* Pd) {
    EOR(Pd, READA(Pd, DirectIndirectLongY(Pd)));
    return true;
}
// CLI
__forceinline static bool OP58(DataSet* Pd) {
    CLR_F(FLAG_I);
    return true;
}

// EOR $****,y
__forceinline static bool OP59(DataSet* Pd) {
    EOR(Pd, READA(Pd, AbsoluteAddressIndexY(Pd)));
    return false;
}
// PHY
__forceinline static bool OP5A(DataSet* Pd) {
    if (IS8Y) {
        Pd->Pstack[Pd->reg.s] = Pd->reg.y.b.l;
        Pd->reg.s++;
    } else {
        Pd->Pstack[Pd->reg.s] = Pd->reg.y.b.l;
        Pd->Pstack[Pd->reg.s + 1] = Pd->reg.y.b.h;
        Pd->reg.s += 2;
    }
    return true;
}

// TCD   C(a16)->D
__forceinline static bool OP5B(DataSet* Pd) {
    Pd->reg.d = Pd->reg.a.w;
    return true;
}

// JMP $******
__forceinline static bool OP5C(DataSet* Pd) {
    WORD tw = Immediate16(Pd);
    JMP(Pd, (Immediate8(Pd) << 16) | tw);
    return true;
}

// EOR $****,x
__forceinline static bool OP5D(DataSet* Pd) {
    EOR(Pd, READA(Pd, AbsoluteAddressIndexX(Pd)));
    return true;
}

// LSR $****,x
__forceinline static bool OP5E(DataSet* Pd) {
    LSRm(Pd, GetMemoryPointer(Pd, AbsoluteAddressIndexX(Pd)));
    return true;
}

// EOR $******,x
__forceinline static bool OP5F(DataSet* Pd) {
    EOR(Pd, READA(Pd, AbsoluteLongAddressIndexX(Pd)));
    return false;
}

// rts
__forceinline static bool OP60(DataSet* Pd) {
    RTS(Pd);
    return true;
}

// ADC ($**,x)
__forceinline static bool OP61(DataSet* Pd) {
    ADC(Pd, DirectIndirectX(Pd));
    return true;
}

// PER??
__forceinline static bool OP62(DataSet* Pd) {
    assert(0);
    return false;
}

// ADC ,s 無視
// ADC ,s ignored
__forceinline static bool OP63(DataSet* Pd) {
    assert(0);
    return false;
}

// STZ $**
__forceinline static bool OP64(DataSet* Pd) {
    STZ(Pd, Zeropage(Pd));
    return true;
}

// ADC $**
__forceinline static bool OP65(DataSet* Pd) {
    ADC(Pd, READA(Pd, Zeropage(Pd)));
    return true;
}

// ROR $**
__forceinline static bool OP66(DataSet* Pd) {
    ROR(Pd, GetMemoryPointer(Pd, Zeropage(Pd)));
    return true;
}

// ADC [$**]
__forceinline static bool OP67(DataSet* Pd) {
    ADC(Pd, READA(Pd, DirectIndirectLong(Pd)));
    return true;
}

// PLA
__forceinline static bool OP68(DataSet* Pd) {
    PLA(Pd);
    return true;
}

// ADC #$**
__forceinline static bool OP69(DataSet* Pd) {
    ADC(Pd, ImmediateA(Pd));
    return true;
}

// ROR A
__forceinline static bool OP6A(DataSet* Pd) {
    RORa(Pd);
    return true;
}
// RTL
__forceinline static bool OP6B(DataSet* Pd) {
    RTL(Pd);
    return true;
}

// JMP ($****)わかんない
// JMP ($****)I don't know
__forceinline static bool OP6C(DataSet* Pd) {
    assert(0);
    return false;
}
// ADC $****
__forceinline static bool OP6D(DataSet* Pd) {
    ADC(Pd, READA(Pd, AbsoluteAddress(Pd)));
    return true;
}

// ROR $****
__forceinline static bool OP6E(DataSet* Pd) {
    ROR(Pd, GetMemoryPointer(Pd, AbsoluteAddress(Pd)));
    return true;
}

// ADC $******
__forceinline static bool OP6F(DataSet* Pd) {
    ADC(Pd, READA(Pd, AbsoluteLongAddress(Pd)));
    return true;
}
// bvs
__forceinline static bool OP70(DataSet* Pd) {
    BYTE tb = Immediate8(Pd);
    if (Pd->reg.p & FLAG_V) Pd->reg.pc += (char)tb;
    return true;
}

// ADC ($**),y
__forceinline static bool OP71(DataSet* Pd) {
    ADC(Pd, READA(Pd, DirectIndirectY(Pd)));
    return true;
}

// ADC ($**)
__forceinline static bool OP72(DataSet* Pd) {
    ADC(Pd, READA(Pd, DirectIndirect(Pd)));
    return true;
}
// ADC (S),y  しらない
// ADC (S),y I don't know
__forceinline static bool OP73(DataSet* Pd) {
    assert(0);
    return false;
}
// STZ $**,x
__forceinline static bool OP74(DataSet* Pd) {
    STZ(Pd, ZeropageIndexX(Pd));
    return true;
}
// ADC $**,x
__forceinline static bool OP75(DataSet* Pd) {
    ADC(Pd, READA(Pd, ZeropageIndexX(Pd)));
    return true;
}
// ROR $**,x
__forceinline static bool OP76(DataSet* Pd) {
    ROR(Pd, GetMemoryPointer(Pd, ZeropageIndexX(Pd)));
    return true;
}
// ADC [$**],y
__forceinline static bool OP77(DataSet* Pd) {
    ADC(Pd, READA(Pd, DirectIndirectLongY(Pd)));
    return true;
}
// SEI
__forceinline static bool OP78(DataSet* Pd) {
    SET_F(FLAG_I);
    return true;
}

// ADC $****,y
__forceinline static bool OP79(DataSet* Pd) {
    ADC(Pd, READA(Pd, AbsoluteAddressIndexY(Pd)));
    return true;
}

// PLY
__forceinline static bool OP7A(DataSet* Pd) {
    if (IS8Y) {
        Pd->reg.y.b.l = Pd->Pstack[Pd->reg.s - 1];
        Pd->reg.s--;
    } else {
        Pd->reg.y.b.l = Pd->Pstack[Pd->reg.s - 2];
        Pd->reg.y.b.h = Pd->Pstack[Pd->reg.s - 1];
        Pd->reg.s -= 2;
    }
    TESTY();
    return true;
}

// TDC
__forceinline static bool OP7B(DataSet* Pd) {
    Pd->reg.a.w = Pd->reg.d;
    return true;
}
// JMP ($nn****,x)  放置
// JMP ($nn****,x) Leave it alone
__forceinline static bool OP7C(DataSet* Pd) {
    assert(0);
    return false;
}
// ADC $****,x
__forceinline static bool OP7D(DataSet* Pd) {
    ADC(Pd, READA(Pd, AbsoluteAddressIndexX(Pd)));
    return true;
}
// ROR $****,x
__forceinline static bool OP7E(DataSet* Pd) {
    ROR(Pd, GetMemoryPointer(Pd, AbsoluteAddressIndexX(Pd)));
    return true;
}
// ADC $******,x
__forceinline static bool OP7F(DataSet* Pd) {
    ADC(Pd, READA(Pd, AbsoluteLongAddressIndexX(Pd)));
    return true;
}
