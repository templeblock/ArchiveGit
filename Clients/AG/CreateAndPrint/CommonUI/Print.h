#pragma once

#include "AGDoc.h"
#include "AGDC.h"
#include "PaperTemplates.h"
#include "PaperFeed.h"
#include "Label.h"
#include "Previewfrm.h"
#include "PrintPreviewWindowEx.h"

class CCtp;
class CAGDoc;
class CDocWindow;

struct PN { char name[64]; };

enum PRINT_QUADRANTS
{
	QUADRANT_NONE  = 0x0,
	QUADRANT_ONE   = 0x1,
	QUADRANT_TWO   = 0x2,
	QUADRANT_THREE = 0x4,
	QUADRANT_FOUR  = 0x8,
};

class CPrint : public CPrintJobInfo
{
public:
	CPrint(CDocWindow* pDocWindow);
	~CPrint();
	bool Start();
	bool StartPreview();
	bool MakePdf(LPCSTR strDocFileName, LPCSTR strFileName);
	void GetMargins(CAGDoc* pAGDoc, RECT& Margins);
	void ActivateNewDoc();
	int ShowDialog(PRINTDLG* pPrintDlg);
	void DoubleSidedTest(bool bFirstTest);

protected:
	bool ContinuePrinting(bool bPortraitPaper);
	int GetPaperDirection(bool bPortraitPaper);
	int FindPaperSizeMatch(SIZE& PaperSize, int iBorderless, WORD* pw, POINT* ps, PN* pn, int nPapers);
	int FindPaperSizeClosest(SIZE& PaperSize, int iBorderless, WORD* pw, POINT* ps, PN* pn, int nPapers);
	bool IsBorderlessSupported(LPCSTR strDevice, LPCSTR strPort);
	bool IsBorderlessSupported(PN* pn, int nPapers);
	bool SetPaperSize(const PaperTemplate* pPaperTemplate, CAGDoc* pAGDoc, bool bSilent);
	bool SetDevModeFullSheet();
	bool SetDevModeSize(int iPaperIndex, const SIZE& DocSize);
	bool SetDevModeSizeNT(LPSTR pstrDevice, const SIZE& DocSize);
	bool IsHpMombiDriver();
	void ComputeMargins();
	void GetDriverMargins(RECT& rMargins);
	bool IsDriverBorderless();
	bool DoMultiplePasses(bool& bPortraitPaper);
	bool DoPass(CAGPrintDC* pPrintDC, int iPass, bool& bPortraitPaper, bool bPreview = false);
	bool DoPass(CAGPrintDC* pPrintDC, int iStart, int iPass, bool& bPortraitPaper, bool bPreview = false);
	bool Send(const int* pPageOrder, int iStart, int nPages, const PaperTemplate& PaperTemplate, bool bPortraitPaper, bool bMirror, bool bCenterOnly, int nCopies, PRINT_QUADRANTS dwQuadrant, CAGPrintDC* pDC, bool bPreview);
	void OrientEnvelope(bool bPortraitPaper, RECT& DestRect, int iDeviceWidth, int iDeviceHeight, bool& bRotate180);
	void OrientCustomPaper(bool bPortraitPaper, RECT& DestRect, int iDeviceWidth, int iDeviceHeight, int iWidth, int iHeight);
	/*
	// TFT Changes ...moved the contents to CPrint::Send()
	CAGDIBSectionDC* CreateMirrorDib(const CAGPrintDC* pDC);
	*/
	bool IsDocDuplex();
	bool IsCustomPaper(AGDOCTYPE PrintDocType = DOC_DEFAULT);
	bool ShowDuplexTest();
	bool NeedDuplexTest();
	bool MailMergePrint(CAGPrintDC* pDC, long lIndex, bool bPrintPreview, bool& bPortraitPaper);
	bool DoMailMerge(bool& bPortraitPaper);
	void LoadDeviceValuesFromRegistry(LPCTSTR strDevice);
	bool HandleDeviceChange(HWND hWnd, int& iLastDeviceChecksum);
    bool HandleOrientation(HWND hWnd, WORD wID);
    bool SaveOrientation(HWND hWnd);
    bool InitializeOrientation(HWND hWnd);
    bool UpdateOrientationImages(HWND hWnd);
	UINT PrintHookProc(HWND hWnd, UINT msg, WPARAM wParam);
	static UINT APIENTRY PrintHookProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void InitWindow(HWND hWnd);
	bool PrintPage(UINT nPage, HDC hDC);
	int DetermineNumPreviewPages();

protected:
    HFONT m_hHdrFont1;
    HFONT m_hHdrFont2;
	CCtp* m_pCtp;
	CAGDoc* m_pAGDoc;
	CDocWindow* m_pDocWindow;
	HGLOBAL m_hDevNames;
	HGLOBAL m_hDevMode;
	DEVMODE* m_pDevMode;
	CRect m_MarginsPortrait;
	CRect m_MarginsLandscape;
	CString m_strDriver;
	CString m_strDevice;
	CString m_strPort;
	int m_iPaperType;
	int m_iDuplex;
	int m_iPaperFeedCode;
	bool m_bForceDuplexTest;
	bool m_bForceBorderless;
	bool m_bBorderlessSupported;
	bool m_bLaunchEnvWizard;

	AGDOCTYPE m_PrintDocType;
    CLabel m_lblEnvelopePreviewGroup;
    CLabel m_lblCopiesGroup;
    CLabel m_lblPaperTypeGroup;
    CLabel m_lblSelectPrintGroup;
    CLabel m_lblPrinterSettingsGroup;
    CLabel m_lblPaperLoadHeader;
    CLabel m_lblPaperAlignHeader;
    CLabel m_lblEnvelopeLoadHeader;
    CLabel m_lblEnvelopeEdgeHeader;
	CLabel m_lblEnvelopeHelp;
	
	CPreviewFrame m_PreviewFrame;
	CPrintPreviewWindowEx m_PreviewWnd;
};
