/***************************************************************************

	File:		SCAPPTEX.H


	$Header: /Projects/Toolbox/ct/SCAPPTEX.H 2	   5/30/97 8:45a Wmanis $

	Contains:	The class for passing content plus typo state
				back between client and toolbox.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
@doc
****************************************************************************/

#ifndef _H_SCAPPTEX
#define _H_SCAPPTEX

#ifdef SCMACINTOSH
#pragma once
#endif

#include "sctypes.h"

class stTextImportExport {
public:
	static	stTextImportExport& MakeTextImportExport( int encoding = 1 );
	
	virtual void	release() = 0;
	
		// writing
	virtual void	StartPara( TypeSpec& ) = 0;
	virtual void	SetParaSpec( TypeSpec& ) = 0;
	virtual void	PutString( const uchar*, int, TypeSpec& ) = 0;
	virtual void	PutString( stUnivString&, TypeSpec& ) = 0;
	virtual void	PutChar( UCS2, TypeSpec& ) = 0;
	
		// reading
	virtual int 	NextPara( TypeSpec& ) = 0;
	virtual int 	GetChar( UCS2&, TypeSpec& ) = 0;	
	virtual void	reset() = 0;
	virtual void	resetpara() = 0;
};


#endif /* _H_SCAPPTEX */


