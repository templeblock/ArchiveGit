// ****************************************************************************
//
//  File Name:		TabbedEditAdjustCol.h
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Declaration of the RTabbedEditAdjustCol class
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

#ifndef _TABBEDEDITADJUSTCOL_H_
#define _TABBEDEDITADJUSTCOL_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "GraphicCompResource.h"
#include "PhotoWorkshopResource.h"
#include "ImagePreviewCtrl.h"
#include "HexColorGrid.h"
#include "UndoableAction.h"
#include "ScratchBitmapImage.h"

////////////////////////////////////////////////////////////////////////////////
//
//	Define the following to enable dynamic preview updates.  When dynamic preview
//	updates are enabled, all six of the "More xxx" preview thumbnails update with
//	each user action.  Each is updated to represent the exact appearance of the
//	image were the user to select that direction.
//
//	If dynamic preview updates are not enabled (by commneting out the definition
//	below), the six surrounding thumbnails display the extreme limits of their
//	given directions, and they do not update with each user action.
//
////////////////////////////////////////////////////////////////////////////////
//
// #define DYNAMIC_PREVIEW_UPDATES

class REditUndoManager;
class RTabbedEditImage;
class RTabbedEditAdjustCol;

// ****************************************************************************
//
//  Class Name:		RCastLUTsCtrl
//
//  Description:	Container class for the array of preview controls
//
// ****************************************************************************
//
class RCastLUTsCtrl
{
	public:
		RCastLUTsCtrl();
		~RCastLUTsCtrl();
	
		RCastLUTs 			m_rLUTs;
		RImagePreviewCtrl	m_rPreviewCtrl;
		RComponentDocument*	m_pComponent;
};	


// ****************************************************************************
//
//  Class Name:		RAdjustColorAction
//
//  Description:	Undoable action for Adjus Color dialog
//
// ****************************************************************************
//
class RAdjustColorAction : public RUndoableAction
{
	public :
		enum EActionType
		{
			kDirection,
			kCoarse,
			kGrayscale
		};	

	public :
								RAdjustColorAction( RTabbedEditAdjustCol* pOwner, EActionType eAction, RHexColorGrid& rHexColorGrid );

		virtual void			Undo( );
		virtual BOOLEAN			WriteScript( RScript& script ) const;

		//	Identifier
		static YActionId		m_ActionId;

	protected :
		RTabbedEditAdjustCol*	m_pOwnerDlg;

	private:
		EActionType	m_eAction;

		RHexColorGrid	m_hexColorGrid;

#ifdef	TPSDEBUG
	//	Debugging code
	public :
		virtual	void			Validate( ) const;
#endif
};

// ****************************************************************************
//
//  Class Name:		RTabbedEditAdjustCol
//
//  Description:	Image Edit Adjust Color dialog
//
// ****************************************************************************
//
class RTabbedEditAdjustCol : public CDialog
{
	public:
				RTabbedEditAdjustCol(CWnd* pParent = NULL);   // standard constructor
				~RTabbedEditAdjustCol();

		enum { IDD = IDD_EDIT_ADJUSTCOLOR_TAB };

		void	ProcessUndo( RAdjustColorAction::EActionType eAction, RHexColorGrid& rHexColorGrid );

	protected:
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(RTabbedAdjustCol)
		protected:
		virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL	OnInitDialog();
//		virtual void	OnOK();
		virtual BOOL	PreTranslateMessage( MSG* pMsg );
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG(RTabbedAdjustCol)
		afx_msg void	OnShowWindow( BOOLEAN bShow, UINT nStatus );
		afx_msg void	OnParentNotify( UINT message, LPARAM lParam );
		afx_msg void	OnUndo();
		afx_msg LONG	OnApplyEditsToIEDBitmap( WPARAM, LPARAM );
		afx_msg LONG	OnResetUpdate( WPARAM, LPARAM );
		afx_msg LONG	OnOKFromIED( WPARAM, LPARAM );
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		afx_msg LONG OnUndoFromIED( WPARAM, LPARAM ) { OnUndo(); return 1L; }

	private:	
		RHexColorGrid			m_hexColorGrid;
		YIntDimension			m_angle;
		YFloatType				m_effectFraction;

		RComponentDocument	*m_pBaseComponent;
		RComponentDocument	*m_pBeforeComponent;
		RImagePreviewCtrl		m_rBeforePreviewCtrl;

		RCastLUTsCtrl			*m_pCastLUTsCtrlArray[7];

		BOOLEAN					m_bCoarseFlag;
		INT						m_nGrayscaleCount;
		RScratchBitmapImage	*m_pGrayscaleButtonBitmap;

		REditUndoManager		*m_pUndoManager;

		CBitmapButton			m_cButtonUndo;

		void 						UpdatePreviews( BOOLEAN bUpdateAll = TRUE );
		BOOLEAN					UpdateLUTs( EDirection direction, BOOLEAN bForceUpdate = FALSE );
		void						CopyLUTs( EDirection srcDirection, EDirection destDirection );
		BOOLEAN					BuildLUTs( RCastLUTsCtrl *pCtrl, YHueAngle angle, YFloatType fraction );
		BOOLEAN					ApplyLUTsToPreviewControl( RCastLUTsCtrl *pCtrl );
		BOOLEAN					ApplyLUTsToComponent( RComponentDocument *pComponent, RCastLUTs *pLUTs );
		void						ApplyLUTsToBitmap( RBitmapImage *pSrcBitmap, RBitmapImage *pDestBitmap, RCastLUTs *pLUTs );

		void						OnFineRadio();
		void						OnCoarseRadio();

		void						OnConvertToGrayscale();

		void						UpdateUndoButton();
		void						SetGrayscaleButtonImage();

#ifdef DYNAMIC_PREVIEW_UPDATES
		BOOLEAN				GenerateNewLUTs(  EDirection direction );

		BOOLEAN				RedShift();
		BOOLEAN				YellowShift();
		BOOLEAN				GreenShift();
		BOOLEAN				CyanShift();
		BOOLEAN				BlueShift();
		BOOLEAN				MagentaShift();
#endif

};

#endif