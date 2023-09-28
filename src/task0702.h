/*
        いろいろ破綻した、自称タスクシステムのルーチン。
        本当にタスクシステムなんだかよくわからない。
        A self-proclaimed task system routine that has broken down in various ways.
        I don't really understand what the task system is.
*/


/*
　このヘッダはtaskpriority.hをインクルードする。あらかじめ作成しておくこと。
　taskpriority.hでは、enum TaskPを定義する。
  This header includes taskpriority.h. Create it in advance.
  In taskpriority.h, enum TaskP is defined.
//////////////以下、taskpriority.hの雛形//////////////////////
///////Below is the template of taskpriority.h///////////////
#ifndef TASKPRIORITY_HEADER_INCLUDED
#define TASKPRIORITY_HEADER_INCLUDED
enum TaskP
{
        TP_NONE=-3,
        TP_NOVALUE=-2,
        TP_HIGHEST=-1,
////////////////////////

////////////////////////
        TP_LOWEST=0x7FFF,
};
#endif /* TASKPRIORITY_HEADER_INCLUDED */
//////////////ここまで/////////////////^これ、兼用になってます//
//////////////So far//////////////////^This is dual-purpose//
/*
　上の、//////～//////間に、任意のタスク優先度を表す定数を追加する。
　TP_で始まるのがいちおう命名規則？らしく、上（値が低い）ほど優先度は高い。
  Add a constant representing any task priority between ////// and ////// above.
  Is it a naming convention to start with TP_? The higher the value (the lower the
  value), the higher the priority.
*/

#ifndef TASK0702_HEADER_INCLUDED
#define TASK0702_HEADER_INCLUDED

#include <windows.h>

#include "taskpriority.h"


/*
        これを定義すると、どのTCBにも、それを抱えるTask0702Managerへのポインタが付加される
        グローバルなTask0702Managerにチマチマアクセスする汚らしさはなくなるが、メモリをいくらか食べる
        なんか、仕様が全体的に間違っている様な気がする
        When this is defined, a pointer to the Task0702Manager that holds it will be
        attached to every TCB. Eliminates the messiness of accessing the global
        Task0702Manager every now and then, but it does eat up some memory. It seems
        like the specs are completely wrong.
*/
//#define TM_POINTER_IN_EACH_TCB

//メインクラスのタグ
// main class tag
class Task0702Manager;

//タスクの所持する値
// Value owned by the task
typedef struct TCB_tag TCB;


union TCBvalue {
    __int32 i;
    float f;
    unsigned __int8 B;
    unsigned __int8 BA[4];
    __int32* Pi;
    float* Pf;
    double* Pd;
    unsigned __int8* PB;
    unsigned __int16* PW;
    char* Pc;
    TCB* PTCB;
    void* PV;
    void (*Pfunc)();
    void (**PPfunc)();
    bool (*PTF)(TCB*);
};

//タスクコントロールブロック？構造体
// Task control block? Structure
struct TCB_tag {
    // TCBの優先順位、種類扱いも……
    // TCB priority and type handling...
    TaskP kind;
    // Pfuncがどんなタイミングで呼ばれたか
    // 0..作成時呼出 1..子より前 2..子より後 3..その他
    // At what timing Pfunc was called
    // 0..Call at creation 1..Before child 2..After child 3..Other
    unsigned __int8 calltiming;
    //フラグ
    // flag
    unsigned __int16 flag;
    // TCBが作られるたびに違う値が振られるインデックス。
    //ゼロは振られないけど、一周しちゃったら……どうしよう……
    // Index that is assigned a different value each time a TCB is created.
    // I can't get zero, but if it goes around...what should I do...
    unsigned __int32 index;
    //特別扱いの座標
    // special coordinates
    __int32 x;
    //同じく
    // Same as x
    __int32 y;
    //前のTCB
    // Previous TCB
    TCB* PprepTCB;
    //次のTCB
    // Next TCB
    TCB* PnextTCB;
    //子供の先頭
    // First child
    TCB* PCheadTCB;
    //子供の最後尾
    // Last child
    TCB* PCtailTCB;
    //親
    // Parent node
    TCB* PmotherTCB;
    //タスク値
    // task value
    TCBvalue* Pv;
    //タスク関数
    // task function
    bool (*Pfunc)(TCB*);
    //デストラクタ関数
    // Destructor function
    void (*Pdest)(TCB*);
    //描画関数
    // drawing function
    void (*Pdraw)(TCB*);

#ifdef TM_POINTER_IN_EACH_TCB
    // Task0702Managerへのポインタ
    // Pointer to Task0702Manager
    Task0702Manager* PTM;
#endif
    // TCBにメンバを追加するときは、タスクマネージャのコンストラクタで初期化すること
    // When adding members to TCB, initialize them in the task manager constructor
};

// TCBのフラグ
// TCB flags
#define TCBF_SLEEP 0x0001
#define TCBF_SLEEP_PROCESS 0x0002
#define TCBF_SLEEP_DRAW 0x0004
#define TCBF_PROCESS_BEFORE_CHILD 0x0100
#define TCBF_PROCESS_AFTER_CHILD 0x0200
#define TCBF_DRAW_BEFORE_CHILD 0x0400
#define TCBF_DRAW_AFTER_CHILD 0x0800
#define TCBF_DEFAULT (TCBF_PROCESS_BEFORE_CHILD | TCBF_DRAW_AFTER_CHILD)

/*
//未実装のエラー処理＾＾
//Unimplemented error handling ^^
enum TaskManagerErrorList
{
        TME_NONE,
        TME_CONSTRUCTOR_ARGUMENT_INVALID,
        TME_OUT_OF_TCB_MEMORY,
};
*/


//メインのクラス
// main class
class Task0702Manager {
private:
    // TCB追加の高速化用
    // For speeding up TCB addition
    int RotatedIndex;

protected:
    //ここにTCBを確保する
    // Secure TCB here
    TCB* TaskBody;
    //初回Run時に呼ばれる
    // Called at the first run
    void (*firstfunc)();
    // TCBの最大数
    // Maximum number of TCBs
    int NOTCB;
    //最初true初回Run時にfalse
    // true at first, false at first run
    bool isfirst;
    //エラー時にtrue　そうなると何やっても応答しなくなる
    // True on error If that happens, it will not respond no matter what you do
    bool onerror;
    //描画関数を、処理順に呼ぶか。デフォルトでは逆順に呼ぶ
    // Should the drawing functions be called in the order of processing? By default,
    // calls are made in reverse order.
    bool draworderissameasTP;
    // TCBに振るインデックスのカウンタ。一周することを考慮されていない＾＾
    // Counter of index to be assigned to TCB. It is not considered to go around ^^
    unsigned __int32 indexcounter;
    // TCB継続
    // TCB continuation
    TCB* TFcontinueTCB;
    // TCB継続設定、子供より前に動いていたか
    // TCB continuation setting, was it running before the child?
    bool TFcontinue_BC;

    //	TaskManagerErrorList firsterror;
    // TCB値の数
    // Number of TCB values
    int NOTCBValue;
    //ルートの先頭のTCB
    // TCB at the beginning of the route
    TCB* PheadTCB;
    //ルートの最後尾のTCB
    // TCB at the end of the route
    TCB* PtailTCB;
    //タスクの鎖を一斉に削除
    // Delete the chain of tasks all at once
    void KillTaskChain(TCB* Pifrom);

public:
    //コンストラクタ　TCBの最大数　初回Run時に呼ぶ関数　TCB値の数　を渡す
    // Constructor, maximum number of TCB, function to call at first run, pass number of
    // TCB values
    Task0702Manager(int iNOTCB, void (*ifirstfunc)() = NULL, int iNOTCBValue = 20);
    //デストラクタ　これに任せて問題ない仕様なハズ……
    // Destructor There should be no problem if you leave it to this...
    ~Task0702Manager();

    // iPmotherTCB上（NULLでルート上）　に、優先度itaskpのTCBを作成
    // Create a TCB with priority itaskp on iPmotherTCB (on the root with NULL)
    TCB* AddTask(TCB* iPmotherTCB, TaskP itaskp, bool (*ifunc)(TCB*) = NULL,
                 void (*idest)(TCB*) = NULL, void (*idraw)(TCB*) = NULL);
    //タスク削除
    // Delete task
    void KillTask(TCB* targ);
    // PmotherTCB上（NULLでルート上）の一定範囲優先度のTCBを総削除
    // Delete all TCBs with a certain range of priorities on PmotherTCB (on the root
    // with NULL)
    void KillP(TCB* PmotherTCB, TaskP ifrom, TaskP ifor = TP_NOVALUE);

    //タスクを寝かす
    // put the task to sleep
    void SleepTask(TCB* dest) {
        if (dest) dest->flag |= TCBF_SLEEP;
    };
    //タスクを起こす
    // wake up the task
    void WakeTask(TCB* dest) {
        if (dest) dest->flag &= ~TCBF_SLEEP;
    };
    //タスク処理を寝かす
    // Sleep task processing
    void SleepTaskProcess(TCB* dest) {
        if (dest) dest->flag |= TCBF_SLEEP_PROCESS;
    };
    //タスク処理を起こす
    // Start task processing
    void WakeTaskProcess(TCB* dest) {
        if (dest) dest->flag &= ~TCBF_SLEEP_PROCESS;
    };
    //タスク描画を寝かす
    // Lay down task drawing
    void SleepTaskDraw(TCB* dest) {
        if (dest) dest->flag |= TCBF_SLEEP_DRAW;
    };
    //タスク描画を起こす
    // Cause task drawing
    void WakeTaskDraw(TCB* dest) {
        if (dest) dest->flag &= ~TCBF_SLEEP_DRAW;
    };
    //タスク関数継続（難解というか、変態仕様）
    // Task function continuation (difficult or perverted specification)
    void ContinueTF(TCB* dest, bool ReturnBeforeChild) {
        TFcontinueTCB = dest;
        TFcontinue_BC = ReturnBeforeChild;
    }
    //タスク関数継続解除
    // Cancel task function continuation
    void DiscontinueTF() { TFcontinueTCB = NULL; }

    // PmotherTCB上（NULLでルート上）　の、優先度ifrom～iforのTCBを探す
    // Find the TCB with priority iffrom-ifor on PmotherTCB (on the root with NULL)
    TCB* SearchTask(TCB* PmotherTCB, TaskP ifrom, TaskP ifor = TP_NOVALUE);

    //１フレームに一回これを呼ぶ
    // Call this once per frame
    bool Run();
    // PmotherTCB上（NULLでルート上）　の、優先度ifrom～iforのTCBの数を数える
    // Count the number of TCBs with priorities iffrom to ifor on PmotherTCB (on the
    // root with NULL)
    int CountP(TCB* PmotherTCB, TaskP ifrom, TaskP ifor = TP_NOVALUE);
    //描画関数を呼ぶ
    // call drawing function
    void DoDrawFunctionChain(TCB* Pifrom = NULL);
    //描画と処理順を一致させるか、逆順にするかをセット
    // Set whether the drawing and processing order should match or be reversed
    void SetDrawOrder(bool sameTP = false) { draworderissameasTP = sameTP; };
    //親子間で、処理をどちらが早くするかをセット
    //両方trueにすると、子供が動く前後に親が動ける
    // Set which will process faster between parent and child
    // If both are set to true, the parent can move before and after the child moves
    void SetProcessOrderPandC(TCB* dest, bool ProcessBeforeChild,
                              bool ProcessAfterChild) {
        if (dest) {
            dest->flag &= ~(TCBF_PROCESS_BEFORE_CHILD | TCBF_PROCESS_AFTER_CHILD);
            dest->flag |= (ProcessBeforeChild * TCBF_PROCESS_BEFORE_CHILD) |
                          (ProcessAfterChild * TCBF_PROCESS_AFTER_CHILD);
        }
    }
    //親子間で、描画をどちらが早くするかをセット
    // Set which one draws faster between parent and child
    void SetDrawOrderPandC(TCB* dest, bool DrawBeforeChild, bool DrawAfterChild) {
        if (dest) {
            dest->flag &= ~(TCBF_DRAW_BEFORE_CHILD | TCBF_DRAW_AFTER_CHILD);
            dest->flag |= (DrawBeforeChild * TCBF_DRAW_BEFORE_CHILD) |
                          (DrawAfterChild * TCBF_DRAW_AFTER_CHILD);
        }
    }
    //親をたどっていき、x,yを足し続けるだけ
    //たどる回数を指定したいときは、depthを与える（デフォルトでは、ルートまで）
    // Just follow the parent and keep adding x,y
    // If you want to specify the number of times to follow, give depth (by default, up
    // to the root)
    void GetGlobalPosition(TCB* dest, INT32* Pox, INT32* Poy, int depth = 0) {
        TCB* Ptmp;
        if (!dest) return;
        INT32 tx, ty;
        tx = ty = 0;
        for (Ptmp = dest; Ptmp; Ptmp = Ptmp->PmotherTCB) {
            tx += Ptmp->x;
            ty += Ptmp->y;
            depth--;
            if (!depth) break;
        }
        *Pox = tx;
        *Poy = ty;
    }
    //タスクの処理関数の鎖を呼ぶ。子供も全て呼ぶ。あまり使わないのがオススメ……
    // Call the chain of task processing functions. Call all the children. I recommend
    // not using it too much...
    void DoTaskFunctionChain(TCB* Pifrom);
    // TCB本体を取得……使い道あるかな
    // Get the TCB body...I wonder if there is a use for it?
    TCB* GetTaskBody() { return TaskBody; };
    //先頭のTCBを取得……これも使い道あるかな
    // Get the first TCB...I wonder if this can be used as well?
    TCB* GetHead() { return PheadTCB; };
};


#endif /* TASK0702_HEADER_INCLUDED */
