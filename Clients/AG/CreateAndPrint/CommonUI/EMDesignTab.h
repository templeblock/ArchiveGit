#pragma once

#include "resource.h"
#include "EMBaseTab.h"

#define MAX_NUM_PICTURES	25
#define	MAX_PAGES_TO_CHECK	3

class CEMDesignTab;
class CDocWindow;
class CAGDoc;
class CAGSymImage;

// CPictureListView
///////////////////////////////////////////////////////////////////////////////
class CPictureListView : public CWindowImpl<CPictureListView, CListViewCtrl>
{
public:
	CPictureListView();
	virtual ~CPictureListView();

	void InitDialog(HWND hWnd);
	void SetParent(CEMDesignTab* pParent);
	void DrawListViewThumbnails();

protected:
	BEGIN_MSG_MAP(CPictureListView)
	END_MSG_MAP()

	CEMDesignTab* m_pParent;
	CImageList m_ImageList;
	int m_nThumbnailSize;
};
// CEMDesignTab dialog
///////////////////////////////////////////////////////////////////////////////
class CEMDesignTab : public CEMBaseTab<CEMDesignTab>
{
public:
	CEMDesignTab(CEMDlg * pEMDlg);
	virtual ~CEMDesignTab();

	BEGIN_MSG_MAP_EX(CEMDesignTab)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CTLCOLORSTATIC(OnCtlColor)
		//MSG_WM_ENABLE(OnEnable)
		COMMAND_HANDLER_EX(IDC_NEWDOC_PAPERTYPE, CBN_SELCHANGE, OnPaperType)
		COMMAND_HANDLER_EX(IDC_ENABLE_BACKGROUND, BN_CLICKED, OnEnableBackground)
		NOTIFY_HANDLER(IDC_PICTURE_SELECT, LVN_BEGINDRAG, OnLvnBegindragPictureSelect)
		CHAIN_MSG_MAP(CEMBaseTab<CEMDesignTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CEMDesignTab)
		
	END_DDX_MAP()

	enum { IDD = IDD_EMDESIGN_TAB };

	BOOL OnInitDialog(HWND hWnd, LPARAM lParam);
	//void OnEnable(BOOL bEnable);
	void OnEnableBackground(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	void OnPaperType(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnLvnBegindragPictureSelect(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnCtlColor(HDC hDC, HWND hwnd);


	void FinishImageDrop(bool fDrop, POINTL pt);
	void OnBegindragListThumbnail(NMHDR* pNMHDR, LRESULT* pResult);
	void SelectBackground();
	void ShowSelectedView(int nSelectedView);

	void SetPaperType(int iPaperType)
		{m_PaperType = iPaperType;}

	bool UpdatePaperType(int iPaperType);
	CPictureListView* GetPictureList()
		{ return &m_PictureList; }

	CAGSymImage* GetGraphicByIndex(int iIndex)
		{
			if (iIndex > MAX_NUM_PICTURES)
				return NULL;

			return m_pGraphics[iIndex]; 
		}

public:
	void AddDroppedGraphic(int nDragImage, POINT point );
	void InitGraphics(CAGDoc* pAGDoc);
	void InitPictureList();


protected:
	virtual void OnEnterTab(bool bFirstTime);
	virtual void OnLeaveTab();

private:
	HBRUSH GetBgBrush(COLORREF Color)
	{
		HBRUSH hBrush = m_Brush.GetHBrush();
		if (hBrush && m_LastColor == Color)
			return hBrush;

		hBrush = m_Brush.Create(Color);
		if (!hBrush)
			return NULL;

		m_LastColor = Color;
		return hBrush;
	}

public:
    CAGSymImage* m_pGraphics[MAX_NUM_PICTURES];

protected:
	int m_PaperType;
	int m_nDragImage;
	bool m_EnableBackground;
	COLORREF m_LastColor;
	CAGBrush m_Brush;
	CPictureListView m_PictureList;
	CDocWindow* m_pDocWindow;
	
	
	


};
