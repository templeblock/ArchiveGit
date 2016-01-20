/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                dlgchrma.h

*****************************************************************************
*/

#ifndef _DLGCHROMA_H
#define _DLGCHROMA_H

#include "ppmodal.h"

#define NUM_CHROMAMASKS    8
#define TORANGE(n,range)	((int)( ((((long) range) * (n)) +  50) / 100 ))

class CChromaDlg;
class CChromaMask
{
   	public:
   	CChromaMask(CChromaDlg* pDlg);
   	CChromaMask(LPCHROMA_PARMS);

	void MaskAddChroma(LPIMAGE lpImage, LPMASK lpMask, LPRECT lpRect, BOOL MaskExist);
	BOOL JoinChromaMask(LPIMAGE lpImage, LPRECT lpUpdateRect);
	void SetupChroma();			
   	void InitMaskValue(BYTE Channel[NUM_CHROMAMASKS][256]);
	void SetupChromaRange(int i);
	void ComputeMaskValue(LPBYTE Channel);
	void ComputeChromaMask(int i);
	void UpdateChromaMask(int i, LPCOLORINFO lpColor);
	void ChromaProc(int dx, FRMTYPEINFO TypeInfo,	LPTR lpLine, LPTR lpSlime, BOOL MaskExist);
	int  GetRGBMask(LPTR lpLine);
	int  GetCMYKMask(LPTR lpLine);

	CChromaDlg* m_pDlg;
   	int         ChromaCombineMode;
   	int         ChromaColorMode;
   	int		   	ActiveChromaMask;
   	int		   	Mask;
   	int         Fade;
   	int		   	MaskRange[NUM_CHROMAMASKS];
   	BOOL        Delete;
   	VIEW_MODE   iMode;
   	COLORINFO   MaskColor[NUM_CHROMAMASKS];
   	BYTE	    RMask[NUM_CHROMAMASKS][256];
   	BYTE	    GMask[NUM_CHROMAMASKS][256];
   	BYTE	    BMask[NUM_CHROMAMASKS][256];

   	BYTE	    CMask[NUM_CHROMAMASKS][256];
   	BYTE	    MMask[NUM_CHROMAMASKS][256];
   	BYTE	    YMask[NUM_CHROMAMASKS][256];
   	BYTE	    KMask[NUM_CHROMAMASKS][256];

   	BYTE	    HMask[NUM_CHROMAMASKS][256];
   	BYTE	    SMask[NUM_CHROMAMASKS][256];
   	BYTE	    LMask[NUM_CHROMAMASKS][256];

	int   		min, max;
	int   		ChromaRange;
	int   		MinMaxR[2];
	int   		MinMaxG[2];
	int   		MinMaxB[2];
	int   		MinMaxH[2];
	int   		MinMaxS[2];
	int   		MinMaxL[2];
	int   		MinMaxC[2];
	int   		MinMaxM[2];
	int   		MinMaxY[2];
	int   		MinMaxK[2];

	int  		xExtL;
	int  		xExtR;
	BOOL 		Dirty;
};

class CChromaDlg : public CPPViewModalDlg
{
public:
    CChromaDlg(CServerView *pView, CWnd* pParentWnd = NULL );

  	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL PreDoModal(LPVOID lpVoid);
    virtual LPVOID PostDoModal();
	void ShowChromaPreview();
	void ShowChromaReset();
	WORD 			idLastTool;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

protected:
	void UpdateChromaMask(int i, LPCOLORINFO lpColor);
	BOOL  			bPreview;
	BOOL			bProbePending;
	int   			iCombo[3];
	int   			nCombo;	  
	CRect 			rPreview;
	CChromaMask*	lpChromaMask;

private:
};


#endif // _DLGSIZE_H_
