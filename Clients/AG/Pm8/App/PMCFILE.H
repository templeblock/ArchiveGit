// pmcfile.h : definition of the CFileFile class
//

#ifndef __PMCFILE_H__
#define __PMCFILE_H__

#include "file.h"

FARCLASS CFileFile : public StorageDevice
{
private:
	CFile *m_pCFile;
public:
/* Virtual functions from StorageDevice. */

	CFileFile(CFile *pCFile = NULL) : m_pCFile(pCFile) {}

	void SetCFile(CFile* pCFile) { m_pCFile = pCFile; }
	
	virtual ERRORCODE initialize()
		{ return ERRORCODE_None; }
	virtual ERRORCODE read(VOIDPTR ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read = NULL);
	virtual ERRORCODE write(VOIDPTR ptr, ST_DEV_IO_SIZE size);
	virtual ERRORCODE seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode);
	virtual ERRORCODE tell(ST_DEV_POSITION far *position);
	virtual ERRORCODE length(ST_DEV_POSITION far *size);
	virtual ERRORCODE flush(void);
	virtual ERRORCODE truncate(void);
	virtual ERRORCODE io_limits(ST_DEV_POSITION limit_start = POSITION_UNKNOWN, ST_DEV_POSITION limit_end = POSITION_UNKNOWN)
		{ return ERRORCODE_None; }
};

#endif							// #ifndef __PMCFILE_H__

