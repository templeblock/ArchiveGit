#ifndef	_CON_FILE_FORMAT_H
#define _CON_FILE_FORMAT_H

#include "fileformat.h"

class ConFileFormat:public FileFormat{
private:
protected:

int32 data_size;
McoHandle image_dataHandle;
Boolean		reuseBuffer;


//add by peter for sequential read and write
int32		readingsize;
int32		currentsize;
int32		currentpos;

void		setCurrentsize(int32);
int32		getCurrentsize(void);
void		setCurrentposition(int32);
int32		getCurrentposition(void);
void		setReadingsize(int32);
int32		getReadingsize(void);
//end

//setFref is changed to be a public method
//void 		setFref(int32);
void 		setDir(short);
void 		setDatasize(int32);
int32 		getDatasize(void);
void 		setDatahandle(McoHandle);
McoHandle 	getDatahandle(void);


public:

//vitual functions should supported by all the subclasses
virtual	McoStatus	prepareRead(void);
virtual	McoStatus	startRead(McoHandle, int32);
virtual	McoStatus	continueRead(int16 *);
virtual	McoStatus	prepareWrite(void);
virtual	McoStatus	startWrite(McoHandle, int32);
virtual	McoStatus	continueWrite(int16 *);

virtual void	setReuse(Boolean f);
virtual Boolean	getReuse(void);

};

#endif