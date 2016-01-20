#pragma once

#include "mcostat.h"
#include "mcotypes.h"
#include "csprofile.h"
#include "mpdata.h"

class Mpfileio{

public:

PrintData*	_printdata;

protected:

void _double2s15Fixed16(double* in, Fixed *out, long num);
void _s15Fixed162double(Fixed *out, double* in, long num);
// convert fields whose value may be greater than 32000
void _convertFromFixed(double *in,long num,int bits);
// convert fields whose value may be greater than 32000
void _convertToFixed(double *in,long num,int bits);
void convertTweak(TweakElement *tweakele, MNTweakType *tweaktype, long size);
void revconvertTweak(MNTweakType *tweaktype, TweakElement *tweakele, long size);

void deletePatchdata(PatchFormat *patchdata);

McoStatus convert(PatchFormat *patchdata, MNPatchType *patchtype);
McoStatus revconvert(MNPatchType *patchtype, PatchFormat *patchdata);

McoStatus _loadPatchdata(FILE *fs, long which);
McoStatus _savePatchdata(FILE *fs, long which);
McoStatus _loadLineardata(FILE *fs);
McoStatus _saveLineardata(FILE *fs);
McoStatus _loadTonedata(FILE *fs);
McoStatus _saveTonedata(FILE *fs);
McoStatus _loadCurveData(FILE *fs, long max, long * numHand, double * x, 
							double * y, char *desc, long *channel);
McoStatus _saveCurveData(FILE *fs, long max, long * numHand, double * x,
									double * y, char *desc, long channel);

McoStatus loadprofile(CsFormat *icc);
McoStatus writetoexistfile(CsFormat *icc);
McoStatus writetonewfile(CsFormat *icc);

	
public:
Mpfileio(PrintData* printdata);
~Mpfileio();

void cleanup(long which);
McoStatus createPatchdata(PatchFormat *patchdata, long *steps);

#if defined(_WIN32)
McoStatus openProfile(char *fsspec);
McoStatus saveProfile(char *fsspec);
McoStatus loadLineardata(void);
McoStatus loadPatchdata(char *fsspec, long which);
McoStatus savePatchdata(char *fsspec, long which);
McoStatus loadLineardata(char *fsspec);
McoStatus saveLineardata(char *fsspec);
McoStatus loadTonedata(char *fsspec);
McoStatus saveTonedata(char *fsspec);

#else
McoStatus openProfile(FSSpec *fsspec);
McoStatus saveProfile(FSSpec *fsspec);
McoStatus loadPatchdata(FSSpec *fsspec, long which);
McoStatus savePatchdata(FSSpec *fsspec, long which);
McoStatus loadLineardata(FSSpec *fsspec);
McoStatus saveLineardata(FSSpec *fsspec);
McoStatus loadTonedata(FSSpec *fsspec);
McoStatus saveTonedata(FSSpec *fsspec);

#endif
};
