////////////////////////////////////////////////////////////////////////////////
//	tweak_patch.h															  //
//																			  //
//	tweak the patch data so that data is more accurate						  //
//																			  //
//	May 23, 1996															  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef IN_TWEAK_PATCH
#define IN_TWEAK_PATCH

#include "calibrate.h"
#include "rawdata.h"

#define TWEAK_MAGIC_NUM 63464
#define TWEAK_VERSION_NUM 100

typedef enum {
	Calibrate_Tweak = 0,
	GamutComp_Tweak
	} Tweak_Types;

class Tweak_Element {
public:
double 	lab_d[3];
double 	lab_g[3];
double	lab_p[3];
double	Lr,Cr;
Tweak_Types	type;
char	name[40];

Tweak_Element(char *n);
~Tweak_Element(void);
};

class Tweak_Patch {
private:
protected:


McoHandle	labTableH;
Tweak_Types	type;
public:

Tweak_Patch(Tweak_Types t);
~Tweak_Patch();

double findPerc(double range, double perc);
McoStatus Init_Table(void);
McoStatus Build_Table(McoHandle tdH,McoHandle tdrH, int32 num);
McoStatus Load_Table(FileFormat *fileF);
McoStatus Save_Table(FileFormat *fileF);
McoStatus Load_Data(FILE *bf);
// Modify the table
McoStatus Modify_Table(int num_tw,Tweak_Element **tweaks);
McoStatus Modify_Table(double *lab_d,double *lab_p,double Lr,double Cr);
McoStatus Modify_Table_L(double light_adj);
McoStatus Modify_Color(double *lab_in,double *lab_out,double *lab_d,double *lab_p,double Lr,double Cr);
McoStatus eval(RawData *inD,RawData *outD,int32 num);
McoStatus eval(double *cmykp,double *labp,int32 num);
McoStatus eval(double *labo,double *labg,double *labt,int32 num);


};

#endif