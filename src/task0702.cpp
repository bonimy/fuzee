/*
	いろいろ破綻した、自称タスクシステムのルーチン。
*/
#include <assert.h>
#include "task0702.h"

//#define		errorset(val)			{if(firsterror==TME_NONE)firsterror=(val);}

Task0702Manager :: Task0702Manager(int iNOTCB , void (*ifirstfunc)()  , int iNOTCBValue )
{
	onerror = false;
	TaskBody = NULL;
	firstfunc = ifirstfunc;
	isfirst = true;
//	firsterror = TME_NONE;
	NOTCB = iNOTCB;
	NOTCBValue = iNOTCBValue ;
	PheadTCB = NULL;
	PtailTCB = NULL;
	draworderissameasTP = false;
	indexcounter = 1;
	RotatedIndex = 0 ;
	TFcontinueTCB = NULL ;

	if(iNOTCB < 5 || iNOTCB >= 60000 || iNOTCBValue < 3 || iNOTCBValue >= 10000 )
	{
		onerror = true;
//		errorset(TME_CONSTRUCTOR_ARGUMENT_INVALID);
	}
	else
	{
		TaskBody = new TCB[iNOTCB];
		if(!TaskBody)
		{
			onerror = true;
//			errorset(TME_OUT_OF_TCB_MEMORY);
		}
		else
		{
TCB nullTCB;
int i;
			nullTCB.kind     = TP_NONE;
			nullTCB.flag     = TCBF_DEFAULT;
			nullTCB.x        = 0;
			nullTCB.y        = 0;
			nullTCB.index    = 0;
			nullTCB.PprepTCB = NULL;
			nullTCB.PnextTCB = NULL;
			nullTCB.PCheadTCB= NULL;
			nullTCB.PCtailTCB= NULL;
			nullTCB.Pdest    = NULL;
			nullTCB.Pdraw    = NULL;
			nullTCB.PmotherTCB=NULL;
			nullTCB.Pv       = NULL;
#ifdef TM_POINTER_IN_EACH_TCB
			nullTCB.PTM = this ;
#endif
			for(i=0 ; i<NOTCB ; i++)
			{
				TaskBody[i] = nullTCB;
				TaskBody[i].Pv = new TCBvalue[ NOTCBValue ] ;
				if( !TaskBody[i].Pv )
				{
					onerror = true ;
					for( i=i-1 ; i>=0 ; i-- )
					{
						delete TaskBody[i].Pv ;
					}
					break ;
				}
#ifdef _DEBUG
				memset( TaskBody[i].Pv , 0xCC , sizeof(TCBvalue)*NOTCBValue ) ;
#endif
			}
		}
	}
}
Task0702Manager :: ~Task0702Manager()
{
	if( onerror ) return ;
	KillTaskChain(PheadTCB);
	if(TaskBody)
	{
		for( int i=0 ; i<NOTCB ; i++ )
			delete TaskBody[i].Pv ;
		delete TaskBody;
	}
}

TCB *Task0702Manager :: AddTask(TCB *iPmotherTCB , TaskP itaskp , bool (*ifunc)(TCB *) , void (*idest)(TCB *) , void (*idraw)(TCB *))
{
	if( onerror ) return NULL;
int i;
	//検索開始位置～最後　を検索
	for(i=RotatedIndex ; i<NOTCB ; i++)
	{
		if(TaskBody[i].kind == TP_NONE)break;
	}
	if( i == NOTCB )//検索開始位置～最後　に見つからなかった
	{
		for(i=0 ; i<RotatedIndex ; i++ )//最初～検索開始位置　を検索
		{
			if(TaskBody[i].kind == TP_NONE)break;
		}
		//それでも見つからなかったら帰る
		if( i==RotatedIndex ) return NULL ;
	}
	//検索開始位置を更新
	RotatedIndex = ( i + 1 ) % NOTCB ;

TCB *Ptmp;
TCB *Ptracing;
TCB **PPheadtarg;
TCB **PPtailtarg;
	Ptmp = &TaskBody[i];

	Ptmp->kind       = itaskp;
	Ptmp->flag       = TCBF_DEFAULT;
	Ptmp->x          = 0;
	Ptmp->y          = 0;
	Ptmp->index      = indexcounter ;
	Ptmp->PCheadTCB  = NULL;
	Ptmp->PCtailTCB  = NULL;
	Ptmp->PmotherTCB = iPmotherTCB;
//デバッグ時・タスク値のぐちゃぐちゃ化
#ifdef _DEBUG
{
	memset( Ptmp->Pv , 0xCC , sizeof(TCBvalue)*NOTCBValue ) ;
}
#endif

	Ptmp->Pdest      = idest;
	Ptmp->Pdraw      = idraw;
	Ptmp->Pfunc      = ifunc;

	//インデックスを進めるが、１周して０は避ける。
	//というか、１周した時点で、少し破綻するんだが。
	indexcounter++;
	if(!indexcounter)indexcounter++;

	if(iPmotherTCB)
	{
		Ptracing = iPmotherTCB->PCheadTCB;
		PPheadtarg = &iPmotherTCB->PCheadTCB;
		PPtailtarg = &iPmotherTCB->PCtailTCB;
	}
	else
	{
		Ptracing = PheadTCB;
		PPheadtarg = &PheadTCB;
		PPtailtarg = &PtailTCB;
	}


	if(!Ptracing)
	{
		Ptmp->PprepTCB = NULL;
		Ptmp->PnextTCB = NULL;
		*PPheadtarg = Ptmp;
		*PPtailtarg = Ptmp;
	}
	else
	{
TCB *PbefTCB=NULL;
		while(Ptracing)
		{
			if(Ptracing->kind >= itaskp)
			{
				if(Ptracing->PprepTCB)
				{
				//中間に追加する
					Ptmp->PprepTCB = Ptracing->PprepTCB;
					Ptmp->PnextTCB = Ptracing;
					Ptracing->PprepTCB->PnextTCB = Ptmp;
					Ptracing->PprepTCB           = Ptmp;
				}
				else
				{
				//先頭に追加する
					Ptmp->PprepTCB = NULL;
					Ptmp->PnextTCB = Ptracing;
					Ptracing->PprepTCB           = Ptmp;
					*PPheadtarg = Ptmp;
				}
				break;
			}
			PbefTCB  = Ptracing;
			Ptracing = Ptracing->PnextTCB;
		}

		//お尻に追加する
		if(!Ptracing)
		{
			assert(PbefTCB);
			assert(!PbefTCB->PnextTCB);
			Ptmp->PprepTCB    = PbefTCB;
			Ptmp->PnextTCB    = NULL;
			PbefTCB->PnextTCB = Ptmp;
			*PPtailtarg = Ptmp;
		}
	}

	//親子間描画タイミング初期化
	SetDrawOrderPandC( Ptmp , false , true ) ;
	//親子間処理タイミング初期化
	SetProcessOrderPandC( Ptmp , true , false ) ;

	//ファーストコール
	if(Ptmp->Pfunc)
	{
		Ptmp->calltiming    =  0;
		if(Ptmp->Pfunc(Ptmp))
		{
			KillTask( Ptmp ) ;
			return NULL;
		}
		Ptmp->calltiming    =  3;
	}
	return Ptmp;
}
void Task0702Manager :: KillTaskChain(TCB *Pifrom)
{
	if( onerror ) return ;
TCB *Ptmp;
	while(Pifrom)
	{
		Ptmp = Pifrom->PnextTCB;
		KillTask(Pifrom);
		Pifrom = Ptmp;
	}
}
void Task0702Manager :: KillTask(TCB *targ)
{
	if( onerror ) return ;
	KillTaskChain(targ->PCheadTCB);
	if(targ->Pdest)targ->Pdest(targ);
	targ->kind  = TP_NONE;
	targ->index = 0 ;
	if(targ->PnextTCB && targ->PprepTCB)
	{
		targ->PnextTCB->PprepTCB = targ->PprepTCB;
		targ->PprepTCB->PnextTCB = targ->PnextTCB;
	}
	else
	{
TCB **PPheadTCB;
TCB **PPtailTCB;
		if(targ->PmotherTCB)
		{
		//親持ち
			PPheadTCB = &targ->PmotherTCB->PCheadTCB;
			PPtailTCB = &targ->PmotherTCB->PCtailTCB;	
		}
		else
		{
		//ルートに……
			PPheadTCB = &PheadTCB;
			PPtailTCB = &PtailTCB;
		}
		if(!targ->PprepTCB)
		{
			if(!targ->PnextTCB)
			{
			//単体
				*PPheadTCB = NULL;
				*PPtailTCB = NULL;
			}
			else
			{
			//先頭
				*PPheadTCB = targ->PnextTCB;
				targ->PnextTCB->PprepTCB = NULL;
			}
		}
		else if(!targ->PnextTCB)
		{
		//お尻
			*PPtailTCB = targ->PprepTCB;
			targ->PprepTCB->PnextTCB = NULL;
		}

	}
}

#define ArgumentCheckAndCorrect(FROM,FOR)				;\
	if((FOR) == TP_NOVALUE)(FOR)=(FROM);				\
	if(													\
		(FROM) < TP_HIGHEST || (FOR) > TP_LOWEST      \
		|| ((FROM) > (FOR))	                          )	
		 
void Task0702Manager :: KillP(TCB *PmotherTCB , TaskP ifrom , TaskP ifor )
{
	if( onerror ) return ;
TCB *Ptmp;
TCB *Pbtmp;
	ArgumentCheckAndCorrect(ifrom,ifor)
	{
		return ;
	}
	Ptmp = PheadTCB;
	if(PmotherTCB)Ptmp = PmotherTCB->PCheadTCB;

	while(Ptmp && Ptmp->kind < ifrom)
	{
		Ptmp = Ptmp->PnextTCB;
	}
	while(Ptmp && Ptmp->kind <= ifor)
	{
		Pbtmp = Ptmp->PnextTCB;
		KillTask(Ptmp);
		Ptmp = Pbtmp;
	}
}
TCB *Task0702Manager :: SearchTask(TCB *PmotherTCB , TaskP ifrom , TaskP ifor)
{
	if( onerror ) return NULL ;
TCB *Ptmp;
	ArgumentCheckAndCorrect(ifrom,ifor)
	{
		return NULL;
	}
	Ptmp = PheadTCB;
	if(PmotherTCB)Ptmp = PmotherTCB->PCheadTCB;
	if(!Ptmp)return NULL;

	while(Ptmp && Ptmp->kind < ifrom)
	{
		Ptmp = Ptmp->PnextTCB;
	}
	if( !Ptmp )return NULL;
	if(Ptmp->kind <= ifor)return Ptmp;
	return NULL;
}


/*
　TCB関数の鎖を実行していくのだが、継続という制度？があり、これがかなり変態仕様。
　TCB関数内で、ContinueTF関数を呼んだあと、TCB関数を終えると、その他の続くTCB関数は全て無視されリターン
　で、次フレームココに来たときは、関数ネストを復元しつつ、先ほどの関数が呼ばれる。
　この状態は、DiscontinueTF関数を呼ぶまで続く。
　要は、DiscontinueTFを呼ぶまで、ウェイトを入れたとも考えられるのだが、
　それを実装するために、goto使いまくりのスパゲッティになっちゃった……

  これ使うと、例えば、攻撃ヒット時に、１フレーム落とすといったことが簡単にできるのだが、
  システムメンテナンス用タスクも実行されなくなるから、
  継続している関数が、自分で処理するなり呼ぶなりしないといけなくなる。
*/
void Task0702Manager :: DoTaskFunctionChain(TCB *Pifrom)
{
	if( onerror ) return ;
TCB *Ppre;
TCB *Pseek ;
TCB *Pmtmp ;


	if( TFcontinueTCB )
	{
		if( Pifrom->PmotherTCB == TFcontinueTCB->PmotherTCB )
		{//同じ階層に到着
			Ppre   = TFcontinueTCB->PnextTCB ;
			Pifrom = TFcontinueTCB ;
			if( TFcontinue_BC )goto PbeforeC;
			goto PafterC ;
		}
		//シーク用
		Pseek = TFcontinueTCB->PmotherTCB ;
		for(;;)
		{
			Pmtmp = Pseek ;
			Pseek = Pseek->PmotherTCB ;
			if( Pseek == Pifrom->PmotherTCB )
			{
				Ppre   = Pmtmp->PnextTCB ;
				Pifrom = Pmtmp ;
				if( Pmtmp->Pfunc )
					goto PintoCwf ;
				goto PintoC ;
			}
		}
	}
	while(Pifrom)
	{
		Ppre = Pifrom->PnextTCB;
		if( !(Pifrom->flag&(TCBF_SLEEP|TCBF_SLEEP_PROCESS)) )
		{
			if(Pifrom->Pfunc)
			{
				if((Pifrom->flag&TCBF_PROCESS_BEFORE_CHILD))
				{
PbeforeC:
					Pifrom->calltiming    =  1;
					if(Pifrom->Pfunc(Pifrom))
					{
						TFcontinueTCB = NULL ;
						KillTask(Pifrom);
						goto TASK_PROCESS_CONTINUE ;
					}
					if( TFcontinueTCB )return ;
					Pifrom->calltiming    =  3;
				}
PintoCwf:
				DoTaskFunctionChain(Pifrom->PCheadTCB);
				if( TFcontinueTCB )return ;
				if((Pifrom->flag&TCBF_PROCESS_AFTER_CHILD))
				{
PafterC:
					Pifrom->calltiming    =  2;
					if(Pifrom->Pfunc(Pifrom))
					{
						TFcontinueTCB = NULL ;
						KillTask(Pifrom);
						goto TASK_PROCESS_CONTINUE ;
					}
					if( TFcontinueTCB )return ;
					Pifrom->calltiming    =  3;
				}
			}
			else//ＴＣＢは関数を持ってない
			{
PintoC:
				//子供の関数を呼ぶ
				DoTaskFunctionChain(Pifrom->PCheadTCB);
				if( TFcontinueTCB )return ;
			}
		}
TASK_PROCESS_CONTINUE:
		Pifrom = Ppre;
	}

}
/*
NULLの扱いが全然違うので気をつける。
これは、NULLを渡すと、「全て」である
*/
void Task0702Manager :: DoDrawFunctionChain(TCB *Pifrom)
{
	if( onerror ) return ;
	if(draworderissameasTP)
	{
		if(!Pifrom)Pifrom = PheadTCB;
		while(Pifrom)
		{
			if( !(Pifrom->flag&(TCBF_SLEEP|TCBF_SLEEP_DRAW)) )
			{

				if((Pifrom->flag&TCBF_DRAW_BEFORE_CHILD) && Pifrom->Pdraw)
				{
					Pifrom->calltiming    =  1;
					Pifrom->Pdraw(Pifrom);
					Pifrom->calltiming    =  3;
				}
				if(Pifrom->PCheadTCB)DoDrawFunctionChain(Pifrom->PCheadTCB);
				if((Pifrom->flag&TCBF_DRAW_AFTER_CHILD) && Pifrom->Pdraw)
				{
					Pifrom->calltiming    =  2;
					Pifrom->Pdraw(Pifrom);
					Pifrom->calltiming    =  3;
				}
			}
			Pifrom = Pifrom->PnextTCB;
		}
	}
	else
	{
		if(!Pifrom)Pifrom = PtailTCB;
		while(Pifrom)
		{
			if( !(Pifrom->flag&(TCBF_SLEEP|TCBF_SLEEP_DRAW)) )
			{

				if((Pifrom->flag&TCBF_DRAW_BEFORE_CHILD) && Pifrom->Pdraw)
				{
					Pifrom->calltiming    =  1;
					Pifrom->Pdraw(Pifrom);
					Pifrom->calltiming    =  3;
				}
				if(Pifrom->PCtailTCB)DoDrawFunctionChain(Pifrom->PCtailTCB);

				if((Pifrom->flag&TCBF_DRAW_AFTER_CHILD) && Pifrom->Pdraw)
				{
					Pifrom->calltiming    =  2;
					Pifrom->Pdraw(Pifrom);
					Pifrom->calltiming    =  3;
				}
			}
			Pifrom = Pifrom->PprepTCB;
		}
	}
}

bool Task0702Manager :: Run()
{
	if(onerror)return false;
	if(isfirst)
	{
		isfirst = false;
		if(firstfunc)firstfunc();
	}
	DoTaskFunctionChain( PheadTCB );
	if(!PheadTCB)return false;
	return true;
}
int Task0702Manager :: CountP(TCB *PmotherTCB , TaskP ifrom , TaskP ifor)
{
	if( onerror ) return 0 ;
TCB *Ptmp;
int counter;
	ArgumentCheckAndCorrect(ifrom,ifor)
	{
		return 0 ;
	}

	Ptmp = PheadTCB;
	if(PmotherTCB)Ptmp = PmotherTCB->PCheadTCB;

	counter = 0;
	while(Ptmp && Ptmp->kind < ifrom)
	{
		Ptmp = Ptmp->PnextTCB;
	}
	while(Ptmp && Ptmp->kind <= ifor)
	{
		counter++;
		Ptmp = Ptmp->PnextTCB;
	}
	return counter;
}

