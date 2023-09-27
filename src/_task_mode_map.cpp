#include "common.h"

static int submode = 0;
static int selected = 0;
static int dragging = 0;


static BYTE* Pclip = NULL;
static int clipx = 0;
static int clipy = 0;
static int clipw = 1;
static int cliph = 1;

#define NO_SUBMODE 2
#define MODE_TITLE "マップ編集"
#define SUBMODE_TITLE "直接編集", "壁編集",


static void CWT(TCB* caller) {
    const char* Pstr[NO_SUBMODE] = {SUBMODE_TITLE};
    char buf[128];
    const char* Ptmp;
    CWT_common(buf);
    sprintf(&buf[strlen(buf)], MODE_TITLE);
    for (int i = 0; i < NO_SUBMODE; i++) {
        Ptmp = "";
        if (submode == i) Ptmp = Pstr[i];
        sprintf(&buf[strlen(buf)], "【 %s 】", Ptmp);
    }
    sprintf(&buf[strlen(buf)], "<spaceで切り替え");

    ALIAS_MM(caller, mm);
    if (mmcurrentbandsize == -1)
        sprintf(&buf[strlen(buf)], "   Band \?\?\?\?/%X  CHIP \?\?\?\?/%X",
                FZCD_BAND_SPACE_LIMIT, FZCD_CHIP_SPACE_LIMIT);
    else
        sprintf(&buf[strlen(buf)], "   Band %X/%X  CHIP %X/%X", mmcurrentbandsize,
                FZCD_BAND_SPACE_LIMIT, mmcurrentchipsize, FZCD_CHIP_SPACE_LIMIT);
    SetWindowText(hWnd, buf);
    ALIAS_END();
}

static bool TF_reset_culc_data(TCB* caller) {
    ALIAS_MM(caller, mm);
    bool iscwt = false;
    if (mmcurrentbandsize != -1) iscwt = true;
    mmcurrentbandsize = mmcurrentchipsize = -1;
    mmcounter = 30;
    if (iscwt) CWT(caller);
    ALIAS_END();
    return false;
}

bool TF_mm_set(TCB* caller) {
    ALIAS_MM(caller, mm);
    ALIAS_MAIN(caller->PmotherTCB, main);


    {
        int tmp = 0;
        if (KeyPush(KC_F)) tmp--;
        if (KeyPush(KC_G)) tmp++;
        if (KeyOn(KC_SHIFT)) tmp *= 16;
        if (tmp) {
            selected += tmp;
            RotateCorrect(&selected, 0x10000);
            RequestRedraw();
        }
    }
    int gmx, gmy;
    gmx = GetMousePosX();
    gmy = GetMousePosY();
    gmx /= MAP_TILE_SIZE;
    gmy /= MAP_TILE_SIZE;
    gmx -= mainx;
    gmy -= mainy;
    gmx /= 2;
    gmy /= 2;

    if (dragging) {
        switch (dragging) {
            case 10:
                if (MouseOff(MB_C)) dragging = 0;
                if (gmx < 0 || gmy < 0 || gmx > FZCD_CHIP_P_MAP_W ||
                    gmy > FZCD_CHIP_P_MAP_H)
                    return false;
                {
                    int tmp;
                    tmp = (gmx - clipx) - clipw;
                    if (tmp) {
                        clipw += tmp;
                        RequestRedraw();
                    }
                    tmp = (gmy - clipy) - cliph;
                    if (tmp) {
                        cliph += tmp;
                        RequestRedraw();
                    }
                }
                break;
            default:
                assert(0);
        }
        return false;
    }


    {
        if (KeyPush(KC_DEL)) {
            if (clipw < 0) {
                clipx += clipw;
                clipw *= -1;
            }
            if (cliph < 0) {
                clipy += cliph;
                cliph *= -1;
            }
            BYTE src[2];
            src[0] = selected;
            src[1] = selected >> 8;
            for (int iy = 0; iy < cliph; iy++)
                for (int ix = 0; ix < clipw; ix++) {
                    BYTE* Ptmp;
                    Ptmp = &working.exptile[editingcn]
                                           [(clipx + ix +
                                             (clipy + iy) * FZCD_CHIP_P_MAP_W) *
                                            2];

                    memcpy(Ptmp, src, 2);
                }
            mmrequestculc = 1;
            RequestRedraw();
        }
    }
    {
        if (KeyPush(KC_C) || KeyPush(KC_INS)) {
            if (clipw < 0) {
                clipx += clipw;
                clipw *= -1;
            }
            if (cliph < 0) {
                clipy += cliph;
                cliph *= -1;
            }
            if (!clipw || !cliph) return false;
            if (Pclip) free(Pclip);
            Pclip = NULL;
            Pclip = (BYTE*)malloc(clipw * cliph * 2);
            for (int iy = 0; iy < cliph; iy++) {
                BYTE* Ptmp;
                Ptmp = &working.exptile[editingcn]
                                       [(clipx + (clipy + iy) * FZCD_CHIP_P_MAP_W) * 2];
                memcpy(Pclip + clipw * 2 * iy, Ptmp, clipw * 2);
            }
            if (KeyOn(KC_CTRL)) {
                FILE* fp;
                fp = fopen("clipped.fzeeclip", "wb");
                if (!fp) return false;
                BYTE buf[16];
                buf[0] = clipw;
                buf[1] = cliph;
                fwrite(buf, 1, 2, fp);
                fwrite(Pclip, 1, clipw * cliph * 2, fp);
                fclose(fp);
            }
            mmclipshowx = -1;
            mmclipshowy = -1;
            return false;
        }
    }


    if (gmx < 0 || gmy < 0 || gmx >= FZCD_CHIP_P_MAP_W || gmy >= FZCD_CHIP_P_MAP_H)
        return false;

    if (Pclip) {
        if (mmclipshowx != gmx || mmclipshowy != gmy) {
            mmclipshowx = gmx;
            mmclipshowy = gmy;
            RequestRedraw();
        }

        if (MouseOn(MB_L)) {
            for (int iy = 0; iy < cliph; iy++)
                for (int ix = 0; ix < clipw; ix++) {
                    if (gmx + ix >= FZCD_CHIP_P_MAP_W || gmy + iy >= FZCD_CHIP_P_MAP_H)
                        continue;
                    BYTE *Ptmp, *Psrc;
                    Psrc = Pclip + (ix + iy * clipw) * 2;
                    if (Psrc[0] == 0xFF && Psrc[1] == 0xFF) continue;
                    Ptmp = &working.exptile[editingcn]
                                           [(gmx + ix +
                                             (gmy + iy) * FZCD_CHIP_P_MAP_W) *
                                            2];
                    memcpy(Ptmp, Pclip + (ix + iy * clipw) * 2, 2);
                }
            mmrequestculc = 1;
            RequestRedraw();
            return false;
        }
        if (MouseRelease(MB_R)) {
            if (Pclip) free(Pclip);
            Pclip = NULL;
            RequestRedraw();
            return false;
        }
        if (KeyPush(KC_M)) {
            static int counter;
            counter++;
            BYTE* Pbuf;
            Pbuf = (BYTE*)malloc(clipw * cliph * 2);
            if (counter % 2 == 1) {
                for (int iy = 0; iy < cliph; iy++) {
                    for (int ix = 0; ix < clipw; ix++) {
                        Pbuf[(iy * clipw + ix) * 2] =
                                Pclip[(iy * clipw + clipw - 1 - ix) * 2];
                        Pbuf[(iy * clipw + ix) * 2 + 1] =
                                Pclip[(iy * clipw + clipw - 1 - ix) * 2 + 1];
                    }
                }
            } else {
                for (int iy = 0; iy < cliph; iy++) {
                    for (int ix = 0; ix < clipw; ix++) {
                        Pbuf[(iy * clipw + ix) * 2] =
                                Pclip[((cliph - 1 - iy) * clipw + ix) * 2];
                        Pbuf[(iy * clipw + ix) * 2 + 1] =
                                Pclip[((cliph - 1 - iy) * clipw + ix) * 2 + 1];
                    }
                }
            }
            memcpy(Pclip, Pbuf, clipw * cliph * 2);
            free(Pbuf);
            RequestRedraw();
        }
        if (KeyPush(KC_R)) {
            BYTE* Pbuf;
            Pbuf = (BYTE*)malloc(clipw * cliph * 2);
            for (int iy = 0; iy < clipw; iy++) {
                for (int ix = 0; ix < cliph; ix++) {
                    Pbuf[(iy * cliph + ix) * 2] =
                            Pclip[((cliph - ix - 1) * clipw + iy) * 2];
                    Pbuf[(iy * cliph + ix) * 2 + 1] =
                            Pclip[((cliph - ix - 1) * clipw + iy) * 2 + 1];
                }
            }
            memcpy(Pclip, Pbuf, clipw * cliph * 2);
            free(Pbuf);
            MYSWAP(&clipw, &cliph);
            RequestRedraw();
        }
    }


    BYTE* Pmouseon;
    Pmouseon = &working.exptile[editingcn][(gmx + gmy * FZCD_CHIP_P_MAP_W) * 2];

    if (MouseOn(MB_R)) {
        selected = Pmouseon[0] | (Pmouseon[1] << 8);
        RequestRedraw();
    }
    if (MouseOn(MB_L)) {
        int mode;
        mode = 0;
        if (KeyOn(KC_AU) || KeyOn(KC_AD)) mode += 1;
        if (KeyOn(KC_AL) || KeyOn(KC_AR)) mode += 2;
        switch (mode) {
            case 0: {
                Pmouseon[0] = selected;
                Pmouseon[1] = selected >> 8;
                RequestRedraw();
            } break;
            case 1: {
                BYTE src[2];
                BYTE dest[2];
                dest[0] = Pmouseon[0];
                dest[1] = Pmouseon[1];
                src[0] = selected & 0xFF;
                src[1] = selected >> 8;
                if (memcmp(dest, src, 2)) {
                    memcpy(Pmouseon, src, 2);
                    int iy;
                    for (iy = gmy - 1; iy >= 0; iy--) {
                        BYTE* Ptmp;
                        Ptmp = &working.exptile[editingcn]
                                               [(gmx + iy * FZCD_CHIP_P_MAP_W) * 2];
                        if (memcmp(Ptmp, dest, 2)) break;
                        memcpy(Ptmp, src, 2);
                    }
                    for (iy = gmy + 1; iy < FZCD_CHIP_P_MAP_H; iy++) {
                        BYTE* Ptmp;
                        Ptmp = &working.exptile[editingcn]
                                               [(gmx + iy * FZCD_CHIP_P_MAP_W) * 2];
                        if (memcmp(Ptmp, dest, 2)) break;
                        memcpy(Ptmp, src, 2);
                    }
                }
                RequestRedraw();
            } break;
            case 2: {
                BYTE src[2];
                BYTE dest[2];
                dest[0] = Pmouseon[0];
                dest[1] = Pmouseon[1];
                src[0] = selected & 0xFF;
                src[1] = selected >> 8;
                if (memcmp(dest, src, 2)) {
                    memcpy(Pmouseon, src, 2);
                    int ix;
                    for (ix = gmx - 1; ix >= 0; ix--) {
                        BYTE* Ptmp;
                        Ptmp = &working.exptile[editingcn]
                                               [(ix + gmy * FZCD_CHIP_P_MAP_W) * 2];
                        if (memcmp(Ptmp, dest, 2)) break;
                        memcpy(Ptmp, src, 2);
                    }
                    for (ix = gmx + 1; ix < FZCD_CHIP_P_MAP_W; ix++) {
                        BYTE* Ptmp;
                        Ptmp = &working.exptile[editingcn]
                                               [(ix + gmy * FZCD_CHIP_P_MAP_W) * 2];
                        if (memcmp(Ptmp, dest, 2)) break;
                        memcpy(Ptmp, src, 2);
                    }
                }
                RequestRedraw();
            } break;
            case 3: {
                BYTE src[2];
                BYTE dest[2];
                BYTE* Pflag;
                dest[0] = Pmouseon[0];
                dest[1] = Pmouseon[1];
                src[0] = selected & 0xFF;
                src[1] = selected >> 8;
                Pflag = (BYTE*)malloc(FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H);
                memset(Pflag, 0, FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H);

                if (memcmp(dest, src, 2)) {
                    Pflag[gmy * FZCD_CHIP_P_MAP_W + gmx] = 1;
                    bool updated = false;
                    for (int timesout = 0; timesout < 100000; timesout++) {
                        updated = false;
                        for (int iy = 0; iy < FZCD_CHIP_P_MAP_H; iy++)
                            for (int ix = 0; ix < FZCD_CHIP_P_MAP_W; ix++) {
                                if (Pflag[iy * FZCD_CHIP_P_MAP_W + ix]) {
                                    Pflag[iy * FZCD_CHIP_P_MAP_W + ix] = 0;
                                    updated = true;
                                    memcpy(&working.exptile
                                                    [editingcn]
                                                    [(ix + iy * FZCD_CHIP_P_MAP_W) * 2],
                                           src, 2);
                                    if (ix != 0 &&
                                        !memcmp(&working.exptile
                                                         [editingcn]
                                                         [((ix - 1) +
                                                           (iy)*FZCD_CHIP_P_MAP_W) *
                                                          2],
                                                dest, 2))
                                        Pflag[(iy + 0) * FZCD_CHIP_P_MAP_W + (ix - 1)] =
                                                1;
                                    if (ix != FZCD_CHIP_P_MAP_W - 1 &&
                                        !memcmp(&working.exptile
                                                         [editingcn]
                                                         [((ix + 1) +
                                                           (iy)*FZCD_CHIP_P_MAP_W) *
                                                          2],
                                                dest, 2))
                                        Pflag[(iy + 0) * FZCD_CHIP_P_MAP_W + (ix + 1)] =
                                                1;
                                    if (iy != 0 &&
                                        !memcmp(&working.exptile
                                                         [editingcn]
                                                         [((ix) +
                                                           (iy - 1) *
                                                                   FZCD_CHIP_P_MAP_W) *
                                                          2],
                                                dest, 2))
                                        Pflag[(iy - 1) * FZCD_CHIP_P_MAP_W + (ix)] = 1;
                                    if (iy != FZCD_CHIP_P_MAP_H - 1 &&
                                        !memcmp(&working.exptile
                                                         [editingcn]
                                                         [((ix) +
                                                           (iy + 1) *
                                                                   FZCD_CHIP_P_MAP_W) *
                                                          2],
                                                dest, 2))
                                        Pflag[(iy + 1) * FZCD_CHIP_P_MAP_W + (ix)] = 1;
                                }
                            }
                        if (!updated) break;
                    }
                }
                free(Pflag);
                RequestRedraw();
            } break;
        }
        mmrequestculc = 1;
    }

    if (MousePush(MB_C)) {
        if (Pclip) free(Pclip);
        Pclip = NULL;
        clipx = gmx;
        clipy = gmy;
        clipw = 0;
        cliph = 0;
        dragging = 10;
        RequestRedraw();
    }

    ALIAS_END();
    ALIAS_END();
    return false;
}
void TV_mm_set(TCB* caller) {
    ALIAS_MAIN(caller->PmotherTCB, main);
    ALIAS_MM(caller, mm);
    //範囲選択
    {
        int x0, y0, w, h;
        x0 = clipx;
        y0 = clipy;
        w = clipw;
        h = cliph;
        if (w < 0) {
            x0 += w;
            w *= -1;
        }
        if (h < 0) {
            y0 += h;
            h *= -1;
        }
        int tx0, ty0, tx1, ty1;
        tx0 = (mainx + x0 * 2) * MAP_TILE_SIZE;
        ty0 = (mainy + y0 * 2) * MAP_TILE_SIZE;
        tx1 = (mainx + x0 * 2 + w * 2) * MAP_TILE_SIZE;
        ty1 = (mainy + y0 * 2 + h * 2) * MAP_TILE_SIZE;
        tx1--;
        ty1--;
        mdo.Box(MDO3normal, SFC_BACK, tx0, ty0, tx1, ty1, myRGB(0, 31, 0));
        mdo.Box(MDO3normal, SFC_BACK, tx0 + 1, ty0 + 1, tx1 - 1, ty1 - 1,
                myRGB(31, 31, 0));
    }
    //クリップされたもの
    if (Pclip) {
        MDO3Opt topt = *MDO3normal;
        topt.flag |= MDO3F_BLEND;
        topt.alpha = 0x80;
        for (int iy = 0; iy < cliph; iy++)
            for (int ix = 0; ix < clipw; ix++) {
                int data;
                BYTE* Pr = &Pclip[(ix + (iy)*clipw) * FZCD_SOE_CHIP];
                data = Pr[0] | (Pr[1] << 8);
                if (data == 0xFFFF) continue;
                for (int q = 0; q < 4; q++) {
                    int px, py;
                    int tx, ty;
                    px = ((mmclipshowx + ix) * 2 + q / 2);
                    py = ((mmclipshowy + iy) * 2 + q % 2);
                    tx = (mainx + px) * MAP_TILE_SIZE;
                    ty = (mainy + py) * MAP_TILE_SIZE;
                    int chr;
                    chr = GETROM8(0xCC380 + data + q);
                    mdo.SBlt(&topt, SFC_BACK, tx, ty, MAP_TILE_SIZE, MAP_TILE_SIZE,
                             SFC_BG, chr % 16 * 8, chr / 16 * 8, MAP_TILE_SIZE_D,
                             MAP_TILE_SIZE_D);
                }
            }
    }
#define SAMPLE_SIZE (MAP_TILE_SIZE_D * 8)
    for (int q = 0; q < 4; q++) {
        int tx, ty, data;
        tx = WINWIDTH - SAMPLE_SIZE + q / 2 * SAMPLE_SIZE / 2;
        ty = WINHEIGHT - SAMPLE_SIZE + q % 2 * SAMPLE_SIZE / 2;
        data = GETROM8(0xCC380 + selected + q);
        mdo.SBlt(MDO3normal, SFC_BACK, tx, ty, SAMPLE_SIZE / 2, SAMPLE_SIZE / 2, SFC_BG,
                 data % 16 * 8, data / 16 * 8, MAP_TILE_SIZE_D, MAP_TILE_SIZE_D);
    }
    {
        char buf[16];
        sprintf(buf, "%X", selected);
#define FS 12
        mdo.Text(0, buf, WINWIDTH - SAMPLE_SIZE, WINHEIGHT - SAMPLE_SIZE - FS, FS,
                 FS / 2, RGB(0, 0, 0));
        mdo.Text(0, buf, WINWIDTH - SAMPLE_SIZE - 1, WINHEIGHT - SAMPLE_SIZE - FS - 1,
                 FS, FS / 2, RGB(255, 255, 255));
#undef FS
    }
#undef SAMPLE_SIZE
    ALIAS_END();
    ALIAS_END();
}
bool TF_mode_map(TCB* caller) {
    ALIAS_MM(caller, mm);
    if (!caller->calltiming) {
        //		CWT() ;
        mmcounter = 1;
        mmrequestculc = 0;
        //		return false ;
    }

    if (mmcounter > 0) {
        mmcounter--;
        if (!mmcounter) {
            mmcounter = -1;
            int cb, cc;
            working.Write2ROM(Prom, romsize, 0, 0, 0, 0, true, &cb, &cc);
            mmcurrentbandsize = cb;
            mmcurrentchipsize = cc;
            CWT(caller);
            RequestRedraw();
        }
    }

    if (!caller->calltiming) return false;

    //酷いコーディングですが……
    if (KeyPush(KC_F5)) CWT(caller);
    if (KeyPush(KC_F6)) CWT(caller);

    if (KeyPush(KC_SPACE)) {
        submode++;
        submode %= NO_SUBMODE;

        TM.KillP(caller, TP_HIGHEST, TP_LOWEST);
        switch (submode) {
            case 0:
                break;
            case 1:
                TM.AddTask(caller, TP_MM_KUI, TF_mm_kui, TD_mm_kui, TV_mm_kui);
                break;
        }
        CWT(caller);
        RequestRedraw();
    }
    caller->Pdraw = NULL;
    switch (submode) {
        case 0:
            TF_mm_set(caller);
            caller->Pdraw = TV_mm_set;
            break;
        case 1:
            break;
        default:
            assert(0);
    }
    if (mmrequestculc == 1) {
        mmrequestculc = 0;
        TF_reset_culc_data(caller);
    }
    ALIAS_END();
    return false;
}


void LoadFZEECLIP(char* filename) {
    if (Pclip) free(Pclip);
    Pclip = NULL;

    FILE* fp;
    fp = fopen(filename, "rb");
    BYTE buf[16];
    fread(buf, 1, 2, fp);
    if (!buf[0] || !buf[1]) return;
    clipw = buf[0];
    cliph = buf[1];
    Pclip = (BYTE*)malloc(clipw * cliph * 2);
    fread(Pclip, 1, clipw * cliph * 2, fp);
    fclose(fp);
}
