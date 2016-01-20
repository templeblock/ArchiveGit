#pragma once

#include "AGDoc.h"
#include "AGDC.h"
#include "PaperTemplates.h"
#include "PaperFeed.h"
#include "Label.h"


class CCtp;
class CAGDoc;
class CDocWindow;

struct PN { char name[64]; };

class CPrint
{
public:
	CPrint(CDocWindow* pDocWindow);
	~CPrint();
	bool Start();
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
	bool DoPass(CAGPrintDC* pPrintDC, int iPass, bool& bPortraitPaper);
	bool Send(const int* pPageOrder, int iStart, int nPages, const PaperTemplate& PaperTemplate, bool bPortraitPaper, bool bMirror, bool bCenterOnly, int nCopies, CAGPrintDC* pDC);
	void OrientEnvelope(bool bPortraitPaper, RECT& DestRect, int iDeviceWidth, int iDeviceHeight, bool& bRotate180);
	void OrientCustomPaper(bool bPortraitPaper, RECT& DestRect, int iDeviceWidth, int iDeviceHeight, int iWidth, int iHeight);
	CAGDIBSectionDC* CreateMirrorDib(const CAGPrintDC* pDC);
	bool IsDocDuplex();
	bool IsCustomPaper(AGDOCTYPE PrintDocType = DOC_DEFAULT);
	bool ShowDuplexTest();
	bool NeedDuplexTest();
	void LoadDeviceValuesFromRegistry(LPCTSTR strDevice);
	bool HandleDeviceChange(HWND hWnd, int& iLastDeviceChecksum);
    bool HandleOrientation(HWND hWnd, WORD wID);
    bool SaveOrientation(HWND hWnd);
    bool InitializeOrientation(HWND hWnd);
    bool UpdateOrientationImages(HWND hWnd);
	UINT PrintHookProc(HWND hWnd, UINT msg, WPARAM wParam);
	static UINT APIENTRY PrintHookProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void InitWindow(HWND hWnd);

protected:
    HFONT m_hHdrFont1;
    HFONT m_hHdrFont2;
	CCtp* m_pCtp;
	CAGDoc* m_pAGDoc;
	CDocWindow* m_pDocWindow;
	HGLOBAL m_hDevNames;
	HGLOBAL m_hDevMode;
	DEVMODE* m_pDevMode;
	RECT m_MarginsPortrait;
	RECT m_MarginsLandscape;
	CString m_strDriver;
	CString m_strDevice;
	CString m_strPort;
	int m_iPaperType;
	int m_iDuplex;
	int m_iPaperFeedCode;
	bool m_bForceDuplexTest;
	bool m_bForceBorderless;
	bool m_bBorderlessSupported;
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

};
