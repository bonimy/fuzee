#include "_task_mode_area.h"
#include "common.h"

using namespace ModeArea;

BYTE ecopy[4];


static void ChangeSpeed(BYTE* Pdest, int inc) {
    int tmp = (*Pdest);
    tmp &= 3;
    tmp += inc;
    RotateCorrect(&tmp, 4);
    (*Pdest) &= ~0x03;
    (*Pdest) |= tmp;
    RequestRedraw();
}
static void ChangeAngle(BYTE* Pdest, int inc) {
    int tmp = (*Pdest);
    tmp >>= 4;
    tmp &= 3;
    tmp += inc;
    RotateCorrect(&tmp, 4);
    (*Pdest) &= ~0x30;
    (*Pdest) |= tmp << 4;
    RequestRedraw();
}
static void ChangeShift(BYTE* Pdest, int inc) {
    static int table[5] = {0, 2, 1, 3, 4};
    static int table_inv[5] = {0, 2, 1, 3, 4};
    int tmp = (*Pdest);
    tmp >>= 2;
    tmp &= 3;
    tmp = table[tmp + 1];
    if (!((*Pdest) & 0xC0))
        tmp = 0;
    else if ((*Pdest) & 0x80)
        tmp *= -1;

    tmp += inc;
    while (tmp > 4) {
        tmp += -9;
    }
    while (tmp < -4) {
        tmp += 9;
    }

    (*Pdest) &= ~0xCC;
    if (tmp) {
        if (tmp < 0) {
            (*Pdest) |= 0x80;
            tmp *= -1;
        } else
            (*Pdest) |= 0x40;
        tmp = table_inv[tmp] - 1;
        (*Pdest) |= (tmp << 2);
    }
    RequestRedraw();
}

bool TF_mode_area_param(TCB* caller) {
    ALIAS_MAIN(caller->PmotherTCB->PmotherTCB, main);
    if (!caller->calltiming) {
        caller->Pdraw = TV_mode_area_param;
        return false;
    }
    if (MousePush(MB_L) && SelectArea(mainx, mainy)) {
        RequestRedraw();
    }

    AREA* Pdest = working.AreaGetSelectedArea(editingcn, selected);
    if (!Pdest) return false;
    if (KeyPush(KC_R)) ChangeSpeed(&Pdest->arg[1 + 0], -1);
    if (KeyPush(KC_T)) ChangeSpeed(&Pdest->arg[1 + 0], 1);
    if (KeyPush(KC_F)) ChangeSpeed(&Pdest->arg[1 + 1], -1);
    if (KeyPush(KC_G)) ChangeSpeed(&Pdest->arg[1 + 1], 1);
    if (KeyPush(KC_V)) ChangeSpeed(&Pdest->arg[1 + 2], -1);
    if (KeyPush(KC_B)) ChangeSpeed(&Pdest->arg[1 + 2], 1);

    if (KeyPush(KC_Y)) ChangeAngle(&Pdest->arg[1 + 0], -1);
    if (KeyPush(KC_U)) ChangeAngle(&Pdest->arg[1 + 0], 1);
    if (KeyPush(KC_H)) ChangeAngle(&Pdest->arg[1 + 1], -1);
    if (KeyPush(KC_J)) ChangeAngle(&Pdest->arg[1 + 1], 1);
    if (KeyPush(KC_N)) ChangeAngle(&Pdest->arg[1 + 2], -1);
    if (KeyPush(KC_M)) ChangeAngle(&Pdest->arg[1 + 2], 1);

    if (KeyPush(KC_I)) ChangeShift(&Pdest->arg[1 + 0], -1);
    if (KeyPush(KC_O)) ChangeShift(&Pdest->arg[1 + 0], 1);
    if (KeyPush(KC_K)) ChangeShift(&Pdest->arg[1 + 1], -1);
    if (KeyPush(KC_L)) ChangeShift(&Pdest->arg[1 + 1], 1);
    if (KeyPush(KC_COMMA)) ChangeShift(&Pdest->arg[1 + 2], -1);
    if (KeyPush(KC_PERIOD)) ChangeShift(&Pdest->arg[1 + 2], 1);

    if (KeyPush(KC_1)) {
        Pdest->arg[0] ^= 0x40;
        RequestRedraw();
    }
    if (KeyPush(KC_2)) {
        Pdest->arg[0] ^= 0x80;
        RequestRedraw();
    }
    if (KeyPush(KC_3)) {
        Pdest->arg[0] ^= 0x10;
        RequestRedraw();
    }
    {
        int tmp = 0;
        if (KeyPush(KC_PAGEUP)) tmp++;
        if (KeyPush(KC_PAGEDOWN)) tmp--;
        if (tmp) {
            int pre;
            pre = selected;
            selected += tmp;
            if (selected < 0 || selected % 0x1000 >= FZCD_MAX_AREA ||
                !working.AreaGetSelectedArea(editingcn, selected)->isexist)
                selected = pre;
            RequestRedraw();
        }
    }
    if (KeyPush(KC_C)) {
        memcpy(ecopy, &Pdest->arg[0], 4);
    }
    if (KeyPush(KC_X)) {
        if (KeyOn(KC_CTRL)) {
            for (int i = 0; i < 2; i++) {
                Pdest->arg[1 + i] &= ~0x33;
                Pdest->arg[1 + i] |= (ecopy[i + 1] & 0x33);
            }
        } else
            memcpy(&Pdest->arg[1], ecopy + 1, 3);
        Pdest->arg[0] &= ~0xF0;
        Pdest->arg[0] |= ecopy[0] & 0xF0;

        RequestRedraw();
    }
    ALIAS_END();
    return false;
}

void TV_mode_area_param(TCB* caller) {
    ALIAS_MAIN(caller->PmotherTCB->PmotherTCB, main);
    AREA* Pdest = working.AreaGetSelectedArea(editingcn, selected);
    DrawArea(mainx, mainy);
    if (selected != -1) {
        KAGEMOJI(SFC_BACK, STRING139.c_str(), 12, WINHEIGHT - 16 * (4), 16, 8);
        for (int i = 0; i < 3; i++) {
            const wchar_t* Pstr[3] = {
                    STRING140.c_str(),
                    STRING141.c_str(),
                    STRING142.c_str(),
            };
            const wchar_t* Pstr2[3] = {
                    STRING143.c_str(),
                    STRING144.c_str(),
                    STRING145.c_str(),
            };
            wchar_t buf[128];
            int tmp = Pdest->arg[1 + i];
            swprintf(buf, L"%s：　%d　　　%d　　　%s", Pstr[i], tmp & 3, (tmp >> 4) & 3,
                     Pstr2[i]);
            KAGEMOJI(SFC_BACK, buf, 12, WINHEIGHT - 16 * (3 - i), 16, 8);
            DrawMachineToken(6, WINHEIGHT - 16 * (3 - i) + 8, i);
        }
        {
            wchar_t buf[128];
            const wchar_t* onoff[2] = {ON.c_str(), OFF.c_str()};
            const wchar_t* offon[2] = {OFF.c_str(), ON.c_str()};
            swprintf(buf, STRING148.c_str(), onoff[!!(Pdest->arg[0] & 0x40)],
                     onoff[!!(Pdest->arg[0] & 0x80)]);
            KAGEMOJI(SFC_BACK, buf, 300, WINHEIGHT - 16 * 3, 16, 8);
            swprintf(buf, STRING149.c_str(), offon[!!(Pdest->arg[0] & 0x10)]);
            KAGEMOJI(SFC_BACK, buf, 300, WINHEIGHT - 16 * 2, 16, 8);
        }
        KAGEMOJI(SFC_BACK, STRING150.c_str(), 300, WINHEIGHT - 16, 16, 8);
    }
    ALIAS_END();
}
