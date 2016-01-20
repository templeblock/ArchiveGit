// ****************************************************************************
//
//  File Name:			MacTrueTypeFont.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				R. Hood
//
//  Description:		Mac rountines to extract font data for RTrueTypeFont class
//
//  Portability:		Mac platform dependent
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
//  $Logfile:: /PM8/Framework/Source/MacTrueTypeFont.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	<script.h>
#include	"FrameworkIncludes.h"

ASSERTNAME

#ifndef	IMAGER
#include "ApplicationGlobals.h"
#else
#include "GlobalBuffer.h"
#endif

#include	"TrueTypeFont.h"
#include "sfnt.h"

// font header data defs
typedef enum 
{
	plat_Unicode,
	plat_Macintosh,
	plat_ISO,
	plat_MS
} platformEnums;

#define tag_FontHeader				0x68656164				// 'head' 
#define tag_HoriHeader				0x68686561				// 'hhea' 
#define tag_VertHeader				0x76686561				// 'vhea' 
#define tag_IndexToLoc				0x6c6f6361				// 'loca' 
#define tag_MaxProfile				0x6d617870				// 'maxp' 
#define tag_ControlValue			0x63767420				// 'cvt ' 
#define tag_PreProgram				0x70726570				// 'prep' 
#define tag_GlyphData				0x676c7966				// 'glyf' 
#define tag_HorizontalMetrics		0x686d7478				// 'hmtx' 
#define tag_VerticalMetrics		0x766d7478				// 'vmtx' 
#define tag_CharToIndexMap			0x636d6170				// 'cmap' 
#define tag_FontProgram				0x6670676d				// 'fpgm' 
#define tag_Kerning					0x6b65726e				// 'kern' 
#define tag_HoriDeviceMetrics		0x68646d78 				// 'hdmx' 
#define tag_NamingTable				0x6e616d65				// 'name' 
#define tag_PostScript				0x706f7374				// 'post' 

//	macros
#define GetUnsignedByte( p ) 		((Byte)(*p++))
#define RoundFixed( f ) 			( ( f < 0 ) ? -FixRound( -f ) : FixRound( f ) )


// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::ExtractCharacterOutline( )
//
//  Description:		Retrieve a glyph outline and parse it into segment records 
//							in the global buffer
//
//  Returns:			Boolean indicating successful completion
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
BOOLEAN	RTrueTypeFont::ExtractCharacterOutline( int character, uLONG cookie )
{
YTTSegmentInfoRecord*	pSegments = (YTTSegmentInfoRecord *)( cookie + sizeof(uLONG) );
const YFontInfo			fontInfo = GetInfo();
GlyphOutline 				glyph;
Matrix 						matrix;
long 							glyphIndex = 0;					// zero is the missing character

	// retrieve font resource
	if ( m_hSfnt == NULL )
	{
		short		rId;
		ResType	rType;
		Str255	rName;
		m_hSfnt = GetSfntHandle( (const LPSZ)fontInfo.sbName, RFont::GetMacStyleBits( fontInfo.attributes ) );
		GetResInfo( m_hSfnt, &rId, &rType, rName);
		m_sSfntId = rId;
	}
	else if ( *m_hSfnt == NULL )
	{
		::LoadResource( m_hSfnt );
//		m_hSfnt = ::GetResource( 'sfnt', m_sSfntId );
	}
	if ( m_hSfnt == NULL || *m_hSfnt == NULL )
		return FALSE;
	::HNoPurge( m_hSfnt );

	// extract character outline
	InitMatrix( matrix );
	InitGlyphOutline( &glyph );
	MakeIdentityMatrix( matrix );
	try
	{
		glyphIndex = GetCharGlyphIndex( m_hSfnt, character );
		GetGlyphOutline( m_hSfnt, glyphIndex, &glyph, matrix );
		ScaleGlyphOutline( &glyph, ::Long2Fix( fontInfo.height ), ::Long2Fix( fontInfo.height ) );
	}
	catch ( YException except )
	{
		::HPurge( m_hSfnt );
		KillGlyphOutline( &glyph );
		switch ( except ) 
		{
			case kResource:
				return FALSE;
				break;
			default:
				throw;
				break;
		}
	}
	catch ( ... )
	{
		::HPurge( m_hSfnt );
		KillGlyphOutline( &glyph );
		throw;
	}

	// loop thru the contours
	LockGlyphOutline( &glyph );
	{
		long			nrSegments = 0;
		long			sp = 0;
		Fixed*		x = *glyph.x;
		Fixed*		y = *glyph.y;
		short*		ep = *glyph.endPoints;
		Byte*			onCurve = *glyph.onCurve;
		RIntPoint	ptStart;
		for ( int i = 0; i < glyph.contourCount; i++ )
		{	
			long	pts = *ep - sp + 1;													// nr pts in contour
			// contour start point
			if ( *onCurve != 0 )															// 1st point on curve
			{
				ptStart.m_x = RoundFixed( *x );
				ptStart.m_y = -RoundFixed( *y ); 
				x++;
				y++;
				onCurve++;
				pts--;
			}
			else if ( *((*glyph.onCurve) + *ep) != 0 )							// use end point
			{
				ptStart.m_x = RoundFixed( *((*glyph.x) + *ep) );
				ptStart.m_y = -RoundFixed( *((*glyph.y) + *ep) ); 
			}
			else																				// compute midpoint between 1st and last curve points
			{	
				Fixed	x2 = ::FixDiv( (*x + *((*glyph.x) + *ep)), ::Long2Fix( 2 ) );
				Fixed	y2 = ::FixDiv( (*y + *((*glyph.y) + *ep)), ::Long2Fix( 2 ) );
				ptStart.m_x = RoundFixed( x2 );
				ptStart.m_y = -RoundFixed( y2 );
			}
			// initial move to
			pSegments->opCode = MOVE_TO;
			pSegments->pt1 = ptStart;
			nrSegments++;
			pSegments++;
			// load segments of contour
			while ( pts-- > 0 )
			{	
				Fixed	x0 = *x;
				Fixed	y0 = *y;
				Byte	onCurve0 = *onCurve;
				x++;
				y++;
				onCurve++;
				pSegments->pt1.m_x = RoundFixed( x0 );
				pSegments->pt1.m_y = -RoundFixed( y0 );
				if ( onCurve0 != 0 )
					pSegments->opCode = LINE_TO;
				else if ( pts < 1 )														// quadratic w/ contour start point as end
				{
					pSegments->opCode = QUADRATIC_TO;
					pSegments->pt2 = ptStart;
				}
				else if ( *onCurve != 0 )												// quadratic w/ end point next line to
				{
					pSegments->opCode = QUADRATIC_TO;
					pSegments->pt2.m_x = RoundFixed( *x );
					pSegments->pt2.m_y = -RoundFixed( *y );
					x++;
					y++;
					onCurve++;
					pts--;
				}
				else 																			// compute end point of quadratic as midpoint to next curve
				{
					Fixed	x2 = ::FixDiv( (x0 + *x), ::Long2Fix( 2 ) );
					Fixed	y2 = ::FixDiv( (y0 + *y), ::Long2Fix( 2 ) );
					pSegments->opCode = QUADRATIC_TO;
					pSegments->pt2.m_x = RoundFixed( x2 );
					pSegments->pt2.m_y = -RoundFixed( y2 );
				}
				nrSegments++;
				pSegments++;
			}
			pSegments->opCode = CLOSE_PATH;											// end of contour
			pSegments->pt1.m_x = pSegments->pt2.m_x = 0;
			pSegments->pt1.m_y = pSegments->pt2.m_y = 0;
			nrSegments++;
			pSegments++;
			sp = *ep++ + 1;
		}
		pSegments->opCode = 0;															// end of glyph
		pSegments->pt1.m_x = pSegments->pt2.m_x = 0;
		pSegments->pt1.m_y = pSegments->pt2.m_y = 0;
		*(uLONG *)cookie = nrSegments;
	}
	UnlockGlyphOutline( &glyph );

	// cleanup
	::HPurge( m_hSfnt );
	KillGlyphOutline( &glyph );

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::myAppendHandle( )
//
//  Description:		Append handle which throws an exception on error
//
//  Returns:			Nothing
//
//  Exceptions:		Mac OS Memory
//
// ****************************************************************************
//
void RTrueTypeFont::myAppendHandle( Handle dst, Handle extra )
{
	OSErr	osErr = ::HandAndHand( extra, dst );
	if ( osErr != noErr )
		throw memFullErr;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::mySetHandleSize( )
//
//  Description:		Set handle size which throws an exception on error
//
//  Returns:			Nothing
//
//  Exceptions:		Mac OS Memory
//
// ****************************************************************************
//
void RTrueTypeFont::mySetHandleSize( Handle hdl, Size size )
{
OSErr			osErr;

	::SetHandleSize( hdl, size );
	if ( (osErr = ::MemError()) != noErr )
		throw memFullErr;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::InitMatrix( )
//
//  Description:		Initialize transform matrix to identity
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::InitMatrix( Matrix mat )
{
	mat[0][0] = mat[1][1] = mat[2][2] = 0x10000;
	mat[0][1] = mat[0][2] = mat[1][0] = mat[1][2] = mat[2][0] = mat[2][1] = 0;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::MakeIdentityMatrix( )
//
//  Description:		Initialize transform matrix to identity
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::MakeIdentityMatrix( Matrix mat )
{
	Fixed* m = (Fixed*)mat;
	*m++ = 1L << 16; *m++ = 0; *m++ = 0;
	*m++ = 0; *m++ = 1L << 16; *m++ = 0;
//	*m++ = 0; *m++ = 0; *m++ = 1L << 30;
	*m++ = 0; *m++ = 0; *m++ = 1L << 16;						// REVEIW RAH a rather stupid error.  why didn't apple catch it?
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetSfntHandle( )
//
//  Description:		Given a font family resource and style types, return the
//							truetype font resource
//
//  Returns:			Mac Handle
//
//  Exceptions:		None
//
// ****************************************************************************
//
Handle RTrueTypeFont::GetSfntHandle( Handle fond, short styleWord )
{
FamRec* 	frec;
short* 	assoc;
short 	sfntID = 0;
short		count;
Handle 	sfnt = nil;

	if ( fond != nil )
	{	
		frec = (FamRec *)*fond;
		assoc = (short *)(frec + 1);
		count = *assoc++;
		while ( count-- >= 0 && sfntID == 0 )
		{	
			if ( *assoc++ == 0 )					// size == 0 means sfnt
				if ( *assoc++ == styleWord )
					sfntID = *assoc;
				else
					assoc++;
			else
				assoc += 2;
		}
		if ( sfntID != 0 )
		{	 
			sfnt = ::GetResource( 'sfnt', sfntID );
			if ( sfnt != nil && ::ResError() == noErr )
				return sfnt;
		}
	}
	return nil;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetSfntHandle( )
//
//  Description:		Given a font family name and style types, return the
//							truetype font resource
//
//  Returns:			Mac Handle
//
//  Exceptions:		None
//
// ****************************************************************************
//
Handle RTrueTypeFont::GetSfntHandle( char* szName, short styleWord )
{
Handle 	fond;
Handle 	sfnt = nil;
Str255	name;

	strcpy( (Ptr)&name[1], szName );
	*name = strlen( szName );
	fond = ::GetNamedResource( 'FOND', name );
	if ( fond != nil && ::ResError() == noErr )
		sfnt = GetSfntHandle( fond, styleWord );
	::HPurge( fond );
	return sfnt;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetSfntHandle( )
//
//  Description:		Given a font family resource Id and style types, return the
//							truetype font resource
//
//  Returns:			Mac Handle
//
//  Exceptions:		None
//
// ****************************************************************************
//
Handle RTrueTypeFont::GetSfntHandle( short fondID, short styleWord )
{
Handle 	fond = ::GetResource( 'FOND', fondID );
Handle 	sfnt = nil;

	if ( fond != nil && ::ResError() == noErr )
		sfnt = GetSfntHandle( fond, styleWord );
	::HPurge( fond );
	return sfnt;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::InitGlyphOutline( )
//
//  Description:		Initialize a truetype gylph structure
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::InitGlyphOutline( GlyphOutline* glyph )
{
	glyph->contourCount = 0;
	glyph->pointCount = 0;
	glyph->origin.x = 0;
	glyph->origin.y = 0;
	glyph->advance.x = 0;
	glyph->advance.y = 0;
	if ( (glyph->endPoints = (short**)::NewHandle( 0 ) ) == nil )
		throw kMemory;
	if ( (glyph->onCurve = (Byte**)::NewHandle( 0 ) ) == nil )
		throw kMemory;
	if ( (glyph->x = (Fixed**)::NewHandle( 0 ) ) == nil )
		throw kMemory;
	if ( (glyph->y = (Fixed**)::NewHandle( 0 ) ) == nil )
		throw kMemory;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::KillGlyphOutline( )
//
//  Description:		Free a truetype gylph structure
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::KillGlyphOutline( GlyphOutline* glyph )
{
	::DisposHandle( (Handle)glyph->endPoints );
	::DisposHandle( (Handle)glyph->onCurve );
	::DisposHandle( (Handle)glyph->x );
	::DisposHandle( (Handle)glyph->y );
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::LockGlyphOutline( )
//
//  Description:		Lock a truetype gylph structure
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::LockGlyphOutline( GlyphOutline* glyph )
{
	::HLock( (Handle)glyph->endPoints );
	::HLock( (Handle)glyph->onCurve );
	::HLock( (Handle)glyph->x );
	::HLock( (Handle)glyph->y );
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::UnlockGlyphOutline( )
//
//  Description:		Unlock a truetype gylph structure
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::UnlockGlyphOutline( GlyphOutline* glyph )
{
	::HUnlock( (Handle)glyph->endPoints );
	::HUnlock( (Handle)glyph->onCurve );
	::HUnlock( (Handle)glyph->x );
	::HUnlock( (Handle)glyph->y );
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetFontState( )
//
//  Description:		
//
//  Returns:			State of handle before call
//
//  Exceptions:		None
//
// ****************************************************************************
//
short RTrueTypeFont::GetFontState( Handle sfnt )
{
short 	state;

	::LoadResource( sfnt );
	state = ::HGetState( sfnt );
	::HNoPurge( sfnt );
	return state;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::SetFontState( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::SetFontState( Handle sfnt, short state )
{
	::HSetState( sfnt, state );
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::SetFontState( )
//
//  Description:		Returns the glyph index for the given character code
//
//  Returns:			glyph index or 0
//
//  Exceptions:		None
//
// ****************************************************************************
//
long RTrueTypeFont::GetCharGlyphIndex( Handle sfnt, unsigned short charCode )
{
short 							state = GetFontState( sfnt );
sfnt_char2IndexDirectory*	table = (sfnt_char2IndexDirectory *)GetSfntTablePtr( sfnt, tag_CharToIndexMap );
short 							i;
long 								mapOffset = 0;
long 								glyphIndex = 0;				// missing character glyph 
sfnt_platformEntry*			plat;
sfnt_mappingTable*			mapping;

	if ( table == NULL )
		return 0;

	plat = table->platform;

	// You can change this section to look for other scripts
	for ( i = table->numTables - 1; i >= 0; --i )
	{	
		if ( plat->platformID == plat_Macintosh && plat->specificID == smRoman )
		{	
			mapOffset = plat->offset;
			break;
		}
		++plat;
	}

	if ( mapOffset )
	{	
		mapping = (sfnt_mappingTable*)((char*)table + mapOffset);
		
		switch ( mapping->format ) 
		{
			case 0:
				{	
					Byte* glyphs = (Byte*)(mapping + 1);
					glyphIndex = glyphs[charCode];
				}
				break;
			case 6:
				{	
					short* glyphs = (short*)(mapping + 1);
					short first = *glyphs++;
					short count = *glyphs++;
					charCode -= first;
					if (charCode < count)
						glyphIndex = glyphs[charCode];
				}
				break;
			default:
				glyphIndex = 0;
		}
	}
	else
		glyphIndex = 0;

	SetFontState( sfnt, state );
	return glyphIndex;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::AppendGlyphOutline( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		Memeory
//
// ****************************************************************************
//
void RTrueTypeFont::AppendGlyphOutline( GlyphOutline* a, GlyphOutline* b )
{
	myAppendHandle( (Handle)a->endPoints, (Handle)b->endPoints );
	{	
		short*	p = *a->endPoints + a->contourCount;
		short*	endp = p + b->contourCount;
		short		newFirstPoint = a->contourCount ? p[-1] + 1 : 0;
		for ( ; p < endp; p++ )
			*p = *p + newFirstPoint;
	}
	a->contourCount += b->contourCount;
	a->pointCount += b->pointCount;
	myAppendHandle( (Handle)a->onCurve, (Handle)b->onCurve );
	myAppendHandle( (Handle)a->x, (Handle)b->x );
	myAppendHandle( (Handle)a->y, (Handle)b->y );
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::MoveGlyphOutline( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::MoveGlyphOutline( GlyphOutline* glyph, Fixed xDelta, Fixed yDelta )
{	
Fixed* 	x = *glyph->x;
Fixed* 	y = *glyph->y;
short 	count = glyph->pointCount;

	for ( --count; count >= 0; --count )
	{	
		*x += xDelta;
		x++;
		*y += yDelta;
		y++;
	}
	glyph->origin.x += xDelta;
	glyph->origin.y += yDelta;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::ScaleGlyphOutline( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::ScaleGlyphOutline( GlyphOutline* glyph, Fixed xScale, Fixed yScale )
{	
Fixed* 	x = *glyph->x;
Fixed* 	y = *glyph->y;
short 	count = glyph->pointCount;

	for ( --count; count >= 0; --count )
	{	
		*x = ::FixMul( *x, xScale );
		x++;
		*y = ::FixMul( *y, yScale );
		y++;
	}
	glyph->origin.x = ::FixMul( glyph->origin.x, xScale );
	glyph->origin.y = ::FixMul( glyph->origin.y, yScale );
	glyph->advance.x = ::FixMul( glyph->advance.x, xScale );
	glyph->advance.y = ::FixMul( glyph->advance.y, yScale );
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::CountSfntTables( )
//
//  Description:		
//
//  Returns:			long
//
//  Exceptions:		None
//
// ****************************************************************************
//
long RTrueTypeFont::CountSfntTables( Handle sfnt )
{
short 	state = GetFontState( sfnt );
short 	tables = ((sfnt_OffsetTable*)*sfnt)->numOffsets;

	SetFontState( sfnt, state );
	return tables;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetSfntTag( )
//
//  Description:		
//
//  Returns:			Tag
//
//  Exceptions:		None
//
// ****************************************************************************
//
fontTableTag RTrueTypeFont::GetSfntTag( Handle sfnt, long index )
{
fontTableTag 	tag = 0;
short 			state = GetFontState( sfnt );

	if ( index < ((sfnt_OffsetTable*)*sfnt)->numOffsets )
		tag = ((sfnt_OffsetTable*)*sfnt)->table[index].tableTag;
	SetFontState( sfnt, state );
	return tag;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetSfntTableInfo( )
//
//  Description:		
//
//  Returns:			Mac OS error
//
//  Exceptions:		None
//
// ****************************************************************************
//
OSErr RTrueTypeFont::GetSfntTableInfo( Handle sfnt, fontTableTag tag, FontTableInfo* fTable )
{
short 					state = GetFontState( sfnt );
sfnt_OffsetTable* 	dir = (sfnt_OffsetTable*)*sfnt;
sfnt_DirectoryEntry*	table = dir->table;
short 					count = dir->numOffsets;
OSErr		 				error = noErr;

	for ( ; --count >= 0; table++ )
		if ( table->tableTag == tag )
		{	
			fTable->offset = table->offset;
			fTable->length = table->length;
			fTable->checkSum = table->checkSum;
			break;
		}
	if ( count < 0 )
		error = fontNotOutlineErr;
	SetFontState( sfnt, state );
	return error;
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetSfntTablePtr( )
//
//  Description:		
//
//  Returns:			Pointer to table
//
//  Exceptions:		None
//
// ****************************************************************************
//
void* RTrueTypeFont::GetSfntTablePtr( Handle sfnt, fontTableTag tag )
{
FontTableInfo 	info;

	if ( GetSfntTableInfo( sfnt, tag, &info ) )
		return 0;
	return( *sfnt + info.offset );
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetSfntGlyphPtr( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
void* RTrueTypeFont::GetSfntGlyphPtr( Handle sfnt, long glyphIndex, long* length )
{
void* 				loc = GetSfntTablePtr( sfnt, tag_IndexToLoc );
char* 				glyphStart = (char *)GetSfntTablePtr( sfnt, tag_GlyphData );
sfnt_FontHeader*	head = (sfnt_FontHeader *)GetSfntTablePtr( sfnt, tag_FontHeader );

	if ( head == NULL || loc == NULL || glyphStart == NULL )
		return 0;

	if ( head->indexToLocFormat == SHORT_INDEX_TO_LOC_FORMAT )
	{	
		unsigned short*	offset = (unsigned short *)loc + glyphIndex;
		*length = (long)(offset[1] - *offset) << 1;
		return glyphStart + ((long)*offset << 1);
	}
	else
	{	
		long*		offset = (long *)loc + glyphIndex;
		*length = offset[1] - *offset;
		return glyphStart + *offset;
	}
}


// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetGlyphOutline( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		Memory, Font
//
// ****************************************************************************
//
void RTrueTypeFont::GetGlyphOutline( Handle sfnt, long glyphIndex, GlyphOutline* pOutline, Matrix xform )
{
short 						state = GetFontState( sfnt );
short 						upem, sideBearing, adjustToLsb;
short* 						glyph;
sfnt_FontHeader* 			head;
sfnt_HorizontalHeader*	hhea;
sfnt_HorizontalMetrics*	hori;
long 							length;
long 							longMetrics;

	try
	{
		::HLock( sfnt );
		head = (sfnt_FontHeader *)GetSfntTablePtr( sfnt, tag_FontHeader );
		hhea = (sfnt_HorizontalHeader *)GetSfntTablePtr( sfnt, tag_HoriHeader );
		hori = (sfnt_HorizontalMetrics *)GetSfntTablePtr( sfnt, tag_HorizontalMetrics );
		if ( head == NULL || hhea == NULL || hori == NULL )
			throw fontNotOutlineErr;
	
		upem = head->unitsPerEm;
		longMetrics = hhea->numberLongMetrics;
		if ( glyphIndex < longMetrics )
		{	
			pOutline->advance.x = ::FixRatio( hori[glyphIndex].advance, upem );
			sideBearing = hori[glyphIndex].sideBearing;
		}
		else
		{	
			short *lsb = (short *)&hori[longMetrics]; 		// first entry after[AW,LSB] array 
			pOutline->advance.x = ::FixRatio( hori[longMetrics-1].advance, upem );
			sideBearing = hori[glyphIndex - longMetrics].sideBearing;
		}
		pOutline->advance.y = 0;
	
		pOutline->origin.x = pOutline->origin.y = 0;
	
		if ( (glyph = (short *)GetSfntGlyphPtr(sfnt, glyphIndex, &length)) == 0 )
			throw fontNotOutlineErr;
	
		if ( length == 0 )
		{	
			pOutline->contourCount = pOutline->pointCount = 0;
			SetFontState( sfnt, state );
			return;
		}
	
		pOutline->contourCount = *glyph++;
		adjustToLsb = *glyph - sideBearing;			// xmin - lsb 
		glyph += 4;											// skip bounds rect 
	
		if ( pOutline->contourCount == 0 )
			pOutline->pointCount = 0;
		else if ( pOutline->contourCount == -1 )
		{	
			short	flags, index, newMatrix;
			pOutline->contourCount = pOutline->pointCount = 0;
			mySetHandleSize( (Handle)pOutline->endPoints, 0 );
			mySetHandleSize( (Handle)pOutline->onCurve, 0 );
			mySetHandleSize( (Handle)pOutline->x, 0 );
			mySetHandleSize( (Handle)pOutline->y, 0 );
			do
			{	
				Matrix compXform;
				short arg1, arg2;
				
				flags = *glyph++;
				index = *glyph++;
				newMatrix = false;
	
				if ( flags & ARG_1_AND_2_ARE_WORDS )
				{	
					arg1 = *glyph++;
					arg2 = *glyph++;
				}
				else
				{	
					char* byteP = (char*)glyph;
					if ( flags & ARGS_ARE_XY_VALUES )
					{									// offsets are signed 
						arg1 = *byteP++;
						arg2 = *byteP;
					}
					else
					{									// anchor points are unsigned 
						arg1 = (unsigned char)*byteP++;
						arg2 = (unsigned char)*byteP;
					}
					++glyph;
				}
#if IMPLEMENT_SCALED_COMPONENTS
				if ( flags & (WE_HAVE_A_SCALE | WE_HAVE_AN_X_AND_Y_SCALE | WE_HAVE_A_TWO_BY_TWO) )
				{	
					Matrix subXform;
					MakeIdentityMatrix( subXform );
					if ( flags & WE_HAVE_A_TWO_BY_TWO )
					{	compXform[0][0] = (Fixed)*glyph++ << 2;
						compXform[0][1] = (Fixed)*glyph++ << 2;
						compXform[1][0] = (Fixed)*glyph++ << 2;
						compXform[1][1] = (Fixed)*glyph++ << 2;
					}
					else if ( flags & WE_HAVE_AN_X_AND_Y_SCALE )
					{	compXform[0][0] = (Fixed)*glyph++ << 2;
						compXform[1][1] = (Fixed)*glyph++ << 2;
					}
					else
						compXform[0][0] = compXform[1][1] = (Fixed)*glyph++ << 2;
					PostMulMatrix (compXform, xform );
					newMatrix = true;
				}
#endif			
				{	
					GlyphOutline out;
					InitGlyphOutline( &out );
					GetGlyphOutline( sfnt, index, &out, newMatrix ? compXform : xform );
					{	
						Fixed dx, dy;
						if ( flags & ARGS_ARE_XY_VALUES )
						{	dx = ::FixRatio(arg1, upem);
							dy = -::FixRatio(arg2, upem);
						}
						else
						{	
							dx = (*pOutline->x)[arg1] - (*out.x)[arg2];
							dy = (*pOutline->y)[arg1] - (*out.y)[arg2];
						}
						MoveGlyphOutline( &out, dx, dy );
					}
					AppendGlyphOutline( pOutline, &out );
					KillGlyphOutline( &out );
				}
			} while ( flags & MORE_COMPONENTS );
		}
		else if ( pOutline->contourCount > 0 )
		{																			//	Load in the end points.
			{	
				long size = pOutline->contourCount * sizeof(short);
	
				mySetHandleSize( (Handle)pOutline->endPoints, size );
				BlockMove( (Ptr)glyph, (Ptr)*pOutline->endPoints, size );
				glyph += pOutline->contourCount;
			}
	
			pOutline->pointCount = (*pOutline->endPoints)[pOutline->contourCount - 1] + 1;
			mySetHandleSize( (Handle)pOutline->onCurve, pOutline->pointCount * sizeof(char) );
			mySetHandleSize( (Handle)pOutline->x, pOutline->pointCount * sizeof(Fixed) );
			mySetHandleSize( (Handle)pOutline->y, pOutline->pointCount * sizeof(Fixed) );
	
			//	Skip the word for instruction count + the instructions.
			// Then load in the onCurve bytes.
			{	
				Byte* p = (Byte*)glyph + sizeof(short) + *glyph;
				Byte* onCurve = *pOutline->onCurve;
				Byte* stop = onCurve + pOutline->pointCount;
				Byte flag;
	
				while ( onCurve < stop )
				{	
					*onCurve++ = flag = GetUnsignedByte( p );
					if ( flag & REPEAT_FLAGS ) 
					{
						short count = GetUnsignedByte( p );
						for ( --count; count >= 0; --count )
							*onCurve++ = flag;
					}
				}
				//	Lets do X
				{	
					short coord = adjustToLsb;
					Fixed* x = *pOutline->x;
	
					onCurve = *pOutline->onCurve;
					while ( onCurve < stop )
					{	
						if ( (flag = *onCurve++) & XSHORT ) 
						{
							if ( flag & SHORT_X_IS_POS )
								coord += GetUnsignedByte( p );
							else
								coord -= GetUnsignedByte( p );
						}
						else if ( !(flag & NEXT_X_IS_ZERO) )
						{	
							coord += (short)(*p++) << 8;
							coord += (Byte)*p++;
						}
						*x = ::FixRatio( coord, upem );
						x++;
					}
				}
				//	Lets do Y
				{	
					short coord = 0;
					Fixed* y = *pOutline->y;
	
					onCurve = *pOutline->onCurve;
					while ( onCurve < stop )
					{	
						if ( (flag = *onCurve) & YSHORT ) 
						{
							if ( flag & SHORT_Y_IS_POS )
								coord += GetUnsignedByte( p );
							else
								coord -= GetUnsignedByte( p );
						}
						else if ( !(flag & NEXT_Y_IS_ZERO) )
						{	
							coord += (short)(*p++) << 8;
							coord += (Byte)*p++;
						}
						*y = -::FixRatio( coord, upem );
						y++;
						
						//	Filter off the extra bits
						*onCurve++ = flag & ONCURVE;
					}
				}
			}
		}
		else
			throw fontNotOutlineErr;
	}
	catch ( OSErr osErr )
	{
		SetFontState( sfnt, state );
		switch ( osErr ) 
		{
			case memFullErr :																// out of memeory
				SetFontState( sfnt, state );
				throw kMemory;
				break;
			case fontNotOutlineErr :		 											// bad font
				SetFontState( sfnt, state );
				throw kResource;
				break;
			default:
				TpsAssertAlways( "Bad exception" ); 
				throw;
				break;
		}
	}
	catch ( ... )
	{
		SetFontState( sfnt, state );
		throw;
	}

	SetFontState( sfnt, state );
}

