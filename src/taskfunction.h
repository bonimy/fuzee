#define DEF_FDV(name)                   \
    ;                                   \
    ;                                   \
    extern bool TF_##name(TCB* caller); \
    extern void TD_##name(TCB* caller); \
    extern void TV_##name(TCB* caller); \
    ;
#define DEF_FD(name)                    \
    ;                                   \
    ;                                   \
    extern bool TF_##name(TCB* caller); \
    extern void TD_##name(TCB* caller); \
    ;
#define DEF_FV(name)                    \
    ;                                   \
    ;                                   \
    extern bool TF_##name(TCB* caller); \
    extern void TV_##name(TCB* caller); \
    ;
#define DEF_DV(name)                    \
    ;                                   \
    ;                                   \
    extern void TD_##name(TCB* caller); \
    extern void TV_##name(TCB* caller); \
    ;
#define DEF_F(name)                     \
    ;                                   \
    ;                                   \
    extern bool TF_##name(TCB* caller); \
    ;
#define DEF_D(name)                     \
    ;                                   \
    ;                                   \
    extern void TD_##name(TCB* caller); \
    ;
#define DEF_V(name)                     \
    ;                                   \
    ;                                   \
    extern void TV_##name(TCB* caller); \
    ;

DEF_FD(app_manager);
DEF_FV(frame_init);
DEF_FV(frame_end);


DEF_FDV(main);
DEF_FV(mode_map);
DEF_FDV(mm_kui);
DEF_FV(mode_area);
DEF_FV(mode_area_param);


/*
bool TF_(TCB *caller)
{
        ALIAS_(caller,)
        if(!caller->calltiming)
        {
                return false;
        }
        ALIAS_END();
        return false;
}
void TD_(TCB *caller)
{
        ALIAS_(caller,)
        ALIAS_END();
}
void TV_(TCB *caller)
{
        ALIAS_(caller,)
        ALIAS_END();
}
*/
