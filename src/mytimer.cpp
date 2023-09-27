/*
	主にゲーム用のために作ったタイマールーチン。
*/

#include <windows.h>
#include "mytimer.h"

//winmm.libが必要
#pragma comment(lib,"winmm.lib")

//固定小数。
//timeGetTimeのときに使う
#define		UBITS		24
#define		DEGMASK		(0xFF000000)
#define		D2F(num)	(DWORD)((num)*256*256*256)
#define		F2I(num)	((num)>>UBITS)

//何フレーム遅れたら、遅れを詰めるのを諦めるか
#define		GIVEUPFRAME		3
//次のフレームまで、この値ミリ秒を切ったら、スリープを終える
#define		SLEEPEND		2
//何フレーム分の時間、処理が遅れたら、フレームスキップ要求を発行するか
#define		FSREQFRAME		0.5

MyTimerObj :: MyTimerObj(int ifps)
{
	MFPS = 0;
	MFPScounter  = 0;
	MNOsleep = 0;
	timeGetDevCaps(&MAtc , sizeof(TIMECAPS));
	timeBeginPeriod(MAtc.wPeriodMin);

	if(QueryPerformanceFrequency(&MUtimervalue.SforPC.Mfreq))
	{
		MisTGT = false;
		QueryPerformanceCounter(&MUtimervalue.SforPC.Mstarttime);
		MUtimervalue.SforPC.Msecondstarttime = MUtimervalue.SforPC.Mstarttime;
	}
	else
	{
		MisTGT = true;
		MUtimervalue.SforTGT.Mstarttime = timeGetTime();
		MUtimervalue.SforTGT.Msecondstarttime = MUtimervalue.SforTGT.Mstarttime;
	}
	SetFPS(ifps);
}
MyTimerObj :: ~MyTimerObj()
{
	timeEndPeriod(MAtc.wPeriodMin);
}
void MyTimerObj :: SetFPS(int ifps)
{
double tmp;
	if(MisTGT)
	{
		tmp = (double)1000 / ifps;
		MUtimervalue.SforTGT.Mfixhztime = D2F(tmp);
		MUtimervalue.SforTGT.Mfixwaittime = MUtimervalue.SforTGT.Mfixhztime;
		MUtimervalue.SforPC.Mgiveuptime = (int)(tmp*GIVEUPFRAME);
		MUtimervalue.SforPC.Mframeskipreqtime = (int)(tmp*(1+FSREQFRAME));
	}
	else
	{
		MUtimervalue.SforPC.Mcountperframe = (INT32)(MUtimervalue.SforPC.Mfreq.QuadPart/ifps);
		MUtimervalue.SforPC.Mgiveuptime = MUtimervalue.SforPC.Mcountperframe * GIVEUPFRAME;
		MUtimervalue.SforPC.Mframeskipreqtime = (int)(MUtimervalue.SforPC.Mcountperframe * (1+FSREQFRAME)) ;
	}
}
bool MyTimerObj :: Wait(MSG *iptrmsg)
{
bool returnvalue=false;
bool pressurerecoded=false;
	if(MisTGT)
	{
DWORD timegap;
		MNOsleep = 0;
		for(;;)
		{
			if(PeekMessage(iptrmsg,NULL,0,0,PM_REMOVE)){
				if(iptrmsg->message == WM_QUIT){
					returnvalue = true;
				}
				TranslateMessage(iptrmsg);
				DispatchMessage(iptrmsg);
				if(returnvalue)break;
			}
			MUtimervalue.SforTGT.Mendtime = timeGetTime();
#ifdef _DEBUG
			if(!pressurerecoded)
			{
				Mpressure = MUtimervalue.SforTGT.Mendtime - MUtimervalue.SforTGT.Mstarttime;
				pressurerecoded = true;
			}
#endif
			timegap = MUtimervalue.SforTGT.Mendtime - MUtimervalue.SforTGT.Mstarttime;
			if(MUtimervalue.SforTGT.Msecondstarttime - MUtimervalue.SforTGT.Mendtime >= 1000)
			{
				MFPS = MFPScounter;
				MFPScounter = 0;
				MUtimervalue.SforTGT.Msecondstarttime += 1000;
			}
			if(timegap >= F2I(MUtimervalue.SforTGT.Mfixwaittime))
			{
				if(timegap < MUtimervalue.SforTGT.Mgiveuptime)MUtimervalue.SforTGT.Mstarttime += F2I(MUtimervalue.SforTGT.Mfixwaittime);
				else MUtimervalue.SforTGT.Mstarttime = MUtimervalue.SforTGT.Mendtime;
				MUtimervalue.SforTGT.Mfixwaittime = MUtimervalue.SforTGT.Mfixwaittime + MUtimervalue.SforTGT.Mfixhztime - (MUtimervalue.SforTGT.Mfixwaittime & DEGMASK);
				Mframeskiprequired = false ;
				if(timegap > MUtimervalue.SforTGT.Mframeskipreqtime)Mframeskiprequired=true ;
			MFPScounter++;
				break;
			}
			else if(timegap < F2I(MUtimervalue.SforTGT.Mfixwaittime)-SLEEPEND)
			{
				Sleep(1);
				MNOsleep++;
			}
			else
				Sleep(0);
		}
	}
	else//!isTGT
	{
long timegap;
		MNOsleep = 0;
		for(;;)
		{
			if(PeekMessage(iptrmsg,NULL,0,0,PM_REMOVE)){
				if(iptrmsg->message == WM_QUIT){
					returnvalue = true;
				}
				TranslateMessage(iptrmsg);
				DispatchMessage(iptrmsg);
				if(returnvalue)break;
			}
			QueryPerformanceCounter(&MUtimervalue.SforPC.Mendtime);
#ifdef _DEBUG
			if(!pressurerecoded)
			{
				Mpressure = (int)(MUtimervalue.SforPC.Mendtime.QuadPart - MUtimervalue.SforPC.Mstarttime.QuadPart);
				pressurerecoded = true;
			}
#endif
			timegap = (long)(MUtimervalue.SforPC.Mendtime.QuadPart - MUtimervalue.SforPC.Mstarttime.QuadPart);
			if(MUtimervalue.SforPC.Mendtime.QuadPart - MUtimervalue.SforPC.Msecondstarttime.QuadPart >= MUtimervalue.SforPC.Mfreq.QuadPart)
			{
				MUtimervalue.SforPC.Msecondstarttime.QuadPart += MUtimervalue.SforPC.Mfreq.QuadPart;
				MFPS = MFPScounter;
				MFPScounter = 0;
			}
			if(timegap >= MUtimervalue.SforPC.Mcountperframe)
			{
				if(timegap < MUtimervalue.SforPC.Mgiveuptime)MUtimervalue.SforPC.Mstarttime.QuadPart += MUtimervalue.SforPC.Mcountperframe;
				else MUtimervalue.SforPC.Mstarttime.QuadPart = MUtimervalue.SforPC.Mendtime.QuadPart;
				Mframeskiprequired = false ;
				if(timegap > MUtimervalue.SforPC.Mframeskipreqtime)Mframeskiprequired=true ;
				MFPScounter++;
				break;
			}
			if(timegap < MUtimervalue.SforPC.Mcountperframe-MUtimervalue.SforPC.Mfreq.QuadPart*SLEEPEND/1000)
			{
				Sleep(1);
				MNOsleep++;
			}else
				Sleep(0);
		}
	}
	return returnvalue;
}
double MyTimerObj :: ProcessTimeCount()
{
	if(MisTGT)
	{
DWORD timegap;
DWORD tmp;
			tmp = timeGetTime();
			timegap = tmp - MUtimervalue.SforTGT.Mprocessstart;
			MUtimervalue.SforTGT.Mprocessstart = tmp ;
			return (double)timegap/1000;
	}
	else
	{
long timegap;
LARGE_INTEGER tmp;
			QueryPerformanceCounter(&tmp);
			timegap = (long)(tmp.QuadPart - 
								MUtimervalue.SforPC.Mprocessstart.QuadPart);
			MUtimervalue.SforPC.Mprocessstart.QuadPart = tmp.QuadPart;
			return (double)timegap/MUtimervalue.SforPC.Mfreq.QuadPart;
	}

}
