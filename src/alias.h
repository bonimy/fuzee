
/*
                  v1/1
#define		ALIAS_(ptr,str);	;{\
	;
               v1/2       v2/2
	INT32 &str## = ptr->Pv[].i ; \
	char *&str## = ptr->Pv[].Pc ; \
	BYTE *&str## = ptr->Pv[].PB ; \
*/


#define		ALIAS_MAIN(ptr,str);	;{\
	__int32 &str##region_pre = ptr->Pv[1].i ; \
	__int32 &str##mode       = ptr->Pv[2].i ; \
	__int32 &str##showmode   = ptr->Pv[3].i ; \
	__int32 &str##x          = ptr->x ; \
	__int32 &str##y          = ptr->y ; \
	;

#define		ALIAS_MM(ptr,str);	;{\
	__int32 &str##counter         = ptr->Pv[0].i ; \
	__int32 &str##currentbandsize = ptr->Pv[1].i ; \
	__int32 &str##currentchipsize = ptr->Pv[2].i ; \
	__int32 &str##clipshowx      = ptr->Pv[3].i ; \
	__int32 &str##clipshowy      = ptr->Pv[4].i ; \
	__int32 &str##requestculc    = ptr->Pv[5].i ; \
	;




#define		ALIAS_FRAME_END(ptr,str);	;{\
	__int32 &str##isredraw = ptr->Pv[0].i ; \
	__int32 &str##isflip = ptr->Pv[1].i ; \
	;





#define		ALIAS_END();		}





#define	SAFE_ADD(pos,kind,ALIAS,aname)		;\
	{										\
	TCB *Ptmp = TM.AddTask( pos , kind ) ;	\
	if(Ptmp)								\
	{										\
	ALIAS(Ptmp,aname)

#define SAFE_ADD_END()						ALIAS_END();}}

#define	SAFE_REF(pos,ALIAS,aname)			;\
	{										\
	TCB *Ptmp = pos ;						\
	if(Ptmp)								\
	{										\
	ALIAS(Ptmp,aname);

#define	SAFE_REF_END()						ALIAS_END();}}
