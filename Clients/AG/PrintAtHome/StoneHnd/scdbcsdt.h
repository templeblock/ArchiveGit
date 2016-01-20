/*=================================================================
Contains:	Class for reading DBCS files.
=================================================================*/

#pragma once

#include "sctypes.h"

class scDBCSDetector
{
public:

	enum eByteType {
		eFirstByte	= -1,
		eOnlyByte,
		eLastByte,
		eMiddleByte
	};		// id's a byte of a multibyte character

	scDBCSDetector( TypeSpec ts );

	void		setDBCS( TypeSpec ts ); 		
	long		StrLen( const char * ) const;

	eByteType	ByteType( uchar ch ) const
	{
		return dbcs_ ? shiftjis_[ch] : eOnlyByte;
	}	

private:
	BOOL				dbcs_;
	static eByteType	shiftjis_[];
};
