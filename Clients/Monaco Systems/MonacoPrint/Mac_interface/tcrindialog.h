////////////////////////////////////////////////////////////////////////////////////////////////
//	The new xrite input dialog																  //
//  tcrindialog.h																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TCR_DIALOG_H
#define TCR_DIALOG_H


#include	<Sound.h>
#include "monacoprint.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "basewin.h"
#include "mcomem.h"
#include "techkon.h"
#include "tcr.h"
#include <QDOffscreen.h>
#include "profiledoc.h"
#include "rawdata.h"
#include "mcoiccmac.h"
#include "tabledialog.h"

class TCRInDialog:public TableDialog {

private:
protected:

TCR	*tcom;	//device communications
public:

TCRInDialog(ProfileDoc *dc,RawData *pD);
~TCRInDialog(void);
McoStatus readInPatches(void);
McoStatus DoStart(void);
McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);
};

#endif