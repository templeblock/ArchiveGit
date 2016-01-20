
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1993 Monaco Systems Inc.
//
//	mcofile.cp
//
//	Defines McoFile File IO class methods
//
//	Create Date:	8/14/93
//
//	Revision History:
//	Special and preference code added by James Vogh
//
////////////////////////////////////////////////////////////////////////////////

#include "mcofile.h"
#include "Files.h"
#include "Folders.h"
#include "Script.h"
#include <string.h>
#include "think_bugs.h"

#include "pascal_string.h"

McoAddNew(int32 s,int32 code)
{
}

McoRemoveNew(int32 s,int32 code)
{
}


McoFile::McoFile(void)
 {
 	unsigned char volName[32];
 	
 	myError = noErr;
	
	myError = GetVol(volName, &workDir);
	
	myRef = 0; 
 }
 


McoStatus McoFile::openFile(unsigned char* name)
 {
 	OSErr sysErr = noErr;
 	short ref;
 	char tempName[MAX_FILE_LENGTH];
 	
 	if(myError!=noErr)
 		return MCO_SYSTEM_ERROR;
 	
 	if(doesFileExist(name)==McoFalse) {
 		// Create the file
 		
 		#ifndef __MOTO__
 		sysErr = Create(name ,workDir, '????', 'MCO1');
		#else 
		sysErr = Create(name ,workDir, '???\?', 'MCO1');
		#endif
 		if(sysErr!=noErr)
 			return MCO_FILE_CREATE_ERROR; 
 	}
 	
 	// Now open the file
 	
 		
 	sysErr = FSOpen(name, workDir, &ref);
 	
 	if(sysErr!=noErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 	setFref(ref);
 	
 	return MCO_SUCCESS;
 }
 
McoStatus McoFile::openFSSpec(FSSpec *fsspec,Boolean Delete, OSType creator,OSType type)
{
OSErr sysErr = noErr;
short ref;
FInfo	fndrInfo;

sysErr = FSpGetFInfo(fsspec,&fndrInfo);

if (sysErr == fnfErr) sysErr = FSpCreate(fsspec,creator,type,smSystemScript);
else if (Delete) 
	{
	sysErr = FSpDelete(fsspec);
	if(sysErr!=noErr)
 		return MCO_FILE_CREATE_ERROR;
	sysErr = FSpCreate(fsspec,creator,type,smSystemScript);
	}

if(sysErr!=noErr)
 		return MCO_FILE_CREATE_ERROR;

sysErr = FSpOpenDF(fsspec,fsCurPerm,&ref);
 				
if(sysErr!=noErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 setFref(ref);
 return MCO_SUCCESS;

}

McoStatus McoFile::openExistingFile(unsigned char* name)
 {
 	OSErr sysErr = noErr;
 	short ref;
 	char tempName[MAX_DIR_LENGTH];
 	
 	if(myError!=noErr)
 		return MCO_SYSTEM_ERROR;
 
 	
 	sysErr = FSOpen(name, workDir, &ref);
 	
 	if(sysErr!=noErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 	setFref(ref);
 	
 	return MCO_SUCCESS;
 }

//Added by James Vogh

McoStatus McoFile::openFileSpecial(unsigned char* name,unsigned char* dirname,Boolean Delete, OSType creater,OSType type)
{
 	OSErr sysErr = noErr;
 	short ref;
	Str255 tempname;
 	
 	if(myError!=noErr)
 		return MCO_SYSTEM_ERROR;

 	if (dirname[0] >0)
		{
		cat_str(tempname,"\p:");
		cat_str(tempname,dirname);
		cat_str(tempname,"\p:");
 		cat_str(tempname,name);
		}
 	else copy_str(tempname,name); 	

 	
 	if(doesFileExist(tempname)==McoFalse) {
 		// Create the file
 		 		
 		sysErr = Create(tempname ,workDir, creater, type);
 		if(sysErr!=noErr)
 			return MCO_FILE_CREATE_ERROR; 
 	}
	else if (Delete)
		{
		sysErr = FSDelete(tempname, workDir);
		if(sysErr!=noErr)
 			return MCO_FILE_ERROR; 
 		sysErr = Create(tempname ,workDir, creater, type);
 		if(sysErr!=noErr)
 			return MCO_FILE_CREATE_ERROR; 
		}
 	
 	// Now open the file
 	
 
 		
 	sysErr = FSOpen(tempname, workDir, &ref);
 	
 	if(sysErr!=noErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 	setFref(ref);
 	
 	return MCO_SUCCESS;
 }
 
 // Added by James Vogh

McoStatus McoFile::openExistingFileSpecial(unsigned char* name,unsigned char* dirname)
 {
 	OSErr sysErr = noErr;
 	short ref;
 	Str255 tempname;
 	
 	if(myError!=noErr)
 		return MCO_SYSTEM_ERROR;

 	if (dirname[0] >0)
		{
		cat_str(tempname,"\p:");
		cat_str(tempname,dirname);
		cat_str(tempname,"\p:");
 		cat_str(tempname,name);
		}
 	else copy_str(tempname,name); 	

 	
 	sysErr = FSOpen(tempname, workDir, &ref);
 	
 	if(sysErr!=noErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 	setFref(ref);
 	
 	return MCO_SUCCESS;
 }

//Added by James Vogh

McoStatus McoFile::openFileParam(unsigned char* name,unsigned char* dirname,Boolean Delete, OSType creater,OSType type)
{
	FSSpec	mySpec;
	short	myVRef;	
	long	myDirID;


 	OSErr sysErr = noErr;
 	short ref;
 	unsigned char *tempname;

	tempname = new unsigned char[256];
	McoAddNew(256,300);
	tempname[0]=0;
 //	McoErrorAlert(1);
 	
 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);
 	
// 	McoErrorAlert(2);
 	if (dirname[0] >0)
		{
		cat_str(tempname,"\p:");
		cat_str(tempname,dirname);
		cat_str(tempname,"\p:");
 		cat_str(tempname,name);
		}
 	else copy_str(tempname,name);

 	
 	if (sysErr == noErr)
 		sysErr = FSMakeFSSpec(myVRef, myDirID, tempname, &mySpec);
 	else  {delete tempname; McoRemoveNew(256,115);  return (McoStatus) sysErr;}

	delete tempname;
	McoRemoveNew(256,116);

 	if (sysErr == fnfErr)
 		sysErr = FSpCreate(&mySpec, creater, type, smSystemScript);
 	else if (Delete)
		{
		sysErr = FSpDelete(&mySpec);
		if(sysErr!=noErr)
 			return MCO_FILE_CREATE_ERROR;
		sysErr = FSpCreate(&mySpec, creater, type, smSystemScript);
		}
 	if (sysErr == noErr)
 		{
 		sysErr = FSpOpenDF(&mySpec,fsCurPerm,&ref);
 				
 		if(sysErr!=noErr)
 			return MCO_FILE_OPEN_ERROR;
 	
 		setFref(ref);
 		return MCO_SUCCESS;
 		}
 	return MCO_FILE_OPEN_ERROR;

 }


McoStatus McoFile::getFssFileParam(unsigned char* name,unsigned char* dirname,FSSpec	*mySpec)
{

	short	myVRef;	
	long	myDirID;


 	OSErr sysErr = noErr;
 	short ref;
 	unsigned char *tempname;

	tempname = new unsigned char[256];
	McoAddNew(256,300);
	tempname[0]=0;
 //	McoErrorAlert(1);
 	
 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);
 	
// 	McoErrorAlert(2);
 	if (dirname[0] >0)
		{
		cat_str(tempname,"\p:");
		cat_str(tempname,dirname);
		cat_str(tempname,"\p:");
 		cat_str(tempname,name);
		}
 	else copy_str(tempname,name);

 	
 	if (sysErr == noErr)
 		sysErr = FSMakeFSSpec(myVRef, myDirID, tempname, mySpec);
 	else  {delete tempname; McoRemoveNew(256,115);  return (McoStatus) sysErr;}

	delete tempname;
	McoRemoveNew(256,116);

 	if (sysErr == noErr) return MCO_SUCCESS;
	return (McoStatus) sysErr;
 }


// Added by James Vogh

McoStatus McoFile::openExistingFileParam(unsigned char* name,unsigned char* dirname)
 {
	FSSpec	mySpec;
	short	myVRef;	
	long	myDirID;


 	OSErr sysErr = noErr;
 	short ref;

 	unsigned char *tempname;

	tempname = new unsigned char[256];
	McoAddNew(256,43);
	tempname[0]=0;
// 	McoErrorAlert(1);
 	
 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);
 			
// 	McoErrorAlert(2);

	if (dirname[0] >0)
		{
		cat_str(tempname,"\p:");
		cat_str(tempname,dirname);
		cat_str(tempname,"\p:");
 		cat_str(tempname,name);
		}
 	else copy_str(tempname,name);
 	
 	if (sysErr == noErr)
 		sysErr = FSMakeFSSpec(myVRef, myDirID, tempname, &mySpec);
 	else   { delete tempname; McoRemoveNew(sizeof(tempname),117); return (McoStatus)sysErr;}
	delete tempname;
	McoRemoveNew(256,118);
 	
 	if (sysErr == fnfErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 	if (sysErr == noErr)
 		{
 		FSpOpenDF(&mySpec,fsCurPerm,&ref);
 				
 		if(sysErr!=noErr)
 			return MCO_FILE_OPEN_ERROR;
 	
 		setFref(ref);
 		return MCO_SUCCESS;
 		}
 	return MCO_FILE_OPEN_ERROR;
 	
 }


// Added by James Vogh
/*
#define MaxMatches 30	//up to 30 matches per search
#define OptBufferSize	$2000	//use 8K cache for speed

McoStatus McoFile::openExistingFileParamSearch(char* name,char* dirname)
 {
 
	FSSpec	mySpec;
	short	myVRef;	
	long	myDirID;
 	OSErr sysErr = noErr;
 	OSErr	myErr;
 	int		myCount;
 	Str255	myFName;
 	HParamBlockRec	myPB;
 	FSSpec	myMatches[MaxMatches];
 	CInfoPBRec	mySpec1;
 	CInfoPBRec	mySpec2;
 	char	myBuffer[OptBufferSize];
 	short ref;
 	
 	char tempname[MAX_DIR_LENGTH];
 	
 	
 	
// 	McoErrorAlert(1);
 	
 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);
 		
 	
 			
 	
// 	McoErrorAlert(2);
 	
 	 if (strlen(dirname) >0)
 		{
 		strcpy(tempname,":");
 		strcat(tempname,dirname);
 		strcat(tempname,":");
 		strcat(tempname,name);
 		}
 	else strcpy(tempname,name);
 	
 	
 	myPB.ioCompletion = 0L;
 	myPB.ioNamePtr = 0L;
 	myPB.ioVRefNum = myVRef;
 	myPB.ioMatchPtr = myMatches;
 	myPB.ioReqMatchCount = MaxMatches;
 	myPB.SearchBits = fsSBPartialName + fsSBFlParID;
 	myPB.ioSearchInfo1 = &mySpec1;
 	myPB.ioSearchInfo2 = &mySpec2;
 	myPB.ioSearchTime = 0;
 	myPB.ioCatPosition.initialize = 0;
 	myPB.ioOptBuffer = myBuffer;
 	myPB.ioOptBufSize = OptBufferSize;
 	
 	mySpec1.ioNamePtr = tempname;
 	mySpec1.ioFlAttrib = $00;
 	mySpec1.ioFlParID = myDirID;
 	
 	mySpec2.ioNamePtr = 0L;
 	mySpec2.ioFlAttrib = $00;
 	mySpec2.ioFlParID = myDirID;
 	
 	myErr = PBCatSearch(&myPB,0);
 	
// 	if (sysErr == noErr)
// 		sysErr = FSMakeFSSpec(myVRef, myDirID, ctopstr(tempname), &mySpec);
// 	else return sysErr;

	
 	
 	if (sysErr == fnfErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 	if (sysErr == noErr)
 		{
 		FSpOpenDF(myMatches[0],fsCurPerm,&ref);
 				
 		if(sysErr!=noErr)
 			return MCO_FILE_OPEN_ERROR;
 	
 		setFref(ref);
 		return MCO_SUCCESS;
 		}
 	return MCO_FILE_OPEN_ERROR;
 	
 } */

 
// Close the file

McoStatus McoFile::closeFile()
  {
	short ref;
	
	ref = (short)getFref();
	
	if(ref==0)
		return MCO_FILE_NOT_OPEN;
	
	FSClose(ref);
	
	setFref(0);	  
	
	return MCO_SUCCESS;	
  }

// file size

McoStatus McoFile::fileSize(int32 *filesize)
{
OSErr sysErr = noErr;

	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;

	sysErr = GetEOF(getFref(),filesize);

	if(sysErr!=noErr)
		return MCO_FILE_READ_ERROR;
	
	return MCO_SUCCESS;	
}

McoStatus McoFile::relRead(int32 numBytes, void* buffer)
 {
	int32 bytesRead = numBytes;
	OSErr sysErr = noErr;
	
	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;
	
	if(!numBytes)
		return MCO_SUCCESS;	// Nothing to read
		
	sysErr = FSRead((short)getFref(), &bytesRead, (char*)buffer);
		
	if(sysErr!=noErr)
		return MCO_FILE_READ_ERROR;
	
	return MCO_SUCCESS;	
 }

McoStatus McoFile::relWrite(int32 numBytes, void* buffer)
 {
 	int32 bytesWritten = numBytes;
	OSErr sysErr = noErr;
	
	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;
	
	sysErr = FSWrite((short)getFref(), &bytesWritten, (char*)buffer);
	
	if(sysErr!=noErr)
		return MCO_FILE_WRITE_ERROR;
	
	if(bytesWritten!=numBytes)
		return MCO_FILE_WRITE_ERROR;
	
	return MCO_SUCCESS;		 
 }
 
int32 McoFile::getFref()
 {
	return myRef; 
 }

void McoFile::setFref(int32 ref)
 {
	myRef = ref; 
 }

McoBool McoFile::doesFileExist(unsigned char* name)
 {
	OSErr sysErr = noErr;
	FInfo	finderInfo;
	
 	if(myError!=noErr)
 		return McoFalse;
	
	sysErr = GetFInfo(name, workDir, &finderInfo);
	
	if(sysErr==noErr)
		return McoTrue;
	else
		return McoFalse;
 }

McoStatus McoFile::setFilePosition(int16 mode, int32 offset)
 {
 	OSErr sysErr = noErr;
 	
 	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;
	
	sysErr = SetFPos(getFref(), mode, offset);

	// Translate some of the errors
	switch(sysErr) {
		case eofErr:
			return MCO_FILE_EOF_ERROR;
		case posErr:
			return MCO_FILE_BOF_ERROR;
		case noErr:
			return MCO_SUCCESS;
		default:
			return MCO_FILE_ERROR;
	}
 
 }

McoStatus McoReadFile(unsigned char* name, int32 numBytes, void* buffer, int32 offset)
 {
 	McoFile *fileObject;
 	McoStatus status;
 	
 	fileObject = new McoFile;
	McoAddNew(sizeof(McoFile),44);
	if(fileObject==0L)
		return MCO_FILE_OPEN_ERROR;
	
	status = fileObject->openFile(name);
	if(status!=MCO_SUCCESS) {
		delete fileObject;
		McoRemoveNew(sizeof(fileObject),119);
		return status;
	}
	
	status = fileObject->setFilePosition(1, offset);
	if(status!=MCO_SUCCESS) {
		delete fileObject;
		McoRemoveNew(sizeof(fileObject),120);
		return status;
	}
	
	status = fileObject->relRead(numBytes, buffer); 
	if(status!=MCO_SUCCESS) {
		delete fileObject;
		McoRemoveNew(sizeof(fileObject),121);
		return status;
	}
	
	fileObject->closeFile();
	
	delete fileObject;
	McoRemoveNew(sizeof(fileObject),122);
	
	return MCO_SUCCESS;
 }

McoStatus McoWriteExistingFile(char* name, int32 numBytes, void* buffer, int32 offset)
 {
 	McoFile *fileObject;
 	McoStatus status = MCO_SUCCESS;
 	
 	fileObject = new McoFile;
	McoAddNew(sizeof(McoFile),45);
	if(fileObject==0L)
		return MCO_FILE_OPEN_ERROR;
	
//	status = fileObject->openExistingFile(name);
	if(status!=MCO_SUCCESS) {
		delete fileObject;
		McoRemoveNew(sizeof(fileObject),123);
		return status;
	}
	
	status = fileObject->setFilePosition(1, offset);
	if(status!=MCO_SUCCESS) {
		delete fileObject;
		McoRemoveNew(sizeof(fileObject),124);
		return status;
	}
	
	status = fileObject->relWrite(numBytes, buffer); 
	if(status!=MCO_SUCCESS) {
		delete fileObject;
		McoRemoveNew(sizeof(fileObject),125);
		return status;
	}
	
	fileObject->closeFile();
	
	delete fileObject;
	McoRemoveNew(sizeof(fileObject),126);
	
	return MCO_SUCCESS;
 
 }

McoStatus McoWriteFile(unsigned char* name, int32 numBytes, void* buffer)
 {
	McoFile *fileObject;
 	McoStatus status;
 	
 	fileObject = new McoFile;
	if(fileObject==0L)
		return MCO_FILE_OPEN_ERROR;
	McoAddNew(sizeof(McoFile),46);
	
	status = fileObject->openFile(name);
	if(status!=MCO_SUCCESS) {
		delete fileObject;
		McoRemoveNew(sizeof(fileObject),127);
		return status;
	}
	
	status = fileObject->relWrite(numBytes, buffer); 
	if(status!=MCO_SUCCESS) {
		delete fileObject;
		McoRemoveNew(sizeof(fileObject),128);
		return status;
	}
	
	fileObject->closeFile();
	
	delete fileObject;
	McoRemoveNew(sizeof(fileObject),129);
	
	return MCO_SUCCESS; 
 }
 
 /* A modification added by James Vogh */
 //This function is used to set up McoFile if a file is already open 
 McoStatus McoFile::setRef(int32 tempref)
 {
 myRef = tempref;
 return MCO_SUCCESS;
 }
 
  McoStatus McoFile::setWD(short wd)
 {
 workDir = wd;
 return MCO_SUCCESS;
 }