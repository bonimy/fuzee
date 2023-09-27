#include "common.h"


static void PrepareCalledTM();

MyDIBObj3 mdo(MAXSFC, WINWIDTH, WINHEIGHT);
Task0702Manager TM(MAXTASK, PrepareCalledTM);

BYTE* Prom;
int romsize;

int multi = 10;

FZCD working;
int editingre = 0;
int editingcn = 0;

static void PrepareCalledTM() { TM.AddTask(NULL, TP_APP_MANAGER, TF_app_manager); }

int AppPrepare(HWND hwnd, HINSTANCE hinstance) { return 0; }
int AppRelease() { return 0; }
