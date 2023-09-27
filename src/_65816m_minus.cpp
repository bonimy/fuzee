#include <stdio.h>

#include "_65816m_local.h"


bool EmuCoreMinus ::AnyTimesOperate(int NOop) {
    for (int i = 0; i < NOop; i++) {
        if (!OperateOnce()) return false;
    }
    return true;
}
bool EmuCoreMinus ::OperateUntilRTS(int limit) {
    for (int i = 0; i < limit; i++) {
        if (!OperateOnce()) return false;
        int op = GetROMinPC(&d);
        if (d.reg.s <= 0 && (op == 0x60 || op == 0x6B)) return true;
    }
    return false;
}

void EmuCoreMinus ::WriteAdr(int adr, BYTE val) { WRITEB(&d, adr, val); }
void EmuCoreMinus ::WriteAdrW(int adr, WORD val) {
    WRITEB(&d, adr, val & 0xFF);
    WRITEB(&d, adr + 1, val >> 8);
}
BYTE EmuCoreMinus ::GetAdr(int adr) { return READB(&d, adr); }
