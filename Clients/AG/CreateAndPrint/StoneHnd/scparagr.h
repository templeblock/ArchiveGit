/*****************************************************************************
Contains:	Method/Function interface to class of paragraph
*****************************************************************************/

#pragma once

#include "sctbobj.h"
#include "sccharex.h"
#include "scspcrec.h"
#include "scmemarr.h"
#include "scappint.h"
#include <vector>
using namespace std;

#ifdef _RUBI_SUPPORT
	class scRubiArray;
#endif

class scColumn;
class scCOLRefData;
class scSpecRecord;
class scMuPoint;
class scAnnotation;
class scLEADRefData; 
class stTextImportExport; 
class scTypeSpecList;
class scSpecLocList;
class scTextline;

// events that the reformatter returns
typedef enum eReformatEvents {
	eNoReformat,				// no reformatting was performed
	eNormalReformat,			// normal reformatting event
	eRebreak,					// rebreak the paragraph, probably for widow/orphan control
	eOverflowGeometry,			// more text than columns
	eOverflowContent			// more columns than text
} eRefEvent;

/* ==================================================================== */
class scStreamChangeInfo {
public:
		// these are the paragraph and offset of character insertion
		// that is used to insure correct update with mono-spaced 
		// characters
	
					scStreamChangeInfo( ) :
						fColumn( 0 ),
						fPara( 0 ),
						fOffset( 0 ),
						fLength( 0 ){}
						
	void			Set( scColumn* col, scContUnit* para, long offset, long len ) {
						fColumn 	= col,
						fPara		= para,
						fOffset 	= offset,
						fLength 	= len;
					}

	scColumn*		GetColumn( void ) const 	{ return fColumn; }
	scContUnit* 	GetPara( void ) const		{ return fPara; }
	long			GetOffset( void ) const 	{ return fOffset; }

	long			GetLength( void ) const 	{ return fLength; }
	void			SetLength( long len )		{ fLength = len; }
	
private:
	scColumn*		fColumn;
	scContUnit* 	fPara;
	long			fOffset;
	long			fLength;
	
};

/* ==================================================================== */

class PrevParaData {
public:
				PrevParaData() 
					{
						Init();
					}
	void		Init( void )
					{ 
						lastLineH = 0;
						lastSpec.clear();
					 }
	scTextline* lastLineH;
	TypeSpec	lastSpec;
	
};

/* ==================================================================== */

class scCharArray : public scHandleArray {
	scDECLARE_RTTI;
public:
				scCharArray() :
						scHandleArray( sizeof( CharRecord ) )
						{
							CharRecord ch( 0, 0 );
							AppendData( (ElementPtr)&ch );		// add null terminator
						}


	virtual int 	IsEqual( const scObject& ) const;
						
	UCS2		GetCharAtOffset( long offset ) const
					{ return (((CharRecordP)GetMem()) + offset )->character; }

	void		RemoveBetweenOffsets( long startOffset, long endOffset );
			   
				// copy the contents from startOffset to endOffset into the 
				// arg scCharArray
	void		Copy( scCharArray&, long startOffset, long endOffset ) const;

				// copy the contents from startOffset to endOffset into the 
				// arg scCharArray and then remove them 
	void		Cut( scCharArray&, long, long );
				
				// paste the contents of the arg scCharArray into the character array
				// at the indicated array
	void		Paste( scCharArray&, long startOffset );

	int 		FindString( const USTR&, const SearchState&, int32, int32, int32& );
	int 		ReplaceToken( const USTR&, int32, int32& );
	int 		GetToken( USTR&, int32, int32 ) const;	

	void		Insert( const CharRecordP, long, long );
	void		Insert( const UCS2*, long, long );
	int 		Insert( const USTR&, int32, int32 );	
	
	void		CopyChars( UCS2*, long, long );

	
				// transform the indicated characters using the type of
				// transformation passed in, ususally for making
				// alternate characters
	void		Transform( long 		startOffset,
						   long 		endOffset,
						   eChTranType	trans,
						   int			numChars );

	void		Retabulate( scSpecRun&	specRun,
							long		start,
							long		end,
							TypeSpec	changedSpec,
							long		charSize );

	void		RepairText( scSpecRun&,
							long		offset1,
							long		offset2 );

	void		SelectWord( long	offset, 
							long&	startWord,
							long&	endWord );

#ifdef _RUBI_SUPPORT
	void		CharInsert( long&,
							scSpecRun&, 
							scRubiArray*, 
							long, 
							scKeyRecord&, 
							BOOL,
							TypeSpec );
#else
	void		CharInsert( long&,
							scSpecRun&, 
							long, 
							scKeyRecord&, 
							BOOL,
							TypeSpec );
#endif

	void		WordSpaceInfo( long, MicroPoint& );

	void		CharInfo( scSpecRun&, 
						  long, 
						  UCS2&, 
						  ulong&, 
						  MicroPoint&, 
						  TypeSpec&, 
						  eUnitType& );

	long		ReadText( scSpecRun&,
						  APPCtxPtr 	ctxPtr,
						  IOFuncPtr 	readFunc,
						  int			charset = 0 );

	void		WriteText( scSpecRun&,
						   BOOL,
						   APPCtxPtr	ctxPtr,
						   IOFuncPtr	writeFunc,
						   int			charset = 0 );

	long		ReadAPPText( scSpecRun&, stTextImportExport& );
	void		WriteAPPText( scSpecRun&, stTextImportExport& );

	long		GetContentSize( void ) const
					{ 
						return fNumItems - 1; 
					}
	void		SetContentSize( long );
	long		ExternalSize( void ) const;
	
	void		Read( APPCtxPtr, IOFuncPtr );
	void		Write( APPCtxPtr, IOFuncPtr );

	virtual ElementPtr	Lock( void );
	virtual void		Unlock( void );
	void				Validate( void ) const;

	
private:
	void		CopyChars( CharRecordP, long, long );
	
#ifdef _RUBI_SUPPORT
	void		DoBackSpace( long&,
							 long&,
							 scSpecRun&, 
							 scRubiArray*, 
							 long, 
							 scKeyRecord&, 
							 BOOL );

	void		DoForwardDelete( long&,
								 long&,
								 scSpecRun&, 
								 scRubiArray*, 
								 long, 
								 scKeyRecord&, 
								 BOOL );

	void		DoDiscHyphen( long&,
							  long&,
							  scSpecRun&, 
							  scRubiArray*, 
							  long, 
							  scKeyRecord&, 
							  BOOL );

	void		DoFixSpace( long&,
							long&,
							scSpecRun&, 
							scRubiArray*, 
							long, 
							scKeyRecord&, 
							BOOL );

	void		DoCharacter( long&,
							 long&,
							 scSpecRun&, 
							 scRubiArray*, 
							 long, 
							 scKeyRecord&, 
							 BOOL );
	
	
#else
	void		DoBackSpace( long&,
							 long&,
							 scSpecRun&, 
							 long, 
							 scKeyRecord&, 
							 BOOL );

	void		DoForwardDelete( long&,
								 long&,
								 scSpecRun&, 
								 long, 
								 scKeyRecord&, 
								 BOOL );

	void		DoDiscHyphen( long&,
							  long&,
							  scSpecRun&, 
							  long, 
							  scKeyRecord&, 
							  BOOL );

	void		DoFixSpace( long&,
							long&,
							scSpecRun&, 
							long, 
							scKeyRecord&, 
							BOOL );

	void		DoCharacter( long&,
							 long&,
							 scSpecRun&, 
							 long, 
							 scKeyRecord&, 
							 BOOL );
	

#endif

};
	
	
long		TXTStartWord( CharRecordP, long, int eleminateLeadingSpaces );
long		TXTEndWord( CharRecordP, long );
long		TXTStartSelectableWord( CharRecordP, long );
long		TXTEndSelectableWord( CharRecordP, long );

MicroPoint	UnivStringWidth( USTR&, MicroPoint[], TypeSpec& );

#ifdef jis4051
BOOL		TXTSameRenMoji( CharRecordP start, CharRecordP ch1, CharRecordP ch2 );
#else
inline BOOL TXTSameRenMoji( CharRecordP, CharRecordP, CharRecordP )
{
	return false;
}
#endif


class scContUnit : public scTBObj {
	scDECLARE_RTTI; 
public:

						// use this to allocate new content units where the content unit
						// has been overridden on the outside.
	static scContUnit* Allocate( TypeSpec&		spec,
								 scContUnit*	cu = 0,
								 long			ct = 0 );	

					scContUnit();
					scContUnit( TypeSpec& spec,
								scContUnit* cu = 0,
								long ct = 0 );	
					~scContUnit();

#if SCDEBUG > 1
	int 			fReformatEvent; 		// the tick the last time the paragraph
												// was reformatted - used only for debugging
	void			SetReformatEvent( int event )	{ fReformatEvent = event; }
#endif


	void			Free( scSelection* sel = 0 );

	void			InitParaSpec( TypeSpec& );

	void			LockMem( CharRecordP&, scSpecRecord*& );
	void			UnlockMem( void  );

	scStream*		GetStream( void ) const 	{ return (scStream*)FirstInChain(); }
	
	scTextline* 	LocateFirstLine( scCOLRefData&,
									 TypeSpec,
									 scColumn*&,
									 MicroPoint&,
									 scLEADRefData&,
									 PrevParaData& );

	scContUnit* 	KeySplit( long& 	offset,
							  scKeyRecord&	keyRec,
							  long& 	tmMove,
							  short&		rebreak );
	scContUnit* 	Split( long& );
	scContUnit* 	Merge( long& );

	
	void			Insert( const CharRecord&, 
							TypeSpec&,
							long );

	
	scContUnit* 	KeyInsert( long&,
							   scKeyRecord&,
							   long&,
							   short&,
							   BOOL,
							   TypeSpec,
							   BOOL& );
	
	void			CharInsert( long,
								long&,
								scKeyRecord&,
								long&,
								short&,
								BOOL,
								TypeSpec );


	scContUnit* 	Copy( scContUnit* ) const;


	status			Iter( SubstituteFunc func, long startOffset, long& limitOffset );
	
	virtual void	Mark( const scLogBits& bits );

#if SCDEBUG > 1
	eRefEvent		Reformat( scCOLRefData&, PrevParaData&, int, int& );
#else
	eRefEvent		Reformat( scCOLRefData&, PrevParaData&, int );
#endif

	void			Deformat( void );

	void			PostInsert( scContUnit* );
	
	void			Append( scContUnit* );
	void			Unlink( void );
	
	void			ReadAPPText( stTextImportExport& );
	void			WriteAPPText( stTextImportExport& );


	long			ReadStream( APPCtxPtr, IOFuncPtr );
	void			WriteStream( BOOL, APPCtxPtr, IOFuncPtr );


						// FILE I/O

						// complete the read
	virtual void	Read( scSet*, APPCtxPtr, IOFuncPtr );

						// complete the write
	virtual void	Write( APPCtxPtr, IOFuncPtr );

						// restore the pointers after completing a read 
	virtual void	RestorePointers( scSet* );

	void			PasteText( const scContUnit*, long& );
	BOOL			ClearText( long, long );

	scContUnit* 	PasteParas( const scContUnit*, long& );


	void			TextTrans( long, long, eChTranType, int );
							   
	
	TypeSpec		SpecAtEnd( void );
	TypeSpec		SpecAtOffset( long );
	TypeSpec		SpecAtStart( void );


						// does this para contain this spec
	BOOL			ContainTS( TypeSpec );

					
	void			GetTSList( scTypeSpecList& );
	void			OffsetGetTSList( long, long, scTypeSpecList& );

	void			OffsetGetCharSpecList( long, long, scSpecLocList& );
	
					// the chararray is null terminated, so to tell
					// use how many characters we have we need to subract
					// one from the array size
					//
	long			GetContentSize( void ) const	
						{ 
							return fCharArray.GetContentSize(); 
						}
	scCharArray&	GetCharArray( void )
						{ 
							return fCharArray;
						}

	scSpecRun&		GetSpecRun( void )		
						{
							return fSpecRun; 
						}
	const scSpecRun& GetSpecRun( void ) const
						{
							return fSpecRun; 
						}

						void			CopySpecRun( const scSpecRun& );

	void			SetDefaultSpec( TypeSpec& );
	TypeSpec&		GetDefaultSpec( void ) 
						{
							return defspec_;
						}
	TypeSpec		GetDefaultSpec( void ) const
						{
							return defspec_;
						}	

#ifdef _RUBI_SUPPORT
	BOOL			GetAnnotation( int nth, long, long, scAnnotation& );
	void			ApplyAnnotation( long, long, const scAnnotation& );

	void			AllocRubiArray( void );
	void			AllocRubiArray( const scRubiArray& );	
	void			DeleteRubiArray( void );
	void			CopyRubiArray( const scRubiArray& );

	scRubiArray*	GetRubiArray( void ) const	{ return fRubiArray; }
	BOOL			IsRubiPresent( size_t, size_t );
#endif
	
	int 			operator==( const scContUnit& ) const;
	int 			operator!=( const scContUnit& p2 ) const { return !(*this == p2); }

	
	scContUnit* 	GetPrev( void ) const	{ return (scContUnit*)Prev(); } 
	scContUnit* 	GetNext( void ) const	{ return (scContUnit*)Next(); } 


	scTextline* 	NextColumn( scColumn*& );
	
	void			SetFirstline( scTextline* firstline )	{ fFirstline = firstline; }
	scTextline* 	GetFirstline( void ) const				{ return fFirstline; }

						// find the line associated with the indicated offset
	scTextline* 	FindLine( long offset ) const;


	scTextline* 	GetLastline( void ) const;
	scTextline* 	GetLastVisibleLine( void ) const;
	scContUnit* 	GetLastVisiblePara( void ) const;
	scContUnit* 	GetPrevVisiblePara( void ) const;
										
					// this returns the column that this paragraph starts in
	scColumn*		GetFirstCol( void ) const;
	

	scContUnit* 	Earlier( const scContUnit* ) const;
	
	long			GetCount( void ) const	{ return fParaCount; }
	void			SetCount( long cnt )	{ fParaCount = cnt; }

						// do this paragraph and this column intersect
	BOOL			ColSect( const scColumn* ) const;

	void			ForceRepaint( long, long );


	void			Renumber( void );
	void			Retabulate( TypeSpec ts );
	void			SelectWord( long, long&, long& );
	void			SetStyle( long, long, TypeSpec, BOOL retabulate, BOOL forceRepaint );
	
	scContUnit* 	CopyText( long, long ) const;
	
#if SCDEBUG > 1
	virtual void	scAssertValid( BOOL recurse = true ) const;
	virtual void	DbgPrintInfo( int debugLevel = 0 ) const;
	void			DebugParaSpecs();
#else	
	virtual void	scAssertValid( BOOL = true ) const{}
#endif


	void			ChInfo( long, UCS2&, 
							ulong&, MicroPoint&, MicroPoint&, 
							TypeSpec&, eUnitType& );
	
	long			ExternalSize( void ) const;
	
	BOOL			FindLocation( long&, BOOL&, scTextline*&, MicroPoint&, eContentMovement );
	
	void			FreeScrap( long& );
	
	int 			ReplaceToken( const USTR&, int32, int32& );
	BOOL			ReplaceWord( long startOffset, long& endOffset, long& limitOffset, const UCS2* replaceBuf );

protected:

#if SCDEBUG > 1
	eRefEvent		Reformat2( scCOLRefData&, PrevParaData&, int, int&, int& );
#else
	eRefEvent		Reformat2( scCOLRefData&, PrevParaData&, int, int& );
#endif

	BOOL			ResetWidow( scCOLRefData&	cData,
								BOOL			testGetStrip );


	scTextline* 	fFirstline; 	/* firstline of paragraph */		

	long			fParaCount; 	/* the # of this para in the stream */
	
	TypeSpec		defspec_;

	scSpecRun		fSpecRun;
	
	scCharArray 	fCharArray; 	/* the charArray */
	
#ifdef _RUBI_SUPPORT
	scRubiArray*	fRubiArray;
#endif
};

inline UCS2 PARACharAtOffset( scContUnit* p, long offset )
					{ return p->GetCharArray().GetCharAtOffset( offset ); }

/* ==================================================================== */

#define NEXT_LINE				LONG_MAX
#define PREV_LINE				LONG_MIN

#define PARAFirstInChain(p) ((scContUnit*)p->FirstInChain( ))
#define PARAChSize( p ) 	( (p)->GetCharArray().GetContentSize() )

class stPara {
public:
		stPara();
		stPara( TypeSpec& );

		~stPara();

	// this should be executed first
	void append( TypeSpec& );
	
	void append( const uchar* str, int );
	void append( USTR& );	
	void append( UCS2 );
	
	int get( UCS2&, TypeSpec& );
	void reset()
	{
		choffset_ = 0;
	}
	
	TypeSpec& paraspec()
	{
		return paraspec_;
	}

	// CAUTION the semantics this are a bit bizarre
	stPara& operator=( const stPara& );

	void setparaspec( TypeSpec& ts );

	int validate() const;
	int complete();
	
private:
	TypeSpec paraspec_;
	vector<UCS2> ch_;
	scSpecRun specs_;
	int32 choffset_;
};
