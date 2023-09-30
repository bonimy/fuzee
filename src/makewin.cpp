#include "common.h"
#include "mytimer.h"


extern LRESULT CALLBACK windowproc(HWND hwnd, UINT message, WPARAM wparam,
                                   LPARAM lparam);


HWND hWnd;
HINSTANCE hInstance;

/*
Load resources for a given region.

0x409: English (US)
0x411: Japanese (J)
*/
void InitializeResources(LCID locale, LANGID language) {
    SetThreadLocale(0x411);
    SetThreadUILanguage(0x411);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hpinstance, LPSTR lpszcmdline,
                   int ncmdshow) {
    InitializeResources(GetThreadLocale(), GetThreadUILanguage());

#ifndef ENABLE_MULTIPLE_RUN
    HWND tmpwnd;
    //簡易二重起動禁止
    // Prohibit simple double startup
    if ((tmpwnd = FindWindow(COMMON_CONST_WINDOW_CLASS_NAME, NULL)) != NULL) {
        ShowWindow(tmpwnd, SW_RESTORE);
        SetForegroundWindow(tmpwnd);
        return -1;
    }
#endif

    HWND hwnd = NULL;
    WNDCLASS wc;
    MSG msg;
    wc.lpszClassName = COMMON_CONST_WINDOW_CLASS_NAME;
    wc.lpszMenuName = NULL;
    wc.hInstance = hinstance;
    wc.lpfnWndProc = windowproc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    if (!RegisterClass(&wc)) return 1;
    if ((hwnd = CreateWindowExW(
#ifdef MY_DROP_ACCEPT
                 WS_EX_ACCEPTFILES
#else
                 0
#endif
                 ,
                 wc.lpszClassName, COMMON_CONST_DEFAULT_WINDOW_TEXT,
                 WS_CAPTION | WS_SYSMENU, 0, 0,
                 WINWIDTH + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
                 WINHEIGHT + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
                         GetSystemMetrics(SM_CYCAPTION),
                 HWND_DESKTOP, NULL, hinstance, NULL)) == NULL) {
        return -1;
    }
    hWnd = hwnd;
    hInstance = hinstance;
    const wchar_t* f = COMMON_CONST_DEFAULT_WINDOW_TEXT;

    ShowWindow(hwnd, ncmdshow);
    UpdateWindow(hwnd);
    RedrawWindow(hwnd, NULL, NULL, RDW_INTERNALPAINT | RDW_UPDATENOW);

    if (AppPrepare(hwnd, hinstance)) {
        DestroyWindow(hwnd);
    }


    MyTimerObj mto(60);

    for (;;) {
        MainRoutine(mto.GetFPS());
        if (mto.Wait(&msg)) break;
    }
    AppRelease();

    return msg.wParam;
}