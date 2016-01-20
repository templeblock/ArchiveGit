// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CTPUtil.h"
#include "MainFrm.h"
#include "AGDoc.h"
#include "AGDib.h"
#include "ProgDlg.h"
#include "ScaleImage.h"

#include <fstream.h>
#include <windowsx.h> // For GlobalAlloc stuff
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MAX_FILEBUF 65535

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_COMMAND(ID_BATCHPRINT, OnBatchPrint)
	ON_COMMAND(ID_BUILDTHUMBS, OnBuildThumbs)
	ON_UPDATE_COMMAND_UI(ID_PRINTBOTH, OnUpdatePrintBoth)
	ON_COMMAND(ID_PRINTBOTH, OnPrintOptions)
	ON_UPDATE_COMMAND_UI(ID_PRINTDEFAULT, OnUpdatePrintDefault)
	ON_UPDATE_COMMAND_UI(ID_PRINTQUARTER, OnUpdatePrintQuarter)
	ON_UPDATE_COMMAND_UI(ID_PRINTSINGLE, OnUpdatePrintSingle)
	ON_COMMAND(ID_PRINTDEFAULT, OnPrintOptions)
	ON_COMMAND(ID_PRINTQUARTER, OnPrintOptions)
	ON_COMMAND(ID_PRINTSINGLE, OnPrintOptions)
	ON_COMMAND(ID_CARDLIST, OnCardList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	Create(NULL, "Create and Print Utility", WS_OVERLAPPEDWINDOW, rectDefault, NULL, MAKEINTRESOURCE(IDR_MAINFRAME));

	m_nPrintOption = ID_PRINTDEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnBatchPrint() 
{
	CString csFilters = "Create and Print Files(*.ctp)|*.ctp||";
	CFileDialog FileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, csFilters, AfxGetMainWnd());
	FileDlg.m_ofn.lpstrTitle = "Select File(s) to Print";
	FileDlg.m_ofn.lpstrFile = (char*)malloc(MAX_FILEBUF);
	*FileDlg.m_ofn.lpstrFile = 0;
	FileDlg.m_ofn.nMaxFile = MAX_FILEBUF;

	if (FileDlg.DoModal() == IDOK)
	{
		PRINTDLG PrintDlg;
		if (AfxGetApp()->GetPrinterDeviceDefaults(&PrintDlg))
		{
			DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(PrintDlg.hDevNames);
			DEVMODE* pDevMode = (DEVMODE*)GlobalLock(PrintDlg.hDevMode);
			char* pszDriver = ((char*)pDevNames) + pDevNames->wDriverOffset;
			char* pszDevice = ((char*)pDevNames) + pDevNames->wDeviceOffset;
			char* pszOutput = ((char*)pDevNames) + pDevNames->wOutputOffset;

			CProgressDlg ProgDlg;
			ProgDlg.Create();

			int nFileCount = 0;
			POSITION Pos = FileDlg.GetStartPosition();
			while (Pos)
			{
				nFileCount++;
				FileDlg.GetNextPathName(Pos);
			}
			ProgDlg.SetRange(0, nFileCount);

			Pos = FileDlg.GetStartPosition();
			while (Pos)
			{
				CString csFileName = FileDlg.GetNextPathName(Pos);
				CString csMsg = "Printing " + csFileName;
				ProgDlg.SetStatus(csMsg);
				if (ProgDlg.CheckCancelButton())
					break;

				CAGDoc* pAGDoc = new CAGDoc();
				ifstream In(csFileName, ios::in | ios::nocreate | ios::binary);

				if (pAGDoc->Read(In))
				{
					int nOption = m_nPrintOption;
					if (nOption == ID_PRINTDEFAULT)
					{
						AGDOCTYPE DocType = pAGDoc->GetDocType();
						if (DocType == DOC_CARD_QUARTERFOLD_PORTRAIT || DocType == DOC_CARD_QUARTERFOLD_LANDSCAPE)
							nOption = ID_PRINTQUARTER;
						else
						if (DocType == DOC_CARD_SINGLEFOLD_PORTRAIT || DocType == DOC_CARD_SINGLEFOLD_LANDSCAPE)
							nOption = ID_PRINTSINGLE;
						else
							nOption = 0;
					}

					if (ProgDlg.CheckCancelButton())
						break;

					if (nOption == ID_PRINTQUARTER || nOption == ID_PRINTBOTH)
						pAGDoc->PrintCardQuarter(pszDriver, pszDevice, pszOutput, pDevMode, csFileName);

					if (ProgDlg.CheckCancelButton())
						break;

					if (nOption == ID_PRINTSINGLE || nOption == ID_PRINTBOTH)
					{
						bool bRotated;
						pAGDoc->PrintCardSingle(PRINT_BOTH, pszDriver, pszDevice, pszOutput, pDevMode, bRotated, csFileName);
					}

					ProgDlg.StepIt();
					if (ProgDlg.CheckCancelButton())
						break;
				}

				delete pAGDoc;
			}

			::GlobalUnlock(PrintDlg.hDevMode);
			::GlobalUnlock(PrintDlg.hDevNames);
		}
	}

	free(FileDlg.m_ofn.lpstrFile);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBuildThumbs()
{
	CString csFilters = "Create and Print Files(*.ctp)|*.ctp||";
	CFileDialog FileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, csFilters, AfxGetMainWnd());
	FileDlg.m_ofn.lpstrTitle = "Select File(s) to Build Thumbs";
	FileDlg.m_ofn.lpstrFile = (char*)malloc(MAX_FILEBUF);
	*FileDlg.m_ofn.lpstrFile = 0;
	FileDlg.m_ofn.nMaxFile = MAX_FILEBUF;

	if (FileDlg.DoModal() == IDOK)
	{
		CProgressDlg ProgDlg(IDS_PROGRESS_THUMBS);
		ProgDlg.Create();

		int nFileCount = 0;
		POSITION Pos = FileDlg.GetStartPosition();
		while (Pos)
		{
			nFileCount++;
			FileDlg.GetNextPathName(Pos);
		}
		ProgDlg.SetRange(0, nFileCount);

		Pos = FileDlg.GetStartPosition();
		while (Pos)
		{
			CString csFileName = FileDlg.GetNextPathName(Pos);
			CString csMsg = "Building Thumbs for " + csFileName;
			ProgDlg.SetStatus(csMsg);
			if (ProgDlg.CheckCancelButton())
				break;

			CAGDoc* pAGDoc = new CAGDoc();
			ifstream In(csFileName, ios::in | ios::nocreate | ios::binary);

			if (pAGDoc->Read(In))
			{
				if (ProgDlg.CheckCancelButton())
					break;
			
				CAGPage* pPage = pAGDoc->GetPage(1);
				if (pPage)
				{
					int nLast = csFileName.ReverseFind('\\');
					CString csThumbDir  = csFileName.Left(nLast) + "\\Thumbs";
					CString csDetailDir = csFileName.Left(nLast) + "\\Details";

					mkdir(csThumbDir);
					mkdir(csDetailDir);

					CString csThumbT = csThumbDir + csFileName.Mid(nLast, csFileName.GetLength() - nLast - 5) + "T.bmp";
					CString csThumbD = csDetailDir + csFileName.Mid(nLast, csFileName.GetLength() - nLast - 5) + "D.bmp";

					CreateThumb(pPage, 153*2, csThumbT);
					CreateThumb(pPage, 400*2, csThumbD);
				}

				ProgDlg.StepIt();
			}

			delete pAGDoc;
		}
	}

	free(FileDlg.m_ofn.lpstrFile);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnCardList()
{
	CString csFilters = "Create and Print Files(*.ctp)|*.ctp||";
	CFileDialog FileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, csFilters, AfxGetMainWnd());
	FileDlg.m_ofn.lpstrTitle = "Select File(s) for Card List";
	FileDlg.m_ofn.lpstrFile = (char*)malloc(MAX_FILEBUF);
	*FileDlg.m_ofn.lpstrFile = 0;
	FileDlg.m_ofn.nMaxFile = MAX_FILEBUF;

	if (FileDlg.DoModal() == IDOK)
	{
		CProgressDlg ProgDlg(IDS_PROGRESS_CARDLIST);
		ProgDlg.Create();

		int nFileCount = 0;
		POSITION Pos = FileDlg.GetStartPosition();
		while (Pos)
		{
			nFileCount++;
			FileDlg.GetNextPathName(Pos);
		}
		ProgDlg.SetRange(0, nFileCount);

		FILE* output = fopen("cardlist.csv", "w");
		if (!output)
		{
			MessageBox("Can't open output file", NULL, MB_OK);
			free(FileDlg.m_ofn.lpstrFile);
			return;
		}

		fprintf(output, "Product, Type, Orientation, Pages\n");
		Pos = FileDlg.GetStartPosition();
		while (Pos)
		{
			CString csFileName = FileDlg.GetNextPathName(Pos);
			CString csMsg = "Processing " + csFileName;
			ProgDlg.SetStatus(csMsg);
			if (ProgDlg.CheckCancelButton())
				break;

			CAGDoc* pAGDoc = new CAGDoc();
			ifstream In(csFileName, ios::in | ios::nocreate | ios::binary);

			if (pAGDoc->Read(In))
			{
				if (ProgDlg.CheckCancelButton())
					break;

				char szFName[_MAX_FNAME];
				_splitpath(csFileName, NULL, NULL, szFName, NULL);
				szFName[7] = 0;

				AGDOCTYPE DocType = pAGDoc->GetDocType();
				CString strType = "unknown";
				CString strOrientation = "unknown";
				switch (DocType)
				{
					case DOC_CARD_SINGLEFOLD_PORTRAIT:
						strType = "single-fold";
						strOrientation = "portrait";
						break;
					case DOC_CARD_SINGLEFOLD_LANDSCAPE:
						strType = "single-fold";
						strOrientation = "landscape";
						break;
					case DOC_CARD_QUARTERFOLD_PORTRAIT:
						strType = "quarter-fold";
						strOrientation = "portrait";
						break;
					case DOC_CARD_QUARTERFOLD_LANDSCAPE:
						strType = "quarter-fold";
						strOrientation = "landscape";
						break;
				}

				fprintf(output, "%s, %s, %s, %d\n", szFName, strType, strOrientation, pAGDoc->GetNumPages());

				ProgDlg.StepIt();
				if (ProgDlg.CheckCancelButton())
					break;
			}

			delete pAGDoc;
		}

		fclose(output);
	}

	free(FileDlg.m_ofn.lpstrFile);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnPrintOptions()
{
	m_nPrintOption = GetCurrentMessage()->wParam;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdatePrintBoth(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_nPrintOption == ID_PRINTBOTH);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdatePrintDefault(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_nPrintOption == ID_PRINTDEFAULT);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdatePrintQuarter(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_nPrintOption == ID_PRINTQUARTER);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdatePrintSingle(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_nPrintOption == ID_PRINTSINGLE);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::CreateThumb(const CAGPage* pPage, int nMaxSize, const char* pszFileName)
{
	if (!pPage)
		return;

	SIZE sizePage;
	pPage->GetPageSize(&sizePage);

	double xScale = (double)nMaxSize / sizePage.cx;
	double yScale = (double)nMaxSize / sizePage.cy;
	if (xScale < yScale)
		yScale = xScale;
	else
		xScale = yScale;

	BITMAPINFO bi;
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth  = (int)((sizePage.cx * xScale) + 0.5);
	bi.bmiHeader.biHeight = (int)((sizePage.cy * yScale) + 0.5);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biXPelsPerMeter = 3938;
	bi.bmiHeader.biYPelsPerMeter = 3938;

	BYTE* pBits = NULL;
	CAGDIBSectionDC dc(&bi, DIB_RGB_COLORS, &pBits);

	RECT DestRect = {0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight};
	::FillRect(dc.GetHDC(), &DestRect, (HBRUSH)::GetStockObject(WHITE_BRUSH));

	CAGMatrix Matrix(xScale, 0, 0, yScale);
	dc.SetDeviceMatrix(Matrix);
	pPage->Draw(dc);

	BITMAPINFOHEADER biNew = bi.bmiHeader;
	biNew.biWidth /= 2;
	biNew.biHeight /= 2;
	biNew.biSizeImage = 0;

	BITMAPINFOHEADER* pdibDest = (BITMAPINFOHEADER*)GlobalAllocPtr(GHND, DibSize(&biNew));
	if (pdibDest)
	{
		*pdibDest = biNew;	
	
		Image dst, src;
		src.xsize = (int)bi.bmiHeader.biWidth;
		src.ysize = (int)bi.bmiHeader.biHeight;
		src.data = pBits;
		src.span = DibWidthBytes(&bi.bmiHeader);
		dst.xsize = (int)pdibDest->biWidth;
		dst.ysize = (int)pdibDest->biHeight;
		dst.data = (BYTE*)DibPtr(pdibDest);
		dst.span = DibWidthBytes(pdibDest);

		ScaleImage(&dst, &src);

		WriteBMP(pszFileName, (BITMAPINFO*)pdibDest, DibPtr(pdibDest));

		GlobalFreePtr(pdibDest);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::WriteBMP(const char* pszFileName, const BITMAPINFO* pbmi, const BYTE* pBits)
{
	int nImageSize = DibSizeImage(&pbmi->bmiHeader);

	BITMAPFILEHEADER bf;
	memset(&bf, 0, sizeof(bf));
	bf.bfType = 0x4d42; /* 'BM' */
	bf.bfSize = sizeof(bf) + pbmi->bmiHeader.biSize + nImageSize;
	bf.bfOffBits = sizeof(bf) + pbmi->bmiHeader.biSize;

	FILE* output = fopen(pszFileName, "wb");
	if (output)
	{
		fwrite(&bf, sizeof(bf), 1, output);
		fwrite(&pbmi->bmiHeader, pbmi->bmiHeader.biSize, 1, output);
		fwrite(pBits, nImageSize, 1, output);
		fclose(output);
	}
}
