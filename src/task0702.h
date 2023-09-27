/*
	いろいろ破綻した、自称タスクシステムのルーチン。
	本当にタスクシステムなんだかよくわからない。
*/


/*
　このヘッダはtaskpriority.hをインクルードする。あらかじめ作成しておくこと。
　taskpriority.hでは、enum TaskPを定義する。
//////////////以下、taskpriority.hの雛形//////////////////////
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
/*
　上の、//////～//////間に、任意のタスク優先度を表す定数を追加する。
　TP_で始まるのがいちおう命名規則？らしく、上（値が低い）ほど優先度は高い。
*/

#ifndef TASK0702_HEADER_INCLUDED
#define TASK0702_HEADER_INCLUDED

#include <windows.h>
#include "taskpriority.h"


/*
	これを定義すると、どのTCBにも、それを抱えるTask0702Managerへのポインタが付加される
	グローバルなTask0702Managerにチマチマアクセスする汚らしさはなくなるが、メモリをいくらか食べる
	なんか、仕様が全体的に間違っている様な気がする
*/
//#define TM_POINTER_IN_EACH_TCB

//メインクラスのタグ
class Task0702Manager;

//タスクの所持する値
typedef struct TCB_tag TCB;


union TCBvalue
{
	__int32 i;
	float f;
	unsigned __int8 B;
	unsigned __int8 BA[4];
	__int32 *Pi;
	float *Pf;
	double *Pd;
	unsigned __int8 *PB;
	unsigned __int16 *PW;
	char *Pc;
	TCB *PTCB;
	void *PV;
	void (*Pfunc)();
	void (**PPfunc)();
	bool (*PTF)(TCB*);
};

//タスクコントロールブロック？構造体
struct TCB_tag
{
	TaskP kind;                 //TCBの優先順位、種類扱いも……
	unsigned __int8 calltiming; //Pfuncがどんなタイミングで呼ばれたか 0..作成時呼出 1..子より前 2..子より後 3..その他
	unsigned __int16 flag;      //フラグ
	unsigned __int32 index;     //TCBが作られるたびに違う値が振られるインデックス。
	                            //ゼロは振られないけど、一周しちゃったら……どうしよう……
	__int32 x;            //特別扱いの座標
	__int32 y;            //同じく
	TCB *PprepTCB;        //前のTCB
	TCB *PnextTCB;        //次のTCB
	TCB *PCheadTCB;       //子供の先頭
	TCB *PCtailTCB;       //子供の最後尾
	TCB *PmotherTCB;      //親
	TCBvalue *Pv;         //タスク値
	bool (*Pfunc)(TCB *); //タスク関数
	void (*Pdest)(TCB *); //デストラクタ関数
	void (*Pdraw)(TCB *); //描画関数
#ifdef TM_POINTER_IN_EACH_TCB
	Task0702Manager *PTM ; //Task0702Managerへのポインタ
#endif
//TCBにメンバを追加するときは、タスクマネージャのコンストラクタで初期化すること
};

//TCBのフラグ
#define	TCBF_SLEEP					0x0001
#define	TCBF_SLEEP_PROCESS			0x0002
#define	TCBF_SLEEP_DRAW				0x0004
#define TCBF_PROCESS_BEFORE_CHILD	0x0100
#define TCBF_PROCESS_AFTER_CHILD	0x0200
#define TCBF_DRAW_BEFORE_CHILD		0x0400
#define TCBF_DRAW_AFTER_CHILD		0x0800
#define	TCBF_DEFAULT				(TCBF_PROCESS_BEFORE_CHILD | TCBF_DRAW_AFTER_CHILD )

/*
//未実装のエラー処理＾＾
enum TaskManagerErrorList
{
	TME_NONE,
	TME_CONSTRUCTOR_ARGUMENT_INVALID,
	TME_OUT_OF_TCB_MEMORY,
};
*/


//メインのクラス
class Task0702Manager
{
private:
	int RotatedIndex ;       //TCB追加の高速化用
protected:
	TCB *TaskBody;           //ここにTCBを確保する
	void (*firstfunc)();     //初回Run時に呼ばれる
	int NOTCB;               //TCBの最大数
	bool isfirst;            //最初true初回Run時にfalse
	bool onerror;            //エラー時にtrue　そうなると何やっても応答しなくなる
	bool draworderissameasTP; //描画関数を、処理順に呼ぶか。デフォルトでは逆順に呼ぶ
	unsigned __int32 indexcounter; //TCBに振るインデックスのカウンタ。一周することを考慮されていない＾＾
	TCB *TFcontinueTCB;         //TCB継続
	bool TFcontinue_BC;      //TCB継続設定、子供より前に動いていたか
//	TaskManagerErrorList firsterror;
	int NOTCBValue; //TCB値の数
	TCB *PheadTCB;  //ルートの先頭のTCB
	TCB *PtailTCB;  //ルートの最後尾のTCB
	void KillTaskChain(TCB *Pifrom);       //タスクの鎖を一斉に削除
public:
	//コンストラクタ　TCBの最大数　初回Run時に呼ぶ関数　TCB値の数　を渡す
	Task0702Manager(int iNOTCB , void (*ifirstfunc)() = NULL , int iNOTCBValue = 20 );
	//デストラクタ　これに任せて問題ない仕様なハズ……
	~Task0702Manager();

	//iPmotherTCB上（NULLでルート上）　に、優先度itaskpのTCBを作成
	TCB *AddTask(TCB *iPmotherTCB , TaskP itaskp , 
					bool (*ifunc)(TCB *) = NULL , void (*idest)(TCB *) = NULL , void (*idraw)(TCB *) = NULL );
	//タスク削除
	void KillTask(TCB *targ);
	//PmotherTCB上（NULLでルート上）の一定範囲優先度のTCBを総削除
	void KillP(TCB *PmotherTCB , TaskP ifrom , TaskP ifor = TP_NOVALUE);

	//タスクを寝かす
	void SleepTask(TCB *dest){if(dest)dest->flag|=TCBF_SLEEP;};
	//タスクを起こす
	void WakeTask(TCB *dest){if(dest)dest->flag&=~TCBF_SLEEP;};
	//タスク処理を寝かす
	void SleepTaskProcess(TCB *dest){if(dest)dest->flag|=TCBF_SLEEP_PROCESS;};
	//タスク処理を起こす
	void WakeTaskProcess(TCB *dest){if(dest)dest->flag&=~TCBF_SLEEP_PROCESS;};
	//タスク描画を寝かす
	void SleepTaskDraw(TCB *dest){if(dest)dest->flag|=TCBF_SLEEP_DRAW;};
	//タスク描画を起こす
	void WakeTaskDraw(TCB *dest){if(dest)dest->flag&=~TCBF_SLEEP_DRAW;};
	//タスク関数継続（難解というか、変態仕様）
	void ContinueTF(TCB *dest , bool ReturnBeforeChild)
	{
		TFcontinueTCB = dest ;
		TFcontinue_BC = ReturnBeforeChild ;
	}
	//タスク関数継続解除
	void DiscontinueTF()
	{
		TFcontinueTCB = NULL ;
	}

	//PmotherTCB上（NULLでルート上）　の、優先度ifrom～iforのTCBを探す
	TCB *SearchTask(TCB *PmotherTCB , TaskP ifrom , TaskP ifor = TP_NOVALUE);

	//１フレームに一回これを呼ぶ
	bool Run();
	//PmotherTCB上（NULLでルート上）　の、優先度ifrom～iforのTCBの数を数える
	int  CountP(TCB *PmotherTCB , TaskP ifrom , TaskP ifor = TP_NOVALUE);
	//描画関数を呼ぶ
	void DoDrawFunctionChain(TCB *Pifrom=NULL);
	//描画と処理順を一致させるか、逆順にするかをセット
	void SetDrawOrder(bool sameTP=false){draworderissameasTP = sameTP;};
	//親子間で、処理をどちらが早くするかをセット
	//両方trueにすると、子供が動く前後に親が動ける
	void SetProcessOrderPandC(TCB *dest , bool ProcessBeforeChild , bool ProcessAfterChild )
	{
		if(dest)
		{ 
			dest->flag &= ~(TCBF_PROCESS_BEFORE_CHILD|TCBF_PROCESS_AFTER_CHILD) ;
			dest->flag |= (ProcessBeforeChild*TCBF_PROCESS_BEFORE_CHILD)|(ProcessAfterChild*TCBF_PROCESS_AFTER_CHILD ) ;
		}
	}
	//親子間で、描画をどちらが早くするかをセット
	void SetDrawOrderPandC(TCB *dest , bool DrawBeforeChild , bool DrawAfterChild )
	{
		if(dest)
		{ 
			dest->flag &= ~(TCBF_DRAW_BEFORE_CHILD|TCBF_DRAW_AFTER_CHILD) ;
			dest->flag |= (DrawBeforeChild*TCBF_DRAW_BEFORE_CHILD)|(DrawAfterChild*TCBF_DRAW_AFTER_CHILD ) ;
		}
	}
	//親をたどっていき、x,yを足し続けるだけ
	//たどる回数を指定したいときは、depthを与える（デフォルトでは、ルートまで）
	void GetGlobalPosition( TCB *dest , INT32 *Pox , INT32 *Poy , int depth=0 )
	{
TCB *Ptmp;
		if(!dest)return;
INT32 tx,ty;
		tx = ty = 0 ;
		for( Ptmp = dest ; Ptmp ; Ptmp=Ptmp->PmotherTCB)
		{
			tx += Ptmp->x ;
			ty += Ptmp->y ;
			depth -- ;
			if( !depth ) break;
		}
		*Pox = tx ;
		*Poy = ty ;
	}
	//タスクの処理関数の鎖を呼ぶ。子供も全て呼ぶ。あまり使わないのがオススメ……
	void DoTaskFunctionChain(TCB *Pifrom); 
	//TCB本体を取得……使い道あるかな
	TCB *GetTaskBody(){return TaskBody;};
	//先頭のTCBを取得……これも使い道あるかな
	TCB *GetHead(){return PheadTCB;};
};


#endif /* TASK0702_HEADER_INCLUDED */
