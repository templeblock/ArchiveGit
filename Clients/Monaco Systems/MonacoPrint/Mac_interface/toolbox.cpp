//implementation of toolbox class
#include "toolbox.h"


#define REMOVE_EVENTS 0

ToolBox::ToolBox(void)
{
	short i;
	/* if you have stack requirements that differ from the default, 
		then you counld use SetApplLimit to increase StackSpace at
		this point, before calling MaxApplZone */
		
	MaxApplZone();
	for(i = 0; i < 50; i++)
		MoreMasters();

	//notice the following order is important
	InitGraf(&(qd.thePort));
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0L);
	InitCursor();
	FlushEvents( everyEvent, REMOVE_EVENTS);
}

ToolBox::~ToolBox(void)
{
;
}
	
