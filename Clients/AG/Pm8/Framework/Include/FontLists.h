// ****************************************************************************
//
//  File Name:			FontLists.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RFontList and RFontSizeInfo class
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
//  $Logfile:: /PM8/Framework/Include/FontLists.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FONTLISTS_H_
#define		_FONTLISTS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	forward references
class		RArchive;
class		RChunkStorage;
class		RFont;	
struct	YFont;

struct	YFontDescriptor
	{
	RFont*		pRFont;
	RSolidColor	color;
	YCounter		uses;
	} ;

class	RFontSubstitution
	{
	RMBCString	m_fontRequested;
	RMBCString	m_fontApplied;
	public :
		RFontSubstitution( const RMBCString& fontRequested, const RMBCString& fontApplied ) : m_fontRequested( fontRequested ), m_fontApplied( fontApplied ) { ; }
		RFontSubstitution( const RFontSubstitution& rhs ) : m_fontRequested( rhs.m_fontRequested ), m_fontApplied( rhs.m_fontApplied ) { ; }

		BOOLEAN					operator==(const RFontSubstitution& ) { return FALSE; }
		RFontSubstitution&	operator=(const RFontSubstitution& rhs ) { m_fontRequested = rhs.m_fontRequested, m_fontApplied = rhs.m_fontApplied; return *this; }
		const RMBCString&		GetFontRequested( )	const { return m_fontRequested; }
		const RMBCString&		GetFontApplied( )		const { return m_fontRequested; }
	} ;

typedef RArray< RFontSubstitution >					RFontSubstitutionCollection;
typedef RFontSubstitutionCollection::YIterator	RFontSubstitutionIteartor;

// ****************************************************************************
//
//  Class Name:	RFontList
//
//  Description:	The Renaissance Font list class. All references to a font
//						will be indicies into this list.
//
// ****************************************************************************
//

class FrameworkLinkage RFontList : private RArray<YFontDescriptor>
	{
	//	Construction, Destruction & Initialization
	public :
													RFontList( );
													RFontList( const RFontList& rhs );
		virtual									~RFontList( );

	//	Operations
	public :
		RFont*									GetRFont( YFontId id ) const;
		void										GetFont( YFontId id, YFont& font) const;
		const YFontInfo&						GetFontInfo( YFontId id ) const;
		YFontInfo&								GetFontInfo( YFontId id );
		const YFontDescriptor&				GetFontDescriptor( YFontId id ) const;

		RSolidColor								GetFontColor( YFontId id ) const;

		YFontId									AddFont( const YFont& font );
		YFontId									FindFont( const YFont& font ) const;
		void										RemoveFont( YFontId id );
		//	This is only really necessary to retrieve a value to give to Stonehand
		//	for their SpecManager I/O code
		YFontId									FindRFont( RFont* pfont ) const;
		YFontId									FindFontFamily( sBYTE* psbFamilyName ) const;

		void										ClearFontList( );
		void										WriteFontList( RChunkStorage& storage ) const;
		void										ReadFontList( RChunkStorage& storage );
		void										Write( RArchive& archive ) const;
		void										Read( RArchive& archive );

		RFontList&								operator=( const RFontList& rhs );

	//	Methods for tracking fonts that cannot be created correctly.
	public :
		static void									ClearFontSubstitutionList( );
		static RFontSubstitutionCollection&	GetFontSubstitutionList( );

		//	Private globals and other such...
	private :
		void										RemoveAppUse( RFont* pFont );
		YFontDescriptor&						GetFontDescriptor( YFontId id );
		void									ConvertFontSize( YFont *pfont );

		//	Static data members
		static RFontSubstitutionCollection	m_gSubstitutionCollection;
	} ;

// ****************************************************************************
//
//  Class Name:	RFontSizeInfo
//
//  Description:	font size info for all projects.
//
// ****************************************************************************
//
typedef RArray<YIntDimension>			YFontSizeArray;
typedef const YFontSizeArray			YCFontSizeArray;
typedef YFontSizeArray::YIterator	YFontSizeIterator;

class FrameworkLinkage RFontSizeInfo 
	{
	//	Construction
	public:
													RFontSizeInfo( );
													RFontSizeInfo( const RFontSizeInfo& rhs );
													RFontSizeInfo( YIntDimension yMin,		YIntDimension ySize1,
																		YIntDimension ySize2,	YIntDimension ySize3,
																		YIntDimension ySize4,	YIntDimension ySize5,
																		YIntDimension ySize6,	YIntDimension ySize7,
																		YIntDimension ySize8,	YIntDimension ySize9,
																		YIntDimension ySize10,	YIntDimension ySize11,
																		YIntDimension ySize12,	YIntDimension ySize13,
																		YIntDimension ySize14,
																		YIntDimension yMax,		YIntDimension yDefault );

	//	Operators
	public:
		RFontSizeInfo&							operator=( const RFontSizeInfo& rhs );

	//	Operations
	public:
		YIntDimension							GetMinFontSize( ) const;
		YIntDimension							GetMaxFontSize( ) const;
		YIntDimension							GetDefaultFontSize( ) const;
		YCFontSizeArray&						GetFontSizes( ) const;
		void										Read( RArchive& archive );
		void										Write( RArchive& archive ) const;

	//	member data
	private:
		YIntDimension							m_MinSize;
		YIntDimension							m_MaxSize;
		YIntDimension							m_DefaultSize;
		YFontSizeArray							m_FontSizes;
	};

// ****************************************************************************
//			Inline functions
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_FONTLISTS_H_
