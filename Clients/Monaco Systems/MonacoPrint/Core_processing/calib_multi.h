////////////////////////////////////////////////////////////////////////////
//	calib_multi.h														  //
//																		  //
//	calibrate by building multiple calibrations and and blending the 	  //
//  calibrations together												  //
//																		  //
//  Created on Feb 16, 1996												  //
//  By James Vogh														  //
////////////////////////////////////////////////////////////////////////////

#ifndef IN_CALIB_MULTI
#define IN_CALIB_MULTI

//#include "calibrate.h"
#include "mcomem.h"
#include "buildtableindex.h"
#include "monacoprint.h"

#include "Thermobject.h"
#include "linearize_data.h"

typedef enum {
	MC_Normal = 0,
	MC_Gauss,
	MC_Inter
	} MC_CalibType;

// process the entire region
class calib_base {
private:
protected:
	
public:
	//Linearcal		*calib;
	calib_base(void);
	~calib_base(void);
	
	double		*_patch;
	
	int 		direction;
	double		max_cal;
	double		inv_max_cal_0;	// some pre-computed values 
	double		inv_max_cal_1;


	double		n0,n1;
	


	McoStatus interpolate_9(double *cmyk,double *lab);
	McoStatus interpolate_33(double *cmyk,double *lab,double *table);
	McoStatus interpolate_3D_n(double *cmyk,double *lab,double *table,int32 n);
	void	  interpolate_3D_n_0(double *lab,double *table);
	void	  interpolate_3D_n_1(double *lab,double *table);
	McoStatus interpolate_4D(double *cmyk,double *lab,double *table_cmy,double *table_cmyk);

	virtual McoStatus convtCMYK_LAB(double *cmyk,double *lab) {return MCO_UNSUPPORTED_OPERATION;}
	virtual McoStatus set_up(double* rgbcolor, double *refrgbin,int32 dimension, int32 total_loc) {return MCO_SUCCESS;}
	virtual McoStatus eval(double *lab,double *cmyk,int32 num,double *dist) {return MCO_SUCCESS;}
//	virtual McoStatus evalCMY(double *lab,double *cmyk,int32 num,double *m_dist) {return MCO_SUCCESS;}
	virtual McoStatus evalCMYSpace(double *lab,double *cmyk) {return MCO_SUCCESS;}

	virtual McoStatus getMinMaxK(double *lab, double *minK,double *maxK,short *gamut) {return MCO_SUCCESS;}
	virtual double getSpacing(void) {return MCO_SUCCESS;}
		
	virtual McoStatus start(void) {return MCO_SUCCESS;}
	virtual McoStatus end(void) {return MCO_SUCCESS;}
	virtual double getSatDistance(double *lab,double K) {return -1;}
	virtual double getDistance(double *lab,double K,double max_dist) {return -1;}
};
		
	


// A calibration bassed on interativly using interpolated Lab values

class calib_inter4:public calib_base {
private:
protected:

public:
	calib_base	**calib;
	double		*_patch;
	McoHandle	_cmykTableH;
	McoHandle	_cmykTableBigH;
	double 		iterations;
	double		max_ink;
	
	double	*cmykTable;
	double	*cmykTableBig;
	
	ThermObject	*thermd;
	
	BuildTableIndex *TableIndex;
	
	LinearData	*linear_data;
	
	double		C_Lut[BT_DIM];
	double		M_Lut[BT_DIM];
	double		Y_Lut[BT_DIM];
	
	calib_inter4(calib_base **cal,int d, double max_c,double mi,BuildTableIndex *ti,LinearData	*linear_data);	

	McoStatus set_up(double* rgbcolor, double *refrgbin,int32 dimension, int32 total_loc,McoHandle invTableH,McoHandle invTableBigH);
	McoStatus convtCMYK_LAB(double *cmyk,double *lab);
	McoStatus eval(double *lab,double *cmyk,int32 num,double *dist);
//	McoStatus evalCMY(double *lab,double *cmyk,int32 num,double *m_dist);
	McoStatus evalCMYSpace(double *lab,double *cmyk);
	
	McoStatus start(void);
	McoStatus end(void);
	
	double getDistance(double *lab,double K,double max_dist);
	double getSatDistance(double *lab,double K);


	McoStatus getMinMaxK(double *lab, double *minK,double *maxK,short *gamut);
	double getSpacing(void);
	
	McoStatus evalCMYSpace(double *lab,double *cmyk,double *cmy,double *dist);
	McoStatus getBestCMYK(double *lab,double K,double *cmy,double *distance);

	
};// A calibration bassed on interativly using interpolated Lab values

class calib_inter3:public calib_base {
private:
protected:

public:
	calib_base	**calib;
	double		*_patch;
	McoHandle	_cmykTableH;
	McoHandle	_cmykTableBigH;
	double 		iterations;
	double		max_ink;
	
	double	*cmykTable;
	double	*cmykTableBig;
	
	ThermObject	*thermd;
	
	calib_inter3(calib_base **cal,int d, double max_c,double mi,ThermObject *thermd);	

	McoStatus set_up(double* rgbcolor, double *refrgbin,int32 dimension, int32 total_loc,McoHandle invTableH,McoHandle invTableBigH);

	McoStatus eval(double *lab,double *cmyk,int32 num,double *dist);

	
	McoStatus start(void);
	McoStatus end(void);
	
	double getDistance(double *lab,double K,double max_dist);

	McoStatus evalRGBSpace(double *lab,double *cmyk,double *cmy,double *dist);
	McoStatus getBestRGB(double *lab,double *rgb,double *distance);

	
};
#endif
