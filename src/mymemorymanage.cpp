/*
	よく意図のわからないメモリ管理ルーチン。
*/
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include <assert.h>


#define mymemorymanage_body
#include "mymemorymanage.h"



//一息で読む量
#define READLENGTH	0x00100000

static int ret;
#define	ERRRET(tok,num)		{ret=-num;goto tok##num;}


/*	ファイルから空のptrbufにメモリを割り当てかつロード。
	ファイルサイズを返す。
	-1	バッファが空でない
	-2　一時バッファが確保できない
	-3　ファイルがオープンできない
	-4　ファイルが読めない
	-5　一時バッファを拡張できない
	-6　返すべきバッファが確保できない
*/
int LoadMemoryFromFile(char *filename , unsigned char **ptrbuf)
{
	//バッファをチェック
	if(*ptrbuf != NULL)return -1 ;

	FILE *fp ;
	fp = fopen( filename , "r" ) ;
	if( !fp ) return -3 ;

	int size ;
	fseek( fp , 0 , SEEK_END ) ;
	size = ftell( fp ) ;
	fseek( fp , 0 , SEEK_SET ) ;

	//実際に返すバッファを確保
	*ptrbuf = (unsigned char*)malloc(size);
	if(*ptrbuf == NULL){fclose(fp);return -6;}

	fread( *ptrbuf , 1 , size , fp ) ;

	fclose( fp ) ;
	return size ;
}
#if 0
//旧式というか、なんでこんなに冗長な記述をしていたのだろう
//ファイルサイズを求める方法を知らなかった？
int LoadMemoryFromFile(char *filename , unsigned char **ptrbuf)
{
int filehand;
int tmplength;
int tmplength_sum;
unsigned char *tmpbuf;
unsigned char *tmptmpbuf;
int i;
	
	//バッファをチェック
	if(*ptrbuf != NULL)ERRRET(ERRMLFF,1);
	//一時バッファ確保
	tmpbuf = (unsigned char*)malloc(sizeof(unsigned char) * READLENGTH);
	if(tmpbuf == NULL)ERRRET(ERRMLFF,2);
	
	//ファイルオープン
	filehand = _open(filename , _O_BINARY | _O_RDONLY);
	if(filehand == -1)ERRRET(ERRMLFF,3);


	//読み込み開始
	tmplength_sum = 0;
	for(i=0 ;; i++)
	{
		//READLENGTH分読む
		tmplength = _read(filehand , &tmpbuf[i*READLENGTH] , READLENGTH);
		//読み込みエラーチェック
		if(tmplength < 0)ERRRET(ERRMLFF,4);
		//今、読んだ量を合計に足す
		tmplength_sum += tmplength;
		//READLENGTH読めた場合は続きがあるかも
		if(tmplength == READLENGTH)
		{
			//バッファを広げる
			tmptmpbuf = (unsigned char *)realloc(tmpbuf , (i+2)*READLENGTH ) ;
			//バッファが確保できなかったらリターン
			if(tmptmpbuf == NULL)ERRRET(ERRMLFF,5);
			//無事広がったらポインタをコピー
			tmpbuf = tmptmpbuf;
			continue;
		}
		//READLENGTH以下しか読めなかったらおしまい
		break;
	}

	//実際に返すバッファを確保
	*ptrbuf = (unsigned char*)malloc(tmplength_sum);
	if(*ptrbuf == NULL)ERRRET(ERRMLFF,6);


	//一時バッファから実際に返すバッファにコピー
	memcpy(*ptrbuf , tmpbuf , tmplength_sum);

	//結局のファイルサイズをリターン値に
	ret = tmplength_sum;
ERRMLFF6:
ERRMLFF5:
ERRMLFF4:
	_close(filehand);
ERRMLFF3:
	free(tmpbuf);
ERRMLFF2:
ERRMLFF1:
	return ret;
}
#endif



/*
	メモリをファイルに書き出す。
	0	正常終了
	-1	ファイルが開けない
	-2	ファイルに書けない
*/
int WriteFileFromMemory(char *filename , unsigned char *data , int length )
{
int filehand;
	filehand = _open(filename , _O_CREAT | _O_BINARY | _O_TRUNC | _O_WRONLY , _S_IREAD | _S_IWRITE );
	if(filehand == -1)ERRRET(ERRWFFM,1);
	if(_write(filehand , data , length) < length)ERRRET(ERRWFFM,2);
	ret = 0;
ERRWFFM2:
	_close(filehand);
ERRWFFM1:
	return ret;
}

#define		EXPAND_SIZE_AT_ONCE		0x0100
#define		BUFFER_SIZE_LIMIT		0x2000

/*
	0...正常終了
	1...終端まで読み込んで終了
	-1..読み込み失敗
	-2..メモリ確保失敗
*/
int ReadALine( int filehand , char **PPreturnbuf)
{
	char *Pbuffer=NULL;
	int buffersize=0;
	int i;
	for( i=0 ;; i++)
	{
		if(buffersize <= i)
		{
			if(!Pbuffer)
			{
				Pbuffer = (char*)malloc(sizeof(char)*EXPAND_SIZE_AT_ONCE);
			}
			else
			{
				if( buffersize >= BUFFER_SIZE_LIMIT )
				{
					Pbuffer[i-1] = '\0';
					*PPreturnbuf = Pbuffer;
					return -2;
				}
				Pbuffer = (char*)realloc(&Pbuffer , buffersize+sizeof(char)*EXPAND_SIZE_AT_ONCE);
			}
			if( !Pbuffer )
			{
				*PPreturnbuf = NULL;
				return -2;
			}
			buffersize += EXPAND_SIZE_AT_ONCE;
		}
		switch(_read( filehand , &Pbuffer[i] , 1 ))
		{
		case 1:
			if(Pbuffer[i] == '\n')
			{
				Pbuffer[i] = '\0';
				*PPreturnbuf = Pbuffer;
				return 0;
			}
		break;
		case 0:
			Pbuffer[i] = '\0';
			*PPreturnbuf = Pbuffer;
			return 1;
		break;
		default:
			Pbuffer[i] = '\0';
			*PPreturnbuf = Pbuffer;
			return 1;
		}
	}
	assert(0);
	return -1;
}

/*
	一行書く。うえのほうのと比べても手抜きなのは明白で、あまり使えない
*/
int WriteALine( int filehand , char *Pbuffer )
{
//int rv;
	_write( filehand , Pbuffer , strlen(Pbuffer) );
	return 0;
}

int MMMOpenForTextInput( char *Pname )
{
	return _open( Pname , _O_TEXT | _O_RDONLY);
}
int MMMOpenForTextOutput( char *Pname )
{
	return _open( Pname , _O_CREAT | _O_TEXT | _O_TRUNC | _O_WRONLY , _S_IREAD | _S_IWRITE );
}
int MMMOpenForBinaryInput( char *Pname )
{
	return _open( Pname , _O_BINARY | _O_RDONLY);
}
int MMMOpenForBinaryOutput( char *Pname )
{
	return _open( Pname , _O_CREAT | _O_BINARY | _O_TRUNC | _O_WRONLY , _S_IREAD | _S_IWRITE );
}
int MMMClose( int filehand )
{
	return _close( filehand ) ;
}