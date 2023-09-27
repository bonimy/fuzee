#ifndef MYDIBOBJ3_PRIVATE_HEADER_INCLUDED
#define MYDIBOBJ3_PRIVATE_HEADER_INCLUDED

/*
        いろいろ破綻した、ソフトウェアレンダリングルーチン。
        ……の、内部で用いるヘッダファイル。
*/

#ifndef PI
#define PI 3.141592653589
#endif

extern BMPD* PAlphaTable;

#define min3(a, b, c) (min(min(a, b), c))
#define max3(a, b, c) (max(max(a, b), c))
#define MYABS(val) ((val >= 0) ? (val) : (-(val)))
template <class TN>
void MYSWAP(TN* val1, TN* val2) {
    TN tmp;
    tmp = *val1;
    *val1 = *val2;
    *val2 = tmp;
}


#define SafeGF(obj)      \
    if (obj) {           \
        GlobalFree(obj); \
        obj = NULL;      \
    }

#ifdef _DEBUG
#define DEBUG_OUTPUT OutputDebugString
#else
#define DEBUG_OUTPUT(str) ;
#endif

//サーフェイスにエラーが無いかどうかチェック
inline bool MyDIBObj3 ::SFCCheck(int num) {
    if (sfc == NULL) return false;
    if (num < 0 || num >= maxsurface) {
        DEBUG_OUTPUT("SFCCheck()サーフェイス番号異常！\n");
        return false;
    }
    if (sfc[num].data == NULL) {
        DEBUG_OUTPUT("SFCCheck()サーフェイスが作られる前に使われようとしました。\n");
        return false;
    }
    return true;
}

//クリッピングを行う
//縦横反転があるせいで、ぐちゃぐちゃなんですが……たぶんあってると思うが。
inline bool MyDIBObj3 ::Clipping(const MDO3Opt* opt, bool srcadjust, int* Px, int* Py,
                                 int* Pw, int* Ph, int* Psx, int* Psy, int sx, int sy,
                                 int sw, int sh) {
    if (*Px < sx) {
        (*Pw) -= (sx - *Px);
        if (srcadjust) {
            if (!(opt->flag & MDO3F_X_MIRROR)) (*Psx) += (sx - *Px);
        }
        (*Px) = sx;
    }
    if (*Py < sy) {
        (*Ph) -= (sy - *Py);
        if (srcadjust) {
            if (!(opt->flag & MDO3F_Y_MIRROR)) (*Psy) += (sy - *Py);
        }
        (*Py) = sy;
    }
    if (sx + sw < (*Px) + (*Pw)) {
        int tmp = sw - ((*Px) - sx);
        if (srcadjust) {
            if (opt->flag & MDO3F_X_MIRROR) (*Psx) += (*Pw) - tmp;
        }
        *Pw = tmp;
    }
    if (sy + sh < (*Py) + (*Ph)) {
        int tmp = sh - ((*Py) - sy);
        if (srcadjust) {
            if (opt->flag & MDO3F_X_MIRROR) (*Psy) += (*Ph) - tmp;
        }
        *Ph = tmp;
    }
    if (*Pw <= 0 || *Ph <= 0) return false;
    return true;
}

//ソース側がはみ出ないように（プログラムをミスらない限りは普通はありえない）
inline bool MyDIBObj3 ::ReverseClipping(int srcx, int srcy, int width, int height,
                                        int src) {
    if (srcx < 0 || srcy < 0 || srcx + width > sfc[src].width ||
        srcy + height > sfc[src].height) {
        DEBUG_OUTPUT("ソース側クリッピングに引っかかっています\n");
        return false;
    }
    return true;
}


#define Rmask 0x7C00
#define Gmask 0x03E0
#define Bmask 0x001F


//マクロ酷使な仕様
//そのせいか、異様にビルドに時間がかかるのだが……
//ついでに、ファイルサイズも大きいな……

// SE……ソースエフェクト……たしか
#define SE_none(opt, srcd) (srcd)
#define SE_and(opt, srcd) ((srcd)&opt->B)
#define SE_or(opt, srcd) ((srcd) | opt->B)
#define SE_simple(opt, srcd) (opt->B)
#define SE_colortable(opt, srcd) (opt->PBMPD[srcd])

// CK……カラーキー
#define CK_none(opt, srcd) (1)
#define CK_colorkey(opt, srcd) ((srcd) != colorkey)

// BL……ブレンド
#define BL_none(opt, Psfc, srcd) (srcd)
#define BL_or(opt, Psfc, srcd) (*Psfc | srcd)
#define BL_and(opt, Psfc, srcd) (*Psfc & srcd)
#define BL_lightblend(opt, Psfc, srcd) (LightBlendAssist(*Psfc, srcd))
#define BL_darkblend(opt, Psfc, srcd) (DarkBlendAssist(*Psfc, srcd))
#define BL_blend(opt, Psfc, srcd) (AT_inv[*Psfc] + AT[srcd])
// ATとAT_invは、ローカル変数としてループ外で計算される

//１行で表せなかったのをしぶしぶ関数分け

//加算合成（重い）
inline BMPD LightBlendAssist(BMPD sfcd, BMPD srcd) {
    BMPD tmp = (sfcd & 0x7BDF) + (srcd & 0x7BDF);
    BMPD mask = (((tmp) >> 5) & 0x0421) * 0x1F;
    return tmp | mask;
}
//減算合成（もっと重い）
inline BMPD DarkBlendAssist(BMPD sfcd, BMPD srcd) {
    BMPD tmp[3] = {
            (sfcd & Rmask) - (srcd & Rmask),
            (sfcd & Gmask) - (srcd & Gmask),
            (sfcd & Bmask) - (srcd & Bmask),
    };
    if (tmp[0] > 0x1F << 10) tmp[0] = 0;
    if (tmp[1] > 0x1F << 5) tmp[1] = 0;
    if (tmp[2] > 0x1F) tmp[2] = 0;
    return tmp[0] | tmp[1] | tmp[2];
}

//こいつに、上のほうのマクロを割り当てて、最終的に１ピクセル出力される
#define PixelSet(opt, Psfc, srcd, BLTYPE, CKTYPE, SETYPE) \
    if (CKTYPE(opt, srcd)) *Psfc = BLTYPE(opt, Psfc, SETYPE(opt, srcd));

//直線を保持する構造体
// isverticalがfalseだと、X-Y座標系、trueだとY-X座標系
//……と、思ったが、なんか違うような。作りかけで、Y-X座標系に対応していない？
typedef struct ELINE_tag {
    double y0;
    double slope;
    bool isvertical;
} ELINE;

//２点を使って、上の直線構造体をセットする
inline bool ELINE_SET(ELINE* Pdest, double x0, double y0, double x1, double y1) {
    double dx, dy;
    dx = x1 - x0;
    dy = y1 - y0;
    if (MYABS(dy) < 0.00001) {
        Pdest->isvertical = false;
        return false;
    }
    Pdest->slope = dx / dy;
    Pdest->y0 = x0 - Pdest->slope * y0;
    Pdest->isvertical = true;
    return true;
}

//直線構造体の直線、yが指定値のときにxがどこにあるか
inline double ELINE_GETX(ELINE* Pdest, double y) {
    return Pdest->slope * y + Pdest->y0;
}

#endif /*MYDIBOBJ3_PRIVATE_HEADER_INCLUDED*/