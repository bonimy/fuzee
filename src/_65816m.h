#ifndef _65816_M_HEADER_INCLUDED
#define _65816_M_HEADER_INCLUDED

#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef WORD
typedef unsigned short WORD;
#endif

#if 1
#define inline
#endif


#ifdef _DEBUG
#define MEMORY_CHECK
#define TRACER_ENABLED
#endif


#ifdef TRACER_ENABLED
#define TRACE_SIZE 0x400
#endif

#define PICTURE_TEST

#ifdef PICTURE_TEST
#define PICTURE_BUFFER_SIZE 0x10000
#endif

typedef struct Register65816_tag {
    union {
        WORD w;
        struct {
            BYTE l;
            BYTE h;
        } b;
    } a;
    BYTE db;
    union {
        WORD w;
        struct {
            BYTE l;
            BYTE h;
        } b;
    } x;
    union {
        WORD w;
        struct {
            BYTE l;
            BYTE h;
        } b;
    } y;
    WORD d;
    WORD s;
    BYTE p;
    BYTE pb;
    WORD pc;
} Register65816;

typedef struct DataSet_tag {
    Register65816 reg;
    BYTE* Pram;
    BYTE* Prom;
    BYTE* Pstack;
    BYTE AADMA[8][0x10];
    BYTE DUMMY_BYTE;
    bool Is64KBank;
#ifdef MEMORY_CHECK
    BYTE* Pramc;
#endif
#ifdef PICTURE_TEST
    BYTE Avram[PICTURE_BUFFER_SIZE];
    int render_point;
    int render_inc_size;
    int render_inc_timing;
#endif
} DataSet;
class EmuCore {
protected:
    DataSet d;
#ifdef TRACER_ENABLED
    int trace_buffer[TRACE_SIZE];
    int trace_point;
#endif
    bool OperateOnce();

public:
    EmuCore();
    ~EmuCore() { Release(); }
    void Initialize();
    void ClearRegister();
    void Release();
    void SetRomData(BYTE* Pin) { d.Prom = Pin; }
    void SetBankSize(bool is64) { d.Is64KBank = is64; }
};

class EmuCoreMinus : public EmuCore {
private:
public:
    void ShowOpPos();
    bool AnyTimesOperate(int NOop);
    bool OperateUntilRTS(int limit);

    int GetPC() { return (d.reg.pb << 16) | d.reg.pc; }
    WORD GetA() { return d.reg.a.w; }
    WORD GetX() { return d.reg.x.w; }
    WORD GetY() { return d.reg.y.w; }
    BYTE GetDB() { return d.reg.db; }
    BYTE GetP() { return d.reg.p; }
    WORD GetS() { return d.reg.s; }

    void WritePC(int val) {
        d.reg.pb = val >> 16;
        d.reg.pc = val & 0xFFFF;
    }
    void WriteA(WORD val) { d.reg.a.w = val; }
    void WriteX(WORD val) { d.reg.x.w = val; }
    void WriteY(WORD val) { d.reg.y.w = val; }
    void WriteDB(BYTE val) { d.reg.db = val; }
    void WriteP(BYTE val) { d.reg.p = val; }

    void WriteAdr(int adr, BYTE val);
    void WriteAdrW(int adr, WORD val);

    void Push8(BYTE val) {
        d.Pstack[d.reg.s] = val;
        d.reg.s++;
    }
    BYTE GetAdr(int adr);
    BYTE* GetRAMPointer() { return d.Pram; }
    void EasyInitialize(int PBPC, BYTE p) {
        d.reg.pb = PBPC >> 16;
        d.reg.pc = PBPC & 0xFFFF;
        d.reg.p = p;
    };
#ifdef PICTURE_TEST
    BYTE* GetVRAMPointer() { return &d.Avram[0]; }
#endif
};


#endif /*_65816_M_HEADER_INCLUDED*/