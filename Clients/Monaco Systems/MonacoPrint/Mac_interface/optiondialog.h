///////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1996 Monaco Systems Inc.
//
//	optiondialog.h
//
//
//	Create Date:	6/23/96 by James
//
//	Revision History:
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef IN_OPTIONDIALOG
#define IN_OPTIONDIALOG

#include "basewin.h"
#include "profiledoc.h"



class OptionDialog:public baseWin{

private:
protected:
public:

ProfileDoc *doc;

RefConType		*refCon1,*refCon2,*refCon3,*refCon4,*refCon5,*refCon6,*refCon7;

OptionDialog(ProfileDoc *dc);
~OptionDialog(void);

McoStatus setBarText(void);
McoStatus updateWindowData(int changed);

McoStatus copyOutOfDial(void);
McoStatus CopyIntoDial(void);

McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);
};

#endif