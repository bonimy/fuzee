#ifndef MYTIMER_HEADER_INCLUDED
#define MYTIMER_HEADER_INCLUDED

#include <Windows.h>

/*
        主にゲーム用のために作ったタイマールーチン。
        A timer routine made mainly for gaming.



//メッセージループの変わりに、このループを使う
//Func()は、一定間隔で呼び出したい関数
//Use this loop instead of the message loop
//Func() is the function you want to call at regular intervals

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
  When you create this, add M at the beginning of the member variables! It seems like I
  was thinking about it ^^ On the other hand, it's hard to see, isn't it?

  Leaving that aside, this timer is
  If possible, use QueryPerformanceCounter, otherwise use timeGetTime.
  The accuracy is different... but I wonder if there are times when the former cannot be
  used.
*/

class MyTimerObj {
private:
    TIMECAPS MAtc;
    bool MisTGT;
    union {
        struct {
            DWORD Mstarttime;
            DWORD Mendtime;
            DWORD Msecondstarttime;
            DWORD Mfixwaittime;
            DWORD Mfixhztime;
            DWORD Mgiveuptime;
            DWORD Mframeskipreqtime;
            DWORD Mprocessstart;
        } SforTGT;
        struct {
            LARGE_INTEGER Mstarttime;
            LARGE_INTEGER Mendtime;
            LARGE_INTEGER Msecondstarttime;
            LARGE_INTEGER Mfreq;
            INT32 Mcountperframe;
            INT32 Mgiveuptime;
            INT32 Mframeskipreqtime;
            LARGE_INTEGER Mprocessstart;
        } SforPC;
    } MUtimervalue;
    int MFPScounter;
    int MFPS;
    int MNOsleep;
    int Mpressure;
    bool Mframeskiprequired;

public:
    MyTimerObj(int ifps = 60);
    ~MyTimerObj();
    void SetFPS(int ifps = 60);
    bool Wait(MSG* iptrmsg);

    //実測ＦＰＳを返す
    // Return the measured FPS
    int GetFPS() { return MFPS; }

    //ウェイトの間に、何回、Sleep(1)したかを返す。多いほど余裕ってこと。
    // Returns how many times Sleep(1) was performed during the wait. The more you have,
    // the more you can afford.
    int GetNofSleep() { return MNOsleep; }

    //デバッグ時のみ。Func()を処理した負荷を返す？いや、プレッシャーじゃないだろ……って話だけど
    // For debugging only. Return the load processed by Func()? No, it's not
    // pressure...I mean...
    int GetPressure() { return Mpressure; }

    //↑のを、なんか時間に変換したものを返す？
    // Do you want to convert the above into some kind of time and return it?
    int GetProcessTime() {
        if (MisTGT) return Mpressure * 1000 * 10;
        return (int)(Mpressure * 1000 * 10 / MUtimervalue.SforPC.Mfreq.QuadPart);
    }

    //？何に使うのか、ウェイトの最後の時間を返す？
    //? What is it used for, returning the last time of the timer?
    DWORD GetEndTime() {
        if (MisTGT) return MUtimervalue.SforTGT.Mendtime;
        return MUtimervalue.SforPC.Mendtime.LowPart;
    }

    //それなりの精度で処理時間を測定するのに使う。
    //１回前に、この関数を呼び出してからの時間経過を秒単位で返す。
    // Used to measure processing time with a certain degree of accuracy.
    // Returns the elapsed time in seconds since the previous call to this function.
    double ProcessTimeCount();

    //フレームスキップが望まれているかを返す
    // Return whether frame skipping is desired
    bool FrameSkipIsRequired() { return Mframeskiprequired; }
};


#endif /*MYTIMER_HEADER_INCLUDED*/