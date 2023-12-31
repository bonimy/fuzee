#ifndef MYEINPUT_HEADER_INCLUDED
#define MYEINPUT_HEADER_INCLUDED

#include <Windows.h>

/*
        キーボード・マウス入力情報管理ルーチン。
        各フレームの最後にKeyMoveを呼び、あと、
        下のコードをウインドウプロシージャのメッセージ別switch内に入れて使う
        Keyboard/mouse input information management routine.
        Call KeyMove at the end of each frame, and then
        Place the code below in the message-specific switch of the window procedure and
        use it.
*/
/*
        case WM_KEYDOWN:
                ProOn(wparam);
        break;
        case WM_KEYUP:
                ProOff(wparam);
        break;
        case WM_MOUSEMOVE:
                ProMMove(lparam);
        break;
        case WM_LBUTTONDOWN:
                ProMOn(MB_L);
        break;
        case WM_LBUTTONUP:
                ProMOff(MB_L);
        break;
        case WM_RBUTTONDOWN:
                ProMOn(MB_R);
        break;
        case WM_RBUTTONUP:
                ProMOff(MB_R);
        break;
        case WM_MBUTTONDOWN:
                ProMOn(MB_C);
        break;
        case WM_MBUTTONUP:
                ProMOff(MB_C);
        break;
*/

/*
        マウスボタンのインデックス
        mouse button index
*/
enum mousebuttonidentifyindex { MB_L = 0, MB_R = 1, MB_C = 2 };

/*
        使う前に、KeyStartを呼んでも良い……
        というか、勝手に呼ばれるので呼ばなくて良い
        You can call KeyStart before using it...
        However, you don't have to call it because it will be called automatically.
*/
extern bool KeyStart();
// extern void KeyEnd();

/*
        １フレームの最後にこれを呼ぶ
        Call this at the end of one frame
*/
extern void KeyMove();


/*
        On......押してある
        Off.....離されている
        Push....押された瞬間
        Release.離された瞬間
        ……ならtrueを返す
        On....Pressed
        Off.....separated
        Push....the moment it was pushed
        Release.The moment you let go
        ...then return true
*/
extern bool KeyOn(int code);
extern bool KeyOff(int code);
extern bool KeyPush(int code);
extern bool KeyRelease(int code);

extern bool MouseOn(mousebuttonidentifyindex wb);
extern bool MouseOff(mousebuttonidentifyindex wb);
extern bool MousePush(mousebuttonidentifyindex wb);
extern bool MouseRelease(mousebuttonidentifyindex wb);

/*
        マウスの位置を取得する
        Get mouse position
*/
extern int GetMousePosX();
extern int GetMousePosY();

/*
        数字キーが押された（瞬間）なら、その番号を返す。
        押されていなければ-1を返す。
        isHEXenableをtrueにすれば、A～Fも対象になる。
        複数同時に押されていた場合は、若い方が優先される。
        If a number key is pressed (instantly), return that number.
        Returns -1 if not pressed.
        If isHEXenable is set to true, A to F will also be targeted.
        If more than one button is pressed at the same time, the younger one takes
        priority.
*/
extern int GetNumberKey(bool isHEXenable = false);

/*
        強制的にキーやマウスボタンを離したことにする。
        ウインドウプロシージャにKEYUP等のメッセージが行かなかったことによる押しっぱなし防止。
        Forcibly releases a key or mouse button.
        Prevents long presses due to messages such as KEYUP not going to the window
        procedure.
*/
extern void ForceReleaseKey();
extern void ForceReleaseMouse();
/*
        呼び出すと押されていない扱いにすることができる
        When called, it can be treated as not being pressed.
*/
extern void InvalidateKey();
extern void InvalidateMouse();
extern void InvalidateKeyCode(int code);
extern bool IsValidKeyCode(int code);
/*
        ウインドウプロシージャ内で呼ぶ
        Call within a window procedure
*/
extern void ProOn(int wparam);
extern void ProOff(int wparam);
extern void ProMOn(mousebuttonidentifyindex wb);
extern void ProMOff(mousebuttonidentifyindex wb);
extern void ProMMove(LONG lparam);

#endif /*MYEINPUT_HEADER_INCLUDED*/
