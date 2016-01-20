/*      Thermometer.h
		
		Draw a dialog with a thermometer
		
		Copyright 1993, Monaco Systems Inc.
		by James Vogh
		
*/



#ifndef IN_THERM
#define IN_THERM

#include "mcostat.h"

typedef enum {
	THERM_DLG = 0,
	THERM_BOX,
	THERM_TEXT,
	THERM_CANCEL
	} THERM_IDS;
	
#define NUM_THERM_IDS 4

typedef enum {
	THERM_SCAN = 0,
	THERM_TIFF
	} ThermType;

/* The data structure for the thermometer dialog */

class ThermDialog {
private:
protected:

DialogPtr 	dp;
GrafPtr		olddp;
Rect 		r;
DialogTHndl dt;
short		ids[NUM_THERM_IDS+4];


int			old_k,old_maxk;
Boolean 	Quit;
public:

/* Create and open dialog, also initilize all data */
/* The string is the message displayed in the dialog */
McoStatus SetUpTherm(int num);

/* display the bar, k = the current value, 
   maxk = maximum value */
Boolean DisplayTherm(int k,int maxk,unsigned char *st);

/* delete the dialog */
void  RemoveTherm(void);
};

#endif