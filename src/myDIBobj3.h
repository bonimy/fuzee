#ifndef MYDIBOBJ3_HEADER_INCLUDED
#define MYDIBOBJ3_HEADER_INCLUDED


/*
        いろいろ破綻した、ソフトウェアレンダリングルーチン。
        デバイス独立ビットマップを取得して、そこにレンダリング。
        汎用性が無いあたりアレだ。
        A software rendering routine that has broken down in various ways.
        Get a device-independent bitmap and render to it.
        It's kind of lacking in versatility.
*/


#define RGB_BITS 5
#define myRGB(r, g, b) (((r) << (RGB_BITS * 2)) | ((g) << RGB_BITS) | (b))
#define RGB_MAX ((1 << RGB_BITS) - 1)
#define MDO3DefaultColorKey myRGB(RGB_MAX, 0, RGB_MAX)

//深度を指定できそうなくせに、他の値を指定するとエラーになる仕様＾＾
// Even though it seems possible to specify the depth, specifying any other value will
// result in an error ^^
#if RGB_BITS == 5
typedef WORD BMPD;
#else
#error
#endif

//描画に際するフラグ
// Flags for drawing
typedef DWORD MDOFLAG;


//*******************************************
//描画フラグ
// drawing flag
//*******************************************
//基本的、単独性が強いもの
// Basic, highly independent

//カラーキーを抜くか
// Remove color key flag
#define MDO3F_COLORKEY 0x00000001

//クリップするか
// Clip or not
#define MDO3F_CLIP 0x00000002

// sfcサイドループするか
// Do sfc side loop flag
#define MDO3F_SFCLOOP 0x00000004

// srcサイドループするか
// src side loop flag
#define MDO3F_SRCLOOP 0x00000008

//直にBitBlt()等を呼ぶか
// Call BitBlt() etc. directly
#define MDO3F_USE_WINAPI 0x00000010

//ソースサイドへ影響が強いもの
// Things that have a strong influence on the source side

//他のsfcのカラーキーを使うか（未実装）
// Use another sfc color key (not implemented)
#define MDO3F_USE_OTHER_COLORKEY 0x00000020

// or演算したものを合成するか
// Combine the or operations
#define MDO3F_OR_OPERATE 0x00000040

// and演算したものを合成するか
// Combine the results of the and operation
#define MDO3F_AND_OPERATE 0x00000080

//単色塗りをするか
// Flag to pain with single color.
#define MDO3F_SIMPLE_COLOR 0x00000100

//カラーテーブルを使うか
// Use color table flag
#define MDO3F_USE_COLOR_TABLE 0x00000200

//明るさを変更するか（未実装）
// Change brightness (not implemented)
#define MDO3F_BRIGHT_CHANGE 0x00000400

//横反転
// horizontal flip
#define MDO3F_X_MIRROR 0x00000800

//縦反転
// Vertical flip
#define MDO3F_Y_MIRROR 0x00001000

//半透明系
// semi-transparent

//ブレンドするか
// Blend flag
#define MDO3F_BLEND 0x00002000

//加算合成するか
// Additive blending flag
#define MDO3F_LIGHT_BLEND 0x00004000

//減算合成するか
// Subtractive blending flag
#define MDO3F_DARK_BLEND 0x00008000

// orブレンドするか
// Logical-OR blend
#define MDO3F_OR_BLEND 0x00010000

// andブレンドするか
// Logical-AND blend
#define MDO3F_AND_BLEND 0x00020000

#define MDO3F_NONE 0x00000000
#define MDO3F_ALL 0xFFFFFFFF


//描画フラグと、各種属性を収める構造体
// Structure that stores drawing flags and various attributes
typedef struct MDO3Opt_tag {
    MDOFLAG flag;
    BYTE alpha;
    union {
        //一番上に置いておくこと
        // Keep it on top
        DWORD DW;
        int i;
        BMPD B;
        int* Pi;
        BMPD* PBMPD;
    };
} MDO3Opt;

//こいつらは、デフォルトの描画属性として、引数に指定したりする
// These can be specified as arguments as default drawing attributes
extern MDO3Opt* MDO3normal;
extern MDO3Opt* MDO3WINAPI;

//？これはプライベートなincludeファイルでいいような……？
//サーフェイス構造体
//? This seems like it should belong in its own private include file...?
// surface structure
struct MDO3Surface {
    //幅
    // width
    int width;

    //本当の幅（実は２の乗数の幅をメモリに確保するので）
    // Actual width (Actually, the width of a power of 2 is reserved in memory)
    int width_true;

    //高さ
    // height
    int height;

    //クリッパーＸ座標
    // Clip space X coordinate
    int clipx;

    //クリッパーＹ座標
    // Clip space Y coordinate
    int clipy;

    //クリッパー幅
    // Clip width
    int clipwidth;

    //クリッパー高さ
    // Clip height
    int clipheight;

    //これが実際のビットマップイメージ……
    // This is the actual bitmap image...
    BMPD* data;
    HBITMAP hbitmap;
    BITMAPINFO info;
    HDC hdc;

    //んあんまり覚えてないや……
    // Store temporary bitmap.
    HBITMAP holdbitmap;

    //幅が何ビットで表されるか
    // How many bits is the width?
    BYTE width_bits;
};

//フリップのしかた
//フリップって使い方間違えてるかも……
//バックサーフェースをプライマリーサーフェイスに転送する……って意味で使ってるんですが。
// How to flip
// Maybe I'm using flip incorrectly...
// I use it to mean transfer the back surface to the primary surface...
#define MDO3FT_NORMAL 0x00
#define MDO3FT_POS 0x01
#define MDO3FT_STRETCH 0x02


//本体クラス
// body class
class MyDIBObj3 {
private:
    //サーフェイス最大数
    // Maximum number of surfaces
    int maxsurface;

    //プライマリーサーフェースの幅
    // width of primary surface
    int primarywidth;

    //同高さ
    // Primary height
    int primaryheight;

    //対象ウインドウのデバイスコンテキストハンドル
    // device context handle of target window
    HDC hdcp;

    //対象ウインドウのウインドウハンドル
    // Window handle of target window
    HWND hwndp;

    //対象アプリケーションのインスタンスハンドル
    // Instance handle of target application
    HINSTANCE hinstancep;

    //カラーキー
    // color key
    BMPD colorkey;

protected:
    //サーフェイスへのポインタ。maxsurface分だけ確保される
    // Pointer to surface. Allocation size is maxsurface value.
    MDO3Surface* sfc;

    //サーフェイスにエラーが無いかを調べる
    // Check if there are any errors on the surface
    inline bool SFCCheck(int num);

    //クリッピング用
    // For clipping
    inline bool Clipping(const MDO3Opt* opt, bool srcadjust, int* Px, int* Py, int* Pw,
                         int* Ph, int* Psx, int* Psy, int sx, int sy, int sw, int sh);

    //ソースからはみ出ないようにクリップ
    // Clip so that it does not protrude from the source
    inline bool ReverseClipping(int srcx, int srcy, int width, int height, int src);

    //？WINAPIのclsを呼ぶための関数？
    //? Function to call WINAPI cls?
    void EXCls(int isfc, int x, int y, int width, int height, BMPD color);

    //線を描画するための関数の、分けたもの
    // Divided functions for drawing lines
    inline void Line_sub1(bool isXY, double sloped, const MDO3Opt* opt, int isfc,
                          double a1d, double b1d, double a2d, double b2d, BMPD color);
    inline void Line_sub2(bool isXY, double sloped, const MDO3Opt* opt, int isfc,
                          double a1d, double b1d, double a2d, double b2d, BMPD color);

public:
    //コンストラクタには、最大サーフェイス、バックサーフェイスの幅、高さ、カラーキーを指定する
    //サーフェイスは、nosfc+1個作られる。
    //０番のサーフェイスが、バックサーフェイスで、nosfc番まで使うことが出来る。
    // Specify the maximum surface, back surface width, height, and color key in the
    // constructor
    // nosfc+1 surfaces are created.
    // Surface number 0 is the back surface and can be used up to number nosfc.
    MyDIBObj3(int nosfc, int width, int height, BMPD icolorkey = MDO3DefaultColorKey);

    //なんも解放しなくても、これに頼ってもいいような構造だったと思うが……
    // I think the structure was such that you could rely on this even if you didn't
    // release anything...
    ~MyDIBObj3();

    //初期化。対象ウインドウのハンドルと、対象アプリケーションのインスタンスハンドルを渡す
    // Initialization. Pass the handle of the target window and the instance handle of
    // the target application
    void Initialize(HWND hwndpval, HINSTANCE hinstanceval);

    //サーフェイスを作成
    // Create the surface
    void CreateSurface(int isfc, int width, int height);

    //サーフェイスを解放
    // release the surface
    void ReleaseSurface(int isfc);

    //各種解放。デストラクタからも呼ばれますが……
    // Various releases. Also called from the destructor...
    void Release();

    //ビットマップを読み込む
    // load bitmap
    void LoadBitmap(int isfc, LPTSTR filename);

    //ビットマップに書き出す　２４ビットカラー限定
    // Export to bitmap 24-bit color only
    void SaveBitmap(const wchar_t* filename, int isfc, int x = -1, int y = -1,
                    int width = -1, int height = -1);

    //サーフェイスのデバイスコンテキストハンドルを取得
    // Get the device context handle of the surface
    HDC GetHDC(int isfc);

    //カラーキーをセット
    // set color key
    void SetColorKey(BMPD icolorkey = MDO3DefaultColorKey);

    //サーフェイスにクリッパーをセット
    // Set the clipper on the surface
    void SetClipper(int isfc, int ix, int iy, int iw, int ih);

    //バックサーフェイスの内容を、プライマリーサーフェイスに転送
    // Transfer the contents of the back surface to the primary surface
    void Flip(int fliptype = MDO3FT_NORMAL, int* argv = NULL);

    //ビットマップへのポインタを取得する
    // get pointer to bitmap
    BMPD* GetSurfacePointer(int isfc);

    //指定位置の色を取得。基本的には使わないが手軽
    // Get the color at the specified position. Basically not used, but easy to use
    BMPD Pick(int isfc, int x, int y);

    //●基本
    //矩形塗りつぶし
    //●Basic
    // fill rectangle
    void Cls(const MDO3Opt* opt, int isfc, int x, int y, int width, int height,
             BMPD color);

    //矩形転送
    // Rectangle transfer
    void Blt(const MDO3Opt* opt, int isfc, int x, int y, int width, int height,
             int isrc, int srcx, int srcy);

    //テキスト描画
    // text drawing
    void Text(int isfc, const wchar_t* str, int x, int y, int height = 12,
              int width = 6, UINT color = 0);
    void TextEX(int isfc, const wchar_t* str, int x, int y, HFONT hfont,
                UINT color = 0);

    //●図形描画
    //線を描画
    //●Graphic drawing
    // draw a line
    void Line(const MDO3Opt* opt, int isfc, double x1, double y1, double x2, double y2,
              BMPD color);

    //三角形描画
    // draw triangle
    void Triangle(const MDO3Opt* opt, int isfc, double x0, double y0, double x1,
                  double y1, double x2, double y2, BMPD color);

    //予定？
    // TODO?
    //	void Circle(const drawoption *opt,int isfc,int cx,int cy,int radius,BMPD color);

    //●割と応用
    //ストレッチ転送
    //● Relatively applied
    // stretch transfer
    void SBlt(const MDO3Opt* opt, int isfc, int x, int y, int width, int height,
              int isrc, int srcx, int srcy, int srcwidth, int srcheight);

    //回転描画予定？
    // Image rotation some day?
    //	void rollblt(const drawoption *opt,int isfc,int x,int y,int width,int height,int
    // isrc,int srcx,int srcy,double angle); 	void brightchange(int isfc,int x,int
    // y,int width,int height,int brightness);

    //●擬似３Ｄ関連
    //三角形変形して転送。やぶれかぶれ欠陥だらけ。つかえたもんじゃない
    //……アレ？作ったと思ったらなぜか無いや……
    //●Pseudo 3D related
    // Transform and transfer the triangle. Full of endless flaws. Not worth it.
    //……Hmm? I thought I already made it, but for some reason it doesn't exist...
    //	void Pory(const MDO3Opt *opt,int isfc,int isrc,
    //		double x0,double y0,double x1,double y1,double x2,double y2,
    //		double u0,double v0,double u1,double v1,double u2,double v2);

    //箱を表示。明らかにいらない気が……
    // Display the box. I obviously don't need it...
    void Box(const MDO3Opt* opt, int isfc, int x1, int y1, int x2, int y2, BMPD color) {
        if (x1 > x2) {
            Box(opt, isfc, x2, y1, x1, y2, color);
            return;
        }
        if (y1 > y2) {
            Box(opt, isfc, x1, y2, x2, y1, color);
            return;
        }
        Cls(opt, isfc, x1, y1, x2 - x1, 1, color);
        Cls(opt, isfc, x2, y1, 1, y2 - y1 + 1, color);
        Cls(opt, isfc, x1, y2, x2 - x1, 1, color);
        Cls(opt, isfc, x1, y1, 1, y2 - y1, color);
    };
};


#endif /*MYDIBOBJ3_HEADER_INCLUDED*/
