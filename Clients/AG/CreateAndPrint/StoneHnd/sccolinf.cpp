/*****************************************************************************
Contains:	code to collect column redisplay information
*****************************************************************************/

#include "scpubobj.h"
#include "sccolumn.h"
#include "scglobda.h"
#include "screfdat.h"

/* ==================================================================== */
void scRedispList::ReInit( )
{
}

/* ==================================================================== */
scColRedisplay* scRedispList::FindCell( const scColumn* col ) const
{
	long			limit		= GetNumItems();
	scColRedisplay* colredisp	= (scColRedisplay*)Lock();

	for ( ; limit--; colredisp++ ) {
		if ( colredisp->fColumnID == col ) {
			Unlock();
			return colredisp;
		}
	}

	Unlock();
	
	return 0;
}

/* ==================================================================== */
void scRedispList::AddCell( scColumn* col ) 
{
	scAssert( !FindCell( col ) );

	scColRedisplay colredisp( col, col->GetAPPName() );

	AppendData( (ElementPtr)&colredisp );
}

/* ==================================================================== */
void scRedispList::AddColumn( const scCOLRefData& colRefData )
{
	Lock();
	
	scColRedisplay* cell = FindCell( colRefData.fCol );
	
	if ( !cell ) {
		Unlock();
		AddCell( colRefData.fCol );
		Lock();
		cell = FindCell( colRefData.fCol );
	}

	colRefData.fCol->ComputeInkExtents( );		
	cell->fWidth			= colRefData.fCol->Width();
	cell->fDepth			= colRefData.fCol->Depth();
	cell->fExRect			= colRefData.fCol->GetInkExtents();
	cell->fAdditionalText	= colRefData.fCol->MoreText( );
	
	scXRect fRepaintRect( cell->fRepaintRect );
	fRepaintRect.Union(  colRefData.fLineDamage );
	cell->fRepaintRect		= fRepaintRect;
	cell->fHasRepaint		= fRepaintRect.Valid();

	scXRect fDamageRect( cell->fDamageRect );
	fDamageRect.Union(	colRefData.fLineDamage );
	cell->fDamageRect		= fDamageRect;
	cell->fHasDamage		= fDamageRect.Valid();


	Unlock();
}

/* ==================================================================== */
void scRedispList::AddColumn( scColumn* col, scXRect& xrect )
{
	Lock();
	
	scColRedisplay* cell = FindCell( col );
	
	if ( !cell ) {
		Unlock();
		AddCell( col );
		Lock();
		cell = FindCell( col );
	}

	col->ComputeInkExtents( );		
	cell->fWidth			= col->Width();
	cell->fDepth			= col->Depth();
	cell->fExRect			= col->GetInkExtents();
	cell->fAdditionalText	= col->MoreText();
	
	scXRect fRepaintRect( cell->fRepaintRect );
	fRepaintRect.Union( xrect );
	cell->fRepaintRect	= fRepaintRect;
	cell->fHasRepaint	= fRepaintRect.Valid();

	Unlock();
}

/* ==================================================================== */
void scRedispList::SetImmediateRect( scColumn*					col,
									 const scImmediateRedisp&	immedredisp )
{
	Lock();
	
	scColRedisplay* cell = FindCell( col );
	
	if ( !cell ) {
		Unlock();
		AddCell( col );
		Lock();
		cell = FindCell( col );
	}

	cell->fImmediateRedisplay	= true;
	cell->fImmediateArea		= immedredisp;

	Unlock();
}

/* ==================================================================== */
status scRedispList::CL_GetColumnData( APPColumn		appname,
									   scColRedisplay&	data ) const
{
	status			stat	= scSuccess;
	volatile int	locked	= false;
	volatile int	found	= false;

	try {
		long			limit		= GetNumItems();
		scColRedisplay* colredisp	= (scColRedisplay*)Lock();
		locked						= true;

		for ( ; limit--; colredisp++ ) {
			if ( colredisp->fAPPName == appname ) {
				data = *colredisp;
				found = true;
			}
		}
		raise_if( found == false, scERRstructure );
	}
	IGNORE_RERAISE;

	return stat;
}
