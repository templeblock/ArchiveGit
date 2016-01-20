// ****************************************************************************
//
//  File Name:		TabbedEditCrop.h
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Declaration of the RTabbedEditCrop class
//
//	Portability:	Win32
//
//	Developed by:	Broderbund Software
//					500 Redwood Blvd
//					Novato, CA 94948
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

#ifndef _TABBEDEDITCROP_H_
#define _TABBEDEDITCROP_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "GraphicCompResource.h"
#include "PhotoWorkshopResource.h"

#include "EditCropControl.h"

#include "UndoableAction.h"
#include "Path.h"

class RTabbedEditImage;
class RPathDocument;
class RFlyout;

class RApplyClipPathAction : public RUndoableAction
{
// Construction & destruction
public :
											RApplyClipPathAction(	RComponentDocument* pComponent,
																			RClippingPath* pClipPath );

// Operations
public :
	virtual BOOLEAN					Do( );
	virtual void						Undo( );

// Scripting Operations
public :
	virtual BOOLEAN					WriteScript( RScript& script ) const;

//	Identifier
public :
	static YActionId					m_ActionId;

// Member data
protected :

	RComponentDocument*				m_pComponent;
	YComponentBoundingRect			m_rBoundingRect;
	RClippingPath						m_rPath;
	RClippingPath						m_rOldPath;

#ifdef	TPSDEBUG
//	Debugging code
public :
	virtual	void						Validate( ) const;
#endif
};

class RRotateImageAction : public RUndoableAction
{
// Construction & destruction
public :
											RRotateImageAction( RTabbedEditImage* pImageEdit, YAngle angle  );

// Operations
public :
	virtual BOOLEAN					Do( );
	virtual void						Undo( );

// Scripting Operations
public :
	virtual BOOLEAN					WriteScript( RScript& script ) const;

//	Identifier
public :
	static YActionId					m_ActionId;

// Member data
protected :

	BOOLEAN								DoRotation( YAngle angle );

	RTabbedEditImage*					m_pImageEdit;
	YComponentBoundingRect			m_rBoundingRect;
	YAngle								m_yAngle;

#ifdef	TPSDEBUG
//	Debugging code
public :
	virtual	void						Validate( ) const;
#endif
};

// ****************************************************************************
//
//  Class Name:		RTabbedEditCrop
//
//  Description:	Image Edit Cropping dialog
//
// ****************************************************************************
class RTabbedEditCrop : public CDialog
{
// Construction
public:
	RTabbedEditCrop(CWnd* pParent = NULL);   // standard constructor
	~RTabbedEditCrop();

	enum { IDD = IDD_EDIT_CROP_TAB };

	void	UpdateButtonControls();

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL	OnInitDialog();
//	virtual void	OnOK();
	virtual BOOL	PreTranslateMessage( MSG* pMsg );

	BOOL				ShowCropShape( UINT nID );
	void				RemovePreviousPath();
	void				UpdateParentsClipPath();


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTestDlg)
	afx_msg void OnShowWindow(  BOOL bShow, UINT nStatus );
	afx_msg void OnUndo();
	afx_msg void OnRotate();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnCutoutShapes();
	afx_msg void OnCutoutFreeHand();
	afx_msg void OnCutoutShape( UINT nID );
	afx_msg void OnDestroy( );
	afx_msg BOOLEAN OnApplyCropShape();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg LONG OnScratchImageChange( WPARAM, LPARAM );
	afx_msg LONG OnApplyEditsToIEDBitmap( WPARAM, LPARAM );
	afx_msg LONG OnCleanupForReset( WPARAM, LPARAM );
	afx_msg LONG OnResetUpdate( WPARAM, LPARAM );
	afx_msg LONG OnOKFromIED( WPARAM, LPARAM );

	afx_msg LONG OnUndoFromIED( WPARAM, LPARAM ) { OnUndo(); return 1L; }
	afx_msg LONG OnZoomInFromIED( WPARAM, LPARAM ) { OnZoomIn(); return 1L; }
	afx_msg LONG OnZoomOutFromIED( WPARAM, LPARAM ) { OnZoomOut(); return 1L; }

private:	
	
	void InitializeDocument();
	void RemoveUnappliedCropPath();

// Data members
	RTabbedEditImage*		m_pParentDlg;

	RComponentDocument*		   m_pComponentDocument;
	REditCropDocument*			m_pControlDocument;
	REditCropView*					m_pControlView;

	RCompositeSelection*			m_pSelection;
	YComponentBoundingRect		m_rBoundingRect;
	RRealRect						m_rPreviewArea;

	RFlyout*				m_pFlyOut;
	UINT					m_nCutoutSel;

	CBitmapButton		m_cButtonShapes;
	CBitmapButton		m_cButtonFreehand;
	CBitmapButton		m_cButtonCutIt;
	CBitmapButton		m_cButtonRemoveCrop;
	CBitmapButton		m_cButtonZoomIn;
	CBitmapButton		m_cButtonZoomOut;
	CBitmapButton		m_cButtonUndo;
	CBitmapButton		m_cButtonRotate;
};


#endif 