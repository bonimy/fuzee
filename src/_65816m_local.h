#include <assert.h>

#ifndef _65816_MACRO_HEADER_INCLUDED
#define _65816_MACRO_HEADER_INCLUDED

#include "_65816m.h"

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_V 0x40
#define FLAG_N 0x80

#define SET_F(fl) (Pd->reg.p |= (fl))
#define CLR_F(fl) (Pd->reg.p &= ~(fl))
#define IS_SET_F(fl) (Pd->reg.p & (fl))
#define IS_CLR_F(fl) ((Pd->reg.p & (fl)) == 0)

#define IS8A (Pd->reg.p & 0x20)
#define IS16A ((Pd->reg.p & 0x20) == 0)
#define IS8X (Pd->reg.p & 0x10)
#define IS16X ((Pd->reg.p & 0x10) == 0)
#define IS8Y (Pd->reg.p & 0x10)
#define IS16Y ((Pd->reg.p & 0x10) == 0)
//#define		IS8AXY		(IS8A&&IS8X)
//#define		IS16AXY		(IS16A&&IS16X)


#define PCLEAR \
    { CLR_F(FLAG_Z | FLAG_V | FLAG_N); }
//#define		PCLEARA		{CLR_F(FLAG_Z|FLAG_V|FLAG_N);}

#define TESTA()        \
    {                  \
        if (IS8A) {    \
            TESTA8();  \
        } else {       \
            TESTA16(); \
        }              \
    }
#define TESTX()        \
    {                  \
        if (IS8X) {    \
            TESTX8();  \
        } else {       \
            TESTX16(); \
        }              \
    }
#define TESTY()        \
    {                  \
        if (IS8Y) {    \
            TESTY8();  \
        } else {       \
            TESTY16(); \
        }              \
    }
#define TESTNUM(num)                 \
    {                                \
        if (IS8A) {                  \
            PCLEAR;                  \
            if (!num) SET_F(FLAG_Z); \
        } else {                     \
            PCLEAR;                  \
            if (!num) SET_F(FLAG_Z); \
        }                            \
    }

#define TEST8(num)                     \
    {                                  \
        PCLEAR;                        \
        if (!num) SET_F(FLAG_Z);       \
        if (num & 0x80) SET_F(FLAG_N); \
    }
#define TEST16(num)                      \
    {                                    \
        PCLEAR;                          \
        if (!num) SET_F(FLAG_Z);         \
        if (num & 0x8000) SET_F(FLAG_N); \
    }

#define TESTX8() \
    { TEST8(Pd->reg.x.b.l); }
#define TESTX16() \
    { TEST16(Pd->reg.x.w); }
#define TESTY8() \
    { TEST8(Pd->reg.y.b.l); }
#define TESTY16() \
    { TEST16(Pd->reg.y.w); }
#define TESTA8() \
    { TEST8(Pd->reg.a.b.l); }
#define TESTA16() \
    { TEST16(Pd->reg.a.w); }


///////////////////////////////////////////////////////

/*

//（ＶＣ＋＋）
//__forceinlineにしないとインライン展開してくれない……
//けど、#ifndef __forceinlineは飛ばしてくれない……
//非ＶＣでコンパイルできるようにするにはどーしたらいいのよ

// (VC++)
// If you don't specify __forceinline, it won't be inline expanded...
// But #ifndef __forceinline doesn't work...
// How can I handle this with non-VC?

#ifndef __forceinline
#define __forceinline inline
#endif

#ifndef inline
#define inline
#endif
*/


//作ったが、使い方を間違えて大変なことになった
// I made it, but I used it incorrectly and ended up in trouble.
inline static int TAdr2Mem(int adr) {
    if ((adr & 0xFF0000) == 0x7E0000) {
        return adr & 0x00FFFF;
    } else if ((adr & 0xFF0000) == 0x7F0000) {
        return (adr & 0x00FFFF) | 0x010000;
    }
    if (adr < 0x400000) {
        if ((adr & 0x00FFFF) < 0x2000) {
            return adr & 0x00FFFF;
        }
    }
    return -1;
}

inline static BYTE READB(DataSet* Pd, int adr) {
#ifdef MEMORY_CHECK
    int tm;
    tm = TAdr2Mem(adr);
    if (tm != -1 && Pd->Pramc[tm] == 0) {
        static FILE* fp = 0;
        if (!fp) fp = fopen("memory_test.txt", "wt");
        fprintf(fp, "%.8X   %.8X\n", tm, (Pd->reg.pb << 16) | Pd->reg.pc);
        Pd->Pramc[tm] = 1;
    }
#endif
    if (Pd->Is64KBank) {
        int bank, loadr, pro;
        bank = adr >> 16;
        loadr = adr & 0xFFFF;
        if (bank < 0x40)
            pro = 0;
        else if (bank < 0x60)
            pro = 1;
        else if (bank < 0x70)
            pro = -1;
        else if (bank < 0x78)
            pro = 2;
        else if (bank < 0x7E)
            pro = -1;
        else if (bank < 0x80)
            pro = 3;
        else if (bank < 0xC0)
            pro = 0;
        else
            pro = 1;
        switch (pro) {
            case -1:
                return 0;
                break;
            case 0: {
                if (loadr >= 0x8000) return Pd->Prom[((bank % 0x40) << 16) | loadr];
                if (loadr >= 0x6000) {
                    if (bank >= 0x30)
                        return 0;
                    else
                        return 0;
                }
                if (loadr < 0x2000) return Pd->Pram[loadr];
                // 2000-5FFF
            } break;
            case 1: {
                bank %= 0x40;
                return Pd->Prom[(bank << 16) | loadr];
            } break;
            case 2: {
                if (loadr >= 0x8000) return 0;
                // 0000-7FFF
            } break;
            case 3: {
                bank -= 0x7E;
                return Pd->Pram[(bank << 16) | loadr];
            } break;
            default:
                assert(0);
        }
    } else {
        if ((adr & 0xFF0000) == 0x7E0000)
            return Pd->Pram[adr & 0x00FFFF];
        else if ((adr & 0xFF0000) == 0x7F0000)
            return Pd->Pram[(adr & 0x00FFFF) | 0x010000];
        if ((adr & 0x00FFFF) >= 0x8000) {
            return Pd->Prom[((adr >> 16) & 0x7F) * 0x8000 + (adr & 0x7FFF)];
        }
        if (adr < 0x400000) {
            if ((adr & 0x00FFFF) < 0x2000) return Pd->Pram[adr & 0x00FFFF];
        }
    }
    //しらん
    // I do not know
    return 0;
}
inline static WORD READW(DataSet* Pb, int adr) {
    return READB(Pb, adr) | (READB(Pb, adr + 1) << 8);
}
inline static WORD READA(DataSet* Pd, int adr) {
    if (IS8A) return READB(Pd, adr);
    return READW(Pd, adr);
}
inline static WORD READX(DataSet* Pd, int adr) {
    if (IS8X) return READB(Pd, adr);
    return READW(Pd, adr);
}
inline static WORD READY(DataSet* Pd, int adr) {
    if (IS8Y) return READB(Pd, adr);
    return READW(Pd, adr);
}
/*

inline static WORD READAXY( DataSet *Pd , int adr )
{
        if(!IS16AXY) return READB( Pd , adr );
        else return READW( Pd , adr );
}
*/
inline static void WRITEB(DataSet* Pd, int adr, BYTE data) {
#ifdef MEMORY_CHECK
    int tm;
    tm = TAdr2Mem(adr);
    if (tm != -1) {
        Pd->Pramc[tm] = 1;
    }
#endif
    if (Pd->Is64KBank) {
        int bank, loadr, pro;
        bank = adr >> 16;
        loadr = adr & 0xFFFF;
        if (bank < 0x40)
            pro = 0;
        else if (bank < 0x60)
            pro = 1;
        else if (bank < 0x70)
            pro = -1;
        else if (bank < 0x78)
            pro = 2;
        else if (bank < 0x7E)
            pro = -1;
        else if (bank < 0x80)
            pro = 3;
        else if (bank < 0xC0)
            pro = 0;
        else
            pro = 1;
        switch (pro) {
            case 0: {
                if (loadr >= 0x8000) Pd->Prom[(bank << 16) | loadr] = data;
                if (loadr >= 0x6000) {
                    if (bank >= 0x30)
                        return;
                    else
                        return;
                }
                if (loadr < 0x2000) Pd->Pram[loadr] = data;
                // 2000-5FFF
            } break;
            case 1: {
                bank %= 0x40;
                Pd->Prom[(bank << 16) | loadr] = data;
            } break;
            case 2: {
                if (loadr >= 0x8000) return;
                // 0000-7FFF
            } break;
            case 3: {
                bank -= 0x7E;
                Pd->Pram[(bank << 16) | loadr] = data;
            } break;
            default:
                assert(0);
        }
    } else {
        if ((adr & 0xFF0000) == 0x7E0000) {
            Pd->Pram[adr & 0x00FFFF] = data;
            return;
        } else if ((adr & 0xFF0000) == 0x7F0000) {
            Pd->Pram[(adr & 0x00FFFF) | 0x010000] = data;
            return;
        }
        if (adr % 0x800000 < 0x400000) {
            //ミラーだからと、あと、バンクにかかわりないらしいから
            // Because it's a mirror, and because it doesn't seem to be related to banks
            adr %= 0x010000;
            // lo ram
            if (adr < 0x2000) {
                Pd->Pram[adr] = data;
                return;
            }
            // DMA関連
            // DMA related
            if (adr >= 0x4300 && adr < 0x4380) {
                Pd->AADMA[(adr & 0x0070) >> 4][adr & 0x000F] = data;
                return;
            }
            // DMA start
            if (adr == 0x420B) {
                static BYTE bittable[8] = {0x01, 0x02, 0x04, 0x08,
                                           0x10, 0x20, 0x40, 0x80};
                int q, end;
                for (int i = 0; i < 8; i++) {
                    if (bittable[i] & data) {
                        //必要に応じて作っただけなので、ボロボロ
                        // I only made it as needed, so it's falling apart
                        if (Pd->AADMA[i][0] & 0x80) return;  // reg->mem
                        if (Pd->AADMA[i][0] & 0x08) return;  // decrement
                        end = Pd->AADMA[i][5] | (Pd->AADMA[i][6] << 8);
                        switch (Pd->AADMA[i][0] & 0x03) {
                            case 0x00:
                                for (q = 0; q < end; q++) {
                                    WRITEB(Pd, 0x2100 | Pd->AADMA[i][1],
                                           READB(Pd, q + (Pd->AADMA[i][2] |
                                                          (Pd->AADMA[i][3] << 8) |
                                                          (Pd->AADMA[i][4] << 16))));
                                }
                                break;
                            case 0x01:
                                for (q = 0; q < end;) {
                                    WRITEB(Pd, 0x2100 | Pd->AADMA[i][1],
                                           READB(Pd, q + (Pd->AADMA[i][2] |
                                                          (Pd->AADMA[i][3] << 8) |
                                                          (Pd->AADMA[i][4] << 16))));
                                    q++;
                                    if (q >= end) break;
                                    WRITEB(Pd, (0x2100 | Pd->AADMA[i][1]) + 1,
                                           READB(Pd, q + (Pd->AADMA[i][2] |
                                                          (Pd->AADMA[i][3] << 8) |
                                                          (Pd->AADMA[i][4] << 16))));
                                    q++;
                                }

                                break;
                            default:
                                return;
                        }
                    }
                }
            }
#ifdef PICTURE_TEST
            static BYTE inc_size_table[16] = {1, 32, 64, 128, 8, 0, 0, 0,
                                              8, 0,  0,  0,   8, 0, 0, 0};
            switch (adr) {
                // VRAM インクリメントサイズ
                // VRAM increment size
                case 0x2115:
                    Pd->render_inc_size = inc_size_table[data & 0x0F];
                    Pd->render_inc_timing = 0;
                    if (data & 0x80) Pd->render_inc_timing = 1;
                    break;
                case 0x2116:
                    Pd->render_point = (Pd->render_point & 0xFF00) | data;
                    break;
                case 0x2117:
                    Pd->render_point = (Pd->render_point & 0x00FF) | (data << 8);
                    break;
                case 0x2118:
                    Pd->Avram[Pd->render_point * 2] = data;
                    if (Pd->render_inc_timing == 0)
                        Pd->render_point += Pd->render_inc_size;
                    break;
                case 0x2119:
                    Pd->Avram[Pd->render_point * 2 + 1] = data;
                    if (Pd->render_inc_timing) Pd->render_point += Pd->render_inc_size;
                    break;
                default:
                    adr = adr;
            }
#endif
        }
    }
    return;
}
inline static void WRITEW(DataSet* Pd, int adr, WORD data) {
    WRITEB(Pd, adr, data & 0xFF);
    WRITEB(Pd, adr + 1, data >> 8);
}
inline static void WRITEA(DataSet* Pd, int adr, WORD data) {
    if (IS8A) {
        WRITEB(Pd, adr, (BYTE)data);
        return;
    }
    WRITEW(Pd, adr, data);
    return;
}
inline static void WRITEX(DataSet* Pd, int adr, WORD data) {
    if (IS8X) {
        WRITEB(Pd, adr, (BYTE)data);
        return;
    }
    WRITEW(Pd, adr, data);
    return;
}
inline static void WRITEY(DataSet* Pd, int adr, WORD data) {
    if (IS8Y) {
        WRITEB(Pd, adr, (BYTE)data);
        return;
    }
    WRITEW(Pd, adr, data);
    return;
}
inline static BYTE* GetMemoryPointer(DataSet* Pd, int adr) {
    if ((adr & 0xFF0000) == 0x7E0000) {
        return &Pd->Pram[adr & 0x00FFFF];
    } else if ((adr & 0xFF0000) == 0x7F0000) {
        return &Pd->Pram[(adr & 0x00FFFF) | 0x010000];
    }
    if (adr < 0x400000) {
        if ((adr & 0x00FFFF) < 0x2000) {
            return &Pd->Pram[adr & 0x00FFFF];
        }
    }
    return &Pd->DUMMY_BYTE;
}

inline static BYTE GetROMinPC(DataSet* Pd) {
    return READB(Pd, (Pd->reg.pb * 0x10000) | (Pd->reg.pc));
}
inline static BYTE GetROMinPCinc(DataSet* Pd) {
    BYTE tb;
    tb = READB(Pd, (Pd->reg.pb * 0x10000) | (Pd->reg.pc));
    Pd->reg.pc++;
    return tb;
}


#endif /*_65816_MACRO_HEADER_INCLUDED*/