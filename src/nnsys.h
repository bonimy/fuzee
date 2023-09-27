#ifndef NNSYS_HEADER_INCLUDED
#define NNSYS_HEADER_INCLUDED

#ifndef PI
#define		PI		3.1415926535897932384626433832795
#endif

#define D2R(ang)		((ang) * PI / 180)//度数法toラジアン
#define R2D(ang)		((ang) * 180 / PI)//ラジアンto度数法


////////////////////////////////
//自作便利（かもしれない）関数郡
////////////////////////////////

/*
公開用に作り直していたら、inlineだらけになってしまった。
*/

//固定小数関連
/*マクロ化用テンプレ
#define	DBITS			16
#define	I2F(i)			I2F(DBITS,i)
#define	F2I(f)			F2I(DBITS,f)
#define	D2F(d)			D2F(DBITS,d)
#define	F2D(f)			F2D(DBITS,f)
#define	FMulti(a,b)		FMulti(DBITS,a,b)
#define	FDivide(a,b)	FDivide(DBITS,a,b)
*/
inline __int32 I2F(int db,int i)
{
	return i<<db;
}
inline __int32 F2I(int db,__int32 f)
{
	return f>>db;
}
inline __int32 D2F(int db,double d)
{
	return (__int32)(d*(1<<db));
}
inline double F2D(int db,__int32 f)
{
	return (double)f/(1<<db) ;
}
inline __int32 FMulti( int db , __int32 a , __int32 b )
{//若干aのほうが精度が高いことが
	return (a>>((db  )/2))*(b>>((db+1)/2)) ;
}
inline __int32 FDivide( int db , __int32 a , __int32 b )
{
	return D2F( db , (double)a/b ) ;
}

//(x,y)が、(srcx,srcy)-(srcx+width,srcy+height),bの内部に存在するか。地味に便利
inline bool RectIn(int x,int y,int srcx,int srcy,int width,int height)
{
	if((srcx <= x) & (srcy <= y) & (srcx + width > x) & (srcy + height > y))return true; 
	return false;
}
inline bool RectIn(double x,double y,double srcx,double srcy,double width,double height)
{
	if((srcx <= x) & (srcy <= y) & (srcx + width > x) & (srcy + height > y))return true; 
	return false;
}

//doubleな角度を１周内に補正
//なんか場合によってやたら重そうだ……
inline void AngleFixPlus(double *Pangle)
{
	while(*Pangle < 0){*Pangle += 2*PI;}
	while(*Pangle >= 2*PI){*Pangle -= 2*PI;}
}
inline void AngleFixEven(double *Pangle)
{
	while(*Pangle < -PI){*Pangle += 2*PI;}
	while(*Pangle >= PI){*Pangle -= 2*PI;}
}
//wayウェイの方向に近いほうに、angleを補正したものを返す
inline double AboutAngle( double angle , double a0 , int way )
{
double eangle = 2*PI/way;
	return ((int)((angle+a0)/eangle+0.5))*eangle+a0 ;
}


//正弦テーブルを作り高速化をはかったらしいのだが……
//マルチスレッドでは使えない
extern void MakeSinTable(int sintablesenv);
extern double ESin(int angle);
extern double ECos(int angle);
extern int GetSinTableSenv();


//sscanfとかsprintfとか知らなかったのよ……本当に。
//マルチスレッドでは使えない
extern char* Int2Char(int num,int place=5,bool issignadd=true,int buf=0);
extern char* Int2CharEX(int num , int buf = 0 );
extern char* CAC(char* str1 , char* str2 , int buf=-1);
extern char* CharacterChain(int NOchar , ... );
extern char* Int2CharE( char* Pbuf , int buflength , int num,int place=5,bool issignadd=true );
extern char* Int2CharEXE(char* Pbuf , int buflength , int num );
extern char* CACE(char* Pbuf , int buflength , char* str1 , char* str2 );
extern char* CharacterChainE(char* Pbuf , int buflength , int NOchar , ... );

extern char *Byte2Hex(unsigned char data,int ub=0);
extern char *Int2Hex16(unsigned short data,int ub=0);
extern char *Int2Hex32(unsigned int data,int ub=0);

//atan2を知らなかった頃に作った奴
extern double AimAngle(double,double,double,double);//２点間の角度

//乱数もどき
typedef struct MYRND_PARAM_tag MYRND_PARAM ;
struct MYRND_PARAM_tag
{
	unsigned int rndtable[33] ;
	int rndptr;
	unsigned char *Pex_rnd ;
};
extern unsigned int mrnd();//0～0xFFFFFFFFまでの整数を返す
extern double dmrnd();//[0,1)の小数を返す
extern void smrnd( int seed , unsigned char *Pes=NULL );//適当なシードと『外乱』へのポインタ（NULL可）を入れる
extern unsigned int mrnd(MYRND_PARAM *Pmrp);//0～0xFFFFFFFFまでの整数を返す
extern double dmrnd(MYRND_PARAM *Pmrp);//[0,1)の小数を返す
extern void smrnd( MYRND_PARAM *Pmrp , int seed , unsigned char *Pes=NULL );//適当なシードと『外乱』へのポインタ（NULL可）を入れる
extern unsigned int EHash( unsigned char *Pd , int length ) ;//かなりテキトーな２４ビットハッシュ（？）値作成



//////////////
//テンプレート
//////////////

//符号を維持したまま、絶対値を足したり引いたり……
//overrun==falseだと、引いたとき原点を通り過ぎたとき、０で止まる
template <class mtp> void AbsPlus(mtp* dest , mtp value , bool overrun=true)
{
	if(*dest > 0)
	{
		*dest += value;
		if(!overrun && *dest < 0)*dest = 0;
	}
	else if(*dest < 0)
	{
		*dest -= value;
		if(!overrun && *dest > 0)*dest = 0;
	}
}
//符号を返す
template <class mtp> int sign( mtp dest )
{
	if( dest>=0 )return 1 ;
	return -1 ;
}
//わかりやすすぎる交換関数
template <class mtp> void MYSWAP(mtp* v1 , mtp* v2)
{
mtp tmp;
	tmp = *v1;
	*v1  = *v2;
	*v2  = tmp;
}

//destを[lo,hi]内に補正する
template <class mtp> void DurCorrect(mtp* dest , mtp valuelo , mtp valuehi )
{
	if( *dest < valuelo ) *dest = valuelo ;
	if( *dest > valuehi ) *dest = valuehi ;
}

//整数を、０～value-1内に補正する。
//-1だったら、value-1になる。
template <class mtp> void RotateCorrect(mtp* dest , mtp value )
{
	*dest = (((*dest)%value)+value)%value;
}

//負での挙動がただの除算と少し異なる整数除算
//正方向から階段を下りていったとき、原点付近が広くならないようになっている
template <class mtp> mtp GaussDivision(mtp dest , mtp value )
{
	if( dest >= 0 )return dest/value;
	dest = -dest-1;
	return -(dest/value)-1 ;
}

////////
//マクロ
////////

//わかりやすく絶対値を返す
#define		MYABS(num)			( (num)<0 ? -(num) : (num) )


#endif /*NNSYS_HEADER_INCLUDED*/