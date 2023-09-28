#include <assert.h>
#include <stdio.h>
#include <windows.h>

#ifndef FZCD_HEADER_INCLUDED
#define FZCD_HEADER_INCLUDED

//マップのサイズ（ブロック数）
// Map size (number of blocks)
#define FZCD_BLOCK_P_MAP_W 32
#define FZCD_BLOCK_P_MAP_H 16
#define FZCD_BLOCK_P_MAP (FZCD_BLOCK_P_MAP_W * FZCD_BLOCK_P_MAP_H)

//ブロックのサイズ（バンド数）
// Block size (number of bands)
#define FZCD_BAND_P_BLOCK_W 1
#define FZCD_BAND_P_BLOCK_H 16
//１つのブロックをさすのに必要なバイト数
// Number of bytes required to point one block
#define FZCD_SOE_BLOCK 1
//１つのブロックに含まれるバンド数
// Number of bands included in one block
#define FZCD_BAND_P_BLOCK (FZCD_BAND_P_BLOCK_W * FZCD_BAND_P_BLOCK_H)
//１つのブロックのバイト数（バンド数×バンド１つ）
// Number of bytes in one block (number of bands x one band)
#define FZCD_SOA_BLOCK (FZCD_BAND_P_BLOCK * FZCD_SOE_BAND)


//バンドのサイズ（チップ数）
// Band size (number of chips)
#define FZCD_CHIP_P_BAND_W 16
#define FZCD_CHIP_P_BAND_H 1
#define FZCD_SOE_BAND 2
#define FZCD_CHIP_P_BAND (FZCD_CHIP_P_BAND_W * FZCD_CHIP_P_BAND_H)
#define FZCD_SOA_BAND (FZCD_CHIP_P_BAND * FZCD_SOE_CHIP)

//チップのサイズ（８×８の数）
// Chip size (8x8 number)
#define FZCD_TILE_P_CHIP_W 2
#define FZCD_TILE_P_CHIP_H 2
#define FZCD_SOE_CHIP 2
#define FZCD_TILE_P_CHIP (FZCD_TILE_P_CHIP_W * FZCD_TILE_P_CHIP_H)
#define FZCD_SOA_CHIP (FZCD_SOE_CHIP * FZCD_TILE_P_CHIP)


//別にこんなの準備する必要ないのだが、
//頭がこんがらがってしまった……
// There is no need to prepare anything like this,
// My head is confused...

#define FZCD_BAND_P_MAP_W (FZCD_BAND_P_BLOCK_W * FZCD_BLOCK_P_MAP_W)
#define FZCD_CHIP_P_MAP_W (FZCD_CHIP_P_BAND_W * FZCD_BAND_P_MAP_W)
#define FZCD_TILE_P_MAP_W (FZCD_TILE_P_CHIP_W * FZCD_CHIP_P_MAP_W)
#define FZCD_CHIP_P_BLOCK_W (FZCD_CHIP_P_BAND_W * FZCD_BAND_P_BLOCK_W)
#define FZCD_TILE_P_BLOCK_W (FZCD_TILE_P_CHIP_W * FZCD_CHIP_P_BLOCK_W)
#define FZCD_TILE_P_BAND_W (FZCD_TILE_P_CHIP_W * FZCD_CHIP_P_BAND_W)

#define FZCD_BAND_P_MAP_H (FZCD_BAND_P_BLOCK_H * FZCD_BLOCK_P_MAP_H)
#define FZCD_CHIP_P_MAP_H (FZCD_CHIP_P_BAND_H * FZCD_BAND_P_MAP_H)
#define FZCD_TILE_P_MAP_H (FZCD_TILE_P_CHIP_H * FZCD_CHIP_P_MAP_H)
#define FZCD_CHIP_P_BLOCK_H (FZCD_CHIP_P_BAND_H * FZCD_BAND_P_BLOCK_H)
#define FZCD_TILE_P_BLOCK_H (FZCD_TILE_P_CHIP_H * FZCD_CHIP_P_BLOCK_H)
#define FZCD_TILE_P_BAND_H (FZCD_TILE_P_CHIP_H * FZCD_CHIP_P_BAND_H)


#define FZCD_MAP_SPACE_LIMIT 0x200
#define FZCD_BAND_SPACE_LIMIT 0x2000
#define FZCD_CHIP_SPACE_LIMIT 0x9000

#define FZCD_MAX_AREA 0xFE
#define FZCD_NO_AREA_ARG 4

#define FZCD_WORKING_PATH "working\\"
#define FZCD_VERSION 0x0102

typedef struct AREA_tag {
    bool isexist;
    int x;
    int y;
    int dir;
    BYTE arg[FZCD_NO_AREA_ARG];
} AREA;

class FZCD {
public:
    BYTE exptile[3][FZCD_CHIP_P_MAP_W * FZCD_CHIP_P_MAP_H * FZCD_SOE_CHIP];

    AREA area[3][FZCD_MAX_AREA];
    AREA area_sub[3][FZCD_MAX_AREA];

    int area_sub_org;

    void Clear();
    const char* Write2ROM(BYTE* Prom, int romsize, int* Poffset, int* Poffsete,
                          int* Poffsetcc, int* Pareaadrlist, bool culcmode = false,
                          int* Psizeofband = NULL, int* Psizeofchip = NULL);


    int Save(int region);
    int Load(int region);

    void AreaDeleteArea(int layer, int areano) {
        AREA(*Pdestarea)[3][FZCD_MAX_AREA];
        if (areano == -1) return;
        Pdestarea = &area_sub;
        if (areano < 0x1000) Pdestarea = &area;
        areano %= 0x1000;
        (*Pdestarea)[layer][areano].isexist = false;
        int i = areano;
        for (; i < FZCD_MAX_AREA - 1; i++) {
            (*Pdestarea)[layer][i] = (*Pdestarea)[layer][i + 1];
        }
        (*Pdestarea)[layer][i].isexist = false;
    }
    bool AreaInsertArea(int layer, int areano) {
        AREA(*Pdestarea)[3][FZCD_MAX_AREA];
        if (areano == -1) return false;
        Pdestarea = &area_sub;
        if (areano < 0x1000) Pdestarea = &area;
        areano %= 0x1000;
        if (area[layer][FZCD_MAX_AREA - 1].isexist) return false;
        for (int q = FZCD_MAX_AREA - 1; q > areano; q--) {
            (*Pdestarea)[layer][q] = (*Pdestarea)[layer][q - 1];
        }
        (*Pdestarea)[layer][areano].x -= 8;
        (*Pdestarea)[layer][areano].y -= 8;
        return true;
    }
    AREA* AreaAddArea(int layer, bool sub, int* Pindex) {
        AREA(*Pdestarea)[3][FZCD_MAX_AREA];
        Pdestarea = &area;
        if (sub) Pdestarea = &area_sub;
        int i = 0;
        for (; i < FZCD_MAX_AREA; i++) {
            if (!(*Pdestarea)[layer][i].isexist) break;
        }
        if (i == FZCD_MAX_AREA) {
            return NULL;
        }
        (*Pindex) = i + sub * 0x1000;
        return &(*Pdestarea)[layer][i];
    }
    AREA* AreaGetSelectedArea(int layer, int areano) {
        if (areano == -1) return NULL;
        if (areano % 0x1000 >= FZCD_MAX_AREA) return NULL;
        if (areano < 0x1000) return &area[layer][areano];
        return &area_sub[layer][areano - 0x1000];
    }

private:
    int region_pre;
};


#endif /*FZCD_HEADER_INCLUDED*/
