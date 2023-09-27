/*
	���낢��j�]�����A���̃^�X�N�V�X�e���̃��[�`���B
	�{���Ƀ^�X�N�V�X�e���Ȃ񂾂��悭�킩��Ȃ��B
*/


/*
�@���̃w�b�_��taskpriority.h���C���N���[�h����B���炩���ߍ쐬���Ă������ƁB
�@taskpriority.h�ł́Aenum TaskP���`����B
//////////////�ȉ��Ataskpriority.h�̐��`//////////////////////
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
//////////////�����܂�/////////////////^����A���p�ɂȂ��Ă܂�//
/*
�@��́A//////�`//////�ԂɁA�C�ӂ̃^�X�N�D��x��\���萔��ǉ�����B
�@TP_�Ŏn�܂�̂��������������K���H�炵���A��i�l���Ⴂ�j�قǗD��x�͍����B
*/

#ifndef TASK0702_HEADER_INCLUDED
#define TASK0702_HEADER_INCLUDED

#include <windows.h>
#include "taskpriority.h"


/*
	������`����ƁA�ǂ�TCB�ɂ��A����������Task0702Manager�ւ̃|�C���^���t�������
	�O���[�o����Task0702Manager�Ƀ`�}�`�}�A�N�Z�X���鉘�炵���͂Ȃ��Ȃ邪�A�������������炩�H�ׂ�
	�Ȃ񂩁A�d�l���S�̓I�ɊԈ���Ă���l�ȋC������
*/
//#define TM_POINTER_IN_EACH_TCB

//���C���N���X�̃^�O
class Task0702Manager;

//�^�X�N�̏�������l
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

//�^�X�N�R���g���[���u���b�N�H�\����
struct TCB_tag
{
	TaskP kind;                 //TCB�̗D�揇�ʁA��ވ������c�c
	unsigned __int8 calltiming; //Pfunc���ǂ�ȃ^�C�~���O�ŌĂ΂ꂽ�� 0..�쐬���ďo 1..�q���O 2..�q���� 3..���̑�
	unsigned __int16 flag;      //�t���O
	unsigned __int32 index;     //TCB������邽�тɈႤ�l���U����C���f�b�N�X�B
	                            //�[���͐U���Ȃ����ǁA��������������c�c�ǂ����悤�c�c
	__int32 x;            //���ʈ����̍��W
	__int32 y;            //������
	TCB *PprepTCB;        //�O��TCB
	TCB *PnextTCB;        //����TCB
	TCB *PCheadTCB;       //�q���̐擪
	TCB *PCtailTCB;       //�q���̍Ō��
	TCB *PmotherTCB;      //�e
	TCBvalue *Pv;         //�^�X�N�l
	bool (*Pfunc)(TCB *); //�^�X�N�֐�
	void (*Pdest)(TCB *); //�f�X�g���N�^�֐�
	void (*Pdraw)(TCB *); //�`��֐�
#ifdef TM_POINTER_IN_EACH_TCB
	Task0702Manager *PTM ; //Task0702Manager�ւ̃|�C���^
#endif
//TCB�Ƀ����o��ǉ�����Ƃ��́A�^�X�N�}�l�[�W���̃R���X�g���N�^�ŏ��������邱��
};

//TCB�̃t���O
#define	TCBF_SLEEP					0x0001
#define	TCBF_SLEEP_PROCESS			0x0002
#define	TCBF_SLEEP_DRAW				0x0004
#define TCBF_PROCESS_BEFORE_CHILD	0x0100
#define TCBF_PROCESS_AFTER_CHILD	0x0200
#define TCBF_DRAW_BEFORE_CHILD		0x0400
#define TCBF_DRAW_AFTER_CHILD		0x0800
#define	TCBF_DEFAULT				(TCBF_PROCESS_BEFORE_CHILD | TCBF_DRAW_AFTER_CHILD )

/*
//�������̃G���[�����O�O
enum TaskManagerErrorList
{
	TME_NONE,
	TME_CONSTRUCTOR_ARGUMENT_INVALID,
	TME_OUT_OF_TCB_MEMORY,
};
*/


//���C���̃N���X
class Task0702Manager
{
private:
	int RotatedIndex ;       //TCB�ǉ��̍������p
protected:
	TCB *TaskBody;           //������TCB���m�ۂ���
	void (*firstfunc)();     //����Run���ɌĂ΂��
	int NOTCB;               //TCB�̍ő吔
	bool isfirst;            //�ŏ�true����Run����false
	bool onerror;            //�G���[����true�@�����Ȃ�Ɖ�����Ă��������Ȃ��Ȃ�
	bool draworderissameasTP; //�`��֐����A�������ɌĂԂ��B�f�t�H���g�ł͋t���ɌĂ�
	unsigned __int32 indexcounter; //TCB�ɐU��C���f�b�N�X�̃J�E���^�B������邱�Ƃ��l������Ă��Ȃ��O�O
	TCB *TFcontinueTCB;         //TCB�p��
	bool TFcontinue_BC;      //TCB�p���ݒ�A�q�����O�ɓ����Ă�����
//	TaskManagerErrorList firsterror;
	int NOTCBValue; //TCB�l�̐�
	TCB *PheadTCB;  //���[�g�̐擪��TCB
	TCB *PtailTCB;  //���[�g�̍Ō����TCB
	void KillTaskChain(TCB *Pifrom);       //�^�X�N�̍�����Ăɍ폜
public:
	//�R���X�g���N�^�@TCB�̍ő吔�@����Run���ɌĂԊ֐��@TCB�l�̐��@��n��
	Task0702Manager(int iNOTCB , void (*ifirstfunc)() = NULL , int iNOTCBValue = 20 );
	//�f�X�g���N�^�@����ɔC���Ė��Ȃ��d�l�ȃn�Y�c�c
	~Task0702Manager();

	//iPmotherTCB��iNULL�Ń��[�g��j�@�ɁA�D��xitaskp��TCB���쐬
	TCB *AddTask(TCB *iPmotherTCB , TaskP itaskp , 
					bool (*ifunc)(TCB *) = NULL , void (*idest)(TCB *) = NULL , void (*idraw)(TCB *) = NULL );
	//�^�X�N�폜
	void KillTask(TCB *targ);
	//PmotherTCB��iNULL�Ń��[�g��j�̈��͈͗D��x��TCB�𑍍폜
	void KillP(TCB *PmotherTCB , TaskP ifrom , TaskP ifor = TP_NOVALUE);

	//�^�X�N��Q����
	void SleepTask(TCB *dest){if(dest)dest->flag|=TCBF_SLEEP;};
	//�^�X�N���N����
	void WakeTask(TCB *dest){if(dest)dest->flag&=~TCBF_SLEEP;};
	//�^�X�N������Q����
	void SleepTaskProcess(TCB *dest){if(dest)dest->flag|=TCBF_SLEEP_PROCESS;};
	//�^�X�N�������N����
	void WakeTaskProcess(TCB *dest){if(dest)dest->flag&=~TCBF_SLEEP_PROCESS;};
	//�^�X�N�`���Q����
	void SleepTaskDraw(TCB *dest){if(dest)dest->flag|=TCBF_SLEEP_DRAW;};
	//�^�X�N�`����N����
	void WakeTaskDraw(TCB *dest){if(dest)dest->flag&=~TCBF_SLEEP_DRAW;};
	//�^�X�N�֐��p���i����Ƃ������A�ϑԎd�l�j
	void ContinueTF(TCB *dest , bool ReturnBeforeChild)
	{
		TFcontinueTCB = dest ;
		TFcontinue_BC = ReturnBeforeChild ;
	}
	//�^�X�N�֐��p������
	void DiscontinueTF()
	{
		TFcontinueTCB = NULL ;
	}

	//PmotherTCB��iNULL�Ń��[�g��j�@�́A�D��xifrom�`ifor��TCB��T��
	TCB *SearchTask(TCB *PmotherTCB , TaskP ifrom , TaskP ifor = TP_NOVALUE);

	//�P�t���[���Ɉ�񂱂���Ă�
	bool Run();
	//PmotherTCB��iNULL�Ń��[�g��j�@�́A�D��xifrom�`ifor��TCB�̐��𐔂���
	int  CountP(TCB *PmotherTCB , TaskP ifrom , TaskP ifor = TP_NOVALUE);
	//�`��֐����Ă�
	void DoDrawFunctionChain(TCB *Pifrom=NULL);
	//�`��Ə���������v�����邩�A�t���ɂ��邩���Z�b�g
	void SetDrawOrder(bool sameTP=false){draworderissameasTP = sameTP;};
	//�e�q�ԂŁA�������ǂ��炪�������邩���Z�b�g
	//����true�ɂ���ƁA�q���������O��ɐe��������
	void SetProcessOrderPandC(TCB *dest , bool ProcessBeforeChild , bool ProcessAfterChild )
	{
		if(dest)
		{ 
			dest->flag &= ~(TCBF_PROCESS_BEFORE_CHILD|TCBF_PROCESS_AFTER_CHILD) ;
			dest->flag |= (ProcessBeforeChild*TCBF_PROCESS_BEFORE_CHILD)|(ProcessAfterChild*TCBF_PROCESS_AFTER_CHILD ) ;
		}
	}
	//�e�q�ԂŁA�`����ǂ��炪�������邩���Z�b�g
	void SetDrawOrderPandC(TCB *dest , bool DrawBeforeChild , bool DrawAfterChild )
	{
		if(dest)
		{ 
			dest->flag &= ~(TCBF_DRAW_BEFORE_CHILD|TCBF_DRAW_AFTER_CHILD) ;
			dest->flag |= (DrawBeforeChild*TCBF_DRAW_BEFORE_CHILD)|(DrawAfterChild*TCBF_DRAW_AFTER_CHILD ) ;
		}
	}
	//�e�����ǂ��Ă����Ax,y�𑫂������邾��
	//���ǂ�񐔂��w�肵�����Ƃ��́Adepth��^����i�f�t�H���g�ł́A���[�g�܂Łj
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
	//�^�X�N�̏����֐��̍����ĂԁB�q�����S�ČĂԁB���܂�g��Ȃ��̂��I�X�X���c�c
	void DoTaskFunctionChain(TCB *Pifrom); 
	//TCB�{�̂��擾�c�c�g�������邩��
	TCB *GetTaskBody(){return TaskBody;};
	//�擪��TCB���擾�c�c������g�������邩��
	TCB *GetHead(){return PheadTCB;};
};


#endif /* TASK0702_HEADER_INCLUDED */
