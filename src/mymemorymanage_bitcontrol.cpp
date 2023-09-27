/*
	よく意図のわからないメモリ管理ルーチン。

	……の１つの機能の、ビット単位でデータを弄くるためのルーチンらしい。
*/
#include <stdio.h>
#include <io.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include <assert.h>


static int bitcursorsenv;
static unsigned char *bitcursorbyte;
static int IObit;
static bool isfirst;
void BitControlStart(unsigned char *headbyte)
{
	bitcursorbyte = headbyte;
	bitcursorsenv = 0;
	IObit = 0;
	isfirst=true;
}
static unsigned char bittable[8] =
{
	0x01,
	0x03,
	0x07,
	0x0F,
	0x1F,
	0x3F,
	0x7F,
	0xFF,
};
void BitControlAdd(unsigned char data , int nobit , bool isnoproce=false)
{
	if(isfirst)
	{
		*bitcursorbyte = 0;
		isfirst = false;
	}
	if(nobit <= 0 || nobit >= 9)return;

	assert(bitcursorsenv >= 0 || bitcursorsenv < 8);

	if(nobit+bitcursorsenv < 9)
	{
		*bitcursorbyte |= (data>>bitcursorsenv);
		if(!isnoproce)
		{
			bitcursorsenv += nobit;
			if(bitcursorsenv == 8)
			{
				bitcursorbyte++;
				*bitcursorbyte = 0;
			}
			if(bitcursorsenv > 8)assert(0);
			bitcursorsenv %= 8; 
		}
	}
	else
	{
		*bitcursorbyte |= (data>>(bitcursorsenv));
		*(bitcursorbyte+1) = (data<<(8-bitcursorsenv));
		if(!isnoproce)
		{
			bitcursorbyte ++;
			bitcursorsenv += nobit;
			bitcursorsenv %= 8;
		}
	}
	if(!isnoproce)IObit += nobit;
}
unsigned char BitControlGet(int nobit , bool isnoproce=false)
{
int rv;
	if(nobit <= 0 || nobit >= 9)return 0;

	assert(bitcursorsenv >= 0 || bitcursorsenv < 8);

	if(nobit+bitcursorsenv < 9)
	{
		rv = (*bitcursorbyte & (0xFF>>bitcursorsenv) & (0xFF<<(8-bitcursorsenv-nobit))) << bitcursorsenv;
		if(!isnoproce)
		{
			bitcursorsenv += nobit;
			if(bitcursorsenv == 8)
			{
				bitcursorbyte++;
			}
			if(bitcursorsenv > 8)assert(0);
			bitcursorsenv %= 8; 
		}
	}
	else
	{
		rv = ((*bitcursorbyte & (0xFF >> bitcursorsenv)) << bitcursorsenv)
			+
			((*(bitcursorbyte+1) & (0xFF << (8-(bitcursorsenv+nobit-8)))) >> (8-bitcursorsenv));
		if(!isnoproce)
		{
			bitcursorbyte ++;
			bitcursorsenv += nobit;
			bitcursorsenv %= 8;
		}
	}
	if(!isnoproce)IObit += nobit;
	return rv;
}
int  BitControlGetIOBit()
{
	return IObit;
}
void BitControlSeek(int nobit)
{
int nobyte;
	if(nobit == 0)return;
	nobyte = nobit/8;
	nobit  = nobit%8;
	if(nobit<0)
	{
		bitcursorbyte -= (-nobyte);
		bitcursorsenv -= (-nobit);
		if(bitcursorsenv < 0)
		{
			bitcursorbyte --;
			bitcursorsenv += 8;
			assert(bitcursorsenv >= 0);
		}
	}
	else	/* nobit > 0*/
	{
		bitcursorbyte += nobyte;
		bitcursorsenv += nobit;
		if(bitcursorsenv >= 8)
		{
			bitcursorbyte++;
			bitcursorsenv -= 8;
			assert(bitcursorsenv < 8);
		}
	}
}
