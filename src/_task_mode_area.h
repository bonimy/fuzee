#ifndef _TASK_MODE_AREA_HEADER_INCLUDED
#define _TASK_MODE_AREA_HEADER_INCLUDED


namespace ModeArea
{
	extern int selected ;
	void DrawMachineToken( int x , int y , int type ) ;
	void DrawArea( int scx , int scy ) ;
	bool SelectArea( int scx , int scy ) ;
} ;



#endif /*_TASK_MODE_AREA_HEADER_INCLUDED*/