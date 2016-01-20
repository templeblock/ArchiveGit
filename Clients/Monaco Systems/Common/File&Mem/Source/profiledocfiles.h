////////////////////////////////////////////////////////////////////////////////
//	profiledocfiles.h														  //
//																			  //
//	The code for dealing with the profile data, replaces much of printdialog.c//
//	James Vogh																  //
// June 4, 1996																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef PRINT_PROFILEFILE_H
#define PRINT_PROFILEFILE_H


#include "csprofile.h"
#include "mpdata.h"

#include "calcurves.h"

#include "profiledoc.h"
#include "printertype.h"


class ProfileDocFiles:public ProfileDoc{
private:
protected:
public:

	PrinterType *printerData;
	
	PrintData	*printData;
	
	CalCurves	*calCurves;
	
	long		last_changedLinearTone;
	long 		last_changedProfile;
	

	
	ProfileDocFiles(void);
	~ProfileDocFiles(void);
	McoStatus initPrintData(void);
	
	// clear the tone and linear data for a revert
	void clearToneLinear(void);
	
	virtual McoStatus createCalCurves(void);
	
	McoStatus copyIntoPrintData(void);
	
	McoStatus copyOutOfPrintData(void);
	
	McoStatus buildInverseCuvre(Ctype *CT);
	
	McoStatus buildOutputCuvre(Ctype *CT);
	
	McoStatus buildInputCuvre(Ctype *CT);
	
	McoStatus buildPrelutCurve(Ctype *CT);
	
	McoStatus _handle_calibrate(void);

	McoStatus buildgamut(void);

	// determine if valid data is present
	// source, destination, and linear must be present
	Boolean dataPresent(void);
	
	// return true if the current calibration is valid
	int validCal(void);
	
	
	// load the linearization data from the raw data object int the printer settings
	// do not load if no data is present or if linearization data has aready been loaded

	McoStatus loadLinearData(void);
	
	
	// save the various parameters in case the user cancels the save
	virtual void SetUpForCancel(void);
	virtual void HandleCancel(void);
};


#endif
