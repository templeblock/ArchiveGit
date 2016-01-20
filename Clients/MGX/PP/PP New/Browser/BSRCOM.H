// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifndef _BSRCOM_H
#define _BSRCOM_H

#include <afxdlgs.h>

/* ******************************* Public Data ***************************** */

/* ********************************* Constants ****************************** */

/* *************************** Class Declarations ************************** */

class CMgxFileDialog : public CFileDialog
{

	// Modeless construct
public:

				CMgxFileDialog( BOOL bOpenFileDialog, BOOL bWin95 );

public:

		inline	OPEN_MODE	GetOpenMode( ) { return( m_nOpenMode ); }	
		inline	BOOL		GetThumbState( ) { return( m_bThumbOpen ); }	
		inline	void		SetThumbDialog( int idDialog ) { m_idThumbDialog = idDialog; }	

// Overridable

	virtual 	BOOL OnInitDialog();

protected:
				
				int			m_idThumbDialog;
				BOOL			m_bThumbOpen;
				OPEN_MODE	m_nOpenMode;
				BOOL			m_bViewThumbNails;
				CButton		m_ThumbButton;
				CComboBox	m_ModeCombo;
				CStatic		m_ComboText;
				CRect			m_DialogRect;
				CRect			m_Rect;       
				CRect			m_ComboRect;       
				CRect			m_CheckRect;       
				CRect			m_StaticRect;       
				CRect			m_RectInit;       
				CRect			m_ComboRectInit;       
				CRect			m_CheckRectInit;       
				CRect			m_StaticRectInit;       

				afx_msg 	void OnViewThumbNails( );
				afx_msg	BOOL OnFileNameOK( );	

				DECLARE_MESSAGE_MAP( )    
};  

#endif
