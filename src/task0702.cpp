/*
	���낢��j�]�����A���̃^�X�N�V�X�e���̃��[�`���B
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
	//�����J�n�ʒu�`�Ō�@������
	for(i=RotatedIndex ; i<NOTCB ; i++)
	{
		if(TaskBody[i].kind == TP_NONE)break;
	}
	if( i == NOTCB )//�����J�n�ʒu�`�Ō�@�Ɍ�����Ȃ�����
	{
		for(i=0 ; i<RotatedIndex ; i++ )//�ŏ��`�����J�n�ʒu�@������
		{
			if(TaskBody[i].kind == TP_NONE)break;
		}
		//����ł�������Ȃ�������A��
		if( i==RotatedIndex ) return NULL ;
	}
	//�����J�n�ʒu���X�V
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
//�f�o�b�O���E�^�X�N�l�̂����Ⴎ���ቻ
#ifdef _DEBUG
{
	memset( Ptmp->Pv , 0xCC , sizeof(TCBvalue)*NOTCBValue ) ;
}
#endif

	Ptmp->Pdest      = idest;
	Ptmp->Pdraw      = idraw;
	Ptmp->Pfunc      = ifunc;

	//�C���f�b�N�X��i�߂邪�A�P�����ĂO�͔�����B
	//�Ƃ������A�P���������_�ŁA�����j�]����񂾂��B
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
				//���Ԃɒǉ�����
					Ptmp->PprepTCB = Ptracing->PprepTCB;
					Ptmp->PnextTCB = Ptracing;
					Ptracing->PprepTCB->PnextTCB = Ptmp;
					Ptracing->PprepTCB           = Ptmp;
				}
				else
				{
				//�擪�ɒǉ�����
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

		//���K�ɒǉ�����
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

	//�e�q�ԕ`��^�C�~���O������
	SetDrawOrderPandC( Ptmp , false , true ) ;
	//�e�q�ԏ����^�C�~���O������
	SetProcessOrderPandC( Ptmp , true , false ) ;

	//�t�@�[�X�g�R�[��
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
		//�e����
			PPheadTCB = &targ->PmotherTCB->PCheadTCB;
			PPtailTCB = &targ->PmotherTCB->PCtailTCB;	
		}
		else
		{
		//���[�g�Ɂc�c
			PPheadTCB = &PheadTCB;
			PPtailTCB = &PtailTCB;
		}
		if(!targ->PprepTCB)
		{
			if(!targ->PnextTCB)
			{
			//�P��
				*PPheadTCB = NULL;
				*PPtailTCB = NULL;
			}
			else
			{
			//�擪
				*PPheadTCB = targ->PnextTCB;
				targ->PnextTCB->PprepTCB = NULL;
			}
		}
		else if(!targ->PnextTCB)
		{
		//���K
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
�@TCB�֐��̍������s���Ă����̂����A�p���Ƃ������x�H������A���ꂪ���Ȃ�ϑԎd�l�B
�@TCB�֐����ŁAContinueTF�֐����Ă񂾂��ƁATCB�֐����I����ƁA���̑��̑���TCB�֐��͑S�Ė������ꃊ�^�[��
�@�ŁA���t���[���R�R�ɗ����Ƃ��́A�֐��l�X�g�𕜌����A��قǂ̊֐����Ă΂��B
�@���̏�Ԃ́ADiscontinueTF�֐����ĂԂ܂ő����B
�@�v�́ADiscontinueTF���ĂԂ܂ŁA�E�F�C�g����ꂽ�Ƃ��l������̂����A
�@������������邽�߂ɁAgoto�g���܂���̃X�p�Q�b�e�B�ɂȂ���������c�c

  ����g���ƁA�Ⴆ�΁A�U���q�b�g���ɁA�P�t���[�����Ƃ��Ƃ��������Ƃ��ȒP�ɂł���̂����A
  �V�X�e�������e�i���X�p�^�X�N�����s����Ȃ��Ȃ邩��A
  �p�����Ă���֐����A�����ŏ�������Ȃ�ĂԂȂ肵�Ȃ��Ƃ����Ȃ��Ȃ�B
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
		{//�����K�w�ɓ���
			Ppre   = TFcontinueTCB->PnextTCB ;
			Pifrom = TFcontinueTCB ;
			if( TFcontinue_BC )goto PbeforeC;
			goto PafterC ;
		}
		//�V�[�N�p
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
			else//�s�b�a�͊֐��������ĂȂ�
			{
PintoC:
				//�q���̊֐����Ă�
				DoTaskFunctionChain(Pifrom->PCheadTCB);
				if( TFcontinueTCB )return ;
			}
		}
TASK_PROCESS_CONTINUE:
		Pifrom = Ppre;
	}

}
/*
NULL�̈������S�R�Ⴄ�̂ŋC������B
����́ANULL��n���ƁA�u�S�āv�ł���
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

