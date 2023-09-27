#ifndef MYDIBOBJ3_HEADER_INCLUDED
#define MYDIBOBJ3_HEADER_INCLUDED


/*
	いろいろ破綻した、ソフトウェアレンダリングルーチン。
	デバイス独立ビットマップを取得して、そこにレンダリング。
	汎用性が無いあたりアレだ。
*/


#define		RGB_BITS	5
#define		myRGB(r,g,b)		( ( (r)<<(RGB_BITS*2)) | ( (g) << RGB_BITS) | (b))
#define		RGB_MAX		((1<<RGB_BITS) - 1)
#define		MDO3DefaultColorKey	myRGB(RGB_MAX,0,RGB_MAX)

//深度を指定できそうなくせに、他の値を指定するとエラーになる仕様＾＾
#if			RGB_BITS==5
typedef		WORD		BMPD;
#else
#error		
#endif

//描画に際するフラグ
typedef		DWORD		MDOFLAG;


//*******************************************
//描画フラグ
//*******************************************
//基本的、単独性が強いもの
#define		MDO3F_COLORKEY			0x00000001		//カラーキーを抜くか
#define		MDO3F_CLIP				0x00000002		//クリップするか
#define		MDO3F_SFCLOOP			0x00000004		//sfcサイドループするか
#define		MDO3F_SRCLOOP			0x00000008		//srcサイドループするか
#define		MDO3F_USE_WINAPI		0x00000010		//直にBitBlt()等を呼ぶか
//ソースサイドへ影響が強いもの
#define		MDO3F_USE_OTHER_COLORKEY	0x00000020		//他のsfcのカラーキーを使うか（未実装）
#define		MDO3F_OR_OPERATE		0x00000040		//or演算したものを合成するか
#define		MDO3F_AND_OPERATE		0x00000080		//and演算したものを合成するか
#define		MDO3F_SIMPLE_COLOR		0x00000100		//単色塗りをするか
#define		MDO3F_USE_COLOR_TABLE	0x00000200		//カラーテーブルを使うか
#define		MDO3F_BRIGHT_CHANGE		0x00000400		//明るさを変更するか（未実装）
#define		MDO3F_X_MIRROR			0x00000800		//横反転
#define		MDO3F_Y_MIRROR			0x00001000		//縦反転
//半透明系
#define		MDO3F_BLEND				0x00002000		//ブレンドするか
#define		MDO3F_LIGHT_BLEND		0x00004000		//加算合成するか
#define		MDO3F_DARK_BLEND		0x00008000		//減算合成するか
#define		MDO3F_OR_BLEND			0x00010000		//orブレンドするか
#define		MDO3F_AND_BLEND			0x00020000		//andブレンドするか

#define		MDO3F_NONE				0x00000000
#define		MDO3F_ALL				0xFFFFFFFF


//描画フラグと、各種属性を収める構造体
typedef struct MDO3Opt_tag
{
	MDOFLAG flag;
	BYTE alpha;
	union
	{
		DWORD	DW;			//一番上に置いておくこと
		int		i;
		BMPD	B;
		int*	Pi;
		BMPD*	PBMPD;
	};
}MDO3Opt;

//こいつらは、デフォルトの描画属性として、引数に指定したりする
extern MDO3Opt *MDO3normal;
extern MDO3Opt *MDO3WINAPI;

//？これはプライベートなincludeファイルでいいような……？
//サーフェイス構造体
struct MDO3Surface{
	int width;			//幅
	int width_true;		//本当の幅（実は２の乗数の幅をメモリに確保するので）
	int height;			//高さ
	int clipx;			//クリッパーＸ座標
	int clipy;			//クリッパーＹ座標
	int clipwidth;		//クリッパー幅
	int clipheight;		//クリッパー高さ
	BMPD *data;			//これが実際のビットマップイメージ……
	HBITMAP hbitmap;	//こ
	BITMAPINFO info;	//の
	HDC hdc;			//へ
	HBITMAP holdbitmap;	//んあんまり覚えてないや……
	BYTE width_bits;	//幅が何ビットで表されるか
};

//フリップのしかた
//フリップって使い方間違えてるかも……
//バックサーフェースをプライマリーサーフェイスに転送する……って意味で使ってるんですが。
#define		MDO3FT_NORMAL		0x00
#define		MDO3FT_POS			0x01
#define		MDO3FT_STRETCH		0x02


//本体クラス
class MyDIBObj3{
private:
	int maxsurface;			//サーフェイス最大数
	int primarywidth;		//プライマリーサーフェースの幅
	int primaryheight;		//同高さ
	HDC hdcp;				//対象ウインドウのデバイスコンテキストハンドル
	HWND hwndp;				//対象ウインドウのウインドウハンドル
	HINSTANCE hinstancep;	//対象アプリケーションのインスタンスハンドル
	BMPD colorkey;			//カラーキー
protected:

	MDO3Surface *sfc;		//サーフェイスへのポインタ。maxsurface分だけ確保される

	//サーフェイスにエラーが無いかを調べる
	inline bool SFCCheck(int num);
	//クリッピング用
	inline bool Clipping(  const MDO3Opt *opt , bool srcadjust , int *Px , int *Py , int *Pw , int *Ph , int *Psx , int *Psy , int sx , int sy , int sw , int sh );
	//ソースからはみ出ないようにクリップ
	inline bool ReverseClipping( int srcx , int srcy , int width , int height , int src );
	//？WINAPIのclsを呼ぶための関数？
	void EXCls(int isfc,int x,int y,int width,int height,BMPD color);

	//線を描画するための関数の、分けたもの
	inline void Line_sub1( bool isXY , double sloped , const MDO3Opt *opt , int isfc , double a1d , double b1d , double a2d , double b2d , BMPD color );
	inline void Line_sub2( bool isXY , double sloped , const MDO3Opt *opt , int isfc , double a1d , double b1d , double a2d , double b2d , BMPD color );
public:
	//コンストラクタには、最大サーフェイス、バックサーフェイスの幅、高さ、カラーキーを指定する
	//サーフェイスは、nosfc+1個作られる。
	//０番のサーフェイスが、バックサーフェイスで、nosfc番まで使うことが出来る。
	MyDIBObj3(int nosfc,int width,int height,BMPD icolorkey=MDO3DefaultColorKey);
	//なんも解放しなくても、これに頼ってもいいような構造だったと思うが……
	~MyDIBObj3();

	//初期化。対象ウインドウのハンドルと、対象アプリケーションのインスタンスハンドルを渡す
	void Initialize(HWND hwndpval,HINSTANCE hinstanceval);
	//サーフェイスを作成
	void CreateSurface(int isfc,int width,int height);
	//サーフェイスを解放
	void ReleaseSurface(int isfc);
	//各種解放。デストラクタからも呼ばれますが……
	void Release();
	//ビットマップを読み込む
	void LoadBitmap(int isfc,LPTSTR filename);
	//ビットマップに書き出す　２４ビットカラー限定
	void SaveBitmap( char *filename , int isfc , int x=-1 , int y=-1 , int width=-1 , int height=-1 ) ;
	//サーフェイスのデバイスコンテキストハンドルを取得
	HDC GetHDC(int isfc);
	//カラーキーをセット
	void SetColorKey(BMPD icolorkey=MDO3DefaultColorKey);
	//サーフェイスにクリッパーをセット
	void SetClipper( int isfc , int ix , int iy , int iw , int ih );
	//バックサーフェイスの内容を、プライマリーサーフェイスに転送
	void Flip( int fliptype=MDO3FT_NORMAL , int* argv = NULL );
	//ビットマップへのポインタを取得する
	BMPD *GetSurfacePointer(int isfc);
	//指定位置の色を取得。基本的には使わないが手軽
	BMPD Pick( int isfc , int x , int y ) ;

	//●基本
	//矩形塗りつぶし
	void Cls(const MDO3Opt *opt,int isfc,int x,int y,int width,int height,BMPD color);
	//矩形転送
	void Blt(const MDO3Opt *opt,int isfc,int x,int y,int width,int height,int isrc,int srcx,int srcy);
	//テキスト描画
	void Text(int isfc,char* str,int x,int y,int height=12,int width=6,UINT color=0);
	void TextEX(int isfc,char* str,int x,int y,HFONT hfont,UINT color=0);

	//●図形描画
	//線を描画
	void Line(const MDO3Opt *opt,int isfc,double x1,double y1,double x2,double y2,BMPD color);
	//三角形描画
	void Triangle( const MDO3Opt *opt , int isfc , double x0 , double y0 , double x1 , double y1 , double x2 , double y2 , BMPD color );
	//予定？
//	void Circle(const drawoption *opt,int isfc,int cx,int cy,int radius,BMPD color);

	//●割と応用
	//ストレッチ転送
	void SBlt(const MDO3Opt *opt,int isfc,int x,int y,int width,int height,int isrc,int srcx,int srcy,int srcwidth , int srcheight);
	//回転描画予定？
//	void rollblt(const drawoption *opt,int isfc,int x,int y,int width,int height,int isrc,int srcx,int srcy,double angle);
//	void brightchange(int isfc,int x,int y,int width,int height,int brightness);

	//●擬似３Ｄ関連
	//三角形変形して転送。やぶれかぶれ欠陥だらけ。つかえたもんじゃない
	//……アレ？作ったと思ったらなぜか無いや……
//	void Pory(const MDO3Opt *opt,int isfc,int isrc,
//		double x0,double y0,double x1,double y1,double x2,double y2,
//		double u0,double v0,double u1,double v1,double u2,double v2);

	//箱を表示。明らかにいらない気が……
	void Box(const MDO3Opt *opt,int isfc,int x1,int y1,int x2,int y2,BMPD color)
	{
		if(x1 > x2)
		{
			Box(opt , isfc , x2 , y1 , x1 , y2 , color );
			return;
		}
		if(y1 > y2)
		{
			Box(opt , isfc , x1 , y2 , x2 , y1 , color );
			return;
		}
		Cls( opt , isfc , x1 , y1 , x2-x1 , 1     , color );
		Cls( opt , isfc , x2 , y1 , 1     , y2-y1+1 , color );
		Cls( opt , isfc , x1 , y2 , x2-x1 , 1     , color );
		Cls( opt , isfc , x1 , y1 , 1     , y2-y1 , color );
	};
};


#endif /*MYDIBOBJ3_HEADER_INCLUDED*/