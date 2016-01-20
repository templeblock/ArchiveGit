#ifndef PRINT_DIALOG_H
#define PRINT_DIALOG_H

#include "basewin.h"
#include "profiledoc.h"

//Define Open_Warn_Dialog item IDs                                               
typedef enum {
	Print_Dialog = 0,	
	PrintIcon,
	ReadIcon,
	CalibrateIcon,
	TargetIcon,
	GamutIcon};
	
#define NUM_Pri_IDS 6

#define Pri_IDS {1999,8,5,6,9,7}
//  Define Open_Warn_Dialog_Ditl item IDs                                               

//design of this class is close related to the event loop
//so interface must be compatible with event interface.
//user should have minimal effect to understand and use this class.
//virtual functions are really intended for user to subclassing.

class PrintDialog:public baseWin{

private:
protected:
ProfileDoc *doc;

int	ids[NUM_Pri_IDS];

public:

int32	gammut_winNum;

PrintDialog(ProfileDoc *d,int winnum);

Boolean alwaysUpdate(void) { return TRUE;}
McoStatus updateWindowData(int changed);
McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);

//void showNum(int32 num);
};

#endif //