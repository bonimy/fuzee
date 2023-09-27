#ifndef TASKPRIORITY_HEADER_INCLUDED
#define TASKPRIORITY_HEADER_INCLUDED
enum TaskP {
    TP_NONE = -3,
    TP_NOVALUE = -2,
    TP_HIGHEST = -1,
    ////////////////////////
    TP_APP_MANAGER,
    TP_FRAMEINIT,

    TP_MAIN,
    TP_MODE_MAP,
    TP_MM_KUI,
    TP_MODE_AREA,
    TP_MA_PARAM,

    TP_DUMMY,

    TP_FRAMEEND,
    ////////////////////////
    TP_LOWEST = 0x7FFF,
};
#endif /* TASKPRIORITY_HEADER_INCLUDED */