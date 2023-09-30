#ifndef NNSYS_HEADER_INCLUDED
#define NNSYS_HEADER_INCLUDED

#include <stdio.h>

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

//度数法toラジアン
// degrees to radians
#define D2R(ang) ((ang)*PI / 180)

//ラジアンto度数法
// radians to degrees
#define R2D(ang) ((ang)*180 / PI)


////////////////////////////////
//自作便利（かもしれない）関数郡
// (possibly) useful homemade functions
////////////////////////////////

/*
公開用に作り直していたら、inlineだらけになってしまった。
When I was remaking it for publication, it ended up being full of inlines.
*/

//固定小数関連
// Fixed decimal numbers related
/*
マクロ化用テンプレ
Macro template
#define	DBITS			16
#define	I2F(i)			I2F(DBITS,i)
#define	F2I(f)			F2I(DBITS,f)
#define	D2F(d)			D2F(DBITS,d)
#define	F2D(f)			F2D(DBITS,f)
#define	FMulti(a,b)		FMulti(DBITS,a,b)
#define	FDivide(a,b)	FDivide(DBITS,a,b)
*/
inline __int32 I2F(int db, int i) { return i << db; }
inline __int32 F2I(int db, __int32 f) { return f >> db; }
inline __int32 D2F(int db, double d) { return (__int32)(d * (1 << db)); }
inline double F2D(int db, __int32 f) { return (double)f / (1 << db); }
//若干aのほうが精度が高いことが
// A is slightly more accurate
inline __int32 FMulti(int db, __int32 a, __int32 b) {
    return (a >> ((db) / 2)) * (b >> ((db + 1) / 2));
}
inline __int32 FDivide(int db, __int32 a, __int32 b) { return D2F(db, (double)a / b); }

//(x,y)が、(srcx,srcy)-(srcx+width,srcy+height),bの内部に存在するか。地味に便利
// Does (x,y) exist inside (srcx,srcy)-(srcx+width,srcy+height),b? Quite handy
inline bool RectIn(int x, int y, int srcx, int srcy, int width, int height) {
    if ((srcx <= x) & (srcy <= y) & (srcx + width > x) & (srcy + height > y))
        return true;
    return false;
}
inline bool RectIn(double x, double y, double srcx, double srcy, double width,
                   double height) {
    if ((srcx <= x) & (srcy <= y) & (srcx + width > x) & (srcy + height > y))
        return true;
    return false;
}

// doubleな角度を１周内に補正
//なんか場合によってやたら重そうだ……
// Correct the double angle to within one rotation
// It seems to be quite heavy depending on the situation...
inline void AngleFixPlus(double* Pangle) {
    while (*Pangle < 0) {
        *Pangle += 2 * PI;
    }
    while (*Pangle >= 2 * PI) {
        *Pangle -= 2 * PI;
    }
}
inline void AngleFixEven(double* Pangle) {
    while (*Pangle < -PI) {
        *Pangle += 2 * PI;
    }
    while (*Pangle >= PI) {
        *Pangle -= 2 * PI;
    }
}
// wayウェイの方向に近いほうに、angleを補正したものを返す
// Returns the corrected angle closer to the way direction
inline double AboutAngle(double angle, double a0, int way) {
    double eangle = 2 * PI / way;
    return ((int)((angle + a0) / eangle + 0.5)) * eangle + a0;
}


//正弦テーブルを作り高速化をはかったらしいのだが……
//マルチスレッドでは使えない
// It seems they created a sine table to speed up the process...
// Cannot be used with multithreading
extern void MakeSinTable(int sintablesenv);
extern double ESin(int angle);
extern double ECos(int angle);
extern int GetSinTableSenv();


// sscanfとかsprintfとか知らなかったのよ……本当に。
//マルチスレッドでは使えない
// I didn't know about sscanf or sprintf...really.
// Cannot be used with multithreading
extern wchar_t* Int2Char(int num, int place = 5, bool issignadd = true, int buf = 0);
extern wchar_t* Int2CharEX(int num, int buf = 0);
extern wchar_t* FZ_CAC(wchar_t* str1, wchar_t* str2, int buf = -1);
extern wchar_t* CharacterChain(int NOchar, ...);
extern wchar_t* Int2CharE(wchar_t* Pbuf, int buflength, int num, int place = 5,
                       bool issignadd = true);
extern wchar_t* Int2CharEXE(wchar_t* Pbuf, int buflength, int num);
extern wchar_t* CACE(wchar_t* Pbuf, int buflength, wchar_t* str1, wchar_t* str2);
extern wchar_t* CharacterChainE(wchar_t* Pbuf, int buflength, int NOchar, ...);

extern wchar_t* Byte2Hex(unsigned char data, int ub = 0);
extern wchar_t* Int2Hex16(unsigned short data, int ub = 0);
extern wchar_t* Int2Hex32(unsigned int data, int ub = 0);

// atan2を知らなかった頃に作った奴
// I made this before I knew about atan2
//２点間の角度
// Angle between two points
extern double AimAngle(double, double, double, double);

//乱数もどき
// Random number imitation
typedef struct MYRND_PARAM_tag MYRND_PARAM;
struct MYRND_PARAM_tag {
    unsigned int rndtable[33];
    int rndptr;
    unsigned char* Pex_rnd;
};
// 0～0xFFFFFFFFまでの整数を返す
// Returns an integer between 0 and 0xFFFFFFFF
extern unsigned int mrnd();
//[0,1)の小数を返す
// Return the decimal number of [0,1)
extern double dmrnd();
//適当なシードと『外乱』へのポインタ（NULL可）を入れる
// Insert an appropriate seed and a pointer to "disturbance" (can be NULL)
extern void smrnd(int seed, unsigned char* Pes = NULL);
// 0～0xFFFFFFFFまでの整数を返す
// Returns an integer between 0 and 0xFFFFFFFF
extern unsigned int mrnd(MYRND_PARAM* Pmrp);
//[0,1)の小数を返す
// Return the decimal number of [0,1)
extern double dmrnd(MYRND_PARAM* Pmrp);
//適当なシードと『外乱』へのポインタ（NULL可）を入れる
// Insert an appropriate seed and a pointer to "disturbance" (can be NULL)
extern void smrnd(MYRND_PARAM* Pmrp, int seed, unsigned char* Pes = NULL);
//かなりテキトーな２４ビットハッシュ（？）値作成
// Create a fairly simple 24-bit hash(?) value
extern unsigned int EHash(unsigned char* Pd, int length);


//////////////
//テンプレート
// template
//////////////

//符号を維持したまま、絶対値を足したり引いたり……
// overrun==falseだと、引いたとき原点を通り過ぎたとき、０で止まる
// Add or subtract the absolute value while maintaining the sign...
// If overrun==false, it will stop at 0 when it passes the origin when pulled
template <class mtp>
void AbsPlus(mtp* dest, mtp value, bool overrun = true) {
    if (*dest > 0) {
        *dest += value;
        if (!overrun && *dest < 0) *dest = 0;
    } else if (*dest < 0) {
        *dest -= value;
        if (!overrun && *dest > 0) *dest = 0;
    }
}
//符号を返す
// return the sign
template <class mtp>
int sign(mtp dest) {
    if (dest >= 0) return 1;
    return -1;
}
//わかりやすすぎる交換関数
// Too easy to understand exchange function
template <class mtp>
void MYSWAP(mtp* v1, mtp* v2) {
    mtp tmp;
    tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
}

// destを[lo,hi]内に補正する
// Correct dest to within [lo,hi]
template <class mtp>
void DurCorrect(mtp* dest, mtp valuelo, mtp valuehi) {
    if (*dest < valuelo) *dest = valuelo;
    if (*dest > valuehi) *dest = valuehi;
}

//整数を、０～value-1内に補正する。
//-1だったら、value-1になる。
// Correct the integer to within 0 to value-1.
// If it is -1, it becomes value-1.
template <class mtp>
void RotateCorrect(mtp* dest, mtp value) {
    *dest = (((*dest) % value) + value) % value;
}

//負での挙動がただの除算と少し異なる整数除算
//正方向から階段を下りていったとき、原点付近が広くならないようになっている
// Integer division behaves slightly differently on negative values ​​than just
// division. When stepping down from the forward direction, the area around the origin
// does not become wider
template <class mtp>
mtp GaussDivision(mtp dest, mtp value) {
    if (dest >= 0) return dest / value;
    dest = -dest - 1;
    return -(dest / value) - 1;
}

////////
//マクロ
// macro
////////

//わかりやすく絶対値を返す
// Return the absolute value in an easy-to-understand way
#define MYABS(num) ((num) < 0 ? -(num) : (num))


#endif /*NNSYS_HEADER_INCLUDED*/
