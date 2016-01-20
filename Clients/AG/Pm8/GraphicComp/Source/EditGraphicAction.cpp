// ****************************************************************************
//
//  File Name:			EditGraphicAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the REditGraphicAction class
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
//  $Logfile:: /PM8/GraphicComp/Source/EditGraphicAction.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"
ASSERTNAME

#include	"EditGraphicAction.h"
#include "GraphicCompResource.h"

#include "ApplicationGlobals.h"
#include	"GraphicDocument.h"
#include "ResourceManager.h"
#include "SelectGraphicDialog.h"
#include "FileStream.h"
#include "Psd3GraphicLibrary.h"
#include "Graphic.h"
#include "GraphicApplication.h"
#include "ComponentView.h"
#include "ComponentTypes.h"
#include "PsdClipboardViewerDialog.h"

YActionId	REditGraphicAction::m_ActionId( "REditGraphicAction" );

InfoMapEntry componentInfoMap[ ] = 
	{
	InfoMapEntry( kSquareGraphicComponent,		"Square Graphics|*.psg|All Files|*.*||",	kHasColor,	!kHasColor ),
	InfoMapEntry( kRowGraphicComponent,			"Row Graphics|*.prg|All Files|*.*||",		kHasColor,	!kHasColor ),
	InfoMapEntry( kColumnGraphicComponent,		"Column Graphics|*.pcg|All Files|*.*||",	kHasColor,	!kHasColor ),
	InfoMapEntry( kBackdropGraphicComponent,	"Backdrops|*.pbk|All Files|*.*||",			kHasColor,	!kHasColor ),
	InfoMapEntry( kBorderComponent,				"Borders|*.pbr||",								kHasColor,	!kHasColor ),
	InfoMapEntry( kMiniBorderComponent,			"Miniborders|*.pbr||",							kHasColor,	!kHasColor ),
	InfoMapEntry( kHorizontalLineComponent,	"Ruled Lines|*.prl||",							kHasColor,	!kHasColor ),
	InfoMapEntry( kVerticalLineComponent,		"Ruled Lines|*.prl||",							kHasColor,	!kHasColor ),
	InfoMapEntry( kTimepieceComponent,			"Timepieces|*.pct||",							kHasColor,	kHasColor  ) 
	};


// ****************************************************************************
//
//  Function Name:	InfoMapEntry::InfoMapEntry( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
InfoMapEntry::InfoMapEntry( const YComponentType& componentType, const RMBCString& filterString, const BOOLEAN fMonoColorable, const BOOLEAN fColorColorable ) 
		: m_ComponentType( componentType )
		, m_FilterString( filterString )
		, m_fMonoColorable( fMonoColorable )
		, m_fColorColorable( fColorColorable )
		{ 
		NULL;
		}

// ****************************************************************************
//
//  Function Name:	REditGraphicAction::REditGraphicAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditGraphicAction::REditGraphicAction( RGraphicDocument* pGraphicDocument )
: m_pGraphic( NULL ),
	m_pGraphicDocument( pGraphicDocument ),
	RUndoableAction( m_ActionId, STRING_UNDO_EDIT_GRAPHIC, STRING_REDO_EDIT_GRAPHIC )
	{
	// Get the file open filter string
	RMBCString filter;
	InfoMapEntry Info( componentInfoMap[ 0 ] );

	for( int i = 0; i < sizeof( componentInfoMap ) / sizeof( componentInfoMap[ 0 ] ); ++i )
		{
		if( pGraphicDocument->GetComponentType( ) == componentInfoMap[ i ].m_ComponentType )
			{
			filter = componentInfoMap[ i ].m_FilterString;
			Info = componentInfoMap[ i ];
			break;
			}
		}

	// Popup the open file dialog
	CFileDialog fileDialog( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, filter, NULL );
	if( fileDialog.DoModal( ) == IDOK )
		{
		try
			{
			// Create a filestream with the result of the openfile dialog
			RFileStream fileStream;
			fileStream.Open( fileDialog.GetPathName( ), kRead );
			RMBCString rFullPath(fileDialog.GetPathName());

			// Create a graphic library 
			RPsd3GraphicLibrary graphicLibrary;

			// Try to load the library. Just throw an exception if we tried to load an
			// invalid library.
			if( !graphicLibrary.OpenLibrary( fileStream ) )
				throw( "Bad Library" );

			// Get a list of available graphics
			uWORD numGraphics;
			char** pGraphicNames = graphicLibrary.GetGraphicNames( numGraphics );
			if( !numGraphics )
				throw( "No Graphics" );
			YPsd3GraphicID* pGraphicIDs = graphicLibrary.GetGraphicIDs(numGraphics);

			// Pop up the graphic selection dialog
			CSelectGraphicDialog selectGraphicDialog( pGraphicNames, numGraphics );
			selectGraphicDialog.InitModalIndirect( GetResourceManager( ).GetResourceDialogTemplate( DIALOG_SELECT_GRAPHIC ) );

			if( selectGraphicDialog.DoModal( ) == IDOK )
				{
				// Get the selected graphic
				m_pGraphic = dynamic_cast<RGraphic*>(graphicLibrary.GetGraphic( fileStream, pGraphicIDs[selectGraphicDialog.m_nGraphic]));
				
				#if 0
				// REVIEW 3/4/97 GKB The following code is test code which will automatically
				// copy the data in m_pGraphic to the clipboard so we can test pasting.
				TpsAssertAlways("Copying PSD graphic to clipboard for testing.");
				{
					RPsd3Graphic* pPsd3Graphic = dynamic_cast<RPsd3Graphic*>(m_pGraphic);
					if (pPsd3Graphic)
					{
						if (::OpenClipboard(NULL))
						{
							::EmptyClipboard();

							HGLOBAL hGlobal = NULL;
							uBYTE* pGlobal = NULL;
							try
							{
								hGlobal = ::GlobalAlloc(GHND, sizeof(RPsd3GraphicInfo) + pPsd3Graphic->GetGraphicDataSize());
								if (!hGlobal) throw ::kMemory;
								pGlobal = reinterpret_cast<uBYTE*>(::GlobalLock(hGlobal));
								if (!pGlobal) throw ::kMemory;

								// Fill in the graphic header
								// NOTE We use the m_Unused field to store the fAdjustLineWidth flag			
								RPsd3GraphicInfo* pGraphicInfo = reinterpret_cast<RPsd3GraphicInfo*>(pGlobal);
								memset(pGraphicInfo, 0x00, sizeof(RPsd3GraphicInfo));
								pGraphicInfo->m_GraphicSize = pPsd3Graphic->GetGraphicDataSize();
								pGraphicInfo->m_Unused = pPsd3Graphic->GetAdjustLineWidth();
								pGraphicInfo->m_Mono = pPsd3Graphic->GetMonochrome();

								// Copy the graphic data
								uBYTE* pGraphicData = pGlobal + sizeof(RPsd3GraphicInfo);
								memcpy(pGraphicData, pPsd3Graphic->GetGraphicData(), pPsd3Graphic->GetGraphicDataSize());

								// Unlock the memory
								if (pGlobal) ::GlobalUnlock(hGlobal);

								// Give the memory to the clipboard
								BOOLEAN fContainsGradientData = pPsd3Graphic ? pPsd3Graphic->ContainsGradientData() : FALSE;
								if (m_pGraphicDocument->GetComponentType() == kSquareGraphicComponent)
								{
									if (fContainsGradientData)
										::SetClipboardData(kPsd5SquareGraphicFormat, hGlobal);
									else
										::SetClipboardData(kPsd3SquareGraphicFormat, hGlobal);
								}
								else if (m_pGraphicDocument->GetComponentType() == kColumnGraphicComponent)
								{
									if (fContainsGradientData)
										::SetClipboardData(kPsd5ColumnGraphicFormat, hGlobal);										
									else
										::SetClipboardData(kPsd3ColumnGraphicFormat, hGlobal);										
								}
								else if (m_pGraphicDocument->GetComponentType() == kRowGraphicComponent)
								{
									if (fContainsGradientData)
										::SetClipboardData(kPsd5RowGraphicFormat, hGlobal);										
									else
										::SetClipboardData(kPsd3RowGraphicFormat, hGlobal);										
								}
								
								// Close the clipboard
								::CloseClipboard();
							}
							catch(YException)
							{
								if (pGlobal) ::GlobalUnlock(hGlobal);
								if (hGlobal) ::GlobalFree(hGlobal);
								::CloseClipboard();
								throw;
							}
						}
					}
				}				
				#endif
				
				//	Set our component attributes.
				SetAttributes( Info );
				}
			}

		catch( ... )
			{

			}
		}
	}

// ****************************************************************************
//
//  Function Name:	REditGraphicAction::REditGraphicAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditGraphicAction::REditGraphicAction( RGraphicDocument* pGraphicDocument, RScript& script )
	: m_pGraphic( NULL ),
	  m_pGraphicDocument( pGraphicDocument ),
	  RUndoableAction( m_ActionId, script )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	REditGraphicAction::~REditGraphicAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditGraphicAction::~REditGraphicAction( )
	{
	delete m_pGraphic;
	}

// ****************************************************************************
//
//  Function Name:	REditGraphicAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			TRUE: this does nothing other than set the state.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditGraphicAction::Do( )
	{
	#if 0
	// REVIEW 3/4/97 GKB The following code is test code.  It opens a dialog which displays the 
	// current clipboard image if it is in Psd3 or Psd5 format.
	TpsAssertAlways("Displaying contents of clipboard in PSD3 or PSD5 format for testing.");
	CPsdClipboardViewerDialog cDialog;
	cDialog.DoModal();	
	#endif

	//	If the graphic was not modified
	if ((m_asLastAction == kActionNotDone) && !m_pGraphic)
		return FALSE;
	m_pGraphic = m_pGraphicDocument->SetGraphic( m_pGraphic );
	m_pGraphicDocument->InvalidateAllViews();

	// Invalidate the render cache
	static_cast<RComponentView*>( m_pGraphicDocument->GetActiveView( ) )->InvalidateRenderCache( );

	//	Let all views of this document, undefine their outline path
	YViewIterator	iterator		= m_pGraphicDocument->GetViewCollectionStart( );
	YViewIterator	iteratorEnd	= m_pGraphicDocument->GetViewCollectionEnd( );
	for ( ; iterator != iteratorEnd; ++iterator )
		{
		TpsAssertIsObject( RComponentView, *iterator );
		RComponentView*	pView	= static_cast<RComponentView*>( *iterator );
		pView->UndefineOutlinePath( );
		}

	//	Update the view that its layout has changed
	RDocument*	pDocument	= m_pGraphicDocument->GetParentDocument( );
	if ( pDocument )
		pDocument->XUpdateAllViews( kLayoutChanged, 0 );

	return RUndoableAction::Do( );
	}

// ****************************************************************************
//
//  Function Name:	REditGraphicAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditGraphicAction::Undo( )
	{
	Do( );
	RUndoableAction::Undo( );
	}

// ****************************************************************************
//
//  Function Name:	REditGraphicAction::SetAttributes( const InfoMapEntry Info )
//
//  Description:		Set the given component attributes.
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	REditGraphicAction::SetAttributes( const InfoMapEntry& )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	REditGraphicAction::WriteScript( )
//
//  Description:		Write the action data to a script.
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditGraphicAction::WriteScript( RScript& /* script */ ) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	REditGraphicAction::Validate( )
//
//  Description:		Verify that the object is valid.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditGraphicAction::Validate( ) const
	{
	RUndoableAction::Validate( );

	}

#endif	// TPSDEBUG
