/**************************/
//
// Revision: add virtual functions on 5/26/94
// Revision: 7/10/07
//	
/**************************/

#ifndef FILE_FORMAT_H
#define FILE_FORMAT_H

#include <string.h>
#include <stdlib.h>
#include "mcotypes.h"
#include "mcostat.h"
#include "swab.h"

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <windows.h>
#endif

class FileFormat {
private:

protected:

McoStatus 	_error;
Swab		_swab;
char		_name[256];

int32 		_file;

#if defined(_WIN32)

#else	
	FSSpec 		_fsspec;
#endif

public:
FileFormat(void);
~FileFormat(void);

void 		setFref(int32);
int32 		getFref(void);
McoStatus 	setFname(char* name);
McoStatus	getFname(char* name);

McoStatus 	closeFile();
McoStatus	deleteFile();
McoStatus 	relRead(int32 numBytes, char* buffer);
McoStatus 	relWrite(int32 numBytes, char* buffer);
McoStatus 	setFilePosition(int16 mode, int32 offset);
McoStatus 	getFilePosition(int32*);
McoStatus 	getFilelength(int32* length);
McoStatus	setFilelength(int32 length);


#if defined(_WIN32)
McoStatus 	createFile(char *name, long mode);
McoBool 	doesFileExist(char* name);

#else
McoStatus 	createFile(FSSpec *, int16);
McoStatus	createFilewithtype(FSSpec *, int16, OSType, OSType);
 //change the file info
McoStatus 	setInfo(FSSpec *fspec,OSType creater, OSType type);
McoStatus 	openFile(FSSpec *);
McoBool 	doesFileExist(FSSpec *);

void 		setFsspec(FSSpec *);
McoStatus   set_default_dir(short, long);

McoStatus	openFilefromPref(FSSpec *fspec, char* name,char* dirname);
McoStatus	createFileinPref(FSSpec *fspec, char* name,char* dirname);
McoStatus	findFilefromPref(FSSpec *fspec, char* name,char* dirname);
#endif

};

#endif //FILE_FORMAT_H
