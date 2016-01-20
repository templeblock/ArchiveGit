#ifndef IN_DTP51_DIAL
#define IN_DTP51_DIAL


#include "inputdialog.h"
#include "xritecom.h"


class DTP51Dialog:public InputDialog {
private:
protected:
public:
DTP51Dialog(ProfileDoc *dc,RawData *pD);
~DTP51Dialog(void);

int32 BlackPatch(int32 i);
McoStatus SetSheetStripText(void);
McoStatus readInPatches(void);
McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);


};

#endif