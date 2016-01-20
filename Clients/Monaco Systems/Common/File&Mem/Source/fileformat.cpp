#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileformat.h"

//should be ok
FileFormat::FileFormat(void)
 { 	
 	_error = MCO_SUCCESS;
	
	setFref(0); 
	_swab.setSwab(FALSE);
 }

FileFormat::~FileFormat(void)
{
;
}

int32 FileFormat::getFref()
 {
	return _file; 
 }

void FileFormat::setFref(int32 ref)
 {
	_file = ref; 
 }

McoStatus 	FileFormat::setFname(char* name)
{
	if(getFref() == 0)
		return MCO_FILE_NOT_OPEN;
		
	memcpy(_name, name, strlen(name));
	return MCO_SUCCESS;
}

McoStatus FileFormat::getFname(char* name)
{
#if defined(_WIN32)
	memcpy((char*)name, (char*)(_name), strlen(_name));
#else
	memcpy((char*)name, (char*)(_fsspec.name), 33);
	PtoCstr((unsigned char*)name);
#endif

	return MCO_SUCCESS;
}

McoStatus FileFormat::closeFile()
  {
	long ref;
	
	if( (ref = getFref()) == 0)
		return MCO_FILE_NOT_OPEN;
	
#if defined(_WIN32)
	close(ref);
#else
	FSClose((short)ref);
#endif
	
	setFref(0);
	
	return MCO_SUCCESS;	  
  }

McoStatus FileFormat::deleteFile()
  {
   	McoStatus status;
  	
  	if( (status = closeFile()) != MCO_SUCCESS)
		return status;	

#if defined(_WIN32)
	if(DeleteFile(_name) == 0)
		return MCO_FAILURE;
#else
	if(FSpDelete(&_fsspec) != noErr)
		return MCO_FAILURE;
#endif
			
	return MCO_SUCCESS;	  
  }

McoStatus FileFormat::relRead(int32 numBytes, char* buffer)
 {
	int32 bytesRead = numBytes;
	
	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;
	
	if(!numBytes)
		return MCO_SUCCESS;	// Nothing to read
		
#if defined(_WIN32)
	bytesRead = read(getFref(), buffer, numBytes);
	if(bytesRead == -1)
		return MCO_FILE_READ_ERROR;
#else
	if( FSRead((short)getFref(), &bytesRead, (char*)buffer) != noErr)
		return MCO_FILE_READ_ERROR;
#endif
	
	return MCO_SUCCESS;	
 }

McoStatus FileFormat::relWrite(int32 numBytes, char* buffer)
 {
 	int32 bytesWritten = numBytes;
	
	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;

	if(!numBytes)
		return MCO_SUCCESS;	// Nothing to read

#if defined(_WIN32)
	bytesWritten = write(getFref(), buffer, numBytes);
	if(bytesWritten == -1)
		return MCO_FILE_WRITE_ERROR;
#else	
	if( FSWrite((short)getFref(), &bytesWritten, (char*)buffer) != noErr)
		return MCO_FILE_WRITE_ERROR;
#endif	
	
	return MCO_SUCCESS;		 
 }

/*
McoStatus FileFormat::setFilePosition(int16 mode, int32 offset)
 {
  	int16 convert_mode[] = {1, 3, 2};
 	
 	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;
	
#if defined(_WIN32)
	if(lseek((getFref(), offset, mode) == -1L))
		return MCO_FILE_ERROR;
#else	
	if( SetFPos(getFref(), convert_mode[mode], offset) != noErr)
		return MCO_FILE_ERROR;
#endif	

	return MCO_SUCCESS;
 }
*/

McoStatus FileFormat::setFilePosition(int16 mode, int32 offset)
{
  
 	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;

#if defined(_WIN32)
	if(lseek(getFref(), offset, mode) == -1L)
		return MCO_FILE_ERROR;
#else	
	OSErr sysErr = noErr;
 	int32	filesize;
	int32	curpos;	//current file position
  	int16 convert_mode[] = {1, 3, 2};

	//get current file mark position
	if( GetFPos(getFref(), &curpos) != noErr)
		return MCO_FILE_ERROR;	

	//get current file length
	if(GetEOF(getFref(), &filesize) != noErr)
		return MCO_FILE_ERROR;

	switch(mode){
		case(SEEK_SET):
			if(filesize < offset) 
				sysErr = SetEOF(getFref(), offset);
			break;

		case(SEEK_END): //need to expand the file except 0
			sysErr = SetEOF(getFref(), (filesize+offset));
			break;

		case(SEEK_CUR):
			if( (curpos + offset) > filesize ) //need to expand the file
				sysErr = SetEOF(getFref(), (curpos+offset));
			break;

		defaults:
			return MCO_FILE_ERROR;
	}

	if(sysErr != noErr)
		return MCO_FILE_ERROR;
		
	if( SetFPos(getFref(), convert_mode[mode], offset) != noErr)
		return MCO_FILE_ERROR;
#endif

	return MCO_SUCCESS;
}

McoStatus FileFormat::getFilePosition(int32* offset)
 { 	
 	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;
		
#if defined(_WIN32)
	if( tell(getFref()) == -1L)
		return MCO_FILE_ERROR; 
#else	
	if(GetFPos(getFref(), offset) != noErr)
		return MCO_FILE_ERROR; 
#endif
	
	return MCO_SUCCESS;
 }

//get file length	
McoStatus 	FileFormat::getFilelength(int32* length)
{ 
 	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;

#if defined(_WIN32)
	if( filelength(getFref()) == 1L)
		return MCO_FILE_ERROR;
#else	
	if( GetEOF(getFref(), length) != noErr)
		return MCO_FILE_ERROR;
#endif
		
	return MCO_SUCCESS;	
}

McoStatus 	FileFormat::setFilelength(int32 length)
{ 
 	if(getFref()==0)
		return MCO_FILE_NOT_OPEN;

#if defined(_WIN32)
	if( setFilePosition(SEEK_SET, length) != MCO_SUCCESS)
		return MCO_FILE_ERROR;
#else	
	if( SetEOF(getFref(), length) != noErr)
		return MCO_FILE_ERROR;
#endif
		
	return MCO_SUCCESS;	
}


#if defined(_WIN32)
McoStatus FileFormat::createFile(char *name, long mode)
{
	long	fp;
	
	fp = open(name, (int)mode, _S_IREAD | _S_IWRITE);
	if(!fp)
		return MCO_FILE_OPEN_ERROR;
		
	setFref(fp);
	setFname(name);	
 	return MCO_SUCCESS;
}

McoBool FileFormat::doesFileExist(char *name)
 {
	long fp;

	fp = open(name, _O_BINARY | _O_RDONLY);
	if(fp == -1)
		return 	McoFalse;
	else{
		close(fp);		
		return McoTrue;
	}
 }

#else	

McoStatus FileFormat::createFile(FSSpec *fspec, int16 script)
 {
 	OSErr sysErr = noErr;
 	short ref;
 	
 	if(_error!=MCO_SUCCESS)
 		return MCO_SYSTEM_ERROR;
 	
 	if(doesFileExist(fspec)==McoFalse) {
  		sysErr = FSpCreate(fspec,'????', 'MCO1', script);
 		if(sysErr!=noErr)
 			return MCO_FILE_CREATE_ERROR; 
 	}
 	 		
 	sysErr = FSpOpenDF(fspec, fsCurPerm, &ref);
 	
 	if(sysErr!=noErr)
 		return MCO_FILE_OPEN_ERROR;
 	setFsspec(fspec);
 	
 	setFref(ref);
	
 	return MCO_SUCCESS;
 }

McoStatus FileFormat::createFilewithtype(FSSpec *fspec, int16 script, OSType creater, OSType type)
 {
 	OSErr sysErr = noErr;
 	short ref;
 	
 	if(_error!=MCO_SUCCESS)
 		return MCO_SYSTEM_ERROR;
 	
 	if(doesFileExist(fspec)==McoFalse) {
 		
 		sysErr = FSpCreate(fspec,creater, type, script);
 		if(sysErr!=noErr)
 			return MCO_FILE_CREATE_ERROR; 
 	}
 	 		
 	sysErr = FSpOpenDF(fspec, fsCurPerm, &ref);
 	
 	if(sysErr!=noErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 	setFref(ref);
 	 
 	setFsspec(fspec);
 	
 	return MCO_SUCCESS;
 }

 //change the file info
McoStatus FileFormat::setInfo(FSSpec *fspec,OSType creater, OSType type)
 {
 	FInfo	fndrInfo;
 	OSErr sysErr = noErr;
 	short ref;
 	
 	sysErr = FSpGetFInfo(fspec,&fndrInfo);
 	
 	if (sysErr == noErr)
 		{
 		fndrInfo.fdType = type;
 		fndrInfo.fdCreator = creater;
 		sysErr = FSpSetFInfo(fspec,&fndrInfo);
 		}
 	
 	if (sysErr == noErr) return MCO_SUCCESS;
 	return MCO_FILE_ERROR;
 }


//if the file is already open, close and open again
McoStatus FileFormat::openFile(FSSpec *fspec)
 {
 	OSErr sysErr = noErr;
 	short ref;
 	
 	if(_error!=MCO_SUCCESS)
 		return MCO_SYSTEM_ERROR;
 	
 	sysErr = FSpOpenDF(fspec, fsCurPerm, &ref);
 	
 	if(sysErr == opWrErr){
 		FSClose(ref);
		setFref(0);
	 	sysErr = FSpOpenDF(fspec, fsCurPerm, &ref);
	}
	
 	if(sysErr!=noErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 	setFref(ref);
 	 
 	setFsspec(fspec);
 	
 	return MCO_SUCCESS;
 }

McoBool FileFormat::doesFileExist(FSSpec *fspec)
 {
	OSErr sysErr = noErr;
	FInfo	finderInfo;
	
 	if(_error!= MCO_SUCCESS)
 		return McoFalse;
	
	sysErr = FSpGetFInfo(fspec, &finderInfo);
	
	if(sysErr==noErr)
		return McoTrue;
	else
		return McoFalse;
 }

void FileFormat::setFsspec(FSSpec * fspec)
{
	_fsspec.vRefNum = fspec->vRefNum;
	_fsspec.parID   = fspec->parID;
	memcpy((char*)(_fsspec.name), (char*)(fspec->name), 64);
}	

McoStatus FileFormat::set_default_dir(short volnum, long dirid)
{
	OSErr err;
	WDPBRec	wdpb;	/* for PBHSetVol and PBOpenWD */
	
	/* iErr = SetVol(volName,vRefNum); */

	wdpb.ioNamePtr = NULL;
	wdpb.ioVRefNum = volnum;
	wdpb.ioWDDirID = dirid;	/* not using a "hard" directory ID */
	err = PBHSetVol( &wdpb, FALSE );
	
	if( err != noErr )
		return MCO_FAILURE;
		
	return MCO_SUCCESS;	
}
	
//Note: currently only find one match file	
McoStatus FileFormat::openFilefromPref(FSSpec *fspec, char* name,char* dirname)
{
 	short	ref;
	FSSpec	mySpec;
	short	myVRef;	
	long	myDirID;
 	OSErr sysErr = noErr;
 	OSErr	myErr;
 	int		myCount;
 	Str255	myFName;
 	CSParam	myPB;
 	HFileInfo	myCPB;
 	CInfoPBRec	mySpec1;
 	CInfoPBRec	mySpec2;
 	char	myBuffer[4096];
 	char tempname[60];

 	 	
 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);
 		
  	myCPB.ioFDirIndex = 0;
 	myCPB.ioDirID = myDirID;
 	strcpy(tempname,dirname);
 	myCPB.ioNamePtr = CtoPstr(tempname);
 	myCPB.ioVRefNum = myVRef;
 	
 	myErr = PBGetCatInfo((CInfoPBPtr)&myCPB, false);
 
 	sysErr = MCO_FILE_OPEN_ERROR;
 	if (myErr == noErr) {
 		if(((myCPB.ioFlAttrib >> 4) & 0x01) == 1) sysErr = 0;
 	}
  	
	strcpy(tempname,name);
 	
 	myPB.ioCompletion = NULL;
 	myPB.ioNamePtr = NULL;
 	myPB.ioVRefNum = myVRef;
 	myPB.ioMatchPtr = fspec;
 	myPB.ioReqMatchCount = 1;
//ATTN! change fsSBPartialName to fsSBFullName for searching
 	myPB.ioSearchBits = fsSBFullName + fsSBFlParID;
 	myPB.ioSearchInfo1 = &mySpec1;
 	myPB.ioSearchInfo2 = &mySpec2;
 	myPB.ioSearchTime = 0;
 	myPB.ioCatPosition.initialize = 0;
 	myPB.ioOptBuffer = myBuffer;
 	myPB.ioOptBufSize = 4096;	//Cache size
 	
 	mySpec1.hFileInfo.ioNamePtr = CtoPstr(tempname);
 	mySpec1.hFileInfo.ioFlAttrib = 0;
 	mySpec1.hFileInfo.ioFlParID = myCPB.ioDirID;
 	
 	mySpec2.hFileInfo.ioNamePtr = NULL;
 	mySpec2.hFileInfo.ioFlAttrib = 0;
 	mySpec2.hFileInfo.ioFlParID = myCPB.ioDirID;
 	
 	myErr = PBCatSearch(&myPB,0);	
 	
 	if ((myErr != noErr) && (myErr != eofErr))
 		return MCO_FILE_OPEN_ERROR;
 		
 	if (myPB.ioActMatchCount < 1) 
 		return MCO_FILE_OPEN_ERROR;
 	
 	sysErr = FSpOpenDF(fspec, fsCurPerm, &ref);
 	
 	if(sysErr!=noErr)
 		return MCO_FILE_OPEN_ERROR;
 	
 	setFref(ref);
 	setFsspec(fspec);
 	
 	return MCO_SUCCESS;
} 

//Note: currently only find one match file, added on 9/6	
McoStatus FileFormat::findFilefromPref(FSSpec *fspec, char* name,char* dirname)
{
 	short	ref;
	FSSpec	mySpec;
	short	myVRef;	
	long	myDirID;
 	OSErr sysErr = noErr;
 	OSErr	myErr;
 	int		myCount;
 	Str255	myFName;
 	CSParam	myPB;
 	HFileInfo	myCPB;
 	CInfoPBRec	mySpec1;
 	CInfoPBRec	mySpec2;
 	char	myBuffer[4096];
 	char tempname[60];

 	 	
 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);
 		
  	myCPB.ioFDirIndex = 0;
 	myCPB.ioDirID = myDirID;
 	strcpy(tempname,dirname);
 	myCPB.ioNamePtr = CtoPstr(tempname);
 	myCPB.ioVRefNum = myVRef;
 	
 	myErr = PBGetCatInfo((CInfoPBPtr)&myCPB, false);
 
 	sysErr = MCO_FILE_OPEN_ERROR;
 	if (myErr == noErr) {
 		if(((myCPB.ioFlAttrib >> 4) & 0x01) == 1) sysErr = 0;
 	}
  	
	strcpy(tempname,name);
 	
 	myPB.ioCompletion = NULL;
 	myPB.ioNamePtr = NULL;
 	myPB.ioVRefNum = myVRef;
 	myPB.ioMatchPtr = fspec;
 	myPB.ioReqMatchCount = 1;
//ATTN! change fsSBPartialName to fsSBFullName for searching
 	myPB.ioSearchBits = fsSBFullName + fsSBFlParID;
 	myPB.ioSearchInfo1 = &mySpec1;
 	myPB.ioSearchInfo2 = &mySpec2;
 	myPB.ioSearchTime = 0;
 	myPB.ioCatPosition.initialize = 0;
 	myPB.ioOptBuffer = myBuffer;
 	myPB.ioOptBufSize = 4096;	//Cache size
 	
 	mySpec1.hFileInfo.ioNamePtr = CtoPstr(tempname);
 	mySpec1.hFileInfo.ioFlAttrib = 0;
 	mySpec1.hFileInfo.ioFlParID = myCPB.ioDirID;
 	
 	mySpec2.hFileInfo.ioNamePtr = NULL;
 	mySpec2.hFileInfo.ioFlAttrib = 0;
 	mySpec2.hFileInfo.ioFlParID = myCPB.ioDirID;
 	
 	myErr = PBCatSearch(&myPB,0);	
 	
 	if ((myErr != noErr) && (myErr != eofErr))
 		return MCO_FILE_OPEN_ERROR;
 		
 	if (myPB.ioActMatchCount < 1) 
 		return MCO_FILE_OPEN_ERROR;
 	 	
 	return MCO_SUCCESS;
} 

//Note: currently only find one match file	
McoStatus FileFormat::createFileinPref(FSSpec *fspec, char* name,char* dirname)
{
	short	myVRef;	
	long	myDirID;
 	OSErr sysErr = noErr;
 	short ref;
 	char tempname[80];
 	char tempdir[80];
 	FSSpec	dirspec;
 	
 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);
	if (strlen(dirname) >0){
 		strcpy(tempname,":");
 		strcat(tempname,dirname);
 		strcat(tempname,":");
 		strcat(tempname,name);
 	}
 	else strcpy(tempname,name);
 	
 	if (sysErr == noErr)
 		sysErr = FSMakeFSSpec(myVRef, myDirID, CtoPstr(tempname), fspec);
 	else return MCO_FILE_CREATE_ERROR;

	if (sysErr == dirNFErr){
		long	createdDirID;
		//try to create a directory first
		strcpy(tempdir, dirname);
 		sysErr = FSMakeFSSpec(myVRef, myDirID, CtoPstr(tempdir), &dirspec);
 		if (sysErr != noErr && sysErr != fnfErr)
 			return  MCO_FILE_CREATE_ERROR;
 		sysErr = FSpDirCreate(&dirspec,smSystemScript,&createdDirID);
 		if(sysErr != noErr)
 			return MCO_FILE_CREATE_ERROR;
 		 sysErr = FSMakeFSSpec(myVRef, myDirID, (StringPtr)tempname, fspec);
 	}		

 	if (sysErr != noErr && sysErr != fnfErr)
 		return  MCO_FILE_CREATE_ERROR;
 	
 	if(sysErr == noErr)
 		sysErr = FSpDelete(fspec);

	return (createFile(fspec,smSystemScript));
}
		
#endif
