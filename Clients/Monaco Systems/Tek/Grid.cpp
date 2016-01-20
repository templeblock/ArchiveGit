// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

#include "stdafx.h"
#include "grid.h"
//j #include "colortran.h"

//j Temporary, until API is plugged in
#define labtontscrgb(lab, rgb) { rgb[0] = (BYTE)lab[0]; rgb[1] = (BYTE)lab[1]; rgb[2] = (BYTE)lab[2]; }

/////////////////////////////////////////////////////////////////////////////
// CGrid

IMPLEMENT_DYNCREATE(CGrid, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CGrid properties

/////////////////////////////////////////////////////////////////////////////
// CGrid extended operations

/////////////////////////////////////////////////////////////////////////////
void CGrid::InitCMYK(int nRows)	//number of rows for data
{
	// Set some default values
	SetCols(5);
	SetRows(1 + nRows);
	SetFixedCols(1);
	SetFixedRows(1);
	SetWordWrap(FALSE);
	SetScrollTrack(TRUE);
	SetScrollBars(2/*0=none,1=horz,2=vert,3=both*/);
	SetSelectionMode(0/*0=free,1=row,2=col*/);
	SetAllowBigSelection(TRUE);
	SetAllowUserResizing(0/*0=none,1=col,2=row,3=both*/);
	SetFillStyle(1/*0=single,1=repeat*/);
	SetFocusRect(0/*0=None, 1=Light, 2=Heavy*/);

	// Get the screen resolution
	long lResWidth = 96; // default
	CWnd* hWnd = CWnd::GetDesktopWindow();
	if ( hWnd )
	{
		CDC* hDC = hWnd->GetDC();
		if ( hDC )
		{
			lResWidth = hDC->GetDeviceCaps( LOGPIXELSX );
			hWnd->ReleaseDC(hDC);
		}
	}

	// Get the control's rectangle (in pixels)
	CRect rect;
	GetClientRect(&rect);

	// Get the control width in pixels
	long lFullWidth = rect.Width();

	// Calculate the Twips conversion (Twips = pixels * 1440/res)
	long lTwips = (1440 + lResWidth/2) / lResWidth;

	// Columns 1-4 are 3/14ths of the space
	long lWidth = ((3 * lFullWidth) + 7) / 14;
	for (int c=1; c<=4; c++)
	{
		SetColWidth(c, lWidth * lTwips);
		lFullWidth -= lWidth; // The remaining space
	}

	// Column 0 is the remaining space (1/7th)
	SetColWidth(0, lFullWidth * lTwips);

	SetRow(0);
	SetCol(1);
	SetText("Cyan");
	SetCellBackColor( RGB(0,255,255) );

	SetCol(2);
	SetText("Magenta");
	SetCellBackColor( RGB(255,0,255) );

	SetCol(3);
	SetText("Yellow");
	SetCellBackColor( RGB(255,255,0) );

	SetCol(4);
	SetText("Black");
	SetCellBackColor( RGB(1,0,0) );
	SetCellForeColor( RGB(255,255,255) );

	int step;
	CString gz("");
	step = 100/(nRows - 1);
	for (int row=1; row<=nRows; row++)
	{
		CString sz;
		sz.Format( "%d%%", (row-1)*step );
		SetTextMatrix(row, 0/*col*/, sz);
		SetTextMatrix(row, 1, gz);
		SetTextMatrix(row, 2, gz);
		SetTextMatrix(row, 3, gz);
		SetTextMatrix(row, 4, gz);
	}

	SetCol(1);
	SetRow(1);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::InitLAB(int nRows, char* rgb)
{
	// Set some default values
	SetCols(5);
	SetRows(1 + nRows);
	SetFixedCols(2);
	SetFixedRows(1);
	SetWordWrap(FALSE);
	SetScrollTrack(TRUE);
	SetScrollBars(2/*0=none,1=horz,2=vert,3=both*/);
	SetSelectionMode(0/*0=free,1=row,2=col*/);
	SetAllowBigSelection(TRUE);
	SetAllowUserResizing(0/*0=none,1=col,2=row,3=both*/);
	SetFillStyle(1/*0=single,1=repeat*/);
	SetFocusRect(0/*0=None, 1=Light, 2=Heavy*/);

	// Get the screen resolution
	long lResWidth = 96; // default
	CWnd* hWnd = CWnd::GetDesktopWindow();
	if ( hWnd )
	{
		CDC* hDC = hWnd->GetDC();
		if ( hDC )
		{
			lResWidth = hDC->GetDeviceCaps( LOGPIXELSX );
			hWnd->ReleaseDC(hDC);
		}
	}

	// Get the control's rectangle (in pixels)
	CRect rect;
	GetClientRect(&rect);

	// Get the control width in pixels
	long lFullWidth = rect.Width();

	// Calculate the Twips conversion (Twips = pixels * 1440/res)
	long lTwips = (1440 + lResWidth/2) / lResWidth;

	// Columns 2-4 are 3/14ths of the space
	//long lWidth = ((3 * lFullWidth) + 7) / 14;
	long lWidth = ((2*lFullWidth) + 5) / 10;
	for (int c=2; c<=4; c++)
	{
		SetColWidth(c, lWidth * lTwips);
		lFullWidth -= lWidth; // The remaining space
	}

	// Column 0 is 1/2 remaining space (1/14th)
	lWidth = lFullWidth / 2;	
	SetColWidth(0, lWidth * lTwips);

	// Column 1 is 1/2 remaining space (1/14th)
	lWidth = lFullWidth - lWidth;
	SetColWidth(1, lWidth * lTwips);

	SetRow(0);
	SetCol(0);
	SetText("Reference");

	SetCol(1);
	SetText("Actual");

	SetCol(2);
	SetText("L");
	//SetCellBackColor( RGB(255,0,0) );

	SetCol(3);
	SetText("a");
	//SetCellBackColor( RGB(0,255,0) );

	SetCol(4);
	SetText("b");
	//SetCellBackColor( RGB(0,0,255) );

	InitLABRows(1 + nRows);
	InitCMYKRowLabels(0, -1, -1, rgb); 
	InitLABRowLabels(-1, -1);

	SetCol(2);
	SetRow(1);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::InitCMYKRowLabels(int Col, int iFirstRow, int iLastRow, char* rgb)
{
	long lColor;

	BOOL bAllRows = (iFirstRow < 0 || iLastRow < 0);
	if (bAllRows)
	{
		iFirstRow = GetFixedRows();
		iLastRow = GetRows() - 1;
	}
	else
	{
		iFirstRow += GetFixedRows();
		iLastRow += GetFixedRows();
	}

	CRect rSaved(GetCol(), GetRow(), GetColSel(), GetRowSel() );

//	char myr[10], myg[10], myb[10];

	for (int row = iFirstRow; row <= iLastRow; row++)
	{
		if(rgb)
		{
			lColor = RGB(rgb[0], rgb[1], rgb[2]);
/*
	sprintf( myr, "%d" ,rgb[0]);
	sprintf( myg, "%d" ,rgb[1]);
	sprintf( myb, "%d", rgb[2]);
	AfxMessageBox(myr, MB_OK|MB_ICONSTOP);
	AfxMessageBox(myg, MB_OK|MB_ICONSTOP); 
	AfxMessageBox(myb, MB_OK|MB_ICONSTOP); 
*/
		}
		else
			lColor = RGB(0, 0, 0);

		SetRow(row);

		SetCol(Col);
		SetCellBackColor( lColor );
		if(rgb) rgb += 3;
	}

	SetCol(rSaved.left);
	SetRow(rSaved.top);
	SetColSel(rSaved.right);
	SetRowSel(rSaved.bottom);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::InitLABRows(int nRows)
{
	int iFirstRow = GetFixedRows();

	if (nRows > 4000) // A safety measure
		nRows = iFirstRow+1;
	if (nRows < iFirstRow+1) // A safety measure
		nRows = iFirstRow+1;

	if (GetRows() == nRows)
		return;

	SetRows(nRows);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::InitLABRowLabels(int iFirstRow, int iLastRow)
{
	BOOL bAllRows = (iFirstRow < 0 || iLastRow < 0);
	if (bAllRows)
	{
		iFirstRow = GetFixedRows();
		iLastRow = GetRows() - 1;
	}
	else
	{
		iFirstRow += GetFixedRows();
		iLastRow += GetFixedRows();
	}

	if (iLastRow < iFirstRow)
		return;

	CRect rSaved(GetCol(), GetRow(), GetColSel(), GetRowSel() );

	int iFirstCol = GetFixedCols();
	for (int row = iFirstRow; row <= iLastRow; row++)
	{
		double Lab[3];
		CString sz;
		sz = GetTextMatrix(row, 0 + iFirstCol); Lab[0] = atof(sz);
		sz = GetTextMatrix(row, 1 + iFirstCol); Lab[1] = atof(sz);
		sz = GetTextMatrix(row, 2 + iFirstCol); Lab[2] = atof(sz);

		unsigned char rgb1[3];
		labtontscrgb(Lab, rgb1);
		if (!(rgb1[0] + rgb1[1] + rgb1[2]) ) rgb1[0] = 1; // Adjust slightly if pure black
		long lColor1 = RGB(rgb1[0], rgb1[1], rgb1[2]);

		SetRow(row);

		SetCol(1);
		SetCellBackColor( lColor1 );
	}

	SetCol(rSaved.left);
	SetRow(rSaved.top);
	SetColSel(rSaved.right);
	SetRowSel(rSaved.bottom);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::GoHome(void)
{
	SetRow(GetFixedRows());
	SetCol(GetFixedCols());
}

CRect gSavedRect(0,0,0,0);

/////////////////////////////////////////////////////////////////////////////
void CGrid::ProcessClick(void)
{
	gSavedRect.SetRect(0,0,0,0);
}

/////////////////////////////////////////////////////////////////////////////
bool CGrid::ProcessKey(short* pKeyCode, short Shift, CRect* pRect)
{
	if (!pKeyCode)
		return false;

	short KeyCode = *pKeyCode;
	CRect rect(GetCol(), GetRow(), GetColSel(), GetRowSel() );

	// If multiple cell are selected, don't allow anything but VK_DELETE
	bool bSingleCell = (rect.left == rect.right && rect.top == rect.bottom);
	if ( KeyCode != VK_DELETE && !bSingleCell )
		return false;

	CString sz;
	int bSelectRectChanged = (gSavedRect != rect);
	if (bSelectRectChanged)
	{
		gSavedRect = rect;
		sz = "";
	}
	else
		sz = GetText();

	if ( KeyCode >= VK_NUMPAD0 && KeyCode <= VK_NUMPAD9 )
		KeyCode -= (VK_NUMPAD0 - '0');

	if ( KeyCode >= '0' && KeyCode <= '9' )
	{
		sz += (TCHAR)KeyCode;
	}
	else
	if ( KeyCode == 189/*'-'*/ || KeyCode == VK_SUBTRACT )
	{
		if ( sz.IsEmpty() )
			sz += '-';
	}
	else
	if ( KeyCode == 190/*'.'*/ || KeyCode == VK_DECIMAL )
	{
		if ( sz.Find('.') < 0 )
			sz += '.';
	}
	else
	if ( KeyCode == VK_DELETE )
		sz = "";
	else
	if ( KeyCode == VK_BACK )
	{
		int i = sz.GetLength();
		if ( i )
			sz = sz.Left( i-1 );
	}
	else // invalid key code
		return false; // to see key codes, use sz.Format( "+%d", KeyCode );

	SetText(sz);

	if (pRect)
	{ // pass back the selection rectangle
		*pRect = gSavedRect;

		// Put the points in order
		if ((pRect->left > pRect->right) && (pRect->top  > pRect->bottom))
			pRect->SetRect( pRect->right, pRect->bottom, pRect->left, pRect->top );

		// Don't count the fixed rows and columns
		pRect->left   -= GetFixedCols();
		pRect->right  -= GetFixedCols();
		pRect->top    -= GetFixedRows();
		pRect->bottom -= GetFixedRows();
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::OutputCMYK( ofstream& out )
{
	int iFirstRow = GetFixedRows();
	int iLastRow = GetRows() - 1;

	// Copy non-zero values to the arrays
	for (int iRow = iFirstRow; iRow <= iLastRow; iRow++ )
	{
		CString szColumn1;
		CString szColumn2;
		CString szColumn3;
		CString szColumn4;
		szColumn1 = GetTextMatrix(iRow, 0 + GetFixedCols());
		szColumn2 = GetTextMatrix(iRow, 1 + GetFixedCols());
		szColumn3 = GetTextMatrix(iRow, 2 + GetFixedCols());
		szColumn4 = GetTextMatrix(iRow, 3 + GetFixedCols());

		if (iRow == iFirstRow)
		{
			if (szColumn1 == "") szColumn1 = "0";
			if (szColumn2 == "") szColumn2 = "0";
			if (szColumn3 == "") szColumn3 = "0";
			if (szColumn4 == "") szColumn4 = "0";
		}
		else
		if (iRow == iLastRow)
		{
			if (szColumn1 == "") szColumn1 = "100";
			if (szColumn2 == "") szColumn2 = "100";
			if (szColumn3 == "") szColumn3 = "100";
			if (szColumn4 == "") szColumn4 = "100";
		}

		BOOL bRowHasData = (szColumn1 != "" || szColumn2 != "" || szColumn3 != "" || szColumn4 != "" );
		if (bRowHasData)
		{
			CString sz;
			sz.Format("%8d", iRow-iFirstRow); out << sz; out << ',';
			sz.Format("%8s", szColumn1);      out << sz; out << ',';
			sz.Format("%8s", szColumn2);      out << sz; out << ',';
			sz.Format("%8s", szColumn3);      out << sz; out << ',';
			sz.Format("%8s", szColumn4);      out << sz; out << ',';
			out << '\n';
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CGrid::GetColumn(int iColumn, double* pdXArray, double* pdYArray, int iArraySize, double dMaxValue)
{
	int iFirstRow = GetFixedRows();
	int iLastRow = GetRows() - 1;
	int iCount = 0;
	int	rowDiff = GetRows() - GetFixedRows() - 1;

	// Copy non-zero values to the arrays
	for (int iRow = iFirstRow; iRow <= iLastRow; iRow++ )
	{
		if (iCount >= iArraySize)
			break;
		CString sz;
		sz = GetTextMatrix(iRow, iColumn + GetFixedCols());
		if (sz != "" || (iRow == iFirstRow) || (iRow == iLastRow))
		{
			double dValue = atof(sz);
			if (iRow == iLastRow & dValue == 0.0)
				dValue = dMaxValue;
			pdYArray[iCount] = dValue / dMaxValue;
			//pdXArray[iCount] = ((double)(iRow - iFirstRow)) / 100.0;
			pdXArray[iCount] = ((double)(iRow - iFirstRow)) / (double)rowDiff;
			iCount++;
		}
	}

	return iCount;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetColumn(int iColumn, double* pdXArray, double* pdYArray, int iArraySize, double dMaxValue)
{
	int iFirstRow = GetFixedRows();
	int iLastRow = GetRows() - 1;
	iColumn += GetFixedCols();
	int	rowDiff = GetRows() - GetFixedRows() - 1;

	// Clear the column completely
	for (int iRow = iFirstRow; iRow <= iLastRow; iRow++ )
		SetTextMatrix(iRow, iColumn, "");

	// Set the values from the arrays
	for (int i = 0; i < iArraySize; i++ )
	{
		double d = pdXArray[i];
		int iRow = iFirstRow + (int)((d * rowDiff) + (d>0 ? 0.5 : -0.5));
		if (iRow < iFirstRow || iRow > iLastRow )
			continue;

		CString sz;
		// "%g" means trailing 0's are truncated, and the decimal point is dropped if no fraction
		sz.Format( "%g", (pdYArray[i] * dMaxValue) );
		SetTextMatrix(iRow, iColumn, sz);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGrid operations

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetRows()
{
	long result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRows(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetCols()
{
	long result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCols(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetFixedRows()
{
	long result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetFixedRows(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetFixedCols()
{
	long result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetFixedCols(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
short CGrid::GetVersion()
{
	short result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
CString CGrid::GetFormatString()
{
	CString result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetFormatString(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetTopRow()
{
	long result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetTopRow(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetLeftCol()
{
	long result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetLeftCol(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetRow()
{
	long result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRow(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetCol()
{
	long result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCol(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetRowSel()
{
	long result;
	InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRowSel(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xc, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetColSel()
{
	long result;
	InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetColSel(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
CString CGrid::GetText()
{
	CString result;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetText(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetBackColor()
{
	unsigned long result;
	InvokeHelper(DISPID_BACKCOLOR, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetBackColor(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(DISPID_BACKCOLOR, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetForeColor()
{
	unsigned long result;
	InvokeHelper(DISPID_FORECOLOR, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetForeColor(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(DISPID_FORECOLOR, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetBackColorFixed()
{
	unsigned long result;
	InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetBackColorFixed(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xe, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetForeColorFixed()
{
	unsigned long result;
	InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetForeColorFixed(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetBackColorSel()
{
	unsigned long result;
	InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetBackColorSel(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x10, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetForeColorSel()
{
	unsigned long result;
	InvokeHelper(0x11, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetForeColorSel(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x11, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetBackColorBkg()
{
	unsigned long result;
	InvokeHelper(0x12, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetBackColorBkg(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x12, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetWordWrap()
{
	BOOL result;
	InvokeHelper(0x13, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetWordWrap(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x13, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRefFont(LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(DISPID_FONT, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
float CGrid::GetFontWidth()
{
	float result;
	InvokeHelper(0x54, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetFontWidth(float newValue)
{
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x54, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
CString CGrid::GetCellFontName()
{
	CString result;
	InvokeHelper(0x4d, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellFontName(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x4d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

/////////////////////////////////////////////////////////////////////////////
float CGrid::GetCellFontSize()
{
	float result;
	InvokeHelper(0x4e, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellFontSize(float newValue)
{
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x4e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetCellFontBold()
{
	BOOL result;
	InvokeHelper(0x4f, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellFontBold(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x4f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetCellFontItalic()
{
	BOOL result;
	InvokeHelper(0x50, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellFontItalic(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x50, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetCellFontUnderline()
{
	BOOL result;
	InvokeHelper(0x51, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellFontUnderline(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x51, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetCellFontStrikeThrough()
{
	BOOL result;
	InvokeHelper(0x52, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellFontStrikeThrough(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x52, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
float CGrid::GetCellFontWidth()
{
	float result;
	InvokeHelper(0x53, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellFontWidth(float newValue)
{
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x53, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetTextStyle()
{
	long result;
	InvokeHelper(0x14, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetTextStyle(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x14, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetTextStyleFixed()
{
	long result;
	InvokeHelper(0x15, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetTextStyleFixed(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x15, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetScrollTrack()
{
	BOOL result;
	InvokeHelper(0x16, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetScrollTrack(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x16, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetFocusRect()
{
	long result;
	InvokeHelper(0x17, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetFocusRect(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x17, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetHighLight()
{
	long result;
	InvokeHelper(0x18, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetHighLight(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x18, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetRedraw()
{
	BOOL result;
	InvokeHelper(0x19, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRedraw(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x19, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetScrollBars()
{
	long result;
	InvokeHelper(0x1a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetScrollBars(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetMouseRow()
{
	long result;
	InvokeHelper(0x1b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetMouseCol()
{
	long result;
	InvokeHelper(0x1c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetCellLeft()
{
	long result;
	InvokeHelper(0x1d, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetCellTop()
{
	long result;
	InvokeHelper(0x1e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetCellWidth()
{
	long result;
	InvokeHelper(0x1f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetCellHeight()
{
	long result;
	InvokeHelper(0x20, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetRowHeightMin()
{
	long result;
	InvokeHelper(0x21, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRowHeightMin(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x21, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetFillStyle()
{
	long result;
	InvokeHelper(0xfffffe01, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetFillStyle(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xfffffe01, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetGridLines()
{
	long result;
	InvokeHelper(0x22, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetGridLines(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x22, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetGridLinesFixed()
{
	long result;
	InvokeHelper(0x23, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetGridLinesFixed(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x23, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetGridColor()
{
	unsigned long result;
	InvokeHelper(0x24, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetGridColor(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x24, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetGridColorFixed()
{
	unsigned long result;
	InvokeHelper(0x25, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetGridColorFixed(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x25, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetCellBackColor()
{
	unsigned long result;
	InvokeHelper(0x26, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellBackColor(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x26, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
unsigned long CGrid::GetCellForeColor()
{
	unsigned long result;
	InvokeHelper(0x27, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellForeColor(unsigned long newValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x27, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
short CGrid::GetCellAlignment()
{
	short result;
	InvokeHelper(0x28, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellAlignment(short nNewValue)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x28, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetCellTextStyle()
{
	long result;
	InvokeHelper(0x29, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellTextStyle(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x29, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
short CGrid::GetCellPictureAlignment()
{
	short result;
	InvokeHelper(0x2b, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetCellPictureAlignment(short nNewValue)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x2b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
CString CGrid::GetClip()
{
	CString result;
	InvokeHelper(0x2d, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetClip(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetSort(short nNewValue)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x2e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetSelectionMode()
{
	long result;
	InvokeHelper(0x2f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetSelectionMode(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x2f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetMergeCells()
{
	long result;
	InvokeHelper(0x30, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetMergeCells(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x30, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetAllowBigSelection()
{
	BOOL result;
	InvokeHelper(0x33, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetAllowBigSelection(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x33, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetAllowUserResizing()
{
	long result;
	InvokeHelper(0x34, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetAllowUserResizing(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x34, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetBorderStyle()
{
	long result;
	InvokeHelper(DISPID_BORDERSTYLE, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetBorderStyle(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(DISPID_BORDERSTYLE, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetHWnd()
{
	long result;
	InvokeHelper(DISPID_HWND, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetEnabled()
{
	BOOL result;
	InvokeHelper(DISPID_ENABLED, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetEnabled(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(DISPID_ENABLED, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetAppearance()
{
	long result;
	InvokeHelper(DISPID_APPEARANCE, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetAppearance(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(DISPID_APPEARANCE, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetMousePointer()
{
	long result;
	InvokeHelper(0x35, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetMousePointer(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x35, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRefMouseIcon(LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x36, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetPictureType()
{
	long result;
	InvokeHelper(0x32, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetPictureType(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x32, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRefCellPicture(LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x2a, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms,
		 newValue);
}

/////////////////////////////////////////////////////////////////////////////
CString CGrid::GetTextArray(long index)
{
	CString result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x37, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetTextArray(long index, LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_BSTR;
	InvokeHelper(0x37, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, lpszNewValue);
}

/////////////////////////////////////////////////////////////////////////////
short CGrid::GetColAlignment(long index)
{
	short result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x38, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetColAlignment(long index, short nNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I2;
	InvokeHelper(0x38, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetColWidth(long index)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x39, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetColWidth(long index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x39, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetRowHeight(long index)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRowHeight(long index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x3a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetMergeRow(long index)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetMergeRow(long index, BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_BOOL;
	InvokeHelper(0x3b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetMergeCol(long index)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3c, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetMergeCol(long index, BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_BOOL;
	InvokeHelper(0x3c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRowPosition(long index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x3d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetColPosition(long index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x3e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetRowData(long index)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRowData(long index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x3f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetColData(long index)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x40, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetColData(long index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x40, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
CString CGrid::GetTextMatrix(long Row, long Col)
{
	CString result;
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x41, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms,
		Row, Col);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetTextMatrix(long Row, long Col, LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_BSTR;
	InvokeHelper(0x41, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Row, Col, lpszNewValue);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::AddItem(LPCTSTR Item, const VARIANT& index)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x42, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Item, &index);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::RemoveItem(long index)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x43, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 index);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::Clear()
{
	InvokeHelper(0x44, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::Refresh()
{
	InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetRowIsVisible(long index)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x55, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetColIsVisible(long index)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x56, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetRowPos(long index)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x57, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetColPos(long index)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x58, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
short CGrid::GetGridLineWidth()
{
	short result;
	InvokeHelper(0x59, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetGridLineWidth(short nNewValue)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x59, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
short CGrid::GetFixedAlignment(long index)
{
	short result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x5a, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms,
		index);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetFixedAlignment(long index, short nNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I2;
	InvokeHelper(0x5a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 index, nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGrid::GetRightToLeft()
{
	BOOL result;
	InvokeHelper(0xfffffd9d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetRightToLeft(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0xfffffd9d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

/////////////////////////////////////////////////////////////////////////////
long CGrid::GetOLEDropMode()
{
	long result;
	InvokeHelper(0x60f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetOLEDropMode(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x60f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::OLEDrag()
{
	InvokeHelper(0x610, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CGridEvents properties

/////////////////////////////////////////////////////////////////////////////
// CGridEvents operations

void CGridEvents::Click()
{
	InvokeHelper(DISPID_CLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CGridEvents::KeyDown(short* KeyCode, short Shift)
{
	static BYTE parms[] =
		VTS_PI2 VTS_I2;
	InvokeHelper(DISPID_KEYDOWN, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeyCode, Shift);
}

void CGridEvents::DblClick()
{
	InvokeHelper(DISPID_DBLCLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CGridEvents::KeyPress(short* KeyAscii)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(DISPID_KEYPRESS, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeyAscii);
}

void CGridEvents::KeyUp(short* KeyCode, short Shift)
{
	static BYTE parms[] =
		VTS_PI2 VTS_I2;
	InvokeHelper(DISPID_KEYUP, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeyCode, Shift);
}

void CGridEvents::MouseDown(short Button, short Shift, long x, long y)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_I4 VTS_I4;
	InvokeHelper(DISPID_MOUSEDOWN, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Button, Shift, x, y);
}

void CGridEvents::MouseMove(short Button, short Shift, long x, long y)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_I4 VTS_I4;
	InvokeHelper(DISPID_MOUSEMOVE, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Button, Shift, x, y);
}

void CGridEvents::MouseUp(short Button, short Shift, long x, long y)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_I4 VTS_I4;
	InvokeHelper(DISPID_MOUSEUP, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Button, Shift, x, y);
}

void CGridEvents::SelChange()
{
	InvokeHelper(0x45, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CGridEvents::RowColChange()
{
	InvokeHelper(0x46, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CGridEvents::EnterCell()
{
	InvokeHelper(0x47, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CGridEvents::LeaveCell()
{
	InvokeHelper(0x48, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CGridEvents::Scroll()
{
	InvokeHelper(0x49, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CGridEvents::Compare(long Row1, long Row2, short* Cmp)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_PI2;
	InvokeHelper(0x4a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Row1, Row2, Cmp);
}

void CGridEvents::OLEStartDrag(LPDISPATCH* Data, long* AllowedEffects)
{
	static BYTE parms[] =
		VTS_PDISPATCH VTS_PI4;
	InvokeHelper(0x60e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Data, AllowedEffects);
}

void CGridEvents::OLEGiveFeedback(long* Effect, BOOL* DefaultCursors)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PBOOL;
	InvokeHelper(0x60f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Effect, DefaultCursors);
}

void CGridEvents::OLESetData(LPDISPATCH* Data, short* DataFormat)
{
	static BYTE parms[] =
		VTS_PDISPATCH VTS_PI2;
	InvokeHelper(0x610, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Data, DataFormat);
}

void CGridEvents::OLECompleteDrag(long* Effect)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x611, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Effect);
}

void CGridEvents::OLEDragOver(LPDISPATCH* Data, long* Effect, short* Button, short* Shift, float* x, float* y, short* State)
{
	static BYTE parms[] =
		VTS_PDISPATCH VTS_PI4 VTS_PI2 VTS_PI2 VTS_PR4 VTS_PR4 VTS_PI2;
	InvokeHelper(0x612, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Data, Effect, Button, Shift, x, y, State);
}

void CGridEvents::OLEDragDrop(LPDISPATCH* Data, long* Effect, short* Button, short* Shift, float* x, float* y)
{
	static BYTE parms[] =
		VTS_PDISPATCH VTS_PI4 VTS_PI2 VTS_PI2 VTS_PR4 VTS_PR4;
	InvokeHelper(0x613, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Data, Effect, Button, Shift, x, y);
}
