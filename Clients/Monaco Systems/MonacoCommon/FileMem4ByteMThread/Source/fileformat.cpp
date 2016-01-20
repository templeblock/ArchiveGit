#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileformat.h"

//should be ok
FileFormat::FileFormat(void)
 { 	
 	myError = 0;
 
	_swab.setSwab(FALSE);
 }
 
FileFormat::~FileFormat(void)
{
;
}

FileFormat::FileFormat(FILEIO *pBlock)
 { 	
 	myError = 0;
	
	//_fileio->open = pBlock->open;
/*
	_fileio->close = pBlock->close;
	_fileio->read = pBlock->read;
	_fileio->write = pBlock->write;
	_fileio->lseek = pBlock->lseek;
	_fileio->tell = pBlock->tell;
	_fileio->eof = pBlock->eof;
	_fileio->filelength = pBlock->filelength;
	_fileio->fh = pBlock->fh;
*/
	_fileio = pBlock;

	_swab.setSwab(FALSE);
}

void	FileFormat::setFileIo(FILEIO *pBlock)
{
	myError = 0;
	_fileio = pBlock;
}

//not implemented yet
McoStatus FileFormat::createFile(char* name, int16 script)
{
 	if(myError!=0)
 		return MCO_SYSTEM_ERROR;
 	 	 		
 	return MCO_SUCCESS;
}

//if the file is already open, close and open again
McoStatus FileFormat::openFile(char *name, long mode)
 {
	McoStatus status = MCO_SUCCESS;
	long yes;

/*
	yes = (long)_fileio->open(&_fileio, name, mode);
	if(yes == EOF)	status = MCO_FILE_OPEN_ERROR;
 	 	
 	return status;
*/
	return MCO_FAILURE;
 }

//should be ok 
McoStatus FileFormat::closeFile()
  {		
	_fileio->close(_fileio);	
	return MCO_SUCCESS;	  
  }

//should be ok 
McoStatus FileFormat::deleteFile()
  {
	return MCO_SUCCESS;	  
  }

//should be ok
McoStatus FileFormat::relRead(int32 numBytes, char* buffer)
 {
	int32 bytesRead = numBytes;
	
	bytesRead = _fileio->read(_fileio->fh, (void*)buffer,  numBytes);
	
	return MCO_SUCCESS;	
 }

//should be ok
McoStatus FileFormat::relWrite(int32 numBytes, char* buffer)
 {
 	int32 bytesRead = numBytes;
 		
	bytesRead = _fileio->write(_fileio->fh, (void*)buffer, numBytes);
	
	return MCO_SUCCESS;		 
 }

//should be ok
McoStatus FileFormat::setFilePosition(int32 mode, int32 offset)
 {
	McoStatus status = MCO_SUCCESS;
 	long where;
 	
 	where = _fileio->lseek(_fileio->fh, offset, mode);
 	if(where == -1L)	status = MCO_FILE_ERROR;
 
 	return status; 
 }

McoStatus FileFormat::getFilePosition(int32* offset)
 {
 	McoStatus status = MCO_SUCCESS;
 	long where;
 	
 	where = _fileio->tell(_fileio->fh);
 	if(where == EOF)	status = MCO_FILE_ERROR;
 	else
	*offset = where;
	
	return status;
 }

//not been tested yet
McoStatus FileFormat::setFilePositionend(int32 mode, int32 offset)
{
	McoStatus status = MCO_SUCCESS;
 	long where;
 	
 	where = _fileio->lseek(_fileio->fh, offset, mode);
 	if(where == EOF)	status = MCO_FILE_ERROR;
 
 	return status; 
}

//get file length	
McoStatus 	FileFormat::getFilelength(int32* length)
{ 
	*length = _fileio->filelength(_fileio->fh);
		
	return MCO_SUCCESS;	
}
	
