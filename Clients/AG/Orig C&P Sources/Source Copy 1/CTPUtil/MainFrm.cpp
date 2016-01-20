// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CTPUtil.h"
#include "MainFrm.h"
#include "AGDoc.h"
#include "AGDib.h"
#include "ProgDlg.h"

#include <fstream.h>
#include <windowsx.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	MAX_FILEBUF	65535

typedef struct {
	int	xsize;			/* horizontal size of the image in Pixels */
	int	ysize;			/* vertical size of the image in Pixels */
	BYTE * data;		/* pointer to first scanline of image */
	int	span;			/* byte offset between two scanlines */
} Image;

void zoom (Image *dst, Image *src);


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
	Create (NULL, "Create and Print Utility", WS_OVERLAPPEDWINDOW,
	  rectDefault, NULL, MAKEINTRESOURCE (IDR_MAINFRAME));

	m_nPrintOption = ID_PRINTDEFAULT;
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnBatchPrint() 
{
	CString	csFilters;

	csFilters = "Create and Print Files (*.ctp)";
	csFilters += "|";
	csFilters += "*.ctp";
	csFilters += "|";
	csFilters += "|";

	CFileDialog FileDlg (TRUE, NULL, NULL,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
	  csFilters, AfxGetMainWnd ());
	FileDlg.m_ofn.lpstrTitle = "Select File(s) to Print";
	FileDlg.m_ofn.lpstrFile = (char *) malloc (MAX_FILEBUF);
	*FileDlg.m_ofn.lpstrFile = 0;
	FileDlg.m_ofn.nMaxFile = MAX_FILEBUF;

	if (FileDlg.DoModal () == IDOK)
	{
		PRINTDLG PrintDlg;
		if (AfxGetApp ()->GetPrinterDeviceDefaults (&PrintDlg))
		{
			DEVNAMES *pDevNames = (DEVNAMES *) GlobalLock (PrintDlg.hDevNames);
			DEVMODE *pDevMode = (DEVMODE *) GlobalLock (PrintDlg.hDevMode);
			char *pszDriver = ((char *) pDevNames) + pDevNames->wDriverOffset;
			char *pszDevice = ((char *) pDevNames) + pDevNames->wDeviceOffset;
			char *pszOutput = ((char *) pDevNames) + pDevNames->wOutputOffset;

			CProgressDlg ProgDlg;
			ProgDlg.Create();

			int nFileCount = 0;
			POSITION Pos = FileDlg.GetStartPosition ();
			while (Pos != NULL)
			{
				nFileCount++;
				FileDlg.GetNextPathName (Pos);
			}
			ProgDlg.SetRange (0, nFileCount);

			Pos = FileDlg.GetStartPosition ();
			while (Pos != NULL)
			{
				CString csFileName = FileDlg.GetNextPathName (Pos);
				CString csMsg = "Printing ";
				csMsg += csFileName;
				ProgDlg.SetStatus (csMsg);
				if (ProgDlg.CheckCancelButton ())
					break;

				CAGDoc *pAGDoc = new CAGDoc ();
				ifstream In (csFileName, ios::in | ios::nocreate | ios::binary);

				if (pAGDoc->Read (In))
				{
					int nOption = m_nPrintOption;
					if (nOption == ID_PRINTDEFAULT)
					{
						AGDOCTYPE DocType = pAGDoc->GetDocType ();
						if (DocType == DOC_CARDFV || DocType == DOC_CARDFH)
							nOption = ID_PRINTQUARTER;
						else if (DocType == DOC_CARDHV || DocType == DOC_CARDHH)
							nOption = ID_PRINTSINGLE;
						else
							nOption = 0;
					}

					if (ProgDlg.CheckCancelButton ())
						break;

					if (nOption == ID_PRINTQUARTER || nOption == ID_PRINTBOTH)
					{
						pAGDoc->PrintCardQuarter (pszDriver, pszDevice,
						  pszOutput, pDevMode, csFileName);
					}

					if (ProgDlg.CheckCancelButton ())
						break;

					if (nOption == ID_PRINTSINGLE || nOption == ID_PRINTBOTH)
					{
						bool bRotated;
						pAGDoc->PrintCardSingle (PRINT_BOTH, pszDriver,
						  pszDevice, pszOutput, pDevMode, bRotated, csFileName);
					}

					ProgDlg.StepIt ();
					if (ProgDlg.CheckCancelButton ())
						break;
				}
				delete pAGDoc;

			}

			::GlobalUnlock (PrintDlg.hDevMode);
			::GlobalUnlock (PrintDlg.hDevNames);
		}
	}
	free (FileDlg.m_ofn.lpstrFile);
}

void CMainFrame::OnBuildThumbs() 
{
	CString	csFilters;

	csFilters = "Create and Print Files (*.ctp)";
	csFilters += "|";
	csFilters += "*.ctp";
	csFilters += "|";
	csFilters += "|";

	CFileDialog FileDlg (TRUE, NULL, NULL,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
	  csFilters, AfxGetMainWnd ());
	FileDlg.m_ofn.lpstrTitle = "Select File(s) to Build Thumbs";
	FileDlg.m_ofn.lpstrFile = (char *) malloc (MAX_FILEBUF);
	*FileDlg.m_ofn.lpstrFile = 0;
	FileDlg.m_ofn.nMaxFile = MAX_FILEBUF;

	if (FileDlg.DoModal () == IDOK)
	{
		CProgressDlg ProgDlg (IDS_PROGRESS_THUMBS);
		ProgDlg.Create();

		int nFileCount = 0;
		POSITION Pos = FileDlg.GetStartPosition ();
		while (Pos != NULL)
		{
			nFileCount++;
			FileDlg.GetNextPathName (Pos);
		}
		ProgDlg.SetRange (0, nFileCount);

		Pos = FileDlg.GetStartPosition ();
		while (Pos != NULL)
		{
			CString csFileName = FileDlg.GetNextPathName (Pos);
			CString csMsg = "Building Thumbs for ";
			csMsg += csFileName;
			ProgDlg.SetStatus (csMsg);
			if (ProgDlg.CheckCancelButton ())
				break;

			CAGDoc *pAGDoc = new CAGDoc ();
			ifstream In (csFileName, ios::in | ios::nocreate | ios::binary);

			if (pAGDoc->Read (In))
			{
				CAGPage *pPage = pAGDoc->GetPage (1);

				int nLast = csFileName.ReverseFind ('\\');
				CString csThumbT = csFileName.Left (nLast) + "\\..\\Thumb" +
				  csFileName.Mid (nLast, csFileName.GetLength () - nLast - 5) +
				  "T.bmp";
				CString csThumbD = csFileName.Left (nLast) + "\\..\\Detail" +
				  csFileName.Mid (nLast, csFileName.GetLength () - nLast - 5) +
				  "D.bmp";

				if (ProgDlg.CheckCancelButton ())
					break;
			
				CreateThumb (pPage, 153*2, csThumbT);

				if (ProgDlg.CheckCancelButton ())
					break;

				CreateThumb (pPage, 400*2, csThumbD);

				ProgDlg.StepIt ();
				if (ProgDlg.CheckCancelButton ())
					break;
			}
			delete pAGDoc;
		}
	}
	free (FileDlg.m_ofn.lpstrFile);
}


void CMainFrame::OnCardList() 
{
	CString	csFilters;

	csFilters = "Create and Print Files (*.ctp)";
	csFilters += "|";
	csFilters += "*.ctp";
	csFilters += "|";
	csFilters += "|";

	CFileDialog FileDlg (TRUE, NULL, NULL,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
	  csFilters, AfxGetMainWnd ());
	FileDlg.m_ofn.lpstrTitle = "Select File(s) for Card List";
	FileDlg.m_ofn.lpstrFile = (char *) malloc (MAX_FILEBUF);
	*FileDlg.m_ofn.lpstrFile = 0;
	FileDlg.m_ofn.nMaxFile = MAX_FILEBUF;

	if (FileDlg.DoModal () == IDOK)
	{
		CProgressDlg ProgDlg (IDS_PROGRESS_CARDLIST);
		ProgDlg.Create();

		int nFileCount = 0;
		POSITION Pos = FileDlg.GetStartPosition ();
		while (Pos != NULL)
		{
			nFileCount++;
			FileDlg.GetNextPathName (Pos);
		}
		ProgDlg.SetRange (0, nFileCount);

		FILE *output = fopen ("cardlist.csv", "w");
		if (output == NULL)
		{
			MessageBox ("Can't open output file", NULL, MB_OK);
			return;
		}

		Pos = FileDlg.GetStartPosition ();
		while (Pos != NULL)
		{
			CString csFileName = FileDlg.GetNextPathName (Pos);
			CString csMsg = "Processing ";
			csMsg += csFileName;
			ProgDlg.SetStatus (csMsg);
			if (ProgDlg.CheckCancelButton ())
				break;

			CAGDoc *pAGDoc = new CAGDoc ();
			ifstream In (csFileName, ios::in | ios::nocreate | ios::binary);

			if (pAGDoc->Read (In))
			{
				if (ProgDlg.CheckCancelButton ())
					break;

				char szFName[_MAX_FNAME];
				_splitpath (csFileName, NULL, NULL, szFName, NULL);
				szFName[7] = 0;

				AGDOCTYPE DocType = pAGDoc->GetDocType ();
				fprintf (output, "%s, %s, %s\n", szFName,
				  (char *) ((DocType == DOC_CARDHV || DocType == DOC_CARDHH) ? "s" : "q"),
				  (char *) ((DocType == DOC_CARDHV || DocType == DOC_CARDFV) ? "p" : "l"));

				ProgDlg.StepIt ();
				if (ProgDlg.CheckCancelButton ())
					break;
			}
			delete pAGDoc;
		}

		fclose (output);
	}
	free (FileDlg.m_ofn.lpstrFile);
}

void CMainFrame::OnPrintOptions() 
{
	m_nPrintOption = GetCurrentMessage ()->wParam;
}

void CMainFrame::OnUpdatePrintBoth(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio (m_nPrintOption == ID_PRINTBOTH);
}

void CMainFrame::OnUpdatePrintDefault(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio (m_nPrintOption == ID_PRINTDEFAULT);
}

void CMainFrame::OnUpdatePrintQuarter(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio (m_nPrintOption == ID_PRINTQUARTER);
}

void CMainFrame::OnUpdatePrintSingle(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio (m_nPrintOption == ID_PRINTSINGLE);
}


void CMainFrame::CreateThumb (const CAGPage *pPage, int nMaxSize,
  const char *pszFileName)
{
	SIZE sizePage;
	pPage->GetPageSize (&sizePage);

	double xScale = (double) nMaxSize / (double) sizePage.cx;
	double yScale = (double) nMaxSize / (double) sizePage.cy;
	if (xScale < yScale)
		yScale = xScale;
	else
		xScale = yScale;

	BITMAPINFO	bi;
	memset (&bi, 0, sizeof (bi));
	bi.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = (int) ((sizePage.cx * xScale) + 0.5);
	bi.bmiHeader.biHeight = (int) ((sizePage.cy * yScale) + 0.5);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biXPelsPerMeter = 3938;
	bi.bmiHeader.biYPelsPerMeter = 3938;

	BYTE *pBits = NULL;
	CAGDIBSectionDC dc (&bi, DIB_RGB_COLORS, &pBits);

	RECT DestRect = {0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight};
	::FillRect (dc.GetHDC (), &DestRect, (HBRUSH) ::GetStockObject (WHITE_BRUSH));

	CAGMatrix Matrix (xScale, 0, 0, yScale);
	dc.SetDeviceMatrix (Matrix);
	pPage->Draw (dc);


	BITMAPINFOHEADER biNew = bi.bmiHeader;
	biNew.biWidth /= 2;
	biNew.biHeight /= 2;
	biNew.biSizeImage = 0;

	BITMAPINFOHEADER *pdibDest = (BITMAPINFOHEADER *) GlobalAllocPtr (GHND, DibSize (&biNew));
	if (pdibDest)
	{
		*pdibDest = biNew;	
	
		Image dst, src;
		src.xsize = (int) bi.bmiHeader.biWidth;
		src.ysize = (int) bi.bmiHeader.biHeight;
		src.data = pBits;
		src.span = DibWidthBytes (&bi.bmiHeader);
		dst.xsize = (int) pdibDest->biWidth;
		dst.ysize = (int) pdibDest->biHeight;
		dst.data = (BYTE *) DibPtr(pdibDest);
		dst.span = DibWidthBytes (pdibDest);

		zoom (&dst, &src);

		WriteBMP (pszFileName, (BITMAPINFO *)pdibDest, DibPtr (pdibDest));

		GlobalFreePtr (pdibDest);
	}

//	WriteBMP (pszFileName, &bi, pBits);
}

void CMainFrame::WriteBMP (const char *pszFileName, const BITMAPINFO *pbmi,
  const BYTE *pBits)
{
	int nImageSize = DibSizeImage (&pbmi->bmiHeader);

	BITMAPFILEHEADER bf;
	memset (&bf, 0, sizeof (bf));
	bf.bfType = 0x4d42;	/* 'BM' */
	bf.bfSize = sizeof (bf) + pbmi->bmiHeader.biSize + nImageSize;
	bf.bfOffBits = sizeof (bf) + pbmi->bmiHeader.biSize;

	FILE *output = fopen (pszFileName, "wb");
	if (output)
	{
		fwrite (&bf, sizeof (bf), 1, output);
		fwrite (&pbmi->bmiHeader, pbmi->bmiHeader.biSize, 1, output);
		fwrite (pBits, nImageSize, 1, output);
		fclose (output);
	}
}


//==========================================================================//
// The following code is from Filtered Image Rescaling by Dale Schumacher	//
// in Graphics Gems III														//
//==========================================================================//
#define M_PI			3.141592
#define	WHITE_PIXEL		(255)
#define	BLACK_PIXEL		(0)
#define CLAMP(v,l,h)	((v)<(l) ? (l) : (v) > (h) ? (h) : v)

typedef	struct {
	BYTE b;
	BYTE g;
	BYTE r;
} Pixel;

typedef struct {
	int	pixel;
	double	weight;
} CONTRIB;

typedef struct {
	int	n;		/* number of contributors */
	CONTRIB	*p;		/* pointer to list of contributions */
} CLIST;

CLIST	*contrib;		/* array of contribution lists */

void get_row(Pixel *row, Image *image, int y)
{
	if((y < 0) || (y >= image->ysize)) {
		return;
	}
	hmemcpy(row,
		image->data + ((long) y * image->span),
		(sizeof(Pixel) * image->xsize));
}

void get_column(Pixel *column, Image *image, int x)
{
	int i, d;
	BYTE *p;

	if((x < 0) || (x >= image->xsize)) {
		return;
	}
	d = image->span;
	for(i = image->ysize, p = image->data + (x * sizeof (Pixel)); i-- > 0; p += d) {
		column->r = p[2];
		column->g = p[1];
		column->b = p[0];
		column++;
	}
}

void put_pixel(Image *image, int x, int y, Pixel *data)
{
	static Image *im = NULL;
	static int yy = -1;
	static BYTE *p = NULL;

	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) {
		return;
	}
	if((im != image) || (yy != y)) {
		im = image;
		yy = y;
		p = image->data + ((long) y * image->span);
	}
	BYTE *pTemp = p + (x * sizeof (Pixel));
	*pTemp++ = data->b;
	*pTemp++ = data->g;
	*pTemp = data->r;
}

inline double sinc(double x)
{
	x *= M_PI;
	if(x != 0) return(sin(x) / x);
	return(1.0);
}

#define	Lanczos3_support	(3.0)
inline double Lanczos3_filter(double t)
{
	if(t < 0) t = -t;
	if(t < 3.0) return(sinc(t) * sinc(t/3.0));
	return(0.0);
}

void zoom (Image *dst, Image *src)
{
	Image tmp;			/* intermediate image */
	double xscale, yscale;		/* zoom scale factors */
	int i, j, k;			/* loop variables */
	int n;				/* pixel number */
	double center, left, right;	/* filter calculation variables */
	double width, fscale, weight;	/* filter calculation variables */
	Pixel *raster;			/* a row or column of pixels */

	/* create intermediate image to hold horizontal zoom */
	tmp.xsize = dst->xsize;
	tmp.ysize = src->ysize;
	tmp.data = (BYTE *) GlobalAllocPtr (GHND,
	  (long) tmp.xsize * tmp.ysize * sizeof (Pixel));
	if (tmp.data == NULL)
		return;
	tmp.span = tmp.xsize * sizeof (Pixel);

	xscale = (double) dst->xsize / (double) src->xsize;
	yscale = (double) dst->ysize / (double) src->ysize;

	/* pre-calculate filter contributions for a row */
	contrib = (CLIST *)GlobalAllocPtr (GHND, dst->xsize * sizeof(CLIST));
	if(xscale < 1.0) {
		width = Lanczos3_support / xscale;
		fscale = 1.0 / xscale;
		for(i = 0; i < dst->xsize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)GlobalAllocPtr (GHND,
			  (long) (width * 2 + 1) * sizeof(CONTRIB));
			center = (double) i / xscale;
			left = ceil(center - width);
			right = floor(center + width);
			for(j = (int) left; j <= (int) right; ++j) {
				weight = center - (double) j;
				weight = Lanczos3_filter(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= src->xsize) {
					n = (src->xsize - j) + src->xsize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->xsize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)GlobalAllocPtr (GHND,
			  (Lanczos3_support * 2 + 1) * sizeof(CONTRIB));
			center = (double) i / xscale;
			left = ceil(center - Lanczos3_support);
			right = floor(center + Lanczos3_support);
			for(j = (int) left; j <= (int) right; ++j) {
				weight = center - (double) j;
				weight = Lanczos3_filter(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= src->xsize) {
					n = (src->xsize - j) + src->xsize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom horizontally from src to tmp */
	raster = (Pixel *)GlobalAllocPtr (GHND, src->xsize * sizeof(Pixel));
	for(k = 0; k < tmp.ysize; ++k) {
		get_row(raster, src, k);
		for(i = 0; i < tmp.xsize; ++i) {
			double rweight = 0.0;
			double gweight = 0.0;
			double bweight = 0.0;
			for(j = 0; j < contrib[i].n; ++j) {
				rweight += raster[contrib[i].p[j].pixel].r
					* contrib[i].p[j].weight;
				gweight += raster[contrib[i].p[j].pixel].g
					* contrib[i].p[j].weight;
				bweight += raster[contrib[i].p[j].pixel].b
					* contrib[i].p[j].weight;
			}
			Pixel p;
			p.r = (BYTE) CLAMP(rweight, BLACK_PIXEL, WHITE_PIXEL);
			p.g = (BYTE) CLAMP(gweight, BLACK_PIXEL, WHITE_PIXEL);
			p.b = (BYTE) CLAMP(bweight, BLACK_PIXEL, WHITE_PIXEL);
			// Since this output is destined for the printer, force to white
			if (p.r > 250 && p.g > 250 && p.b > 250)
				p.r = p.g = p.b = 255;
			put_pixel(&tmp, i, k, &p);
		}
	}
	GlobalFreePtr (raster);

	/* free the memory allocated for horizontal filter weights */
	for(i = 0; i < tmp.xsize; ++i) {
		GlobalFreePtr (contrib[i].p);
	}
	GlobalFreePtr (contrib);

	/* pre-calculate filter contributions for a column */
	contrib = (CLIST *)GlobalAllocPtr (GHND, dst->ysize * sizeof(CLIST));
	if(yscale < 1.0) {
		width = Lanczos3_support / yscale;
		fscale = 1.0 / yscale;
		for(i = 0; i < dst->ysize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)GlobalAllocPtr (GHND,
			  (long) (width * 2 + 1) * sizeof(CONTRIB));
			center = (double) i / yscale;
			left = ceil(center - width);
			right = floor(center + width);
			for(j = (int) left; j <= (int) right; ++j) {
				weight = center - (double) j;
				weight = Lanczos3_filter(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= tmp.ysize) {
					n = (tmp.ysize - j) + tmp.ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->ysize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)GlobalAllocPtr (GHND,
			  (Lanczos3_support * 2 + 1) * sizeof(CONTRIB));
			center = (double) i / yscale;
			left = ceil(center - Lanczos3_support);
			right = floor(center + Lanczos3_support);
			for(j = (int) left; j <= (int) right; ++j) {
				weight = center - (double) j;
				weight = Lanczos3_filter(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= tmp.ysize) {
					n = (tmp.ysize - j) + tmp.ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom vertically from tmp to dst */
	raster = (Pixel *)GlobalAllocPtr (GHND, tmp.ysize * sizeof(Pixel));
	for(k = 0; k < dst->xsize; ++k) {
		get_column(raster, &tmp, k);
		for(i = 0; i < dst->ysize; ++i) {
			double rweight = 0.0;
			double gweight = 0.0;
			double bweight = 0.0;
			for(j = 0; j < contrib[i].n; ++j) {
				rweight += raster[contrib[i].p[j].pixel].r
					* contrib[i].p[j].weight;
				gweight += raster[contrib[i].p[j].pixel].g
					* contrib[i].p[j].weight;
				bweight += raster[contrib[i].p[j].pixel].b
					* contrib[i].p[j].weight;
			}
			Pixel p;
			p.r = (BYTE) CLAMP(rweight, BLACK_PIXEL, WHITE_PIXEL);
			p.g = (BYTE) CLAMP(gweight, BLACK_PIXEL, WHITE_PIXEL);
			p.b = (BYTE) CLAMP(bweight, BLACK_PIXEL, WHITE_PIXEL);
			// Since this output is destined for the printer, force to white
			if (p.r > 250 && p.g > 250 && p.b > 250)
				p.r = p.g = p.b = 255;
			put_pixel(dst, k, i, &p);
		}
	}
	GlobalFreePtr (raster);

	/* free the memory allocated for vertical filter weights */
	for(i = 0; i < dst->ysize; ++i) {
		GlobalFreePtr (contrib[i].p);
	}
	GlobalFreePtr (contrib);

	GlobalFreePtr (tmp.data);
}
