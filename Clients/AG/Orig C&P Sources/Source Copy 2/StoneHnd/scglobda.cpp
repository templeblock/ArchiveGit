/***************************************************************************

	File:		SCGLOBDA.C

	$Header: /Projects/Toolbox/ct/SCGLOBDA.CPP 2	 5/30/97 8:45a Wmanis $

	Contains:	Global data, which should be gone soon!

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

****************************************************************************/

#include "scexcept.h"

#include <string.h> 	  
#include "scmem.h"

#include "scglobda.h"
#include "scparagr.h"
#include "sccolumn.h"
#include "sctextli.h"

scDEFINE_RTTI( scTBObj, scObject );
scDEFINE_RTTI( scColumn, scTBObj );
scDEFINE_RTTI( scTextline, scTBObj );
scDEFINE_RTTI( scContUnit, scTBObj );

scDEFINE_ABSTRACT_RTTI( scAbstractArray, scObject );
scDEFINE_RTTI( scHandleArray, scAbstractArray );
scDEFINE_RTTI( scMemArray, scAbstractArray );
scDEFINE_RTTI( scCharArray, scHandleArray );
	
	
char *SCS_Copyright  = "Copyright (c) 1988-1994 Stonehand Inc. All rights reserved.";

BreakStruct 		gbrS;
GlobalColumnStruct	ggcS;
scStreamChangeInfo	gStreamChangeInfo;

Bool				gHiliteSpaces;	// hilite trailing spaces at the end of a line	

long	scLogUnitsPerPixel = 20;
