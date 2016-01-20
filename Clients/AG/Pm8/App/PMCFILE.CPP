// pmcfile.cpp : implementation of the CFileFile class
//

#include "stdafx.h"
#include "misc.h"
#include "pmcfile.h"

/*
// Read data from a file.
*/

ERRORCODE CFileFile::read(VOIDPTR ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read)
{
	ST_DEV_IO_SIZE amount_read = 0;

	TRY
	{
		amount_read = m_pCFile->Read(ptr, size);
	}
	CATCH (CFileException, e)
	{
	}
	END_CATCH

	if (size_read != NULL)
	{
	/* Return the amount actually read with no error reported. */
		*size_read = amount_read;
		return ERRORCODE_None;
	}

	return amount_read == size ? ERRORCODE_None : ERRORCODE_Read;
}

/*
// Write data to the file.
*/

ERRORCODE CFileFile::write(VOIDPTR ptr, ST_DEV_IO_SIZE size)
{
	TRY
	{
		m_pCFile->Write(ptr, size);
	}
	CATCH (CFileException, e)
	{
		return ERRORCODE_Write;
	}
	END_CATCH

	return ERRORCODE_None;
}

/*
// Seek to a position in the file.
*/

ERRORCODE CFileFile::seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode)
{
	static UINT mode_trans[] = { CFile::begin, CFile::current, CFile::end };

	TRY
	{
		m_pCFile->Seek(position, mode_trans[mode]);
	}
	CATCH (CFileException, e)
	{
		return ERRORCODE_IoError;
	}
	END_CATCH

	return ERRORCODE_None;
}

/*
// Return the current position of the file.
*/

ERRORCODE CFileFile::tell(ST_DEV_POSITION far *position)
{
	TRY
	{
		*position = m_pCFile->GetPosition();
	}
	CATCH (CFileException, e)
	{
		return ERRORCODE_IoError;
	}
	END_CATCH

	return ERRORCODE_None;
}

/*
// Return the length of the file.
*/

ERRORCODE CFileFile::length(ST_DEV_POSITION far *size)
{
	TRY
	{
		*size = m_pCFile->GetLength();
	}
	CATCH (CFileException, e)
	{
		return ERRORCODE_IoError;
	}
	END_CATCH

	return ERRORCODE_None;
}

/*
// Flush the CFileFile object.
*/

ERRORCODE CFileFile::flush()
{
	TRY
	{
		m_pCFile->Flush();
	}
	CATCH (CFileException, e)
	{
		return ERRORCODE_Write;
	}
	END_CATCH

	return ERRORCODE_None;
}

/*
// Truncate the file at the current position.
*/

ERRORCODE  CFileFile::truncate()
{
	TRY
	{
		m_pCFile->SetLength(m_pCFile->GetPosition());
	}
	CATCH (CFileException, e)
	{
		return ERRORCODE_Write;
	}
	END_CATCH

	return ERRORCODE_None;
}
