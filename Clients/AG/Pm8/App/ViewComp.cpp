#include "stdafx.h"
ASSERTNAME

#include "Pmw.h"
#include "PmwDoc.h"
#include "PmwView.h"
#include "Command.h"
#include "Commands.h"
#include "CompObj.h"
#include "CreateData.h"
#include "Browser.h"

#include "HeadlineDlg.h"
#include "TabbedEditImage.h"

// Framework Support
#include "ComponentTypes.h"
#include "ComponentView.h"
#include "ComponentDocument.h"

#include "ParentRDocument.h"	// Provides a generic parent document for use in custom graphic dialog handling.
#include "TimePieceDlg.h"
#include "SealDialog.h"
#include "LogoMakerDialog.h"
#include "InitCapDialog.h"
#include "NumberDialog.h"
#include "SignatureDialog.h"

// Component data map entries.  These are used in the CPComponentDataMap constructor below.
const SPComponentDataMapEntry kHeadlineDataMapEntry( STRING_MENU_EDIT_HEADLINE, IDCmd_InsertHeadline, IDCmd_ReplaceHeadline, CPmwView::HeadlineDialog );
const SPComponentDataMapEntry kImageDataMapEntry( IDS_MENU_EDIT_PICTURE, IDCmd_AddPicture, IDCmd_ReplacePicture, CPmwView::ImageGallery );
const SPComponentDataMapEntry kTimePieceDataMapEntry( IDS_MENU_EDIT_TIMEPIECE, IDCmd_InsertTimepiece, IDCmd_ReplaceTimepiece, CPmwView::TimePieceDialog );
const SPComponentDataMapEntry kSealDataMapEntry( IDS_MENU_EDIT_SEAL, IDCmd_InsertSeal, IDCmd_ReplaceSeal, CPmwView::SealDialog );
const SPComponentDataMapEntry kLogoDataMapEntry( IDS_MENU_EDIT_LOGO, IDCmd_InsertLogo, IDCmd_ReplaceLogo, CPmwView::LogoDialog );
const SPComponentDataMapEntry kInitCapDataMapEntry( IDS_MENU_EDIT_INITCAP, IDCmd_InsertInitCap, IDCmd_ReplaceInitCap, CPmwView::InitCapDialog );
const SPComponentDataMapEntry kNumberDataMapEntry( IDS_MENU_EDIT_NUMBER, IDCmd_InsertNumber, IDCmd_ReplaceNumber, CPmwView::NumberDialog );
const SPComponentDataMapEntry kSignatureDataMapEntry( IDS_MENU_EDIT_SIGNATURE, IDCmd_InsertSignature, IDCmd_ReplaceSignature, CPmwView::SignatureDialog );

// Component data map. (See constructor below)
static CPComponentDataMap _cComponentDataMap;

// ****************************************************************************
//
//  Function Name:	CPComponentDataMap::CPComponentDataMap( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
CPComponentDataMap::CPComponentDataMap()
{
	SetAt( kHeadlineComponent,						(void *) &kHeadlineDataMapEntry );
	SetAt( kVerticalHeadlineComponent,			(void *) &kHeadlineDataMapEntry );
	SetAt( kSpecialHeadlineComponent,			(void *) &kHeadlineDataMapEntry );
	SetAt( kVerticalSpecialHeadlineComponent, (void *) &kHeadlineDataMapEntry );
	SetAt( kImageComponent,							(void *) &kImageDataMapEntry );
	SetAt( kSquareGraphicComponent,				(void *) &kImageDataMapEntry );
	SetAt( kRowGraphicComponent,					(void *) &kImageDataMapEntry );
	SetAt( kColumnGraphicComponent,				(void *) &kImageDataMapEntry );
	SetAt( kCGMGraphicComponent,					(void *) &kImageDataMapEntry );
	SetAt( kTimepieceComponent,					(void *) &kTimePieceDataMapEntry );
	SetAt( kSealComponent,							(void *) &kSealDataMapEntry );
	SetAt( kLogoComponent,							(void *) &kLogoDataMapEntry );
	SetAt( kInitialCapComponent,					(void *) &kInitCapDataMapEntry );
	SetAt( kSmartNumberComponent,					(void *) &kNumberDataMapEntry );
	SetAt( kSignatureComponent,					(void *) &kSignatureDataMapEntry );
}

// ****************************************************************************
//
//  Function Name:	SPComponentDataMapEntry::SPComponentDataMapEntry( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
SPComponentDataMapEntry::SPComponentDataMapEntry(	YResourceId menuStringId,
																	YResourceId insertUndoRedoPairId,
																	YResourceId editUndoRedoPairId,
																	YComponentEditFunction pFunction )
	: m_InsertUndoRedoPairId( insertUndoRedoPairId )
	, m_EditUndoRedoPairId( editUndoRedoPairId )
	, m_MenuStringId( menuStringId )
	, m_pFunction( pFunction )
{
}

/*
// Helper that creates the doc's chosen component at the right place in the
// right mode.
*/

BOOL CPmwView::CreateNewComponent(CCreationData* pCreationData, PPNT* pOrigin /*=NULL*/, BOOL bAddUndo/* = TRUE*/)
{
	BOOLEAN fResult = FALSE;

	if (pCreationData)
	{
		CPmwDoc* pDoc = GetDocument();
		ComponentObjectPtr pObject = NULL;
		set_arrow_tool();

		if (pDoc->create_new_component(pCreationData, &(get_rc()->source_pbox), pOrigin, &pObject) == ERRORCODE_None)
		{
			//if this function is called from a paste command, the command will be added to the undo by the PostPaste call
			if(bAddUndo)
			{
				YComponentType type = pObject->GetComponentType();
				SPComponentDataMapEntry* pMapData;

				UINT nResID = IDCmd_InsertObject;

				if (_cComponentDataMap.Lookup( type, (void *&) pMapData ) && pMapData->m_InsertUndoRedoPairId)
					nResID = pMapData->m_InsertUndoRedoPairId;

				pDoc->AddCreateCommand( nResID );
			}

			fResult = TRUE;
		}
	}

	return fResult;
}

void CPmwView::OnUpdateReplaceObject(CCmdUI* pCmdUI)
{
   CPmwDoc *pDoc = GetDocument();
	BOOL fEnable = FALSE;

	if (pDoc->one_selection())
	{
		if (pDoc->selected_objects()->type() == OBJECT_TYPE_Component)
		{
			ComponentObject* pObject = (ComponentObject*) pDoc->selected_objects();
			YComponentType type = pObject->GetComponentType();
			SPComponentDataMapEntry* pMapData;

			if (_cComponentDataMap.Lookup( type, (void *&) pMapData ))
			{
				fEnable = pMapData->m_pFunction != NULL;

				if (fEnable)
				{
					// REVIEW: use resource manager
					CString strMenuText;
					if (!strMenuText.LoadString( pMapData->m_EditUndoRedoPairId ))
						strMenuText.LoadString( IDCmd_ReplaceObject );

					strMenuText += "...";
					pCmdUI->SetText( strMenuText );
				}
			}
		}
		else
		{
			// TODO: add code to handle non-component objects
		}
	}

   pCmdUI->Enable( fEnable );
}

void CPmwView::OnReplaceObject()
{
   CPmwDoc *pDoc = GetDocument();
	TpsAssert( pDoc->one_selection(), "Invalid Object Count!" );
	
	if (pDoc->selected_objects()->type() == OBJECT_TYPE_Component)
	{
		ComponentObject* pObject = (ComponentObject*) pDoc->selected_objects();
		EditComponentObject( pObject );
	
	} 
}

void CPmwView::EditComponentObject( ComponentObject* pObject, PPNT_PTR p /*= NULL*/ )
{
   CPmwDoc *pDoc = GetDocument();
	TpsAssert( pObject, "Invalid Object!" );
	
	YComponentType type = pObject->GetComponentType();
	SPComponentDataMapEntry* pMapData;

	if (_cComponentDataMap.Lookup( type, (void *&) pMapData ) && pMapData->m_pFunction)
	{
		UINT nResID = (pMapData->m_EditUndoRedoPairId ? pMapData->m_EditUndoRedoPairId : IDCmd_ReplaceObject);
		CCmdChangeSelects* pCommand = new CCmdChangeSelects( pMapData->m_EditUndoRedoPairId );

		if (pCommand->Before( this ))
		{
			RRealPoint mousePoint;

			if (p)
			{
				mousePoint.m_x = p->x;
				mousePoint.m_y = p->y;
			}

			WORD wEditFlags = kCopyAttributes;
			RComponentView* pOldComponent = pObject->GetComponentView();
			CCreationData* pReplaceData = ( this->*(pMapData->m_pFunction) )( 
				pOldComponent, mousePoint, (LONG) &wEditFlags );

			if (pReplaceData)
			{
				pDoc->replace_component( pObject, pReplaceData, wEditFlags );
				delete pReplaceData;

				if (pCommand->After())
				{
					pDoc->AddCommand( pCommand, FALSE );
					return;
				}
			}
		}

		delete pCommand;

	} // Lookup
}


// ****************************************************************************
//
//  Function Name:	CPmwView::DoUpdatePhotoWorkshop()
//
//  Description:		Handles enabling / disabling Photo Workshop Button and
//							menu item.
//
//							Note:	Called from CPmwView::OnUpdatePhotoWorkshop() in
//									ViewCmd.cpp
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPmwView::DoUpdatePhotoWorkshop( CCmdUI* pCmdUI ) 
{
	BOOLEAN bEnable = FALSE;

	CPmwDoc* pDoc = GetDocument();

	// only enable Photo Workshop if a single image is selected
	if( pDoc->one_selection() && pDoc->selected_objects()->type() == OBJECT_TYPE_Component )
	{
		// check for Photo Shop frame path
		ComponentObject* pObject = (ComponentObject*)pDoc->selected_objects();
		if( pObject->GetComponentType() == YComponentType( "Image" ) )
		{
			RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(pObject->GetComponentView()->GetInterface(kImageInterfaceId));
			if (pImageInterface)
			{
				RImage *pImage = pImageInterface->GetImage();
				if ( pImage )
				{
					bEnable = TRUE;

					// if the image has a frame path; we will not edit it
					RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(pImage);
					if ( pBitmap && pBitmap->GetFrameRPath() )
					{
						bEnable = FALSE;
					}
				}

				delete pImageInterface;
			}
		}
	}

	pCmdUI->Enable( bEnable );
}

// ****************************************************************************
//
//  Function Name:	CPmwView::DoPhotoWorkshop()
//
//  Description:		Processes the Photo Workshop Button and menu item.
//
//							Note:	Called from CPmwView::OnPhotoWorkshop() or
//									OnPhotoWorkshopMenuPalette() in ViewCmd.cpp
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPmwView::DoPhotoWorkshop( int nTabIndex ) 
{
	CPmwDoc* pDoc = GetDocument();

	TpsAssert( pDoc->one_selection(), "Menu item should have been disabled.");
	TpsAssert( pDoc->selected_objects()->type() == OBJECT_TYPE_Component, "Menu item should have been disabled.");
	TpsAssert( ((ComponentObject*)pDoc->selected_objects())->GetComponentType() == YComponentType( "Image" ), "Menu item should have been disabled.");

	// get the selected object
	ComponentObject* pObject = (ComponentObject*)pDoc->selected_objects();

	// create and undoable command
	CCmdChangeSelects* pCommand = new CCmdChangeSelects( IDS_PHOTO_WORKSHOP_UNDO_STRING );

	if( pCommand->Before( this ) )
	{
		RComponentView* pOldComponentView = pObject->GetComponentView();

		// invoke the Photo Workshop dialog
		RComponentDocument* pNewComponent = PhotoWorkshopDialog( pOldComponentView->GetComponentDocument(), nTabIndex );
		if( pNewComponent )
		{
			// on OK, swap components
			RComponentView* pNewComponentView = (RComponentView*)pNewComponent->GetActiveView();

			// get the relative sizes of the old and new components
			RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(pOldComponentView->GetInterface(kImageInterfaceId));
			RBitmapImage *pOldImage = static_cast<RBitmapImage *>( pImageInterface->GetImage() );
			RRealSize rOldImageSize = pOldImage->GetSizeInLogicalUnits();

			pImageInterface = dynamic_cast<RImageInterface*>(pNewComponentView->GetInterface(kImageInterfaceId));
			RBitmapImage *pNewImage = static_cast<RBitmapImage *>( pImageInterface->GetImage() );
			RRealSize rNewImageSize = pNewImage->GetSizeInLogicalUnits();
			delete pImageInterface;
			
			// re-scale the new component bounding rect by the relative dimensions
			RRealSize rScaleFactor( rNewImageSize.m_dx  / rOldImageSize.m_dx,
											rNewImageSize.m_dy / rOldImageSize.m_dy );

			YComponentBoundingRect rBoundingRect = pOldComponentView->GetBoundingRect();
			rBoundingRect.UnrotateAndResizeAboutPoint( rBoundingRect.GetCenterPoint(), rScaleFactor );

			// if a previous crop had been resized, so that removing that crop or recropping
			//	the image now causes it to be humungous, keep it inside the panel
			PBOX panel;
			pDoc->get_panel_world( &panel );
			YRealDimension panelWidth  = PageResolutionToSystemDPI( panel.Width() );
			YRealDimension panelHeight = PageResolutionToSystemDPI(panel.Height() );

			if ( rBoundingRect.Width() > panelWidth || rBoundingRect.Height() > panelHeight )
			{
				// let's avoid resizing images that were already larger than the panel
				YComponentBoundingRect rOldBoundingRect = pOldComponentView->GetBoundingRect();
				if( rBoundingRect.Width()  > rOldBoundingRect.Width()  ||
					 rBoundingRect.Height() > rOldBoundingRect.Height() )
				{
					// but allow the rotations of oversized images
					if( rNewImageSize.m_dx != rOldImageSize.m_dy ||
						 rNewImageSize.m_dy != rOldImageSize.m_dx )
					{
						// ok, we're really going to be too big, so scale to panel
						RRealRect rPanelRect( RRealSize(panelWidth, panelHeight) );
						RRealRect rBoundsRect( rBoundingRect.WidthHeight() );

						YScaleFactor rPanelScale = rBoundsRect.ShrinkToFit( rPanelRect );
						rBoundingRect.ScaleAboutPoint( rBoundingRect.GetCenterPoint(), RRealSize(rPanelScale, rPanelScale) );

					}
				}
			}

			// put origin at 0, 0 (not sure if this is really necessary)
			rBoundingRect.Offset( RRealSize(-rBoundingRect.m_TopLeft.m_x, -rBoundingRect.m_TopLeft.m_y) );

			// we've got our new bounding rect
			pNewComponentView->SetBoundingRect( rBoundingRect );

			// swap components
			CComponentData cData( pNewComponent );
			WORD wEditFlags = kCopyAttributes | kResetSize;
			pDoc->replace_component( pObject, &cData, wEditFlags );

			if( pCommand->After() )
			{
				pDoc->AddCommand( pCommand, FALSE );
				return;
			}
		}

		delete pCommand;
	}
}

// ****************************************************************************
//
//  Function Name:	CPmwView::PhotoWorkshopDialog()
//
//  Description:		Brings up the dialog.
//
//  Returns:			New component, if user edited the image
//
// ****************************************************************************
//
RComponentDocument* CPmwView::PhotoWorkshopDialog( RComponentDocument* pComponent, int nSelTab )
{
	RComponentDocument* pNewComponent = NULL;

	// get default path for "Save As" option
	CPmwDoc *pPmwDoc = GetDocument();
	RMBCString csDefaultArtDir = pPmwDoc->GetPathManager()->ExpandPath("[[P]]");

	// invoke the image edit dialog
	RTabbedEditImage rTabbedEditImageDlg( pComponent, nSelTab, &csDefaultArtDir );
	if (rTabbedEditImageDlg.DoModal() == IDOK)
	{
		// create a component with edited features
		RWaitCursor	waitCursor;
		pNewComponent = rTabbedEditImageDlg.CreateComponentWithEffects();
	}

	return pNewComponent;
}

/* Add a headline component to the current project.
*/
void CPmwView::OnAddHeadline() 
{
	RRealPoint pt;
	CCreationData* pCreateData = HeadlineDialog( NULL, pt, 0 );

	if (pCreateData)
	{
		CreateNewComponent( pCreateData );
		delete pCreateData;
	}
}

/* Add a timepiece component to the current project.
*/
void CPmwView::OnAddTimePiece()
{
	RRealPoint pt;
	CCreationData* pCreateData = TimePieceDialog( NULL, pt, 0 );

	if (pCreateData)
	{
		CreateNewComponent( pCreateData );
		delete pCreateData;
	}
}


/* Add a seal component to the current project.
*/
void CPmwView::OnAddSeal()
{
	RRealPoint pt;
	CCreationData* pCreateData = SealDialog( NULL, pt, 0 );

	if (pCreateData)
	{
		CreateNewComponent( pCreateData );
		delete pCreateData;
	}
}


/* Add a logo component to the current project.
*/
void CPmwView::OnAddLogo()
{
	RRealPoint pt;
	CCreationData* pCreateData = LogoDialog( NULL, pt, 0 );

	if (pCreateData)
	{
		CreateNewComponent( pCreateData );
		delete pCreateData;
	}
}


/* Add an initial capital component to the current project.
*/
void CPmwView::OnAddInitCap()
{
	RRealPoint pt;
	CCreationData* pCreateData = InitCapDialog( NULL, pt, 0 );

	if (pCreateData)
	{
		CreateNewComponent( pCreateData );
		delete pCreateData;
	}
}


/* Add a number component to the current project.
*/
void CPmwView::OnAddNumber()
{
	RRealPoint pt;
	CCreationData* pCreateData = NumberDialog( NULL, pt, 0 );

	if (pCreateData)
	{
		CreateNewComponent( pCreateData );
		delete pCreateData;
	}
}


/* Add a signature component to the current project.
*/
void CPmwView::OnAddSignature()
{
	RRealPoint pt;
	CCreationData* pCreateData = SignatureDialog( NULL, pt, 0 );

	if (pCreateData)
	{
		CreateNewComponent( pCreateData );
		delete pCreateData;
	}
}

// ****************************************************************************
//
//  Function Name:	CPmwView::HeadlineDialog( )
//
//  Description:		Brings up the headline dialog
//
//  Returns:			Created component
//
//  Exceptions:		None
//
// ****************************************************************************
//
CCreationData* CPmwView::HeadlineDialog( RComponentView* pComponent, const RRealPoint& , uLONG ulData ) const
{
	BOOLEAN	fNewHeadlineUI = FALSE;

	// Get the edited component
	RComponentDocument* pEditedComponent = pComponent ? pComponent->GetComponentDocument( ) : NULL;

	// Create a graphic browser object
	RHeadlineDlg dlg( pEditedComponent );

	// Invoke the dialog
	if( dlg.DoModal( ) == IDOK )
	{
		RWaitCursor	waitCursor;

		//
		// Set the component attributes...
		//
		RComponentDocument* pComponentDoc = dlg.CreateNewComponent( NULL );

		if (pEditedComponent)
		{
			WORD& wFlags = *(WORD *) ulData;
			wFlags &= ~kCopyAttributes;
			wFlags &= ~kResetSize;

			pComponentDoc->SetComponentAttributes(
				pEditedComponent->GetComponentAttributes() ) ;

			if (pComponentDoc->GetComponentType() != pEditedComponent->GetComponentType())
				wFlags |= kResetSize;
		}

		if (pComponentDoc)
			return new CComponentData( pComponentDoc );
	}

	return NULL;
}

/* Launches the time piece dialog to either create a new timepiece or edit an
	existing time piece.
*/
CCreationData * CPmwView::TimePieceDialog(
	RComponentView* pComponent,	// The time piece to edit.  NULL if creating a new timepiece.
	const RRealPoint& ,				// The mouse point. (Unused)
	uLONG )								// Edit information. (Unused).
	const
{
	// Get the edited component
	RComponentDocument* pComponentDoc = pComponent ? pComponent->GetComponentDocument( ) : NULL;

	// Launch the dialog and allow the user to edit it.
	CPParentRDocument rParentDoc;
	RTimepieceDialog dialogTimepiece( &rParentDoc, pComponentDoc );
	int nRetVal = dialogTimepiece.DoModal();

	RComponentDocument * pNewComponent = NULL;
	if (nRetVal == IDOK)
	{
		RWaitCursor	waitCursor;

		// Request the new component.
		pNewComponent = dialogTimepiece.GetComponent();

		// Transfer the component information from the original.
		if (pComponentDoc)
		{
			pNewComponent->SetComponentAttributes( pComponentDoc->GetComponentAttributes() ) ;
		}

		if (pNewComponent)
			return new CComponentData( pNewComponent );
	}

	return NULL;
}

/* Launches the seal dialog to either create a new seal or edit an
	existing seal.
*/
CCreationData * CPmwView::SealDialog(
	RComponentView* pComponent,	// The time piece to edit.  NULL if creating a new timepiece.
	const RRealPoint& ,				// The mouse point. (Unused)
	uLONG )								// Edit information. (Unused).
	const
{
	// Get the edited component
	RComponentDocument* pComponentDoc = pComponent ? pComponent->GetComponentDocument( ) : NULL;

	// Launch the dialog and allow the user to edit it.
	CPParentRDocument rParentDoc;
	RSealDialog dialogSeal( &rParentDoc, pComponentDoc );
	int nRetVal = dialogSeal.DoModal();

	RComponentDocument * pNewComponent = NULL;
	if (nRetVal == IDOK)
	{
		RWaitCursor	waitCursor;

		// Request the new component.
		pNewComponent = dialogSeal.GetComponent();

		// Transfer the component information from the original.
		if (pComponentDoc)
		{
			pNewComponent->SetComponentAttributes( pComponentDoc->GetComponentAttributes() ) ;
		}

		if (pNewComponent)
			return new CComponentData( pNewComponent );
	}

	return NULL;
}

/* Launches the logo dialog to either create a new logo or edit an
	existing one.
*/
CCreationData *	CPmwView::LogoDialog(
	RComponentView* pComponent,	// The time piece to edit.  NULL if creating a new timepiece.
	const RRealPoint& ,				// The mouse point. (Unused)
	uLONG )								// Edit information. (Unused).
	const
{
	// Get the edited component
	RComponentDocument* pComponentDoc = pComponent ? pComponent->GetComponentDocument( ) : NULL;

	// Launch the dialog and allow the user to edit it.
	CPParentRDocument rParentDoc;
	RLogoMakerDialog dialogLogo( &rParentDoc, pComponentDoc );
	int nRetVal = dialogLogo.DoModal();

	if (nRetVal == IDOK)
	{
		RWaitCursor	waitCursor;

		// Request the new component.
		RComponentDocument *pNewComponent = dialogLogo.GetComponent();

		// Transfer the component information from the original.
		if (pComponentDoc)
		{
			pNewComponent->SetComponentAttributes( pComponentDoc->GetComponentAttributes() ) ;
		}

		if (pNewComponent)
			return new CComponentData( pNewComponent );
	}

	return NULL;
}


/* Launches the initial capital dialog to either create a new initial capital
	component or edit an existing one.
*/
CCreationData *	CPmwView::InitCapDialog(
	RComponentView* pComponent,	// The time piece to edit.  NULL if creating a new timepiece.
	const RRealPoint& ,				// The mouse point. (Unused)
	uLONG )								// Edit information. (Unused).
	const
{
	// Get the edited component
	RComponentDocument* pComponentDoc = pComponent ? pComponent->GetComponentDocument( ) : NULL;

	// Launch the dialog and allow the user to edit it.
	CPParentRDocument rParentDoc;
	RInitCapDialog dialogInitCap( &rParentDoc, pComponentDoc );
	int nRetVal = dialogInitCap.DoModal();

	if (nRetVal == IDOK)
	{
		RWaitCursor	waitCursor;

		// Request the new component.
		RComponentDocument *pNewComponent = dialogInitCap.GetComponent();

		// Transfer the component information from the original.
		if (pComponentDoc)
		{
			pNewComponent->SetComponentAttributes( pComponentDoc->GetComponentAttributes() ) ;
		}

		if (pNewComponent)
			return new CComponentData( pNewComponent );
	}

	return NULL;
}


/* Launches the number dialog to either create a new number or edit an
	existing one.
*/
CCreationData *	CPmwView::NumberDialog(
	RComponentView* pComponent,	// The time piece to edit.  NULL if creating a new timepiece.
	const RRealPoint& ,				// The mouse point. (Unused)
	uLONG )								// Edit information. (Unused).
	const
{
	// Get the edited component
	RComponentDocument* pComponentDoc = pComponent ? pComponent->GetComponentDocument( ) : NULL;

	// Launch the dialog and allow the user to edit it.
	CPParentRDocument rParentDoc;
	RNumberDialog dialogNumber( &rParentDoc, pComponentDoc );
	int nRetVal = dialogNumber.DoModal();

	if (nRetVal == IDOK)
	{
		RWaitCursor	waitCursor;

		// Request the new component.
		RComponentDocument *pNewComponent = dialogNumber.GetComponent();

		// Transfer the component information from the original.
		if (pComponentDoc)
		{
			pNewComponent->SetComponentAttributes( pComponentDoc->GetComponentAttributes() ) ;
		}

		if (pNewComponent)
			return new CComponentData( pNewComponent );
	}

	return NULL;
}


/* Launches the signature dialog to either create a new signature or edit an
	existing one.
*/
CCreationData *	CPmwView::SignatureDialog(
	RComponentView* pComponent,	// The time piece to edit.  NULL if creating a new timepiece.
	const RRealPoint& ,				// The mouse point. (Unused)
	uLONG )								// Edit information. (Unused).
	const
{
	// Get the edited component
	RComponentDocument* pComponentDoc = pComponent ? pComponent->GetComponentDocument( ) : NULL;

	// Launch the dialog and allow the user to edit it.
	CPParentRDocument rParentDoc;
	RSignatureDialog dialogSignature( &rParentDoc, pComponentDoc );
	int nRetVal = dialogSignature.DoModal();

	if (nRetVal == IDOK)
	{
		RWaitCursor	waitCursor;

		// Request the new component.
		RComponentDocument *pNewComponent = dialogSignature.GetComponent();

		// Transfer the component information from the original.
		if (pComponentDoc)
		{
			pNewComponent->SetComponentAttributes( pComponentDoc->GetComponentAttributes() ) ;
		}

		if (pNewComponent)
			return new CComponentData( pNewComponent );
	}

	return NULL;
}

CCreationData * CPmwView::ImageGallery( RComponentView* pComponent, const RRealPoint& mousePoint, uLONG ulEditInfo ) const
{
	CPmwDoc* pDoc = (CPmwDoc *) GetDocument();
	CArtBrowserDialog Browser( CArtBrowserDialog::TYPE_ModalPicture );
	CString strLastPicture;

	if (pComponent)
	{
		RImageInterface* pInterface = (RImageInterface *) pComponent->GetInterface( kImageInterfaceId );

		if (pInterface)
		{
			strLastPicture = (CString) pInterface->GetPath();
			delete pInterface;
		}
	}

	Browser.m_pDoc = pDoc;
	Browser.m_pszLastPicture = strLastPicture;

	if (IDOK == Browser.DoModal())
	{
		GRAPHIC_CREATE_STRUCT* pGCS = pDoc->get_gcs();

		if (ulEditInfo)
		{
			WORD& wFlags = *(WORD *) ulEditInfo;
			wFlags |= kResetSize;
		}

		return new CGraphicData( pGCS );
	}

	return NULL;
}
