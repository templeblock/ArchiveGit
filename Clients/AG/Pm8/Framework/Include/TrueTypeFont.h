// ****************************************************************************
//
//  File Name:			TrueTypeFont.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RTrueTypeFont class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/TrueTypeFont.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_TRUETYPEFONT_H_
#define		_TRUETYPEFONT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#ifdef	MAC
	typedef long 	fontTableTag;
	typedef Fixed 	Matrix[3][3];
	typedef struct FixPoint 
	{
		Fixed	x;
		Fixed	y;
	} FixPoint;
	typedef struct GlyphOutline 
	{
		long			contourCount;
		long			pointCount;
		FixPoint		origin;
		FixPoint		advance;
		short**		endPoints;		// [contourCount] 
		Byte**		onCurve;			// [pointCount] 
		Fixed**		x;					// [pointCount] 
		Fixed**		y;					// [pointCount] 
	} GlyphOutline;
	typedef struct FontTableInfo 
	{
		long			offset;			// from beginning of sfnt to beginning of the table 
		long			length;			// length of the table 
		long			checkSum;		// checkSum of the table 
	} FontTableInfo;
#endif	// MAC


// ****************************************************************************
//
//  Class Name:	RTrueTypeFont
//
//  Description:	The Renaissance True Type Font information class. All information
//						relavent to a font will be retrieved from this class.
//
// ****************************************************************************
//
class	FrameworkLinkage RTrueTypeFont : public RFont
{
	//	Construction, Destruction & Initialization
	public :
												RTrueTypeFont( const YFontInfo& info, BOOLEAN fInitialize );
		virtual 								~RTrueTypeFont( );

	//	Operations
	public :
		virtual RPath*						GetGlyphPath( const RCharacter& character );

	// Implementation
	private :
		BOOLEAN								ExtractCharacterOutline( int character, uLONG cookie );
#ifdef	MAC
		// get handle to sfnt of font family with given style
		static Handle 						GetSfntHandle( Handle fond, short styleWord );
		static Handle 						GetSfntHandle( Ptr fontName, short styleWord );
		static Handle 						GetSfntHandle( short txFont, short styleWord );
		// allocate initial arrays for GlyphOutline structure
		static void		 					InitGlyphOutline( GlyphOutline* glyph );
		// dispose memory allocated for GlyphOutline structure
		static void 						KillGlyphOutline( GlyphOutline* glyph );
		//	lock/unlock arrays in GlyphOutline structure
		static void 						LockGlyphOutline( GlyphOutline* glyph );
		static void 						UnlockGlyphOutline( GlyphOutline* glyph );
		//	manipulate arrays in GlyphOutline structure
		static void 						AppendGlyphOutline( GlyphOutline* a, GlyphOutline* b );
		static void 						MoveGlyphOutline( GlyphOutline* out, Fixed xDelta, Fixed yDelta );
		static void 						ScaleGlyphOutline( GlyphOutline* out, Fixed xScale, Fixed yScale );
		// parse font data
		static void* 						GetSfntTablePtr( Handle sfnt, fontTableTag tag );
		static void* 						GetSfntGlyphPtr( Handle sfnt, long glyphIndex, long* length );
		static long 						CountSfntTables( Handle sfnt );
		static fontTableTag 				GetSfntTag( Handle sfnt, long index );
		static OSErr 						GetSfntTableInfo( Handle sfnt, fontTableTag tag, FontTableInfo* fTable );
		static long							GetCharGlyphIndex( Handle sfnt, unsigned short charCode );
		// extract font data
		static void 						GetGlyphOutline( Handle sfnt, long glyphIndex, GlyphOutline* gOutline, Matrix xform );
		// utils
		static void 						myAppendHandle( Handle dst, Handle extra );
		static void 						mySetHandleSize( Handle hdl, Size size );
		static void 						InitMatrix( Matrix mat );
		static void 						MakeIdentityMatrix( Matrix mat );
		static short 						GetFontState( Handle sfnt );
		static void 	 					SetFontState( Handle sfnt, short state );
#endif

	// Member Data
	private :
		HTMEM									m_hSfnt;
		sWORD									m_sSfntId;

	//	Private structures for class use
	private :
		typedef struct tagYTTSegmentInfoRecord
		{
			short							opCode; 
			RIntPoint					pt1;
			RIntPoint					pt2;
			RIntPoint					pt3;
		}	YTTSegmentInfoRecord;

#ifdef	TPSDEBUG
	//	Debugging Support
	public :
		virtual	void						Validate( ) const;
#endif
} ;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_TRUETYPEFONT_H_
