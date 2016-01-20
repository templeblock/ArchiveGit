/****************************************************************************
Contains:	LineBreaker interface
****************************************************************************/

#pragma once

#include "sccolumn.h"

enum eHyphenRank
{
	eDiscHyphRank	= 1,
	eBestHyphRank,
	eGoodHyphRank,
	eBadHyphRank
};

struct Hyphen
{
	short		offset;
	eHyphenRank rank;
};

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
	#endif scUseRubi
							  short, 
							  short&, 
							  scSpecRecord **, 
							  scXRect&, 
							  GlyphSize&,
							  DCState& );

	//MicroPoint BRKComposeRenMoji(  CharRecordP chRec, TypeSpec ts, scFlowDir& fd, BOOL fit );
#endif scJIS4051

BOOL BRKJustify( CharRecordP, long, long, MicroPoint );
