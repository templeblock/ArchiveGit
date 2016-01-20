#pragma once

#include "atlcoll.h"
#include "AxEditor.h"

enum SATYPES { PRE = 0, POST };

class CDraw
{
public:
	CDraw():m_hWnd(NULL){};
	virtual ~CDraw(){};

public:
	virtual void Init(HWND hWnd){};
	virtual void DoDraw(HDC hDC) = 0;

protected:
	HWND m_hWnd;
};

class CStringDraw : public CDraw
{
protected:
	struct StringData 
	{
		int  Index;
		int  fontSize;
		int  lineHeight;
		int  charWidth;
		int  avgCharWidth;
		bool bDirty;
		CString str;
		RectF boundsRect;
		GraphicsPath GuidePath;
		GraphicsPath TextPath;
	};


public:
	CStringDraw();
	~CStringDraw();

public:
	void Init(HWND hWnd);
	void DoDraw(HDC hDC);
	void RefreshPreviewWindow();

	void DeleteString(int nIndex);
	void SetString(CString str, int nIndex);
	void SetCurveType(STR_TYPES type = STR_BEZIER);
	void SetFont(CString strFont);
	void SetFontSize(int size);
	void SetImageWidth(float width);
	void SetImageHeight(float height);
	void SetImageSize(const SizeF& size);
	void SetColorGradientMode(LINEAR_MODES sweepMode = SWEEP_RIGHT);
	void SetColor1(Color clr);
	void SetColor2(Color clr);
	void SetImageBkColor(Color clr);
	void SetWhiteBkgdTransparent(bool bTransparent);
	void IncrementLineHeight();
	void DecrementLineHeight();
	void IncrementCharWidth();
	void DecrementCharWidth();

	void SaveImage(CString fileName, IMAGE_TYPES imgType);

protected:
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	float ExtraAllowance(SATYPES type, char ch, float lineHeight);

	void InitStringData(StringData& strData, int nIndex);
	void SetStringData(StringData& strData, CString str);
	void CopyData(StringData& newData, StringData& OldData);

	void ScalePreviewImage();
	void TranslatePreviewImage();

	void GetScaleFactor(SizeF& scaleFactor);
	void GetFontMetrics(LOGFONTA* lf, int& lineHeight, int& charWidth, int& avgCharWidth);
	void GetScaledRect(float scaleFactor, RectF& orgImgRect, RectF& scaledImgRect);

	bool NeedToScale(SizeF& newSize);
	bool SetStringPath(StringData& strData);

	bool DoVrtLine(StringData& strData);
	bool DoHzLine(StringData& strData);
	bool DoBezier(StringData& strData);
	bool DoEllipse(StringData& strData);
	bool DoCircle(StringData& strData);
	bool DoSemiCircle(StringData& strData);
	bool DoInvSemiCircle(StringData& strData);
	bool DoInvBezier(StringData& strData);
	bool DoSpiral(StringData& strData);

protected:
	RectF m_windowRect;
	Matrix m_prvMatrix;

	// string Current Stuff
	FontFamily*  m_currFontFamily;
	Font*        m_currFont;
	STR_TYPES	 m_currStrCurve;

	int			 m_currIndent;		// Not really useful for now
	int			 m_currRotation;	// Not really useful for now
	int			 m_currFontSize;
	int			 m_currLineSpacing;
	int			 m_currAvgCharWidth;
	int			 m_WhiteBkgdAlpha;
	float		 m_currCharSpacing;

	SizeF		 m_currImageSize;
	int			 m_logPixelsX;
	int			 m_logPixelsY;

	Color*       m_currTextGradClr1;
	Color*       m_currTextGradClr2;
	Color*		 m_currImageBkColor;

	LinearGradientMode m_currSweepMode;

	class StringDataTraits : public CElementTraits< StringData* >
	{
	public:
		static bool CompareElements(const StringData* element1, const StringData* element2)
		{
			if (element1->str == element2->str)
				return true;
			return false;
		};
	};

	CAtlList<StringData*, StringDataTraits> m_strList;
};


