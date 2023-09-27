#include "common.h"


void MainRoutine(int fps) {
    if (!TM.Run()) DestroyWindow(hWnd);
}
