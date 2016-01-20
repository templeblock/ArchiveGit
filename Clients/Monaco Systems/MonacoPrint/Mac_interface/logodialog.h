#ifndef LOGO_DIALOG_H
#define LOGO_DIALOG_H

#include "mdialog.h"

//  Define Logo_Dialog item IDs                                               

#define Logo_Dialog		1805


//design of this class is close related to the event loop
//so interface must be compatible with event interface.
//user should have minimal effect to understand and use this class.
//virtual functions are really intended for user to subclassing.

class LogoDialog:public MDialog{

private:
protected:
	Str255 message_str;
	
public:
	LogoDialog(void);

	virtual int16	hit(void);
	
	void setMessage(StringPtr);

};

#endif //LOGO_DIALOG_H