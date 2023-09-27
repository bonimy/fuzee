#ifndef SNES_SYS_HEADER_INCLUDED
#define SNES_SYS_HEADER_INCLUDED

extern void SetSNESPalette(UINT* Pict4, UINT* Pict16, UINT* Pict256);
extern void SetSNESDrawRoutine(void (*PiCLSFUNK)(int, int, int, int, int, UINT),
                               UINT* Pict4, UINT* Pict16, UINT* Pict256);
extern void DrawSNESCharacter(BYTE* Pbuf, int sfc, int x, int y, int mode,
                              bool emphasis, int zoom);

#endif /*SNES_SYS_HEADER_INCLUDED*/