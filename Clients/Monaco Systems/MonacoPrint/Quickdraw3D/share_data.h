/******************************************************************/
/* share_data.h													  */
/* class that allows sharing of data and events between windows	  */
/* Feb 26, 1996												      */
/* James Vogh													  */
/******************************************************************/

#ifndef IN_SHARE_DATA
#define IN_SHARE_DATA

#include "basewin.h"

class Shared_Data {
private:
protected:
	int		num_wins;
	baseWin **wins; // a list of wins that share the data
	MS_WindowTypes	type;

public:	
	long	priority;

	Shared_Data(void);
	~Shared_Data(void);

	virtual Boolean MyType(MS_WindowTypes t) { return (t == type);}
	virtual McoStatus AddWin(baseWin *win);
	McoStatus RemoveWin(baseWin *win);
	Boolean   isMyWindow(MS_WindowTypes winType,int32 winNum); 
	Boolean   isMyWindow(baseWin *win);
	virtual McoStatus ProcessMouseDown(Point where, int code,baseWin *frontWin) {return MCO_SUCCESS;}
	virtual McoStatus DoAlwaysUpdate(baseWin *frontWin) {return MCO_SUCCESS;}
};

#endif