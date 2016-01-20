#ifndef DEFINE_H
#define DEFINE_H

// Number of measured intensity points.  Right now I use 9 - measured at
// R,G,B = 0, 8191, 16383, 24575, 32767, 40959, 49151, 57343, 65535.
// A full RGB-intensity look-up table will be constructed by interpolating
// between these values.

#define NUM_INTENSITY_POINTS 9

typedef enum{
	NODEVICE = 1,
	SEQUEL,
	XRITE
}Devicetype;	

// max levels of RGB and gamma correction used to create 
// the initial gamma table before calibration
#define INIT_MAX_RED			(255)
#define INIT_MAX_GREEN			(255)
#define INIT_MAX_BLUE			(255)
#define INIT_TARGET_GAMMA		(1.8) 
#define INIT_CORRECTION_RED	(1.0)  // was 0.7
#define INIT_CORRECTION_GREEN	(1.0)  // was 0.7
#define INIT_CORRECTION_BLUE	(1.0)  // was 0.7
#define INIT_CHANNEL_COUNT		(3)

// Y = 0.2125*R + 0.7154*G + 0.0721*B
// This is needed to decompose black.Y into the appopriate R,G,B components.
#define STD_RED_COMPONENT		(1.0)	// 1.0 was (0.2125)
#define STD_GREEN_COMPONENT	(1.0)	// 0.95 was (0.7154)
#define STD_BLUE_COMPONENT	(1.0)	// 1.0 was (0.0721)

#define kNeverCalibrated 		1
#define kWasCalibrated			2
#define kJustBeenCalibrated		3


#define MAX(x,y) ((x)>(y)?(x):(y))
#define ABS(x) ((x)>=(0.0)?(x):(-(x)))

enum {
	dataOK = 0,
	dataNotMonotonic = 1,
	dataRangeBad = 2
};

typedef struct
{
	double	Y;
	double	x;
	double	y;
} YxyColorType, *YxyColorPtr;


// XYZ color type  
typedef struct {
	double	X;
	double	Y;
	double	Z;
} XYZColorType, *XYZColorPtr, **XYZColorHdl;

// Lab color type  
typedef struct {
	double	L;
	double	a;
	double	b;
} LabColorType, *LabColorPtr, **LabColorHdl;

typedef struct {
	double red;
	double green;
	double blue;
} RGBNumber, *RGBNumberPtr, **RGBNumberHdl;

// A simple intensity table entry.
typedef struct {
	double 			index;
	YxyColorType 	intensity;
} IntensityTable;


typedef struct {
	// This first set of values specify the desired monitor 
	// characteristics.  They are to be retrieved from a 
	// preference file.                    -JCY 03/30/96
	YxyColorType 		targetWhitePoint;
	RGBNumber 			targetGamma;

	// This set of data are the actual RGB-intensity curves of
	// the display monitor.  They are necessary for generating
	// the gamma table which enables simulating the target values.	
	IntensityTable redCurve[NUM_INTENSITY_POINTS];
	IntensityTable greenCurve[NUM_INTENSITY_POINTS];
	IntensityTable blueCurve[NUM_INTENSITY_POINTS];
} MonitorData, *MonitorDataPtr, **MonitorDataHdl;

#endif 