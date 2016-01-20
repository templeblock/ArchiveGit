//////////////////////////////////////////////////////////////////////////////////
//  patchset.h																	//
//  load a set of patch definitions in 											//
//																				//
//  James Vogh																	//
//  Nov 19, 1997																//
//////////////////////////////////////////////////////////////////////////////////

#ifndef IN_PATCHSET
#define IN_PATCHSET

#include "rawdata.h"


#define MAX_PATCH_NAMES (100)

class PatchSet{

private:
protected:
public:
	Str32	patchames[MAX_PATCH_NAMES]; 
	
	RawData	*patches[MAX_PATCH_NAMES];
	int32	filenum;
	

	McoStatus _loadpatchfile(int32 filenum,long ioDirID,short myVRef,unsigned char *filename);
	McoStatus _setDefaultPatch(int filenum);

	PatchSet(void);
	~PatchSet(void);

};

#endif