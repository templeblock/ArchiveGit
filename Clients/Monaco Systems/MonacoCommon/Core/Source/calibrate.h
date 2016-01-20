#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "matrix.h"
#include "mcostat.h"
#include "mcotypes.h"
//#include "fileformat.h"
//#include "mcolut.h"
//#include "newcaltype.h"

#define MAX_DATA (2000)

/*
typedef enum {
	McoLut1	= 1,	//one dimensional lookup table
	McoLut2, 	//two dimensional lookup table
	McoLut3,	//three dimensional lookup table
	McoLut4		//four dimensional lookup table
}	McoLutDimension;

typedef enum {
	McoInRed = 1, 	//red
	McoInGreen,
	McoInBlue,
	
	McoInRG,
	McoInRB,
	McoInGR,
	McoInGB,
	McoInBR,
	McoInBG,

	McoInRGB
}	McoLutInChannel;

typedef enum {
	McoOutRed = 1,	//red
	McoOutGreen,
	McoOutBlue
}	McoLutOutChannel;

typedef enum {
	McoFormatUsual = 0,	//R, G, B sequence, no headers between channels
	McoFormatUnusual	//not usual sequence
}	McoLutFormat;

//multidimension lookup table header
typedef struct {
	McoLutFileHeader header;
	int32	lutDimension;	//1, 2, 3, 4 dimensional table
	int32	lutInChannel;	//what channel is input data represents
	int32	lutOutChannel;	//what channel is output data represents
	int32	lutFormat;		//R, G, B sequence or what?
}	McoMLutFileHeader;
*/	

class Linearcal {
private:

protected:
	

	int32 	_dimension;	//linear operation dimension
	int32 	_num_data;	//number of data points
	Matrix	*_weights;	//weight matrix
	Matrix *_core_r;		//core matrix, inverse matrix is needed for this matrix
	Matrix *_core_g;		//core matrix, inverse matrix is needed for this matrix
	Matrix *_core_b;		//core matrix, inverse matrix is needed for this matrix
	Matrix	*_ref_data_r;//reference data for r part, left size of equation
	Matrix	*_ref_data_g;//reference data for r part, left size of equation
	Matrix	*_ref_data_b;//reference data for r part, left size of equation
	Matrix	*_refr;
	Matrix	*_refg;
	Matrix	*_refb;

	Matrix	*_rm;		//temporary matrix containing all the data
	Matrix	*_rmt;		//temporary matrix transverse data
	
	//FileFormat* _weightfile;
	char	_savename[32];

	//the sequence is from A1 to A22, then from B1 to B22, and finally from L1 to L22
	//patch locations are stored here, there are 264 locations totally
	//we may use all of them, or just some of them
	int16	*_patchloc; 

//added on 8/24
	int32	_total_loc;	//total number of patches
//end 
	int32 	_num_loc;	//number of patches which been used

	int16	_cur_loc;	//how many patches has been processed
	int16	_one_time;	//how many patches to be processed once
	int16	_cur_chunk;	//current table chunk
	int16	_total_chunk; //total num of table chunks
	int16	_one_time_chunk;//how many chunk to process once
	int16	_start_loc;	//start location in current channel
	int16	_cur_channel; //current channel in building table process, 1/red, 2/g, 3/b
	double	_rw[60];	//structure to save parameters

	int16	_table_type;	//what type of table we are building
	int32	_d_size;		//channel size (for two dimen table)
	int32	_sub_d_size;	//sub channel size (for two dimen table)

	//added for bias patches on 8/17
	int32	_num_bias;	//total num of bias patches

	//added for bias patches on 8/17
	int32	_bias_id[10];	//which patch to be biased
	int32	_bias_times[10]; //how many time to be biased 

	void _deallocate(void);
	void _setstartloc(int16 start_loc);
	int16	_getstartloc(void);
	void _setparmchannel(int16 cur_channel);
	double* _getparmchannel(void);
	
public:

	McoStatus _err;
	Linearcal(int32 dimension, int16 one_time, int32 total_loc);
	~Linearcal(void);

	void _applyf(double r, double g, double b, double *cmyk);
	void _applyf_ab(double r, double g, double b, double *ab);
	McoStatus loaddirect(double*, double*, int16* end); //signature to show its finished
	McoStatus compute(void);
	void testR(double* rgbcolor, double *refrgb, 
		double *rv_r, double *rv_g, double *rv_b);
	McoStatus saveLabtoCMYK(McoHandle cmykH);
	McoStatus saveLabtoCMYK2(McoHandle cmykH,  McoHandle labH);
	McoStatus saveLabtoCMYK3(McoHandle cmykH,  McoHandle labH);
	McoStatus gray_to_cmy(double *cmyk);
};
	
#endif