#include <math.h>
#include "CMICCProfile.h"
#include "viewdata.h"
#include "fileformat.h"
#include "lookuptable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>  // JCY 4/11/96
#include <fstream.h>   // JCY 4/44/96
#include "csprofile.h"
#include "think_bugs.h"
#include "colorconv.h"
#include "colorconvert.h"

#include "customwhitedialog.h"


#define PREF_FILE_DIR 		"MonacoView"
#define PREF_FILE_NAME 		"McoView1.0Pref"

Viewdata::Viewdata(void)
{
	_show = 1;
	
	gTheGammaTable = 0;
	gTheMonitor = 0;
	
	gCurrentTargetWhitePoint 	= WM_5000;	
	gCurrentTargetGamma 		= GM_18;
	gCurrentGamutControl		= CM_NORMAL;
	gCalibrationStatus = kNeverCalibrated;
	
	gStateChanged 	= FALSE;
	gSavedProfiles = TRUE;
	
	gDefaultPort = k92ModemPort;
	gUseCalibration = TRUE;
	
	_devmea = 0;
	_device_type = NODEVICE;
	
	_has_mea_data = FALSE;
	_has_val_data = FALSE;
	_has_new_cal = FALSE;
	 
	setphototable(TRUE);
	seticctable(TRUE);

	_inlab[0] = 50.0;
	_inlab[1] = 0.0;
	_inlab[2] = 0.0;
	
	_initvalid();
}

Viewdata::~Viewdata(void)
{
	if ( gTheGammaTable ) delete gTheGammaTable;
	if ( gTheMonitor ) delete  gTheMonitor;
	if (_devmea ) delete _devmea;

}

McoStatus Viewdata::switchdevice(Devicetype type)
{
	short status;
	
	if (_devmea ) {
		delete _devmea;
		_devmea = 0;
	}		

	_device_type = NODEVICE;
	
	switch(type){
		case XRITE:
			_devmea = new Dtp92mea();
			if(_devmea){
				status = _devmea->initialize(k92ModemPort);
				if(status == 0){
					_device_type = XRITE;
					return MCO_SUCCESS;
				}	
				else{
					status = _devmea->initialize(k92PrintPort);
					if(status == 0){
						_device_type = XRITE;
						return MCO_SUCCESS;
					}
				}
			}
			//showError( kDeviceErr, kStopDlg);		
			return MCO_FAILURE;
			
		case SEQUEL:
			_devmea = new Sequelmea();
			if(_devmea){
				status = _devmea->initialize(k92ModemPort);
				if(status == 0){
					_device_type = SEQUEL;
					return MCO_SUCCESS;
				}
			}
			return MCO_FAILURE;		
		
		case NODEVICE:
			return MCO_SUCCESS;
				
		default:	
			return MCO_FAILURE;
	}	
	
	return MCO_FAILURE;
}

Devicetype Viewdata::getdevicetype(void)
{
	return _device_type;
}	

// Initialize gamma tables
McoStatus Viewdata::initgammaTables(){
	// Initialize the gamma table
	if (!GammaTable::isGammaAvailable(GetMainDevice()))  {
		showError(kGammaTableErr,kStopDlg);
		return (MCO_FAILURE);
	}

	gTheGammaTable = new GammaTable;

	if (!gTheGammaTable){
		showError(kMemAllocErr,kStopDlg);
		return (MCO_MEM_ALLOC_ERROR);
	}
		
	if (gTheGammaTable->initialize(GetMainDevice()) != 0)  {
		showError(kMemAllocErr,kStopDlg);
		return (MCO_MEM_ALLOC_ERROR);
	}
	
	return(MCO_SUCCESS);
}


// ***********************************************
// Initialize global variables for handling gamma tables. 
// Add function to save/retrieve preferences for these values.
// Check these numbers a gain for accuracy.  - JCY 3/31/96
// ***********************************************
McoStatus Viewdata::initColorValues() {
	if ((gTheMonitor = new MonitorData) == nil) {
			showError(kMemAllocErr,kStopDlg);
			return (MCO_MEM_ALLOC_ERROR);
	}

	// set target gamma and white point 	
	handleGammaChoice(gCurrentTargetGamma);  		// Should've been initialized to 1.8
	handleWhiteChoice(gCurrentTargetWhitePoint);  	// Should've been initialized to D65

	McoStatus status;
	
	// read preferences from file
	if ((status = readPreference()) == MCO_SUCCESS) {
		gCalibrationStatus = kWasCalibrated;
		handleGammaChoice(gCurrentTargetGamma);  	
		handleWhiteChoice(gCurrentTargetWhitePoint); 
		if (gUseCalibration&&(gCalibrationStatus!=kNeverCalibrated)) 
			_gamma.adjustGamma(gTheMonitor, gTheGammaTable);
			
		//do the dataconvert calibration
		_dataconvert.reset(gTheMonitor);
	}
	
	return (MCO_SUCCESS);
}

Boolean Viewdata::getchromaticity(double *r, double *g, double *b)
{
	if(gCalibrationStatus == kNeverCalibrated){
		r[0] = 0.63;
		r[1] = 0.33;
		g[0] = 0.29;
		g[1] = 0.60;
		b[0] = 0.15;
		b[1] = 0.07;
	}
	else{
		r[0] = gTheMonitor->redCurve[NUM_INTENSITY_POINTS-1].intensity.x;
		r[1] = gTheMonitor->redCurve[NUM_INTENSITY_POINTS-1].intensity.y;
		g[0] = gTheMonitor->greenCurve[NUM_INTENSITY_POINTS-1].intensity.x;
		g[1] = gTheMonitor->greenCurve[NUM_INTENSITY_POINTS-1].intensity.y;
		b[0] = gTheMonitor->blueCurve[NUM_INTENSITY_POINTS-1].intensity.x;
		b[1] = gTheMonitor->blueCurve[NUM_INTENSITY_POINTS-1].intensity.y;		
	}
	
	return TRUE;
}

Boolean Viewdata::iscalibrated()
{
	return (gCalibrationStatus != kNeverCalibrated);
}
		
//----------------------------------------------------------------
//		Handle the Target Gamma Menu selection.  added 4/4/96 - JCY
//----------------------------------------------------------------
McoStatus Viewdata::handleGammaChoice( short item ) {

	switch( item ) 	{
		case GM_07:
			gTheMonitor->targetGamma.red = 0.7;
			gTheMonitor->targetGamma.green = 0.7;
			gTheMonitor->targetGamma.blue = 0.7;
			if (gCurrentTargetGamma != GM_07) {
				gCurrentTargetGamma = GM_07;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case GM_10:
			gTheMonitor->targetGamma.red = 1.0;
			gTheMonitor->targetGamma.green = 1.0;
			gTheMonitor->targetGamma.blue = 1.0;
			if (gCurrentTargetGamma != GM_10) {
				gCurrentTargetGamma = GM_10;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case GM_12:
			gTheMonitor->targetGamma.red = 1.2;
			gTheMonitor->targetGamma.green = 1.2;
			gTheMonitor->targetGamma.blue = 1.2;
			if (gCurrentTargetGamma != GM_12) {
				gCurrentTargetGamma = GM_12;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case GM_14:
			gTheMonitor->targetGamma.red = 1.4;
			gTheMonitor->targetGamma.green = 1.4;
			gTheMonitor->targetGamma.blue = 1.4;
			if (gCurrentTargetGamma != GM_14) {
				gCurrentTargetGamma = GM_14;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case GM_16:
			gTheMonitor->targetGamma.red = 1.6;
			gTheMonitor->targetGamma.green = 1.6;
			gTheMonitor->targetGamma.blue = 1.6;
			if (gCurrentTargetGamma != GM_16) {
				gCurrentTargetGamma = GM_16;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case GM_18:
			gTheMonitor->targetGamma.red = 1.8;
			gTheMonitor->targetGamma.green = 1.8;
			gTheMonitor->targetGamma.blue = 1.8;
			if (gCurrentTargetGamma != GM_18) {
				gCurrentTargetGamma = GM_18;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case GM_20:
			gTheMonitor->targetGamma.red = 2.0;
			gTheMonitor->targetGamma.green = 2.0;
			gTheMonitor->targetGamma.blue = 2.0;
			if (gCurrentTargetGamma != GM_20) {
				gCurrentTargetGamma = GM_20;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case GM_22:
			gTheMonitor->targetGamma.red = 2.2;
			gTheMonitor->targetGamma.green = 2.2;
			gTheMonitor->targetGamma.blue = 2.2;
			if (gCurrentTargetGamma != GM_22) {
				gCurrentTargetGamma = GM_22;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case GM_28:
			gTheMonitor->targetGamma.red = 2.8;
			gTheMonitor->targetGamma.green = 2.8;
			gTheMonitor->targetGamma.blue = 2.8;
			if (gCurrentTargetGamma != GM_28) {
				gCurrentTargetGamma = GM_28;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		default:
			gTheMonitor->targetGamma.red = 1.8;
			gTheMonitor->targetGamma.green = 1.8;
			gTheMonitor->targetGamma.blue = 1.8;
			if (gCurrentTargetGamma != GM_18) {
				gCurrentTargetGamma = GM_18;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
	}
	
	if( gStateChanged&&gUseCalibration&&(gCalibrationStatus!=kNeverCalibrated)){ 
		_gamma.adjustGamma(gTheMonitor, gTheGammaTable);
		_dataconvert.reset(gTheMonitor);
	}
	
	return (MCO_SUCCESS);
	
}// handleFileChoice()


//----------------------------------------------------------------
//		Handle the Target White Point Menu selection.  Added 4/4/96 - JCY
//----------------------------------------------------------------
McoStatus Viewdata::handleWhiteChoice( short item ) {

	switch( item )	{
		case WM_5000:
			gTheMonitor->targetWhitePoint.Y =  100.0;		// D50 white point
			gTheMonitor->targetWhitePoint.x =  0.3457;
			gTheMonitor->targetWhitePoint.y =  0.3585;
			if (gCurrentTargetWhitePoint != WM_5000) {
				gCurrentTargetWhitePoint = WM_5000;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case WM_5500:
			gTheMonitor->targetWhitePoint.Y =  100.0;		// 5500K white point
			gTheMonitor->targetWhitePoint.x =  0.3324;
			gTheMonitor->targetWhitePoint.y =  0.3474;
			if (gCurrentTargetWhitePoint != WM_5500) {
				gCurrentTargetWhitePoint = WM_5500;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case WM_6500:
			gTheMonitor->targetWhitePoint.Y =  100.0;		//  D65 white point
			gTheMonitor->targetWhitePoint.x =  0.3127;
			gTheMonitor->targetWhitePoint.y =  0.3290;
			if (gCurrentTargetWhitePoint != WM_6500) {
				gCurrentTargetWhitePoint = WM_6500;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;	
		case WM_7500:
			gTheMonitor->targetWhitePoint.Y =  100.0;		// 7500K white point
			gTheMonitor->targetWhitePoint.x =  0.2990;
			gTheMonitor->targetWhitePoint.y =  0.3149;
			if (gCurrentTargetWhitePoint != WM_7500) {
				gCurrentTargetWhitePoint = WM_7500;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case WM_9300:
			gTheMonitor->targetWhitePoint.Y =  100.0;		// 9300K white point	
			gTheMonitor->targetWhitePoint.x =  0.2848;
			gTheMonitor->targetWhitePoint.y =  0.2932;
			if (gCurrentTargetWhitePoint != WM_9300) {
				gCurrentTargetWhitePoint = WM_9300;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;
		case WM_MONITOR:	
			gTheMonitor->targetWhitePoint.Y =  -100.0;	// uncorrected = monitor white point	
			gTheMonitor->targetWhitePoint.x =  0.33;
			gTheMonitor->targetWhitePoint.y =  0.33;
			if (gCurrentTargetWhitePoint != WM_MONITOR) {
				gCurrentTargetWhitePoint = WM_MONITOR;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;

		case WM_CUSTOM:
			if(_show){	
				short	item;
				CustomWhitewin customwhitewin(this);
				customwhitewin.handleEvents(&item);
				
				if(item == 1){
					gCurrentTargetWhitePoint = WM_CUSTOM;
					gStateChanged = TRUE;
					gSavedProfiles = FALSE;
				}
			}
			else{
				_show = 1;
			}	
			break;
			
		default:
			gTheMonitor->targetWhitePoint.Y =  100.0;		//  D65 white point
			gTheMonitor->targetWhitePoint.x =  0.3127;
			gTheMonitor->targetWhitePoint.y =  0.3290;
			if (gCurrentTargetWhitePoint != WM_6500) {
				gCurrentTargetWhitePoint = WM_6500;
				gStateChanged = TRUE;
				gSavedProfiles = FALSE;
			}
			break;	
	}
	
	//adjustGamma();
	if( gStateChanged&&gUseCalibration&&(gCalibrationStatus!=kNeverCalibrated)){ 
		_gamma.adjustGamma(gTheMonitor, gTheGammaTable);
		_dataconvert.reset(gTheMonitor);
	}
	return (MCO_SUCCESS);	
}

//called by the measurement dialog
McoStatus Viewdata::handleCalibrate(int result) 
{
	int 			status;

	if (result) {
		MonitorData tempMonitorData;
		
		memcpy(&tempMonitorData, gTheMonitor, sizeof(MonitorData));
		
		status = _gamma.extractMonitorData(theColor, devData, colorCnt, gTheMonitor);
		if ((status & dataRangeBad) == 0) {
			if ((status & dataNotMonotonic) !=0) {
				showError(kMonotonicErr,kStopDlg);
				if (gUseCalibration&&(gCalibrationStatus!=kNeverCalibrated)) 
					_gamma.adjustGamma(gTheMonitor, gTheGammaTable);
				else gTheGammaTable->restoreOriginalTable();
				return MCO_FAILURE; 
			}	
			if (gUseCalibration) _gamma.adjustGamma(gTheMonitor, gTheGammaTable);
			else gTheGammaTable->restoreOriginalTable();
			gCalibrationStatus = kJustBeenCalibrated;
			gStateChanged = TRUE;
			gSavedProfiles = FALSE;
			_has_new_cal = TRUE;
			//redo the dataconvert after the new calibration
			_dataconvert.reset(gTheMonitor);
		}
		else {
			memcpy(gTheMonitor, &tempMonitorData, sizeof(MonitorData));
		
			showError(kRangeErr,kStopDlg);
			if (gUseCalibration&&(gCalibrationStatus!=kNeverCalibrated)) 
				_gamma.adjustGamma(gTheMonitor, gTheGammaTable);
			else gTheGammaTable->restoreOriginalTable(); 
			return (MCO_FAILURE);
		}
	}
	else {
		if (gUseCalibration&&(gCalibrationStatus!=kNeverCalibrated)) 
			_gamma.adjustGamma(gTheMonitor, gTheGammaTable);
		else gTheGammaTable->restoreOriginalTable(); // restore original gamma able
		return (MCO_CANCEL);
	}
	
	
	return (MCO_SUCCESS);
}

McoStatus Viewdata::handleUseCalibrate() 
{
	if (gUseCalibration) {
		gUseCalibration = FALSE;
		gTheGammaTable->restoreOriginalTable();
	}
	else {
		gUseCalibration = TRUE;
		if (gCalibrationStatus!=kNeverCalibrated) 
			_gamma.adjustGamma(gTheMonitor, gTheGammaTable);
	}

	return MCO_SUCCESS;
}

//restore the Previous Calibrating if the current one is canceled
McoStatus Viewdata::restorePrevCalibrate() 
{
	if (gUseCalibration) {
		if (gCalibrationStatus!=kNeverCalibrated) 
			_gamma.adjustGamma(gTheMonitor, gTheGammaTable);
	}
	else {
		gTheGammaTable->restoreOriginalTable();
	}

	return MCO_SUCCESS;
}


Boolean Viewdata::needsave()
{
	if( gSavedProfiles == FALSE && (_has_photo_table || _has_icc_table) )
		return TRUE;
	return FALSE;
}
		
//Convert the measure data back to lab data
McoStatus Viewdata::Yxytolab(YxyColorType *aYxyColor, LabColorType *mLabColor, long num)
{
	_dataconvert.Yxytolab(aYxyColor, mLabColor, num);

	return MCO_SUCCESS;
}

McoStatus Viewdata::gamutbound(LabColorType *aLabColor)
{
	LabColorType inbound, outbound, curbound;
	Boolean yes;
	double delta = 10000;

	inbound.L = aLabColor->L;
	inbound.a = 0;
	inbound.b = 0;
		
	yes = isingamut(&inbound);
	if(!yes){
		aLabColor->a = 0;
		aLabColor->b = 0;
		return MCO_SUCCESS;
	}	
	
	outbound.L = aLabColor->L;
	outbound.a = aLabColor->a;
	outbound.b = aLabColor->b;
	
	curbound.L = aLabColor->L;
	while(delta > 4){
		curbound.a = (outbound.a + inbound.a)/2;
		curbound.b = (outbound.b + inbound.b)/2;
		yes = isingamut(&curbound);
		if(yes){	//inside the boundary
			inbound.a = curbound.a;
			inbound.b = curbound.b;
		}
		else{
			outbound.a = curbound.a;
			outbound.b = curbound.b;
		}
			
		delta = (outbound.a - inbound.a)*(outbound.a - inbound.a) + 
				(outbound.b - inbound.b)*(outbound.b - inbound.b);
	}	
	
	aLabColor->a = (outbound.a + inbound.a)/2;
	aLabColor->b = (outbound.b + inbound.b)/2;	
	return MCO_SUCCESS;	
}

Boolean Viewdata::isingamut(LabColorType *aLabColor) 
{
	RGBColor		rgbcolor;
	long			outgamut;

	_dataconvert.labtorgb(aLabColor, &rgbcolor, &outgamut, 1);
	if(outgamut)	return FALSE;
	
	return TRUE;
}	

McoStatus Viewdata::handleMeasure(LabColorType *aLabColor, RGBColor *anRGBColor) 
{
	long			outgamut;

	_dataconvert.labtorgb(aLabColor, anRGBColor, &outgamut, 1);

	return (MCO_SUCCESS);
}

//doesn't care wether it's in gamut or not
McoStatus Viewdata::labtorgb(LabColorType *aLabColor, RGBColor *anRGBColor, 
			long *outgamut, long num) 
{
	_dataconvert.labtorgb(aLabColor, anRGBColor, outgamut, num);
	
	return MCO_SUCCESS;
}

//----------------------------------------------------------------
//		Get the RGB data from the resource fork. They are stored in
//		the resource fork because this is a demo program, and I don't
//		want to bother asking for the name of a data file every time.
//		Return the number of colors read.
//----------------------------------------------------------------
short Viewdata::getData( RGBColor *outColor, const short inMaxColors ) {
	short		colorCnt = 0;
	short		resColorCnt, *resColorCntPtr;
	Handle	theResource;
	ResColorsPtr	theColors;
	
	if( (theResource = GetResource(kRGBResType, kRGBResID)) == nil )
		return colorCnt;
	
	DetachResource( theResource );
	HLock( theResource );
	resColorCntPtr = (short *) (*theResource + 2);
	resColorCnt = *resColorCntPtr;
	theColors = (ResColorsPtr)(*theResource + 4);	// get past the list count
	
	if( (colorCnt = resColorCnt) > inMaxColors )
		colorCnt = inMaxColors;
	
	for( int i = 0; i < colorCnt; i++ ) {
		if( theColors[i].red > 65535 )
			theColors[i].red = 65535;
		if( theColors[i].red < 0 )
			theColors[i].red = 0;
			
		if( theColors[i].green > 65535 )
			theColors[i].green = 65535;
		if( theColors[i].green < 0 )
			theColors[i].green = 0;
		
		if( theColors[i].blue > 65535 )
			theColors[i].blue = 65535;
		if( theColors[i].blue < 0 )
			theColors[i].blue = 0;

		outColor[i].red = theColors[i].red;
		outColor[i].green = theColors[i].green;
		outColor[i].blue = theColors[i].blue;
	}
	
	HUnlock( theResource );
	DisposeHandle( theResource );
	
	return colorCnt;
	
}// getData()


McoStatus Viewdata::precalibrate()
{
	_gamma.initGamma(gTheGammaTable);
	colorCnt = getData(theColor, kMaxColorCnt);
	
	if(_devmea){
		_devmea->setdata(CALIBRATE_TYPE, colorCnt, theColor, devData);
		return MCO_SUCCESS;
	}
	
	return MCO_FAILURE;
}				

//for the 
McoStatus Viewdata::setmeasuredata(long count, double *inlab)
{
	long i;
	
	if(_devmea){
		for(i = 0; i < 3; i++)
			_inlab[i] = inlab[i];
		handleMeasure((LabColorType*)inlab, theColor);
		_devmea->setdata(MEASURE_TYPE, count, theColor, devData);
		return MCO_SUCCESS;
	}
	
	return MCO_FAILURE;	
}

Boolean Viewdata::getmeasuredata(double *outlab, double *deltaE)
{
	long i;
	double temp;
	
	if(_has_mea_data){
		Yxytolab(&devData[0], (LabColorType*)_mealab, 1);

		for(i = 0; i < 3; i++)
			outlab[i] = _mealab[i];	
		temp = 0;
		for(i = 0; i < 3; i++)
			temp += (_mealab[i] - _inlab[i])*(_mealab[i] - _inlab[i]);
		*deltaE = sqrt(temp);
		_has_mea_data = FALSE;
		return TRUE;
	}
	
	return FALSE;	
}

Boolean Viewdata::getvalidatedata(double *outlab, double *deltaE)
{
	long i, j;
	double mealab[3*kMaxColorCnt];
	double temp, average = 0;
	double	*inlab;
	
	if(_has_val_data){
		Yxytolab(devData, (LabColorType*)mealab, _valid_in_num);

		for(j = 0; j < _valid_in_num; j++){
			inlab =  &_valid_lab[3*_valid_in_index[j]];
			temp = 0;
			for(i = 0; i < 3; i++)
				temp += (mealab[j*3+i] - inlab[i])*(mealab[j*3+i] - inlab[i]);
			average += sqrt(temp);
		}
		
		*deltaE	= average/_valid_in_num;
		
		_has_val_data = FALSE;
		return TRUE;
	}
	
	return FALSE;	
}

McoStatus Viewdata::setvalidatedata(void)
{
	long i;
	RGBColor	rgbcolor[kMaxColorCnt];	
	long	outgamut[kMaxColorCnt];
	
	if(_devmea){
		labtorgb((LabColorType *)_valid_lab, rgbcolor, outgamut, _valid_num);
		_valid_in_num = 0; 
		for(i = 0; i < _valid_num; i++){
			if(!outgamut[i]){
				theColor[_valid_in_num].red = rgbcolor[i].red;
				theColor[_valid_in_num].green = rgbcolor[i].green;
				theColor[_valid_in_num].blue = rgbcolor[i].blue;
				_valid_in_index[_valid_in_num] = i;
				_valid_in_num++;
			}
		}		
		if(_valid_in_num == 0 )	return MCO_FAILURE;
		
		_devmea->setdata(VALIDATE_TYPE,_valid_in_num, theColor, devData);
		return MCO_SUCCESS;
	}
	
	return MCO_FAILURE;	
}


McoStatus Viewdata::openProfile()
{
	McoStatus status;
	StandardFileReply	sReply;
	CM2Header	header;
	long 		tagcount;
	long		valid;
		
//load in Icc monitor file
	SFTypeList	filelist;
	filelist[0] = 'prof';

	StandardGetFile(NULL, 1, filelist, &sReply);
	if(!sReply.sfGood)	return MCO_CANCEL;

	CsFormat icc;
	status = icc.openFile(&(sReply.sfFile));
	if(status != MCO_SUCCESS)	goto bail;
	
	if(!icc.isVersion2())	goto bail;
	status = icc.getHeader(&header, &tagcount);
	if(status != MCO_SUCCESS)	goto bail;

	//make sure the profile is a display profile 
	if( !(header.profileVersion | cmCS2ProfileVersion) )	goto bail;
	if(header.profileClass != cmDisplayClass)	goto bail;	
	if(header.dataColorSpace != cmRGBData)	goto bail;
		
	valid = icc.validProfile();
	if(!valid) goto bail;

	CMXYZType	white;

	status = icc.getTagdata(cmMediaWhitePointTag, (unsigned char*)&white);	
	if(status != MCO_SUCCESS) goto bail;

	//read in the input curve
	CMCurveType rch;
	long	rtagsize;
	unsigned short rcurv[256];
	long		i;
	
	//check tag size, and load tag data
	rtagsize = icc.getTagsize(cmRedTRCTag);
	status = icc.getCurveTypeinfo(cmRedTRCTag, &rch);
	if(status != MCO_SUCCESS) goto bail;
	status = icc.getcurveTagdata(cmRedTRCTag, (char*)&rcurv);
	if(status != MCO_SUCCESS) goto bail;

	double rgammat[256];
	double gamma;

	if(rch.countValue == 0){
		gamma = 1.0;
	}
	else if(rch.countValue == 1){
		gamma = (double)rch.data[0]/256.0;
	}
	else{
		gamma = 1.8;	
	}

	//gamma
	double ag[9] = {0.7, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.8};
	short  index[9] = { GM_07, GM_10, GM_12, GM_14, GM_16, GM_18, GM_20, GM_22, GM_28};
	
	for(i = 0 ; i < 9; i++)
		if(gamma < ag[i] ) break;
	if(i == 0)
		handleGammaChoice(GM_07);
	if(i == 9)
		handleGammaChoice(GM_28);
	if( gamma - ag[i-1]	>= ag[i] - gamma )
		handleGammaChoice(index[i]);
	else
		handleGammaChoice(index[i-1]);	
	
	//white point
	double wx, wy, wz, x, y;
	
	wx = white.XYZ[0].X/65536.0;
	wy = white.XYZ[0].Y/65536.0;
	wz = white.XYZ[0].Z/65536.0;
	
	x = wx/(wx + 1.0 + wz);
	y = 1.0/(wx + 1.0 + wz);
	
	double tx[5] = {0.3457, 0.3324, 0.3127, 0.2990, 0.2848};
	short  ty[5] = { 0.3585, 0.3474, 0.3290, 0.3149, 0.2932};
	short  windex[5] = { WM_5000, WM_5500, WM_6500, WM_7500, WM_9300};
	double min = 100, diff;
	short	minindex = 2;	//this is the default
	
	for(i = 0; i < 5; i++){
		diff = (x - tx[i])*(x - tx[i]) + (y - ty[i])*(y - ty[i]);
		if(diff < min){
			min = diff;
			minindex = i;
		}
	}
	
	handleWhiteChoice(windex[minindex]);		
		
	return MCO_SUCCESS;		
	
bail:
	icc.closeFile();
	showError(kIccErr, kStopDlg);
	return MCO_FAILURE;	
}	

// short saveDataMonacoMonitor( YxyColorPtr inData, const short inDataCnt )
McoStatus Viewdata::saveProfile(void)
{
	McoStatus status = MCO_SUCCESS;
	CsFormat	*csformat;
	McoHandle _A2B0_H;	//lut
	McoHandle _B2A0_H;	//lut
	McoHandle _out_of_gamut_H;	//lut
	Ctype _A2B0;
	Ctype _B2A0;
	Ctype _out_of_gamut;
	FileFormat *photoshop;
	Msetup msetup;
	double	monitor[12];	//monitor information
	double	xyztorgb[9];	//xyz to rgb conversion
	double	rgbtoxyz[9];	//rgb to xyz conversion
	long		i;

	StandardFileReply 	sinReply, soutReply;
	SFTypeList			typelist;
	Str255				prompt1 = "\pSave the Photoshop Profile As";
	Str255				prompt2 = "\pSave the ICC Profile As";
	Str255				outname = "\p";

	double		maxY,gammad,norm;
	MonitorData  *theMonitor = gTheMonitor;

/***************************************************
	// These original code are not correct anymore with monitor	
	// calibration.  The correct output should be the target white
	// point and target gamma info stored in the globals.  - JCY 3/31/96 
	
	maxY = inData[3].Y;
	gammad = log(inData[5].Y/maxY)/log(0.6);  

	msetup.ver = 2;
	msetup.gamma = (short)(100.0*gammad);
	msetup.wx = (short)(10000*inData[3].x);
	msetup.wy = (short)(10000*inData[3].y);
	msetup.rx = (short)(10000*inData[0].x);
	msetup.ry = (short)(10000*inData[0].y);
	msetup.gx = (short)(10000*inData[1].x);
	msetup.gy = (short)(10000*inData[1].y);
	msetup.bx = (short)(10000*inData[2].x);
	msetup.by = (short)(10000*inData[2].y);
***************************************************/
	
	// Use the global variables which contain the results 
	// from the calibration.                             - JCY 3/31/96, 4/4/96
	msetup.ver = 2;
	// The three channels should have the same gamma value:
	// The 0.5 is there for rounding.  Somehow the cast just truncates.
	msetup.gamma = (short)(100.0*(theMonitor->targetGamma.red + 
		theMonitor->targetGamma.green + theMonitor->targetGamma.blue)/3.0 + 0.5);
		
	// if user specifies uncorrected white point, then
	// save the monitor white point in the profiles
	if (theMonitor->targetWhitePoint.Y < 0.0) {  
		XYZColorType r, g, b, w, k;
		YxyColorType whitePoint;
		
		convertYxyToXYZ(theMonitor->redCurve[NUM_INTENSITY_POINTS-1].intensity,&r);
		convertYxyToXYZ(theMonitor->greenCurve[NUM_INTENSITY_POINTS-1].intensity,&g);
		convertYxyToXYZ(theMonitor->blueCurve[NUM_INTENSITY_POINTS-1].intensity,&b);
		convertYxyToXYZ(theMonitor->redCurve[0].intensity,&k);
		
		w.X = r.X + g.X + b.X - k.X - k.X;
		w.Y = r.Y + g.Y + b.Y - k.Y - k.Y;
		w.Z = r.Z + g.Z + b.Z - k.Z - k.Z;
		convertXYZToYxy(w,&whitePoint);

		msetup.wx = (short)(10000*whitePoint.x);
		msetup.wy = (short)(10000*whitePoint.y);
	}
	else {  // otherwise save the specified target
		msetup.wx = (short)(10000*theMonitor->targetWhitePoint.x);
		msetup.wy = (short)(10000*theMonitor->targetWhitePoint.y);
	}
	
	// Normal gamut = gamut of calibrated monitor
	msetup.rx = (short) (10000 * 
		theMonitor->redCurve[NUM_INTENSITY_POINTS-1].intensity.x);
	msetup.ry = (short) (10000 * 
		theMonitor->redCurve[NUM_INTENSITY_POINTS-1].intensity.y);
	msetup.gx = (short) (10000 * 
		theMonitor->greenCurve[NUM_INTENSITY_POINTS-1].intensity.x);
	msetup.gy = (short) (10000 * 
		theMonitor->greenCurve[NUM_INTENSITY_POINTS-1].intensity.y);
	msetup.bx = (short) (10000 * 
		theMonitor->blueCurve[NUM_INTENSITY_POINTS-1].intensity.x);
	msetup.by = (short) (10000 * 
		theMonitor->blueCurve[NUM_INTENSITY_POINTS-1].intensity.y);
		
	// save Photo Shop profile
	if(_has_photo_table){
	
		StandardPutFile(prompt1, outname, &soutReply);
		if(!soutReply.sfGood)  return MCO_CANCEL;

		if(soutReply.sfGood) {  // Modified not to quit if user hits cancel - JCY 4/4/96
			photoshop = new FileFormat;
			if(!photoshop)	return (MCO_MEM_ALLOC_ERROR);
		
			status = photoshop->createFilewithtype(&(soutReply.sfFile),
					soutReply.sfScript,'8BIM','8BMS');
			if(status != MCO_SUCCESS) return status;

			status = photoshop->relWrite(sizeof(Msetup), (char*)&msetup);
		
			photoshop->closeFile();
			delete photoshop;
			gSavedProfiles = TRUE;
		}
	}

	// save ICC profile:
	
	if(_has_icc_table){
		//convert to ICC data format	
		monitor[0] = (double)msetup.gamma/100.0;
		monitor[1] = (double)msetup.wx/10000.0;
		monitor[2] = (double)msetup.wy/10000.0;
		monitor[3] = (double)msetup.rx/10000.0;
		monitor[4] = (double)msetup.ry/10000.0;
		monitor[5] = (double)msetup.gx/10000.0;
		monitor[6] = (double)msetup.gy/10000.0;
		monitor[7] = (double)msetup.bx/10000.0;
		monitor[8] = (double)msetup.by/10000.0;

		status = newbuildrgbxyz(monitor, xyztorgb, rgbtoxyz);
		if(status != MCO_SUCCESS)	return status;

		//scale to D50 because of ICC
		//D50 is 0.9642, 1, 0.8249
		double wX = monitor[1]/monitor[2];
		double wY = 1;
		double wZ = (1.0 - monitor[1] - monitor[2])/monitor[2];
		 
		for(i = 0; i < 3; i++){
			rgbtoxyz[i] *= 0.9642/wX;
			rgbtoxyz[i+6] *= 0.8249/wZ;
		}
			
		StandardPutFile(prompt2, outname, &soutReply);
		if(!soutReply.sfGood)  return MCO_CANCEL;
			
		csformat = new CsFormat();
		if(!csformat)	return MCO_MEM_ALLOC_ERROR;
		
		status = csformat->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, 'sync', 'prof');
		if(status != MCO_SUCCESS)	return status;

		status = csformat->setHeader(cmDisplayClass, cmSigLut8Type);
		if(status != MCO_SUCCESS)	return status;

	//set the parameters
		CMXYZType	xyz;
		double 		x, y, z;
		
		xyz.typeDescriptor = cmSigXYZType;
		xyz.reserved = 0x00000000;
			
		xyz.XYZ[0].X = (unsigned long)(rgbtoxyz[0]*65536.0);
		xyz.XYZ[0].Y = (unsigned long)(rgbtoxyz[3]*65536.0);
		xyz.XYZ[0].Z = (unsigned long)(rgbtoxyz[6]*65536.0);
		status = csformat->setXYZtypedata(cmRedColorantTag, &xyz);
		if(status != MCO_SUCCESS)	return status;

		xyz.XYZ[0].X = (unsigned long)(rgbtoxyz[1]*65536.0);
		xyz.XYZ[0].Y = (unsigned long)(rgbtoxyz[4]*65536.0);
		xyz.XYZ[0].Z = (unsigned long)(rgbtoxyz[7]*65536.0);
		status = csformat->setXYZtypedata(cmGreenColorantTag, &xyz);
		if(status != MCO_SUCCESS)	return status;

		xyz.XYZ[0].X = (unsigned long)(rgbtoxyz[2]*65536.0);
		xyz.XYZ[0].Y = (unsigned long)(rgbtoxyz[5]*65536.0);
		xyz.XYZ[0].Z = (unsigned long)(rgbtoxyz[8]*65536.0);
		status = csformat->setXYZtypedata(cmBlueColorantTag, &xyz);
		if(status != MCO_SUCCESS)	return status;

		xyz.XYZ[0].X = (unsigned long)(wX*65536.0);
		xyz.XYZ[0].Y = (unsigned long)(wY*65536.0);
		xyz.XYZ[0].Z = (unsigned long)(wZ*65536.0);
		status = csformat->setXYZtypedata(cmMediaWhitePointTag, &xyz);
		if(status != MCO_SUCCESS)	return status;

		CMCurveType rh;
		unsigned short gamma;

		gamma = (unsigned short)( monitor[0]*256);

		rh.typeDescriptor = cmSigCurveType;
		rh.reserved = 0x00000000;
		rh.countValue = 1;
		rh.data[0] = gamma;
		
		status = csformat->setCurvetypedata(cmRedTRCTag, &rh, &(rh.data[0]));
		if(status != MCO_SUCCESS)	return status;

		status = csformat->setCurvetypedata(cmGreenTRCTag, &rh, &(rh.data[0]));
		if(status != MCO_SUCCESS)	return status;

		status = csformat->setCurvetypedata(cmBlueTRCTag, &rh, &(rh.data[0]));
		if(status != MCO_SUCCESS)	return status;

	//added on 11/15
		ptocstr(soutReply.sfFile.name);
		status = csformat->setDesctypedata((char*)(soutReply.sfFile.name), strlen((char*)(soutReply.sfFile.name))+1);
			
		status = csformat->writeTag(0);
		if(status != MCO_SUCCESS)	return status;
		csformat->closeFile();
		
		delete csformat;
		gSavedProfiles = TRUE;
	}
	
	// return MCO_FAILURE
	return status;
}

// Read preferences and results of previous calibrations.
// This Code Warrior c++ implementation really sucks -
// there's no ios::nocreate mode!!!!!   
// Can't really detect if file exists or not.  Redo this! - JCY 4/2/94 
// Done.  New function uses fileformat object. - JCY 4/10/96
McoStatus Viewdata::readPreference() {
	FileFormat 	prefFile;  // Code Warrior really sucks!!!  I can't allocate statically!!!
	FSSpec 		fSpec;
	McoStatus 		status;
		
	// Does file exist?
	status = prefFile.findFilefromPref(&fSpec, PREF_FILE_NAME, PREF_FILE_DIR);
	if (status != MCO_SUCCESS) {
		prefFile.closeFile();
		return (status);
	}
	
	// If not, return
	status = prefFile.openFile(&fSpec);
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}
	
	// Get monitor data
	status = prefFile.relRead(sizeof(MonitorData), (char*) (gTheMonitor)); 
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}
	
	// Get state variables
	status = prefFile.relRead(sizeof(port_type), (char*) (&gDefaultPort));
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}
	
	status = prefFile.relRead(sizeof(short), (char*) (&gCurrentTargetWhitePoint));
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}
	
	status = prefFile.relRead(sizeof(short), (char*) (&gCurrentTargetGamma));
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}

	status = prefFile.relRead(sizeof(short), (char*) (&gCurrentGamutControl));
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}

	status = prefFile.relRead(sizeof(Boolean), (char*) (&gUseCalibration));
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}

//following are the things added by Peter
	status = prefFile.relRead(sizeof(Boolean), (char*) (&_has_photo_table));
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}

	status = prefFile.relRead(sizeof(Boolean), (char*) (&_has_icc_table));
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}

	char lab[3];
	status = prefFile.relRead(sizeof(char)*3, lab);
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}
	
	for(short i = 0; i < 3; i++)
		_inlab[i] = (double)lab[i];

//following are the data added by Peter for validation
	long num;
	status = prefFile.relRead(sizeof(long), (char*)&num);
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (MCO_SUCCESS);
	}

	status = prefFile.relRead(sizeof(double)*3*num, (char*)_valid_lab);
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return status;
	}	
	
	_valid_num = num;
	
	status = prefFile.relRead(sizeof(double), (char*)&_validation_deltaE);
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return status;
	}	
	
	prefFile.closeFile();

	if( gCurrentTargetWhitePoint == WM_CUSTOM)
		_show = 0;
	else
		_show = 1;
			
	return(MCO_SUCCESS);
}

//should always save preference before quit
// Save preferences.
McoStatus Viewdata::savePreference() 
{
	FileFormat 	prefFile;
	FSSpec 		fSpec;
	McoStatus 		status;

	if(gCalibrationStatus == kNeverCalibrated)
		return MCO_CANCEL;
	
	if(!gUseCalibration)	return MCO_CANCEL;
			
	// Create preference file.
	status = prefFile.createFileinPref(&fSpec, PREF_FILE_NAME, PREF_FILE_DIR);
	if (status != MCO_SUCCESS) {
		return (status);
	}
	
	// save monitor data
	status = prefFile.relWrite(sizeof(MonitorData), (char*) (gTheMonitor)); 
	if (status != MCO_SUCCESS) {
		prefFile.closeFile();
		return (status);
	}
	
	// save state variables
	status = prefFile.relWrite(sizeof(port_type), (char*) (&gDefaultPort));
	if (status != MCO_SUCCESS) {
		prefFile.closeFile();
		return (status);
	}
	
	status = prefFile.relWrite(sizeof(short), (char*) (&gCurrentTargetWhitePoint));
	if (status != MCO_SUCCESS) {
		prefFile.closeFile();
		return (status);
	}
	
	status = prefFile.relWrite(sizeof(short), (char*) (&gCurrentTargetGamma));
	if (status != MCO_SUCCESS) {
		prefFile.closeFile();
		return (status);
	}
	
	status = prefFile.relWrite(sizeof(short), (char*) (&gCurrentGamutControl));
	if (status != MCO_SUCCESS) {
		prefFile.closeFile();
		return (status);
	}
	
	status = prefFile.relWrite(sizeof(Boolean), (char*) (&gUseCalibration));
	if (status != MCO_SUCCESS) {
		prefFile.closeFile();
		return (status);
	}

//following are the things added by Peter
	status = prefFile.relWrite(sizeof(Boolean), (char*) (&_has_photo_table));
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}

	status = prefFile.relWrite(sizeof(Boolean), (char*) (&_has_icc_table));
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}

	char lab[3];

	for(short i = 0; i < 3; i++)
		lab[i]  = (char)_inlab[i];

	status = prefFile.relWrite(sizeof(char)*3, lab);
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return (status);
	}

//added by Peter for handling the validation process
	status = prefFile.relWrite(sizeof(long), (char*)&_valid_num);
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return status;
	}

	status = prefFile.relWrite(sizeof(double)*3*_valid_num, (char*)_valid_lab);
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return status;
	}	

	status = prefFile.relWrite(sizeof(double), (char*)&_validation_deltaE);
	if (status != MCO_SUCCESS)  {
		prefFile.closeFile();
		return status;
	}	
		
	prefFile.closeFile();

	return (MCO_SUCCESS);
	
}

void Viewdata::_initvalid()
{
	long i;
	
	_valid_num = VALID_PATCH_NUM;

	_valid_lab[0] = 40;
	_valid_lab[1] = 20;
	_valid_lab[2] = -20;

	_valid_lab[3] = 50;
	_valid_lab[4] = 20;
	_valid_lab[5] = 20;

	_valid_lab[6] = 60;
	_valid_lab[7] = -20;
	_valid_lab[8] = 20;

	_valid_lab[9] = 70;
	_valid_lab[10] = 0;
	_valid_lab[11] = -20;

	_valid_lab[12] = 80;
	_valid_lab[13] = -20;
	_valid_lab[14] = 0;	
	
	_validation_deltaE = 6.0;
}

McoStatus Viewdata::getvaliddata(double *out, long *num, double *de)
{
	if(*num <= 0)	return MCO_FAILURE;
	
	if(*num < _valid_num)
		memcpy((char*)out, (char*)_valid_lab, sizeof(double)*3*(*num));
	else{
		memcpy((char*)out, (char*)_valid_lab, sizeof(double)*3*_valid_num);
		*num = _valid_num;
	}
	*de = _validation_deltaE;
	return MCO_SUCCESS;
}

McoStatus Viewdata::setvaliddata(double *out, long num, double de)
{
	if(num <= 0)	return MCO_FAILURE;
	
	if(num < _valid_num){
		memcpy((char*)_valid_lab, (char*)out, sizeof(double)*3*num);
		_valid_num = num;
	}		
	else{
		memcpy((char*)_valid_lab, (char*)out, sizeof(double)*3*_valid_num);
	}
	_validation_deltaE = de;
	
	return MCO_SUCCESS;
}

long Viewdata::getwhitepointdata(double *Y, double *x, double *y)
{
	*Y = gTheMonitor->targetWhitePoint.Y;
	*x = gTheMonitor->targetWhitePoint.x;
	*y = gTheMonitor->targetWhitePoint.y;
	
	return 1;
}

void Viewdata::setwhitepointdata(double Y, double x, double y)
{
	gTheMonitor->targetWhitePoint.Y = Y;
	gTheMonitor->targetWhitePoint.x = x;
	gTheMonitor->targetWhitePoint.y = y;
}
		