/*
        よく意図のわからないメモリ管理ルーチン。
        Memory management routines whose purpose is often unclear.
*/
#include <assert.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


#define mymemorymanage_body
#include "mymemorymanage.h"


//一息で読む量
// Amount of reading in one breath
#define READLENGTH 0x00100000

static int ret;
#define ERRRET(tok, num) \
    {                    \
        ret = -num;      \
        goto tok##num;   \
    }


/*
    ファイルから空のptrbufにメモリを割り当てかつロード。
        ファイルサイズを返す。
        -1	バッファが空でない
        -2　一時バッファが確保できない
        -3　ファイルがオープンできない
        -4　ファイルが読めない
        -5　一時バッファを拡張できない
        -6　返すべきバッファが確保できない

    Allocate and load memory from a file to an empty ptrbuf.
        Returns the file size.
        -1 buffer is not empty
        -2 Unable to secure temporary buffer
        -3 File cannot be opened
        -4 File cannot be read
        -5 Unable to expand temporary buffer
        -6 Unable to secure buffer to return
*/
int LoadMemoryFromFile(const wchar_t* filename, unsigned char** ptrbuf) {
    //バッファをチェック
    // check buffer
    if (*ptrbuf != NULL) return -1;

    FILE* fp;
    fp = _wfopen(filename, L"r");
    if (!fp) return -3;

    int size;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //実際に返すバッファを確保
    // Secure the buffer that will actually be returned
    *ptrbuf = (unsigned char*)malloc(size);
    if (*ptrbuf == NULL) {
        fclose(fp);
        return -6;
    }

    fread(*ptrbuf, 1, size, fp);

    fclose(fp);
    return size;
}
#if 0
//旧式というか、なんでこんなに冗長な記述をしていたのだろう
//ファイルサイズを求める方法を知らなかった？
//It's old-fashioned, I wonder why it was written so redundantly
//Don't know how to find the file size?
int LoadMemoryFromFile(wchar_t *filename , unsigned char **ptrbuf)
{
int filehand;
int tmplength;
int tmplength_sum;
unsigned char *tmpbuf;
unsigned char *tmptmpbuf;
int i;

	//バッファをチェック
    //check buffer
	if(*ptrbuf != NULL)ERRRET(ERRMLFF,1);

	//一時バッファ確保
    //Secure temporary buffer
	tmpbuf = (unsigned char*)malloc(sizeof(unsigned char) * READLENGTH);
	if(tmpbuf == NULL)ERRRET(ERRMLFF,2);

	//ファイルオープン
    //file open
	filehand = _wopen(filename , _O_BINARY | _O_RDONLY);
	if(filehand == -1)ERRRET(ERRMLFF,3);


	//読み込み開始
    //Start loading
	tmplength_sum = 0;
	for(i=0 ;; i++)
	{
		//READLENGTH分読む
        //Read for READLENGTH
		tmplength = _read(filehand , &tmpbuf[i*READLENGTH] , READLENGTH);

        //読み込みエラーチェック
        //Reading error check
		if(tmplength < 0)ERRRET(ERRMLFF,4);

        //今、読んだ量を合計に足す
        //Add the amount read just now to the total
		tmplength_sum += tmplength;
		//READLENGTH読めた場合は続きがあるかも
        //READLENGTHIf you can read it, there may be a continuation
		if(tmplength == READLENGTH)
		{
			//バッファを広げる
            //expand the buffer
			tmptmpbuf = (unsigned char *)realloc(tmpbuf , (i+2)*READLENGTH ) ;
			//バッファが確保できなかったらリターン
            //Return if buffer cannot be allocated
			if(tmptmpbuf == NULL)ERRRET(ERRMLFF,5);
			//無事広がったらポインタをコピー
            //Copy the pointer after it spreads successfully
			tmpbuf = tmptmpbuf;
			continue;
		}

		//READLENGTH以下しか読めなかったらおしまい
        //If you can only read less than READLENGTH, it's over
		break;
	}

	//実際に返すバッファを確保
    //Secure the buffer that will actually be returned
	*ptrbuf = (unsigned char*)malloc(tmplength_sum);
	if(*ptrbuf == NULL)ERRRET(ERRMLFF,6);


	//一時バッファから実際に返すバッファにコピー
    //Copy from temporary buffer to actual buffer to return
	memcpy(*ptrbuf , tmpbuf , tmplength_sum);

	//結局のファイルサイズをリターン値に
    //Use the final file size as the return value
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
        Write memory to file.
        0 Successful completion
        -1 File cannot be opened
        -2 Cannot write to file
*/
int WriteFileFromMemory(const wchar_t* filename, unsigned char* data, int length) {
    int filehand;
    filehand = _wopen(filename, _O_CREAT | _O_BINARY | _O_TRUNC | _O_WRONLY,
                      _S_IREAD | _S_IWRITE);
    if (filehand == -1) ERRRET(ERRWFFM, 1);
    if (_write(filehand, data, length) < length) ERRRET(ERRWFFM, 2);
    ret = 0;
ERRWFFM2:
    _close(filehand);
ERRWFFM1:
    return ret;
}

#define EXPAND_SIZE_AT_ONCE 0x0100
#define BUFFER_SIZE_LIMIT 0x2000

/*
        0...正常終了
        1...終端まで読み込んで終了
        -1..読み込み失敗
        -2..メモリ確保失敗
        0...Normal end
        1...Read to the end and exit
        -1..Failed to load
        -2..Failure to secure memory
*/
int ReadALine(int filehand, wchar_t** PPreturnbuf) {
    wchar_t* Pbuffer = NULL;
    int buffersize = 0;
    int i;
    for (i = 0;; i++) {
        if (buffersize <= i) {
            if (!Pbuffer) {
                Pbuffer = (wchar_t*)malloc(sizeof(wchar_t) * EXPAND_SIZE_AT_ONCE);
            } else {
                if (buffersize >= BUFFER_SIZE_LIMIT) {
                    Pbuffer[i - 1] = L'\0';
                    *PPreturnbuf = Pbuffer;
                    return -2;
                }
                Pbuffer = (wchar_t*)realloc(
                        &Pbuffer, buffersize + sizeof(wchar_t) * EXPAND_SIZE_AT_ONCE);
            }
            if (!Pbuffer) {
                *PPreturnbuf = NULL;
                return -2;
            }
            buffersize += EXPAND_SIZE_AT_ONCE;
        }
        switch (_read(filehand, &Pbuffer[i], 1)) {
            case 1:
                if (Pbuffer[i] == L'\n') {
                    Pbuffer[i] = L'\0';
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
                Pbuffer[i] = L'\0';
                *PPreturnbuf = Pbuffer;
                return 1;
        }
    }
    assert(0);
    return -1;
}

/*
        一行書く。うえのほうのと比べても手抜きなのは明白で、あまり使えない
        Write one line. It's obvious that it's sloppy compared to Ue's version, so it
        can't be used much.
*/
int WriteALine(int filehand, wchar_t* Pbuffer) {
    // int rv;
    _write(filehand, Pbuffer, wcslen(Pbuffer));
    return 0;
}

int MMMOpenForTextInput(const char* Pname) { return _open(Pname, _O_TEXT | _O_RDONLY); }
int MMMOpenForTextOutput(const char* Pname) {
    return _open(Pname, _O_CREAT | _O_TEXT | _O_TRUNC | _O_WRONLY,
                 _S_IREAD | _S_IWRITE);
}
int MMMOpenForBinaryInput(const char* Pname) {
    return _open(Pname, _O_BINARY | _O_RDONLY);
}
int MMMOpenForBinaryOutput(const char* Pname) {
    return _open(Pname, _O_CREAT | _O_BINARY | _O_TRUNC | _O_WRONLY,
                 _S_IREAD | _S_IWRITE);
}
int MMMClose(int filehand) { return _close(filehand); }
