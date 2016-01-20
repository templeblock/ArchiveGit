/****************************************************************************

	File:		SCBREAK.H


	$Header: /Projects/Toolbox/ct/SCBREAK.H 2	  5/30/97 8:45a Wmanis $

	Contains:	LineBreaker interface

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************************/

#ifndef _H_SCBREAK
#define _H_SCBREAK

#ifdef SCMACINTOSH
#pragma once
#endif

#include "sccolumn.h"

typedef enum eHyphenRanks {
	eDiscHyphRank	= 1,
	eBestHyphRank,
	eGoodHyphRank,
	eBadHyphRank
} eHyphenRank;


struct Hyphen {
	short		offset;
	eHyphenRank rank;
};


/* =============================================== */
/* =============================================== */

#include "screfdat.h"

class scSpecRecord;
class scRubiArray;
class scLEADRefData;
class scLINERefData;




class DCState;

eBreakType	BRKRomanLineBreak( CharRecordP, 
							  long,
							  long&,
							  scLINERefData&,
							  short, 
							  short&, 
							  scSpecRecord **, 
							  scXRect&, 
							  GlyphSize& );
						  
#ifdef scJIS4051

eBreakType	BRKJapanLineBreak( CharRecordP, 
							  long,
							  long&,
							  scLINERefData&,
#ifdef scUseRubi
							  scRubiArray *,
#endif
							  short, 
							  short&, 
							  scSpecRecord **, 
							  scXRect&, 
							  GlyphSize&,
							  DCState& );

//MicroPoint BRKComposeRenMoji(  CharRecordP chRec, TypeSpec ts, scFlowDir& fd, Bool fit );
											
#endif

Bool		BRKJustify( CharRecordP, long, long, MicroPoint );


#endif /* _H_SCBREAK */


