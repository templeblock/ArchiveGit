////////////////////////////////////////////////////////////////////////////////////////////////
//	The spectrochart dialog																  //
//  specchartdial.h																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SPC_DIALOG_H
#define SPC_DIALOG_H


#include	<Sound.h>
#include "monacoprint.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "basewin.h"
#include "mcomem.h"
#include "spectrochart.h"
#include "tcr.h"
#include <QDOffscreen.h>
#include "profiledoc.h"
#include "rawdata.h"
#include "mcoiccmac.h"
#include "tabledialog.h"

class SpChInDialog:public TableDialog {

private:
protected:

public:

SpChInDialog(ProfileDoc *dc,RawData *pD);
~SpChInDialog(void);
McoStatus readInPatches(void);
McoStatus DoStart(void);
McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);
};

#endif