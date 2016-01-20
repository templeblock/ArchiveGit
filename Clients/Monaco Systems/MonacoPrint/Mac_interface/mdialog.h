#ifndef M_DIALOG_H
#define M_DIALOG_H

#include "basedialog.h"


//design of this class is close related to the event loop
//so interface must be compatible with event interface.
//user should have minimal effect to understand and use this class.
//virtual functions are really intended for user to subclassing.

class MDialog:public BaseDialog{

private:
protected:

public:
	MDialog(void);	// constructor
	~MDialog(void);	// destructor
	
	//abstract function, should be filled by subclass
	//hit means mouse select contents area
	virtual int16	hit(void);

};

#endif //M_DIALOG_H