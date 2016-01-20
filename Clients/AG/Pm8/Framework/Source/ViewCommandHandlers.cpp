// ****************************************************************************
//
//  File Name:			ViewCommandHandlers.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RView class command handlers
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
//  $Logfile:: /PM8/Framework/Source/ViewCommandHandlers.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "View.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "DeactivateComponentAction.h"
#include "DeleteSelectionAction.h"
#include "GroupSelectionAction.h"
#include "BreakGroupAction.h"
#include "EditComponentAttribAction.h"
#include "CopySelectionAction.h"
#include "PasteAction.h"
#include "SetLockStateActions.h"
#include "NewComponentAction.h"
#include "FrameSelectionAction.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ApplicationGlobals.h"
#include "ComponentDocument.h"
#include "ClipboardDataTransfer.h"
#include "StandaloneApplication.h"
#include "ComponentDataRenderer.h"
#include "FramesDialog.h"
#include "ScaleDialog.h"
#include "AlignDialog.h"
#include "RotateDialog.h"
#include "ResourceManager.h"
#include "DuplicateSelectionAction.h"
#include "ResizeSelectionAction.h"
#include "AlignSelectionAction.h"
#include "RotateSelectionAction.h"
#include "ScaleSelectionToParentAction.h"
#include "FlipSelectionAction.h"
#include "Settings.h"
#include "SettingsInterface.h"
#include	"ComponentShadowSelectionAction.h"
#include	"ComponentGlowSelectionAction.h"
#include	"ComponentVignetteSelectionAction.h"
#include "ComponentTypes.h"
#include "OrderSelectionActions.h"
#include "FrameColorSelectionAction.h"
#include "ComponentManager.h"
#include "ChangeBackgroundColorAction.h"
#include "SoftShadowSettings.h"
#include "SoftGlowSettings.h"
#include "SoftVignetteSettings.h"

#ifdef _WINDOWS
#include "WinColorDlg.h"
#endif

// Command Map
RCommandMap<RView, RCommandTarget> RView::m_CommandMap;

RCommandMap<RView, RCommandTarget>::RCommandMap( )
	{
	HandleAndUpdateCommand( COMMAND_MENU_EDIT_PASTE, RView::OnPaste, RView::OnUpdatePaste );
	HandleAndUpdateCommand( COMMAND_MENU_EDIT_PASTE_HERE, RView::OnPasteHere, RView::OnUpdatePaste );
	HandleAndUpdateCommand( COMMAND_MENU_EDIT_SELECTALL, RView::OnSelectAll, RView::OnUpdateSelectAll );
	HandleAndUpdateCommand( COMMAND_MENU_EDIT_DELETE, RView::OnDelete, RView::OnUpdateDelete );
	HandleAndUpdateCommand( COMMAND_MENU_EDIT_CLEAR, RView::OnDelete, RView::OnUpdateDelete );
	HandleAndUpdateCommand( COMMAND_MENU_ARRANGE_GROUP, RView::OnGroup, RView::OnUpdateGroup );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_CREATEGROUP, RView::OnCreateGroup, RView::OnUpdateCreateGroup );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_BREAKGROUP, RView::OnBreakGroup, RView::OnUpdateBreakGroup );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_EDIT_COMPONENT_ATTRIBUTES, RView::OnEditComponentAttributes, RView::OnUpdateEditComponentAttributes );
	HandleAndUpdateCommand( COMMAND_MENU_EDIT_COPY, RView::OnCopy, RView::OnUpdateCopy );
	HandleAndUpdateCommand( COMMAND_MENU_EDIT_CUT, RView::OnCut, RView::OnUpdateCut );
	HandleAndUpdateCommand( COMMAND_MENU_EDIT_DUPLICATE, RView::OnDuplicate, RView::OnUpdateDuplicate );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_LOCK,	RView::OnLock, RView::OnUpdateLock );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_SCALE, RView::OnScale, RView::OnUpdateScale );
	HandleAndUpdateCommand(	COMMAND_MENU_EFFECTS_ALIGN, RView::OnAlign, RView::OnUpdateAlign );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_ROTATE, RView::OnRotate, RView::OnUpdateRotate );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_ROTATE_LEFT90, RView::OnRotateLeft90, RView::OnUpdateRotate );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_ROTATE_RIGHT90, RView::OnRotateRight90, RView::OnUpdateRotate );
	HandleCommandRange( COMMAND_MENU_INSERT_NONSTANDARDSTART, COMMAND_MENU_INSERT_NONSTANDARDEND, RView::OnInsertNonstandard );
	HandleAndUpdateCommandRange( MIN_SETTINGS_COMMAND_ID, MAX_SETTINGS_COMMAND_ID, RView::OnComponentCommand, RView::OnUpdateComponentCommand );
	HandleCommand( COMMAND_MENU_EFFECTS_FLIP_HORIZONTAL, RView::OnFlipHorizontal );
	HandleCommand( COMMAND_MENU_EFFECTS_FLIP_VERTICAL, RView::OnFlipVertical );
	HandleCommand( COMMAND_MENU_EFFECTS_FLIP_BOTH, RView::OnFlipBoth );
	UpdateCommandRange( COMMAND_MENU_EFFECTS_FLIP_HORIZONTAL, COMMAND_MENU_EFFECTS_FLIP_BOTH, RView::OnUpdateFlip );
	HandleCommand( COMMAND_MENU_EFFECTS_SHADOW, RView::OnShadow );
	HandleCommand( COMMAND_MENU_EFFECTS_GLOW_ON_OFF, RView::OnGlowOnOff );
	HandleCommand( COMMAND_MENU_EFFECTS_VIGNETTE_ON_OFF, RView::OnVignetteOnOff );
	HandleAndUpdateCommandRange( COMMAND_MENU_EFFECTS_FRAME_NONE, COMMAND_MENU_EFFECTS_FRAME_DIALOG, RView::OnFrame, RView::OnUpdateFrame );
	UpdateCommandRange( COMMAND_MENU_EFFECTS_SHADOW, COMMAND_MENU_EFFECTS_SHADOW, RView::OnUpdateShadow );
	UpdateCommandRange( COMMAND_MENU_EFFECTS_GLOW_ON_OFF, COMMAND_MENU_EFFECTS_GLOW_ON_OFF, RView::OnUpdateGlowOnOff );
	UpdateCommandRange( COMMAND_MENU_EFFECTS_VIGNETTE_ON_OFF, COMMAND_MENU_EFFECTS_VIGNETTE_ON_OFF, RView::OnUpdateVignetteOnOff );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_ORDER_BRINGFORWARD, RView::OnBringForward, RView::OnUpdateBringForward );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_ORDER_SENDBACKWARD, RView::OnSendBackward, RView::OnUpdateSendBackward );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_ORDER_BRINGTOFRONT, RView::OnBringToFront, RView::OnUpdateBringToFront );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_ORDER_SENDTOBACK, RView::OnSendToBack, RView::OnUpdateSendToBack );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_COLOR_BEHIND_OBJECT, RView::OnColorBehindObject, RView::OnUpdateColorBehindObject );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_USE_OFFSCREENS, RView::OnSetUseOffscreens, RView::OnUpdateSetUseOffscreens );
	HandleCommand( COMMAND_MENU_EDIT_EDIT_SELECTED, RView::OnEditSelected );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_CENTER_ON_PANEL, RView::OnCenterInView, RView::OnUpdateCenterInView );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_SIZE_TO_PANEL, RView::OnSizeToView, RView::OnUpdateScaleToView );
	HandleAndUpdateCommand( COMMAND_MENU_EFFECTS_SCALE_TO_PANEL, RView::OnScaleToView, RView::OnUpdateScaleToView );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_ALIGN_TO_PANEL_LEFT, RView::OnAlignToViewLeft, RView::OnUpdateCenterInView );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_ALIGN_TO_PANEL_RIGHT, RView::OnAlignToViewRight, RView::OnUpdateCenterInView );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_ALIGN_TO_PANEL_TOP, RView::OnAlignToViewTop, RView::OnUpdateCenterInView );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_ALIGN_TO_PANEL_BOTTOM, RView::OnAlignToViewBottom, RView::OnUpdateCenterInView );



	RouteCommandToFunction( (RCommandTarget* (RView::*)( ) const ) RView::GetRDocument );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnPaste( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnPaste( )
	{
	RPasteAction* pAction = new RPasteAction( GetRDocument( ) );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdatePaste( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdatePaste( RCommandUI& commandUI ) const
	{
	// If a supported format was found, enable the menu
	if( ::GetApplication( ).SupportedFormatOnClipboard( ) )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnPasteHere( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnPasteHere( )
	{
	RPasteHereAction* pAction = new RPasteHereAction( GetRDocument( ), m_ContextMenuPoint );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnSelectAll( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnSelectAll( )
	{
	// Select everything that isn't already selected
	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	for( ; iterator != m_ComponentViewCollection.End( ); ++iterator )
		if( !GetSelection( )->IsSelected( *iterator ) )
			GetSelection( )->Select( *iterator, TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateSelectAll( )
//
//  Description:		Enables the Edit::SelectAll menu item if appropriate
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateSelectAll( RCommandUI& commandUI ) const
	{
	// Only enable the menu item if there is a selectable component that isnt selected
	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	for( ; iterator != m_ComponentViewCollection.End( ); ++iterator )
		{
		if( !GetSelection( )->IsSelected( *iterator ) && ( *iterator )->GetComponentAttributes( ).IsSelectable( ) )
			{
			commandUI.Enable( );
			return;
			}
		}

	commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnDelete( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnDelete( )
	{
	// Only do if there are unlocked objects in the current selection
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		{
		if( ( *iterator )->GetComponentAttributes( ).IsDeletable( ) )
			{
			// Create and do a delete selection action
			RDeleteSelectionAction* pAction = new RDeleteSelectionAction( GetSelection( ) );
			GetRDocument( )->SendAction( pAction );
			break;
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateDelete( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateDelete( RCommandUI& commandUI ) const
	{
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		{
		if( ( *iterator )->GetComponentAttributes( ).IsDeletable( ) )
			{
			commandUI.Enable( );
			return;
			}
		}

	commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnGroup( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnGroup( )
	{
	RAction*					pAction		= NULL;
	RCompositeSelection*	pSelection	= GetSelection();
	// Create and do a create group action if more than one object is selected
	if( pSelection->Count( ) > 1 )
		pAction = new RGroupSelectionAction( pSelection );
	else if ( pSelection->Count() == 1 )
		pAction = new RBreakGroupAction( GetSelection( ) );

	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateGroup( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateGroup( RCommandUI& commandUI ) const
	{
	YResourceId				menuId		= 0;
	BOOLEAN					fEnable		= FALSE;
	RCompositeSelection*	pSelection	= GetSelection();
	YCounter					count			= pSelection->Count( );

	// Enable the 'Group' menu if more than one object selected.
	//		and we have the group component
	if( count>1  && ::GetComponentManager( ).IsComponentAvailable( kGroupComponent ) )
		{
		YSelectionIterator	iterator		= pSelection->Start( );
		YSelectionIterator	iteratorEnd	= pSelection->End( );

		//	First, make sure that all selected objects can be put into a group...
		for ( ; iterator != iteratorEnd; ++iterator )
			{
			//	Can't group objects not in the NormalZLayer
			if ( ( *iterator )->GetComponentAttributes( ).GetZLayer( ) != kNormalZLayer )
				--count;
			}

		if ( count > 1 )
			menuId	= STRING_MENU_GROUP;	//	Assume we can group
		}
	//	
	if ( count == 1 )
		{
		RComponentDocument* pDocument = static_cast<RComponentDocument*>( ( *GetSelection( )->Start( ) )->GetRDocument( ) );
		if( pDocument->GetComponentType( ) == kGroupComponent )
			menuId	= STRING_MENU_UNGROUP;
		}

	if ( menuId != 0)
		{
		commandUI.Enable( );
		commandUI.SetString( GetResourceManager().GetResourceString( menuId ) );
		}
	else
		{
 		commandUI.Disable( );
		commandUI.SetString( GetResourceManager().GetResourceString( STRING_MENU_GROUP ) );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnCreateGroup( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnCreateGroup( )
	{
	// Create and do a create group action
	if( !GetSelection( )->IsEmpty( ) )
		{
		RGroupSelectionAction* pAction = new RGroupSelectionAction( GetSelection( ) );
		GetRDocument( )->SendAction( pAction );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateCreateGroup( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateCreateGroup( RCommandUI& commandUI ) const
	{
	// Enable the menu item if the group component is available and there is more than 1 item selected
	if( GetSelection( )->Count( ) > 1 && ::GetComponentManager( ).IsComponentAvailable( kGroupComponent ) )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnBreakGroup( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnBreakGroup( )
	{
	// Create and do a break group action
	RBreakGroupAction* pAction = new RBreakGroupAction( GetSelection( ) );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateBreakGroup( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateBreakGroup( RCommandUI& commandUI ) const
	{
	// Enable the menu item if there is only one item selected, and it is a group.
	if( GetSelection( )->Count( ) == 1 )
		{
		RComponentDocument* pDocument = static_cast<RComponentDocument*>( ( *GetSelection( )->Start( ) )->GetRDocument( ) );
		if( pDocument->GetComponentType( ) == kGroupComponent )
			{
			commandUI.Enable( );
			return;
			}
		}

	commandUI.Disable( );
	}
		
// ****************************************************************************
//
//  Function Name:	RView::OnEditComponentAttributes( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnEditComponentAttributes( )
	{
	// Create and do an edit component attributes action
	REditComponentAttributesAction* pAction = new REditComponentAttributesAction( GetSelection( ) );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateEditComponentAttributes( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateEditComponentAttributes( RCommandUI& commandUI ) const
	{
	// Only enable the menu item if there is a single component selected. I dont yet support
	// setting the attributes for a multiple selection
	if( GetSelection( )->Count( ) == 1 )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnCopy( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnCopy( )
	{
	// Create and do a copy selection action
	RClipboardDataTarget clipboard( new RComponentDataRenderer );
	RCopySelectionAction* pAction = new RCopySelectionAction( GetSelection( ), &clipboard, FALSE );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateCopy( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateCopy( RCommandUI& commandUI ) const
	{
	// Enable the menu item if the selection is not empty
	if( GetSelection( )->IsEmpty( ) )
		commandUI.Disable( );
	else
		commandUI.Enable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnCut( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnCut( )
	{
	// Create and do a cut selection action
	RCutSelectionAction* pAction = new RCutSelectionAction( GetSelection( ) );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateCut( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateCut( RCommandUI& commandUI ) const
	{
	// Same as delete
	OnUpdateDelete( commandUI );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnDuplicate( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnDuplicate( )
	{
	// Create and do a duplicate selection action
	RDuplicateSelectionAction* pAction = new RDuplicateSelectionAction( GetSelection( ) );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateDuplicate( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateDuplicate( RCommandUI& commandUI ) const
	{
	// Enable the menu item if there are any non-unique components selected
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		if( ( *iterator )->GetComponentAttributes( ).GetUniqueId( ) == kNotUnique )
			{
			commandUI.Enable( );
			return;
			}

	commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnScale( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnScale( )
	{
	// Put the selection in a component collection
	RComponentCollection componentCollection( *GetSelection( ) );

	// Get the min and max scale factors
	RRealSize minScaleFactor;
	RRealSize maxScaleFactor;
	componentCollection.GetSizeLimits( minScaleFactor, maxScaleFactor );

	// Create and invoke the scale dialog
	RScaleDialog dlgScale( minScaleFactor, maxScaleFactor );
	if( dlgScale.DoModal( ) != IDOK )
		return;

	// Get the center of the selection and use it as the center of scaling
	RIntPoint centerPoint = GetSelection( )->GetBoundingRect( ).GetCenterPoint( );

	// Get the scale factor and aspect-correctness from the scale dialog
	RRealSize scaleFactor;
	BOOLEAN fAspectCorrect;
	dlgScale.GetScaleInfo( scaleFactor, fAspectCorrect );

	// Remove the selection, because the resize selection action does not expect it to be there
	GetSelection( )->Remove( );

	// Create and send an action
	RResizeSelectionAction* pAction = new RResizeSelectionAction( GetSelection( ), centerPoint, scaleFactor, fAspectCorrect );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateScale( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnUpdateScale( RCommandUI& commandUI ) const
	{
	// Enable the menu item if there are any resizable components selected
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		if( ( *iterator )->GetComponentAttributes( ).IsResizable( ) )
			{
			commandUI.Enable( );
			return;
			}

	commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnAlign( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnAlign( )
{
	RAlignDialog	dlgAlign;

	if ( dlgAlign.DoModal() == IDOK )
		{
		RAlignSelectionAction* pAction = new RAlignSelectionAction( GetSelection( ), dlgAlign.GetHorizontalAlignment(), dlgAlign.GetVerticalAlignment(), RAlignSelectionAction::kToEachOther );
		GetRDocument( )->SendAction( pAction );
		}

	// Construct and send an action
}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateAlign( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

void	RView::OnUpdateAlign( RCommandUI& commandUI ) const
{
	// Only enable the command if there are more than one unlocked items in the selection.
	YCounter	i = 0;

	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		{
		const RComponentAttributes& componentAttributes = ( *iterator )->GetComponentAttributes( );
		
		if( !componentAttributes.IsLocked( ) )
			++i;

		if( i > 1 )
			{
			commandUI.Enable( );
			return;
			}
 		}

	commandUI.Disable( );
}


// ****************************************************************************
//
//  Function Name:	RView::OnRotate( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnRotate( )
{
	RRotateDialog	dlgRotate;

	BOOLEAN	fFlipped	= FALSE;
	YCounter	i			= 0;
	YAngle	angle		= 0.0;

	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		{
		if( ++i > 1 )
			{
			angle = 0.0;
			break;
			}
		YRealDimension	xScale;
		YRealDimension	yScale;
		fFlipped	= ( *iterator )->GetBoundingRect( ).GetTransform( ).Decompose( angle, xScale, yScale );
 		}

	if ( fFlipped )
		angle	= 2*kPI - angle;

	// If the selection only contains one component, set the dialog's rotation
	// angle to it.  If the selection is multiple, the dialog's rotation defaults
	// to 0.
	if ( i == 1 )
		dlgRotate.SetRotationAngle( angle );

	if ( dlgRotate.DoModal() != IDOK )
		return;

	// The action adds to the existing angle, but we want to set an absolute angle,
	// so add the difference between what it was before the dialog and what it was
	// set to.
	YAngle angleRotation = dlgRotate.GetRotationAngle() - angle;

	// Create a component collection from the current selection
	RComponentCollection componentCollection( *GetSelection( ) );

	// Get the center of the selection and use it as the center of scaling
	RIntPoint centerPoint = componentCollection.GetBoundingRect( ).GetCenterPoint( );

	// Remove the selection
	GetSelection( )->Remove( );

	// Create and send an action
	RRotateSelectionAction* pAction = new RRotateSelectionAction( GetSelection( ), centerPoint, angleRotation );
	GetRDocument( )->SendAction( pAction );
}

// ****************************************************************************
//
//  Function Name:	RView::OnRotate( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnRotateLeft90()
{
	YAngle angleRotation = ::DegreesToRadians( 90.0 );
	OnRotate( angleRotation );
}

// ****************************************************************************
//
//  Function Name:	RView::OnRotate( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnRotateRight90()
{
	YAngle angleRotation = ::DegreesToRadians( -90.0 );
	OnRotate( angleRotation );
}

// ****************************************************************************
//
//  Function Name:	RView::OnRotate( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnRotate( YAngle angleRotation )
{
	// Create a component collection from the current selection
	RComponentCollection componentCollection( *GetSelection( ) );

	// Get the center of the selection and use it as the center of scaling
	RIntPoint centerPoint = componentCollection.GetBoundingRect( ).GetCenterPoint( );

	// Remove the selection
	GetSelection( )->Remove( );

	// Create and send an action
	RRotateSelectionAction* pAction = new RRotateSelectionAction( GetSelection( ), centerPoint, angleRotation );
	GetRDocument( )->SendAction( pAction );
}


// ****************************************************************************
//
//  Function Name:	RView::OnUpdateRotate( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnUpdateRotate( RCommandUI& commandUI ) const
	{
	// Enable the menu item if there are any rotatable components selected
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		if( ( *iterator )->GetComponentAttributes( ).IsRotatable( ) )
			{
			commandUI.Enable( );
			return;
			}

	commandUI.Disable( );
	}
	
// ****************************************************************************
//
//  Function Name:	RView::OnLock( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnLock( )
	{
	BOOLEAN fLockedObjectFound;
	BOOLEAN fUnlockedObjectFound;

	GetLockState( fLockedObjectFound, fUnlockedObjectFound );

	// If there are any locked objects, do an unlock
	if( fLockedObjectFound )
		{
		RUnlockSelectionAction* pAction = new RUnlockSelectionAction( GetSelection( ) );
		GetRDocument( )->SendAction( pAction );
		}

	// Otherwise do an unlock
	else if( fUnlockedObjectFound )
		{
		RLockSelectionAction* pAction = new RLockSelectionAction( GetSelection( ) );
		GetRDocument( )->SendAction( pAction );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateLock( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateLock( RCommandUI& commandUI ) const
	{
	// If there is nothing selected, disable the menu
	if( GetSelection( )->Count( ) == 0 )
		commandUI.Disable( );

	else
		{
		// Figure out if we are locking or unlocking
		BOOLEAN fLockedObjectFound;
		BOOLEAN fUnlockedObjectFound;

		GetLockState( fLockedObjectFound, fUnlockedObjectFound );

		// Figure out which string to set the menu to
		YResourceId menuStringResourceId;

		// Unlocking
		if( fLockedObjectFound )
			{
			if( GetSelection( )->Count( ) == 1 )
				menuStringResourceId = STRING_MENU_UNLOCK;
			else
				menuStringResourceId = STRING_MENU_UNLOCKALL;
			}

		// Locking
		else if( fUnlockedObjectFound )
			{
			if( GetSelection( )->Count( ) == 1 )
				menuStringResourceId = STRING_MENU_LOCK;
			else
				menuStringResourceId = STRING_MENU_LOCKALL;
			}

		// No objects found which can have their lock state manipulated. Disable the menu and return
		else
			{
			commandUI.Disable( );
			return;
			}

		commandUI.Enable( );
		commandUI.SetString( GetResourceManager( ).GetResourceString( menuStringResourceId ) );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::GetLockState( )
//
//  Description:		Gets information on whether there are locked or unlocked
//							objects in the selection. Returns immediatly upon finding
//							a locked object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::GetLockState( BOOLEAN& fLockedObjectFound, BOOLEAN& fUnlockedObjectFound ) const
	{
	fLockedObjectFound = fUnlockedObjectFound = FALSE;

	// Look for object that can be locked or unlocked
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		{
		const RComponentAttributes& componentAttributes = ( *iterator )->GetComponentAttributes( );
		if( componentAttributes.CanChangeLockState( ) )
			{
			if( componentAttributes.IsLocked( ) )
				{
				// Unlocking has priority, so we can just break
				fLockedObjectFound = TRUE;
				break;
				}

			else
				{
				// Unlocking has priority, so we must keep looking
				fUnlockedObjectFound = TRUE;
				}
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnInsertNonstandard( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnInsertNonstandard( YCommandID commandId )
	{
	// REVIEW - STA - Ask the component application what default size to use
	RRealSize defaultSize( ::InchesToLogicalUnits( 2 ), ::InchesToLogicalUnits( 2 ) );

	// Get the bounding rect of the new component
	RRealRect boundingRect( GetInsertionPoint( defaultSize ), defaultSize );
	boundingRect.Offset( RRealSize( -defaultSize.m_dx / 2, -defaultSize.m_dy / 2 ) );

	// Create a new component document
	RComponentDocument* pComponentDocument = ::GetComponentManager( ).CreateNewComponent( GetComponentManager( ).GetNonstandardComponentType( commandId - COMMAND_MENU_INSERT_NONSTANDARDSTART ),
																													  GetRDocument( ),
																													  this,
																													  RComponentAttributes( ),
																													  boundingRect,
																													  FALSE );

	// Insert it
	RNewComponentAction* pAction = new RNewComponentAction( pComponentDocument, GetRDocument( ), GetSelection( ), STRING_UNDO_NEW_COMPONENT );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnComponentCommand( )
//
//  Description:		Command specified by the Command UI 
//
//  Returns:			TRUE if the command was Handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnComponentCommand( YCommandID commandId )
	{
	YInterfaceId					interfaceId				= kInvalidInterfaceId;
	RInterface*						pInterface				= NULL;
	RSettingsInterface*			pSettingsInterface	= NULL;

	try
		{
		RComponentView*		pView;
		RCompositeSelection::YIterator	selectionIterator		= GetSelection( )->Start( );
		RCompositeSelection::YIterator	selectionIteratorEnd	= GetSelection( )->End( );
		for( ; selectionIterator != selectionIteratorEnd; ++selectionIterator )
			{
			pView = (*selectionIterator);
			if ( pView->GetInterfaceId( commandId, interfaceId ) )
				{
				TpsAssert( interfaceId != kInvalidInterfaceId, "GetInterfaceId returned TRUE, but interfaceId was kInvalidInterfaceId" );
				pInterface = pView->GetInterface( interfaceId );
				TpsAssert( pInterface != NULL, "Component said it understood an interfaceId, but can not create the Interface" );
				break;
				}
			}

		//	If no interface, just return - noone can perform this action
		if ( pInterface == NULL )
			{
			TpsAssertAlways( "A Command was enabled, but noone can process it" );
			return;
			}

		RSettings*						pSettings;
		YComponentViewCollection	viewCollection;

		TpsAssertIsObject( RSettingsInterface, pInterface );
		pSettingsInterface	= static_cast<RSettingsInterface*>( pInterface );
		pInterface				= NULL;
  		//	Loop through the remaining selection and see if anyone else can handle Interface
		viewCollection.InsertAtEnd( pView );

		for ( ++selectionIterator; selectionIterator != selectionIteratorEnd; ++selectionIterator )
			{
			pView	= (*selectionIterator);
			if ( (pInterface = pView->GetInterface( interfaceId )) != NULL )
				{
				TpsAssertIsObject( RSettingsInterface, pInterface );
				RSettingsInterface*	pOtherInterface	= static_cast<RSettingsInterface*>( pInterface );

				//	Merge the data into the primary settings.
				pSettings	= pOtherInterface->GetData( );
				(pSettingsInterface->GetData())->MergeSetting( pSettings );
				//	Remember this view in the list.
				viewCollection.InsertAtEnd( pView );
				//
				//	Free up the interface
				delete pOtherInterface;
				pInterface = NULL;
				}
			}

		//	If the settings are altered, I must apply them for all of the preserved views
		if ( pSettingsInterface->AlterSettings( commandId ) )
			{
			GetRDocument( )->BeginMultiUndo( );
			try
				{
				YComponentViewIterator	viewIterator		= viewCollection.Start( );
				YComponentViewIterator	viewIteratorEnd	= viewCollection.End( );
				for ( ; viewIterator != viewIteratorEnd; ++viewIterator )
					pSettingsInterface->ApplySettings( (*viewIterator) );

				uWORD	uwUndoId;
				uWORD	uwRedoId;
				pSettingsInterface->GetUndoRedoStringIds( uwUndoId, uwRedoId );
				GetRDocument( )->EndMultiUndo( uwUndoId, uwRedoId );
				}
			catch (...)
				{
				GetRDocument( )->CancelMultiUndo( );
				throw;
				}
			}
		}
	catch( YException exception )
		{
		delete pInterface;
		::ReportException( exception );
		}

	//	Delete the main settings
	delete pSettingsInterface;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetMergedSettings( )
//
//  Description:		Return the settings for the given selection.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSettingsInterface* RView::GetMergedSettings( YCommandID commandId, RCompositeSelection* pSelection ) const
	{
	//	We have no interface if we are given no selection.
	if ( !pSelection )
		return NULL;

	//	Get the interface id we will search the given selection for.
	YInterfaceId							interfaceId				= kInvalidInterfaceId;	
	RComponentView*						pView						= NULL;
	RCompositeSelection::YIterator	selectionIterator		= GetSelection( )->Start( );
	RCompositeSelection::YIterator	selectionIteratorEnd	= GetSelection( )->End( );
	
	//	Look for the first selected item that can handle our command.
	while( selectionIterator != selectionIteratorEnd )
		{
		pView = (*selectionIterator);
		if ( pView->GetInterfaceId( commandId, interfaceId ) )
			{
			//	We found it. We're done.
			TpsAssert( interfaceId != kInvalidInterfaceId, "GetInterfaceId returned TRUE, but interfaceId was kInvalidInterfaceId" );
			break;
			}
		++selectionIterator;
		}

	//	Now return the merged settings if we have a valid interface id.
	if ( interfaceId != kInvalidInterfaceId )
		return GetMergedSettings( interfaceId, pSelection );
	else
		return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetMergedSettings( )
//
//  Description:		Return the settings for the given interface id from the given selection.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSettingsInterface* RView::GetMergedSettings( YInterfaceId interfaceId, RCompositeSelection* pSelection ) const
	{
	RInterface*						pInterface				= NULL;
	RSettingsInterface*			pSettingsInterface	= NULL;

	//	We have no interface if we are given no selection.
	if ( !pSelection )
		return NULL;

	try
		{
		RCompositeSelection::YIterator	selectionIterator		= pSelection->Start( );
		RCompositeSelection::YIterator	selectionIteratorEnd	= pSelection->End( );
		RComponentView*		pView;

		//	Find the first interface for our id.
		for ( ; (selectionIterator != selectionIteratorEnd) && !pInterface; ++selectionIterator )
			pInterface = (*selectionIterator)->GetInterface( interfaceId );

		//	If no interface, just return - no one can perform this action
		if ( pInterface != NULL )
			{
			RSettings*						pSettings;

			TpsAssertIsObject( RSettingsInterface, pInterface );
			pSettingsInterface	= static_cast<RSettingsInterface*>( pInterface );
			pInterface				= NULL;

	 		//	Loop through the remaining selection, Merge Settings
			for ( ; selectionIterator != selectionIteratorEnd; ++selectionIterator )
				{
				pView	= (*selectionIterator);
				if ( (pInterface = pView->GetInterface( interfaceId )) != NULL )
					{
					TpsAssertIsObject( RSettingsInterface, pInterface );
					RSettingsInterface*	pOtherInterface	= static_cast<RSettingsInterface*>( pInterface );

					//	Merge the data into the primary settings.
					pSettings	= pOtherInterface->GetData( );
					(pSettingsInterface->GetData())->MergeSetting( pSettings );
					//
					//	Free up the interface
					delete pOtherInterface;
					pInterface = NULL;
					}
				}
			}
 		}
	catch( ... )
		{
		delete pInterface;
		}

	return pSettingsInterface;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateComponentCommand( )
//
//  Description:		Command updater
//
//  Returns:			TRUE if the command was enabled/disabled
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateComponentCommand( RCommandUI& commandUI ) const
	{
	RSettingsInterface*			pSettingsInterface	= NULL;

	commandUI.SetCheck( RCommandUI::kNoCheck );
	commandUI.Disable();

	pSettingsInterface = GetMergedSettings( commandUI.GetCommandID( ), GetSelection( ) );

	if ( pSettingsInterface )
		{
			//	Let the interface set the commandUI
			pSettingsInterface->SettingsUI( commandUI );
		}	//	If pInterface != NULL

	delete pSettingsInterface;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnFlipHorizontal( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnFlipHorizontal( )
	{
	// Create and do a flip selection horizontal action
	RFlipSelectionHorizontalAction* pAction = new RFlipSelectionHorizontalAction( GetSelection( ) );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnFlipVertical( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnFlipVertical( )
	{
	// Create and do a flip selection vertical action
	RFlipSelectionVerticalAction* pAction = new RFlipSelectionVerticalAction( GetSelection( ) );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnFlipBoth( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnFlipBoth( )
	{
	// Create and do a flip selection both action
	RFlipSelectionBothAction* pAction = new RFlipSelectionBothAction( GetSelection( ) );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateFlip( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateFlip( RCommandUI& commandUI ) const
	{
	// Enable the menu item if there are any flippable components selected
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		if( ( *iterator )->GetComponentAttributes( ).IsFlippable( ) )
			{
			commandUI.Enable( );
			return;
			}

	commandUI.Disable( );
	}


// ****************************************************************************
//
//  Function Name:	RView::GetShadowState( BOOLEAN& fUnShadowedComponentFound )
//
//  Description:		If a component which is unshadowed is found in the current
//							selection, sets fUnShadowedComponentFound to TRUE.
//
//  Returns:			TRUE if a shadowable component was found
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::GetShadowState( BOOLEAN& fUnShadowedComponentFound ) const
{
	BOOLEAN fShadowableComponentFound = FALSE;
	fUnShadowedComponentFound = FALSE;

	// if *some* of the selected components are NOT shadowed then the menu
	// item should say "Shadow On"
	// if ALL of the componenets are shadowed then the menu says "Shadow Off"
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
	{
		if( ( *iterator )->GetComponentAttributes( ).IsShadowable( ) )
		{
// REVIEW GCB 3/30/98 - spec allows banner headlines (which are locked) to be shadowed
// and there doesn't seem to be any reason to disallow for other locked objects
//			if( !(*iterator)->GetComponentAttributes( ).IsLocked( ) )
			{
				fShadowableComponentFound = TRUE;
				if( (*iterator)->HasShadow() == FALSE )
					fUnShadowedComponentFound = TRUE;
			}
		}
	}

	if ( fShadowableComponentFound )
		return TRUE;

	return FALSE;
}


// ****************************************************************************
//
//  Function Name:	RView::OnShadow( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnShadow( )
	{
	BOOLEAN fUnShadowedComponentFound = FALSE;
	BOOLEAN fShadowableComponentFound = FALSE;

	fShadowableComponentFound = GetShadowState( fUnShadowedComponentFound );

	// should not have got here if none of the components can be shadowed
	ASSERT( fShadowableComponentFound != FALSE );

	// Create and do a shadow action
	RSoftShadowSettings shadowSettings( fUnShadowedComponentFound );
	RComponentShadowSelectionAction* pAction = new RComponentShadowSelectionAction( GetSelection( ), shadowSettings );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateShadow( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateShadow( RCommandUI& commandUI ) const
	{
	BOOLEAN fShadowableComponentFound = FALSE;
	BOOLEAN fUnShadowedComponentFound = FALSE;

	// if *some* of the selected components are NOT shadowed then the menu
	// item should say "Shadow On"
	// if ALL of the componenets are shadowed then the menu says "Shadow Off"

	fShadowableComponentFound = GetShadowState( fUnShadowedComponentFound );

	if ( fShadowableComponentFound )
	{
		commandUI.Enable( );
		RMBCString szMenuText;

		if ( fUnShadowedComponentFound == FALSE )
		{
			szMenuText = GetResourceManager().GetResourceString( YResourceId( STRING_MENU_SHADOW_OFF ) );
			commandUI.SetString( szMenuText );
		}
		else
		{
			szMenuText = GetResourceManager().GetResourceString( YResourceId( STRING_MENU_SHADOW_ON ) );
			commandUI.SetString( szMenuText );
		}
	}
	else
		commandUI.Disable( );
}

// ****************************************************************************
//
//  Function Name:	RView::GetGlowState( BOOLEAN& fUnGlowedComponentFound )
//
//  Description:		If a component which is unglowed is found in the current
//							selection, sets fUnGlowedComponentFound to TRUE.
//
//  Returns:			TRUE if a glowable component was found
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::GetGlowState( BOOLEAN& fUnGlowedComponentFound ) const
{
	BOOLEAN fGlowableComponentFound = FALSE;
	fUnGlowedComponentFound = FALSE;

	// if *some* of the selected components are NOT glowed then the menu
	// item should say "Glow On"
	// if ALL of the componenets are glowed then the menu says "Glow Off"
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
	{
		if( ( *iterator )->GetComponentAttributes( ).IsGlowable( ) )
		{
// REVIEW GCB 3/30/98 - spec allows banner headlines (which are locked) to be glowed
// and there doesn't seem to be any reason to disallow for other locked objects
//			if( !(*iterator)->GetComponentAttributes( ).IsLocked( ) )
			{
				fGlowableComponentFound = TRUE;
				if( (*iterator)->HasGlow() == FALSE )
					fUnGlowedComponentFound = TRUE;
			}
		}
	}

	if ( fGlowableComponentFound )
		return TRUE;

	return FALSE;
}


// ****************************************************************************
//
//  Function Name:	RView::OnGlowOnOff( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnGlowOnOff( )
	{
	BOOLEAN fUnGlowedComponentFound = FALSE;
	BOOLEAN fGlowableComponentFound = FALSE;

	fGlowableComponentFound = GetGlowState( fUnGlowedComponentFound );

	// should not have got here if none of the components can be glowed
	ASSERT( fGlowableComponentFound != FALSE );

	// Create and do a glow action
	RSoftGlowSettings glowSettings( fUnGlowedComponentFound );
	RComponentGlowSelectionAction* pAction = new RComponentGlowSelectionAction( GetSelection( ), glowSettings );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateGlowOnOff( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateGlowOnOff( RCommandUI& commandUI ) const
	{
	BOOLEAN fGlowableComponentFound = FALSE;
	BOOLEAN fUnGlowedComponentFound = FALSE;

	// if *some* of the selected components are NOT glowed then the menu
	// item should say "Glow On"
	// if ALL of the componenets are glowed then the menu says "Glow Off"

	fGlowableComponentFound = GetGlowState( fUnGlowedComponentFound );

	if ( fGlowableComponentFound )
	{
		commandUI.Enable( );
		RMBCString szMenuText;

		if ( fUnGlowedComponentFound == FALSE )
		{
			szMenuText = GetResourceManager().GetResourceString( YResourceId( STRING_MENU_GLOW_OFF ) );
			commandUI.SetString( szMenuText );
		}
		else
		{
			szMenuText = GetResourceManager().GetResourceString( YResourceId( STRING_MENU_GLOW_ON ) );
			commandUI.SetString( szMenuText );
		}
	}
	else
		commandUI.Disable( );
}

// ****************************************************************************
//
//  Function Name:	RView::GetVignetteState( BOOLEAN& fUnVignettedComponentFound )
//
//  Description:		If a component which is unvignetted is found in the current
//							selection, sets fUnVignettedComponentFound to TRUE.
//
//  Returns:			TRUE if a vignetteable component was found
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::GetVignetteState( BOOLEAN& fUnVignettedComponentFound ) const
{
	BOOLEAN fVignetteableComponentFound = FALSE;
	fUnVignettedComponentFound = FALSE;

	// if *some* of the selected components are NOT vignetted then the menu
	// item should say "Vignette On"
	// if ALL of the componenets are vignetted then the menu says "Vignette Off"
	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
	{
		if( ( *iterator )->GetComponentAttributes( ).IsVignetteable( ) )
		{
			if( !(*iterator)->GetComponentAttributes( ).IsLocked( ) )
			{
				fVignetteableComponentFound = TRUE;
				if( (*iterator)->HasVignette() == FALSE )
					fUnVignettedComponentFound = TRUE;
			}
		}
	}

	if ( fVignetteableComponentFound )
		return TRUE;

	return FALSE;
}


// ****************************************************************************
//
//  Function Name:	RView::OnVignetteOnOff( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnVignetteOnOff( )
	{
	BOOLEAN fUnVignettedComponentFound = FALSE;
	BOOLEAN fVignetteableComponentFound = FALSE;

	fVignetteableComponentFound = GetVignetteState( fUnVignettedComponentFound );

	// should not have got here if none of the components can be vignetted
	ASSERT( fVignetteableComponentFound != FALSE );

	// Create and do a vignette action
	RSoftVignetteSettings vignetteSettings( fUnVignettedComponentFound );
	RComponentVignetteSelectionAction* pAction = new RComponentVignetteSelectionAction( GetSelection( ), vignetteSettings );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateVignetteOnOff( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateVignetteOnOff( RCommandUI& commandUI ) const
	{
	BOOLEAN fVignetteableComponentFound = FALSE;
	BOOLEAN fUnVignettedComponentFound = FALSE;

	// if *some* of the selected components are NOT vignetted then the menu
	// item should say "Vignette On"
	// if ALL of the componenets are vignetted then the menu says "Vignette Off"

	fVignetteableComponentFound = GetVignetteState( fUnVignettedComponentFound );

	if ( fVignetteableComponentFound )
	{
		commandUI.Enable( );
		RMBCString szMenuText;

		if ( fUnVignettedComponentFound == FALSE )
		{
			szMenuText = GetResourceManager().GetResourceString( YResourceId( STRING_MENU_VIGNETTE_OFF ) );
			commandUI.SetString( szMenuText );
		}
		else
		{
			szMenuText = GetResourceManager().GetResourceString( YResourceId( STRING_MENU_VIGNETTE_ON ) );
			commandUI.SetString( szMenuText );
		}
	}
	else
		commandUI.Disable( );
}

// ****************************************************************************
//
//  Function Name:	RView::OnBringForward( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnBringForward( )
	{
	// Create and send a bring selection forward action
	RBringSelectionForwardAction* pAction = new RBringSelectionForwardAction( GetSelection( ) );

	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateBringForward( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateBringForward( RCommandUI& commandUI ) const
	{
	// Get the components that can be moved forward
	YComponentViewCollection collection;
	GetComponentsToBringForward( collection );

	// Enable if there is anything in the collection
	if( collection.Count( ) )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnSendBackward( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnSendBackward( )
	{
	// Create and send a send selection backward action
	RSendSelectionBackwardAction* pAction = new RSendSelectionBackwardAction( GetSelection( ) );

	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateSendBackward( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateSendBackward( RCommandUI& commandUI ) const
	{
	// Get the components that can be send backward
	YComponentViewCollection collection;
	GetComponentsToSendBackward( collection );

	// Enable if there is anything in the collection
	if( collection.Count( ) )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnBringToFront( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnBringToFront( )
	{
	// Create and send a bring selection to front action
	RBringSelectionToFrontAction* pAction = new RBringSelectionToFrontAction( GetSelection( ) );

	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateBringToFront( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateBringToFront( RCommandUI& commandUI ) const
	{
	// Update the same way as bring forward
	OnUpdateBringForward( commandUI );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnSendToBack( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnSendToBack( )
	{
	// Create and send a send selection to back action
	RSendSelectionToBackAction* pAction = new RSendSelectionToBackAction( GetSelection( ) );

	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateSendToBack( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateSendToBack( RCommandUI& commandUI ) const
	{
	// Update the same way as send backward
	OnUpdateSendBackward( commandUI );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnFrame( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnFrame( YCommandID commandId )
	{
	EFrameType	eFrame;
	EFrameType	eFrameType		= kNoFrame;
	EFrameType	eOldFrameType	= kNoFrame;
	RSolidColor	rFrameColor		= RSolidColor( kBlack );
	RSolidColor	rOldFrameColor	= rFrameColor;
	BOOLEAN fMultipleFrameTypes = FALSE;
	BOOLEAN fMultipleFrameColors = FALSE;

	//
	// Figure out what frame color you start with
	RCompositeSelection::YIterator iterator = GetSelection( )->Start( );
	for ( int i = 0; iterator != GetSelection( )->End( ); ++iterator, ++i )
		{
		rFrameColor	= (*iterator)->GetFrameColor( );
		if ( rFrameColor != rOldFrameColor )
			{
			if ( i != 0 )
				{
				rFrameColor	= RSolidColor( kBlack );
				fMultipleFrameColors = TRUE;
				break;
				}
			else
				rOldFrameColor = rFrameColor;
			}
		}
		//
		// Save the frame color
		rOldFrameColor = rFrameColor;
	//
	//	Figure out what kind of frame to set to the selected components.
	//	If  we are asked to get the frame from the frames dialog then do so
	//	else translate the command id into a frame enum.
	if( commandId == COMMAND_MENU_EFFECTS_FRAME_DIALOG )
		{
		RFramesDialog	rFramesDlg;
	
		//
		// Get the frame type
		iterator = GetSelection( )->Start( );
		for( i = 0; iterator != GetSelection( )->End( ); ++iterator, ++i )
			{
			eFrameType = (*iterator)->GetFrameType();

			if( eFrameType != eOldFrameType )
				{
				if ( i != 0 )
					{
					// You have a selection with more than one frame type
					eFrameType	= kNoFrame;
					fMultipleFrameTypes = TRUE;
					break;
					}
				else
					eOldFrameType	= eFrameType;
				}
			}
		//
		// Save the frame type you had walking into the dialog
		eOldFrameType = eFrameType;
		//
		// Initialize the dialog
		rFramesDlg.SetMultipleFrameTypes( fMultipleFrameTypes );
		rFramesDlg.SetFrameType( eFrameType );
		rFramesDlg.SetFrameColor( rFrameColor );
		//
		// Bring up the dialog
		if( rFramesDlg.DoModal() != IDOK )
			return;	//	Cancel chosen, do nothing
		eFrame = rFramesDlg.GetFrameType();
		rFrameColor = rFramesDlg.GetFrameColor();

		// if we have a multiple selection, and the user does not
		// explicitly select a frame, they just hit OK, do not blow
		// away the differing frames on their selected objects.
		if ( eFrame == kNoFrame && rFramesDlg.GetMultipleFrameTypes() != FALSE )
			return;
		}
	else
		{
		eFrame = RFrameTypeMapEntry::GetFrameType( commandId );
		}
	
	try
		{
		// Setup a multi-undo action
		GetRDocument( )->BeginMultiUndo( );

		// If the frame type was changed, do a change frame action
		if ( eOldFrameType != eFrame || fMultipleFrameTypes )
			{
			RFrameSelectionAction* pAction = new RFrameSelectionAction( GetSelection( ), eFrame );
			GetRDocument( )->SendAction( pAction );
			}
		
		// If the frame color was changed, do a change frame color action
		if ( rOldFrameColor != rFrameColor || fMultipleFrameColors )
			{
			RFrameColorSelectionAction* pColorAction = new RFrameColorSelectionAction( GetSelection( ), rFrameColor );
			GetRDocument( )->SendAction( pColorAction );
			}

		// End the multi-undo
		GetRDocument( )->EndMultiUndo( STRING_UNDO_CHANGE_FRAME, STRING_REDO_CHANGE_FRAME );
		}

	catch( ... )
		{
		GetRDocument( )->CancelMultiUndo( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateFrame( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateFrame( RCommandUI& commandUI ) const
	{
	//	Look for a frameable component in the current selection.
	BOOLEAN fFrameEnabled = FALSE;
	BOOLEAN fMultipleFrameTypesSelected = FALSE;
	EFrameType eSelectedFrameType = kNumFrameTypes;
	YSelectionIterator iterator = GetSelection( )->Start( );
	
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		{
		if( ( *iterator )->GetComponentDocument( )->IsFrameable( ) )
			{
			//	We found a fremeable component so allow the dialog to be presented.
			commandUI.Enable( );
			return;
			}
		}
	
	//	There are no frameable components so don't allow the frames dialog to be presented
	commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnColorBehindObject( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnColorBehindObject( )
	{
	RColor	newColor;
	BOOLEAN	fColorChanged	= FALSE;

#ifdef _WINDOWS
	// Create the color dialog
	RWinColorDlg dialog( NULL, kShowTransparent | kShowGradients | kAllowGradients );
	// Set the current color
	dialog.SetColor( ( *GetSelection( )->Start( ) )->GetBackgroundColor( ) );
	// Do the dialog
	if( dialog.DoModal( ) == IDOK )
		{
		fColorChanged	= TRUE;
		// Get the selected color
		newColor = dialog.SelectedColor( );
		}
#endif	// _WINDOWS

	if ( fColorChanged )
		{
		// Create and do a change background color action
		RChangeSelectionBackgroundColorAction* pAction = new RChangeSelectionBackgroundColorAction( GetSelection( ), newColor );
		GetRDocument( )->SendAction( pAction );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateColorBehindObject( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateColorBehindObject( RCommandUI& commandUI ) const
	{
	if( GetSelection( )->Count( ) )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}


// ****************************************************************************
//
//  Function Name:	RView::OnEditSelected( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnEditSelected( ) 
	{
	TpsAssert( GetSelection( )->Count( ) == 1, "EditSelection called with selection count != 1" );

	RComponentView*		pView			= *(GetSelection( )->Start());
	TpsAssert( pView->GetComponentAttributes( ).IsEditable( ), "Editing a component that is not editable" );
	CallEditComponent( pView, kMenu, RRealPoint( -1, -1 ) );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnSetUseOffscreens( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnSetUseOffscreens( )
	{
	::GetUseOffscreens( ) = !::GetUseOffscreens( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnUpdateSetUseOffscreens( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateSetUseOffscreens( RCommandUI& commandUI ) const
	{
	commandUI.Enable( );
	commandUI.SetCheck( (::GetUseOffscreens())? RCommandUI::kCheckMark : RCommandUI::kNoCheck );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnSetUseOffscreens( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnCenterInView( )
	{
	RAlignSelectionAction* pAction = new RAlignSelectionAction( GetSelection( ), RAlignSelectionAction::kAlignCenterHorz, RAlignSelectionAction::kAlignCenterVert, RAlignSelectionAction::kToParentView );
	GetRDocument( )->SendAction( pAction );
	}
// ****************************************************************************
//	Function Name:	RView::OnAlignToViewLeft( )
//
//	Description:	Command Handler
//
//	Returns:		Nothing
//
// ****************************************************************************
//
void RView::OnAlignToViewLeft( )
	{
	RAlignSelectionAction* pAction = new RAlignSelectionAction( GetSelection( ), RAlignSelectionAction::kAlignLeft, RAlignSelectionAction::kAlignNoneVert, RAlignSelectionAction::kToParentView );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//	Function Name:	RView::OnAlignToViewRight( )
//
//	Description:	Command Handler
//
//	Returns:		Nothing
//
// ****************************************************************************
//
void RView::OnAlignToViewRight( )
	{
	RAlignSelectionAction* pAction = new RAlignSelectionAction( GetSelection( ), RAlignSelectionAction::kAlignRight, RAlignSelectionAction::kAlignNoneVert, RAlignSelectionAction::kToParentView );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//	Function Name:	RView::OnAlignToViewTop( )
//
//	Description:	Command Handler
//
//	Returns:		Nothing
//
// ****************************************************************************
//
void RView::OnAlignToViewTop( )
	{
	RAlignSelectionAction* pAction = new RAlignSelectionAction( GetSelection( ), RAlignSelectionAction::kAlignNoneHorz, RAlignSelectionAction::kAlignTop, RAlignSelectionAction::kToParentView );
	GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//	Function Name:	RView::OnAlignToViewBottom( )
//
//	Description:	Command Handler
//
//	Returns:		Nothing
//
// ****************************************************************************
//
void RView::OnAlignToViewBottom( )
	{
	RAlignSelectionAction* pAction = new RAlignSelectionAction( GetSelection( ), RAlignSelectionAction::kAlignNoneHorz, RAlignSelectionAction::kAlignBottom, RAlignSelectionAction::kToParentView );
	GetRDocument( )->SendAction( pAction );
	}







// ****************************************************************************
//
//  Function Name:	RView::OnUpdateSetUseOffscreens( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateCenterInView( RCommandUI& commandUI ) const
	{
	// Only enable the command if there are more than one unlocked items in the selection.
	YCounter	i = 0;

	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		{
		const RComponentAttributes& componentAttributes = ( *iterator )->GetComponentAttributes( );
		if( componentAttributes.IsMovable( ) )
			++i;
 		}

	if (i == 1)
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

// ****************************************************************************
//	Function Name:	RView::OnScaleToView
//
//	Description:	Command Handler
//
//	Returns:		Nothing
//
//	History:		Modified to really do scaling, Mike Taber - 7 Jan 1997
// ****************************************************************************

void RView::OnScaleToView( )
	{
	RScaleSelectionToParentAction* pAction = new RScaleSelectionToParentAction( GetSelection( ), TRUE );
	GetRDocument( )->SendAction( pAction );
	// Scaling does not center the selection so...
	// Call OnCenterInView to center the object on the panel
	// OnCenterInView(); Removedthe centering... UNDO is the reason
	}

// ****************************************************************************
//	Function Name:	RView::OnSizeToView
//
//	Description:	Command Handler
//
//	Returns:		Nothing
//
//	Author: Mike Taber - 7 Jan 1997
// ****************************************************************************
void RView::OnSizeToView( )
	{
	RScaleSelectionToParentAction* pAction = new RScaleSelectionToParentAction( GetSelection( ), FALSE );
	GetRDocument( )->SendAction( pAction );
	// No need to center the object here, since it is sized to the panel
	// Objects Bounding Rect is same size & location as panels
	}


// ****************************************************************************
//
//  Function Name:	RView::OnUpdateSetUseOffscreens( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnUpdateScaleToView( RCommandUI& commandUI ) const
	{
	// Only enable the command if there are more than one unlocked items in the selection.
	YCounter	i = 0;

	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		{
		const RComponentAttributes& componentAttributes = ( *iterator )->GetComponentAttributes( );
		//	Can't scale the Line Components
		RComponentDocument*		pComponent	= (static_cast<RComponentView*>( *iterator ))->GetComponentDocument();
		const YComponentType&	compType	= pComponent->GetComponentType( );
		if( componentAttributes.IsMovable( ) && componentAttributes.IsResizable( ) &&
			 compType != kHorizontalLineComponent && compType != kVerticalLineComponent )
			++i;
 		}

	if (i == 1)
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

