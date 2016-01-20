#ifndef XYZTORGB_H
#define XYZTORGB_H

#include "matrix.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "fileformat.h"
#include "monitor.h"
//#include "SC_Plugin.h"

class XyztoRgb{

protected:
	Monitor	_monitor;
	Matrix	*_xyztorgbt;
	Matrix  *_rgbtoxyzt;
	
public:
	XyztoRgb(void);
	~XyztoRgb(void);
	
//computing the transformation matrix from XYZ to RGB
//based on the white point and phosphor values
//NOTE! the transformation matrix is on normalized space	
	McoStatus xyz2rgb(Monitor *monitor);

//from scanned RGB and target transformed RGB to 
//estimate the Gamma of the scanner
//ATTN! not been used anymore	
	McoStatus getGamma(double*, int16 *, int16, int16, double*);

//Based on the FINAL highlight RGB value of 245	and input photoshop
//gamma to find out the appropriate scaling factor for luminance  channel
//xyz are HIGHLIGHT XYZ value, maxrgb are expected HIGHLIGHT rgb values
//gamma is gamma applied XYZ transformed rgb values 
//returned scaling factor are stored in scale
	McoStatus getLumScale(double* xyz, double maxrgb, double gamma, double* scale);
	
//apply (1) the transformation matrix and 
//(2) the gamma to the XYZ, the results are the converted RGB values	
	McoStatus compute(double* xyz, double gamma, long num);
	
	McoStatus computeInv(double* xyz, double ingamma, long num);
	
//scaling the luminance channel in Lab by scaling factor scale
//XYZ(not RGB) values are changed accordingly	
	McoStatus scaleLuminance(double* xyz, double scale);

	McoStatus eval(double* xyz, unsigned char *rgb, int32 num, double ingamma);
	McoStatus getMatrix(double *matrix);
	
	McoStatus getrgb2xyzMatrix(double *matrix);

};
	
// added by James for conversion from photoshop format
McoStatus ConvertMonitor(PhotoMonitor *pmonitor, Monitor *monitor);
	
#endif //XYZTORGB_H
