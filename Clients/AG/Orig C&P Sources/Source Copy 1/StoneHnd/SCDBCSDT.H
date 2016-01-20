/*=================================================================
 
	File:		chfile.h

	$Header: /Projects/Toolbox/ct/SCDBCSDT.H 2     5/30/97 8:45a Wmanis $

	Contains:	Class for reading DBCS files.

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

=================================================================*/



#ifndef _H_SCDBCSDT
#define _H_SCDBCSDT


#include "sctypes.h"

class scDBCSDetector {
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
	Bool				dbcs_;
	
	static eByteType	shiftjis_[];
};

#endif /* _H_SCDBCSDT */
