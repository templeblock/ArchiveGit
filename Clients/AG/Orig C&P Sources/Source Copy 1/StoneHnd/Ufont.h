/****************************************************************************

        $Header: /btoolbox/lib/UFONT.H 2     5/06/96 3:49p Will $

        Contains:

        Written by:     Manis

        Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
        All rights reserved.

        This notice is intended as a precaution against inadvertent publication
        and does not constitute an admission or acknowledgment that publication
        has occurred or constitute a waiver of confidentiality.

        Composition Toolbox software is the proprietary
        and confidential property of Stonehand Inc.

********************************************************************/

#ifndef _H_UFONT
#define _H_UFONT

#ifdef _WINDOWS
#include <windows.h>
#endif

#ifdef _MAC
	#define LF_FACESIZE		32
	struct LOGFONT {
		int	    lfHeight;
		int		lfWidth;
		long	lfWeight;
		uint8	lfItalic;
		uint8	lfUnderline;
		uint8	lfStrikeOut;
		uint8	lfCharSet;
		uint8	lfPitchAndFamily;
	    char	lfFaceName[LF_FACESIZE];
		int		sysnum;		
	};

	struct KERNINGPAIR {
	   uint16 wFirst;
	   uint16 wSecond;
	   int	  iKernAmount;
	};

	struct TEXTMETRIC {
		int32	tmHeight;
		int32	tmAscent;
		int32	tmDescent;
		int32	tmInternalLeading;
		int32	tmExternalLeading;
		int32	tmAveCharWidth;
		int32	tmMaxCharWidth;
		int32	tmWeight;
		int32	tmOverhang;
		int32	tmDigitizedAspectX;
		int32	tmDigitizedAspectY;
		uint8	tmFirstChar;
		uint8	tmLastChar;
		uint8	tmDefaultChar;
		uint8	tmBreakChar;
		uint8	tmItalic;
		uint8	tmUnderlined;
		uint8	tmStruckOut;
		uint8	tmPitchAndFamily;
		uint8	tmCharSet;
	};

#endif

class scStyle;

#ifdef UNICODE
class UFont : public tagLOGFONTW {
#else
class UFont : public tagLOGFONTA {
#endif
public:
			UFont( const scStyle& );
			UFont( const LOGFONT* font = 0 );
			UFont( APPFont, int32, eEmphasisType, int32 );			
			~UFont(){}

    void	setfont( APPFont );			

	void	setLogFont( const LOGFONT& lfont );

	void	setFontname( const scChar* name );
	void	setFontname( const scChar* name, unsigned long );

	void 	update( const scStyle& );
	void	setSize( int32 size );
	void	setEmphasis( eEmphasisType );
	void	setCharSet( int32 );
	void	set( const scChar*, int32, eEmphasisType, int32 );
	
#ifdef _MAC
	void	maccodes( short&, short& );
#endif
	
#if SCDEBUG > 1
	void	DebugPrint( void ) const;
#else
	void	DebugPrint( void ) const {}
#endif

	int		operator==( const LOGFONT& ) const;
	int		operator==( const UFont& ) const;
	int		operator!=( const LOGFONT& ) const;
	int		operator!=( const UFont& ) const;

	UFont&	operator=( const LOGFONT& );
	UFont&	operator=( const UFont& );
	
};

/* ==================================================================== */

#endif
