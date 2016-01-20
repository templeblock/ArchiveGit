#pragma once
//////////////////////////////////////////////////////////////////////////////////////////
//
// calcurves.h
//
// build tone and linerization curves given previous curves and new curves
//
// Sept 26, 1997
// James Vogh
//////////////////////////////////////////////////////////////////////////////////////////

#include "mpdata.h"
#include "mcostat.h"

class CalCurves {
private:
protected:

	unsigned long	numBits;
	
	Ctype	*A2B0;	// the icc data
	
	// the 'new' data, data that has been entered into MonacoPrint
	
	unsigned long					*numLinearHand;
	double							*linearX;
	double							*linearY;	// in density format (not %dot)
	unsigned long					*numToneHand;
	double							*toneX;
	double							*toneY;
	
	// the backup data, from the original profile
	
	unsigned long	numlinear;		//can not be larger than 6
	unsigned long	sizelinear;
	double			**linear;
	unsigned long	numtone;	//can not be larger than 6
	unsigned long 	sizetone;
	double			**tone;
	
	char			*linearname,*tonename;
	
	unsigned long	*linearDate;
	
	void _get18PerXY(double *x, double *y,int num);

	McoStatus _build18PerCurve(double *curve);
	McoStatus _build18PerCurve(double *curve,int n);
	void _convertPercentDot(double *in, double *out,int n);
	void _convertPercent(double *in, double *out,int n);
	void _convertPercentDotToDensity(double *in, double *out,double solid,int n);


	
	
public:	
	
	CalCurves(PrintData *pd);
	
	McoStatus buildToneCurve(void);
	McoStatus cleanReversals(double *curve, double *values,int32 *size);
	// restore the linear data because it has been deleted
	McoStatus restoreLinear(void);
	// check to make sure that either all linear data is present or alll is not present
	short checkLinearData(void);
	McoStatus buildLinearCurve(int monaco);
	McoStatus backupLuts(void);
	// create a backup of the luts if the icc is not a Monaco file
	McoStatus copyBackUps(void);
	McoStatus createBackupLuts(void);
	McoStatus importTone(char *fname);
	McoStatus exportTone(char *fname);
	McoStatus importLinear(char *fname);
	McoStatus exportLinear(char *fname);
	
	virtual McoStatus importTone(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus exportTone(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus importLinear(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus exportLinear(void) {return MCO_UNSUPPORTED_OPERATION;}
	
	// return 1 if calibration is valid
	int	validCalib(void);

};