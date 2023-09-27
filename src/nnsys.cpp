#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "nnsys.h"




#define MAXINT2CHARBUFFER		20
#define	MAXINT2CHARPLACE		15

char* Int2Char(int num,int place,bool issignadd,int buf)
{
static char buffer[MAXINT2CHARBUFFER][MAXINT2CHARPLACE+2];
static char nullbuf = '\0';
	if(buf < 0 || buf >= MAXINT2CHARBUFFER)return &nullbuf;
	if(place <= 0 || place > MAXINT2CHARPLACE)return &nullbuf;

	int fnum;
	char *tmp;
	if(issignadd)
	{
		//先頭に符号をセット
		buffer[buf][0] = ' ';
		if(num < 0)
		{
			buffer[buf][0] = '-';
			num = -num;
		}
		//おしりにヌル文字を入れる
		buffer[buf][place+1] = '\0';
	
		for(tmp = &buffer[buf][place] ; tmp != &buffer[buf][0] ; tmp--)
		{
			fnum = num % 10;
			num /= 10;
			*tmp = fnum + '0' ;
		}
	}
	else
	{
		if(num < 0)return &nullbuf;
		//おしりにヌル文字を入れる
		buffer[buf][place] = '\0';
	
		for(tmp = &buffer[buf][place-1] ;  ; tmp--)
		{
			fnum = num % 10;
			num /= 10;
			*tmp = fnum + '0' ;
			if(tmp == &buffer[buf][0])break;
		}
	}
	return tmp;
}

#undef MAXINT2CHARBUFFER
#undef MAXINT2CHARPLACE

char* Int2CharEX(int num , int buf )
{
int tmp;
int place;
static char numover = '\0';
	tmp = num;
	if(num < 0)tmp = -num;
	if(tmp < 10)place = 1;
	else if(tmp < 100)place = 2;
	else if(tmp < 1000)place = 3;
	else if(tmp < 10000)place = 4;
	else if(tmp < 100000)place = 5;
	else if(tmp < 1000000)place = 6;
	else if(tmp < 10000000)place = 7;
	else if(tmp < 100000000)place = 8;
	else if(tmp < 1000000000)place = 9;
	else if(tmp < 10000000000)place = 10;
	else return &numover;
	if( num >= 0)
		return Int2Char( num , place , false , buf );
	return Int2Char( num , place , true , buf );
}

#define MAXCHARANDCHARBUFFER		8
#define	MAXCHARANDCHARLENGTH		100

char* CAC(char* str1,char* str2 , int buf){
static char buffer[MAXCHARANDCHARBUFFER][MAXCHARANDCHARLENGTH];
static char nullstr = '\0';
static int  bufloop=0;
char *bptr,*eptr;
	if(buf == -1)
	{
		buf = bufloop;
		bufloop++;
		if(bufloop >= MAXCHARANDCHARBUFFER)bufloop = 0;
	}
	if(buf < 0 || buf >= MAXCHARANDCHARBUFFER)return &nullstr;
	bptr = &buffer[buf][0];
	eptr = &buffer[buf][MAXCHARANDCHARLENGTH-1];
	for( ; ; )
	{
		if(*str1 == '\0')
		{
			break;
		}
		*bptr = *str1;
		bptr++;
		str1++;
		if(bptr == eptr)
		{
			bptr = '\0';
			return &buffer[buf][0];
		}
	}
	for( ; ; )
	{
		if(*str2 == '\0')
		{
			break;
		}
		*bptr = *str2;
		bptr++;
		str2++;
		if(bptr == eptr)
		{
			bptr = '\0';
			return &buffer[buf][0];
		}
	}
	*bptr = '\0';
	return &buffer[buf][0];
}

#undef	MAXCHARANDCHARBUFFER
#undef	MAXCHARANDCHARLENGTH
#define MAXCCBUFFER		256
char* CharacterChain(int NOchar , ... )
{
	static char buffer[MAXCCBUFFER];
	va_list vap;

	va_start( vap , NOchar );

	int bs;
	char *Pstr;
	bs = 0;
	for( int i=0 ; i<NOchar ; i++ )
	{
		Pstr = va_arg( vap , char* );
		for( int q=0 ; Pstr[q] != '\0' ; q++)
		{
			buffer[bs] = Pstr[q];
			bs++;
			if(bs >= MAXCCBUFFER)bs = MAXCCBUFFER-1;
		}
	}
	buffer[bs] = '\0';

	va_end( vap );
	return buffer;
}
#undef MAXCCBUFFER

char* Int2CharE( char* Pbuf , int buflength , int num,int place,bool issignadd )
{
static char nullbuf = '\0';
	if(!Pbuf)return &nullbuf;
	if(buflength <= 0 )return &nullbuf;

	int fnum;
	char *tmp;
	if(issignadd)
	{
		//先頭に符号をセット
		Pbuf[0] = ' ';
		if(num < 0)
		{
			Pbuf[0] = '-';
			num = -num;
		}
		//おしりにヌル文字を入れる
		Pbuf[place+1] = '\0';
	
		for(tmp = &Pbuf[place] ; tmp != &Pbuf[0] ; tmp--)
		{
			fnum = num % 10;
			num /= 10;
			*tmp = fnum + '0' ;
		}
	}
	else
	{
		if(num < 0)return &nullbuf;
		//おしりにヌル文字を入れる
		Pbuf[place] = '\0';
	
		for(tmp = &Pbuf[place-1] ;  ; tmp--)
		{
			fnum = num % 10;
			num /= 10;
			*tmp = fnum + '0' ;
			if(tmp == &Pbuf[0])break;
		}
	}
	return Pbuf;
}
char* Int2CharEXE(char* Pbuf , int buflength , int num )
{
int tmp;
int place;
static char numover = '\0';
	tmp = num;
	if(num < 0)tmp = -num;
	if(tmp < 10)place = 1;
	else if(tmp < 100)place = 2;
	else if(tmp < 1000)place = 3;
	else if(tmp < 10000)place = 4;
	else if(tmp < 100000)place = 5;
	else if(tmp < 1000000)place = 6;
	else if(tmp < 10000000)place = 7;
	else if(tmp < 100000000)place = 8;
	else if(tmp < 1000000000)place = 9;
	else if(tmp < 10000000000)place = 10;
	else return &numover;
	if( num >= 0)
		return Int2CharE( Pbuf , buflength , num , place , false  );
	return Int2CharE( Pbuf , buflength , num , place , true );
}
char* CACE(char* Pbuf , int buflength , char* str1 , char* str2 )
{
static char nullstr = '\0';
char *bptr,*eptr;
	if(!Pbuf)return &nullstr;
	if(buflength<=0)return &nullstr;

	bptr = &Pbuf[0];
	eptr = &Pbuf[buflength-1];
	for( ; ; )
	{
		if(*str1 == '\0')
		{
			break;
		}
		*bptr = *str1;
		bptr++;
		str1++;
		if(bptr == eptr)
		{
			bptr = '\0';
			return Pbuf;
		}
	}
	for( ; ; )
	{
		if(*str2 == '\0')
		{
			break;
		}
		*bptr = *str2;
		bptr++;
		str2++;
		if(bptr == eptr)
		{
			bptr = '\0';
			return Pbuf;
		}
	}
	*bptr = '\0';
	return Pbuf;
}
char* CharacterChainE(char* Pbuf , int buflength , int NOchar , ... )
{
static char nullstr = '\0';
	va_list vap;
	if(!Pbuf)return &nullstr;
	if(buflength<=0)return &nullstr;

	va_start( vap , NOchar );

	int bs;
	char *Pstr;
	bs = 0;
	for( int i=0 ; i<NOchar ; i++ )
	{
		Pstr = va_arg( vap , char* );
		for( int q=0 ; Pstr[q] != '\0' ; q++)
		{
			Pbuf[bs] = Pstr[q];
			bs++;
			if(bs >= buflength)bs = buflength-1;
		}
	}
	Pbuf[bs] = '\0';

	va_end( vap );
	return Pbuf;
}

static char hextable[17] = "0123456789ABCDEF";

char *Byte2Hex(unsigned char data,int ub)
{
static char buf[10][3];
	buf[ub][0] = hextable[data>>4];
	buf[ub][1] = hextable[data&0x0F];
	buf[ub][2] = '\0';
	return buf[ub];
}
char *Int2Hex16(unsigned short data,int ub)
{
static char buf[10][5];
	buf[ub][0] = hextable[(data>>12)];
	buf[ub][1] = hextable[(data>> 8)&0x0F];
	buf[ub][2] = hextable[(data>> 4)&0x0F];
	buf[ub][3] = hextable[(data>> 0)&0x0F];
	buf[ub][4] = '\0';
	return buf[ub];
}
char *Int2Hex32(unsigned int data,int ub)
{
static char buf[10][9];
	buf[ub][0] = hextable[(data>>28)];
	buf[ub][1] = hextable[(data>>24)&0x0F];
	buf[ub][2] = hextable[(data>>20)&0x0F];
	buf[ub][3] = hextable[(data>>16)&0x0F];
	buf[ub][4] = hextable[(data>>12)&0x0F];
	buf[ub][5] = hextable[(data>> 8)&0x0F];
	buf[ub][6] = hextable[(data>> 4)&0x0F];
	buf[ub][7] = hextable[(data>> 0)&0x0F];
	buf[ub][8] = '\0';
	return buf[ub];
}


double AimAngle(double EEX,double EEY,double MEX,double MEY){
double TX,TY;
double temp;
TX = (MEX - EEX);//先が右側の時＋
TY = (MEY - EEY);//先が下側の時＋
if(MYABS(TX) > 0.000001){
	temp = (double)atan(TY / TX);
	if(TX<0)temp+=PI;
	while(temp < 0)temp += 2*PI;
	while(temp > 2*PI) temp -= 2*PI;
	return temp;
}else{
	if(TY > 0)return (double)PI/2;
	if(TY < 0)return (double)PI*3/2;
}
	return -1;
}









static class sintableclass
{
private:
	double *MPsintable;
	int     Msintablesenv;
public:
	sintableclass()
	{
		MPsintable=NULL;
		Msintablesenv=0;
	}
	~sintableclass()
	{
		if(MPsintable)
		{
			GlobalFree(MPsintable);
			MPsintable = NULL;
			Msintablesenv = 0;
		}
	}
	void   Set(int senv)
	{
		if(senv > 0)
		{
			if(MPsintable)GlobalFree(MPsintable);
			MPsintable = (double *)GlobalAlloc(GMEM_FIXED , sizeof(double)*senv);
			for(int i=0 ; i<senv ; i++){
				MPsintable[i] = sin(2*PI/senv*i);
			}
		}
	}
	double Get(int angle)
	{
		return MPsintable[angle];
	}
	int GetSenv()
	{
		return Msintablesenv;
	}
}sintable;

void MakeSinTable(int sintablesenv){
	sintable.Set(sintablesenv);
}
double ESin(int angle){
	while(angle < 0)return -ESin(-angle);
	angle %= sintable.GetSenv();
	return sintable.Get(angle);
}
double ECos(int angle){
	return ESin(angle + sintable.GetSenv()/4);
}

int GetSinTableSenv()
{
	return sintable.GetSenv();
}





static bool rndtablebemade=false ;

static unsigned int rndtable[33] ;
static int rndptr;
static unsigned char *Pex_rnd ;


unsigned int mrnd()
{
	if( !rndtablebemade )smrnd( 34057 , NULL ) ;
	rndptr %= 33 ;
int pos ;
	if( !Pex_rnd )
		pos = rndptr+9 ;
	else
		pos = rndptr+6+((*Pex_rnd)*59)%9 ;
	pos %= 33 ;
	rndtable[ rndptr ] += rndtable[pos] ;
	rndptr++ ;

	return rndtable[ rndptr-1 ] ;
}
double dmrnd()
{
	return mrnd()/(double)0x100000000 ;
}
void smrnd( int seed , unsigned char *Pes )
{
int i ;
	rndtablebemade = true ;
	Pex_rnd = Pes ;
	rndptr = 15 ;
	rndtable[0] = seed ;
	for( i=1 ; i<33 ; i++ )
		rndtable[i] = 0x46A2E297*rndtable[i-1] + 0x00EF353B;
}

unsigned int mrnd(MYRND_PARAM *Pmrp)
{
	Pmrp->rndptr %= 33 ;
int pos ;
	if( !Pmrp->Pex_rnd )
		pos = Pmrp->rndptr+9 ;
	else
		pos = Pmrp->rndptr+6+((*Pmrp->Pex_rnd)*59)%9 ;
	pos %= 33 ;
	Pmrp->rndtable[ Pmrp->rndptr ] += Pmrp->rndtable[pos] ;
	Pmrp->rndptr++ ;

	return Pmrp->rndtable[ Pmrp->rndptr-1 ] ;
}
double dmrnd(MYRND_PARAM *Pmrp)
{
	return mrnd(Pmrp)/(double)0x100000000 ;
}
void smrnd( MYRND_PARAM *Pmrp , int seed , unsigned char *Pes )
{
int i ;
	Pmrp->Pex_rnd = Pes ;
	Pmrp->rndptr = 15 ;
	Pmrp->rndtable[0] = seed ;
	for( i=1 ; i<33 ; i++ )
		Pmrp->rndtable[i] = 0x46A2E297*Pmrp->rndtable[i-1] + 0x00EF353B;
}


//かなりテキトーな２４ビットハッシュ（？）値作成
unsigned int EHash( unsigned char *Pd , int length )
{
unsigned int h;
int i;
	h = 0;
	for ( i=0 ; i<length ; i++) {
		h = h * 137 + Pd[i] ;
	}
	return h % 0x01000000;
}
