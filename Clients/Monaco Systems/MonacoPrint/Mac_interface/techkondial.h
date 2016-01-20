////////////////////////////////////////////////////////////////////////////////////////////////
//	the gretag spectrolino dialog															  //
//  techkondial.h																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef IN_TECHKON_DIAL
#define IN_TECHKON_DIAL


#include "inputdialog.h"
#include "techkon.h"


class TechkonDialog:public InputDialog {
private:
protected:
Techkon	*tcom;	//gretag communications
public:
TechkonDialog(ProfileDoc *dc,RawData *pD);
~TechkonDialog(void);

McoStatus SetSheetStripText(void);
McoStatus readInPatches(void);
McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);


};

#endif