////////////////////////////////////////////////////////////////////////////////////////////////
//	The table input dialog																  //
//  tcrindialog.h																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TABLE_DIALOG_H
#define TABLE_DIALOG_H



#include "monacoprint.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "basewin.h"
#include "mcomem.h"
#include <QDOffscreen.h>
#include "profiledoc.h"
#include "rawdata.h"
#include "mcoiccmac.h"
#include "inputdialog.h"

class TableDialog:public InputDialog {

private:
protected:

int32		*next_patch;
int32		last_patch;

public:

TableDialog(ProfileDoc *dc,RawData *pD);

~TableDialog(void);


McoStatus readInPatches(void);

McoStatus SetSheetStripText(void);

Boolean KeyFilter(EventRecord *theEvent,short *itemHit );


};

#endif