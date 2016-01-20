#ifndef VIEW_DATA_H
#define VIEW_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcostat.h"
#include "mcotypes.h"
#include "gammaTable.h"
#include "define.h"
#include "gamma.h"
#include "colorconvert.h"
#include "showerror.h"
#include "resource.h"
#include "devicemeasure.h"
#include "sequelmea.h"
#include "dtp92mea.h"
#include "dataconvert.h"

//#include "DTP92Master.h"

#define kMaxLineSize	36		// maximum expected line size

// Application creator (this must correspond to the BNDL
// and the development environment's setting.
#define kAppCreator	'XRD9'
#define kDocFileType	'TEXT'

// max number of colors (used for array declarations)
#define kMaxColorCnt	64

#define	VALID_PATCH_NUM	(5)

typedef struct Msetup{
	short ver;
	short gamma;
	short wx;
	short wy;
	short rx;
	short ry;
	short gx;
	short gy;
	short bx;
	short by;
} Msetup;
	
// short saveDataMonacoMonitor( YxyColorPtr inData, const short inDataCnt );
McoStatus saveDataMonacoMonitor(MonitorData *theMonitor);  // Modified 4/4/96 - JCY


typedef struct
{
	long	red;
	long	green;
	long	blue;
} ResColors, *ResColorsPtr, **ResColorsHdl;


class Viewdata{
private:
protected:

	short _show;
	
	Gamma _gamma;
	DataConvert	_dataconvert;

	//colors used for measurements
	short			colorCnt;
	RGBColor		theColor[kMaxColorCnt];
	YxyColorType	devData[kMaxColorCnt];
	
	
	short gCurrentTargetGamma;
	short gCurrentTargetWhitePoint;
	short gCurrentGamutControl;	//it's not been used anymore, stay here only for compatible
	long _monitor;

	Boolean			gStateChanged;
	Boolean			gSavedProfiles;
	
	//default port type
	port_type gDefaultPort;
	
	//from old code
	MonitorData	*gTheMonitor;
	
	// Set gamma table using previous calibration values
	Boolean	gUseCalibration;

	// Indicates whether monitor has never been calibrated
	// has been calibrated before, or has just been calibrated.
	short gCalibrationStatus;  
	GammaTable		*gTheGammaTable;

	Boolean _has_mea_data;
	Boolean _has_val_data;
	Boolean _has_new_cal;
	
	double	_mealab[3];
	double 	_validation_deltaE;
	
	Boolean _has_photo_table;
	Boolean _has_icc_table;

	long	_valid_in_num;	//# of patches for validation which is inside the gamut
	long	_valid_in_index[kMaxColorCnt];
	long	_valid_num;	//# of patches for validation
	double 	_valid_lab[3*kMaxColorCnt];
	RGBColor	_valid_color[kMaxColorCnt];	//rgb patches for validation
	
	McoStatus Yxytolab(YxyColorType *aYxyColor, LabColorType *mLabColor, long num);
	void _initvalid();
public:

	double 	_inlab[3];

	//Sequel or DTP51
	Devicetype _device_type;
	Devicemea *_devmea;

	Viewdata(void);
	virtual ~Viewdata(void);

	void setphototable(Boolean b){ _has_photo_table = b;}
	void seticctable(Boolean b){ _has_icc_table = b; }
	Boolean	getphototable(){ return _has_photo_table;}
	Boolean geticctable(){ return _has_icc_table;}

	Boolean iscalibrated();
	Boolean usecalibrate(){ return (gUseCalibration == TRUE); }
	
	McoStatus switchdevice(Devicetype type);
	Devicetype	getdevicetype(void);
	
	McoStatus initgammaTables();
	McoStatus initColorValues();

	McoStatus handleGammaChoice( short item );
	McoStatus handleWhiteChoice( short item );
	
	McoStatus handleMeasure(LabColorType *aLabColor, RGBColor *anRGBColor);
	McoStatus handleCalibrate(int result);
	McoStatus handleUseCalibrate(); 
	short getData( RGBColor *outColor, const short inMaxColors );

	McoStatus precalibrate();


	void sethasmeasure(){ _has_mea_data = TRUE; }
	void sethasvalidate(){ _has_val_data = TRUE; }
	
	long getwhitepointdata(double *Y, double *x, double *y);
	void setwhitepointdata(double Y, double x, double y);
	
	short getgamma(void){return gCurrentTargetGamma;}
	short getwhitepoint(void){return gCurrentTargetWhitePoint;}
	Boolean needsave(void);
	McoStatus labtorgb(LabColorType *aLabColor, RGBColor *anRGBColor, 
			long *outgamut, long num); 
	Boolean isingamut(LabColorType *aLabColor);	
	Boolean getchromaticity(double *r, double *g, double *b);
	
	McoStatus gamutbound(LabColorType *aLabColor);

	McoStatus setmeasuredata(long count, double *inlab);
	Boolean getmeasuredata(double *outlab, double *deltaE);
	McoStatus setvalidatedata(void);	
	Boolean getvalidatedata(double *outlab, double *deltaE);
	Boolean getcalibrate(){ if(_has_new_cal){ _has_new_cal = FALSE; return TRUE;} else return FALSE; }
	
	McoStatus openProfile();
	McoStatus saveProfile();
	McoStatus restorePrevCalibrate(); 

	McoStatus readPreference();
	McoStatus savePreference();

	McoStatus getvaliddata(double *out, long *num, double *de);
	McoStatus setvaliddata(double *in, long num, double de);
	
	Boolean iscalibrationused(){ return gUseCalibration; }
		
};	//end of VIEW_DATA_H	
	
#endif	