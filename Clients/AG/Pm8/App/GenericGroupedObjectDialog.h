//****************************************************************************
//
// File Name: RGenericGroupedObjectDialog.h
//
// Project:			Renaissance
//
// Author:			Mike Heydlauf
//
// Description:	Definition of RGenericGroupedObjectDialog and 
//						RCommonGroupedObjectDialog Base class for all grouped object
//						dialogs.
//
// Portability:	Windows Specific
//
// Developed by:  Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/GenericGroupedObjectDialog.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _GENERICGROUPEDOBJECTDIALOG_H_
#define _GENERICGROUPEDOBJECTDIALOG_H_

//#include "RenaissanceResource.h"
#include "Resource.h"
#include "ControlContainer.h"
#include "SelectionTracker.h"
#include "CollectionManager.h"
#include "ComponentAttributes.h"

const UINT	kInvalidTimerID =				0;
const UINT	kTimerID =						1;
// Amount of time to elapse after the last keystroke and before the preview 
// is updated (in milliseconds)
const UINT kRenderTimeout = 600;

class RComponentAttributes;
class IComponentInfo;
class RUIContextData;


///****************************************************************************
//
//  Class Name:		RGenericGroupedObjectDialog
//
//  Description:		Base class for grouped object dialogs
//
// ****************************************************************************
//
class RGenericGroupedObjectDialog : public CDialog
{
// Construction
public:
	RGenericGroupedObjectDialog( IComponentInfo* pCompInfo,
										  RComponentDocument* pEditedComponent,
										  int nResourceId = RGenericGroupedObjectDialog::IDD );
	// Force this class to be abstract...
	virtual ~RGenericGroupedObjectDialog() = 0;
// Dialog Data
	//{{AFX_DATA(RGenericGroupedObjectDialog)
	enum { IDD = IDD_GENERIC_DIALOG_TEMPLATE };
	CStatic	m_cPreviewCtrl;
	CListBox	m_cGraphicListBox;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RGenericGroupedObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Operations
public :
	RComponentDocument*					GetComponent( );

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RGenericGroupedObjectDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
protected :	
	RComponentView*						AddComponent( const YComponentType& componentType, const YComponentBoundingRect& boundingRect, const RComponentAttributes& componentAttributes );
	void										RemoveComponent( RComponentDocument* pComponentDocument );	
	void										NewComponent( RComponentDocument* pNewDocument, YComponentZPosition yZPosition = 0 );
	void										ReplaceComponent( RComponentDocument* pOldDocument, RComponentDocument* pNewDocument);
	virtual const YComponentType&		GetComponentType( ) const = 0;
	virtual void							WriteUIContextData( RUIContextData& contextData ) const = 0;
	virtual RUIContextData*				ReadUIContextData() = 0;

private:
	void										CreateControlDocument( );

// Members
protected :
	RControlDocument*						m_pControlDocument;
	RControlView*							m_pControlView;
	IComponentInfo*						m_pCompInfo ;
	RComponentDocument*					m_pGroupComponent;
	RComponentDocument*					m_pEditedComponent;
};

// ****************************************************************************
//
//  Class Name:		RGroupDialogControlDocument
//
//  Description:		Control document class for the grouped object dialogs
//
// ****************************************************************************
//
class RGroupDialogControlDocument : public RControlDocument 
	{
	// Construction
	public :
													RGroupDialogControlDocument( IComponentInfo* pCompInfo );
	// Operations
	public :
		virtual RRealSize						GetMinObjectSize( const YComponentType&, BOOLEAN fMaintainAspect = FALSE );
		virtual RRealSize						GetMaxObjectSize( const YComponentType& );
		virtual RRealSize						GetDefaultObjectSize( const YComponentType& );
		virtual const RFontSizeInfo*		GetFontSizeInfo( ) const;
		virtual RControlView*				CreateView( CDialog* pDialog, int nControlId );

	// Members
	public :
		IComponentInfo*						m_pCompInfo ;
	};

// ****************************************************************************
//
//  Class Name:		RGroupDialogControlView
//
//  Description:		Control view class for the grouped object dialogs
//
// ****************************************************************************
//
class RGroupDialogControlView : public RControlView
	{
	// Construction
	public :
													RGroupDialogControlView( CDialog* pDialog, int nControlId, RControlDocument* pDocument );

	// Operations
	public :
		virtual RSelectionTracker*			GetResizeTracker( ) const;

		virtual void 								OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void								OnXRButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys );

		// Implementation
	protected :
		virtual void							DoDragDrop( const RRealPoint& mousePoint, BOOLEAN fUseTargetDefaultSize, YDropEffect );
		virtual YDropEffect					OnXDragEnter( RDataTransferSource& dataSource, YModifierKey modifierKeys, const RRealPoint& point );
		virtual RComponentView*				EditComponent( RComponentView* pComponent,
																		EActivationMethod activationMethod,
																		const RRealPoint& mousePoint,
																		YResourceId& editUndoRedoPairId,
																		uLONG ulEditInfo );

	//	Members
	private :
		CDialog*									m_pDialog;
	};

// ****************************************************************************
//
//  Class Name:	RGroupDialogDragSelectionTracker
//
//  Description:	This class is used to encapsulate a drag selection operation,
//						constrained to the limits of the control view.
//
// ****************************************************************************
//
class RGroupDialogDragSelectionTracker : public RDragSelectionTracker
	{
	// Construction
	public :
													RGroupDialogDragSelectionTracker( RCompositeSelection* pSelection,
																								 const RRealRect& contraintRect );
													
	// Implementation							
	protected :									
		virtual void							ApplyConstraint( RRealPoint& mousePoint ) const;
													
	// Member data								
	private :									
		RRealSize								m_TopLeftOffset;
		RRealSize								m_BottomRightOffset;
	};

// ****************************************************************************
//
//  Class Name:	RGroupDialogResizeSelectionTracker
//
//  Description:	This class is used to encapsulate a resize selection operation,
//						constrained to the limits of the control view.
//
// ****************************************************************************
//
class RGroupDialogResizeSelectionTracker : public RResizeSelectionTracker
	{
	// Construction
	public :
													RGroupDialogResizeSelectionTracker( RCompositeSelection* pSelection,
																									const RRealRect& contraintRect,
																									BOOLEAN fConstrainBoundingRect = FALSE );

	// Operations
	public :
		virtual void							BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
													
	// Implementation							
	protected :									
		virtual void							ApplyConstraint( RRealPoint& mousePoint ) const;
		virtual void							ConstrainScaleFactor( RRealSize& scaleFactor ) const;
													
	// Member data								
	private :									
		UINT										m_eHitLocation;
		BOOLEAN									m_fConstrainBoundingRect;
		RRealRect								m_ConstraintRect;
	};

#if 1
// ****************************************************************************
//
//  Class Name:	RGroupDialogResizeSelectionTracker
//
//  Description:	This class is used to encapsulate a resize selection operation,
//						constrained to the limits of the control view.
//
// ****************************************************************************
//
class RGroupDialogRotateSelectionTracker : public RRotateSelectionTracker
	{
	// Construction
	public :
										RGroupDialogRotateSelectionTracker( RCompositeSelection* pSelection,
																						const RRealRect& contraintRect );

	// Operations
	public :
		virtual void				BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );

	// Implementation							
	protected :									
		virtual YAngle				ConstrainAngle( YAngle angle ) const;
													
	// Member data								
	private :									
		RRealRect					m_ConstraintRect;
		BOOLEAN						m_fIsConstrained;
		YAngle						m_flStartAngle;
		YAngle						m_flAngle;
	};
#endif

#endif //_GENERICGROUPEDOBJECTDIALOG_H_
