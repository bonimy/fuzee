#include "common.h"


#define SAVE(er)          \
    {                     \
        working.Save(er); \
        RenderMINIMAP();  \
    }

bool PathableTile(BYTE mc) {
    if (mc >= 0xD0) return false;
    if (mc == 0x69 || mc == 0xA5) return false;
    return true;
}
bool PathableChip(int cno) {
    if (PathableTile(ROM(ADR_TILE_DEFINITION + cno + 0))) return true;
    if (PathableTile(ROM(ADR_TILE_DEFINITION + cno + 1))) return true;
    if (PathableTile(ROM(ADR_TILE_DEFINITION + cno + 2))) return true;
    if (PathableTile(ROM(ADR_TILE_DEFINITION + cno + 3))) return true;
    return false;
}

void RenderMINIMAP() {
    BYTE* tmap;
    BYTE* flag;
    tmap = (BYTE*)malloc(FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H * FZCD_SOE_CHIP);
    flag = (BYTE*)malloc(FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H);
    for (int ic = 0; ic < 2 + (editingre == 7); ic++) {
        mdo.Cls(MDO3normal, SFC_MINIMAP, 0, 0, 32, 64, myRGB(31, 0, 31));
        memcpy(tmap, working.exptile[0],
               FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H * FZCD_SOE_CHIP);
        memset(flag, 0, FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H);

        //繋ぎ換え有り
        // With reconnection
        if (ic) {
            for (int i = 0; i < FZCD_CHIP_P_MAP_H * FZCD_CHIP_P_MAP_W; i++) {
                BYTE* Ptmp;
                Ptmp = working.exptile[ic] + i * FZCD_SOE_CHIP;
                if ((Ptmp[0] & Ptmp[1]) == 0xFF) {
                    memcpy(tmap + i * FZCD_SOE_CHIP,
                           working.exptile[0] + i * FZCD_SOE_CHIP, FZCD_SOE_CHIP);
                } else {
                    memcpy(tmap + i * FZCD_SOE_CHIP, Ptmp, FZCD_SOE_CHIP);
                }
            }
        }

        {
            for (int i = 0; i < FZCD_MAX_AREA; i++) {
                if (working.area[ic][i].isexist) {
                    int tx, ty;
                    tx = working.area[ic][i].x;
                    ty = working.area[ic][i].y;
                    int offset;
                    offset = tx / 16 + (ty / 16) * FZCD_CHIP_P_MAP_W;

                    BYTE* Ptmp;
                    int tmp;
                    Ptmp = &tmap[offset * 2];
                    tmp = Ptmp[0] | (Ptmp[1] << 8);
                    if (PathableChip(tmp)) {
                        flag[offset] = 1;
                    }
                }
            }
        }
        {
            for (int timesout = 0; timesout < 10000; timesout++) {
                bool updated = false;
                int ydef, xdef, inc;
                if (timesout % 2) {
                    ydef = 0;
                    xdef = 0;
                    inc = 1;
                } else {
                    ydef = FZCD_CHIP_P_MAP_H - 1;
                    xdef = FZCD_CHIP_P_MAP_W - 1;
                    inc = -1;
                }
                for (int iy = ydef; timesout % 2 ? iy < FZCD_CHIP_P_MAP_H : iy >= 0;
                     iy += inc) {
                    for (int ix = xdef; timesout % 2 ? ix < FZCD_CHIP_P_MAP_W : ix >= 0;
                         ix += inc) {
                        int offset;
                        BYTE* Ptmp;
                        int tmp;

                        offset = ix + iy * FZCD_CHIP_P_MAP_W;
                        Ptmp = &tmap[offset * 2];
                        tmp = Ptmp[0] | (Ptmp[1] << 8);
                        if (flag[offset]) continue;
                        if (!PathableChip(tmp)) continue;

                        bool LE, RE, TE, BE;
                        LE = RE = TE = BE = false;
                        if (ix == 0) LE = true;
                        if (iy == 0) TE = true;
                        if (ix == FZCD_CHIP_P_MAP_W - 1) RE = true;
                        if (iy == FZCD_CHIP_P_MAP_H - 1) BE = true;

#define LM(doff)                     \
    {                                \
        if (flag[offset + (doff)]) { \
            flag[offset] = 1;        \
            updated = true;          \
        }                            \
    } /**/

                        if (!LE) LM(-1);
                        if (!RE) LM(1);
                        if (!TE) LM(-FZCD_CHIP_P_MAP_W);
                        if (!BE) LM(FZCD_CHIP_P_MAP_W);
#undef LM
                    }
                }
                if (!updated) break;
            }
        }
        for (int iy = 0; iy < 64; iy++) {
            for (int ix = 0; ix < 32; ix++) {
                int counter = 0;
                for (int q = 0; q < 64; q++) {
                    if (flag[(((64 - 1 - iy) * 8 + q / 8) +
                              (ix * 8 + q % 8) * FZCD_CHIP_P_MAP_W)]) {
                        counter++;
                    }
                }
                if (counter >= 10)
                    mdo.Cls(MDO3normal, SFC_MINIMAP, ix, iy, 1, 1, myRGB(31, 31, 31));
            }
        }
        {
            BMPD* Pbmp;
            int shift;
            Pbmp = mdo.GetSurfacePointer(SFC_MINIMAP);
            shift = 5;
            for (int iy = 0; iy < 64; iy++) {
                for (int ix = 0; ix < 32; ix++) {
                    if (Pbmp[((iy) << shift) | (ix)] == myRGB(31, 31, 31)) continue;

#define LM(dx, dy)                                                         \
    {                                                                      \
        if (Pbmp[((iy + dy) << shift) | (ix + dx)] == myRGB(31, 31, 31)) { \
            Pbmp[((iy) << shift) | (ix)] = myRGB(0, 0, 0);                 \
            continue;                                                      \
        }                                                                  \
    } /**/
                    bool LE, RE, TE, BE;
                    LE = RE = TE = BE = false;
                    if (ix == 0) LE = true;
                    if (iy == 0) TE = true;
                    if (ix == 31) RE = true;
                    if (iy == 63) BE = true;
                    if (!LE && !TE) LM(-1, -1);
                    if (!TE) LM(0, -1);
                    if (!RE && !TE) LM(1, -1);
                    if (!LE) LM(-1, 0);
                    if (!RE) LM(1, 0);
                    if (!LE && !BE) LM(-1, 1);
                    if (!BE) LM(0, 1);
                    if (!RE && !BE) LM(1, 1);
#undef LM
                }
            }
        }
        {
            wchar_t str[64];
            swprintf(str, L"minimap%d-%d.bmp", editingre, ic);
            mdo.SaveBitmap(str, SFC_MINIMAP);
        }
    }
    free(tmap);
    free(flag);
}


void CWT_common(wchar_t* Pdest) {
    Pdest[0] = '\0';
    swprintf(&Pdest[wcslen(Pdest)], L"地域%d-%d   ", editingre, editingcn);
}

#define NO_MAIN_MODE 2

static double sinline(double arg) {
    if (arg > 1) arg -= (int)arg;
    if (arg > 0.6666) return 0;
    return sin(arg / 0.6666 * 3.141592);
}
static BMPD GetAreaColorBMPD(int no) {
    double pro;
    pro = 1.0 - (double)no / 128 * 0.5;
    return myRGB(
            ((int)(sinline((double)(no % 8) / 8) * 14 * pro + 29 * (1 - pro))),
            ((int)(sinline((double)(no % 8) / 8 + 0.3333) * 14 * pro + 29 * (1 - pro))),
            ((int)(sinline((double)(no % 8) / 8 + 0.6666) * 14 * pro +
                   29 * (1 - pro))));
}
static UINT GetAreaColorUINT(int no) {
    double pro;
    pro = 1.0 - (double)no / 256 * 0.5;
    return RGB(((int)(sinline((double)(no % 8) / 8) * 196 * pro + 220 * (1 - pro))),
               ((int)(sinline((double)(no % 8) / 8 + 0.3333) * 196 * pro +
                      220 * (1 - pro))),
               ((int)(sinline((double)(no % 8) / 8 + 0.6666) * 196 * pro +
                      220 * (1 - pro))));
}

void TV_main(TCB* caller) {
    ALIAS_MAIN(caller, main);
    {
        //基本レイヤーの描画
        // Draw the base layer
        for (int ibl = 0; ibl < 32 * 16; ibl++) {
            {
                //高速化
                // Speeding up
                int px, py;
                int tx, ty;
                px = (ibl % 32 * 32);
                py = (ibl / 32 * 32);
                tx = (mainx + px) * MAP_TILE_SIZE;
                ty = (mainy + py) * MAP_TILE_SIZE;
                if (tx >= WINWIDTH || ty >= WINHEIGHT) continue;
                px = ((ibl % 32 + 1) * 32);
                py = ((ibl / 32 + 1) * 32);
                tx = (mainx + px) * MAP_TILE_SIZE;
                ty = (mainy + py) * MAP_TILE_SIZE;
                if (tx < 0 || ty < 0) continue;
            }

            for (int iba = 0; iba < 16; iba++) {
                for (int ich = 0; ich < 16; ich++) {
                    for (int q = 0; q < 4; q++) {
                        int px, py;
                        int tx, ty;
                        //			tx = mainx+(ibl%32)*16 ;
                        //			ty = mainy+(ibl/32)*16 ;
                        px = (ibl % 32 * 32 + 0 + ich * 2 + q / 2);
                        py = (ibl / 32 * 32 + iba * 2 + 0 + q % 2);
                        tx = (mainx + px) * MAP_TILE_SIZE;
                        ty = (mainy + py) * MAP_TILE_SIZE;

                        if (tx < 0 || ty < 0 || tx >= WINWIDTH || ty >= WINHEIGHT)
                            continue;
                        int data;

                        BYTE* Pr = &working.exptile[0][(px / 2 +
                                                        py / 2 * FZCD_CHIP_P_MAP_W) *
                                                       FZCD_SOE_CHIP];
                        data = Pr[0] | (Pr[1] << 8);
                        data = GETROM8(0xCC380 + data + q);

                        mdo.SBlt(MDO3normal, SFC_BACK, tx, ty, MAP_TILE_SIZE,
                                 MAP_TILE_SIZE, SFC_BG, data % 16 * 8,
                                 data / 16 * 8 + (editingcn != 0) * 128,
                                 MAP_TILE_SIZE_D, MAP_TILE_SIZE_D);
                    }
                }
            }
        }
    }


    if (editingcn != 0) {
        //繋ぎ換えレイヤーの表示
        // Display the splicing layer
        for (int ibl = 0; ibl < 32 * 16; ibl++) {
            {
                //高速化
                // Speeding up
                int px, py;
                int tx, ty;
                px = (ibl % 32 * 32);
                py = (ibl / 32 * 32);
                tx = (mainx + px) * MAP_TILE_SIZE;
                ty = (mainy + py) * MAP_TILE_SIZE;
                if (tx >= WINWIDTH || ty >= WINHEIGHT) continue;
                px = ((ibl % 32 + 1) * 32);
                py = ((ibl / 32 + 1) * 32);
                tx = (mainx + px) * MAP_TILE_SIZE;
                ty = (mainy + py) * MAP_TILE_SIZE;
                if (tx < 0 || ty < 0) continue;
            }
            /*
            {
                //表示するかどうか判定
                //Determine whether to display
                    for( int iba=0 ; iba<16 ; iba++ )
                    {
                            for( int ich=0 ; ich<16 ; ich++ )
                            {
                                    int px,py ;
                                    px = (ibl%32*16+ 0 +ich ) ;
                                    py = (ibl/32*16+iba+ 0  ) ;
                                    for( int i=0 ; i<FZCD_SOE_CHIP ; i++ )
                                            if(
            working.exptile[editingcn][(px+py*FZCD_CHIP_P_MAP_W)*FZCD_SOE_CHIP+i] ) iba
            = -1 ;

                                    if( iba==-1 )break ;
                            }
                            if( iba==-1 )break ;
                    }
                    if( iba!=-1 )continue ;
            }
            */
            for (int iba = 0; iba < 16; iba++) {
                for (int ich = 0; ich < 16; ich++) {
                    for (int q = 0; q < 4; q++) {
                        int px, py;
                        int tx, ty;
                        px = (ibl % 32 * 32 + 0 + ich * 2 + q / 2);
                        py = (ibl / 32 * 32 + iba * 2 + 0 + q % 2);
                        tx = (mainx + px) * MAP_TILE_SIZE;
                        ty = (mainy + py) * MAP_TILE_SIZE;

                        if (tx < 0 || ty < 0 || tx >= WINWIDTH || ty >= WINHEIGHT)
                            continue;
                        int data;
                        BYTE* Pr =
                                &working.exptile[editingcn]
                                                [(px / 2 + py / 2 * FZCD_CHIP_P_MAP_W) *
                                                 FZCD_SOE_CHIP];
                        data = Pr[0] | (Pr[1] << 8);
                        if (data == 0xFFFF) continue;
                        data = GETROM8(0xCC380 + data + q);
                        mdo.SBlt(MDO3normal, SFC_BACK, tx, ty, MAP_TILE_SIZE,
                                 MAP_TILE_SIZE, SFC_BG, data % 16 * 8, data / 16 * 8,
                                 MAP_TILE_SIZE_D, MAP_TILE_SIZE_D);
                    }
                }
            }
        }
    }

    //グリッド
    // grid
    {
        {
            for (int i = 0; i < FZCD_BLOCK_P_MAP_W + 1; i++) {
                int tx, ty, tw, th;
                tx = (mainx + i * FZCD_TILE_P_BLOCK_W) * MAP_TILE_SIZE;
                ty = (mainy)*MAP_TILE_SIZE;
                tw = 1;
                th = FZCD_TILE_P_MAP_H * MAP_TILE_SIZE;
                mdo.Cls(MDO3normal, SFC_BACK, tx, ty, tw, th, myRGB(0, 31, 0));
            }
        }
        {
            for (int i = 0; i < FZCD_BLOCK_P_MAP_H + 1; i++) {
                int tx, ty, tw, th;
                tx = (mainx)*MAP_TILE_SIZE;
                ty = (mainy + i * FZCD_TILE_P_BLOCK_H) * MAP_TILE_SIZE;
                tw = FZCD_TILE_P_MAP_W * MAP_TILE_SIZE;
                th = 1;
                mdo.Cls(MDO3normal, SFC_BACK, tx, ty, tw, th, myRGB(0, 31, 0));
            }
        }
    }
#if 1

    {
/*
#define AREA_BANK		0x020000
/*/
#define AREA_BANK 0x100000
        //*/
        int adr;
        int adry, adrx, adr0, adr1, adr2, adr3;
        int curx, cury;
        int maxtrace;
        /*
                        adr = 0x02E0F9 ;
        /*/
        adr = 0x108000;
        //*/
        static int shifts = 0, shiftc = 0;
        if (KeyOn(KC_K)) shifts--;
        if (KeyOn(KC_L)) shifts++;
        if (KeyOn(KC_K) && KeyOn(KC_L)) shifts = 0;

        if (KeyOn(KC_COMMA)) shiftc--;
        if (KeyOn(KC_PERIOD)) shiftc++;
        if (KeyOn(KC_COMMA) && KeyOn(KC_PERIOD)) shiftc = 0;
        adr = adr + shifts * 2;
        adr = GETROM16(adr);
        adr += AREA_BANK;
        adr += shiftc * 9;

        adrx = GETROM16(adr + 1) | AREA_BANK;
        adry = adrx + 2;
        adr0 = adrx + 4;
        adr1 = adrx + 6;
        adr2 = adrx + 8;
        adr3 = adrx + 10;
        adrx = GETROM16(adrx) | AREA_BANK;
        adry = GETROM16(adry) | AREA_BANK;
        adr0 = GETROM16(adr0) | AREA_BANK;
        adr1 = GETROM16(adr1) | AREA_BANK;
        adr2 = GETROM16(adr2) | AREA_BANK;
        adr3 = GETROM16(adr3) | AREA_BANK;
        curx = GETROM16(adr + 5);
        cury = GETROM16(adr + 7);
        maxtrace = GETROM8(adr + 4) + 1 - GETROM8(adr + 3);

        adrx += GETROM8(adr + 3);
        adry += GETROM8(adr + 3);
        adr0 += GETROM8(adr + 3);
        adr1 += GETROM8(adr + 3);
        adr2 += GETROM8(adr + 3);
        adr3 += GETROM8(adr + 3);

        int bx, by;
        bx = by = -1;
        {
            int tx, ty;
            tx = (int)(curx * TRUE_MULTI) + mainx * MAP_TILE_SIZE;
            ty = (int)(cury * TRUE_MULTI) + mainy * MAP_TILE_SIZE;
            mdo.Cls(MDO3normal, SFC_BACK, tx - 3, ty - 3, 7, 7, myRGB(31, 31, 0));
        }
        for (int i = 0; i < maxtrace; i++) {
            curx += ((char)GETROM8(adrx)) * 8;
            cury += ((char)GETROM8(adry)) * 8;
            adrx++;
            adry++;
            int tx, ty;
            tx = (int)(curx * TRUE_MULTI) + mainx * MAP_TILE_SIZE;
            ty = (int)(cury * TRUE_MULTI) + mainy * MAP_TILE_SIZE;
            mdo.Cls(MDO3normal, SFC_BACK, tx, ty, 3, 3, myRGB(31, 31, 0));
            {
                int tmp;
                tmp = GETROM8(adr0);
                tmp &= 0x07;
                double angle;
                angle = tmp * PI / 4;
                double x0, x1, y0, y1, x2, y2;
                x0 = tx + cos(angle + PI / 2) * 30;
                y0 = ty + sin(angle + PI / 2) * 30;
                x1 = tx + cos(angle - PI / 2) * 30;
                y1 = ty + sin(angle - PI / 2) * 30;
                x2 = tx + cos(angle) * 30;
                y2 = ty + sin(angle) * 30;
                mdo.Line(MDO3normal, SFC_BACK, x0, y0, x1, y1, myRGB(0, 31, 0));
                mdo.Line(MDO3normal, SFC_BACK, tx, ty, x2, y2, myRGB(0, 31, 0));

                {
                    for (int i = 0; i < 3; i++) {
                        double length;
                        switch (i) {
                            case 0:
                                tmp = GETROM8(adr1);
                                break;
                            case 1:
                                tmp = GETROM8(adr2);
                                break;
                            case 2:
                                tmp = GETROM8(adr3);
                                break;
                        }
                        static int table[4] = {32, 16, 48, 64};
                        length = table[(tmp >> 2) & 3];
                        if (!(tmp & 0xC0))
                            length = 0;
                        else if (tmp & 0x80)
                            length = -length;
                        else
                            length = length;
                        length *= TRUE_MULTI;
                        x0 = tx + cos(angle + PI / 2) * length;
                        y0 = ty + sin(angle + PI / 2) * length;
                        mdo.Cls(MDO3normal, SFC_BACK, (int)x0 - 1, (int)y0 - 1, 3, 3,
                                myRGB(31, 31, 0));
                    }
                }
            }
            if (bx != -1 && by != -1)
                mdo.Line(MDO3normal, SFC_BACK, bx, by, tx, ty, myRGB(31, 0, 0));
            bx = tx;
            by = ty;
            //*
            {
                wchar_t buf[16];
                swprintf(buf, L"%02X", (GETROM8(adr0) >> 4));
                mdo.Cls(MDO3normal, SFC_BACK, tx, ty, 6 * wcslen(buf), 12,
                        myRGB(31, 31, 31));
                mdo.Text(SFC_BACK, buf, tx, ty, 12, 6);
            }
            //*/
            /*
                                    {
                                    char buf[16] ;
                                    sprintf( buf , "%02X" , (GETROM8( adr1 )>>4)&3 ) ;
                                    mdo.Cls( MDO3normal , SFC_BACK , tx , ty ,
            6*strlen(buf) , 12 , myRGB(31,31,31) ) ; mdo.Text( SFC_BACK , buf , tx , ty
            , 12 , 6 ) ;
                                    }
                                    {
                                    char buf[16] ;
                                    sprintf( buf , "%02X" , (GETROM8( adr2 )>>4)&3 ) ;
                                    mdo.Cls( MDO3normal , SFC_BACK , tx , ty+12 ,
            6*strlen(buf) , 12 , myRGB(31,31,31) ) ; mdo.Text( SFC_BACK , buf , tx ,
            ty+12 , 12 , 6 ) ;
                                    }
                                    {
                                    char buf[16] ;
                                    sprintf( buf , "%02X" , (GETROM8( adr3 )>>4)&3 ) ;
                                    mdo.Cls( MDO3normal , SFC_BACK , tx , ty+24 ,
            6*strlen(buf) , 12 , myRGB(31,31,31) ) ; mdo.Text( SFC_BACK , buf , tx ,
            ty+24 , 12 , 6 ) ;
                                    }
            //*/
            adr0++;
            adr1++;
            adr2++;
            adr3++;
        }
    }
#endif

    ALIAS_END();
}
void TD_main(TCB* caller) {
    ALIAS_MAIN(caller, main);
    SAVE(editingre);
    free(Prom);
    ALIAS_END();
}
bool TF_main(TCB* caller) {
    ALIAS_MAIN(caller, main);
    if (!caller->calltiming) {
        caller->Pdraw = TV_main;
        caller->Pdest = TD_main;

        mainregion_pre = -1;
        mainx = mainy = 0;
        TM.SetDrawOrderPandC(caller, true, false);
        TM.SetProcessOrderPandC(caller, true, false);

        working.Clear();
        working.Load(editingre);

        return false;
    }


    {
        int tmp;
        tmp = 0;
        if (KeyPush(KC_A)) tmp--;
        if (KeyPush(KC_S)) tmp++;
        tmp *= 1;
        if (KeyOn(KC_SHIFT)) tmp *= 32;
        if (KeyOn(KC_A) && KeyOn(KC_S)) tmp = -mainx;
        if (tmp) {
            mainx += tmp;
            RequestRedraw();
        }
    }

    {
        int tmp;
        tmp = 0;
        if (KeyPush(KC_W)) tmp--;
        if (KeyPush(KC_Z)) tmp++;
        tmp *= 1;
        if (KeyOn(KC_SHIFT)) tmp *= 32;
        if (KeyOn(KC_W) && KeyOn(KC_Z)) tmp = -mainy;
        if (tmp) {
            mainy += tmp;
            RequestRedraw();
        }
    }
    {
        int tmp = 0;
        if (KeyPush(KC_D)) tmp += -1;
        if (KeyPush(KC_E)) tmp += 1;
        if (KeyOn(KC_SHIFT)) tmp *= 5;
        if (tmp) {
            multi += tmp;
            DurCorrect(&multi, 2, 40);
            RequestRedraw();
        }
    }

    {
        int tmp;
        tmp = 0;
        if (KeyPush(KC_F7)) tmp--;
        if (KeyPush(KC_F8)) tmp++;
        if (KeyOn(KC_F7) && KeyOn(KC_F8)) tmp = -editingre;
        if (tmp) {
            SAVE(editingre);
            editingre += tmp;
            RotateCorrect(&editingre, 9);
            if (editingre != 7 && editingcn == 2) editingcn = 0;
            working.Load(editingre);
            RequestRedraw();
        }
    }
    //*
    {
        int tmp;
        tmp = 0;
        if (KeyPush(KC_F5)) tmp--;
        if (KeyPush(KC_F6)) tmp++;
        if (KeyOn(KC_F5) && KeyOn(KC_F6)) tmp = -editingcn;
        if (tmp) {
            editingcn += tmp;
            if (editingre == 7)
                RotateCorrect(&editingcn, 3);
            else
                RotateCorrect(&editingcn, 2);
            RequestRedraw();
        }
    }
    //*/
    if (KeyPush(KC_F9)) {
        //*
        //		mainregion_pre = editingre ;
        //下で

        /*LABEL_YOMIKOMI*/

        //*/
    }

    {
        //ここからグラフィック展開
        // Graphic development starts here
        if (editingre != mainregion_pre) {
            // preの更新は下で
            // Pre update below
            EmuCoreMinus tem;
            tem.SetRomData(Prom);
            tem.SetBankSize(false);

            tem.WriteAdr(0x0CF5, (unsigned char)editingre);
            tem.WriteAdrW(0x0F50, 0x0000);
            tem.WriteAdrW(0x0cff, 0x0000);
            //		tem.WriteAdrW( 0x0058 , 0x0000 ) ;
            //		tem.WriteAdrW( 0x0052 , 0x0000 ) ;
            //		tem.WriteAdrW( 0x0CF3 , 0x0000 ) ;

            //パレット
            // palette
            tem.EasyInitialize(0x00A10D, 0x30);
            tem.OperateUntilRTS(1000000);
            static UINT colortable[256];
            BYTE* Pram;
            Pram = tem.GetRAMPointer();
            Pram += 0x500;
            {
                for (int i = 0; i < 256; i++) {
                    int tmp;
                    tmp = Pram[0] | (Pram[1] << 8);
                    Pram += 2;
                    int r, g, b;
                    r = ((tmp >> 0) & 0x1F);
                    g = ((tmp >> 5) & 0x1F);
                    b = ((tmp >> 10) & 0x1F);
                    colortable[i] = myRGB(r, g, b);
                }
            }
            SetSNESPalette(NULL, NULL, colortable);
            //グラフィック
            // graphic
            tem.EasyInitialize(0x00A0B0, 0x30);
            tem.OperateUntilRTS(1000000);
            BYTE* Pvram;
            Pvram = tem.GetVRAMPointer();
            for (int i = 0; i < 256; i++) {
                int tx, ty;
                tx = i % 16 * 8;
                ty = i / 16 * 8;
                DrawSNESCharacter(Pvram, SFC_BG, tx, ty, 2, true, 1);
                Pvram += 2 * 8 * 8;
            }
            {
#ifdef MOSAIC_ENABLED
                    //説明用モザイク
                    // Explanatory mosaic
                    {BMPD * Pb;
            Pb = mdo.GetSurfacePointer(SFC_BG);
            for (int i = 0; i < 256; i++) {
                int r, g, b;
                BMPD tmp;
                r = g = b = 0;
                for (int iy = 0; iy < 8; iy++) {
                    for (int ix = 0; ix < 8; ix++) {
                        tmp = Pb[(i / 16 * 8 + iy) * 128 + i % 16 * 8 + ix];
                        r += tmp & (0x1F << 10);
                        g += tmp & (0x1F << 5);
                        b += tmp & (0x1F);
                    }
                }
                r /= 64;
                g /= 64;
                b /= 64;
                r &= (0x1F << 10);
                g &= (0x1F << 5);
                b &= (0x1F);
                mdo.Cls(MDO3normal, SFC_BG, i % 16 * 8, i / 16 * 8, 8, 8, r | g | b);
            }
        }
#endif
    }
    mdo.Text(SFC_BG, L"Ｍ", 0, 96, 8, 4);
    mdo.Text(SFC_BG, L"Ｂ", 8, 96, 8, 4);
    mdo.Text(SFC_BG, L">>", 0, 80, 8, 4);
    mdo.Text(SFC_BG, L"<<", 8 * 12, 72, 8, 4);
    mdo.Text(SFC_BG, L" ", 8 * 10, 72, 8, 4);
    mdo.Text(SFC_BG, L" ", 0x9 * 8, 0x6 * 8, 8, 4, RGB(255, 255, 255));
    mdo.Text(SFC_BG, L"Ｂ", 0x5 * 8, 0xA * 8, 8, 4, RGB(255, 255, 255));
    //暗くコピーしておく
    // copy darkly
    {
        mdo.Blt(MDO3normal, SFC_BG, 0, 128, 128, 128, SFC_BG, 0, 0);
        MDO3Opt topt = *MDO3normal;
        topt.flag |= MDO3F_DARK_BLEND;
        mdo.Cls(&topt, SFC_BG, 0, 128, 128, 128, myRGB(10, 10, 10));
    }

    /*
    //書き出し
    // start writing
    mdo.SaveBitmap( "tile.bmp" , SFC_BG ) ;
    //*/
}
}


{
    int tmp = 0;
    if (KeyPush(KC_F1)) tmp--;
    if (KeyPush(KC_F2)) tmp++;

    if (tmp || editingre != mainregion_pre) {
        mainregion_pre = editingre;
        mainmode += tmp;
        RotateCorrect(&mainmode, (__int32)NO_MAIN_MODE);
        TM.KillP(caller, TP_HIGHEST, TP_LOWEST);
        switch (mainmode) {
            case 0:
                TM.AddTask(caller, TP_MODE_MAP, TF_mode_map);
                break;
            case 1:
                TM.AddTask(caller, TP_MODE_AREA, TF_mode_area);
                break;
            default:
                assert(0);
        }
        RequestRedraw();
    }
}
if (KeyPush(KC_F11)) {
    SAVE(editingre);

    FILE* flog;
    bool onerror = false;

    flog = _wfopen(L"cmplog.txt", L"wt");
    fwprintf(flog, L"梱包ログ\n\n\n");
    fwprintf(flog, L"●●メモリ確保●●\n");
    BYTE* Pexprom;
    Pexprom = (BYTE*)malloc(romsize * 2);
    memcpy(Pexprom, Prom, romsize);
    memset(Pexprom + romsize, 0, romsize);

    fwprintf(flog, L"●●プログラム修正●●\n");
    {
        BYTE buf[32];
        //ＲＯＭ情報中のサイズを補正
        // Correct the size in ROM information
        SETROME8(Pexprom, romsize * 2, 0x007FD7, 0x0A);

        //読み込みルーチンを変更
        // Change loading routine
        memcpy(Pexprom + ROMADR2OFFSET(0x009F88), Pexprom + ROMADR2OFFSET(0x009F7E), 3);

        //バンクテーブルの更新
        // Update bank table
        {
            for (int i = 0; i < 16; i++) buf[i] = 16 + i;
            memcpy(Pexprom + ROMADR2OFFSET(0x00A006), buf, 16);
        }

        //バンクテーブルの指す場所の変更
        // Change the location pointed to by the bank table
        Pexprom[ROMADR2OFFSET(0x00A060) + 1] = 0x06;
        Pexprom[ROMADR2OFFSET(0x00A060) + 2] = 0xA0;

        //暫定？リージョン02と03の分離
        // preliminary? Separation of regions 02 and 03
        SETROME8(Pexprom, romsize * 2, 0x009F76, 0xEA);

        //エリア情報バンクの変更
        // Change area information bank
        SETROME8(Pexprom, romsize * 2, 0x00D60C, 0x10);

        //プラクティス時のエリア情報ポインタリスト
        // Area information pointer list during practice
        SETROME16(Pexprom, romsize * 2, 0x00D625 + 1, 0x801E);

        //ＧＰでのエリア情報ポインタリスト
        // Area information pointer list in GP
        SETROME16(Pexprom, romsize * 2, 0x00D647 + 1, 0x8000);

        //繋ぎ換えデータオフセットデータのあるバンク
        // Bank with reconnection data offset data
        SETROME24(Pexprom, romsize * 2, 0x009F63 + 1, 0x118000);

        //繋ぎ替えデータのあるバンク
        // Bank with reconnection data
        SETROME16(Pexprom, romsize * 2, 0x009F97 + 1, 0x0011);

        //繋ぎ替えデータ
        // Connection data
        SETROME16(Pexprom, romsize * 2, 0x009FA1 + 1, 0x8100);
        SETROME16(Pexprom, romsize * 2, 0x009FA6 + 1, 0x8100 + 2);
        SETROME16(Pexprom, romsize * 2, 0x009FAD + 1, 0x8100 + 4);

        //強制送還マシンに自重してもらう
        // Let the deportation machine take care of itself
        {
            for (int i = 0; i < 15 + 7; i++)
                SETROME8(Pexprom, romsize * 2, 0x00DAC6 + i, 0x0C);
        }
    }


    fwprintf(flog, L"●●globalsetting.txt読み込み●●\n");

    int gplist[15];
    int gpcclist[15];
    int arealist[3 * 9];
    int prlist[7];
    {
        {
            int i;
            for (i = 0; i < 15; i++) {
                gplist[i] = 0;
                gpcclist[i] = 0;
            }
            for (i = 0; i < 3 * 9; i++) arealist[i] = -1;
            for (i = 0; i < 7; i++) prlist[i] = 0;
        }
        for (;;) {
            FILE* fp;
            fp = _wfopen(L"working\\globalsetting.txt", L"rt");
            if (!fp) {
                fwprintf(flog, L"globalsetting.txt未存在・デフォルト使用します\n");
                break;
            }
            for (;;) {
#define ERROR_CONT                                                                  \
    {                                                                               \
        fwprintf(flog, L"不自然な記述\n「%s」\nがありますが無視して続行します。\n", \
                 buf);                                                              \
        continue;                                                                   \
    }
                wchar_t buf[512 + 10];
                wchar_t buf2[512 + 10];
                int rpos;
                rpos = 0;
                if (!fgetws(buf, 512, fp)) {
                    break;
                }
                if (buf[wcslen(buf) - 1] == L'\n') buf[wcslen(buf) - 1] = L'\0';
                if (buf[0] != L'\\') continue;
                rpos++;
                if (!swscanf(buf + rpos, L"%10s", buf2)) ERROR_CONT;
                rpos += wcslen(buf2) + 1;

                if (!wcscmp(buf2, L"GP")) {
                    int tnum;
                    if (!swscanf(buf + rpos, L"%10s", buf2)) ERROR_CONT;
                    rpos += wcslen(buf2) + 1;
                    if (!swscanf(buf2, L"%d", &tnum)) ERROR_CONT;

                    if (tnum <= 0 || tnum >= 4) ERROR_CONT;

                    int courseno;
                    courseno = tnum;

                    if (!swscanf(buf + rpos, L"%10s", buf2)) ERROR_CONT;
                    rpos += wcslen(buf2) + 1;
                    if (!swscanf(buf2, L"%d", &tnum)) ERROR_CONT;

                    if (tnum <= 0 || tnum >= 6) ERROR_CONT;
                    courseno--;
                    courseno *= 5;
                    courseno += (tnum - 1);

                    if (!swscanf(buf + rpos, L"%10s", buf2)) ERROR_CONT;
                    rpos += wcslen(buf2) + 1;
                    if (!swscanf(buf2, L"%d", &tnum)) ERROR_CONT;
                    if (tnum < 0 || tnum >= 9) ERROR_CONT;

                    gplist[courseno] = tnum;

                    if (!swscanf(buf + rpos, L"%10s", buf2)) ERROR_CONT;
                    rpos += wcslen(buf2) + 1;
                    if (!swscanf(buf2, L"%d", &tnum)) ERROR_CONT;
                    if (tnum < 0 || tnum >= 3) ERROR_CONT;

                    gpcclist[courseno] = tnum;
                } else if (!wcscmp(buf2, L"PR")) {
                    int tnum;
                    if (!swscanf(buf + rpos, L"%10s", buf2)) ERROR_CONT;
                    rpos += wcslen(buf2) + 1;
                    if (!swscanf(buf2, L"%d", &tnum)) ERROR_CONT;
                    if (tnum <= 0 || tnum >= 8) ERROR_CONT;
                    int prno;
                    prno = tnum;

                    if (!swscanf(buf + rpos, L"%10s", buf2)) ERROR_CONT;
                    rpos += wcslen(buf2) + 1;
                    if (!swscanf(buf2, L"%d", &tnum)) ERROR_CONT;
                    if (tnum <= 0 || tnum >= 4) ERROR_CONT;
                    int courseno;
                    courseno = tnum;

                    if (!swscanf(buf + rpos, L"%10s", buf2)) ERROR_CONT;
                    rpos += wcslen(buf2) + 1;
                    if (!swscanf(buf2, L"%d", &tnum)) ERROR_CONT;
                    if (tnum <= 0 || tnum >= 6) ERROR_CONT;
                    courseno--;
                    courseno *= 5;
                    courseno += (tnum - 1);
                    prlist[prno - 1] = courseno;
                } else if (!wcscmp(buf2, L"MM")) {
                    int gpno, cono, dx, dy;
                    wchar_t filename[128];
                    if (swscanf(buf + rpos, L"%d %d %x %x %100s", &gpno, &cono, &dx, &dy,
                               filename) != 5)
                        ERROR_CONT;
                    if (gpno <= 0 || gpno >= 4) ERROR_CONT;
                    if (cono <= 0 || cono >= 6) ERROR_CONT;
                    wchar_t filename2[128];
                    swprintf(filename2, FZCD_WORKING_PATH L"%s", filename);
                    mdo.Cls(MDO3normal, SFC_MINIMAP, 0, 0, 32, 64, myRGB(31, 0, 31));
                    mdo.LoadBitmap(SFC_MINIMAP, filename2);


                    int gindex;
                    gpno--;
                    cono--;

                    gindex = gpno * 5 + cono;

                    switch (gpcclist[gindex]) {
                        case 2:
                            gindex = 12;
                            break;
                        case 1:
                            gindex = gplist[gindex];
                            break;
                        default:
                            if (gplist[gindex] >= 3 && gplist[gindex] != 6)
                                gindex = gplist[gindex] + 6;
                            else
                                gindex = gplist[gindex];
                    }
                    SETROME8(Pexprom, romsize * 2, 0x0FC27A + gindex, dx);
                    SETROME8(Pexprom, romsize * 2, 0x0FC289 + gindex, dy);
                    int mapadr;
                    mapadr = GETROM16(0x0FC25C + gindex * 2);
                    mapadr += 0x0F8000;
                    mapadr += 2;

                    BYTE chr[16 * 4 * 8];
                    memset(chr, 0, 16 * 4 * 8);
                    for (int i = 0; i < 4 * 8; i++) {
                        for (int q = 0; q < 64; q++) {
                            BMPD color;
                            static BYTE table[8] = {
                                    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
                            };
                            color = mdo.Pick(SFC_MINIMAP, i % 4 * 8 + q % 8,
                                             i / 4 * 8 + q / 8);
                            color &= 0x739C;
                            switch (color) {
                                case 0x0000:
                                    chr[16 * 4 * (i / 8) + 16 * 16 * (i % 8 / 4) +
                                        (i % 4 * 8 + q / 8) * 2] |= table[q % 8];
                                    break;
                                case 0x739C:
                                    chr[16 * 4 * (i / 8) + 16 * 16 * (i % 8 / 4) +
                                        (i % 4 * 8 + q / 8) * 2 + 1] |= table[q % 8];
                                    break;
                                default:;
                            }
                        }
                    }
                    memcpy(&ROME(Pexprom, romsize * 2, mapadr), chr, 16 * 4 * 8);
                }
                /*
                if( !sscanf( buf+rpos , "%10s" , buf2 ) )ERROR_CONT;
                rpos += strlen( buf2 ) + 1 ;
                if( !sscanf( buf+rpos , "%10s" , buf2 ) )ERROR_CONT;
                rpos += strlen( buf2 ) + 1 ;
*/
#undef ERROR_CONT
            }
            fclose(fp);
            break;
        }

        fwprintf(flog, L"●●梱包開始●●\n");
        int wpos = romsize + 0x8000 + 0x1000;
        int wpose = 0x108100;
        int wposcc = 0;
        //グダグダになっていることは自覚している……
        //書き込み位置パラメータはまとめて管理したほうがいいだろうなぁ
        // I am aware that I am feeling exhausted...
        // It would be better to manage the write position parameters all at once.
        for (int st = 0; st < 9; st++) {
            fwprintf(flog, L"地域%dの梱包...\n", st);
            static FZCD tmpfzcd;
            wchar_t buf[64];
            swprintf(buf, L"梱包　...　%d/%d ", st + 1, 9);
            SetWindowText(hWnd, buf);
            tmpfzcd.Clear();
            tmpfzcd.Load(st);
            const wchar_t* Perror;
            Perror = tmpfzcd.Write2ROM(Pexprom, romsize * 2, &wpos, &wpose, &wposcc,
                                       arealist + 3 * st);
            if (Perror[0] != L'\0') {
                fwprintf(flog, L"エラーが発生！\n");
                fwprintf(flog, L"%s\n", Perror);
                onerror = true;
            }
        }
        {
            fwprintf(flog, L"★サイズ情報\n");
            fwprintf(flog, L"－－マップ　　残り容量 %5X/%5X(%f％使用)\n",
                    wpos - romsize - 0x8000 - 0x1000, romsize - 0x8000 - 0x1000,
                    (double)(wpos - romsize - 0x8000 - 0x1000) /
                            (romsize - 0x8000 - 0x1000) * 100);
            fwprintf(flog, L"－－エリア　　残り容量 %4X/%4X(%f％使用)\n", wpose & 0x7FFF,
                    0x8000, (double)(wpose & 0x7FFF) / 0x8000 * 100);
            fwprintf(flog, L"－－繋ぎ換え　残り容量 %3X/%3X(%f％使用)\n", wposcc, 0x100,
                    (double)wposcc / 0x100 * 100);
        }
        fwprintf(flog, L"●●コース情報書き込み●●\n");
        {
            int i;
            for (i = 0; i < 15; i++) {
                int tmp;
                int list;
                switch (gpcclist[i]) {
                    case 0:
                        tmp = 0xE0;
                        list = 0;
                        break;
                    case 1:
                        tmp = 0xC0;
                        list = 1;
                        break;
                    case 2:
                        tmp = 0xD0;
                        list = 2;
                        break;
                }
                int al = arealist[3 * gplist[i] + list];
                if (al == -1) {
                    fwprintf(flog, L"エラー\n");
                    fwprintf(flog, L"%d-%d(地域%d-%d) エリア未設定\n", i / 5 + 1,
                            i % 5 + 1, gplist[i], list);
                    onerror = true;
                }

                SETROME16(Pexprom, romsize * 2, 0x108000 + i * 2, al);
                SETROME8(Pexprom, romsize * 2, 0x02E129 + i, tmp | gplist[i]);
            }
            for (i = 0; i < 7; i++) {
                SETROME16(Pexprom, romsize * 2, 0x108000 + (i + 15) * 2,
                          GETROME16(Pexprom, romsize * 2, 0x108000 + prlist[i] * 2));
                SETROME8(Pexprom, romsize * 2, 0x02E138 + i,
                         GETROME8(Pexprom, romsize * 2, 0x02E129 + prlist[i]));
            }
        }
    }
    {
        fwprintf(flog, L"●●チェックサム矯正●●\n");
        unsigned int tmp;
        tmp = 0;
        for (int i = 0; i < romsize * 2; i++) {
            tmp += Pexprom[i];
        }
        tmp &= 0xFFFF;
        SETROME16(Pexprom, romsize * 2, 0x007FDE, tmp);
        tmp = ~tmp;
        tmp &= 0xFFFF;
        SETROME16(Pexprom, romsize * 2, 0x007FDC, tmp);
    }
    fwprintf(flog, L"●●書き出し●●\n");
    WriteFileFromMemory(L"output.smc", Pexprom, romsize * 2);
    free(Pexprom);

    fwprintf(flog, L"●●梱包終了●●\n");

    if (onerror) {
        SetWindowText(hWnd, L"梱包エラーが発生　cmplog.txt　を参考にしてください");
        fwprintf(flog,
                L"梱包に際し、１回以上のエラーが発生しました\n複数箇所でエラーが出ている"
                L"ときは、先に出ているものから解決してください\n");
    } else {
        SetWindowText(hWnd, L"梱包終了");
    }

    fclose(flog);
}


ALIAS_END();
return false;
}
