#include "stdafx.h"
#include "Draw.h"
#include "QPathText.h"
#include "math.h"

#define DEFAULT_FONT_SIZE 24
#define MIN(x, y) ((x) < (y) ? (x) : (y))

const WCHAR* ImageTypes[] = {L"image/bmp", L"image/jpeg", L"image/gif", L"image/tiff", L"image/png"};
LinearGradientMode LinearModes[] = {LinearGradientModeHorizontal, LinearGradientModeVertical, LinearGradientModeForwardDiagonal, LinearGradientModeBackwardDiagonal};


//////////////////////////////////////////////////////////////////////
CStringDraw::CStringDraw()
:m_currFont(NULL),
 m_currFontFamily(NULL),
 m_currSweepMode(LinearGradientModeHorizontal),
 m_currTextGradClr1(NULL),
 m_currTextGradClr2(NULL),
 m_WhiteBkgdAlpha(255),
 m_currImageBkColor(NULL),
 m_currIndent(0),
 m_currRotation(100),
 m_currFontSize(DEFAULT_FONT_SIZE),
 m_currStrCurve(STR_BEZIER),
 m_logPixelsX(97),
 m_logPixelsY(97)
{
	m_currTextGradClr1 = new Color(255, 255,0,0);
	m_currTextGradClr2 = new Color(255, 0,0,255);
	m_currImageBkColor = new Color(Color::White);
	m_currImageSize.Height = 1.0;
	m_currImageSize.Width = 1.0;
	m_currLineSpacing = 0;
	m_currAvgCharWidth = 0;
	m_currCharSpacing = 0.0f;
}

////////////////////////////////////////////////////////////////////////
CStringDraw::~CStringDraw()
{
	if (m_currFontFamily)
		delete m_currFontFamily;

	if (m_currFont)
		delete m_currFont;

	if (m_currTextGradClr1)
		delete m_currTextGradClr1;

	if (m_currTextGradClr2)
		delete m_currTextGradClr2;

	if (m_currImageBkColor)
		delete m_currImageBkColor;

	m_currFontFamily = NULL;
	m_currFont = NULL;
	m_currTextGradClr1 = NULL;
	m_currTextGradClr2 = NULL;
	m_currImageBkColor = NULL;

	if (m_strList.GetCount() > 0)
	{
		StringData *strData = NULL;
		POSITION pos = m_strList.GetHeadPosition();
		while (pos)
		{
			strData = m_strList.GetNext(pos);
			delete strData;
		}
		m_strList.RemoveAll();
	}
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::Init(HWND hWnd)
{
	m_hWnd = hWnd;

	m_currFontSize = DEFAULT_FONT_SIZE;
	m_currFontFamily = new FontFamily(L"Arial");
	m_currFont = new Font(m_currFontFamily, (float)m_currFontSize, FontStyleRegular, UnitPixel);

	CRect prvRect;
	::GetClientRect(m_hWnd, &prvRect);
	prvRect.DeflateRect(1,1);

	m_windowRect.X = (float)prvRect.left;
	m_windowRect.Y = (float)prvRect.top;
	m_windowRect.Width = (float)prvRect.Width();
	m_windowRect.Height = (float)prvRect.Height();

	HDC hdc = GetDC(m_hWnd);

	int Rez = ::GetDeviceCaps(hdc, HORZRES);
	m_logPixelsX = ::GetDeviceCaps(hdc, LOGPIXELSX);
	m_logPixelsY = ::GetDeviceCaps(hdc, LOGPIXELSY);

	ReleaseDC(m_hWnd, hdc);

	m_currImageSize.Width = m_currImageSize.Width*m_logPixelsX;
	m_currImageSize.Height = m_currImageSize.Height*m_logPixelsY;

	m_currLineSpacing = 0;
	m_currAvgCharWidth = 0;
	m_currCharSpacing = 0.0f;
	m_currStrCurve = STR_BEZIER;
	m_currSweepMode = LinearGradientModeHorizontal;
	m_WhiteBkgdAlpha = 255;

	RectF imageRect(0, 0, m_currImageSize.Width, m_currImageSize.Height);

	float x = (float)(m_windowRect.X + m_windowRect.Width/2);
	float y = (float)(m_windowRect.Y + m_windowRect.Height/2);
	PointF ClCtrPt(x, y);

	x = (float)(imageRect.X + imageRect.Width/2);
	y = (float)(imageRect.Y + imageRect.Height/2);
	PointF ImgCtrPt(x, y);

	PointF Offset((ClCtrPt.X - ImgCtrPt.X), (ClCtrPt.Y - ImgCtrPt.Y));
	m_prvMatrix.Translate(Offset.X, Offset.Y);
}

////////////////////////////////////////////////////////////////////////
int CStringDraw::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetImageWidth(float width)
{
	m_currImageSize.Width = width*m_logPixelsX;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetImageHeight(float height)
{
	m_currImageSize.Height = height*m_logPixelsY;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetImageSize(const SizeF& size)
{
	m_currImageSize.Width = size.Width*m_logPixelsX;
	m_currImageSize.Height = size.Height*m_logPixelsY;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetCurveType(STR_TYPES type) 
{ 
	m_currStrCurve = type; 
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetFont(CString strFont)
{
	CComBSTR bstrName;
	m_currFontFamily->GetFamilyName(bstrName);
	CString FamilyName(bstrName);

	if (FamilyName != strFont)
	{
		delete m_currFont;
		delete m_currFontFamily;

		m_currFont = NULL;
		m_currFontFamily = NULL;

		USES_CONVERSION;
		m_currFontFamily = new FontFamily(T2W(strFont));
		m_currFont = new Font(m_currFontFamily, (float)m_currFontSize, FontStyleRegular, UnitPixel);
	}
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetFontSize(int size)
{
	m_currFontSize = abs(size);
	if (m_currFont)
		delete m_currFont;

	m_currFont = NULL;
	m_currFont = new Font(m_currFontFamily, (float)m_currFontSize, FontStyleRegular, UnitPixel);
}

//////////////////////////////////////////////////////////////////////// 
void CStringDraw::SetColorGradientMode(LINEAR_MODES sweepMode)
{
	m_currSweepMode = LinearModes[sweepMode];
}

//////////////////////////////////////////////////////////////////////// 
void CStringDraw::SetColor1(Color clr)
{
	if (m_currTextGradClr1)
	{
		delete m_currTextGradClr1;
		m_currTextGradClr1 = NULL;
	}
	m_currTextGradClr1 = new Color(clr.GetValue());
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetColor2(Color clr)
{
	if (m_currTextGradClr2)
	{
		delete m_currTextGradClr2;
		m_currTextGradClr2 = NULL;
	}
	m_currTextGradClr2 = new Color(clr.GetValue());
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetImageBkColor(Color clr)
{
	if (m_currImageBkColor)
	{
		delete m_currImageBkColor;
		m_currImageBkColor = NULL;
	}
	m_currImageBkColor = new Color(clr.GetValue());
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetWhiteBkgdTransparent(bool bTransparent)
{
	m_WhiteBkgdAlpha = bTransparent ? 0 : 255;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::IncrementLineHeight()
{
	m_currLineSpacing++;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::DecrementLineHeight()
{
	m_currLineSpacing--;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::IncrementCharWidth()
{
	m_currCharSpacing ++;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::DecrementCharWidth()
{
	m_currCharSpacing --;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::DeleteString(int nIndex)
{
	POSITION Pos = m_strList.FindIndex(nIndex);
	if (!Pos)
		return;
	
	StringData* strData = m_strList.GetAt(Pos);
	if (strData)
		delete strData;

	m_strList.RemoveAt(Pos);
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetString(CString str, int nIndex)
{
	StringData* strData;
	bool bStringPath = true;
	POSITION Pos = m_strList.FindIndex(nIndex);

	if (!Pos)
	{
		// new guidepath
		strData = new StringData();
		InitStringData(*strData, nIndex);
		bStringPath = SetStringPath(*strData);
		if (!bStringPath)
		{
			delete strData;
			strData = NULL;
		}
		else
			Pos = m_strList.AddTail(strData);
	}
	else
		strData = m_strList.GetAt(Pos);

	// set to dirty if string or font has changed
	if (!strData || !Pos)
		return;

	SetStringData(*strData, str);
	m_strList.SetAt(Pos, strData);
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::InitStringData(StringData& strData, int nIndex)
{
	strData.GuidePath.Reset();
	strData.TextPath.Reset();
	strData.bDirty = true;
	strData.Index = nIndex;

	float scaleFactor = 1.0F;
	if (NeedToScale(m_currImageSize))
	{
		SizeF ScaleFactors;
		GetScaleFactor(ScaleFactors);
		scaleFactor = MIN(ScaleFactors.Width, ScaleFactors.Height);
	}
	strData.fontSize = (int)(m_currFontSize*scaleFactor);

	int lineHeight = 0;
	int charWidth = 0;
	int avgCharWidth = 0;
	{
		LOGFONTA lf;
		Graphics g(m_hWnd);
		m_currFont->GetLogFontA(&g, &lf);
		lf.lfHeight = -1*abs(m_currFontSize);

		GetFontMetrics(&lf, lineHeight, charWidth, avgCharWidth);
	}
	strData.lineHeight = lineHeight;
	strData.charWidth = charWidth;
	strData.avgCharWidth = avgCharWidth;
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::SetStringPath(StringData& strData)
{
	bool bRetVal = true;

	switch (m_currStrCurve)
	{
		case STR_CIRCLE:
			{
				bRetVal = DoCircle(strData);
				break;
			}
		case STR_SEMICIRCLE:
			{
				bRetVal = DoSemiCircle(strData);
				break;
			}
		case STR_INVERTEDBEZIER:
			{
				bRetVal = DoInvBezier(strData);
				break;
			}
		case STR_INVERTEDSEMICIRCLE:
			{
				bRetVal = DoInvSemiCircle(strData);
				break;
			}
		case STR_ELLIPSE:
			{
				bRetVal = DoEllipse(strData);
				break;
			}
		case STR_VERTICALLINE:
			{
				bRetVal = DoVrtLine(strData);
				break;
			}
		case STR_HORIZONTALLINE:
			{
				bRetVal = DoHzLine(strData);
				break;
			}
		case STR_SPIRAL:
			{
				bRetVal = DoSpiral(strData);
				break;
			}
		case STR_BEZIER:
		default:
		{
			bRetVal = DoBezier(strData);
			break;
		}
	};

	strData.GuidePath.Transform(&m_prvMatrix);
	return bRetVal;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::GetFontMetrics(LOGFONTA* lf, int& lineHeight, int& charWidth, int& avgCharWidth)
{
	lineHeight = 0;
	charWidth = 0;
	HFONT hFont = ::CreateFontIndirect(lf);
	if (hFont)
	{
		HDC hDC = ::GetDC(NULL);
		if (hDC)
		{
			TEXTMETRIC tm;
			HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
			::GetTextMetrics(hDC, & tm);
			::SelectObject(hDC, hOldFont);
			::ReleaseDC(NULL, hDC);

			int nFontHeight = tm.tmAscent - tm.tmInternalLeading;
			int nLeading = tm.tmDescent + tm.tmInternalLeading + tm.tmExternalLeading;
			lineHeight = nFontHeight + nLeading;

			avgCharWidth = tm.tmAveCharWidth;
			charWidth = tm.tmMaxCharWidth/3;
		}
		::DeleteObject(hFont);
	}
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SetStringData(StringData& strData, CString str)
{
	LOGFONTA lf;
	{
		Graphics g(m_hWnd);
		m_currFont->GetLogFontA(&g, &lf);
		lf.lfHeight = -1*abs(strData.fontSize);
	}

	if (!strData.bDirty)
		strData.bDirty = (strData.str != str);
	strData.str = str;

	if ( m_currStrCurve == STR_VERTICALLINE )
	{
		DoVrtLine(strData);
		return;
	}

	strData.TextPath.Reset();
	QPathText pt(strData.GuidePath);
	pt.SetFont(lf);
	pt.SetRotation((REAL) m_currRotation / 100.0f);
	pt.SetSpacing(m_currCharSpacing);
	pt.GetPathText(strData.TextPath, strData.str, (REAL)m_currIndent);
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::NeedToScale(SizeF& newSize)
{
	return ( (newSize.Width > m_windowRect.Width) ||
			 (newSize.Height > m_windowRect.Height) );
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::GetScaleFactor(SizeF& scaleFactor)
{
	SizeF PrvWndSize((float)m_windowRect.Width, (float)m_windowRect.Height);
	scaleFactor.Width = (float)(PrvWndSize.Width/m_currImageSize.Width);
	scaleFactor.Height = (float)(PrvWndSize.Height/m_currImageSize.Height);
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::ScalePreviewImage()
{
	if (NeedToScale(m_currImageSize))
	{
		// Calculate the scale factor
		SizeF ScaleFactors;
		GetScaleFactor(ScaleFactors);
		float scaleFactor = MIN(ScaleFactors.Width, ScaleFactors.Height);

		// Now Scale original Image Rect
		m_prvMatrix.Scale(scaleFactor, scaleFactor);
	}
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::TranslatePreviewImage()
{
	float scaleFactor = 1.0F;
	RectF OrgImageRect(0, 0, m_currImageSize.Width, m_currImageSize.Height);
	RectF ScaledImageRect = OrgImageRect;

	if (NeedToScale(m_currImageSize))
	{
		SizeF ScaleFactors(1.0F, 1.0F);
		GetScaleFactor(ScaleFactors);
		scaleFactor = MIN(ScaleFactors.Width, ScaleFactors.Height);
		GetScaledRect(scaleFactor, OrgImageRect, ScaledImageRect);
	}

	float x = (float)(m_windowRect.X + m_windowRect.Width/2);
	float y = (float)(m_windowRect.Y + m_windowRect.Height/2);
	PointF ClCtrPt(x, y);

	x = (float)(ScaledImageRect.X + ScaledImageRect.Width/2);
	y = (float)(ScaledImageRect.Y + ScaledImageRect.Height/2);
	PointF ImgCtrPt(x, y);

	PointF Offset((float)(ClCtrPt.X - ImgCtrPt.X), (float)(ClCtrPt.Y - ImgCtrPt.Y));
	m_prvMatrix.Translate(Offset.X/scaleFactor, Offset.Y/scaleFactor);
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::GetScaledRect(float scaleFactor, RectF& orgImgRect, RectF& scaledImgRect)
{
	Matrix mMatrix;
	mMatrix.Scale(scaleFactor, scaleFactor);

	GraphicsPath gp;
	gp.AddRectangle(orgImgRect);
	gp.Transform(&mMatrix);

	int n = gp.GetPointCount();
	PointF *pts = new PointF[n];
	gp.GetPathPoints(pts, n);
	RectF NewImgRect(pts[0].X, pts[0].Y, pts[1].X-pts[0].X, pts[3].Y-pts[0].Y);
	scaledImgRect = NewImgRect;

	delete [] pts;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::RefreshPreviewWindow()
{
	m_prvMatrix.Reset();
	ScalePreviewImage();
	TranslatePreviewImage();

	// Apply it to all the string GP's 
	POSITION Pos = m_strList.GetHeadPosition();
	while (Pos)
	{
		StringData* strData = m_strList.GetNext(Pos);
		if (strData)
		{
			CString str = strData->str;
			InitStringData(*strData, strData->Index);
			SetStringPath(*strData);
			SetStringData(*strData, str);
		}
	}
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::CopyData(StringData& newData, StringData& oldData)
{
	newData.bDirty = oldData.bDirty;
	newData.Index = oldData.Index;
	newData.str = oldData.str;
	newData.fontSize = m_currFontSize;

	LOGFONTA lf;
	Graphics g(m_hWnd);
	m_currFont->GetLogFontA(&g, &lf);
	lf.lfHeight = -1*abs(m_currFontSize);
	GetFontMetrics(&lf, newData.lineHeight, newData.charWidth, newData.avgCharWidth);
}

////////////////////////////////////////////////////////////////////////
float CStringDraw::ExtraAllowance(SATYPES type, char ch, float lineHeight)
{
	float Allowance = 0.0F;
	CString Temp;

	if (type == PRE)
		Temp = " bdfhijkltABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

	if (type == POST)
		Temp = " gjpqyQ";

	if ( Temp.Find(ch) > 0 )
		Allowance = lineHeight * 0.2F;

	return Allowance;
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::DoDraw(HDC hDC)
{
	Graphics g(hDC);
	g.SetSmoothingMode(SmoothingModeHighQuality);

	// Always draw the Image First 
	SolidBrush BkgBrush(*m_currImageBkColor);
	RectF imageRect(0, 0, m_currImageSize.Width, m_currImageSize.Height);

	GraphicsPath Prvgp;
	Prvgp.AddRectangle(imageRect);
	Prvgp.Transform(&m_prvMatrix);
	g.FillPath(&BkgBrush, &Prvgp);

	POSITION Pos = m_strList.GetHeadPosition();
	while (Pos)
	{
		StringData* strData = m_strList.GetNext(Pos);
		if ( strData )
		{
			PointF pts[2];
			pts[0].X = strData->boundsRect.X;
			pts[0].Y = strData->boundsRect.Y;
			pts[1].X = strData->boundsRect.X + strData->boundsRect.Width;
			pts[1].Y = strData->boundsRect.Y + strData->boundsRect.Height;

			m_prvMatrix.TransformPoints(pts, 2);
			RectF boundsRect(pts[0].X, pts[0].Y, pts[1].X - pts[0].X, pts[1].Y - pts[0].Y);
			LinearGradientBrush brushText(boundsRect, *m_currTextGradClr1, *m_currTextGradClr2, m_currSweepMode);
			g.FillPath(& brushText, & strData->TextPath);
			strData->bDirty = false;
		}
	}

}

////////////////////////////////////////////////////////////////////////
bool IsWhite(Color& color)
{
	return ( (color.GetR() == 255) &&
			 (color.GetG() == 255) &&
			 (color.GetB() == 255));
}

////////////////////////////////////////////////////////////////////////
void CStringDraw::SaveImage(CString fileName, IMAGE_TYPES imgType)
{
	Bitmap myBitmap((int)m_currImageSize.Width, (int)m_currImageSize.Height);
	Graphics* g = Graphics::FromImage(&myBitmap);

	//if (*m_currImageBkColor-> == Color::White)
	if (IsWhite(*m_currImageBkColor))
	{
		Color bkColor(m_WhiteBkgdAlpha, 255, 255, 255);
		g->Clear(bkColor);
	}
	else
		g->Clear(*m_currImageBkColor);

	float matrixElements[6];
	m_prvMatrix.GetElements(matrixElements);
	m_prvMatrix.Reset();

	POSITION Pos = m_strList.GetHeadPosition();
	while (Pos)
	{
		StringData* strData = m_strList.GetNext(Pos);
		StringData newData;
		if ( strData )
		{
			CopyData(newData, *strData);
			SetStringPath(newData);
			SetStringData(newData, newData.str);

			LinearGradientBrush brushText(strData->boundsRect, *m_currTextGradClr1, *m_currTextGradClr2, m_currSweepMode);
			g->FillPath(& brushText, & newData.TextPath);
		}
	}

	m_prvMatrix.SetElements(matrixElements[0], matrixElements[1], matrixElements[2], matrixElements[3], matrixElements[4], matrixElements[5]);

	CComBSTR bstr(fileName);
	CLSID gifClsid;
	GetEncoderClsid(ImageTypes[imgType], &gifClsid);
	myBitmap.Save(bstr, &gifClsid);
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::DoEllipse(StringData& strData)
{
	if (m_currImageSize.Width == m_currImageSize.Height)
		return DoCircle(strData);

	strData.GuidePath.Reset();
	float lineHeight = (float)((strData.Index+1) * strData.lineHeight);

	RectF previewRect(0, 0, m_currImageSize.Width, m_currImageSize.Height);
	previewRect.Inflate(-1*lineHeight, -1*lineHeight);
	if ((previewRect.X >= previewRect.Width) || 
		(previewRect.Y >= previewRect.Height))
		return false;

	float right = previewRect.X + previewRect.Width;
	float bottom = previewRect.Y + previewRect.Height;
	float width = previewRect.Width;
	float height = previewRect.Height;

	strData.boundsRect = previewRect;
	strData.GuidePath.AddEllipse(right, bottom, -width, -height);
	return true;
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::DoCircle(StringData& strData)
{
	strData.GuidePath.Reset();

	// Set the preview Rect
	float width = (m_currImageSize.Width < m_currImageSize.Height) ? m_currImageSize.Width : m_currImageSize.Height;
	float lineHeight = (float)strData.lineHeight;
	lineHeight += m_currLineSpacing;
	lineHeight = (float)((strData.Index+1) * lineHeight);

	RectF previewRect(0, 0, width, width);
	previewRect.Inflate(-1*lineHeight, -1*lineHeight);

	while (previewRect.X < strData.lineHeight)
		previewRect.Inflate(-1, -1);

	if (previewRect.Width <= 0)
		return false;

	float right = previewRect.X + previewRect.Width;
	float bottom = previewRect.Y + previewRect.Height;
	float height = previewRect.Height;
	width = previewRect.Width;

	strData.boundsRect = previewRect;
	strData.GuidePath.AddEllipse(right, bottom, -width, -height);
	return true;
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::DoSpiral(StringData& strData)
{
	strData.GuidePath.Reset();
	if (strData.Index > 0)
		return true;

	float startAngle = 0.0F;
	float sweepAngle = 180.0F;

	float width = (m_currImageSize.Width < m_currImageSize.Height) ? m_currImageSize.Width : m_currImageSize.Height;
	RectF previewRect(0.0F, 0.0F, width, width);
	PointF CtrPt(previewRect.X + (previewRect.Width/2), previewRect.Y + (previewRect.Height/2)); // cheesy but works
	float lineHeight = (float)strData.lineHeight;
	lineHeight += m_currLineSpacing;
	
	PointF location(CtrPt);
	SizeF size(0.0F, 0.0F);
	RectF tempRect;

	bool Stop = false;
	if (lineHeight <= 0)
		Stop = true;

	for (int i=0; !Stop; i++)
	{
		size.Height += lineHeight;
		size.Width += lineHeight;
		location.Y -= lineHeight/2;
		location.X -= ((i % 2)==0) ? 0.0F : lineHeight;

		tempRect = RectF(location, size);
		startAngle = ((i % 2)==0) ? 180.0F : 0.0F;

		float rightOffset = (tempRect.GetRight() - previewRect.GetRight()) + strData.fontSize;
		float leftOffset = (tempRect.X - previewRect.X) - strData.fontSize;
		if ( (rightOffset > 0) || (leftOffset < 0))
			break;
		strData.GuidePath.AddArc(tempRect, startAngle, sweepAngle);
	}

	strData.boundsRect = RectF(0, 0, m_currImageSize.Width, m_currImageSize.Height);
	return true;
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::DoInvSemiCircle(StringData& strData)
{
	strData.GuidePath.Reset();
	float width = (m_currImageSize.Width < m_currImageSize.Height) ? m_currImageSize.Width : m_currImageSize.Height;
	float lineHeight = (float)strData.lineHeight;
	lineHeight += m_currLineSpacing;
	lineHeight = (float)((strData.Index+1) * lineHeight);

	RectF previewRect(0, 0, width, width);
	previewRect.Inflate(-1*lineHeight, -1*lineHeight);

	while (previewRect.X < strData.lineHeight)
		previewRect.Inflate(-1, -1);

	if (previewRect.Width <= 0)
		return false;

	float left = previewRect.X;
	float top = previewRect.Y;
	width = previewRect.Width;
	float height = width;

	RectF crcRect(left, top, width, height);
	strData.boundsRect = crcRect;
	strData.GuidePath.AddArc(crcRect, 180.0F, -180.0F);
	return true;
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::DoSemiCircle(StringData& strData)
{
	strData.GuidePath.Reset();
	float width = (m_currImageSize.Width < m_currImageSize.Height) ? m_currImageSize.Width : m_currImageSize.Height;
	float lineHeight = (float)strData.lineHeight;
	lineHeight += m_currLineSpacing;
	lineHeight = (float)((strData.Index+1) * lineHeight);

	RectF previewRect(0, 0, width, width);
	previewRect.Inflate(-1*lineHeight, -1*lineHeight);

	while (previewRect.X < strData.lineHeight)
		previewRect.Inflate(-1, -1);

	if (previewRect.Width <= 0)
		return false;

	float left = previewRect.X;
	float top = previewRect.Y;
	width = previewRect.Width;
	float height = width;

	RectF crcRect(left, top, width, height);
	strData.boundsRect = crcRect;
	strData.GuidePath.AddArc(crcRect, 180.0F, 180.0F);
	return true;
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::DoVrtLine(StringData& strData)
{
	strData.GuidePath.Reset();
	strData.TextPath.Reset();

	RectF imageRect(0, 0, m_currImageSize.Width, m_currImageSize.Height);
	float charWidth = (float)strData.charWidth;
	float lineHeight = (float)strData.lineHeight;

	lineHeight += m_currLineSpacing;
	charWidth += m_currCharSpacing;

	RectF charRect(0, 0, charWidth, lineHeight);
	float XOffset = imageRect.X + ((strData.Index) * charWidth);
	charRect.Offset(XOffset, 0.0F);

	strData.boundsRect.X = charRect.X;
	strData.boundsRect.Y = charRect.Y;
	strData.boundsRect.Width = charRect.Width;
	strData.boundsRect.Height = imageRect.Height;

	if (charRect.GetRight() <= imageRect.GetRight())
	{
		for (int i = 0; i < strData.str.GetLength(); i++)
		{
			CComBSTR bstr(CString(strData.str[i]));

			charRect.Offset(0.0F, ExtraAllowance(PRE, strData.str[i], lineHeight));
			strData.TextPath.AddString(bstr, 1, m_currFontFamily, FontStyleRegular, (float)strData.fontSize, charRect, NULL);
			charRect.Offset(0.0F, lineHeight + ExtraAllowance(POST, strData.str[i], lineHeight));

			if (charRect.GetBottom() > imageRect.GetBottom())
				break;
		}
	}

	strData.TextPath.Transform(&m_prvMatrix);
	return true;
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::DoHzLine(StringData& strData)
{
	strData.GuidePath.Reset();
	RectF imageRect(0, 0, m_currImageSize.Width, m_currImageSize.Height);

	// Make the bezier Rect
	int lineHeight = strData.lineHeight;
	lineHeight += m_currLineSpacing;
	float linePos = imageRect.Y + ((strData.Index+1) * lineHeight);

	if (linePos < strData.lineHeight)
	{
		while (linePos < strData.lineHeight)
		{
			++lineHeight;
			linePos = imageRect.Y + ((strData.Index+1) * lineHeight);
		}
	}

	float top = linePos - lineHeight;
	float bottom = linePos;

	if (linePos >= imageRect.GetBottom())
		return false;

	strData.boundsRect = RectF(imageRect.X, top, imageRect.Width, (float)lineHeight);
	strData.GuidePath.AddLine(imageRect.X, linePos, imageRect.Width, linePos);
	return true;
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::DoInvBezier(StringData& strData)
{
	strData.GuidePath.Reset();
	RectF imageRect(0, 0, m_currImageSize.Width, m_currImageSize.Height);

	// Make the bezier Rect
	int lineHeight = strData.lineHeight;
	lineHeight += m_currLineSpacing;
	float linePos = imageRect.Y + (((strData.Index+1) * lineHeight) + lineHeight);

	if (linePos < strData.lineHeight)
	{
		while (linePos < strData.lineHeight)
		{
			++lineHeight;
			linePos = imageRect.Y + (((strData.Index+1) * lineHeight) + lineHeight);
		}
	}

	float top = linePos - (lineHeight * 2);
	float bottom = linePos + lineHeight;

	if (linePos >= imageRect.GetBottom())
		return false;

	RectF bezierRect(imageRect.X, top, imageRect.Width, bottom-top);

	PointF pts[4];
	pts[0].X = (float)bezierRect.X;
	pts[0].Y = (float)(bezierRect.Y + bezierRect.Height/2);

	pts[2].X = (float)(bezierRect.X + bezierRect.Width/2);
	pts[2].Y = (float)bezierRect.Y;

	pts[1].X = (float)(bezierRect.X + bezierRect.Width/2);
	pts[1].Y = (float)bezierRect.Y + bezierRect.Height;

	pts[3].X = (float)bezierRect.X + bezierRect.Width;
	pts[3].Y = (float)(bezierRect.Y + bezierRect.Height/2);

	// if the pts are beyond the rect then do not add
	strData.boundsRect = bezierRect;
	strData.GuidePath.AddBezier(pts[0], pts[1], pts[2], pts[3]);
	return true;
}

////////////////////////////////////////////////////////////////////////
bool CStringDraw::DoBezier(StringData& strData)
{
	strData.GuidePath.Reset();
	RectF imageRect(0, 0, m_currImageSize.Width, m_currImageSize.Height);

	// Make the bezier Rect
	int lineHeight = strData.lineHeight;
	lineHeight += m_currLineSpacing;
	float linePos = imageRect.Y + (((strData.Index+1) * lineHeight) + lineHeight);

	if (linePos < strData.lineHeight)
	{
		while (linePos < strData.lineHeight)
		{
			++lineHeight;
			linePos = imageRect.Y + (((strData.Index+1) * lineHeight) + lineHeight);
		}
	}

	float top = linePos - (lineHeight * 2);
	float bottom = linePos + lineHeight;

	if (linePos >= imageRect.GetBottom())
		return false;

	RectF bezierRect(imageRect.X, top, imageRect.Width, bottom-top);

	PointF pts[4];
	pts[0].X = (float)bezierRect.X;
	pts[0].Y = (float)(bezierRect.Y + bezierRect.Height/2);

	pts[1].X = (float)(bezierRect.X + bezierRect.Width/2);
	pts[1].Y = (float)bezierRect.Y;

	pts[2].X = (float)(bezierRect.X + bezierRect.Width/2);
	pts[2].Y = (float)bezierRect.Y + bezierRect.Height;

	pts[3].X = (float)bezierRect.X + bezierRect.Width;
	pts[3].Y = (float)(bezierRect.Y + bezierRect.Height/2);

	// if the pts are beyond the rect then do not add
	strData.boundsRect = bezierRect;
	strData.GuidePath.AddBezier(pts[0], pts[1], pts[2], pts[3]);
	return true;
}