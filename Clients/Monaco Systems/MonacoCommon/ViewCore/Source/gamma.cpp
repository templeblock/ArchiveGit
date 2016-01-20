#include <math.h>
#include "gamma.h"
#include "lookuptable.h"
#include <iostream.h>
#include <fstream.h>

#define VIRTUALLY_ZERO (0.000001)

Gamma::Gamma(void)
{
	_redTable = 0;
	_greenTable = 0;
	_blueTable = 0;
}

Gamma::~Gamma(void)
{
	if(_redTable)
	{
		delete _redTable;
		_redTable = 0;
	}
	if(_greenTable)
	{
		delete _greenTable;
		_greenTable = 0;
	}
	if(_blueTable)
	{
		delete _blueTable;
		_blueTable = 0;
	}
}

McoStatus Gamma::_Init(void)
{
	if(!_redTable)
	{
		_redTable = new LookUpTable;
		if (!(_redTable->initialize(256))) {
			return(MCO_MEM_ALLOC_ERROR);
		}
	}

	if(!_greenTable)
	{
		_greenTable = new LookUpTable;
		if (!(_greenTable->initialize(256))) {
			return(MCO_MEM_ALLOC_ERROR);
		}
	}
	
	if(!_blueTable)
	{
		_blueTable = new LookUpTable;
		if (!(_blueTable->initialize(256))) {
			return(MCO_MEM_ALLOC_ERROR);
		}
	}
	return MCO_SUCCESS;
}

void Gamma::calcProjection(XYZColorType W, XYZColorType R, XYZColorType G, 
								 XYZColorType B, RGBNumber *projection) {

	//determinant	
	double det = R.X*(G.Z*B.Y - G.Y*B.Z) + R.Y*(G.X*B.Z - G.Z*B.X) + R.Z*(G.Y*B.X - G.X*B.Y);

	if ((det-0.0)<0.000000001) {
		// Report error or something
		// den should never be zero unless the colors are linearly dependent
		// or one or more of the color has zero intensity.
		;
	}


	// Red component
	projection->red = 
		(W.X*(G.Z*B.Y - G.Y*B.Z) + W.Y*(G.X*B.Z - G.Z*B.X) + W.Z*(G.Y*B.X - G.X*B.Y)) / det;	

	// Green component
	projection->green = 
		(W.X*(R.Y*B.Z - R.Z*B.Y) + W.Y*(R.Z*B.X - R.X*B.Z) + W.Z*(R.X*B.Y - R.Y*B.X)) / det;

	// Blue component
	projection->blue = 
		(W.X*(R.Z*G.Y - R.Y*G.Z) + W.Y*(R.X*G.Z - R.Z*G.X) + W.Z*(R.Y*G.X - R.X*G.Y)) / det;

}


// Calculate the monitor's measured RGB gammas given the RGB intensity curves.

McoStatus Gamma::calcGamma(MonitorData theMonitor, RGBNumber *gamma) {

	int maxIndex, midIndex;
	double range, black; 

#ifdef DEBUG	
	ofstream outf("calcGamma.out",ios::out|ios::trunc);
#endif

	maxIndex = NUM_INTENSITY_POINTS - 1;
	midIndex = NUM_INTENSITY_POINTS/2;

	// Calculate measured gamma. 
	// Max intensity range is (Y at max saturation - Y of black)
	
	black = theMonitor.redCurve[0].intensity.Y;
	range = theMonitor.redCurve[maxIndex].intensity.Y - black;
	
	gamma->red = log((theMonitor.redCurve[midIndex].intensity.Y - black) / 
					range ) / log(theMonitor.redCurve[midIndex].index);

	black = theMonitor.greenCurve[0].intensity.Y;
	range = theMonitor.greenCurve[maxIndex].intensity.Y - black;
	
	gamma->green = log((theMonitor.greenCurve[midIndex].intensity.Y - black) / 
					range ) / log(theMonitor.greenCurve[midIndex].index);

	black = theMonitor.blueCurve[0].intensity.Y;
	range = theMonitor.blueCurve[maxIndex].intensity.Y - black;
	
	gamma->blue = log((theMonitor.blueCurve[midIndex].intensity.Y - black) / 
					range ) / log(theMonitor.blueCurve[midIndex].index);

#ifdef DEBUG		
		outf << "measured gamma.red     " << gamma->red << "\n";
		outf << "measured gamma.green " << gamma->green << "\n";
		outf << "measured gamma.blue   " << gamma->blue << "\n";
#endif

#ifdef DEBUG		
	outf.close();
#endif

	return (MCO_SUCCESS);
}

/*
McoStatus Gamma::initGamma(GammaTable *theGammaTable) {	
	if (!GammaTable::isGammaAvailable(GetMainDevice())) return(MCO_FAILURE);

	RGBNumber ratio;
	double m;
	m = MAX(INIT_MAX_RED, INIT_MAX_GREEN);
	m = MAX(m, INIT_MAX_BLUE);
	ratio.red 	= INIT_MAX_RED/m;
	ratio.green = INIT_MAX_GREEN/m;
	ratio.blue 	= INIT_MAX_BLUE/m;

	RGBNumber monitorGamma, targetGamma;
	targetGamma.red 		= INIT_TARGET_GAMMA;
	targetGamma.green 	= INIT_TARGET_GAMMA;
	targetGamma.blue 		= INIT_TARGET_GAMMA;
	monitorGamma.red 	= INIT_TARGET_GAMMA / INIT_CORRECTION_RED;
	monitorGamma.green 	= INIT_TARGET_GAMMA / INIT_CORRECTION_GREEN;
	monitorGamma.blue 	= INIT_TARGET_GAMMA / INIT_CORRECTION_BLUE;

	// Create a new gamma table and set the system table to this
	if (theGammaTable->createGammaTable(ratio, targetGamma, 
		monitorGamma, INIT_CHANNEL_COUNT)) {
		// Report error or do something similar
		return(MCO_FAILURE);
	}

	return(MCO_SUCCESS);
}
*/

// Create an appropriate gamma table to achieve the 
// target/desired white point and the target gamma.
McoStatus Gamma::ComputeGamma(MonitorData *theMonitor)
{
	int i;	

#ifdef DEBUG
	ofstream outf("adjustGamma.out",ios::out|ios::trunc);
#endif

	RGBNumber 	whiteRatio, blackOffset;
	XYZColorType	tw, r, g, b, k, maxRed, maxGreen, maxBlue, 
					minRed, minGreen, minBlue, tempXYZ;

	// correct for white point:			
	// Convert to XYZ color intensity so we can do computation.	
	convertYxyToXYZ(theMonitor->targetWhitePoint,&tw);
	convertYxyToXYZ(theMonitor->redCurve[NUM_INTENSITY_POINTS-1].intensity,&r);
	convertYxyToXYZ(theMonitor->greenCurve[NUM_INTENSITY_POINTS-1].intensity,&g);
	convertYxyToXYZ(theMonitor->blueCurve[NUM_INTENSITY_POINTS-1].intensity,&b);
	convertYxyToXYZ(theMonitor->redCurve[0].intensity,&k);
			
#ifdef DEBUG
	outf << "r.XYZ     = " << r.X << "\t" << r.Y  << "\t" << r.Z  << "\n";
	outf << "g.XYZ     = " << g.X << "\t" << g.Y  << "\t" << g.Z  << "\n";
	outf << "b.XYZ     = " << b.X << "\t" << b.Y  << "\t" << b.Z  << "\n";
	outf << "k.XYZ     = " << k.X << "\t" << k.Y  << "\t" << k.Z  << "\n\n";
#endif


	// Find the RGB basis vectors by correcting for monitor black point.
	r.X -= k.X;
	r.Y -= k.Y;
	r.Z -= k.Z;
	g.X -= k.X;
	g.Y -= k.Y;
	g.Z -= k.Z;
	b.X -= k.X;
	b.Y -= k.Y;
	b.Z -= k.Z;

#ifdef DEBUG
	outf << "corrected for monitor black point:\n";
	outf << "r.XYZ     = " << r.X << "\t" << r.Y  << "\t" << r.Z  << "\n";
	outf << "g.XYZ     = " << g.X << "\t" << g.Y  << "\t" << g.Z  << "\n";
	outf << "b.XYZ     = " << b.X << "\t" << b.Y  << "\t" << b.Z  << "\n\n";
#endif


	if (theMonitor->targetWhitePoint.Y < 0.0) {  
		// If target is neg, uncorrected monitor white point and 
		// black point are selected.
		whiteRatio.red 	= -1.0;
		whiteRatio.green 	= -1.0;
		whiteRatio.blue 	= -1.0;
		blackOffset.red 	= 0.0;
		blackOffset.green 	= 0.0;
		blackOffset.blue 	= 0.0;

#ifdef DEBUG
		outf << "whiteRatio.RGB     = " << whiteRatio.red << "\t";
		outf << whiteRatio.green << "\t";
		outf << whiteRatio.blue << "\n";
		outf << "blackOffset.RGB =" << blackOffset.red << "\t";
		outf << blackOffset.green << "\t";
		outf << blackOffset.blue << "\n";
#endif
		
	}
	else {  
		// Find decomposition of the target white point into the RGB intensity 
		// components, i.e.,  find ratio triplet (r,g,b) such that 
		// w*w.XYZ = r*(r.XYZ) + g*(g.XYZ) + b*(b.XYZ) + k.XYZ
		// Since one of r,g,b is 1.0 and w is not known, we have exactly 3
		// equations and three unknowns.  But we don't which of r,g,b is 1.0 - 
		// need 3 tries.
		tempXYZ.X = r.X + k.X;
		tempXYZ.Y = r.Y + k.Y;
		tempXYZ.Z = r.Z + k.Z;
		calcProjection(tempXYZ, tw, g, b, &whiteRatio);

#ifdef DEBUG
		outf << "calc whiteRatio: \n";
		outf << "1st try: \n";
		outf << "whiteRatio.WGB = " << whiteRatio.red << "\t";
		outf << whiteRatio.green << "\t";
		outf << whiteRatio.blue << "\n";
#endif
		
		if ((whiteRatio.green<(-1.0)) || (whiteRatio.blue<(-1.0))) {
			tempXYZ.X = g.X + k.X;
			tempXYZ.Y = g.Y + k.Y;
			tempXYZ.Z = g.Z + k.Z;
			calcProjection(tempXYZ, r, tw, b, &whiteRatio);

#ifdef DEBUG
			outf << "2nd try: \n";
			outf << "whiteRatio.RWB =" << whiteRatio.red << "\t";
			outf << whiteRatio.green << "\t";
			outf << whiteRatio.blue << "\n";
#endif
		
			if ((whiteRatio.red<(-1.0)) || (whiteRatio.blue<(-1.0))) {
				tempXYZ.X = b.X + k.X;
				tempXYZ.Y = b.Y + k.Y;
				tempXYZ.Z = b.Z + k.Z;
				calcProjection(tempXYZ, r, g, tw, &whiteRatio);

#ifdef DEBUG
				outf << "3rd try: \n";
				outf << "whiteRatio.RGW =" << whiteRatio.red << "\t";
				outf << whiteRatio.green << "\t";
				outf << whiteRatio.blue << "\n";
#endif
		
				if ((whiteRatio.red<(-1.0)) || (whiteRatio.green<(-1.0))) {
					whiteRatio.red 	= -1.0;
					whiteRatio.green 	= -1.0;
					whiteRatio.blue 	= -1.0;

#ifdef DEBUG
					outf << "couldn't find whiteRatio \n";
#endif
				}
				else whiteRatio.blue = -1.0;
			}
			else whiteRatio.green = -1.0;
		}
		else whiteRatio.red = -1.0;

		
#ifdef DEBUG
		outf << "whiteRatio.RGB     = " << whiteRatio.red << "\t";
		outf << whiteRatio.green << "\t";
		outf << whiteRatio.blue << "\n\n";
#endif

		// Correct for black point:
		// Find projection on the RGB basis that shifts the monitor black 
		// point to the target gray axis.  At least one of the RGB components
		// should be zero, which is replaced by the gray axis vector, i.e.
		// the shift is along two of the RGB basis vectors.  Since we don't 
		// know which component should be zero, we do this three times.
		// The projection blackOffset is inverted so that if the two 
		// non-zero components are negative, we have a valid projection.  
		calcProjection(k, tw, g, b, &blackOffset);

#ifdef DEBUG
		outf << "calc blackOffset: \n";
		outf << "1st try: \n";
		outf << "blackOffset.WGB = " << blackOffset.red << "\t";
		outf << blackOffset.green << "\t";
		outf << blackOffset.blue << "\n";
#endif
		
		if ((blackOffset.green>0.0) || (blackOffset.blue>0.0)) {
			calcProjection(k, r, tw, b, &blackOffset);

#ifdef DEBUG
			outf << "2nd try: \n";
			outf << "blackOffset.RWB =" << blackOffset.red << "\t";
			outf << blackOffset.green << "\t";
			outf << blackOffset.blue << "\n";
#endif
		
			if ((blackOffset.red>0.0) || (blackOffset.blue>0.0)) {
				calcProjection(k, r, g, tw, &blackOffset);

#ifdef DEBUG
				outf << "3rd try: \n";
				outf << "blackOffset.RGW =" << blackOffset.red << "\t";
				outf << blackOffset.green << "\t";
				outf << blackOffset.blue << "\n";
#endif
		
				if ((blackOffset.red>0.0) || (blackOffset.green>0.0)) {
					blackOffset.red 	= 0.0;
					blackOffset.green 	= 0.0;
					blackOffset.blue 	= 0.0;

#ifdef DEBUG
					outf << "couldn't find black offset \n";
#endif
				}
				else blackOffset.blue = 0.0;
			}
			else blackOffset.green = 0.0;
		}
		else blackOffset.red = 0.0;
	}

		
#ifdef DEBUG
		outf << "blackOffset.RGB     = " << blackOffset.red << "\t";
		outf << blackOffset.green << "\t";
		outf << blackOffset.blue << "\n\n";
#endif

	
	
	// The corrected intensity values of each channel that 
	// when summed yields the target white point. 

	maxRed.X = k.X - whiteRatio.red * r.X;
	maxRed.Y = k.Y - whiteRatio.red * r.Y;
	maxRed.Z = k.Z - whiteRatio.red * r.Z;
	maxGreen.X = k.X - whiteRatio.green * g.X;
	maxGreen.Y = k.Y - whiteRatio.green * g.Y;
	maxGreen.Z = k.Z - whiteRatio.green * g.Z;
	maxBlue.X = k.X - whiteRatio.blue * b.X;
	maxBlue.Y = k.Y - whiteRatio.blue * b.Y;
	maxBlue.Z = k.Z - whiteRatio.blue * b.Z;


#ifdef DEBUG
	outf << "\n";
	outf << "maxRed.XYZ    = " << maxRed.X << "\t";
	outf << maxRed.Y << "\t";
	outf << maxRed.Z << "\n";
	outf << "maxGreen.XYZ = " << maxGreen.X << "\t";
	outf << maxGreen.Y << "\t";
	outf << maxGreen.Z << "\n";
	outf << "maxBlue.XYZ   = " << maxBlue.X << "\t";
	outf << maxBlue.Y << "\t";
	outf << maxBlue.Z << "\n\n";
#endif


	// The corrected intensity values of each channel that 
	// when summed, yields a lowest intensity point on the 
	// target grey axis. 
	minRed.X = k.X - blackOffset.red * r.X;
	minRed.Y = k.Y - blackOffset.red * r.Y;
	minRed.Z = k.Z - blackOffset.red * r.Z;
	minGreen.X = k.X - blackOffset.green * g.X;
	minGreen.Y = k.Y - blackOffset.green * g.Y;
	minGreen.Z = k.Z - blackOffset.green * g.Z;
	minBlue.X = k.X - blackOffset.blue * b.X;
	minBlue.Y = k.Y - blackOffset.blue * b.Y;
	minBlue.Z = k.Z - blackOffset.blue * b.Z;

#ifdef DEBUG
	outf << "\n";
	outf << "minRed.XYZ    = " << minRed.X << "\t";
	outf << minRed.Y << "\t";
	outf << minRed.Z << "\n";
	outf << "minGreen.XYZ = " << minGreen.X << "\t";
	outf << minGreen.Y << "\t";
	outf << minGreen.Z << "\n";
	outf << "minBlue.XYZ   = " << minBlue.X << "\t";
	outf << minBlue.Y << "\t";
	outf << minBlue.Z << "\n";
#endif

	McoStatus status = _Init();
	if(status != MCO_SUCCESS)	return status;
	
	SparseTable redSTable[NUM_INTENSITY_POINTS];
	for (i=0; i<NUM_INTENSITY_POINTS; i++) {
		redSTable[i].index = theMonitor->redCurve[i].index;
		redSTable[i].value = theMonitor->redCurve[i].intensity.Y;
	}
	
	_redTable->constructTable(redSTable, NUM_INTENSITY_POINTS);
	
	
	SparseTable greenSTable[NUM_INTENSITY_POINTS];
	for (i=0; i<NUM_INTENSITY_POINTS; i++) {
		greenSTable[i].index = theMonitor->greenCurve[i].index;
		greenSTable[i].value = theMonitor->greenCurve[i].intensity.Y;
	}

	_greenTable->constructTable(greenSTable, NUM_INTENSITY_POINTS);
	
	SparseTable blueSTable[NUM_INTENSITY_POINTS];
	for (i=0; i<NUM_INTENSITY_POINTS; i++) {
		blueSTable[i].index = theMonitor->blueCurve[i].index;
		blueSTable[i].value = theMonitor->blueCurve[i].intensity.Y;
	}

	_blueTable->constructTable(blueSTable, NUM_INTENSITY_POINTS);
	
#ifdef DEBUG
	outf << "\n";
	outf << "LookUpTable: \n";

// take out by Peter
/*	
	for (i = 0; i < redTable->getSize(); i++) {
		double vr, vg, vb;
		redTable->lookUp(i,&vr);
		greenTable->lookUp(i,&vg);
		blueTable->lookUp(i,&vb);
		outf << "RGB[" << i << "]  = " << vr << "\t" << vg << "\t" << vb << "\t"<< "\n";
	}
*/	
#endif

	if(!ComputeTable(theMonitor, minRed.Y, maxRed.Y, _redTable, &_calGammaLut[256*0]))
		return MCO_FAILURE;

	if(!ComputeTable(theMonitor, minGreen.Y, maxGreen.Y, _greenTable, &_calGammaLut[256*1]))
		return MCO_FAILURE;

	if(!ComputeTable(theMonitor, minBlue.Y, maxBlue.Y, _blueTable, &_calGammaLut[256*2]))
		return MCO_FAILURE;

#ifdef DEBUG
	outf.close();
#endif
	
	return(MCO_SUCCESS);
}

BOOL Gamma::ComputeTable(MonitorData *theMonitor, double min, double max, LookUpTable *lut, short* table)
{
	short xIndex, lastXIndex;
 	double intensityValue, normIndex;

	lastXIndex = 0;
	for (int i=0; i<256; i++) {
		normIndex = pow(((double) i)/((double) 256), 
			theMonitor->targetGamma.red);
		intensityValue = (max - min)*normIndex + min;
		if (!(lut->inverseLookUp(intensityValue, &xIndex, lastXIndex, 1))) 
			return FALSE;   // make sure look-up is successful
		table[i] = xIndex << 8;
		
		lastXIndex = xIndex;
	}
	return TRUE;
}


short*	Gamma::GetGammaLut(int* lutSize)
{
	*lutSize = 256;
	return _calGammaLut;
}


