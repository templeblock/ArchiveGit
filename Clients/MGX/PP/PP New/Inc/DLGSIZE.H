/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                dlgsize.h

*****************************************************************************
*/

#ifndef _DLGSIZE_H
#define _DLGSIZE_H

#include "ppmodal.h"

/* ****************************** Public Data ***************************** */
/* ****************************** Constants ******************************* */
/* ****************************** Typedefs ******************************** */
/* ****************************** Classes ********************************* */
class CSizeImageDlg : public CPPViewModalDlg
{
public:
        
    CSizeImageDlg(CServerView *pView, CWnd* pParentWnd = NULL );

  	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL PreDoModal(LPVOID lpVoid);
    virtual LPVOID PostDoModal();

    LFIXED GetWidth() {return (m_Width) ;};
    LFIXED GetHeight() {return (m_Height) ;};
    WORD   GetResolution() {return (m_Resolution) ;};
    BOOL   GetSmartSize() {return (m_SmartSize) ;};
    BOOL   GetDistortable() {return (m_Distortable) ;};
    BOOL   GetMaintainSize() {return (m_MaintainSize) ;};

	DECLARE_MESSAGE_MAP()
    afx_msg void OnClose();

protected:

    int CROPWIDTH ()  {return (m_Crop.right-m_Crop.left+1);};
    int CROPHEIGHT () {return (m_Crop.bottom-m_Crop.top+1);};

private:
    VOID DoSizeInits(  );
    BOOL DoSizeControls( ITEMID id, UINT codeNotify, BOOL bResAdjust );

    VOID SetUnitLabels();
    int SetUnitResolution( int res );

    WORD m_Resolution;
    LFIXED m_Width;
    LFIXED m_Height;
    WORD m_Units;
    int  m_UnitRes;		
    int	 m_Points;		
    BOOL m_Distortable;
    BOOL m_MaintainSize;
    WORD m_ScaleX;
    WORD m_ScaleY;
    RECT m_Crop;
    WORD m_CropResolution;
    WORD m_OrigResolution;
    int	 m_Depth;
    LFIXED	m_Angle;
    BOOL  m_SmartSize;
};

#endif // _DLGSIZE_H_
