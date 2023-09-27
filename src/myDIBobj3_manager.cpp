#include <stdio.h>
#include <windows.h>

#include "myDIBobj3.h"
#include "myDIBobj3_p.h"

//デフォルト……ではないが、とくに描画に特殊な物を指定しないという前提での、
//描画関数に渡す、描画属性ポインタ

static MDO3Opt MDO3normal_body = {MDO3F_COLORKEY | MDO3F_CLIP, 0xFF, 0};
MDO3Opt* MDO3normal = &MDO3normal_body;
static MDO3Opt MDO3WINAPI_body = {MDO3F_COLORKEY | MDO3F_CLIP | MDO3F_USE_WINAPI, 0xFF,
                                  SRCCOPY};
MDO3Opt* MDO3WINAPI = &MDO3WINAPI_body;
BMPD* PAlphaTable = NULL;

//半透明テーブルが、何回参照されているか
//もっとも、このクラスを２つ以上使うアプリケーションってありえるんだろうか
static int NOAlphaTableReferenced = 0;

//サーフェイスのHDCを返す。
//っていうか、これは恒常的に保持していていいものなのだろうか……
HDC MyDIBObj3 ::GetHDC(int isfc) {
    if (!SFCCheck(isfc)) return NULL;
    return sfc[isfc].hdc;
}

//カラーキーをセットする
void MyDIBObj3 ::SetColorKey(BMPD icolorkey) { colorkey = icolorkey; }

//サーフェイスにクリッパーをセット。
//セット時に、変な値が指定されていないかをチェック。
//これとは別に、(x0,y0)-(x1,y1)タイプも作ったほうがいい気がしないでもない。

void MyDIBObj3 ::SetClipper(int isfc, int ix, int iy, int iw, int ih) {
    if (!SFCCheck(isfc)) return;
    //クリッパーのクリッピング
    if (ix < 0) {
        iw += ix;
        ix = 0;
    }
    if (iy < 0) {
        ih += iy;
        iy = 0;
    }
    if (iw < 0 || ih < 0) {
        ix = iy = iw = ih = 0;
        return;
    }
    if (ix + iw > sfc[isfc].width_true) {
        iw = sfc[isfc].width_true - ix;
    }
    if (iy + ih > sfc[isfc].height) {
        ih = sfc[isfc].height - iy;
    }
    sfc[isfc].clipx = ix;
    sfc[isfc].clipy = iy;
    sfc[isfc].clipwidth = iw;
    sfc[isfc].clipheight = ih;
}
/*
サーフェイスから、ビットマップへのポインタを取得。
これを利用して、アプリケーション側でグリグリいじくることも出来るわけ。
……もっとも、オーバーライドすればいいのでは、という話もあるけどね。
*/
BMPD* MyDIBObj3 ::GetSurfacePointer(int isfc) {
    if (!SFCCheck(isfc)) return NULL;
    return sfc[isfc].data;
}

//色を取得
BMPD MyDIBObj3 ::Pick(int isfc, int x, int y) {
    if (!SFCCheck(isfc)) return 0xFFFF;
    if (x < 0 || y < 0 || x > sfc[isfc].width_true || y > sfc[isfc].height)
        return 0xFFFF;
    return sfc[isfc].data[(y << sfc[isfc].width_bits) | x];
}

/*
コンストラクタ。
サーフェイス数　　バックサーフェイス幅　　高さ　　カラーキー
を渡す。
失敗すると、sfcがNULLになっている。
この状態では、他のいかなる関数も、落ちないが動かない……はず。
このほかの関数にしても、失敗しても、落ちないが動かない……というのを理想としている。
あくまで理想で、関数にマズイ値を食べさせるとあっさり死んだりするけど。
ついでに、半透明合成に使うテーブルを作成。
*/
MyDIBObj3 ::MyDIBObj3(int nosfc, int width, int height, BMPD icolorkey) {
    nosfc++;
    maxsurface = nosfc;
    primarywidth = width;
    primaryheight = height;
    colorkey = icolorkey;
    sfc = (MDO3Surface*)GlobalAlloc(GMEM_FIXED, sizeof(MDO3Surface) * nosfc);
    if (sfc) {
        for (int i = 0; i < nosfc; i++) {
            sfc[i].data = NULL;
        }
        SetColorKey(icolorkey);
    }
    if (!PAlphaTable) {
        NOAlphaTableReferenced = 1;
        // 2 x 8000h * 16 * 2 = 2 x 2^15 x 2^4 * 2 = 2^21 = (2^10)^2*2^1 = 2M！
        //テーブル酷使も、逆に速度悪化の原因になると聴いたが……
        // lは、ビットマップの最上位ビットが不定なため必要
        //切捨て割り算なため、若干暗くなるが、裏同士（？）を足し算して安全
        PAlphaTable = (BMPD*)GlobalAlloc(GMEM_FIXED, sizeof(BMPD) * 0x8000 * 16 * 2);
        for (int a = 0; a < 16; a++)
            for (int l = 0; l < 2; l++)
                for (int r = 0; r < 32; r++)
                    for (int g = 0; g < 32; g++)
                        for (int b = 0; b < 32; b++) {
                            PAlphaTable[(a << 16) | (l << 15) | (r << 10) | (g << 5) |
                                        b] = ((r * a / 15) << 10) |
                                             ((g * a / 15) << 5) | (b * a / 15);
                        }
    }
}
/*
デストラクタ。
なんも解放しなくても、これだけで大丈夫な仕様。
*/
MyDIBObj3 ::~MyDIBObj3() {
    Release();
    NOAlphaTableReferenced--;
    if (!NOAlphaTableReferenced) {
        PAlphaTable = (BMPD*)GlobalFree(PAlphaTable);
    }
}

/*
初期化。
対象のウインドウハンドルと、インスタンスハンドルを渡す
ついでに、バックサーフェイスを作成する。
*/
void MyDIBObj3 ::Initialize(HWND hwndpval, HINSTANCE hinstanceval) {
    if (!sfc) return;
    hwndp = hwndpval;
    hinstancep = hinstanceval;
    hdcp = GetDC(hwndp);
    CreateSurface(0, primarywidth, primaryheight);
}

/*
サーフェイスを作成する。
こんな関数なのに、失敗しても何も返さないのはどうなのよ？
作成失敗したサーフェイスを使っても、落ちないけど動かない……という仕様らしい
*/
void MyDIBObj3 ::CreateSurface(int isfc, int width, int height) {
    if (!sfc) return;
    if (isfc < 0 || isfc >= maxsurface) return;
    if (isfc != 0 && !sfc[0].data) return;
    if (sfc[isfc].data) return;
    if (width <= 0 || height <= 0) return;

    sfc[isfc].clipx = 0;
    sfc[isfc].clipy = 0;
    sfc[isfc].clipwidth = width;
    sfc[isfc].clipheight = height;
    sfc[isfc].width_true = width;
    {
        int tmp, i;
        tmp = 1;
        for (i = 0; i < 10; i++) {
            if (width <= tmp) break;
            tmp <<= 1;
        }
        width = 1 << i;
        sfc[isfc].width_bits = i;
    }
    sfc[isfc].width = width;
    sfc[isfc].height = height;

    BITMAPINFOHEADER* pbh;
    pbh = &sfc[isfc].info.bmiHeader;
    pbh->biSize = sizeof(BITMAPINFOHEADER);
    pbh->biWidth = width;
    pbh->biHeight = -height;  //不で天地しない
    pbh->biPlanes = 1;
    pbh->biBitCount = 16;
    pbh->biCompression = BI_RGB;
    pbh->biSizeImage = 0;
    pbh->biXPelsPerMeter = 0;
    pbh->biYPelsPerMeter = 0;
    pbh->biClrUsed = 0;
    pbh->biClrImportant = 0;

    sfc[isfc].hbitmap = CreateDIBSection(hdcp, &(sfc[isfc].info), DIB_RGB_COLORS,
                                         (void**)&(sfc[isfc].data), NULL, NULL);

    if (sfc[isfc].hbitmap == 0 || !sfc[isfc].data) {
        if (sfc[isfc].hbitmap != 0) DeleteObject(sfc[isfc].hbitmap);
        sfc[isfc].data = NULL;
        DEBUG_OUTPUT("makesurface() DIBの準備に失敗！\n");
        return;
    }

    sfc[isfc].hdc = CreateCompatibleDC(hdcp);
    if (sfc[isfc].hdc == NULL) {
        DeleteObject(sfc[isfc].hbitmap);
        sfc[isfc].data = NULL;
        DEBUG_OUTPUT("makesurface() HDCの準備に失敗！\n");
        return;
    }
    sfc[isfc].holdbitmap = (HBITMAP)SelectObject(sfc[isfc].hdc, sfc[isfc].hbitmap);
    if (sfc[isfc].holdbitmap == NULL) {
        DeleteObject(sfc[isfc].hbitmap);
        sfc[isfc].data = NULL;
        DEBUG_OUTPUT("makesurface() ビットマップ取り付けに失敗！\n");
    }
}

/*
サーフェイスを解放する。
普通自分では呼ぶ必要が無いが、サーフェイスをいったん消して作り直したい場合など。
*/
void MyDIBObj3 ::ReleaseSurface(int isfc) {
    if (isfc < 0 || isfc >= maxsurface) return;
    if (sfc[isfc].data) {
        SelectObject(sfc[isfc].hdc, sfc[isfc].holdbitmap);
        DeleteObject(sfc[isfc].hbitmap);
        DeleteDC(sfc[isfc].hdc);
        sfc[isfc].data = NULL;
    }
}
/*
解放。デストラクタが呼ぶので、別に自分で呼ぶ必要は無い。
*/
void MyDIBObj3 ::Release() {
    if (sfc) {
        if (sfc[0].data) ReleaseDC(hwndp, hdcp);
        for (int i = 0; i < maxsurface; i++) ReleaseSurface(i);
        SafeGF(sfc);
    }
}
/*
        ビットマップのロード。
*/
void MyDIBObj3 ::LoadBitmap(int isfc, LPTSTR filename) {
    if (!SFCCheck(isfc)) return;
    //一時作業用のHDCとBITMAPハンドル
    HDC mem1;
    HBITMAP hbm, old;
    //ビットマップを読み込む
    hbm = (HBITMAP)LoadImage(hinstancep, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hbm == 0) {
        DEBUG_OUTPUT("ビットマップの読みこみに失敗！\n");
        return;
    }
    //ウインドウのＨＤＣと交換性のあるメモリＤＣを作る
    mem1 = CreateCompatibleDC(hdcp);
    //セットして
    old = (HBITMAP)SelectObject(mem1, hbm);
    //転送
    BitBlt(sfc[isfc].hdc, 0, 0, sfc[isfc].width, sfc[isfc].height, mem1, 0, 0, SRCCOPY);
    //全部消す
    SelectObject(mem1, old);
    DeleteDC(mem1);
    DeleteObject(hbm);
}
/*
        ビットマップのセーブ
        ２４ビットbmpを出力
        x以降の引数を全て-1にすると、サーフェイス全体を出力する
*/
void MyDIBObj3 ::SaveBitmap(const char* filename, int isfc, int x, int y, int width,
                            int height) {
    if (!SFCCheck(isfc)) return;
    if (x == -1 && y == -1 && width == -1 && height == -1) {
        x = y = 0;
        width = sfc[isfc].width_true;
        height = sfc[isfc].height;
    }
    if (x < 0 || y < 0 || width <= 0 || height <= 0 ||
        x + width > sfc[isfc].width_true || y + height > sfc[isfc].height)
        return;
    int WIDTH;
    WIDTH = (width + 3) / 4 * 4;
    FILE* fp;
    fp = fopen(filename, "wb");
    if (!fp) return;
    BYTE buf[256];
    int tmp;
    buf[0] = 'B';
    buf[1] = 'M';
    fwrite(buf, 1, 2, fp);
    tmp = 0x36 + WIDTH * height * 3;
    fwrite(&tmp, 4, 1, fp);
    tmp = 0x00000000;
    fwrite(&tmp, 4, 1, fp);
    tmp = 0x00000036;
    fwrite(&tmp, 4, 1, fp);
    tmp = 0x00000028;
    fwrite(&tmp, 4, 1, fp);
    tmp = width;
    fwrite(&tmp, 4, 1, fp);
    tmp = height;
    fwrite(&tmp, 4, 1, fp);
    tmp = 1;
    fwrite(&tmp, 2, 1, fp);
    tmp = 24;
    fwrite(&tmp, 2, 1, fp);
    tmp = 0;
    fwrite(&tmp, 4, 1, fp);
    tmp = WIDTH * height * 3;
    fwrite(&tmp, 4, 1, fp);
    tmp = 0;
    fwrite(&tmp, 4, 1, fp);
    fwrite(&tmp, 4, 1, fp);
    fwrite(&tmp, 4, 1, fp);
    fwrite(&tmp, 4, 1, fp);

    for (int iy = height - 1; iy >= 0; iy--) {
        BMPD* Pbmp;
        Pbmp = &sfc[isfc].data[(iy << sfc[isfc].width_bits)];
        for (int ix = 0; ix < width; ix++) {
            BMPD dot;
            dot = Pbmp[ix];
            BYTE r, g, b;
            //			r = ( dot >>10 ) & 0x1F ;
            //			g = ( dot >>5  ) & 0x1F ;
            //			b = ( dot      ) & 0x1F ;
            r = (dot >> 7) & 0xF1;
            g = (dot >> 2) & 0xF1;
            b = (dot << 3) & 0xF1;
            fwrite(&b, 1, 1, fp);
            fwrite(&g, 1, 1, fp);
            fwrite(&r, 1, 1, fp);
        }
        for (int ix = 0; ix < WIDTH; ix++) {
            BYTE dummy = 0x00;
            fwrite(&dummy, 1, 1, fp);
            fwrite(&dummy, 1, 1, fp);
            fwrite(&dummy, 1, 1, fp);
        }
    }
    fclose(fp);
}

/*
        バックサーフェイスをプライマリーサーフェイスに転送して画面に表示する。
        ……実はここでめちゃ重になってるのよね。
        画面を３２ビットに設定していると、どうも、BitBltで16bit->32bit転送がかなり重いみたいで……
        画面を１６ビットにしていると速くなるんですがね……
        ところで、フリップの意味を間違えているような。
        引数には、フリップのタイプと、あと、タイプによる属性を渡す。
        どんな属性かは、下を見ればすぐわかるかと。
*/
void MyDIBObj3 ::Flip(int fliptype, int* argv) {
    switch (fliptype) {
        case 0:
            BitBlt(hdcp, 0, 0, primarywidth, primaryheight, sfc[0].hdc, 0, 0, SRCCOPY);
            break;
        case 1:
            BitBlt(hdcp, argv[0], argv[1], primarywidth, primaryheight, sfc[0].hdc, 0,
                   0, SRCCOPY);
            break;
        case 2:
            StretchBlt(hdcp, 0, 0, argv[0], argv[1], sfc[0].hdc, 0, 0, primarywidth,
                       primaryheight, SRCCOPY);
            break;
        case 3:
            StretchBlt(hdcp, argv[0], argv[1], argv[2], argv[3], sfc[0].hdc, 0, 0,
                       primarywidth, primaryheight, SRCCOPY);
            break;
        default:
            __assume(0);
    }
}

/*
        テキスト描画。
*/

void MyDIBObj3 ::Text(int isfc, const char* str, int x, int y, int height, int width,
                      UINT color) {
    if (!SFCCheck(isfc)) return;
    HFONT tmpfont;
    tmpfont = CreateFont(height, width, 0, 0, 0, false, false, false, SHIFTJIS_CHARSET,
                         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                         DEFAULT_PITCH | FF_DONTCARE, "ＭＳ 明朝");
    if (tmpfont == NULL) {
        DEBUG_OUTPUT("textdraw() フォントの作成に失敗！");
        return;
    }

    TextEX(isfc, str, x, y, tmpfont, color);

    DeleteObject(tmpfont);
}
void MyDIBObj3 ::TextEX(int isfc, const char* str, int x, int y, HFONT hfont,
                        UINT color) {
    if (!SFCCheck(isfc)) return;
    HFONT oldfont;
    SetBkMode(sfc[isfc].hdc, TRANSPARENT);
    SetTextColor(sfc[isfc].hdc, color);

    oldfont = (HFONT)SelectObject(sfc[isfc].hdc, hfont);

    TextOut(sfc[isfc].hdc, x, y, str, strlen(str));

    SelectObject(sfc[isfc].hdc, oldfont);
}
