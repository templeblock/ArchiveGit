/*****************************************************************************
Contains:	The code to vj columns and to save the line state
			before reformatting and then compare it post reformatting
			to determine redisplay.
****************************************************************************/

#include "sccolumn.h"
#include "scglobda.h"
#include "sctextli.h"
#include "scpubobj.h"
#include "scmem.h"
#include "scexcept.h"
#include "screfdat.h"
#include "sccallbk.h"
#include "scstcach.h"
#include "scstream.h"
#include <limits.h>

struct VJSpace {
	MicroPoint	opt;
	MicroPoint	max;
	REAL		upperBound;
};

static void 	COLFlushTop( scColumn* );
static void 	COLFlushTopBottom( scColumn* );

enum eDoVJ {
	eVJBottom = 1,
	eVJCenter
};

#define 	growUnits	30
#define 	growSize	( sizeof( VJSpace ) * growUnits )

/*===================================================================*/
/* Add a space record to the end of the handle. 					 */
static void InsertSpaceRecord( VJSpace*&	space,
							   MicroPoint	opt,
							   MicroPoint	max,
							   size_t		numRecords )
{
	size_t newSize	= ( numRecords + 1 ) * sizeof( VJSpace );
	
	if ( MEMGetSizePtr( space ) <= newSize * sizeof( VJSpace ) )
		space = (VJSpace*)MEMResizePtr( (void**)&space, newSize * sizeof( VJSpace ) );

	VJSpace*	spacePtr = space;
	spacePtr += numRecords;

	spacePtr->opt				= opt;
	spacePtr->max				= max;
	spacePtr->upperBound		= (REAL)max / opt;
}

/*===================================================================*/
/* Return the minimum line space multiplier allowed 				 */
/* by any line in the column.										 */
static REAL MaxSpaceStretch( VJSpace*	space,
							 size_t 	numRecords )
{
	REAL			maxStretch;

	if ( numRecords-- ) {
		maxStretch = space->upperBound;
		space++;
	}
	else
		return 0;

	for ( ; numRecords--; space++ ) {
		if ( space->upperBound < maxStretch )
			maxStretch = space->upperBound;
	}

	return maxStretch;
}

/*===================================================================*/
/* Return the total line space expansion if every line's space is	 */
/* multiplied by maxStretch.										 */
static MicroPoint TotalSpaceStretch( VJSpace*	space,
									 size_t 	numRecords,
									 REAL		maxStretch )
{
	REAL			totalStretch = 0;

	for ( ; numRecords--; space++ )
		totalStretch += space->opt * ( maxStretch - 1 );

	return scRoundMP( totalStretch );
}

/*===================================================================*/
/* Return the product of the optimum line space and 				 */
/* the stretchFactor to calculate the distance to shift a line down. */
static MicroPoint LineShift( VJSpace*	space,
							 REAL		stretchFactor,
							 short		index )
{
	MicroPoint		shift;

		/* MicroPoint * REAL */
	shift = (MicroPoint)( space[index].opt * ( stretchFactor - 1 ) );

	return shift;
}

/*===================================================================*/
/* If flag == center, we do center vertical justification			 */
/* Otherwise, we do flush bottom.									 */
static void COLFlushBottom( scColumn*	col,
							eDoVJ		flag )
{
	scTextline	*txl;
	scTextline	*lastLineH;
	scTextline	*tLine;
	TypeSpec	spec;
	MicroPoint	vDiff,
				maxDepth	= LONG_MIN;
	scXRect 	lineRect;
	BOOL		vertical	= false;

	COLFlushTop( col );   // remove effects of previous vj

	if ( col->GetFlowdir().IsVertical() ) {
		vertical = true;
		maxDepth = LONG_MAX;
	}

	lastLineH	= txl	= col->GetFirstline();
	if ( txl == NULL ) {
		return;
	}

	for ( ; txl != NULL; txl = LNNext( txl ) ) {
		tLine		= txl;
		if ( vertical )
			maxDepth	= MIN( tLine->GetOrigin().x, maxDepth );
		else {
				/* Find the depth of the last line */
			maxDepth	= MAX( tLine->GetOrigin().y, maxDepth );
		}
		lastLineH	= txl;
	}

		/* Calculate the distance between the last possible */
		/* line position and our last actual position		*/
	if ( vertical )
		vDiff	= maxDepth;
	else
		vDiff	= col->Depth() - maxDepth;

	MicroPoint maxlead	= lastLineH->MaxLead( spec );
	vDiff				-= CSlastLinePosition( col->GetAPPName(), spec );

	if ( flag == eVJCenter ) {
		/* For center justification, cut the distance to move each line in half
		 */
		if ( ( txl = col->GetFirstline() ) != NULL ) {

			MicroPoint maxlead = txl->MaxLead( spec );
				
			scCachedStyle::SetFlowdir( col->GetFlowdir() );
			scCachedStyle::GetCachedStyle( spec );

//j			vDiff += CSfirstLinePosition( col->GetAPPName(), spec );
			vDiff /= 2;
		}
	}

	if ( vDiff != 0 ) {

		col->SetInkExtents( 0, 0, 0, 0 );

		for ( txl = col->GetFirstline(); txl != NULL; txl = LNNext( txl ) ) {
				/* Shift all the lines down by	*/
				/* the specified distance		*/
			txl->SetVJ( vDiff );
			txl->QueryExtents( lineRect );
			col->UnionInkExtents( lineRect );
		}
	}
}


/*===================================================================*/
/* Vertical justification on a column. Includes both feathering and  */
/* paragraph spacing.												 */

// TOOLBOX BEHAVIOR
//		We will exceed maximum values to VJ at all costs
//		In such excessive conditions, we won't use extra
//		para spacing to achieve our end; we will use
//		extra line spacing.
//				or
//		We will not exceed max values and thus may not achieve
//		vertical justification
//
static const BOOL		exceedMaxValues 	= false;
static const BOOL		extraPPspacing		= false;

static void COLFlushTopBottom( scColumn *col )
{
	VJSpace*	lineSpace = 0;
	VJSpace*	paraSpace = 0;
	
	scTextline* 	txl;
	scTextline* 	tLine = NULL;
	TypeSpec		spec;
	scContUnit* lnParaH;
	scContUnit* paraH = NULL;
	short			interPara,
					interLine,
					lineAdj,
					paraAdj;
	MicroPoint		vDiff,
					currDepth	= LONG_MIN,
					currLineSpace,
					currParaSpace,
					maxTotalLineStretch,
					maxTotalParaStretch,
					adjustment,
					lead;
	REAL			maxLineStretch,
					maxParaStretch,
					lineStretchFactor,
					paraStretchFactor;
	scXRect 		lineRect;
	BOOL			vertical	= false;

	COLFlushTop( col ); 		// remove effects of vj
	
	if ( col->GetFlowdir().IsVertical() ) {
		vertical	= true;
		currDepth	= LONG_MAX;
	}

	/* These handles will store arrays of structures to represent	*/
	/* the optimum and the maximum spacing for each line in 		*/
	/* the column, and for each paragraph in the column.			*/

	try {
		lineSpace = (VJSpace*)MEMAllocPtr( sizeof( VJSpace ) * growUnits );
		paraSpace = (VJSpace*)MEMAllocPtr( sizeof( VJSpace ) * growUnits );

		interPara	= interLine = 0;
		txl 	= col->GetFirstline();

		currLineSpace	= 0;	/* These represent the current total line	*/
		currParaSpace	= 0;	/* and para spacing before VJ				*/

		for ( ; txl; txl = LNNext( txl ) ) {
			MicroPoint maxlead	= txl->MaxLead( spec );
			tLine	= txl;
			scCachedStyle& cs = scCachedStyle::GetCachedStyle( spec );
			
			if ( paraH == NULL )
				;
			else if ( paraH == tLine->GetPara() ) {
				lead = cs.GetComputedLead( );
//j				maxlead = cs.GetComputedMaxLead();
				maxlead = lead;

				/* Accumulate line space information for each line	*/
				InsertSpaceRecord( lineSpace, lead, maxlead, interLine++ );
				currLineSpace += lead;
			}
			else if ( paraH != tLine->GetPara() ) {
				lead = scCachedStyle::GetParaSpace( paraH, tLine->GetPara() );
//j				maxlead = scCachedStyle::GetMaxParaSpace( paraH, tLine->GetPara() );
				maxlead = lead;
						  
				/* Accumulate para space information for each para	*/
				InsertSpaceRecord( paraSpace, lead, maxlead, interPara++ );
				currParaSpace += lead;
			}

			paraH		= tLine->GetPara();

			if ( vertical )
				currDepth	= MIN( tLine->GetOrigin().x, currDepth );
			else
				currDepth	= MAX( tLine->GetOrigin().y, currDepth );
						/* This will tell us the	*/
						/* depth of the last line	*/
		}

		/* Calculate the difference between where the last line is and
		 * where we want it to be
		 */
		if ( vertical )
			vDiff = -CSlastLinePosition( col->GetAPPName(), spec ) + currDepth;
		else
			vDiff = col->Depth() - CSlastLinePosition( col->GetAPPName(), spec ) - currDepth;

			/* The greatest factors by which we can 	*/
			/* multiply the space of each line and para */
		maxLineStretch		= MaxSpaceStretch( lineSpace, interLine );
		maxParaStretch		= MaxSpaceStretch( paraSpace, interPara );

			/* How much space this	*/
			/* will buy us			*/
		maxTotalLineStretch = TotalSpaceStretch( lineSpace, interLine,
													maxLineStretch );
		maxTotalParaStretch = TotalSpaceStretch( paraSpace, interPara,
													maxParaStretch );

			/* How much we are currently stretching the line space	*/
			/* and para space										*/
		lineStretchFactor	= 1;
		paraStretchFactor	= 1;

			/* If VJ is impossible or unnecessary	*/
		if ( currParaSpace < 0
				|| currLineSpace < 0
				|| ( currParaSpace == 0 && currLineSpace == 0 )
				|| maxLineStretch < 0
				|| maxParaStretch < 0
				|| vDiff <= 0 )
		{
			COLFlushTop( col );
			return;
		}

		if ( maxTotalParaStretch >= vDiff && currParaSpace > 0 ) {
				/* If we can do it with para spacing alone, */
				/* do it. -- REAL / MicroPoint				*/
			paraStretchFactor = 1 + ((REAL)vDiff) / currParaSpace;
		}
		else {
			if ( currParaSpace > 0 ) {
					/* Start off by stretching paragraph spacing	*/
					/* to the max.									*/
				paraStretchFactor	= maxParaStretch;
				vDiff				-= maxTotalParaStretch;
			}

			if ( maxTotalLineStretch >= vDiff && currLineSpace > 0 ) {
					/* If we can do remaining VJ within */
					/* max line spacing, do it --  REAL / MicroPoint */
				lineStretchFactor = 1 + ((REAL)vDiff) / currLineSpace;
			}
			else {
				if ( currLineSpace > 0 ) {
						/* Stretch line spacing to the max, */
						/* and see what's left over 		*/
					lineStretchFactor	= maxLineStretch;
					vDiff				-= maxTotalLineStretch;
				}

				if ( exceedMaxValues ) {
					if ( currParaSpace > 0 && extraPPspacing ) {
							/* If extraPPspacing were true (it isn't),	*/
							/* we would simply increase para spacing	*/
							/* to cover the excess. 					*/
						paraStretchFactor = 1 + ( ((REAL)vDiff)
									+ maxTotalParaStretch ) / currParaSpace;

					}		/* (REAL + MicroPoint) / MicroPoint 		*/
					else {
						/* Spread remaining space evenly over all remaining
						 * lines, including both inter line and inter para
						 * spacing.
						 */
						/* Some care is requires to do it evenly.	*/
						MicroPoint	totalParaSpace
							= currParaSpace ? currParaSpace+maxTotalParaStretch:0;

						MicroPoint	totalLineSpace
							= currLineSpace ? currLineSpace+maxTotalLineStretch:0;

						MicroPoint	totalSpace
							= totalParaSpace + totalLineSpace;

						MicroPoint	paraDiff
							= scRoundMP( ((REAL)totalParaSpace) / totalSpace * vDiff );

						MicroPoint	lineDiff
							= scRoundMP( ((REAL)totalLineSpace) / totalSpace * vDiff );

						/* REAL / MicroPoint / MicroPoint	*/

						if ( currParaSpace )
							paraStretchFactor	= 1 + ( ((REAL)paraDiff)
										+ maxTotalParaStretch ) / currParaSpace;
						if ( currLineSpace )
							lineStretchFactor	= 1 + ( ((REAL)lineDiff)
										+ maxTotalLineStretch ) / currLineSpace;
						/* ( REAL + MicroPoint ) / MicroPoint	*/
					}
				}
			}
		}


		adjustment	= 0;
		lineAdj 	= paraAdj	= 0;
		paraH		= NULL;

		col->SetInkExtents( 0, 0, 0, 0 );

		for ( txl = col->GetFirstline(); txl; txl = LNNext( txl ) ) {
			lnParaH = txl->GetPara( );

			/* Shift each line down the appropriate amount	*/

			if ( paraH == NULL )
				txl->SetVJ( 0 );
			else if ( paraH == txl->GetPara() ) {
					/* Add line space */
				adjustment += LineShift( lineSpace, lineStretchFactor, lineAdj );
				txl->SetVJ( adjustment );
				lineAdj++;
			}
			else if ( paraH != tLine->GetPara() ) {
					/* Add para space */
				adjustment += LineShift( paraSpace, paraStretchFactor, paraAdj );
				txl->SetVJ( adjustment );
				paraAdj++;
			}
			paraH = lnParaH;
			txl->QueryExtents( lineRect );
			col->UnionInkExtents( lineRect );
		}
	}
	catch( ... ) {
		MEMFreePtr( lineSpace );
		MEMFreePtr( paraSpace );
		throw;
	} 

	MEMFreePtr( lineSpace );
	MEMFreePtr( paraSpace );

}

/*===================================================================*/
/* shove the lines to the top */

static void COLFlushTop( scColumn* col )
{
	for ( scTextline* txl = col->GetFirstline(); txl; txl = LNNext( txl ) )
		txl->RemoveVJ( );
}

/*===================================================================*/
void scColumn::SetDepthNVJ( MicroPoint		dimension,
							scRedispList*	redispList )
{
	scXRect lineDamage;

	if ( Marked( scINVALID ) )
		LimitDamage( redispList, scReformatTimeSlice ); 

	if ( fFlowDir.IsHorizontal() )
		SetDepth( dimension );
	else
		SetWidth( dimension );
	
	scRedisplayStoredLine rdl( GetLinecount() );
	rdl.SaveLineList( this );
	VertJustify( );
	rdl.LineListChanges( this, lineDamage, redispList );

}

/*===================================================================*/
/* align the text lines in the column,
 * this function just serves as a dispatcher
 */
void scColumn::VertJustify( )
{
	eVertJust		attributes		= GetVertJust();
	eColShapeType	colShape		= GetShapeType();

	if ( ! ( colShape == eNoShape  || ( colShape & eFlexShape )  ) )
		COLFlushTop( this );
	else {
		switch ( attributes ) {
			case eVertJustified:
				if ( !GetNext() ) {
						/* If this is the stream's last column, don't VJ	*/
						/* unless force VJ is set. If it isn't the last 	*/
						/* column, fall through to the next case to VJ. 	*/
					COLFlushTop( this );		// remove effects of vj
					break;
				}
				// let this fall thru

			case eVertForceJustify:
				COLFlushTopBottom( this );
				break;

			case eVertBottom:
				COLFlushBottom( this, eVJBottom );
				break;

			case eVertCentered:
				COLFlushBottom( this, eVJCenter );
				break;

			default:
			case eVertTop:
				COLFlushTop( this );
				break;
		}
	}
}

/*===================================================================*/
/* determine the number of lines in a column */
ushort scColumn::GetLinecount( ) const
{
	scTextline* txl;
	ushort		lineCount;

	for ( lineCount = 0, txl = GetFirstline(); txl; txl = txl->GetNext() )
		lineCount++;
	return lineCount;
}

/*===================================================================*/
/* The functions that follow are used to keep track of which lines	 */
/* move during VJ, to minimize repainting. If any of it fails due	 */
/* lack of memory, VJ is not jeopardized, but everything will end	 */
/* up being repainted.												 */
/*=================================================================*/
scRedisplayStoredLine::scRedisplayStoredLine( int lines )
{
	LineListInit( lines );
}

/*=================================================================*/

scRedisplayStoredLine::~scRedisplayStoredLine(	)
{
	delete [] fStoredData;
	fStoredData = NULL;
	fStoredLines = 0;
}

/*=================================================================*/

void scRedisplayStoredLine::LineListInit( int lines )
{
	fUsingStoredData	= false;
	fData				= 0;
	fNumItems			= 0;	
	fStoredData 		= (lines > 0 ? new scTextline[lines] : NULL);
	fStoredLines		= (short)lines;
}

/*=================================================================*/

void scRedisplayStoredLine::LineListFini(  )
{
	for (ushort  i = 0; i < fStoredLines; i++)
		fStoredData[i].InitForReuse( 0 );

	delete [] fStoredData;
	fStoredData = NULL;
	fStoredLines = 0;
}

/*=================================================================*/
/* save an image of the lines in a column to determine repainting
 * at the completion of reformatting
 */

void scRedisplayStoredLine::SaveLineList( scColumn* col )
{
	scTextline* txlCopy;
	scTextline* txl;
	ushort		lines;

	lines	= col->GetLinecount( );

	fNumItems	= lines;
	fOrgExtents = col->GetInkExtents();

	if ( lines ) {
			// determine if we are using the stored lines ( about 200 )
			// or do we dynamically allocate a list - if more than 200 lines
			// - which should be almost never
			//
		if (  fStoredData && lines < fStoredLines  ) {
			fUsingStoredData	= true;
			fData				= fStoredData;
		}
		else {
			fData  = new scTextline[ lines ];
			fUsingStoredData  = false;
		}

			// copy current state to the list of lines
		txl = col->GetFirstline();
		for ( txlCopy =  fData ; lines--; txl = LNNext( txl ), txlCopy++ )
			*txlCopy = *txl;

		scAssert( txl == NULL );
	}
	else
		 fData	= NULL;
}

/*=================================================================*/
// compare the list of saved lines with the current column lines and
// determine the repainting that needs to be done

void scRedisplayStoredLine::LineListChanges( scColumn*		col,
											 const scXRect& oldLineDamage,
											 scRedispList*	redispList )
{
	scTextline* txlOrg;
	scTextline* txl;
	scXRect 	lineDamage( oldLineDamage );
	ushort		lines		= col->GetLinecount();
	
	scStreamChangeInfo streamChange;
			
	streamChange = gStreamChangeInfo;

	if (  fData  == NULL ) {
			// redraw the entire column 
		if ( redispList ) {
			col->QueryMargins( fOrgExtents );
			redispList->AddColumn( col, fOrgExtents );
		}
		col->Unmark( scREPAINT );
	}
	else {
		txl = col->GetFirstline();


			//
			// compare old lines and new lines and where they differ
			// mark that area to be repainted
			//
		for ( txlOrg = fData; lines &&	fNumItems; txl = LNNext( txl ), txlOrg++ ) {
			lines--;
			fNumItems --;
			if ( !txl->Compare( txlOrg, streamChange ) ) {

					// handle old line position now
				scXRect xrect,
						xrect2;
				txl->QueryExtents( xrect, 1 );
				txlOrg->QueryExtents( xrect2, 1 );
				lineDamage.Union( xrect );
				lineDamage.Union( xrect2 );
				txl->Unmark( scREPAINT );
			}
		}

			// fData ran out first, mark the rest of the new lines
		for ( ; lines--; txl = LNNext( txl ) ) {
			scXRect xrect;
			txl->QueryExtents( xrect, 1 );
			lineDamage.Union( xrect );
		}

			// current lines ran out first
		for( ;	fNumItems--; txlOrg++ ) {
			scXRect xrect;
			txlOrg->QueryExtents( xrect, 1 );
			lineDamage.Union( xrect );
		}

		if ( redispList )
			redispList->AddColumn( col, lineDamage );
		col->Unmark( scREPAINT );

			// free the list
		if ( !fUsingStoredData	)
			delete [] fData;

		fData		= NULL;
		fNumItems	= 0;
	}
}
