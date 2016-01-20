////////////////////////////////////////////////////////////////////////////////
//	profiledoc.h															  //
//																			  //
//	The code for dealing with the profile data, replaces much of printdialog.c//
//	James Vogh																  //
// June 4, 1996																  //
////////////////////////////////////////////////////////////////////////////////


#ifndef PRINT_PROFILE_H
#define PRINT_PROFILE_H

#include "monacoprint.h"

#include "mcostat.h"
#include "mcotypes.h"
#include "mcomem.h"
#include "tweak_patch.h"
#include "scantarg.h"
#include "rawdata.h"

#include "calibrate.h"
#include "interpolate.h"
#include "document.h"

#include "thermobject.h"
#include "printertype.h"

#include "condata.h"

#include "csprofile.h"
#include "mpdata.h"

#include "calcurves.h"
#include "linearize_data.h"

struct BlackCurve {
	double curve[101];
	};





//design of this class is close related to the event loop
//so interface must be compatible with event interface.
//user should have minimal effect to understand and use this class.
//virtual functions are really intended for user to subclassing.

class ProfileDoc:public Document{

private:
protected:
public:

	// The gammut surface for just the CMY inks
	McoHandle _gammutSurfaceH;
	// The gammut surface for the CMYK inks
	McoHandle _gammutSurfaceBlackH;
	// gammut surface for 10 levels of K
	McoHandle _gammutSurfaceManyBlackH;
	// gammut surface L values for 10 levels of K
	McoHandle _gammutSurfaceManyBlackLH;
	// The cmyk to lab table added by James on 11/11
	Boolean didGammutSurface; // added by James on 1/19
	Boolean didGammutCompression; // added by James on 1/19
	Boolean _smoothedPatches;
	
	double	_SaturationAmount; // The distance to move into the gammut surface
	double	_PaperWhite;	   // The amount to move towards the color of the paper
	double	_col_bal_cutoff;   // The cutoff for paper color balance
	
	int 	_staurationpreview; // the amount of saturation removal or boost for 
	
	McoHandle _cmyktableH;
	//control data passed to the black dialog
	ConData 	_condata;
	long		_rev_table_grid;
	long		_rev_table_size;
	
	McoHandle	_con_to_dev_tableH;
	double		*_table;
	
	McoHandle	_dev_to_con_tableH;
	McoHandle	_dev_to_con_tableH2;

	double		*K_only_cmyk;
	
	McoHandle	_out_of_gamutH;
	
//need a Lab table because of gamut compression
	McoHandle 	_labtableH;
	McoHandle 	_labtableCopyH;
	
	
	double		*_patch;
	RawData		*patchD;
	// a "safe" copy of _patchH
	RawData		*patchDcopy;
			


//added on 4/11/96 by Peter for ink simulation
	RawData		*simupatchD;
	RawData		*simupatchDcopy;
	double		_simumaxL;
	double		_simuminL;
	
//table of black generation
	McoHandle	_blacktableH;
	double		*_blacktable;
	double		_black_ink_limit;
	double		_total_ink_limit;

//added on 9/22, state of the current phase
	short		_state_phase;
	
// the name of the patchfile

	Str255	patchFName;
		
// Array that performs clipping
	unsigned char _clipt[10496];

	ThermObject	*thermd;
	
	PrinterType *printerData;
	
	PrintData	*printData;
	
	CalCurves	*calCurves;
	
// functions that performs linearization
	LinearData	*linear_data;	
	
	int	changedProfile;	// flag indicates if a profile should be created
	int monacoProfile;	// flag indicates if profile was edited or created by monaco
	int	monacoEdited;	// flag indicating that profile was edited by monaco
	int createdProfile; // flag indicating that profile was created
	int	changedLinearTone; // flag indicating that linear or tone data has changed
	
	int processing;
	

//end of add	

	int	_monacoprint;

	McoStatus _handle_target(void);
	virtual McoStatus _handle_print(void) {return MCO_UNSUPPORTED_OPERATION;}
	
	McoStatus _handle_calibrate(void);
	McoStatus _handle_calibrate_tp(void);
	McoStatus _apply_gamut_comp(void);
	McoStatus _apply_black(void);	
	virtual McoStatus _handle_gamut(void) {return MCO_UNSUPPORTED_OPERATION;}
	void _norm_white_point(void);
	void _extend_black_point(void);


	void 	_initphotolab(void);
	void	_initicclab(void);

	McoStatus _initGammutC(void);
	McoStatus _initAll(void);
	//double _adjustangle(double angle);
	

	McoStatus _handle_read_xrite(short pagesize,short portnum);

	//for rgb calibration
	McoStatus _handle_rgb_calibrate(void);
	
	// for cmyk calibration
	McoStatus _handle_cmyk_calibrate(void);
	
	
	//Max white and black points, used to stretch the L values for the CMYK to LAB table
	//These values are set in apply gamut
	double	_maxgrayL,_mingrayL;
	

	Tweak_Patch *tweak;
	
	Tweak_Patch *gtweak;
	

	// settings for reading in strips
	short whichsize,whichport;

	//A flag to indicate the type a patches (RGB or CMYK)
	PatchType patchtype;
	
	
	// data for reading in the tiff file
	//croptarget	
	u_int16 	samplesperpixel;
	u_int32		width;
	u_int32		length;
	u_int16		bitspersample;
	u_int16		photometric;
	u_int16		compression;
	u_int16		fillorder;
	u_int16		orientation;
	u_int32		rowsperstrip;
	u_int16		planarconfig;	
	u_int16		resolutionunit;	
	u_int32		xresnumerator;
	u_int32		yresnumerator;
	u_int32		xresdenominator;
	u_int32		yresdenominator;	
	

	McoHandle targetBufferH;
	//short		_need_save;

	ProfileDoc(void);
	~ProfileDoc(void);
	
	McoStatus setData(ConData *data);
	McoStatus setKCurve(BlackCurve *curve);
	McoStatus _loadPatchRef2(void);
	
	McoStatus initPrintData(void);
	// clear the tone and linear data for a revert
	void clearToneLinear(void);
	McoStatus copyIntoPrintData(void);
	McoStatus copyOutOfPrintData(void);
	
	virtual long	inputStatus(void) { return No_Input;}

	// check the dongle and decrement the counter
	virtual McoStatus _checkEve(void) {return MCO_SUCCESS;}
	// Remove a window from the list of windows
	//virtual McoStatus removeWindow(baseWin *win) {return MCO_UNSUPPORTED_OPERATION;}

	virtual McoStatus _handle_read_datafile(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus _handle_read_sim_datafile(void) {return MCO_UNSUPPORTED_OPERATION;}
	// get a filename for the current file (used by polaroid)
	virtual McoStatus GetFileName(int flag) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus handle_read_iccfile(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus handle_reread_iccfile(void) {return MCO_UNSUPPORTED_OPERATION;}
#ifdef powerc
	virtual McoStatus handle_read_iccfile(FSSpec *fspec) {return MCO_UNSUPPORTED_OPERATION;}
#endif
	//read the file, now only read the data file
	virtual McoStatus _read_datafile(RawData **patchd,FILE *fs) {return MCO_UNSUPPORTED_OPERATION;}

	virtual McoStatus savetoPhototable(int32 pass) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus savetoRGBtoRGB(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus savetoIcctable(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus savetoCrdtable(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual	McoStatus save_datafile_menu(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual	McoStatus handle_save_datafile(RawData *p) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus savetoIccLinktable(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus saveToneAndLinear(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus savetoTekCmyktable(void) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus _handle_read_tifffile(RawData *PatchD) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus _handle_read(short *item,short *which,int density) {return MCO_UNSUPPORTED_OPERATION;}
	//virtual McoStatus doCommand(Window_Events *code,MS_WindowTypes *winType,int32 *winNum,void **data,Boolean *waitdoc,Boolean *changed) {return MCO_UNSUPPORTED_OPERATION;}

	virtual McoStatus aftercrop(Rect	*selectrect, RawData *PatchD) {return MCO_UNSUPPORTED_OPERATION;}


	// produce a lut for the inverse table that correct for linearization in the table

	McoStatus buildLinearData(void);

	McoStatus buildInverseCuvre(Ctype *CT);
	
	McoStatus buildOutputCuvre(Ctype *CT);
	
	McoStatus buildInputCuvre(Ctype *CT);


	McoStatus _createLinktable(McoHandle linkH);

	McoStatus aftercropCancel(void);

	McoStatus _apply_color_balance(void);
	McoStatus _apply_color_balance(RawData *pD,double paperwhite,McoHandle charData,int32 num);

	McoStatus _create_simul_inverse(void);

	McoStatus _smooth_patch(McoHandle *);
	McoStatus _smooth_Lab(void);
	McoStatus _create_CMYK_to_Lab(int32 BlackWhiteFlag);
	// create a photoshop cmyk to lab table, the black is not very good
	McoStatus _create_PS_CMYK_to_Lab(int32 BlackWhiteFlag);
	McoStatus _normalizeInvTable(int32 BlackWhiteFlag);
	Boolean dataPresent(void);
	
	int validCal(void);

	virtual Boolean checkCloseDoc(void) {return FALSE;}

	void	_init_patchref(void);
	McoStatus _build_small_invers(McoHandle BigH,McoHandle Table1H,McoHandle Table2H,double *maxTL,double *minTL);
	 // the rgb version of the inverse builder
	McoStatus _build_small_invers_3(McoHandle BigH,McoHandle Table1H,McoHandle Table2H,double *maxTL,double *minTL);

// load the linearization data from the raw data object int the printer settings
// do not load if no data is present or if linearization data has aready been loaded

	McoStatus loadLinearData(void);
	
};

#endif //