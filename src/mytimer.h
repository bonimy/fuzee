#ifndef MYTIMER_HEADER_INCLUDED
#define MYTIMER_HEADER_INCLUDED

/*
	主にゲーム用のために作ったタイマールーチン。



//メッセージループの変わりに、このループを使う
//Func()は、一定間隔で呼び出したい関数

MyTimerObj mto(60);
	for(;;)
	{
		Func();
		if(mto.Wait(&msg))break;
	}
*/

/*
　これを作ったときには、メンバ変数は最初にMをつけよう！とか考えてたみたい＾＾
　逆に見づらくね？

　それは置いておいて、このタイマーは、
　使えるなら、QueryPerformanceCounterを、使えないならtimeGetTimeを使います。
　精度が段違いなんですが……でも、前者が使えないことってあるのかなぁ。
*/

class MyTimerObj
{
private:
	TIMECAPS MAtc;
	bool MisTGT;
	union
	{
		struct
		{
			DWORD Mstarttime;
			DWORD Mendtime;
			DWORD Msecondstarttime;
			DWORD Mfixwaittime;
			DWORD Mfixhztime;
			DWORD Mgiveuptime;
			DWORD Mframeskipreqtime;
			DWORD Mprocessstart;
		}SforTGT;
		struct
		{
			LARGE_INTEGER Mstarttime;
			LARGE_INTEGER Mendtime;
			LARGE_INTEGER Msecondstarttime;
			LARGE_INTEGER Mfreq;
			INT32 Mcountperframe;
			INT32 Mgiveuptime;
			INT32 Mframeskipreqtime;
			LARGE_INTEGER Mprocessstart;
		}SforPC;
	}MUtimervalue;
	int MFPScounter;
	int MFPS;
	int MNOsleep;
	int Mpressure;
	bool Mframeskiprequired ;
public:
	MyTimerObj(int ifps=60);
	~MyTimerObj();
	void SetFPS(int ifps=60);
	bool Wait(MSG *iptrmsg);

	//実測ＦＰＳを返す
	int GetFPS(){return MFPS;}

	//ウェイトの間に、何回、Sleep(1)したかを返す。多いほど余裕ってこと。
	int GetNofSleep(){return MNOsleep;}

	//デバッグ時のみ。Func()を処理した負荷を返す？いや、プレッシャーじゃないだろ……って話だけど
	int GetPressure(){return Mpressure;}

	//↑のを、なんか時間に変換したものを返す？
	int GetProcessTime(){if(MisTGT)return Mpressure*1000*10;return (int)(Mpressure * 1000 * 10 / MUtimervalue.SforPC.Mfreq.QuadPart);}

	//？何に使うのか、ウェイトの最後の時間を返す？
	DWORD GetEndTime(){if(MisTGT)return MUtimervalue.SforTGT.Mendtime;return MUtimervalue.SforPC.Mendtime.LowPart;}

	//それなりの精度で処理時間を測定するのに使う。
	//１回前に、この関数を呼び出してからの時間経過を秒単位で返す。
	double ProcessTimeCount();
	
	//フレームスキップが望まれているかを返す
	bool FrameSkipIsRequired(){return Mframeskiprequired ; }
};


#endif /*MYTIMER_HEADER_INCLUDED*/