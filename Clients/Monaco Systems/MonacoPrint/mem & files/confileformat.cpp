#include "confileformat.h"


void ConFileFormat::setCurrentsize(int32 size)
{
	currentsize = size;
}

int32 ConFileFormat::getCurrentsize(void)
{
	return currentsize;
}

void ConFileFormat::setCurrentposition(int32 position)
{
	currentpos = position;
}

int32 ConFileFormat::getCurrentposition(void)
{
	return currentpos;
}

void ConFileFormat::setReadingsize(int32 size)
{
	readingsize = size;
}
	
int32 ConFileFormat::getReadingsize(void)
{
	return readingsize;
}

void ConFileFormat::setDatahandle(McoHandle ihandle)
{
	image_dataHandle = ihandle;
}

McoHandle ConFileFormat::getDatahandle(void)
{
	return image_dataHandle;
}

//should ok
void ConFileFormat::setDatasize(int32 datasize)
{
	data_size = datasize;
}

//should be ok
int32 ConFileFormat::getDatasize(void)
{
	return data_size;
}

McoStatus ConFileFormat::prepareRead(void)
{
	return MCO_FAILURE;
}
	
McoStatus ConFileFormat::startRead(McoHandle nouse1, int32 nouse2)
{
	return MCO_FAILURE;
}
	
McoStatus ConFileFormat::continueRead(int16 * nouse1)
{
	return MCO_FAILURE;
}
	
McoStatus ConFileFormat::prepareWrite(void)
{
	return MCO_FAILURE;
}
	
McoStatus ConFileFormat::startWrite(McoHandle nouse1, int32 nouse2)
{
	return MCO_FAILURE;
}
	
McoStatus ConFileFormat::continueWrite(int16 * nouse1)
{
	return MCO_FAILURE;
}

void	ConFileFormat::setReuse(Boolean f)
{
	reuseBuffer = f;
}
	
Boolean	ConFileFormat::getReuse(void)
{
	return reuseBuffer;
}
