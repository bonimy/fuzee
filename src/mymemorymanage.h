/*
        よく意図のわからないメモリ管理ルーチン。

        ファイルからガバっと読み込みをしたり、書き込んだり。ダンプってやつ。
        その他、自分でも何のために作ったかわからないようなルーチンが複数。

        Memory management routines whose purpose is often unclear.

        Effortlessly read and write from files. It's a dump.
        There are also multiple routines that I don't even know why they were created.
*/
#ifndef MYMEMORYMANAGE_INCLUDED
#define MYMEMORYMANAGE_INCLUDED

#ifndef mymemorymanage_body


/*	ファイルから空のptrbufにメモリを割り当てかつロード。
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
extern int LoadMemoryFromFile(const wchar_t* filename, unsigned char** ptrbuf);

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
extern int WriteFileFromMemory(const wchar_t* filename, unsigned char* data,
                               int length);


/*
　　渡したポインタに、メモリを確保して、そこに１行分の詰める。
　　ちゃんと呼び出した側でfreeすること
        0...正常終了
        1...終端まで読み込んで終了
        -1..読み込み失敗
        -2..メモリ確保失敗
    Allocates memory for the passed pointer and fills it with one line.
　  Properly free on the calling side
        0...Normal end
        1...Read to the end and exit
        -1..Failed to load
        -2..Failure to secure memory
*/
extern int ReadALine(int filehand, char** PPreturnbuf);

extern int WriteALine(int filehand, char* Pbuffer);

/*
　基本的に手抜き用。
　オプショナルヘッダの多さに嫌気がさしただけ
　Outputは、CREAT,TRUNCである。
  Basically for cutting corners.
  I just got fed up with the number of optional headers.
　Output is CREAT,TRUNC.
*/
extern int MMMOpenForTextInput(const wchar_t* Pname);
extern int MMMOpenForTextOutput(const wchar_t* Pname);
extern int MMMOpenForBinaryInput(const wchar_t* Pname);
extern int MMMOpenForBinaryOutput(const wchar_t* Pname);
extern int MMMClose(int filehand);


/*
        ビット単位で読み書きする際の初期化
        Initialization when reading and writing in bits
*/
extern void BitControlStart(unsigned char* headbyte);
/*
        ビットカーソルにデータを加える
        ビットカーソルはその分進む
        Add data to bit cursor
        The bit cursor advances by that amount.
*/
extern void BitControlAdd(unsigned char data, int nobit, bool isnoproce = false);
/*
        ビットカーソルからデータを得る
        ビットカーソルはその分進む
        Get data from bit cursor
        The bit cursor advances by that amount.
*/
extern unsigned char BitControlGet(int nobit, bool isnoproce = false);
/*
        ビットカーソルの移動量
        Bit cursor movement amount
*/
extern int BitControlGetIOBit();
/*
        ビットカーソルを操作
        Update bit cursor
*/
extern void BitControlSeek(int nobit);

#endif  // memorymanage_body

#endif  // MYMEMORYMANAGE_INCLUDED
