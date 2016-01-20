//****************************************************************************
//
// File Name:		FramesDialog.h
//
// Project:			Renaissance Text Component
//
// Author:			Mike Heydlauf
//
// Description:	Definition of RFramesDialog.   
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/FramesDialog.h              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _FRAMESDIALOG_H_
#define _FRAMESDIALOG_H_

#ifndef _FRAMEWORKRESOURCEIDS_H_
#include "FrameworkResourceIDs.h"
#endif

#ifndef _FRAME_H_
#include "Frame.h"
#endif

#ifndef _WINCOLORDLG_H_
	#include "WinColorDlg.h"
#endif

#ifndef _BITMAPIMAGE_H_
#include "BitmapImage.h"
#endif // _BITMAPIMAGE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

/////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

enum EFrameType;

// ****************************************************************************
//
//  Class Name:		RFramesDialog
//
//  Description:		Dialog which allows the user to choose or remove a frame
//							from around selected objects.
//
// ****************************************************************************
//
class RFramesDialog : public CDialog
{
public:
// Construction
	RFramesDialog(CWnd* pParent = NULL);   // standard constructor

public:
	// Operations
	void								SetMultipleFrameTypes( BOOL bMulti ) { m_bMultipleFrameTypes = bMulti; }
	BOOL								GetMultipleFrameTypes() { return m_bMultipleFrameTypes; }

	void								SetFrameType( EFrameType eFrameType );
	EFrameType						GetFrameType();
	RSolidColor						GetFrameColor() const;
	void								SetFrameColor( const RSolidColor rNewColor );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RFramesDialog)
	protected:
	virtual void					DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Data members
	EFrameType						m_eFrameType;
	CArray<CBitmap, CBitmap&>	m_aButtonBitmaps;
	CBitmap							m_bmpColorBtn;
	RBitmapImage					m_rBitmapImage;
	RSolidColor						m_rColor;
	RRealRect						m_rRealRectPreview;
	RIntRect							m_rIntRectImage;
	RIntRect							m_rIntRectLogicalImage;
	BOOL								m_bMultipleFrameTypes;

	//{{AFX_DATA(RFramesDialog)
	enum { IDD = DIALOG_FRAMES };

	CStatic							m_ctrlFrameDisplay;

	RWinColorBtn					m_ColorBtn;
	CButton							m_btnThinFrame;
	CButton							m_btnMediumFrame;
	CButton							m_btnThickFrame;

	CButton							m_btnDblThinFrame;
	CButton							m_btnDblMediumFrame;
	CButton							m_btnDblThickFrame;

	CButton							m_btnRoundCornerFrame;
	CButton							m_btnPictureFrame;
	CButton							m_btnDropShadowFrame;
	//}}AFX_DATA

protected:
	// Operations
	void								PushFrameButton( int nID );
	void								LoadFrameButtonBitmaps();
	void								RenderPreview();
	BOOL								CheckActiveFrameButton();

	CButton*							MapFrameToButton( EFrameType eFrameType );

	virtual BOOL					PreTranslateMessage( MSG* pMsg );


	// Generated message map functions
	//{{AFX_MSG(RFramesDialog)
	afx_msg	void					OnPaint();
	virtual	BOOL					OnInitDialog();

	afx_msg	void					OnButtonThinFrame();
	afx_msg	void					OnButtonMediumFrame();
	afx_msg	void					OnButtonThickFrame();

	afx_msg	void					OnButtonDblThinFrame();
	afx_msg	void					OnButtonDblMediumFrame();
	afx_msg	void					OnButtonDblThickFrame();

	afx_msg	void					OnButtonRoundCornerFrame();
	afx_msg	void					OnButtonPictureFrame();
	afx_msg	void					OnButtonDropShadowFrame();

	afx_msg	void					OnButtonRemoveFrame();
	afx_msg	void					OnButtonSetFrameColor();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//********************************************************************************************************
//********************************************************************************************************
// Inlines
//********************************************************************************************************
//********************************************************************************************************

//*****************************************************************************************************
// Function Name:	RFramesDialog::SetFrameType
//
// Description:	Set the frame type
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
inline void RFramesDialog::SetFrameType( EFrameType eFrameType )
{
	m_eFrameType = eFrameType;
}

//*****************************************************************************************************
//
// Function Name:	RFramesDialog::GetFrameType
//
// Description:	Get the frame type
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
//
inline EFrameType RFramesDialog::GetFrameType()
{
	return EFrameType(m_eFrameType);
}

//*****************************************************************************************************
//
// Function Name:	RFramesDialog::GetFrameColor
//
// Description:	Returns the frame color
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
//
inline RSolidColor RFramesDialog::GetFrameColor() const
{
	return m_rColor;
}


//*****************************************************************************************************
//
// Function Name:	RFramesDialog::SetFrameColor
//
// Description:	Sets the frame color
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
//
inline void RFramesDialog::SetFrameColor( const RSolidColor rNewColor )
{
	m_rColor = rNewColor;	
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _FRAMESDIALOG_H_