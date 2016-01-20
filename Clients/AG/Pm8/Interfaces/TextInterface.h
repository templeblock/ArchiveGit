// ****************************************************************************
//
//  File Name:			TextInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RTextInterface class
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
//  $Logfile:: /PM8/Interfaces/TextInterface.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_TEXTINTERFACE_H_
#define		_TEXTINTERFACE_H_

#ifndef		_COMPONENTDATAINTERFACE_H_
#include		"ComponentDataInterface.h"
#endif

#ifndef		_SETTINGSINTERFACE_H_
#include		"SettingsInterface.h"
#endif

class RURL;
class RURLList;
class RHTMLBulletInfo;
class RUndoableAction;

const	YInterfaceId	kTextInterfaceId			= 50;
const YInterfaceId	kCharacterSettingsInterfaceId	= 51;
const YInterfaceId	kParagraphSettingsInterfaceId	= 52;
const YInterfaceId	kTextViewSettingsInterfaceId	= 53;
const YInterfaceId	kTextDefaultsInterfaceId		= 54;
const YInterfaceId	kReturnAddressModifiedId		= 55;
const YInterfaceId	kLineSpacingInterfaceId			= 56;
const YInterfaceId	kTextScrollInterfaceId			= 57;
const YInterfaceId	kTextClipboardInterfaceId	  	= 58;
const YInterfaceId	kTextHTMLLinkInterfaceId	  	= 59;
const YInterfaceId	kThesaurusInterfaceId			= 60;

typedef bool (*FontAlternatesFunction)( const RMBCString *pFontName, RArray<RMBCString> *pAlternates );
// ****************************************************************************
//
//  Class Name:	RTextHolder
//
//  Description:	A Pure virtual class for Holding the data of a data interface
//
// ****************************************************************************
//
class RTextHolder : public RComponentDataHolder
	{
	//	Construction and Destruction
	public :
													RTextHolder( ) { ; };
		virtual									~RTextHolder( ) { ; };
	} ;

// ****************************************************************************
//
//  Class Name:	RTextInterface
//
//  Description:	A pure virtual class interfacing with a text component
//
// ****************************************************************************
//
class RTextInterface : public RComponentDataInterface
	{
	//	Construction & Destruction
	public :
													RTextInterface( const RComponentView* pView ) : RComponentDataInterface( pView ) { ; }

	//	Operations
	public :
		virtual RComponentDataHolder*		GetData( ) = 0;
		virtual void							EditData( ) = 0;
		virtual void							LoadRTF( RStorage& storage, BOOLEAN fClear = FALSE ) = 0;
		virtual void							GetRTF( RStorage& storage, BOOLEAN fAll = TRUE ) = 0;
		virtual void							SetData( RComponentDataHolder* ) = 0;
		virtual void							InsertString( const RMBCString& string ) = 0;
		virtual void							ClearData( ) = 0;
		virtual void							Read( RChunkStorage& ) = 0;
		virtual void							Write( RChunkStorage& ) = 0;
		virtual void							Render( RDrawingSurface& , const R2dTransform&, const RIntRect&, const RIntRect& ) = 0;
		virtual void							GetHTML( RMBCString *pstrStyleSheet, RMBCString *pstrHTMLText, RArray<RHTMLBulletInfo> *pBulletInfoList, 
														 const char *strDocumentID = NULL, const char *strComponentID = NULL, FontAlternatesFunction = NULL) = 0; 
												// returns true if the text selection contains text (not a caret)
		virtual bool							IsTextSelected() = 0;
												// deletes the last action the text component has been holding onto. 
												// Needed if the action is deleted by the application.
		virtual void							ClearLastUndoAction( RUndoableAction *pAction ) = 0;

	//	Private Members
	private :

	} ;

// ****************************************************************************
//
//  Class Name:	RCharacterSettingsInterface
//
//  Description:	Interface for setting the tint of an object.
//
// ****************************************************************************
class RCharacterSettingsInterface : public RSettingsInterface
{
	//	Construction
	public :
													RCharacterSettingsInterface( const RComponentView* pView ) : RSettingsInterface( pView ) { ; }

	//	Get & Set Attributes
	public :
		virtual BOOLEAN						GetFontName( RMBCString& ) const = 0;
		virtual BOOLEAN						GetFontSize( YFontSize& ) const = 0;
		virtual BOOLEAN						GetFontColor( RSolidColor& ) const = 0;
		virtual int								GetBoldState( ) const = 0;
		virtual int								GetItalicState( ) const = 0;
		virtual int								GetUnderlineState( ) const = 0;

		virtual void							SetFontName( RMBCString&, BOOLEAN ) = 0;
		virtual void							SetFontSize( YFontSize, BOOLEAN ) = 0;
		virtual void							SetFontColor( RSolidColor&, BOOLEAN ) = 0;
		virtual void							SetBoldState( BOOLEAN, BOOLEAN ) = 0;
		virtual void							SetItalicState( BOOLEAN, BOOLEAN ) = 0;
		virtual void							SetUnderlineState( BOOLEAN, BOOLEAN ) = 0;
		virtual void 							InsertSymbol( RMBCString& fontName, RCharacter& character ) = 0;
											// Changes the color of text in the component from oldColor to newColor
		virtual void						ChangeTextColor( const RSolidColor &oldcolor, const RSolidColor &newcolor ) = 0;

};

// ****************************************************************************
//
//  Class Name:	RParagraphSettingsInterface
//
//  Description:	Interface for setting the tint of an object.
//
// ****************************************************************************
class RParagraphSettingsInterface : public RSettingsInterface
	{
	//	Construction
	public :
													RParagraphSettingsInterface( const RComponentView* pView ) : RSettingsInterface( pView ) { ; }

	//	Get & Set Attributes
	public :
		virtual int								GetLeftJustifiedState( ) const = 0;
		virtual int								GetCenterJustifiedState( ) const = 0;
		virtual int								GetRightJustifiedState( ) const = 0;
		virtual int								GetFullJustifiedState( ) const = 0;
		virtual int								GetBulletState( ) const = 0;

		virtual void							SetLeftJustify( BOOLEAN fApply ) = 0;
		virtual void							SetCenterJustify( BOOLEAN fApply ) = 0;
		virtual void							SetRightJustify( BOOLEAN fApply ) = 0;
		virtual void							SetFullJustify( BOOLEAN fApply ) = 0;
	};

// ****************************************************************************
//
//  Class Name:	RTextViewSettingsInterface
//
//  Description:	Interface for setting the tint of an object.
//
// ****************************************************************************
class RTextViewSettingsInterface : public RSettingsInterface
	{
	//	Construction
	public :
													RTextViewSettingsInterface( const RComponentView* pView ) : RSettingsInterface( pView ) { ; }

	//	Get & Set Attributes
	public :
		virtual int								GetTopJustifiedState( ) const = 0;
		virtual int								GetMiddleJustifiedState( ) const = 0;
		virtual int								GetBottomJustifiedState( ) const = 0;
		virtual int								GetFullJustifiedState( ) const = 0;

		virtual void							SetTopJustify( ) = 0;
		virtual void							SetMiddleJustify( ) = 0;
		virtual void							SetBottomJustify( ) = 0;
		virtual void							SetFullJustify( ) = 0;
		virtual void							SelectAll( ) = 0;
	};

// ****************************************************************************
//
//  Class Name:	RTextViewSettingsInterface
//
//  Description:	Interface for setting the text defaults.
//
// ****************************************************************************
class RTextDefaultsInterface : public RInterface
	{
	//	Construction
	public :
													RTextDefaultsInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Persistant defaults Attributes
	public :
		virtual void							SetFontName( const RMBCString& fontName ) = 0;
		virtual void							SetFontSize( YFontSize fontSize ) = 0;

	//	Non-persistant defaults
	public :
		virtual void							SetLeftRightIndents( YIntDimension left, YIntDimension right ) = 0;
		virtual void							SetRenderEmptyContainer( BOOLEAN fRender ) = 0;
		virtual void							SetPreserveSelectionOnDeactivate( BOOLEAN fPreserve ) = 0;
	};

// ****************************************************************************
//
//  Class Name:	RTextViewSettingsInterface
//
//  Description:	Interface for setting the text defaults.
//
// ****************************************************************************
class RReturnAddressModifiedInterface : public RInterface
	{
	//	Construction
	public :
													RReturnAddressModifiedInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Implementation
	public :
		virtual void							SetNewAddress( ) = 0;
	};

// ****************************************************************************
//
//  Class Name:	RLineSpacingSettingsInterface
//
//  Description:	Interface for setting the Line spacing of a text component.
//
// ****************************************************************************
class RLineSpacingSettingsInterface : public RSettingsInterface
	{
	//	Construction
	public :
													RLineSpacingSettingsInterface( const RComponentView* pView ) : RSettingsInterface( pView ) { ; }

	//	Get & Set Attributes
	public :
		virtual BOOLEAN						GetLineSpacing( YRealDimension& spacing ) = 0;
		virtual void							SetLineSpacing( YRealDimension  spacing ) = 0;
	};


// ****************************************************************************
//
//  Class Name:	RTextScrollInterface
//
//  Description: Interface for turning on and off scrolling 
//				 and retrieving information about scrolling.
//
// ****************************************************************************
class RTextScrollInterface : public RInterface
	{
	//	Construction
	public :
								RTextScrollInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Get & Set Attributes
	public :
											// If positive returns the amount the text overflows. 					
											// If < 0 the text is not overflowing
		virtual YRealDimension				GetTextOverflowAmount( ) = 0;
											// Turn scrolling on, does not invalidate the container
		virtual void						EnableScrolling() = 0;
											// Turn scrolling off, does not invalidate the container
		virtual void						DisableScrolling() = 0;
	};

// ****************************************************************************
//
//  Class Name:	RTextClipBoardInterface
//
//  Description: Interface for cut/copy/paste to/from the clipboard.
//
// ****************************************************************************
class RTextClipBoardInterface : public RInterface
{
	//	Construction
	public :
								RTextClipBoardInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Get & Set Attributes
	public :
											// copy the text from the clipboard to the current position
		virtual BOOLEAN						CopyText() = 0;
											// cut the text from the clipboard to the current position
		virtual BOOLEAN						CutText() = 0;
											// Paste the text from the clipboard to the current position
		virtual BOOLEAN						PasteText() = 0;
											// Paste the text from a drag/drop source  to the current position
		virtual void						PasteText( RDataTransferSource& dataSource, const RRealPoint& point, YDropEffect dropEffect ) = 0;
};

// ****************************************************************************
//
//  Class Name:	RThesaurusInterface
//
//  Description: Interface to the thesaurus.
//
// ****************************************************************************
class RThesaurusInterface : public RInterface
{
	//	Construction
	public :
								RThesaurusInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Get & Set Attributes
	public :
											// Bring up Thesaurus dialog
		virtual void						Thesaurus() = 0;
};




// ****************************************************************************
//
//  Class Name:	RTextHTMLLinkInterface
//
//  Description: Interface for setting and retrieving HTML links in text 
//
// ****************************************************************************
class RTextHTMLLinkInterface : public RInterface
	{
	//	Construction
	public :
								RTextHTMLLinkInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Get & Set Attributes
	public :
								// returns TRUE if the selection is contained within any part of a link(s)
		virtual bool			IsLinkSelected() = 0;
								// Finds the first Link the selection contains and sets the selection to it
		virtual const RURL*		SelectFirstLink() = 0;
								// Inserts a URL into the text at the selection
		virtual void			InsertLink(const RURL *pURL, const RSolidColor &color) = 0;
								// Removes the link(s) within the selection
		virtual void			RemoveLink() = 0;
								// Gets a list of URL present in the text at the selection
		virtual void			GetLink( RURLList *pList ) = 0;
								// Gets a list of all URLs present in the text document
		virtual void			GetAllLinks( RURLList *pList ) = 0;
								// Changes the color of all text links in the component
		virtual void			ChangeAllLinksColor( const RSolidColor &color ) = 0;
	};
	
#endif	//	_TEXTINTERFACE_H_
