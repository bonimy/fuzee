/*
	よく意図のわからないメモリ管理ルーチン。

	ファイルからガバっと読み込みをしたり、書き込んだり。ダンプってやつ。
	その他、自分でも何のために作ったかわからないようなルーチンが複数。
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
*/
extern int LoadMemoryFromFile(char *filename , unsigned char **ptrbuf);

/*
	メモリをファイルに書き出す。
	0	正常終了
	-1	ファイルが開けない
	-2	ファイルに書けない
*/
extern int WriteFileFromMemory(char *filename , unsigned char *data , int length);


/*
　　渡したポインタに、メモリを確保して、そこに１行分の詰める。
　　ちゃんと呼び出した側でfreeすること
	0...正常終了
	1...終端まで読み込んで終了
	-1..読み込み失敗
	-2..メモリ確保失敗
*/
extern int ReadALine( int filehand , char **PPreturnbuf);

extern int WriteALine( int filehand , char *Pbuffer );

/*
　基本的に手抜き用。
　オプショナルヘッダの多さに嫌気がさしただけ
　Outputは、CREAT,TRUNCである。
*/
extern int MMMOpenForTextInput( char *Pname );
extern int MMMOpenForTextOutput( char *Pname );
extern int MMMOpenForBinaryInput( char *Pname );
extern int MMMOpenForBinaryOutput( char *Pname );
extern int MMMClose( int filehand ) ;















/*
	ビット単位で読み書きする際の初期化
*/
extern void BitControlStart(unsigned char *headbyte);
/*
	ビットカーソルにデータを加える
	ビットカーソルはその分進む
*/
extern void BitControlAdd(unsigned char data , int nobit , bool isnoproce=false);
/*
	ビットカーソルからデータを得る
	ビットカーソルはその分進む
*/
extern unsigned char BitControlGet(int nobit , bool isnoproce=false);
/*
	ビットカーソルの移動量
*/
extern int  BitControlGetIOBit();
/*
	ビットカーソルを操作
*/
extern void BitControlSeek(int nobit);

#endif//memorymanage_body

#endif//MYMEMORYMANAGE_INCLUDED