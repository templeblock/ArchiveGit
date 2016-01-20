#include "dataconvert.h"

DataConvert::DataConvert()
{
	_gTheMonitor = NULL;

	tw.X = 0;	tw.Y = 0;	tw.Z = 0;
	r.X  = 0;	r.Y  = 0;	r.Z  = 0;
	g.X  = 0;	g.Y  = 0;	g.Z  = 0;
	b.X  = 0;	b.Y  = 0;	b.Z  = 0;
	k.X  = 0;	k.Y  = 0;	k.Z  = 0;
	ak.X = 0;	ak.Y = 0;	ak.Z = 0;
	aw.X = 0;	aw.Y = 0;	aw.Z = 0;
	ar.X = 0;	ar.Y = 0;	ar.Z = 0;
	ag.X = 0;	ag.Y = 0;	ag.Z = 0;
	ab.X = 0;	ab.Y = 0;	ab.Z = 0;

	rgb.red = 0;
	rgb.green = 0;
	rgb.blue = 0;
	whiteRatio.red = 0;
	whiteRatio.green = 0;
	whiteRatio.blue = 0;
	blackOffset.red = 0;
	blackOffset.green = 0;
	blackOffset.blue = 0;
}

DataConvert::DataConvert(MonitorData* TheMonitor)
{
	reset(TheMonitor);
}

DataConvert::~DataConvert()
{

}

McoStatus DataConvert::reset(MonitorData* TheMonitor)
{
	XYZColorType tempXYZ;
	
	_gTheMonitor = TheMonitor;
	
	// First, calculate the monitor (target) white and black points
	// in terms of the phospher characteristics. 

	convertYxyToXYZ(_gTheMonitor->targetWhitePoint,&tw);
	convertYxyToXYZ(_gTheMonitor->redCurve[NUM_INTENSITY_POINTS-1].intensity,&r);
	convertYxyToXYZ(_gTheMonitor->greenCurve[NUM_INTENSITY_POINTS-1].intensity,&g);
	convertYxyToXYZ(_gTheMonitor->blueCurve[NUM_INTENSITY_POINTS-1].intensity,&b);
	convertYxyToXYZ(_gTheMonitor->redCurve[0].intensity,&k);

	// Phospher characteristics (RGB basis vectors) corrected for 
	// the monitor black point.
	r.X -= k.X;
	r.Y -= k.Y;
	r.Z -= k.Z;
	g.X -= k.X;
	g.Y -= k.Y;
	g.Z -= k.Z;
	b.X -= k.X;
	b.Y -= k.Y;
	b.Z -= k.Z;

	if (_gTheMonitor->targetWhitePoint.Y < 0.0) { 
		// If uncorrected white and black points option selected.
		whiteRatio.red 	= -1.0;
		whiteRatio.green 	= -1.0;
		whiteRatio.blue 	= -1.0;
		blackOffset.red 	= 0.0;
		blackOffset.green 	= 0.0;
		blackOffset.blue 	= 0.0;
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
		
		if ((whiteRatio.green<(-1.0)) || (whiteRatio.blue<(-1.0))) {
			tempXYZ.X = g.X + k.X;
			tempXYZ.Y = g.Y + k.Y;
			tempXYZ.Z = g.Z + k.Z;
			calcProjection(tempXYZ, r, tw, b, &whiteRatio);
		
			if ((whiteRatio.red<(-1.0)) || (whiteRatio.blue<(-1.0))) {
				tempXYZ.X = b.X + k.X;
				tempXYZ.Y = b.Y + k.Y;
				tempXYZ.Z = b.Z + k.Z;
				calcProjection(tempXYZ, r, g, tw, &whiteRatio);

				if ((whiteRatio.red<(-1.0)) || (whiteRatio.green<(-1.0))) {
					whiteRatio.red 	= -1.0;
					whiteRatio.green 	= -1.0;
					whiteRatio.blue 	= -1.0;

				}
				else whiteRatio.blue = -1.0;
			}
			else whiteRatio.green = -1.0;
		}
		else whiteRatio.red = -1.0;

		// Correct for black point:
		// Find decomposition of the black point into the RGB intensity components.
		
		calcProjection(k, tw, g, b, &blackOffset);
		
		if ((blackOffset.green>0.0) || (blackOffset.blue>0.0)) {
			calcProjection(k, r, tw, b, &blackOffset);
		
			if ((blackOffset.red>0.0) || (blackOffset.blue>0.0)) {
				calcProjection(k, r, g, tw, &blackOffset);

				if ((blackOffset.red>0.0) || (blackOffset.green>0.0)) {
					blackOffset.red 	= 0.0;
					blackOffset.green 	= 0.0;
					blackOffset.blue 	= 0.0;

				}
				else blackOffset.blue = 0.0;
			}
			else blackOffset.green = 0.0;
		}
		else blackOffset.red = 0.0;
	}
		
	// New black point which lies on the target gray axis.
	ak.X = k.X - blackOffset.red*r.X - blackOffset.green*g.X - blackOffset.blue*b.X;
	ak.Y = k.Y - blackOffset.red*r.Y - blackOffset.green*g.Y - blackOffset.blue*b.Y;
	ak.Z = k.Z - blackOffset.red*r.Z - blackOffset.green*g.Z - blackOffset.blue*b.Z;

	ar.X = -whiteRatio.red*r.X;
	ar.Y = -whiteRatio.red*r.Y;
	ar.Z = -whiteRatio.red*r.Z;
	ag.X = -whiteRatio.green*g.X;
	ag.Y = -whiteRatio.green*g.Y;
	ag.Z = -whiteRatio.green*g.Z;
	ab.X = -whiteRatio.blue*b.X;
	ab.Y = -whiteRatio.blue*b.Y;
	ab.Z = -whiteRatio.blue*b.Z;

	// The target white point of the calibrated monitor is just the sum 
	// of the max R, G, B components shifted by the monitor black point.
	aw.X = ar.X + ag.X + ab.X + k.X;
	aw.Y = ar.Y + ag.Y + ab.Y + k.Y;
	aw.Z = ar.Z + ag.Z + ab.Z + k.Z;
	
	return MCO_SUCCESS;
}

void DataConvert::calcProjection(XYZColorType W, XYZColorType R, XYZColorType G, 
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

McoStatus DataConvert::labtorgb(LabColorType *aLabColor, RGBColor *anRGBColor, long *outgamut, long num)
{
	long i;
	XYZColorType	anXYZColor;
	XYZColorType	tempXYZ,sr,sg,sb;	
	RGBNumber 		aRatio;

	for(i = 0; i < num; i++){
		convertLabToXYZ(*(aLabColor+i),&anXYZColor);

		anXYZColor.X *= aw.X;
		anXYZColor.Y *= aw.Y;
		anXYZColor.Z *= aw.Z;

		// Check for out of gamut color.
		tempXYZ.X = anXYZColor.X - k.X;
		tempXYZ.Y = anXYZColor.Y - k.Y;
		tempXYZ.Z = anXYZColor.Z - k.Z;
		calcProjection(tempXYZ, r, g, b, &aRatio);
		
		//check out of gammut
		if (( aRatio.red > (-whiteRatio.red + 0.001)) || 
		     ( aRatio.green > (-whiteRatio.green + 0.001)) || 
		     ( aRatio.blue > (-whiteRatio.blue + 0.001)) || 
		     ( aRatio.red < (-blackOffset.red)) || 
		     ( aRatio.green < (-blackOffset.green)) || 
		     ( aRatio.blue < (-blackOffset.blue)) 
		) {
			outgamut[i] = 1;
		}
		else
			outgamut[i] = 0;
		
		// To convert to RGB, first find the XYZ subspace the endpoints of 
		// which are the target white and black points, i.e. the space of the 
		//calibrated monitor.   sr,sg,sb are the basis vectors of the subspace.
		tempXYZ.X = anXYZColor.X - ak.X;
		tempXYZ.Y = anXYZColor.Y - ak.Y;
		tempXYZ.Z = anXYZColor.Z - ak.Z;
		sr.X = (-whiteRatio.red + blackOffset.red)*r.X;
		sr.Y = (-whiteRatio.red + blackOffset.red)*r.Y;
		sr.Z = (-whiteRatio.red + blackOffset.red)*r.Z;
		sg.X = (-whiteRatio.green + blackOffset.green)*g.X;
		sg.Y = (-whiteRatio.green + blackOffset.green)*g.Y;
		sg.Z = (-whiteRatio.green + blackOffset.green)*g.Z;
		sb.X = (-whiteRatio.blue + blackOffset.blue)*b.X;
		sb.Y = (-whiteRatio.blue + blackOffset.blue)*b.Y;
		sb.Z = (-whiteRatio.blue + blackOffset.blue)*b.Z;

		RGBNumber targetGamma;
		if (_gTheMonitor)
			targetGamma = _gTheMonitor->targetGamma;
		else
		{
			targetGamma.red   = 1.8;
			targetGamma.green = 1.8;
			targetGamma.blue  = 1.8;
		}

		convertXYZToRGB(tempXYZ, sr, sg, sb, targetGamma, &rgb);

		if (rgb.red<0.0) rgb.red = 0.0;
		if (rgb.red>1.0) rgb.red = 1.0;
		if (rgb.green<0.0) rgb.green = 0.0;
		if (rgb.green>1.0) rgb.green = 1.0;
		if (rgb.blue<0.0) rgb.blue = 0.0;
		if (rgb.blue>1.0) rgb.blue = 1.0;
		
		anRGBColor[i].red = (unsigned short) (rgb.red * 65535);
		anRGBColor[i].green = (unsigned short) (rgb.green * 65535);
		anRGBColor[i].blue = (unsigned short) (rgb.blue * 65535);
	}
	
	return MCO_SUCCESS;
}

McoStatus DataConvert::Yxytolab(YxyColorType *aYxyColor, LabColorType *mLabColor, long num)
{
	XYZColorType anXYZColor;
	
	for(long i = 0; i < num; i++){
		convertYxyToXYZ(*(aYxyColor+i),&anXYZColor);

		// Normalize by white point.
		anXYZColor.X /= aw.X;
		anXYZColor.Y /= aw.Y;
		anXYZColor.Z /= aw.Z;

		convertXYZToLab(anXYZColor,mLabColor+i);
	}
	
	return MCO_SUCCESS;
}
