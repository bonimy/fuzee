#include <stdio.h>
#include "_65816m_local.h"

EmuCore :: EmuCore()
{
	d.Pram = new BYTE[0x020000];
	d.Pstack = new BYTE[0x0200];
#ifdef MEMORY_CHECK
	d.Pramc = new BYTE[0x020000];
#endif
	d.Is64KBank = false ;
	Initialize();
}
void EmuCore :: ClearRegister()
{
	d.reg.a.w = 0x0000;
	d.reg.db  = 0x00;
	d.reg.x.w = 0x0000;
	d.reg.y.w = 0x0000;
	d.reg.d   = 0x00;
	d.reg.s   = 0x00;
	d.reg.pb  = 0x00;
	d.reg.pc  = 0x0000;
int i,q;
	for( i=0 ; i<8 ; i++ )
		for( q=0 ; q<0x10 ; q++ )
			d.AADMA[i][q] = 0x00;
#ifdef TRACER_ENABLED
	for( i=0 ; i<TRACE_SIZE ; i++)trace_buffer[i]=0;
	trace_point = 0;
#endif
}

void EmuCore :: Initialize()
{
	ClearRegister();
int i;
#ifdef MEMORY_CHECK
	for( i=0 ; i<0x20000 ; i++)d.Pramc[i]=0;
#endif
#ifdef PICTURE_TEST
	for( i=0 ; i<PICTURE_BUFFER_SIZE ; i++)d.Avram[i]=0;
	d.render_point = 0;
	d.render_inc_size = 1;
#endif
}
void EmuCore :: Release()
{
	delete d.Pram;
	delete d.Pstack;
#ifdef MEMORY_CHECK
	delete d.Pramc;
#endif
}
