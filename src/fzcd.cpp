#include "fzcd.h"

#include "resource_strings.hxx"

#define ROMADR2OFFSET(adr) ((unsigned int)((((adr) >> 16) * 0x8000) | ((adr)&0x7FFF)))

#define ROME(ptr, size, adr) \
    ((ptr)[(unsigned int)((((adr) >> 16) * 0x8000) | ((adr)&0x7FFF)) % (size)])

#define SETROME8(ptr, size, adr, val) \
    { ROME(ptr, size, adr) = (BYTE)(val); }
#define SETROME16(ptr, size, adr, val)              \
    {                                               \
        SETROME8(ptr, size, adr, val);              \
        SETROME8(ptr, size, (adr) + 1, (val) >> 8); \
    }
#define SETROME24(ptr, size, adr, val)               \
    {                                                \
        SETROME16(ptr, size, adr, val);              \
        SETROME8(ptr, size, (adr) + 2, (val) >> 16); \
    }
#define SETROME32(ptr, size, adr, val)                \
    {                                                 \
        SETROME16(ptr, size, adr, val);               \
        SETROME16(ptr, size, (adr) + 2, (val) >> 16); \
    }

#define GETROME8(ptr, size, adr) (ROME(ptr, size, adr))
#define GETROME16(ptr, size, adr) \
    (ROME(ptr, size, adr) | (ROME(ptr, size, (adr) + 1) << 8))
#define GETROME24(ptr, size, adr)		(ROME(ptr,size,adr)|(ROME(ptr,size,(adr)+1)<<8)|ROME(ptr,size,(adr)+2)<<16))
#define GETROME32(ptr, size, adr)		(ROME(ptr,size,adr)|(ROME(ptr,size,(adr)+1)<<8)|ROME(ptr,size,(adr)+2)<<16)|(ROME(ptr,size,(adr)+3)<<24))


void FZCD ::Clear() {
    region_pre = -1;
    area_sub_org = 0;
    memset(exptile, 0xFF,
           sizeof(BYTE[3][FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H * FZCD_SOE_CHIP]));
    {
        for (int q = 0; q < FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H * FZCD_SOE_CHIP;
             q += 2) {
            exptile[0][q] = 0x56;
            exptile[0][q + 1] = 0x00;
        }
    }
    {
        for (int cc = 0; cc < 3; cc++) {
            for (int i = 0; i < FZCD_MAX_AREA; i++) {
                area[cc][i].isexist = false;
                area[cc][i].x = 0;
                area[cc][i].y = 0;
                area[cc][i].dir = 0;
                area_sub[cc][i].isexist = false;
                area_sub[cc][i].x = 0;
                area_sub[cc][i].y = 0;
                area_sub[cc][i].dir = 0;
                for (int q = 0; q < FZCD_NO_AREA_ARG; q++) {
                    area[cc][i].arg[q] = 0;
                    area_sub[cc][i].arg[q] = 0;
                }
            }
        }
    }
}


const wchar_t* FZCD ::Write2ROM(BYTE* Prom, int romsize, int* Poffset, int* Poffsete,
                                int* Poffsetcc, int* Pareaadrlist, bool culcmode,
                                int* Psizeofband, int* Psizeofchip) {
    BYTE blbuf[3][FZCD_MAP_SPACE_LIMIT];
    BYTE babuf[FZCD_BAND_SPACE_LIMIT * 2 + 256];
    BYTE chbuf[FZCD_CHIP_SPACE_LIMIT * 2 + 256];
    int bap, chp;

    bap = chp = 0;

    for (int icn = 0; icn < 2 + (region_pre == 7); icn++)
        for (int ibl = 0; ibl < FZCD_BLOCK_P_MAP; ibl++) {
            bool isprocess;
            isprocess = true;

            //もし、繋ぎ換えレイヤーのブロックが全てFFFFなら処理をスキップ
            // If all blocks in the connection layer are FFFF, skip processing
            if (icn) {
                isprocess = false;
                for (int q = 0; q < FZCD_BAND_P_BLOCK; q++) {
                    int px, py;
                    px = (ibl % 32 * 16 + 0);
                    py = (ibl / 32 * 16 + q);
                    BYTE* Pr = &this->exptile[icn][(px + py * FZCD_CHIP_P_MAP_W) *
                                                   FZCD_SOE_CHIP];
                    for (int k = 0; k < FZCD_CHIP_P_BAND; k++) {
                        int tmp;
                        tmp = Pr[k * 2] | (Pr[k * 2 + 1] << 8);
                        if (tmp != 0xFFFF) {
                            q = -1;
                            isprocess = true;
                            break;
                        }
                    }
                    if (q == -1) break;
                }
            }
            if (!isprocess) {
                blbuf[icn][ibl] = 0xFF;
                continue;
            }

            int sameblock = -1;
            BYTE tmpband[FZCD_SOA_BAND];
            for (int iba = 0; iba < FZCD_BAND_P_BLOCK; iba++) {
                int px, py;
                px = (ibl % 32 * 16 + 0);
                py = (ibl / 32 * 16 + iba);
                BYTE tmptile[FZCD_CHIP_P_BAND * FZCD_SOE_CHIP];
                // BYTE *Pr =
                // &this->exptile[icn][(px+py*FZCD_CHIP_P_MAP_W)*FZCD_SOE_CHIP] ;
                memcpy(tmptile,
                       exptile[icn] + (px + py * FZCD_CHIP_P_MAP_W) * FZCD_SOE_CHIP,
                       FZCD_CHIP_P_BAND * FZCD_SOE_CHIP);
                for (int ich = 0; ich < FZCD_CHIP_P_BAND; ich++) {
                    if (tmptile[ich * 2] == 0xFF && tmptile[ich * 2 + 1] == 0xFF) {
                        memcpy(tmptile + ich * 2,
                               exptile[0] +
                                       (px + py * FZCD_CHIP_P_MAP_W) * FZCD_SOE_CHIP +
                                       ich * 2,
                               2);
                    }
                }


                bool included_pizza = false;
                {
                    for (int i = 0; i < FZCD_SOA_BAND; i += FZCD_SOE_CHIP) {
                        int tpos;
                        tpos = tmptile[i + 0] | (tmptile[i + 1] << 8);
                        tpos -= 0x0000;
                        //					tpos =
                        // chbuf[i*2+0]|(chbuf[i*2+1]<<8) ;

                        BYTE* Ptile;
                        Ptile = &Prom[ROMADR2OFFSET(0xCC380) + tpos];
                        for (int q = 0; q < FZCD_TILE_P_CHIP; q++) {
                            switch (Ptile[q]) {
                                case 0xC8:
                                case 0xC9:
                                case 0xCA:
                                case 0xCB:
                                    included_pizza = true;
                            }
                        }
                    }
                }

                int comp;
                if (!included_pizza) {
                    for (comp = 0; comp < chp; comp += FZCD_SOA_BAND) {
                        if (!memcmp(chbuf + comp, tmptile, FZCD_SOA_BAND)) break;
                    }
                } else {
                    comp = chp;
                }

                if (comp == chp) {
                    memcpy(chbuf + comp, tmptile, FZCD_SOA_BAND);
                    chp += FZCD_SOA_BAND;
                    if (chp >= FZCD_CHIP_SPACE_LIMIT * 2)
                        return BAND_CAPACITY_EXCEEDED.c_str();
                }
                comp += 0x7000;
                tmpband[iba * 2 + 0] = comp;
                tmpband[iba * 2 + 1] = comp >> 8;
            }
            int comp = 0;
            for (; comp < bap; comp += FZCD_SOA_BLOCK) {
                if (!memcmp(babuf + comp, tmpband, FZCD_SOA_BLOCK)) break;
            }
            if (comp == bap) {
                memcpy(babuf + comp, tmpband, FZCD_SOA_BLOCK);
                bap += FZCD_SOA_BLOCK;
                if (bap >= FZCD_BAND_SPACE_LIMIT * 2)
                    return BLOCK_CAPACITY_EXCEEDED.c_str();
            }
            blbuf[icn][ibl] = comp / FZCD_SOA_BLOCK;
        }

    if (Psizeofband) {
        (*Psizeofband) = bap;
    }
    if (Psizeofchip) {
        (*Psizeofchip) = chp;
    }
    if (culcmode) return L"";
    if (bap >= FZCD_BAND_SPACE_LIMIT) return BLOCK_CAPACITY_EXCEEDED.c_str();
    if (chp >= FZCD_CHIP_SPACE_LIMIT) return BAND_CAPACITY_EXCEEDED.c_str();
    if (region_pre < 0) return PROGRAM_ERROR.c_str();

    //繋ぎ替え
    // Reconnect
    {
        for (int icn = 1; icn < 2 + (region_pre == 7); icn++) {
            SETROME8(Prom, romsize, 0x118000 + region_pre + (icn == 2) * 2, *Poffsetcc);
            BYTE ccnec[FZCD_MAP_SPACE_LIMIT];
            memset(ccnec, 0, FZCD_MAP_SPACE_LIMIT);
            {
                for (int i = 0; i < FZCD_MAP_SPACE_LIMIT; i++) {
                    if (blbuf[icn][i] != 0xFF) ccnec[i] = 1;
                }
            }
            {
                for (int i = 0; i < FZCD_MAP_SPACE_LIMIT; i++) {
                    if (ccnec[i]) {
                        int x0, y0;
                        x0 = i % FZCD_BLOCK_P_MAP_W;
                        y0 = i / FZCD_BLOCK_P_MAP_W;
                        if (x0 == FZCD_BLOCK_P_MAP_W - 1) x0--;
                        if (y0 == FZCD_BLOCK_P_MAP_H - 1) y0--;
                        ccnec[(x0) + (y0)*FZCD_BLOCK_P_MAP_W] =
                                ccnec[(x0) + 1 + (y0)*FZCD_BLOCK_P_MAP_W] =
                                        ccnec[(x0) + (y0 + 1) * FZCD_BLOCK_P_MAP_W] =
                                                ccnec[(x0) + 1 +
                                                      (y0 + 1) * FZCD_BLOCK_P_MAP_W] =
                                                        0;
                        int tmp;

#define TMPPUT(dx, dy, dv)                                                     \
    ;                                                                          \
    tmp = blbuf[icn][x0 + dx + (y0 + dy) * FZCD_BLOCK_P_MAP_W];                \
    if (tmp == 0xFF) tmp = blbuf[0][x0 + dx + (y0 + dy) * FZCD_BLOCK_P_MAP_W]; \
    SETROME8(Prom, romsize, 0x118100 + (*Poffsetcc) + dv, tmp); /**/

                        SETROME16(Prom, romsize, 0x118100 + (*Poffsetcc),
                                  x0 + (y0)*FZCD_BLOCK_P_MAP_W);
                        TMPPUT(0, 0, 2);
                        TMPPUT(1, 0, 3);
                        TMPPUT(0, 1, 4);
                        TMPPUT(1, 1, 5);
                        (*Poffsetcc) += 6;
                    }
                }
            }
            SETROME16(Prom, romsize, 0x118100 + (*Poffsetcc), 0xFFFF);
            (*Poffsetcc) += 2;
            if ((*Poffsetcc) > 256) return BLOCK_CAPACITY_EXCEEDED.c_str();
        }
    }


    if ((*Poffset) % 0x10 != 0) {
        (*Poffset) = (*Poffset) / 16 * 16 + 16;
    }
    {
        int tpos;
        int tb, tp;
        tb = ((*Poffset) - 0x8000 * 0x10) / 0x8000;
        tp = ((*Poffset) % 0x08000) + 0x8000;
        tpos = tp >> 4;
        tpos |= tb << 12;
        Prom[ROMADR2OFFSET(0x039f00) + region_pre * 8 + 0] = (tpos);
        Prom[ROMADR2OFFSET(0x039f00) + region_pre * 8 + 1] = (tpos) >> 8;
        Prom[ROMADR2OFFSET(0x039f00) + region_pre * 8 + 2] = ((bap + 0x200) >> 4);
        Prom[ROMADR2OFFSET(0x039f00) + region_pre * 8 + 3] = ((bap + 0x200) >> 4) >> 8;
    }

    memcpy(Prom + *Poffset, blbuf[0], 0x200);
    (*Poffset) += 0x200;
    memcpy(Prom + *Poffset, babuf, bap);
    (*Poffset) += bap;

    {
        int tpos;
        int tb, tp;
        tb = ((*Poffset) - 0x8000 * 0x10) / 0x8000;
        tp = ((*Poffset) % 0x08000) + 0x8000;
        tpos = tp >> 4;
        tpos |= tb << 12;
        Prom[ROMADR2OFFSET(0x039f00) + region_pre * 8 + 4] = (tpos);
        Prom[ROMADR2OFFSET(0x039f00) + region_pre * 8 + 5] = (tpos) >> 8;
        Prom[ROMADR2OFFSET(0x039f00) + region_pre * 8 + 6] = (chp >> 4);
        Prom[ROMADR2OFFSET(0x039f00) + region_pre * 8 + 7] = (chp >> 4) >> 8;
    }

    memcpy(Prom + *Poffset, chbuf, chp);
    (*Poffset) += chp;


    //エリア
    // Reconnect
    for (int cc = 0; cc < 2 + (region_pre == 7); cc++) {
        int noarea = -1;
        int noarea_sub = -1;
        {
            for (int i = 0; i < FZCD_MAX_AREA; i++) {
                if (area[cc][i].isexist) noarea = i;
            }
        }
        {
            for (int i = 0; i < FZCD_MAX_AREA; i++) {
                if (area_sub[cc][i].isexist) noarea_sub = i;
            }
        }
        if (noarea < 0) continue;
        //エリア数オーバー
        //よくわからんが、分岐道があると、一つメモリ上にスキマを作って展開するらしいので１を足す
        // The number of areas exceeded
        // I don't really understand, but if there is a branching path, it seems that
        // one space is created in memory and expanded, so add 1
        if ((noarea + 1) + (noarea_sub + 1) + (noarea_sub != -1) > FZCD_MAX_AREA) {
            return BRANCH_AREA_LIMIT_EXCEEDED.c_str();
        }
        //容量不足
        // Capacity shortage
        if (
                //開始位置
                // starting position
                (*Poffsete) +
                        //エリアヘッダ（？）
                        // Area header (?)
                        (9 + (noarea_sub != -1) * 9 + 1) +
                        //エリア情報のアドレスリスト
                        // Area information address list
                        12 * (1 + (noarea_sub != -1)) +
                        //情報実体
                        // information entity
                        noarea * 6 + (noarea_sub + 1) * 6 >=
                0x110000)
            return INSUFFICIENT_WRITE_SPACE.c_str();
        Pareaadrlist[cc] = (*Poffsete);

        int adr;
        adr = (*Poffsete);

        SETROME16(Prom, romsize, 0x108000 + region_pre * 2, 0x8000 | (adr & 0x7FFF));


        SETROME8(Prom, romsize, adr + 0, 1);
        SETROME16(Prom, romsize, adr + 1, (*Poffsete) + 10 + (noarea_sub != -1) * 9);
        SETROME8(Prom, romsize, adr + 3, 0);
        SETROME8(Prom, romsize, adr + 4, noarea - 1);
        SETROME16(Prom, romsize, adr + 5, (area[cc][0].x));
        SETROME16(Prom, romsize, adr + 7, (area[cc][0].y));
        (*Poffsete) += 9;
        adr += 9;

        //分岐エリアあり
        // With branch area
        if (noarea_sub != -1) {
            SETROME8(Prom, romsize, adr + 0, 0xFF);
            SETROME16(Prom, romsize, adr + 1, (*Poffsete) + 10 + 2 * 6);
            SETROME8(Prom, romsize, adr + 3, 0);
            SETROME8(Prom, romsize, adr + 4, noarea_sub);
            SETROME16(Prom, romsize, adr + 5, (area[cc][area_sub_org].x));
            SETROME16(Prom, romsize, adr + 7, (area[cc][area_sub_org].y));
            (*Poffsete) += 9;
            adr += 9;
        }

        //終端コード
        // terminal code
        SETROME8(Prom, romsize, adr, 0);
        (*Poffsete) += 1;
        adr += 1;

        //各データのアドレスのリスト書き出し
        // Write out a list of addresses for each data
        {
            int listsize = 2 * 6;
            if (noarea_sub != -1) listsize *= 2;

            for (int i = 0; i < 6; i++) {
                int tmp;
                tmp = (*Poffsete) + listsize + noarea * i;
                SETROME16(Prom, romsize, adr + i * 2, tmp);
            }
            if (noarea_sub != -1) {
                for (int i = 0; i < 6; i++) {
                    int tmp;
                    tmp = (*Poffsete) + listsize + noarea * 6 + (noarea_sub + 1) * i;
                    SETROME16(Prom, romsize, adr + (i + 6) * 2, tmp);
                }
            }
            adr += listsize;
            (*Poffsete) += listsize;
        }

        //データの実体
        // data entity
        {
            for (int k = 0; k < 1 + (noarea_sub != -1); k++) {
                int llen = noarea;
                if (k) llen = noarea_sub + 1;
                for (int q = 0; q < llen; q++) {
                    int tmp;
                    int tadr;
                    int shift;
                    tadr = adr;
                    tadr += k * noarea * 6;
                    tadr += q;
                    shift = llen;
                    //ΔＸ
                    if (!k)
                        tmp = (area[cc][q + 1].x - area[cc][q].x) / 8;
                    else {
                        if (q)
                            tmp = (area_sub[cc][q].x - area_sub[cc][q - 1].x) / 8;
                        else
                            tmp = (area_sub[cc][q].x - area[cc][area_sub_org].x) / 8;
                    }
                    if (tmp < -0x80 || tmp > 0x7F) return DISJOINT_AREAS.c_str();
                    SETROME8(Prom, romsize, tadr + shift * 0, tmp);
                    //ΔＹ
                    if (!k)
                        tmp = (area[cc][q + 1].y - area[cc][q].y) / 8;
                    else {
                        if (q)
                            tmp = (area_sub[cc][q].y - area_sub[cc][q - 1].y) / 8;
                        else
                            tmp = (area_sub[cc][q].y - area[cc][area_sub_org].y) / 8;
                    }
                    if (tmp < -0x80 || tmp > 0x7F) return DISJOINT_AREAS.c_str();
                    SETROME8(Prom, romsize, tadr + shift * 1, tmp);

                    AREA(*Pdest)[3][FZCD_MAX_AREA];
                    int offset = 1;
                    Pdest = &area;
                    if (k) {
                        offset = 0;
                        Pdest = &area_sub;
                    }

                    //パラメータ
                    // parameter
                    BYTE bval = 0x00;
                    if (k)
                        bval = 0x20;
                    else {
                        if ((noarea_sub != -1) && (q - area_sub_org) >= 0 &&
                            (q - area_sub_org) < noarea_sub + 1)
                            bval = 0x20;
                    }
                    {
                        BYTE val;
                        val = (*Pdest)[cc][q + offset].arg[0] | bval |
                              (*Pdest)[cc][q + offset].dir;
                        //					val |= 0x80 ;
                        SETROME8(Prom, romsize, tadr + shift * 2, val);
                    }
                    //ＣＰＵ挙動情報
                    // CPU behavior information
                    SETROME8(Prom, romsize, tadr + shift * 3,
                             (*Pdest)[cc][q + offset].arg[1]);
                    SETROME8(Prom, romsize, tadr + shift * 4,
                             (*Pdest)[cc][q + offset].arg[2]);
                    SETROME8(Prom, romsize, tadr + shift * 5,
                             (*Pdest)[cc][q + offset].arg[3]);
                }
            }
        }
        adr += noarea * 6;
        (*Poffsete) += noarea * 6;
        if (noarea_sub != -1) {
            adr += (noarea_sub + 1) * 6;
            (*Poffsete) += (noarea_sub + 1) * 6;
        }
    }


    return L"";
}
int FZCD ::Save(int region) {
    region_pre = region;
    wchar_t filename[64];
    swprintf(filename, FZCD_WORKING_PATH L"region%d.txt", region);
    FILE* fp;
    fp = _wfopen(filename, L"wt");
    if (!fp) return -2;

    wchar_t buf[128];
    const wchar_t* Pstr;
#define ERR_RET     \
    {               \
        fclose(fp); \
        return -1;  \
    }

    Pstr = STRING164.c_str();
    if (fwprintf(fp, L"%s\n", Pstr) < 0) ERR_RET;


#define WRITE_INT16_PARAM(token, val)                      \
    swprintf(buf, L"#%s%04X", token, (unsigned short)val); \
    if (fwprintf(fp, L"%s\n", buf) < 0) ERR_RET; /**/

    WRITE_INT16_PARAM(L"FZCD_VERSION ", FZCD_VERSION);
    WRITE_INT16_PARAM(L"AREA_BRANCH_ORG ", area_sub_org);

#define WRITE_BLOCK(Pd, size)                                 \
    ;                                                         \
    buf[0] = L'#';                                            \
    {                                                         \
        for (int i = 0; i < size; i++) {                      \
            swprintf(&buf[i % 32 * 2 + 1], L"%02X", (Pd)[i]); \
            if (i % 32 == 31) {                               \
                if (fwprintf(fp, L"%s\n", buf) < 0) ERR_RET;  \
                buf[1] = L'\0';                               \
            }                                                 \
        }                                                     \
        if (buf[1] != L'\0')                                  \
            if (fwprintf(fp, L"%s\n", buf) < 0) ERR_RET;      \
    } /**/

    Pstr = L"\n#EXPCHIP";
    if (fwprintf(fp, L"%s\n", Pstr) < 0) ERR_RET;
    WRITE_BLOCK(exptile[0], FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H * FZCD_SOE_CHIP);
    if (fwprintf(fp, L"%s\n", Pstr) < 0) ERR_RET;
    WRITE_BLOCK(exptile[1], FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H * FZCD_SOE_CHIP);
    if (fwprintf(fp, L"%s\n", Pstr) < 0) ERR_RET;
    WRITE_BLOCK(exptile[2], FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H * FZCD_SOE_CHIP);

    Pstr = L"\n#AREA";
    if (fwprintf(fp, L"%s\n", Pstr) < 0) ERR_RET;
    {
        for (int q = 0; q < 3; q++) {
            for (int i = 0; i < FZCD_MAX_AREA; i++) {
                if (fwprintf(fp, L"#%d,%d,%d,%d,", (int)area[q][i].isexist,
                             (int)area[q][i].x, (int)area[q][i].y,
                             (int)area[q][i].dir) < 0)
                    ERR_RET;
                for (int k = 0; k < FZCD_NO_AREA_ARG; k++)
                    if (fwprintf(fp, L"%d,", (int)area[q][i].arg[k]) < 0) ERR_RET;
                if (fwprintf(fp, L"\n") < 0) ERR_RET;
            }
            if (fwprintf(fp, L"\n") < 0) ERR_RET;
        }
    }

    Pstr = L"\n#AREA_SUB";
    if (fwprintf(fp, L"%s\n", Pstr) < 0) ERR_RET;
    {
        for (int q = 0; q < 3; q++) {
            for (int i = 0; i < FZCD_MAX_AREA; i++) {
                if (fwprintf(fp, L"#%d,%d,%d,%d,", (int)area_sub[q][i].isexist,
                             (int)area_sub[q][i].x, (int)area_sub[q][i].y,
                             (int)area_sub[q][i].dir) < 0)
                    ERR_RET;
                for (int k = 0; k < FZCD_NO_AREA_ARG; k++)
                    if (fwprintf(fp, L"%d,", (int)area_sub[q][i].arg[k]) < 0) ERR_RET;
                if (fwprintf(fp, L"\n") < 0) ERR_RET;
            }
            if (fwprintf(fp, L"\n") < 0) ERR_RET;
        }
    }
#undef INT16_PARAM
#undef ERR_RET
#undef WRITE_BLOCK

    fclose(fp);
    return 0;
}
int FZCD ::Load(int region) {
    wchar_t filename[64];
    int FZCDversion = 0x0000;
    swprintf(filename, FZCD_WORKING_PATH L"region%d.txt", region);
    FILE* fp;
    wchar_t buf[1024];
#define ERR_RET     \
    {               \
        fclose(fp); \
        return -1;  \
    }
    Clear();
    region_pre = region;

    fp = _wfopen(filename, L"rt");
    if (!fp) return -2;

    BYTE* Pwriting;
    int wsizelimit;
    int wpos;
    int mode;
    Pwriting = NULL;
    wpos = 0;
    wsizelimit = 0;
    mode = 0;
    for (;;) {
        if (!fgetws(buf, 1024, fp)) break;
        if (buf[0] != L'#') continue;
        if (buf[wcslen(buf) - 1] == L'\n') buf[wcslen(buf) - 1] = L'\0';
        if (!wcscmp(buf, L"#EXPCHIP")) {
            Pwriting = exptile[0];
            wsizelimit = FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H * FZCD_SOE_CHIP * 3;
            wpos = 0;
            mode = 0;
            continue;
        }
        if (!wcscmp(buf, L"#AREA")) {
            wpos = 0;
            mode = 1;
            continue;
        }
        if (!wcscmp(buf, L"#AREA_SUB")) {
            wpos = 0;
            mode = 2;
            continue;
        }
        {
            int tmp;
#define INT16_PARAM(token, val)                                               \
    if (!memcmp(buf, token, wcslen(token))) {                                 \
        if (swscanf(&buf[wcslen(token)], L"%x", &tmp) == 1) val = (short)tmp; \
        continue;                                                             \
    }

            INT16_PARAM(L"#FZCD_VERSION ", FZCDversion);
            INT16_PARAM(L"#AREA_BRANCH_ORG ", area_sub_org);

#undef INT16_PARAM
        }
        switch (mode) {
            case 0:
                if (!Pwriting) continue;
                {
                    wchar_t* Pread;
                    for (Pread = &buf[1];; Pread += 2) {
                        int tmp;
                        if (swscanf(Pread, L"%02X", &tmp) < 0) break;
                        Pwriting[wpos % wsizelimit] = tmp;
                        wpos++;
                    }
                }
                break;
            case 1:
            case 2: {
                AREA(*Pdest)[3][FZCD_MAX_AREA];
                Pdest = &area;
                if (mode == 2) Pdest = &area_sub;
                wchar_t* Pread;
                int tmp[10];
                Pread = &buf[1];
                if (swscanf(Pread, L"%d,%d,%d,%d,%d,%d,%d,%d", &tmp[0], &tmp[1],
                            &tmp[2], &tmp[3], &tmp[4], &tmp[5], &tmp[6], &tmp[7]) < 8)
                    break;
                (*Pdest)[wpos / FZCD_MAX_AREA][wpos % FZCD_MAX_AREA].isexist =
                        (tmp[0] != 0);
                (*Pdest)[wpos / FZCD_MAX_AREA][wpos % FZCD_MAX_AREA].x = tmp[1];
                (*Pdest)[wpos / FZCD_MAX_AREA][wpos % FZCD_MAX_AREA].y = tmp[2];
                (*Pdest)[wpos / FZCD_MAX_AREA][wpos % FZCD_MAX_AREA].dir = tmp[3];
                (*Pdest)[wpos / FZCD_MAX_AREA][wpos % FZCD_MAX_AREA].arg[0] = tmp[4];
                (*Pdest)[wpos / FZCD_MAX_AREA][wpos % FZCD_MAX_AREA].arg[1] = tmp[5];
                (*Pdest)[wpos / FZCD_MAX_AREA][wpos % FZCD_MAX_AREA].arg[2] = tmp[6];
                (*Pdest)[wpos / FZCD_MAX_AREA][wpos % FZCD_MAX_AREA].arg[3] = tmp[7];
                wpos++;
                wpos %= (FZCD_MAX_AREA * 3);
            } break;
            default:
                assert(0);
        }
    }
    fclose(fp);
#undef ERR_RET

    // FZCDバージョンコンバート
    // FZCD version conversion
    if (FZCDversion != FZCD_VERSION) {
        switch (FZCDversion) {
            //バージョン無し
            // No version
            case 0x0000: {
                //エリアパラメータをセットしてやる
                // Set area parameters
                for (int q = 0; q < 3; q++)
                    for (int i = 0; i < FZCD_MAX_AREA; i++) {
                        area[q][i].arg[0] = 0x00;
                        area[q][i].arg[1] = 0x23;
                        area[q][i].arg[2] = 0x22;
                        area[q][i].arg[3] = 0x11;
                    }
            }

            //通過
            // pass

            // 1.00 -> 1.01 繋ぎ換えのFFFFを透過に
            // 1.00 -> 1.01 Make connection FFFF transparent
            case 0x0100: {
                for (int icn = 1; icn < 3; icn++) {
                    //そのブロックを補正するか
                    // Correct that block?
                    for (int ibl = 0; ibl < FZCD_BLOCK_P_MAP_W * FZCD_BLOCK_P_MAP_H;
                         ibl++) {
                        int iba = 0;
                        for (; iba < FZCD_BAND_P_BLOCK_H; iba++) {
                            for (int ich = 0; ich < FZCD_CHIP_P_BAND_W; ich++) {
                                int px, py;
                                px = (ibl % 32 * 16 + 0 + ich);
                                py = (ibl / 32 * 16 + iba + 0);
                                for (int i = 0; i < FZCD_SOE_CHIP; i++)
                                    if (exptile[icn][(px + py * FZCD_CHIP_P_MAP_W) *
                                                             FZCD_SOE_CHIP +
                                                     i]) {
                                        iba = -1;
                                        break;
                                    }

                                if (iba == -1) break;
                            }
                            if (iba == -1) break;
                        }
                        if (iba == -1) continue;
                        //補正
                        // correction
                        for (iba = 0; iba < FZCD_BAND_P_BLOCK_H; iba++) {
                            int px, py;
                            px = (ibl % 32 * 16 + 0);
                            py = (ibl / 32 * 16 + iba);
                            memset(exptile[icn] + (px + py * FZCD_CHIP_P_MAP_W) *
                                                          FZCD_SOE_CHIP,
                                   0xFF, FZCD_CHIP_P_BAND_W * FZCD_SOE_CHIP);
                        }
                    }
                }
            }

            //通過
            // pass

            // 1.01 -> 1.02　分岐開始位置設定
            //コンバートの必要なし
            //通過
            // 1.01 -> 1.02 Branch start position setting
            // No need to convert
            // pass
            case 0x0101:;
        }
    }
    return 0;
}
