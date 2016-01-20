#include "sctypes.h"
#include "scstyle.h"

void UFont::update( const scStyle& spec )
{
	lfHeight		= -spec.GetGlyphHeight();
	lfWeight		= (int)spec.GetEmphasis() & (int)eBold ? 700 : 400;
	lfItalic		= (int)spec.GetEmphasis() & (int)eItalic;
	lfUnderline 	= (int)spec.GetEmphasis() & (int)eUnderline;
	lfStrikeOut 		= (int)spec.GetEmphasis() & (int)eStrikeThru;
}

/* ==================================================================== */

UFont::UFont( const scStyle& spec )
{
	lfHeight			= scPOINTS( -spec.GetGlyphHeight() );
	lfWidth 			= 0;
	lfEscapement		= 0;
	lfOrientation		= 0;
	lfWeight			= (int)spec.GetEmphasis() & (int)eBold ? 700 : 400;
	lfItalic			= (int)spec.GetEmphasis() & (int)eItalic;
	lfUnderline 		= (int)spec.GetEmphasis() & (int)eUnderline;
	lfStrikeOut 		= (int)spec.GetEmphasis() & (int)eStrikeThru;
	lfCharSet			= ANSI_CHARSET;

	lfOutPrecision		= OUT_CHARACTER_PRECIS;
			
	lfClipPrecision 	= CLIP_DEFAULT_PRECIS;
	lfQuality			= DEFAULT_QUALITY;

	lfPitchAndFamily	= FF_DONTCARE | DEFAULT_PITCH;

//	::scStrcpy( lfFaceName, spec.GetFont() );

}

/* ==================================================================== */

UFont::UFont( const LOGFONT* font )
{
	if ( font )
		setLogFont( *font );
	else {
#ifdef _WINDOWS 	
		lfHeight			= scPOINTS( 14 );
		lfWidth 			= 0;
		lfEscapement		= 0;
		lfOrientation		= 0;
		lfWeight			= 0;
		lfItalic			= 0;
		lfUnderline 		= 0;
		lfStrikeOut 		= 0;
		lfCharSet			= ANSI_CHARSET;
		
		lfOutPrecision		= OUT_CHARACTER_PRECIS;
					
		lfClipPrecision 	= CLIP_DEFAULT_PRECIS;
		lfQuality			= DEFAULT_QUALITY;

		lfPitchAndFamily	= FF_DONTCARE | DEFAULT_PITCH;

		::scStrcpy( lfFaceName, "Times New Roman" );
#endif

#ifdef _MAC
		lfHeight			= scPOINTS( 14 );
		lfWidth 			= 0;
		lfWeight			= 0;
		lfItalic			= 0;
		lfUnderline 		= 0;
		lfStrikeOut 		= 0;
		lfCharSet			= 0;
		lfPitchAndFamily	= 0;

		::scStrcpy( lfFaceName, "Times" );
#endif
	}
}
 
/* ==================================================================== */
//
// !HACK ALERT! - Quick hack to handle symbol encodings on Windows.  (GAA)
//
#ifdef _WINDOWS
static int IsSymbolFont ( const scChar * name )
{
	if ( ( scStrcmp ( name, "Symbol" ) == 0 ) ||
		 ( scStrcmp ( name, "Wingdings" ) == 0 ) ||
		 ( scStrcmp ( name, "Monotype Sorts" ) == 0 ) )
		return 1;
	else
		return 0;
}
#endif

/* ==================================================================== */

void UFont::setFontname( const scChar* name )
{
	::scStrcpy( lfFaceName, name );
#ifdef _WINDOWS
	if ( ::IsSymbolFont ( lfFaceName ) )
		lfCharSet = SYMBOL_CHARSET;
#endif
}

/* ==================================================================== */

void UFont::setFontname( const scChar* name, unsigned long len )
{
	if ( len >= LF_FACESIZE )
		return;
	for ( scChar* p = lfFaceName; len; len-- )
		*p++ = *name++;
	*p++ = 0;
#ifdef _WINDOWS
	if ( IsSymbolFont ( lfFaceName ) )
		lfCharSet = SYMBOL_CHARSET;
#endif
}

/* ==================================================================== */

void UFont::setLogFont( const LOGFONT& lfont )
{
#ifdef _WINDOWS 
	lfHeight		= lfont.lfHeight;
	lfWidth 		= lfont.lfWidth;
	lfEscapement	= lfont.lfEscapement;
	lfOrientation	= lfont.lfOrientation;
	lfWeight		= lfont.lfWeight;
	lfItalic		= (lfont.lfItalic != 0);
	lfUnderline 	= (lfont.lfUnderline != 0);
	lfStrikeOut 	= (lfont.lfStrikeOut != 0);
	lfCharSet		= lfont.lfCharSet;
	lfOutPrecision	= lfont.lfOutPrecision;
	lfClipPrecision = lfont.lfClipPrecision;
	lfQuality		= lfont.lfQuality;
	lfPitchAndFamily	= lfont.lfPitchAndFamily;
	::memcpy( lfFaceName, lfont.lfFaceName, LF_FACESIZE );
#endif

#ifdef _MAC
	lfHeight		= lfont.lfHeight;
	lfWidth 		= lfont.lfWidth;
	lfWeight		= lfont.lfWeight;
	lfItalic		= lfont.lfItalic;
	lfUnderline 	= lfont.lfUnderline;
	lfStrikeOut 	= lfont.lfStrikeOut;
	lfCharSet		= lfont.lfCharSet;
	lfPitchAndFamily	= lfont.lfPitchAndFamily;
	::memcpy( lfFaceName, lfont.lfFaceName, LF_FACESIZE );
#endif
}

/* ==================================================================== */

#ifdef _MAC
void UFont::maccodes( short& fnt, short& face )
{
	Str255 fontname;
	char* p = (char*)&fontname[1];
	
	::strcpy( p, lfFaceName );
	fontname[0] = ::strlen( lfFaceName );
 
	face = 0;
	if ( lfItalic )
		face |= italic;
	if ( lfWeight > 400 )
		face |= bold;
	if ( lfUnderline )
		face |= underline;;

	::GetFNum( fontname, &fnt );
}
#endif

/* ==================================================================== */

int UFont::operator==( const LOGFONT& lfont ) const
{
	return lfHeight 			== lfont.lfHeight			&&
		   lfWidth				== lfont.lfWidth			&&
		   lfEscapement 		== lfont.lfEscapement		&&
		   lfOrientation		== lfont.lfOrientation		&&
		   lfWeight 			== lfont.lfWeight			&&
		   (lfItalic != 0)		== (lfont.lfItalic != 0)	&&
		   (lfUnderline != 0)	== (lfont.lfUnderline != 0) &&
		   (lfStrikeOut != 0)	== (lfont.lfStrikeOut != 0) &&
		   lfCharSet			== lfont.lfCharSet			&&
		   lfOutPrecision		== lfont.lfOutPrecision 	&&
		   lfClipPrecision		== lfont.lfClipPrecision	&&
		   lfQuality			== lfont.lfQuality			&&
		   lfPitchAndFamily 	== lfont.lfPitchAndFamily	&&
		   !::strcmp( lfFaceName, lfont.lfFaceName );
}

/* ==================================================================== */

int UFont::operator==( const UFont& lfont ) const
{
	return lfHeight 			== lfont.lfHeight			&&
		   lfWidth				== lfont.lfWidth			&&
		   lfEscapement 		== lfont.lfEscapement		&&
		   lfOrientation		== lfont.lfOrientation		&&
		   lfWeight 			== lfont.lfWeight			&&
		   (lfItalic != 0)		== (lfont.lfItalic != 0)	&&
		   (lfUnderline != 0)	== (lfont.lfUnderline != 0) &&
		   (lfStrikeOut != 0)	== (lfont.lfStrikeOut != 0) &&
		   lfCharSet			== lfont.lfCharSet			&&
		   lfOutPrecision		== lfont.lfOutPrecision 	&&
		   lfClipPrecision		== lfont.lfClipPrecision	&&
		   lfQuality			== lfont.lfQuality			&&
		   lfPitchAndFamily 	== lfont.lfPitchAndFamily	&&
		   !::strcmp( lfFaceName, lfont.lfFaceName );
}

/* ==================================================================== */

int UFont::operator!=( const LOGFONT& lfont ) const
{
	return !operator==( lfont );
}

/* ==================================================================== */

int UFont::operator!=( const UFont& lfont ) const
{
	return !operator==( lfont );	
}

/* ==================================================================== */

UFont&	UFont::operator=( const LOGFONT& lfont )
{
	lfHeight		= lfont.lfHeight;
	lfWidth 		= lfont.lfWidth;
	lfEscapement	= lfont.lfEscapement;
	lfOrientation	= lfont.lfOrientation;
	lfWeight		= lfont.lfWeight;
	lfItalic		= (lfont.lfItalic != 0);
	lfUnderline 	= (lfont.lfUnderline != 0);
	lfStrikeOut 	= (lfont.lfStrikeOut != 0);
	lfCharSet		= lfont.lfCharSet;
	lfOutPrecision	= lfont.lfOutPrecision;
	lfClipPrecision = lfont.lfClipPrecision;
	lfQuality		= lfont.lfQuality;
	lfPitchAndFamily	= lfont.lfPitchAndFamily;
	::memcpy( lfFaceName, lfont.lfFaceName, LF_FACESIZE );

	return *this;
}


/* ==================================================================== */

UFont& UFont::operator=( const UFont& lfont )
{
	lfHeight		= lfont.lfHeight;
	lfWidth 		= lfont.lfWidth;
	lfEscapement	= lfont.lfEscapement;
	lfOrientation	= lfont.lfOrientation;
	lfWeight		= lfont.lfWeight;
	lfItalic		= (lfont.lfItalic != 0);
	lfUnderline 	= (lfont.lfUnderline != 0);
	lfStrikeOut 	= (lfont.lfStrikeOut != 0);
	lfCharSet		= lfont.lfCharSet;
	lfOutPrecision	= lfont.lfOutPrecision;
	lfClipPrecision = lfont.lfClipPrecision;
	lfQuality		= lfont.lfQuality;
	lfPitchAndFamily	= lfont.lfPitchAndFamily;
	::memcpy( lfFaceName, lfont.lfFaceName, LF_FACESIZE );

	return *this;
}

/* ==================================================================== */

UFont::UFont( APPFont		font,
			  int32 		ptsize,
			  eEmphasisType style,
			  int32 		charset )
{
	lfHeight			= -ptsize;
	lfWidth 			= 0;
	lfEscapement		= 0;
	lfOrientation		= 0;

	lfWeight			= (int)style & (int)eBold ? 700 : 400;
	lfItalic			= (int)style & (int)eItalic ? 1 : 0;
	lfUnderline 		= (int)style & (int)eUnderline ? 1 : 0;
	lfStrikeOut 		= (int)style & (int)eStrikeThru ? 1 : 0;

	lfCharSet			= (BYTE)charset;

	lfOutPrecision		= OUT_CHARACTER_PRECIS;
					
	lfClipPrecision 	= CLIP_DEFAULT_PRECIS;
	lfQuality			= DEFAULT_QUALITY;
	
	lfPitchAndFamily	= FF_DONTCARE | DEFAULT_PITCH;

	::strncpy( lfFaceName, (const char*)font, sizeof( lfFaceName ) );
	lfFaceName[ sizeof(lfFaceName) - 1 ] = 0;

#ifdef _WINDOWS
	if ( IsSymbolFont ( lfFaceName ) )
		lfCharSet = SYMBOL_CHARSET;
#endif
}

/* ==================================================================== */
