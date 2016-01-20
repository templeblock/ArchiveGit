#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include "mdialog.h"

//  Define About_Dialog item IDs                                               

#define About_Dialog		1799

//  Define About_Dialog_Ditl item IDs                                               

#define	About_Ok			1
 

//design of this class is close related to the event loop
//so interface must be compatible with event interface.
//user should have minimal effect to understand and use this class.
//virtual functions are really intended for user to subclassing.

class AboutDialog:public MDialog{

private:
protected:

public:
	AboutDialog(void);
	
	//abstract function, should be filled by subclass
	//hit means mouse select contents area
	virtual int16	hit(void);
};

#endif //