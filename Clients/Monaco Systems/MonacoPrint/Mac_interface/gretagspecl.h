////////////////////////////////////////////////////////////////////////////////////////////////
//	the gretag spectrolino dialog															  //
//  gretagspecl.h																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef IN_GTSL_DIAL
#define IN_GTSL_DIAL


#include "inputdialog.h"
#include "gretag.h"


class GTSpecLinDialog:public InputDialog {
private:
protected:
GretagCom	*tcom;	//gretag communications
public:
GTSpecLinDialog(ProfileDoc *dc,RawData *pD);
~GTSpecLinDialog(void);

McoStatus SetSheetStripText(void);
McoStatus readInPatches(void);
McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);


};

#endif