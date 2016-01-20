#ifndef MCO_FILE_FORMAT_H
#define MCO_FILE_FORMAT_H

#include "mcotypes.h"
#include "mcostat.h"
#include "mlfileio.h"
#include "swab.h"

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <windows.h>
#endif

class FileFormat {
private:
FILEIO	*_fileio;

protected:
long myError;

Swab	_swab;

public:
FileFormat(void);
FileFormat::FileFormat(FILEIO *pBlock);
~FileFormat(void);

void		setFileIo(FILEIO *pBlock);
McoStatus 	openFile(char *name, long mode);
McoStatus 	createFile(char *name, int16);
McoStatus 	closeFile();
McoStatus	deleteFile();
McoStatus 	relRead(int32 numBytes, char* buffer);
McoStatus 	relWrite(int32 numBytes, char* buffer);
McoStatus 	setFilePosition(int32 mode, int32 offset);
McoStatus 	getFilePosition(int32*);
McoStatus	setFilePositionend(int32 mode, int32 offset);
McoStatus 	getFilelength(int32* length);

McoStatus 	openExistingFile(unsigned char* name);

#if defined(_WIN32)
McoBool 	doesFileExist(char* name){ return 	McoFalse; }
#else
McoBool 	doesFileExist(FSSpec *){ return 	McoFalse;}
#endif
McoStatus	getFname(char* name){ return MCO_FAILURE;}
McoStatus	setFilelength(int32 length){ return MCO_FAILURE;}

};

#endif //FILE_FORMAT_H
