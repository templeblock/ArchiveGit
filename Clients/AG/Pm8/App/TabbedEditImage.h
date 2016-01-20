// ****************************************************************************
//
//  File Name:		TabbedEditImage.h
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Declaration of the RTabbedEditImage class
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

#ifndef _TABBEDEDITIMAGE_H_
#define _TABBEDEDITIMAGE_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GraphicCompResource.h"
#include "TabbedEditCrop.h"
#include "TabbedEditAdjustCol.h"
#include "TabbedEditBrightFocus.h"
#include "TabbedEditFixFlaw.h"
#include "TabbedEditArtEffect.h"

#include "ComponentDocument.h"
#include "ImageInterface.h"
#include "UndoableAction.h"

#include "ComponentDocument.h"
#include "UndoManager.h"
#include "FrameworkResourceIDs.h"

#define WM_USER_APPLY_EDITS_TO_IED_BITMAP	( WM_USER + 201 )
#define WM_USER_CLEANUP_IED_TAB_FOR_RESET	( WM_USER + 202 )
#define WM_USER_RESET_IED_TAB					( WM_USER + 203 )
#define WM_USER_SCRATCH_IMAGE_CHANGED		( WM_USER + 204 )
#define WM_USER_ON_OK_FROM_IED				( WM_USER + 205 )
#define WM_USER_UNDO_FROM_IED					( WM_USER + 206 )
#define WM_USER_ZOOMIN_FROM_IED				( WM_USER + 207 )
#define WM_USER_ZOOMOUT_FROM_IED				( WM_USER + 208 )

// the tabbed sections of the Image Edit dialog box
//enum EEditImageTab
enum
{
	kCropOrientTab = 0,
	kAdjustColorTab,
	kLightFocusTab,
	kFixFlawTab,
	kArtEffectsTab
};

class RModifyImageAction : public RUndoableAction
{
// Construction & destruction
public :
											RModifyImageAction(	RComponentDocument* pComponent,
																		RIntRect* pRect = NULL,
																		RBitmapImage* pImage = NULL );

// Operations
public :
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
	RBitmapImage*						m_pFullImage;
	RIntPoint							m_ptImageOffset;
	RBitmapImage						m_biImage;

	BOOLEAN								m_fReplaceWhole;

#ifdef	TPSDEBUG
//	Debugging code
public :
	virtual	void						Validate( ) const;
#endif
};

// ****************************************************************************
//
//  Class Name:		RTabbedEditImage
//
//  Description:	Image Edit dialog
//
// ****************************************************************************
class RTabbedEditImage : public CDialog
{
// Construction
public:
	RTabbedEditImage(RComponentDocument* pDesignDeskComponent, int selTab = kCropOrientTab, RMBCString *pSaveAsDir = NULL, CWnd* pParent = NULL);
	~RTabbedEditImage();

	enum { IDD = IDD_EDITIMAGE_TABBED };

	RComponentDocument*	CreateComponentWithEffects();
	RComponentDocument*	GetComponent() { return m_pLocalComponent; }

	RComponentDocument*	GetFullComponentCopy();
	RComponentDocument*	GetPreviewCopyFromIEDComponent(RImagePreviewCtrl *pCtrl);
	RComponentDocument*	GetPreviewComponentCopy(RComponentDocument *pComponent, RImagePreviewCtrl *pCtrl);
	RComponentDocument* 	GetComponentCopy( RComponentDocument *pComponent, CSize& cNewSize );
	CSize 					GetPreviewImageSize( RImage *pImage, CRect cRect );

	RBitmapImage*			GetImage() { return m_pImage ; }
	void						SetImage( RBitmapImage* pImage );
	void						ApplyImageEffects( RComponentView* pSourceView = NULL, BOOLEAN fResize = FALSE );
	void						ResetImageToLastSavedState();

	void						GetCropRect( RIntRect *pRect );
	BOOLEAN					ImageHasFramePath() { return m_bImageHasFramePath; }

	void						EnableRevertAndOkButtons( BOOLEAN bEnable, BOOLEAN bLock = FALSE );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	virtual BOOL OnInitDialog();

	afx_msg void OnSelChangeTabs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnParentNotify( UINT message, LPARAM lParam );

	virtual void OnOK();
	
	afx_msg void OnReset();
	afx_msg void OnSaveAs();

	DECLARE_MESSAGE_MAP()


private:
	CTabCtrl				m_ctlTabCtrl ;

	CDialog*				m_pCurrentPage ;
	int					m_nCurSelPage;		// index of current page

	RTabbedEditCrop			m_pageCrop ;
	RTabbedEditAdjustCol		m_pageAdjustColor ;
	RTabbedEditBrightFocus	m_pageBrightFocus ;
	RTabbedEditFixFlaw		m_pageFixFlaw ;
	RTabbedEditArtEffect		m_pageArtEffect ;

	RBitmapImage*				m_pImage;

	RComponentDocument*		m_pDesignDeskComponent;
	RComponentDocument*		m_pLocalComponent;
	RMBCString*					m_pSaveAsDir; 

	BOOLEAN						m_bImageHasFramePath;
	BOOLEAN						m_bButtonsEnabledAndLocked;

	BOOLEAN						InitializeLocalComponent( BOOLEAN bFirstTime = FALSE );
	
	void							ShowPage( int nPage );
};

// ****************************************************************************
//
//  Class Name:		EditUndoDocument
//
//  Description:	this class allows access to Undo mechanism without going
//					through the RView MessageDispatcher
//
// ****************************************************************************
class REditUndoManager : public RUndoManager
{
	public:
						REditUndoManager() { ; }

		virtual BOOLEAN	SendAction( RAction* pAction );
};	

#endif	// _TABBEDEDITIMAGE_H_
