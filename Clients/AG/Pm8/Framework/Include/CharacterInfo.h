// ****************************************************************************
//
//  File Name:			CharacterInfo.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RCharacterWidths and RKerningInfo class
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
//  $Logfile:: /PM8/Framework/Include/CharacterInfo.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CHARACTERINFO_H_
#define		_CHARACTERINFO_H_

#ifndef		_DATAOBJECT_H_
#include		"DataObject.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RPath;

// ****************************************************************************
//
//  Class Name:	RCharacterWidths
//
//  Description:	Allocate and maintain an array of character widths.
//
// ****************************************************************************
//
class	FrameworkLinkage RCharacterWidths : public RDataObject
	{
	//	Construction & Desctruction
	public :
													RCharacterWidths( const YFontInfo& fontInfo );
	virtual										~RCharacterWidths( );


	//	Operations
	public :	
		virtual void							Allocate( );
		virtual void							Deallocate( );

		YFontSize								GetCharacterWidth( const RCharacter& character ) const;
		YFontSize								GetFontHeight( ) const;
		YFontSize								GetFontAverageWidth( ) const;
		YFontSize								GetFontMaximumWidth( ) const;
		YFontSize								GetOverhang( ) const;
		YFontSize								GetAscent( ) const;
		YFontSize								GetDescent( ) const;
		YFontSize								GetLeading( ) const;
		YFontDimensions&						GetDimensions( );

	///xxx		Would it be useful to have the italic size in here also???

	//	Private members
	private :
		int										m_nFirstCharacter;
		int										m_nCharacters;
		YFontInfo								m_FontInfo;
		YFontSize*								m_pWidthTable;
		YFontSize								m_AverageWidth;
		YFontSize								m_MaximumWidth;
		YFontSize								m_DefaultHeight;
		YFontDimensions						m_Dimensions;

#ifdef	TPSDEBUG
	public :
		virtual void							Validate( ) const;
#endif	//	TPSDEBUG
	} ;

// ****************************************************************************
//
//  Class Name:	RKerningInfo
//
//  Description:	Allocate and maintain an array of sorted character kern pairs.
//
// ****************************************************************************
//
class	RKerningInfo : public RDataObject
	{
	//	Construction & Desctruction
	public :
													RKerningInfo( const YFontInfo& fontInfo );
	virtual										~RKerningInfo( );


	//	Operations
	public :	
		virtual void							Allocate( );
		virtual void							Deallocate( );

		YKernSize								GetKerningPair( const RCharacter& char1, const RCharacter& char2 ) const;

	//	Implemtation
	private :
		void										Sort( );

	//	Private members
	private :
		typedef	RArray<YKernPair>			RKernArray;
		typedef	RKernArray::YIterator	RKernArrayIterator;

		int										m_nEntries;
		BOOLEAN									m_fTableValid;
		YFontInfo								m_FontInfo;
		RKernArray								m_KernTable;
		RKernArray								m_TableIndex;

#ifdef	TPSDEBUG
	public :
		virtual void							Validate( ) const;
#endif	//	TPSDEBUG
	} ;


// ****************************************************************************
//
//  Class Name:	RCharacterPaths
//
//  Description:	Allocate and maintain an array of character paths.
//
// ****************************************************************************
//
class	RCharacterPaths : public RDataObject
	{
	//	Construction & Desctruction
	public :
													RCharacterPaths( const YFontInfo& fontInfo );
	virtual										~RCharacterPaths( );


	//	Operations
	public :	
		virtual void							Allocate( );
		virtual void							Deallocate( );

		RPath*									GetCharacterPath( const RCharacter& character ) const;
		void										SetCharacterPath( const RCharacter& character, RPath* pPath );

	//	Private members
	private :
		int										m_nFirstCharacter;
		int										m_nCharacters;
		YFontInfo								m_FontInfo;
		RPath**									m_ppPaths;		//	array of RPath pointers

#ifdef	TPSDEBUG
	public :
		virtual void							Validate( ) const;
#endif	//	TPSDEBUG
	} ;

// ****************************************************************************
//							Inlines 
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetCharacterWidth( )
//
//  Description:		Return the width of the requested character
//
//  Returns:			m_pWidthsTable[index] or DefaultWidth
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFontSize RCharacterWidths::GetCharacterWidth( const RCharacter& character ) const
	{
	int	index = character - m_nFirstCharacter;

	if ((index < 0) || (index > m_nCharacters))
		return 0;
	else if (m_pWidthTable)
		return m_pWidthTable[index];
	return m_MaximumWidth;
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetFontHeight( )
//
//  Description:		Return the height of the font
//
//  Returns:			m_DefaultHeight
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFontSize RCharacterWidths::GetFontHeight( ) const
	{
	return m_DefaultHeight;
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetFontAverageWidth( )
//
//  Description:		Return the Width of the font
//
//  Returns:			m_AverageWidth
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFontSize RCharacterWidths::GetFontAverageWidth( ) const
	{
	return m_AverageWidth;
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetFontMaximumWidth( )
//
//  Description:		Return the Width of the font
//
//  Returns:			m_MaximumWidth
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFontSize RCharacterWidths::GetFontMaximumWidth( ) const
	{
	return m_MaximumWidth;
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetOverhang( )
//
//  Description:		Return the font overhang (italic amount)
//
//  Returns:			m_Overhang
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFontDimensions& RCharacterWidths::GetDimensions( )
	{
	return m_Dimensions;
	}


// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetOverhang( )
//
//  Description:		Return the font overhang (italic amount)
//
//  Returns:			m_Overhang
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFontSize RCharacterWidths::GetOverhang( ) const
	{
	return m_Dimensions.m_Overhang;
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetAscent( )
//
//  Description:		Return the font Ascend
//
//  Returns:			m_Ascent
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFontSize RCharacterWidths::GetAscent( ) const
	{
	return m_Dimensions.m_Ascent;
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetDescent( )
//
//  Description:		Return the font descent
//
//  Returns:			m_Descent
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFontSize RCharacterWidths::GetDescent( ) const
	{
	return m_Dimensions.m_Descent;
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetLeading( )
//
//  Description:		Return the font leading
//
//  Returns:			m_Leading
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFontSize RCharacterWidths::GetLeading( ) const
	{
	return m_Dimensions.m_Leading;
	}


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_CHARACTERINFO_H_


