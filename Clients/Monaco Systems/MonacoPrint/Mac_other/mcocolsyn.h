// Confidential
#ifndef MCO_CS_LUTS
#define MCO_CS_LUTS
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1996 Monaco Systems Inc.
//
//	mcocolsyn.h
//
//	Defines color sync color transform class
//
//	Create Date:	8/96
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////

#include "mcofile.h"
//#include "mcoxform.h"

// This struct is the header of the file encoding of a McoXformLuts
typedef struct {
	int32 xformMagic;	// A magic number for verification
	int32 xformVersion;	// File version
} McoXCSFileHeader;

#define MCO_XFORMCS_MAGIC (933858)
#define MCO_XFORMCS_VERSION_1 (0)

// location of the transform

typedef enum {
	Mco_ICC_File = 0, // the transform is a file
	Mco_ICC_System,	  // the transform is a system default
	Mco_ICC_Null	  // the transform is null
	} McoICCLocation;

typedef enum {
	Mco_ICC_Scanner = 0,
	Mco_ICC_Monitor,
	Mco_ICC_Printer
	} MCOICCType;

//class McoColorSync : public McoXform {
class McoColorSync  {
private:
protected:

	int32		type;

	
	AliasHandle	_alias;	//alias handle
	int32		_aliaslen;	//alias length
	
	int16		validRef; // a flag indicating if a refrence is valid
	CMProfileRef ref;	  // a refrence to the profile
	int16		used;
	
public:
	McoColorSync(void);
	
	~McoColorSync(void);
	
	void setUsed(void) { used = TRUE;}
	int16 notUsed(void) {return !used;}
	
	void setType(int32 t);
	int32 getType(void ) {return type;}
	
	// return true if this refers to a real profile
	Boolean realProf(void) { return (type != Mco_ICC_Null); }
	
//	McoXfClass getXfClass();
	
	McoStatus setAlias(McoHandle alias,int32 alias_length);
	
	McoStatus getAlias(McoHandle *alias,int32 *alias_length);
	
	McoStatus setFSSpec(FSSpec *fs);

	McoStatus getFSSpec(FSSpec *fs);

	//Saves the McoXformCrrLuts data structure. 
	//This function should only need to save the alias and a header.
	//The first 32 bits written should be a flag that describes the derived class 
	//of the structure so that compatibility with Al's code is maintained. 
	//DO NOT read in this flag in the construct functions.
	McoStatus storeToFile(McoFile* fileObject);

	//same as above, but save to memory.  mem is a pointer to a memory pointer.  
	//When you write data into memory update the pointer to the memory. 
	//You can use the CopyMem2 function to do this automatically. 
	//The first argument to CopyMem is mem, the second is the address of the data 
	//to be written and the third is the size of the data to be written. 
	//See mcoxluts.c for an example.
	McoStatus storeToMem(char **mem);

	//report the size of the alias and the header.
	int32 storedSize(void);

	//load in the alias and the header	
	McoStatus constructFileLoad(McoFile *);
	
	//same as above but from memory. Use CopyMem2 to copy the data from mem 
	//into the data structure.  The parameters are the same as CopyMem but data 
	//is copied from mem instead of into mem.
	McoStatus constructMemLoad(char **mem);

	// Debuging code, used to return the size of the lut that is not acconted for by 
	// other memory counting methods
	// Rember to call BEFORE delete is called 
	int32 lutSizeOf(void) { return sizeof(McoColorSync);}
	
	// copy one profile into another
	McoStatus copyProfile(McoColorSync *copy);

	// find the first device for a given colorspace and return and setup to that device
	// usefull if CMYK is used and no device has been set yet 
	McoStatus getFirstProfile(int inputType);
	
	// get the colorsync profile refrence
	McoStatus getProfRef(CMProfileRef *pref);
	
	// get the data colorspace of the profile
	int16 getDataSpace(void);
	
	// close the colorsync refrence
	McoStatus closeRef(void);

	// get the alias for a profile in the colorsync folder given a filename
	McoStatus GetProfile(unsigned char *name);


};

int16 getProfileDataSpace(CMProfileRef ref);
OSType getProfileType(CMProfileRef ref);
int16 getColoSpacePixelChar(int16 colorspace);

McoStatus GetProfileList(int inputType,MCOICCType type, long *count, FSSpec *specs);



#endif


