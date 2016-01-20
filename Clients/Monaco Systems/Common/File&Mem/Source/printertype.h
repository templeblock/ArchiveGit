////////////////////////////////////////////////////////////////////////////////////
// describes setting for a particular printer and loads in and saves data		  //
// printertype.h																  //
// James Vogh																	  //
// (c) Monaco Systems Inc.														  //
// 6/27/97																		  //
////////////////////////////////////////////////////////////////////////////////////

#ifndef IN_PRINTER_TYPE
#define IN_PRINTER_TYPE

#define PRINTERTYPE_MAGICNUM 4534
#define PRINTERTYPE_VERSION 2

#include "condata.h"
#include "tweak_patch.h"

#define MAX_NUM_TWEAKS 1000

class PrinterType {
private:
protected:
public:

ConData	data;

int		num_hands;
double  x_hands[31];
double	y_hands[31];

int		sim_num;

long	num_tweaks;
Tweak_Element	**tweaks;

 PrinterType(void);
~PrinterType(void);

//save to input black table when window is closed
McoStatus	setblack(McoHandle blacktableH);

#ifndef _WIN32
McoStatus Save(void);
McoStatus Save(FSSpec *fspec);
McoStatus Load(FSSpec *fspec);
#endif

};

McoStatus copyprinterType(PrinterType *p1, PrinterType *p2);


#endif