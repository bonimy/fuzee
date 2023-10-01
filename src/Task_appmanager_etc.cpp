#include <wchar.h>

#include "common.h"
#include "resource_strings.hxx"

void T_CLS(int sfc, int x, int y, int w, int h, UINT color) {
    mdo.Cls(MDO3normal, sfc, x, y, w, h, color);
}

bool TF_app_manager(TCB* caller) {
    if (!caller->calltiming) {
        caller->Pdest = TD_app_manager;


        Prom = NULL;
        if ((romsize = LoadMemoryFromFile(BASE_ROM_NAME, &Prom)) <= 0) {
            MessageBox(hWnd, /*BASE_ROM_NAME*/ STRING171.c_str(), ERROR_TEXT.c_str(),
                       MB_OK);
            return true;
        }
        {
            bool onerror = false;
            if (romsize != DEFAULT_ROM_SIZE) {
                if (romsize == DEFAULT_ROM_SIZE + 0x200) {
                    MessageBox(hWnd, /*BASE_ROM_NAME*/ STRING172.c_str(),
                               ERROR_TEXT.c_str(), MB_OK);
                    onerror = true;
                } else {
                    MessageBox(hWnd, STRING173.c_str(), ERROR_TEXT.c_str(), MB_OK);
                    onerror = true;
                }
            } else {
                unsigned int eh = EHash(Prom, romsize);
                if (eh != EHASH_OF_AUTHOR) {
                    wchar_t str[256];
                    swprintf(str, STRING174.c_str(), EHASH_OF_AUTHOR, eh);
                    MessageBox(hWnd, str, ERROR_TEXT.c_str(), MB_OK);
                    onerror = true;
                }
            }
            if (onerror) {
                int ans;
                ans = MessageBox(hWnd, STRING175.c_str(), STRING176.c_str(), MB_YESNO);
                if (ans == IDNO) return true;
            }
        }


        mdo.Initialize(hWnd, hInstance);

        mdo.CreateSurface(SFC_BG, 128, 256);
        mdo.CreateSurface(SFC_MINIMAP, 32, 64);

        SetSNESDrawRoutine(T_CLS, NULL, NULL, NULL);

        TM.AddTask(caller, TP_FRAMEINIT, TF_frame_init);
        TM.AddTask(caller, TP_FRAMEEND, TF_frame_end);

        TM.AddTask(caller, TP_MAIN, TF_main);
        return false;
    }
    if (!TM.SearchTask(caller, TP_MAIN)) return true;
    return false;
}
void TD_app_manager(TCB* caller) { mdo.Release(); }

bool TF_frame_init(TCB* caller) {
    if (!caller->calltiming) {
        caller->Pdraw = TV_frame_init;
        return false;
    }
    return false;
}
void TV_frame_init(TCB* caller) {}


bool TF_frame_end(TCB* caller) {
    if (!caller->calltiming) {
        caller->Pdraw = TV_frame_end;
        return false;
    }
    KeyMove();
    ALIAS_FRAME_END(caller, fe);
    if (feisredraw) {
        mdo.Cls(MDO3normal, 0, 0, 0, WINWIDTH, WINHEIGHT, myRGB(28, 31, 28));
        TM.DoDrawFunctionChain();
        feisredraw = 0;
        feisflip = 1;
    }
    if (feisflip) {
        mdo.Flip(MDO3FT_NORMAL);
        feisflip = 0;
    }
    ALIAS_END();
    return false;
}

void TV_frame_end(TCB* caller) {}


static TCB* Pfe = NULL;
static unsigned __int32 index;

static bool CheckFE() {
    if (!Pfe || Pfe->index != index) {
        Pfe = TM.SearchTask(NULL, TP_APP_MANAGER);
        Pfe = TM.SearchTask(Pfe, TP_FRAMEEND);
        if (!Pfe) return false;
        index = Pfe->index;
    }
    return true;
}
void RequestRedraw() {
    if (!CheckFE()) return;
    ALIAS_FRAME_END(Pfe, fe);
    feisredraw = 1;
    ALIAS_END();
}
void RequestFlip() {
    if (!CheckFE()) return;
    ALIAS_FRAME_END(Pfe, fe);
    feisflip = 1;
    ALIAS_END();
}
