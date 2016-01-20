// Confidential
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1996 Monaco Systems Inc.
//
//	mcocolsyn.c
//
//	Defines color sync color transform class
//
//	Create Date:	8/96
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////
#include "mcocolsyn.h"
#include "mcomem.h"
//#include "mcoplug.h"
#include "monacoprint.h"


int16 getProfileDataSpace(CMProfileRef ref)
{
CMAppleProfileHeader header;

	CMGetProfileHeader(ref,&header);
	
	switch (header.cm2.dataColorSpace)
		{
		case 'XYZ ':return cmXYZ32Space;	
			break;
		case 'Lab ':return cmLAB24Space;
			break;
		case 'Luv ':return cmLUV32Space;
			break;
		case 'Yxy ':return cmYXY32Space;
			break;
		case 'RGB ':return cmRGB24Space;
			break;
		case 'GRAY':return cmGrayASpace;
			break;
		case 'HSV ':return cmHSV32Space;
			break;
		case 'HLS ':return cmHLS32Space;
			break;
		case 'CMYK':return cmCMYK32Space;
			break;
		case 'CMY ':
		case 'MCH5':
		case 'MCH6':
		case 'MCH7':
		case 'MCH8':
					return cmNoSpace;
			break;
		}
return cmNoSpace;
}

OSType getProfileType(CMProfileRef ref)
{
CMAppleProfileHeader header;

	CMGetProfileHeader(ref,&header);
	
	return header.cm2.profileClass;
}

int16 getColoSpacePixelChar(int16 colorspace)
{
	
	switch (colorspace)
		{
		case  cmXYZ32Space:	
		case  cmLAB32Space:
		case  cmLUV32Space:
		case  cmYXY32Space:
		case  cmRGB32Space:
		case  cmHSV32Space:
		case  cmHLS32Space:
		case  cmCMYK32Space:
			return 4;
		case cmGrayASpace:
			return 2;
		case cmLAB24Space:
		case cmRGB24Space:
			return 3;
		}
return 4;
}

McoColorSync::McoColorSync(void)
{
	_alias = 0;
	_aliaslen = 0;
	validRef = FALSE;
	type = Mco_ICC_Null;
	used = FALSE;
}
 
McoColorSync::~McoColorSync(void)
{
	if(_alias ){
		McoDeleteHandle((McoHandle)_alias);
		_alias = 0;
		_aliaslen = 0;
	}
	if (validRef) CMCloseProfile(ref);
}

/*McoXfClass McoColorSync::getXfClass()
 {
	return McoColSyn; 
 }
*/ 
 
 McoStatus McoColorSync::setAlias(McoHandle alias,int32 alias_length)
{
if (_alias != NULL) McoDeleteHandle((McoHandle)_alias);
_alias = (AliasHandle)alias;
_aliaslen = alias_length;

if (validRef) CMCloseProfile(ref);
validRef = FALSE;

type = Mco_ICC_File;

used = FALSE;

return MCO_SUCCESS;
}

 McoStatus McoColorSync::getAlias(McoHandle *alias,int32 *alias_length)
{
if (alias == NULL) return MCO_FAILURE;
*alias = (McoHandle)_alias;
*alias_length = _aliaslen;
return MCO_SUCCESS;
}

McoStatus McoColorSync::setFSSpec(FSSpec *fs)
{
OSErr	iErr;

if (_alias != NULL) McoDeleteHandle((McoHandle)_alias);

iErr = NewAlias(nil,fs, &_alias);		
if(iErr != 0) return MCO_FILE_OPEN_ERROR;

_aliaslen = (**_alias).aliasSize;

if (validRef) CMCloseProfile(ref);
validRef = FALSE;

type = Mco_ICC_File;

used = FALSE;

return MCO_SUCCESS;
}

McoStatus McoColorSync::getFSSpec(FSSpec *fs)
{
OSErr	iErr;
Boolean wasChanged;

if (_alias == NULL) return MCO_FAILURE;
iErr = ResolveAlias(NULL, _alias, fs, &wasChanged);	
if(iErr == 0) return MCO_SUCCESS;

return MCO_FILE_OPEN_ERROR;
}

void McoColorSync::setType(int32 t) 
{ 
if ((type != t) && (validRef)) CMCloseProfile(ref);
type = t;
validRef = FALSE;

if (type != Mco_ICC_File) 
	{
	if (_alias != NULL) McoDeleteHandle((McoHandle)_alias);
	_alias = NULL;
	_aliaslen = 0;
	}

used = FALSE;
}

McoStatus McoColorSync::storeToFile(McoFile* fileObject)
{
 	McoStatus status;
 	int32 derivedClass;
 	int16 i;
 	McoXCSFileHeader header;
 	
 	// FIrst, write the derived class type
	// Modified by James (changed to McoTable)
 //	derivedClass = McoColSyn;
 	
 	status = fileObject->relWrite(sizeof(int32), &derivedClass);
	if(status!=MCO_SUCCESS)
	 	return status;

	 	
	 // Next, write the derived class header
	header.xformMagic = MCO_XFORMCS_MAGIC;
	header.xformVersion = MCO_XFORMCS_VERSION_1;

	status = fileObject->relWrite(sizeof(McoXCSFileHeader), &header);
	if(status!=MCO_SUCCESS)
	 	return status;
	 	
	// write the alias
//	if(!_aliaslen || !_alias)	return MCO_FILE_DATA_ERROR;	
	status = fileObject->relWrite(4, (char*)&_aliaslen);
	if(status != MCO_SUCCESS) 	return status;

	if (_alias)
		{
		McoLockHandle((McoHandle)_alias);
		status = fileObject->relWrite(_aliaslen, (char*)(*_alias));
		if(status != MCO_SUCCESS) 	return status;
		McoUnlockHandle((McoHandle)_alias);
		}
	// write the type
	status = fileObject->relWrite(sizeof(int32), &type);
	if(status != MCO_SUCCESS) 	return status;
	
	return MCO_SUCCESS; 
 }

//mem should be large enough to contain the header structure and alias
McoStatus McoColorSync::storeToMem(char **mem)
{
 	McoStatus status;
 	int32 derivedClass;
 	int16 i;
 	McoXCSFileHeader header;
 	
	// This section has been rewritten by James to use the CopyMem function
	// The mem variable must be updated as data is written

 	// FIrst, write the derived class type
	// Modified by James
 	//derivedClass = McoColSyn;
 	
	CopyMem(mem,&derivedClass, 4);
	 	
	 // Next, write the derived class header
	header.xformMagic = MCO_XFORMCS_MAGIC;
	header.xformVersion = MCO_XFORMCS_VERSION_1;

	CopyMem(mem,&header, sizeof(McoXCSFileHeader));

	 	
	// write the alias
	//if(!_aliaslen || !_alias)	return MCO_FILE_DATA_ERROR;	
	CopyMem(mem,&_aliaslen, 4);
	
	if (_alias)
		{
		McoLockHandle((McoHandle)_alias);
		CopyMem(mem,(*_alias),_aliaslen);
		McoUnlockHandle((McoHandle)_alias);
		}
		
	CopyMem(mem,&type,sizeof(int32));

	return MCO_SUCCESS; 
 }


//report the size of the alias and the header.
int32  McoColorSync::storedSize(void)
{
	//alias is not loaded yet
//	if(!_aliaslen)	return 0;

	// alias  + header + alias_length + xform type + file type
	return(_aliaslen+ sizeof(McoXCSFileHeader)+4+4+4);
}


//load alias and fileheader
McoStatus  McoColorSync::constructFileLoad(McoFile *fileObject)
{
 	McoStatus status;
 	int32 derivedClass;
 	McoXCSFileHeader header;

	//read derived class

// Modified by James	
// DO NOT DO THIS!!!
// Al's code does not read this in
/*
	status = fileObject->relRead(4, &derivedClass);
	if(status != MCO_SUCCESS)	return status;

	if( derivedClass != McoTable)	return MCO_FILE_DATA_ERROR;
*/

	//read in the structure
	status = fileObject->relRead(sizeof(McoXCSFileHeader), (char*)&header);
	if(status != MCO_SUCCESS) 	return status;

	if(header.xformMagic != MCO_XFORMCS_MAGIC || 
		header.xformVersion != MCO_XFORMCS_VERSION_1)
		return MCO_FILE_DATA_ERROR;

	//read in the length of alias
	status = fileObject->relRead(4, (char*)&_aliaslen);
	if(status != MCO_SUCCESS) 	return status;

	if(_alias)	McoDeleteHandle((McoHandle)_alias);
	_alias = NULL;
	if (_aliaslen > 0) 
		{
		_alias = (AliasHandle)McoNewHandle(_aliaslen);
		if(!_alias)	return MCO_MEM_ALLOC_ERROR;
		McoLockHandle((McoHandle)_alias);
		status = fileObject->relRead(_aliaslen, (char*)(*_alias));
	//iErr = ResolveAlias(nil, (AliasHandle)_alias, &cmykfsspec, &wasChanged);	
	
		McoUnlockHandle((McoHandle)_alias);
		}
	
	status = fileObject->relRead(sizeof(int32),&type);
	
	used = FALSE;
	
	return MCO_SUCCESS;
}

	
McoStatus McoColorSync::constructMemLoad(char **mem)
{
 	McoStatus status;
 	int32 derivedClass;
 	int16 i;
 	McoXCSFileHeader header;
 	 	
// Modified by James
// This section has been rewriten to use the CopyMem2 function that automatically 
// updates the mem variable.  The derived class is no longer read in because 
// Al's code does it this way.


	CopyMem2(mem,&header,sizeof(McoXCSFileHeader));		// ?OK

	if(header.xformMagic != MCO_XFORMCS_MAGIC || 
		header.xformVersion != MCO_XFORMCS_VERSION_1)
		return MCO_FILE_DATA_ERROR;
	
	CopyMem2(mem,&_aliaslen,4);
	if(_alias)	McoDeleteHandle((McoHandle)_alias);
	_alias = NULL;
	if (_aliaslen > 0)
		{
		_alias = (AliasHandle)McoNewHandle(_aliaslen);
		if(!_alias)	return MCO_MEM_ALLOC_ERROR;
		McoLockHandle((McoHandle)_alias);
		
		CopyMem2(mem,*_alias,_aliaslen);
		McoUnlockHandle((McoHandle)_alias);
		}
		
	CopyMem2(mem,&type,sizeof(int32));

	used = FALSE;

	return MCO_SUCCESS; 
 }
 
McoStatus McoColorSync::copyProfile(McoColorSync *copy)
{
 int i;
 unsigned char *alias,*aliasc;
 CMProfileRef pref;
 
 if (validRef) closeRef();
 used = FALSE;
 
 type = copy->type;
 if ((copy->_alias) && (copy->_aliaslen >0))
 	{
 	_alias = (AliasHandle)McoNewHandle(copy->_aliaslen);
 	aliasc = (unsigned char*)McoLockHandle((Handle)copy->_alias);
 	alias = (unsigned char*)McoLockHandle((Handle)_alias);
 	for (i=0; i<copy->_aliaslen; i++) alias[i] = aliasc[i];
 	McoUnlockHandle((Handle)copy->_alias);
 	McoUnlockHandle((Handle)_alias);
 	}
 	
 _aliaslen = copy->_aliaslen;
 
 if (copy->validRef != NULL)
 	{
 	getProfRef(&pref);
 	}
 return MCO_SUCCESS;
 }
 
  
// find the first device for a given colorspace and return and setup to that device
// usefull if CMYK is used and no device has been set yet 
McoStatus McoColorSync::getFirstProfile(int inputType)
{
FSSpec profile;
McoStatus status = MCO_SUCCESS;
CMSearchRecord rec = {cmMatchAnyProfile,0,0,0,0,0,0,0,0,0};
CMProfileSearchRef searchRes;
unsigned long cnt;
int i;


switch (inputType) {
	case cmGrayASpace:
		rec.dataColorSpace = 'GRAY';
		rec.searchMask = 4;
		break;
	case cmLAB24Space:
		rec.dataColorSpace = 'LAB ';
		rec.searchMask = 4;
		break;				
	case cmRGB24Space:
		rec.dataColorSpace = 'RGB ';
		rec.searchMask = 4;
		break;
	case cmCMYK32Space:
		rec.dataColorSpace = 'CMYK';
		rec.searchMask = 4;
		break;
	}

CMNewProfileSearch(&rec,nil,&cnt,&searchRes);
if (cnt <=0) return MCO_FAILURE;

CMSearchGetIndProfileFileSpec(searchRes,1,&profile);

CMDisposeProfileSearch(searchRes);


status = setFSSpec(&profile);

return status;
}



CMError OpenFBProfileFSSpec(FSSpec spec, CMProfileRef *prof);
CMError OpenFBProfileFSSpec(FSSpec spec, CMProfileRef *prof)
{ 
	CMError cmerr; 
	CMProfileLocation profLoc;
	
	profLoc.locType = cmFileBasedProfile; 
	profLoc.u.fileLoc.spec = spec;
	cmerr = CMOpenProfile(prof, &profLoc);
	return cmerr; 
}

// get the colorsync profile refrence
McoStatus McoColorSync::getProfRef(CMProfileRef *pref)
{
CMError cmErr;
OSErr	iErr;
FSSpec fspec;
Boolean wasChanged;

	if (type == Mco_ICC_Null) 
		{
		*pref = NULL;
		return MCO_SUCCESS;
		}

	if (!validRef) 
		{
		if (type == Mco_ICC_File)
			{
			if(!_aliaslen || !_alias)	return MCO_FILE_OPEN_ERROR;
			iErr = ResolveAlias(NULL, (AliasHandle)_alias, &fspec, &wasChanged);	
			if(iErr != 0)	return MCO_FILE_OPEN_ERROR;	
			
			cmErr = OpenFBProfileFSSpec(fspec, &ref);
			if(cmErr != noErr) return 	MCO_FAILURE;			
			validRef = TRUE;
			}
		else if (type == Mco_ICC_System)
			{
			cmErr = CMGetSystemProfile(&ref);
			if(cmErr != noErr) return 	MCO_FAILURE;
			validRef = TRUE;
			}
		}
	
	*pref = ref;


	return MCO_SUCCESS;
}


// get the colorsync profile refrence
int16 McoColorSync::getDataSpace(void)
{
CMError cmErr;
OSErr	iErr;
FSSpec fspec;
Boolean wasChanged;


	if (validRef) return getProfileDataSpace(ref);

	if (type == Mco_ICC_Null) 
		{
		return cmNoSpace;
		}

	if (!validRef) 
		{
		if (type == Mco_ICC_File)
			{
			if(!_aliaslen || !_alias)	return MCO_FILE_OPEN_ERROR;
			iErr = ResolveAlias(NULL, (AliasHandle)_alias, &fspec, &wasChanged);	
			if(iErr != 0)	return MCO_FILE_OPEN_ERROR;	
			
			cmErr = OpenFBProfileFSSpec(fspec, &ref);
			if(cmErr != noErr) return 	MCO_FAILURE;			
			validRef = TRUE;
			}
		else if (type == Mco_ICC_System)
			{
			cmErr = CMGetSystemProfile(&ref);
			if(cmErr != noErr) return 	MCO_FAILURE;
			validRef = TRUE;
			}
		}
	
	
	return getProfileDataSpace(ref);

}


// close the colorsync refrence
McoStatus McoColorSync::closeRef(void)
{
if (validRef) CMCloseProfile(ref);
validRef = FALSE;
return MCO_SUCCESS;
}

// get the alias for a profile in the colorsync folder given a filename
McoStatus McoColorSync::GetProfile(unsigned char *name)
{
FSSpec	mySpec;
short	myVRef;	
long	myDirID;
CMError cmErr;
OSErr	iErr;

	// find the profile
	cmErr = CMGetColorSyncFolderSpec(kOnSystemDisk,kDontCreateFolder, 
		&myVRef, &myDirID);

 	if (cmErr == noErr)
 		cmErr = FSMakeFSSpec(myVRef, myDirID, name, &mySpec);
 	else  {return MCO_FILE_OPEN_ERROR;}

	
	if (cmErr == noErr)
		{
		// make the alias to the file
		iErr = NewAlias(nil, &mySpec, &_alias);		
		if(iErr != 0) return MCO_FILE_OPEN_ERROR;
		// set the alias
		_aliaslen = (**_alias).aliasSize;
		type = Mco_ICC_File;
		}
	else  {return MCO_FAILURE;}	
	
    return MCO_SUCCESS;
}		

McoStatus GetProfileList(int inputType,MCOICCType type, long *count, FSSpec *specs)
{
CMSearchRecord rec = {cmMatchAnyProfile,0,0,0,0,0,0,0,0,0};
CMProfileSearchRef searchRes;
unsigned long cnt;
int i;

switch (type) {
	case Mco_ICC_Scanner:
		switch (inputType) {
			case cmGrayASpace:
				rec.dataColorSpace = 'GRAY';
				rec.searchMask = 4;
				break;
			case cmLAB24Space:
				rec.dataColorSpace = 'LAB ';
				rec.searchMask = 4;
				break;				
			case cmRGB24Space:
				rec.dataColorSpace = 'RGB ';
				rec.searchMask = 4;
				break;
			case cmCMYK32Space:
				rec.dataColorSpace = 'CMYK';
				rec.searchMask = 4;
				break;
			}
		break;
	case Mco_ICC_Monitor:
		rec.profileClass = 'mntr';
		rec.dataColorSpace = 'RGB ';
		rec.searchMask = 6;
		break;
	case Mco_ICC_Printer:
		rec.profileClass = 'prtr';
		rec.searchMask = 2;
		break;
	}

CMNewProfileSearch(&rec,nil,&cnt,&searchRes);

*count = (long)cnt;

if (*count > MaxProfiles) *count = MaxProfiles;

for (i=1; i<=*count; i++) CMSearchGetIndProfileFileSpec(searchRes,i,&specs[i-1]);

CMDisposeProfileSearch(searchRes);
return MCO_SUCCESS;
}




