/***************************************************************************
Contains:	Maintains typespec list.
***************************************************************************/

#include "scpubobj.h"

/* ==================================================================== */
void scTypeSpecList::Insert( TypeSpec& ts )
{	
	for ( int i = 0; i < NumItems(); i++ ) {
		if ( ts.ptr() == (*this)[i].ptr() )
			return;
	}
	Append( ts );
}

/* ==================================================================== */
scKeyRecord::scKeyRecord() :
	type_( insert ),
	fKeyCode( 0 ),
	fReplacedChar( 0 ),
	field_( 0 ),
	replacedfield_( 0 ),
	fEscapement( 0 ),
	fSpec( 0 ),
	fNoOp( 0 ),
	fRestoreSelect( 0 )
{
}

/* ==================================================================== */
scKeyRecord::scKeyRecord( const scKeyRecord& rec )
{
	type_			= rec.type_;
	fKeyCode		= rec.fKeyCode;
	field_			= rec.field_;
	fReplacedChar	= rec.fReplacedChar;
	replacedfield_	= rec.replacedfield_;
	fEscapement 	= rec.fEscapement;
	fSpec			= rec.fSpec;
	fNoOp			= rec.fNoOp;
	fRestoreSelect	= rec.fRestoreSelect;
	fMark			= rec.fMark;
}

/* ==================================================================== */
scKeyRecord& scKeyRecord::operator=( const scKeyRecord& rec )
{
	type_			= rec.type_;
	fKeyCode		= rec.fKeyCode;
	field_			= rec.field_;
	fReplacedChar	= rec.fReplacedChar;
	replacedfield_	= rec.replacedfield_;
	fEscapement 	= rec.fEscapement;
	fSpec			= rec.fSpec;
	fNoOp			= rec.fNoOp;
	fRestoreSelect	= rec.fRestoreSelect;
	fMark			= rec.fMark;

	return *this;
}

/* ==================================================================== */
scKeyRecord::~scKeyRecord()
{
}

/* ==================================================================== */
void scKeyRecord::Invert()
{
	UCS2 tmpChar = fReplacedChar;
	fReplacedChar = fKeyCode;
	fKeyCode = tmpChar;

	uint8 tmpfield = replacedfield_;
	replacedfield_ = field_;
	field_ = tmpfield;		
}

/* ==================================================================== */
scStreamLocation& scStreamLocation::operator=( const scStreamLocation& sl )
{
	fStream 		= sl.fStream;
	fAPPColumn		= sl.fAPPColumn;
	fParaNum		= sl.fParaNum;
	fParaOffset 	= sl.fParaOffset;
	fEndOfLine		= sl.fEndOfLine;
	fTheCh			= sl.fTheCh;
	fFlags			= sl.fFlags;
	fUnitType		= sl.fUnitType;
	fTheChWidth 	= sl.fTheChWidth;
	fChSpec 		= sl.fChSpec;
	fParaSpec		= sl.fParaSpec; 
	fPosOnLine		= sl.fPosOnLine;
	fSelMaxX		= sl.fSelMaxX;
	fFont			= sl.fFont;
	fPointSize		= sl.fPointSize;
	fBaseline		= sl.fBaseline;
	fMeasure		= sl.fMeasure;
	fLetterSpace	= sl.fLetterSpace;
	fWordSpace		= sl.fWordSpace;

	return *this;
}

/* ==================================================================== */	
scStreamLocation::scStreamLocation( const scStreamLocation& sl ) :
	fStream( sl.fStream ),
	fAPPColumn( sl.fAPPColumn ),
	fParaNum( sl.fParaNum ), 
	fParaOffset( sl.fParaOffset ), 
	fEndOfLine( sl.fEndOfLine ), 
	fTheCh( sl.fTheCh ), 
	fFlags( sl.fFlags ), 
	fUnitType( sl.fUnitType ), 
	fTheChWidth( sl.fTheChWidth ), 
	fChSpec( sl.fChSpec ),
	fParaSpec( sl.fParaSpec ),	
	fPosOnLine( sl.fPosOnLine ), 
	fSelMaxX( sl.fSelMaxX ), 
	fFont( sl.fFont ), 
	fPointSize( sl.fPointSize ), 
	fBaseline( sl.fBaseline ), 
	fMeasure( sl.fMeasure ), 
	fLetterSpace( sl.fLetterSpace ), 
	fWordSpace( sl.fWordSpace )
{
}

/* ==================================================================== */						
scStreamLocation::scStreamLocation() :
	fStream( 0 ),
	fAPPColumn( 0 ),
	fParaNum( 0 ), 
	fParaOffset( 0 ), 
	fEndOfLine( 0 ), 
	fTheCh( 0 ), 
	fFlags( 0 ), 
	fUnitType( eNoUnit ), 
	fTheChWidth( 0 ), 
	fChSpec( 0 ),
	fParaSpec( 0 ), 	
	fPosOnLine( 0 ), 
	fSelMaxX( 0 ), 
	fFont( 0 ), 
	fPointSize( 0 ), 
	fBaseline( 0 ), 
	fMeasure( 0 ), 
	fLetterSpace( 0 ), 
	fWordSpace( 0 )
{
}

/* ==================================================================== */
void scStreamLocation::Init()
{
	fStream 		= 0;
	fAPPColumn		= 0;
	fParaNum		= 0;
	fParaOffset 	= 0;
	fEndOfLine		= 0;
	fTheCh			= 0;
	fFlags			= 0;
	fUnitType		= eNoUnit;
	fTheChWidth 	= 0;
	fChSpec.clear();
	fParaSpec.clear();
	fPosOnLine		= 0;
	fSelMaxX		= 0;
	fFont			= 0;
	fPointSize		= 0;
	fBaseline		= 0;
	fMeasure		= 0;
	fLetterSpace	= 0;
	fWordSpace		= 0;
}

#if SCDEBUG > 1

/* ==================================================================== */
void scSpecLocList::DbgPrint( void ) const
{
	SCDebugTrace( 0, scString( "\nSCSPECLOCLIST\n" ) );
	for ( int i = 0; i < NumItems(); i++ ) {
		SCDebugTrace( 0, scString( "\tscCharSpecLoc ( %d %d ) 0x%08x\n" ),
					  (*this)[i].offset().fParaOffset,
					  (*this)[i].offset().fCharOffset,
					  (*this)[i].spec() );
	}
	SCDebugTrace( 0, scString( "SCSPECLOCLIST\n" ) );	
}

#endif

/* ==================================================================== */
TypeSpec scSpecLocList::GetLastValidSpec( void ) const
{
	for ( int i = NumItems() - 1; i >= 0; i-- ) {
		if ( (*this)[i].spec().ptr() )
			return (*this)[i].spec();
	}
	return 0;
}

/* ==================================================================== */	
TypeSpec scSpecLocList::GetFirstValidSpec( void ) const
{
	for ( int i = 0; i < NumItems(); i++ ) {
		if ( (*this)[i].spec().ptr() )
			return (*this)[i].spec();
	}
	return 0;
}

/* ==================================================================== */	
TypeSpec scSpecLocList::GetNthValidSpec( int nth ) const
{
	for ( int i = 0; i < NumItems(); i++ ) {
		if ( (*this)[i].spec().ptr() && --nth == 0 )
			return (*this)[i].spec();
	}
	return 0;
}
