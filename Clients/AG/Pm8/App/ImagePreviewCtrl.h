//****************************************************************************
//
// File Name:			ImagePreviewCtrl.h
//
// Project:				Framework
//
// Author:				G. Brown
//
// Description:		RImagePreviewCtrl declaration
//
//							NOTE SS_NOTIFY must be set on any controls which use 
//							this class.
//
// Portability:		Windows
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, Ma 02158
//							(617)332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ImagePreviewCtrl.h                                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
// *****************************************************************************

#ifndef	_IMAGEPREVIEWCTRL_H_
#define	_IMAGEPREVIEWCTRL_H_

class RImage;
class RComponentDocument;

class RImagePreviewCtrl : public CStatic
{
public:
								RImagePreviewCtrl();
								~RImagePreviewCtrl();
						
	void					SetImageOnly(RImage* pImage, BOOLEAN fResetCropArea = TRUE);
	void					SetDocument(RComponentDocument* pImageDocument, BOOLEAN fResetCropArea = TRUE);
	void						EnableCropping(BOOLEAN fEnableCropping);	
	void						SetCropArea(const RRealRect& rCropArea);
	const RRealRect&		GetCropArea();

protected:
	virtual BOOL			PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg BOOL			OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnSetFocus(CWnd* pOldWnd);
	afx_msg void			OnKillFocus(CWnd* pNewWnd);	
	afx_msg void			OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	void					SetDisplayRect(BOOLEAN fResetCropArea);

	void						VerifyRectTracker();
	void						UpdateRectTracker();
	void						UpdateCropArea();

private:	
	RImage*					m_pImage;
	RComponentDocument*		m_pComponent;
	BOOLEAN					m_fEnableCropping;
	CRect					m_cImageRect;
	CRectTracker			m_cRectTracker;
	RRealRect				m_rCropArea;	
};

#endif	// _IMAGEPREVIEWCTRL_H_
