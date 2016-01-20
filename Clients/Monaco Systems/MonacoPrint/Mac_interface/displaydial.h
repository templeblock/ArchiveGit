#ifndef IN_DISPLAY_DIAL
#define IN_DISPLAY_DIAL


#include "inputdialog.h"
#include "xritecom.h"


class DisplayDialog:public InputDialog {
private:
protected:
XriteCom	*tcom;	//Xrite communications
public:
DisplayDialog(ProfileDoc *dc,RawData *pD,int n);


McoStatus SetSheetStripText(void);
Boolean TimerEventPresent(void);
McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);

};

#endif