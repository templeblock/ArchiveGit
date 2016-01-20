// ****************************************************************************
//
//  File Name:			Font.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RFont class
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
//  $Logfile:: /PM8/Framework/Include/Font.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FONT_H_
#define		_FONT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
//	Forward references
class RCharacterWidths;
class RCharacterPaths;
class RKerningInfo;
class RPath;
class RArchive;

// enums
enum	EFontType	{ 	kRaster, kTrueType, kAdobeType };

//
//		Defines and macros and typedefs
typedef	uLONG		YFontAttributes;
typedef	uLONG		YFontSize;
typedef	sLONG		YKernSize;
typedef	uLONG		YFontId;

const		uWORD		kMaxFontNameSize		= 256;
const		uWORD		kNormFontCharacters	= 256;
const		uWORD		kFontListGrowSize		= 16;
const		YFontId	kInvalidFontId			= ((YFontId)-1);

#ifdef	_WINDOWS
	typedef	KERNINGPAIR	YKernPair;
	inline	WORD GetKernPairFirst( YKernPair kp )						{ return kp.wFirst; }
	inline	WORD GetKernPairSecond( YKernPair kp )						{ return kp.wSecond; }
	inline	int  GetKernValue( YKernPair kp )							{ return kp.iKernAmount; }
	inline	void SetKernPairFirst( YKernPair& kp, WORD value )		{ kp.wFirst = value; }
	inline	void SetKernPairSecond( YKernPair& kp, WORD value )	{ kp.wSecond = value; }
#else		//	_WINDOWS - must be Mac
	typedef	KernPair		YKernPair;
	inline	uBYTE GetKernPairFirst( YKernPair kp )						{ return kp.kernFirst; }
	inline	uBYTE GetKernPairSecond( YKernPair kp )					{ return kp.kernSecond; }
	inline	int  GetKernValue( YKernPair kp )							{ return kp.kernWidth; }
	inline	void SetKernPairFirst( YKernPair& kp, uBYTE value )	{ kp.kernFirst = value; }
	inline	void SetKernPairSecond( YKernPair& kp, uBYTE value )	{ kp.kernSecond = value; }
#endif	//	_WINDOWS

//	segment op codes 
#define	LINE_TO										'lt'
#define	BEZIER_TO									'bt'
#define	QUADRATIC_TO								'qt'
#define	MOVE_TO										'mt'
#define	CLOSE_PATH									'cp'

struct YFontDimensions
	{
	YFontSize		m_Overhang;
	YFontSize		m_Ascent;
	YFontSize		m_Descent;
	YFontSize		m_Leading;
	} ;

struct FrameworkLinkage YFontInfo
	{
		BOOLEAN		operator==( const YFontInfo& rhs ) const;
		BOOLEAN		operator!=( const YFontInfo& rhs ) const;
		YFontInfo	operator*( const R2dTransform& transform ) const;

		YFontAttributes						attributes;
		YFontSize								height;
		YFontSize								width;
		YIntDegrees								angle;
		uLONG										ulPadding;
		sBYTE										sbName[ kMaxFontNameSize ];
	};

struct	YFont
	{
	YFontInfo	info;
	RSolidColor	color;
	} ;

struct	YFileFont
	{
	YFontInfo		info;
	uLONG				ulUses;
	RSolidColor		color;
	} ;

FrameworkLinkage RArchive& operator<<( RArchive& archive, const YFileFont& font );
FrameworkLinkage RArchive& operator>>( RArchive& archive, YFileFont& font );

// ****************************************************************************
//
//  Class Name:	RFont
//
//  Description:	The base Renaissance Font information class. All information
//						relavent to a font will be retrieved from this class.
//
//						It is derived from RDataObject to allow the character width
//						and kerning tables to be purged from memory and recreated as
//						as necessary.
//
// ****************************************************************************
//

class	FrameworkLinkage RFont : public RObject
	{
	//
	//	Structures and Enums
	public :
		typedef	enum tagEAttributes
			{
			//	basic attributes
			kBold				= 0x01,
			kItalic			= 0x02,
			kUnderline		= 0x04,

			//	special attributes that require more work
			kSmallCaps		= 0x1000,
			kSubscript		= 0x2000,
			kSuperscript	= 0x4000,

			//	Special tag for InsertSymbol because of Stonehand
				//	It can be used for other purposes if necessary
			kHiddenFont		= 0x80000000
			} EAttributes;

	//	Construction, Destruction & Initialization
	public :
													RFont( const YFontInfo& info, BOOLEAN fInitialize );
		virtual									~RFont( );

	//	Operations
	public :
		static	RFont*						CreateFont( const YFontInfo& fontInfo, BOOLEAN fInitialize );
		static	BOOLEAN						IsFontOnSystem( const char* pFontName );
#ifdef	_WINDOWS
		static	HDC							GetSystemDC( );
		static	void							ReleaseSystemDC( );
#endif	// _WINDOWS

		YKernSize								GetKernAmount( const RCharacter& char1, const RCharacter& char2 ) const;
		YFontSize								GetCharacterWidth( const RCharacter& char1 );
		YKernSize								GetCompensationAmount( );

		void										GetCharacterBounds( const RCharacter& character, RIntRect* pBounds ) const;
		void										GetMaxCharBounds( RIntRect* pBounds ) const;

		YFontSize								GetStringWidth( const RMBCString& string ) const;
		YFontSize								GetStringWidth( const RMBCString& string, BOOLEAN fKern ) const;
		void										GetStringBounds( const RMBCString& string, RIntRect* pBounds ) const;
		void										GetStringBounds( const RMBCString& string, RIntRect* pBounds, BOOLEAN fKern ) const;

		const YFontInfo&						GetInfo( ) const;
		YFontInfo&								GetInfo( );
		YFontSize								GetHeight( ) const;
		YFontDimensions&						GetDimensions( ) const;
		YFontAttributes						GetAttributes( ) const;
		void										GetName( LPSZ lpszName ) const;
		
		virtual RPath*							GetGlyphPath( const RCharacter& character ) = 0;

		BOOLEAN									IsAttribute( YFontAttributes flags ) const;

		void										AddUse( );
		BOOLEAN									RemoveUse( );

#ifdef	MAC
		static short							GetMacFontFamilyId( const LPSZ pFontName );
		static Style							GetMacStyleBits( YFontAttributes attributes );
#endif

	// Accessors
	public :
#ifdef	TPSDEBUG
		YCounter									GetUses( );
#endif	//	TPSDEBUG

	//	Implementation
	private :

	//	Members
	private :
		YFontInfo								m_Info;
		YCounter									m_uses;
		RCharacterWidths*						m_pWidthsTable;
		RKerningInfo*							m_pKernTable;

	protected :
		RCharacterPaths*						m_pPaths;

#ifdef	TPSDEBUG
	//	Debugging Support
	public :
		virtual	void							Validate( ) const = 0;
#endif
	} ;


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_FONT_H_
