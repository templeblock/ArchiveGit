#ifndef MCO_FILE
#define MCO_FILE
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1993 Monaco Systems Inc.
//
//	mcofile.h
//
//	Defines McoFile File IO class
//
//	Create Date:	8/14/93
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////

#include "mcotypes.h"
#include "mcostat.h"

class McoFile {
private:

int32 myRef;

void setFref(int32);

short workDir;

OSErr myError;

McoBool doesFileExist(unsigned char* name);

protected:
public:

// Initialize data members
McoFile(void);

// Open/create a file
McoStatus openFile(unsigned char* name);

//Open a file using the FSSpec descriptor.  Create the file if neccasry
McoStatus openFSSpec(FSSpec *fsspec, Boolean Delete, OSType creater,OSType type);

// Open an existing file
McoStatus openExistingFile(unsigned char* name);

// Open/create a file with specific creater and type and a directory

McoStatus openFileSpecial(unsigned char* name,unsigned char* dirname, Boolean Delete, OSType creater,OSType type);

// Open a file which exist inside of a directory

McoStatus openExistingFileSpecial(unsigned char* name,unsigned char *dirname);

// Open/create a file with specific creater and type and a directory which is inside of
// the parameter directory.

McoStatus openFileParam(unsigned char* name,unsigned char* dirname, Boolean Delete, OSType creater,OSType type);

// get the FSSpec of a file that is located in the prefrences
McoStatus getFssFileParam(unsigned char* name,unsigned char* dirname,FSSpec	*mySpec);


// Open a file which exist inside of a directory which is inside of the parameter 
// directory.

McoStatus openExistingFileParam(unsigned char* name,unsigned  char *dirname);


// Close the file
McoStatus closeFile();

// file size
McoStatus fileSize(int32 *filesize);

// Read from current position in file. Move position pointer.
McoStatus relRead(int32 numBytes, void* buffer);

// Write to current position in file. Move position pointer.
McoStatus relWrite(int32 numBytes, void* buffer);

int32 getFref();

// Set the position in the file where the next read/write will occur.
// mode controls meaning of offset. 
// mode=1 means from start
// mode=2 means from end
// mode=3 means from current position
// offset is the file offset in bytes, depending on parameter mode
McoStatus setFilePosition(int16 mode, int32 offset);

McoStatus setRef(int32 tempref);

McoStatus setWD(short wd);


};

// Open a file. Go to offset. Read numBytes into buffer. Close file.
McoStatus McoReadFile(unsigned char* name, int32 numBytes, void* buffer, int32 offset);

// Open an existing file. Go to offset. Write numBytes to buffer. Close file.
McoStatus McoWriteExistingFile(char* name, int32 numBytes, void* buffer, int32 offset);

// Open/create a file. Write numBytes to buffer. Close file.
McoStatus McoWriteFile(unsigned char* name, int32 numBytes, void* buffer);

#endif
