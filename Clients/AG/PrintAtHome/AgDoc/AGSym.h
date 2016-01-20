#pragma once

#include "AGDoc.h"
#include "AGDib.h"
#include "AGMatrix.h"
#include "AGText.h"
#include "Dib.h"

#define _OFFSET 40
#define PROCESS_HIDDEN 1

enum SYMTYPE
{
	ST_ANY				 = 255,
	ST_IMAGE_OLD		 = 0,	// Obsolete - if found in an file, create a ST_IMAGE instead
	ST_TEXT				 = 1,
	ST_COLORBOX_OLD		 = 2,	// Obsolete - if found in an file, create a ST_RECTANGLE instead
	ST_RECTANGLE		 = 3,
	ST_ELLIPSE			 = 4,
	ST_LINE				 = 5,
	ST_DRAWING			 = 6,
	ST_ADDAPHOTO		 = 7,
	ST_IMAGE			 = 8,
	ST_CALENDAR			 = 9,
};

class FileStruct
{
public:
	FileStruct()
	{
		dwSize = 0;
		dwCRC32 = 0;
	}

	~FileStruct() {}

	bool operator == (FileStruct rhs)
	{
		return ((Name == rhs.Name) &&
				(Type == rhs.Type));
		/*
				&&
			(FileName == rhs.FileName) &&
				(dwSize == rhs.dwSize) &&
				(dwCRC32 == dwCRC32)
		*/
	}

	bool operator != (FileStruct rhs)
	{
		return !(*this == rhs);
	}

	CString Name;
	CString Type;
	CString FileName;
	DWORD dwSize;
	DWORD dwCRC32;
};

typedef void (CALLBACK * FNFILESPECSCALLBACK)(FileStruct* fstruct, LPARAM lParam);

class CAGSym
{
public:
	CAGSym(SYMTYPE SymType);
	virtual ~CAGSym();

	virtual CAGSym* Duplicate() = 0;
	virtual void Draw(CAGDC& dc, DWORD dwFlags) = 0;
	virtual bool HitTest(POINT Pt, DWORD dwFlags) = 0;
	virtual bool Read(CAGDocIO* pInput);
	virtual bool Write(CAGDocIO* pOutput);
	virtual void GetDestInflateSize(SIZE& Size) = 0;
	virtual const RECT& GetDestRect() const = 0;
	virtual void SetDestRect(const RECT& DestRect) = 0;
	virtual void WriteFYSXml(FILE* output, int itabs) = 0;

	void RegisterFileSpecsCallback(FNFILESPECSCALLBACK fnFileSpecsCallback, LPARAM lParam);
	void UnregisterFileSpecsCallback();

	void GetXMLData(double& xpos, double& ypos, double& width, double& ht, double& angle);
	void GetXMLData(double& cx, double& cy, bool& brotated);

	void DuplicateTo(CAGSym* pSym);
	CAGDIBSectionDC* CreateShadowDib(const RECT& DestRect, bool bTransparent, int iResolution = 0);

	const CString GetTypeName() const
		{
			switch (m_SymType)
			{
				case ST_IMAGE:				return "Picture";
				case ST_TEXT:				return "Text";
				case ST_RECTANGLE:			return "Rectangle";
				case ST_ELLIPSE:			return "Ellipse";
				case ST_LINE:				return "Line";
				case ST_DRAWING:			return "Shape";
				case ST_ADDAPHOTO:			return "Add-A-Photo";
				case ST_CALENDAR:			return "Calendar";
				default:					return "Unknown";
			}
		}
	SYMTYPE GetType() const
		{ return m_SymType; }
	int GetID() const
		{ return m_nID; }
	void SetID(int nID)
		{ m_nID = nID; }
	bool IsImage() const
		{ return m_SymType == ST_IMAGE; }
	bool IsText() const
		{ return m_SymType == ST_TEXT; }
	bool IsAddAPhoto() const
		{ return m_SymType == ST_ADDAPHOTO; }
	bool IsLine() const
		{ return m_SymType == ST_LINE; }
	bool IsRectangle() const
		{ return m_SymType == ST_RECTANGLE; }
	bool IsEllipse() const
		{ return m_SymType == ST_ELLIPSE; }
	bool IsShape() const
		{ return m_SymType == ST_DRAWING; }
	bool IsCalendar() const
		{ return m_SymType == ST_CALENDAR; }
	bool IsGraphic() const
		{ return m_SymType == ST_RECTANGLE ||
				 m_SymType == ST_ELLIPSE ||
				 m_SymType == ST_LINE ||
				 m_SymType == ST_DRAWING; }
	bool IsHidden(DWORD dwFlags = 0)
		{
			if (m_bDeleted)
				return true;
			return (m_bHidden && !(dwFlags & PROCESS_HIDDEN));
		}
	void SetHidden(bool bHidden)
		{ m_bHidden = bHidden; }
	bool IsDeleted()
		{ return m_bDeleted; }
	void SetDeleted(bool bDeleted)
		{ m_bDeleted = bDeleted; }
	RECT GetDestRectTransformed()
		{
			RECT DestRect = GetDestRect();
			m_Matrix.Transform(DestRect);
			return DestRect;
		}
	const CAGMatrix& GetMatrix() const
		{ return m_Matrix; }
	void SetMatrix(const CAGMatrix& Matrix)
		{ m_Matrix = Matrix; }
	void Transform(const CAGMatrix& Matrix)
		{ m_Matrix *= Matrix; }

protected:
	SYMTYPE m_SymType;
	int m_nID;
	bool m_bHidden;
	bool m_bDeleted;
	CAGMatrix m_Matrix;
	FNFILESPECSCALLBACK m_pfnFSCallback;
	LPARAM m_pCallbackParam;
};


// CAGSym flags
#define SYM_HIDDEN				1

// CAGSymImage flags
#define SYMIMAGE_DIB_WRITTEN	1
#define SYMIMAGE_CROP_WRITTEN	2
#define SYMIMAGE_LOCKED			4

class CAGSymImage : public CAGSym
{
public:
	CAGSymImage(bool bOldReader = false);
	CAGSymImage(const BITMAPINFOHEADER* pDIB, const RECT& MaxBounds);
	CAGSymImage(HGLOBAL hMemory, const RECT& MaxBounds);
	CAGSymImage(CAGSymImage* pImageSym, int nQuality);

	~CAGSymImage();

	CAGSym* Duplicate();
	void Draw(CAGDC& dc, DWORD dwFlags);
	bool HitTest(POINT Pt, DWORD dwFlags);
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput);
//	bool DumpFileEx();
	BYTE* GetImage(DWORD& dwSrcSize);
	bool IsCoverAllowed() { return m_bCoverAllowed; }
	void WriteFYSXml(FILE* output, int itabs);
	void WriteXmlRects(FILE* output, SIZE& PageSize, int itabs);
	void GetDestInflateSize(SIZE& Size);
	const RECT& GetDestRect() const
		{ return m_DestRect; }
	void SetDestRect(const RECT& DestRect)
		{ m_DestRect = DestRect; }

	RECT& GetCropRect()
		{ return m_CropRect; }
	void SetCropRect(RECT& CropRect)
		{ m_CropRect = CropRect; ::IntersectRect(&m_CropRect, &m_CropRect, &m_DestRect); }
	BITMAPINFOHEADER* GetDib()
		{ return m_pDIB;}
	void SetDib(BITMAPINFOHEADER* pDIB)
		{ m_pDIB = pDIB; }
	DWORD GetCompressedSize()
		{ return m_dwCompressedSize; }
	void SetCompressedSize(DWORD dwCompressedSize)
		{ m_dwCompressedSize = dwCompressedSize; }
	void SetTransparent(bool bOn)
		{ m_TransparentColor = (bOn ? m_LastHitColor : CLR_NONE); }
	COLORREF GetTransparentColor()
		{ return m_TransparentColor; }
	void SetLocked(bool bLocked)
		{ m_bLocked = bLocked; }
	bool IsLocked()
		{ return m_bLocked; }
	bool GetLocked()
		{ return m_bLocked; }
	bool DoCoverDraw()
		{ return m_TransparentColor != CLR_NONE; }
	COLORREF GetLastHitColor()
		{ return m_LastHitColor; }
	void OrientChangeState(int iRotateDirection, bool bFlipX, bool bFlipY)
		{ CDib::OrientChangeState(m_cOrientation, iRotateDirection, bFlipX, bFlipY); }
	bool GetOrient(int& iRotateDirection, bool& bFlipX, bool& bFlipY)
		{ return CDib::OrientDecode(m_cOrientation, iRotateDirection, bFlipX, bFlipY); }
	CString& GetNativeType()
		{ return m_strNativeType; }
	bool CanChangeOrientation()
		{ return m_strNativeType == "DIB"; }

	HANDLE CopyDIB();
	HGLOBAL RestoreNativeData();

protected:
	void FreeDIB();
	bool LoadDIB(const BITMAPINFOHEADER* pDIB, bool bTakeOverMemory, const RECT* pMaxBounds);
	void SaveNativeData(HGLOBAL hMemory);
	CString DuplicateNativeData();
	LPCSTR DetermineNativeType(BYTE* pMemory, DWORD dwSize);

protected:
	RECT m_DestRect;
	RECT m_CropRect;
	bool m_bLocked;
	bool m_bOldReader;
	COLORREF m_LastHitColor;
	COLORREF m_TransparentColor;
	DWORD m_dwCompressedSize;
	BITMAPINFOHEADER* m_pDIB;
	BYTE m_cOrientation;
	CString m_strNativeFile;
	CString m_strNativeType;
	bool m_bCoverAllowed;
};

class CAGSymText : public CAGSym, public CAGText
{
public:
	CAGSymText(SYMTYPE SymType);
	~CAGSymText();

	CAGSym* Duplicate();
	void Draw(CAGDC& dc, DWORD dwFlags);
	bool HitTest(POINT Pt, DWORD dwFlags);
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput);
//	bool DumpFileEx();
	void WriteFYSXml(FILE* output, int itabs);
	void GetDestInflateSize(SIZE& Size);
	const RECT& GetDestRect() const
		{ return CAGText::GetDestRect(); }
	void SetDestRect(const RECT& DestRect)
		{ CAGText::SetDestRect(DestRect); }

	void Swap(CAGSymText* pSym);

protected:
	void WriteXmlFontSpecs(FILE* output, SpecArray& cSpecArray, int iTabs);
	void WriteXmlFillType(FILE* output, CAGSpec& ExamineSpec, int iTabs);
	void WriteXmlOutline(FILE* output, CAGSpec& ExamineSpec, int iTabs);

};

class CAGSymGraphic : public CAGSym
{
public:
	CAGSymGraphic(SYMTYPE SymType);
	virtual ~CAGSymGraphic();

	void DuplicateTo(CAGSymGraphic* pSym);
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput);
	void WriteFYSXml(FILE* output, int itabs);

	void SetLineColor(COLORREF Color)
		{ m_LineColor = Color; }
	COLORREF GetLineColor()
		{ return m_LineColor; }

	void SetLineWidth(int LineWidth)
		{ m_LineWidth = LineWidth; }
	int GetLineWidth()
		{ return m_LineWidth; }

	void SetFillType(FillType Type)
		{ m_FillType = Type; }
	FillType GetFillType()
		{ return m_FillType; }

	void SetFillColor(COLORREF Color)
		{ m_FillColor = Color; }
	COLORREF GetFillColor()
		{ return m_FillColor; }

	void SetFillColor2(COLORREF Color)
		{ m_FillColor2 = Color; }
	COLORREF GetFillColor2()
		{ return m_FillColor2; }

	void SetFillColor3(COLORREF Color)
		{ m_FillColor3 = Color; }
	COLORREF GetFillColor3()
		{ return m_FillColor3; }

	void SetShadowType(ShadowType Type)
		{ m_ShadowType = Type; }
	ShadowType GetShadowType()
		{ return m_ShadowType; }

	void SetShadowColor(COLORREF Color)
		{ m_ShadowColor = Color; }
	COLORREF GetShadowColor()
		{ return m_ShadowColor; }

	void SetShadowOffset(POINT Offset)
		{ m_ShadowOffset = Offset; }
	POINT GetShadowOffset()
		{ return m_ShadowOffset; }

	virtual void SetShapeName(LPCTSTR pShapeName);
	virtual LPCTSTR GetShapeName();

protected:
	int m_LineWidth;
	COLORREF m_LineColor;

	FillType m_FillType;
	COLORREF m_FillColor;
	COLORREF m_FillColor2;
	COLORREF m_FillColor3;

	ShadowType m_ShadowType;
	COLORREF m_ShadowColor;
	POINT m_ShadowOffset;
	CString m_strShapeName;
};

class CAGSymRectangle : public CAGSymGraphic
{
public:
	CAGSymRectangle(bool bReadAsObsoleteColorbox = false);
	~CAGSymRectangle();

	CAGSym* Duplicate();
	void Draw(CAGDC& dc, DWORD dwFlags);
	bool HitTest(POINT Pt, DWORD dwFlags);
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput);
	void WriteFYSXml(FILE* output, int itabs);
	void GetDestInflateSize(SIZE& Size);
	const RECT& GetDestRect() const
		{ return m_DestRect; }
	void SetDestRect(const RECT& DestRect)
		{ m_DestRect = DestRect; }

protected:
	RECT m_DestRect;
	bool m_bReadAsObsoleteColorbox;
};

class CAGSymEllipse : public CAGSymGraphic
{
public:
	CAGSymEllipse();
	~CAGSymEllipse();

	CAGSym* Duplicate();
	void Draw(CAGDC& dc, DWORD dwFlags);
	bool HitTest(POINT Pt, DWORD dwFlags);
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput);
	void WriteFYSXml(FILE* output, int itabs);
	void GetDestInflateSize(SIZE& Size);
	const RECT& GetDestRect() const
		{ return m_DestRect; }
	void SetDestRect(const RECT& DestRect)
		{ m_DestRect = DestRect; }

protected:
	RECT m_DestRect;
};

class CAGSymLine : public CAGSymGraphic
{
public:
	CAGSymLine();
	~CAGSymLine();

	CAGSym* Duplicate();
	void Draw(CAGDC& dc, DWORD dwFlags);
	bool HitTest(POINT Pt, DWORD dwFlags);
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput);
	void WriteFYSXml(FILE* output, int itabs);
	void GetDestInflateSize(SIZE& Size);
	const RECT& GetDestRect() const
		{ return m_DestRect; }
	void SetDestRect(const RECT& DestRect)
		{ m_DestRect = DestRect; }

	void SetLineStart(LineStart Start)
		{ m_LineStart = Start; }
	LineStart GetLineStart()
		{ return m_LineStart; }

protected:
	RECT m_DestRect;
	LineStart m_LineStart;
};

class CAGSymDrawing : public CAGSymGraphic
{
public:
	CAGSymDrawing();
	~CAGSymDrawing();

	CAGSym* Duplicate();
	void Draw(CAGDC& dc, DWORD dwFlags);
	bool HitTest(POINT Pt, DWORD dwFlags);
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput);
	void WriteFYSXml(FILE* output, int itabs);
	void GetDestInflateSize(SIZE& Size);
	const RECT& GetDestRect() const
		{ return m_DestRect; }
	void SetDestRect(const RECT& DestRect)
		{ m_DestRect = DestRect; }

	bool PtInShape(POINT Pt, POINT* pCommands, int nCommands, POINT* pPoints, int nPoints);
	void SetPoints(POINT* pPoints, int nPoints);
	int GetPoints(POINT** ppPoints);
	void SetShapeName(LPCTSTR pShapeName);
	LPCTSTR GetShapeName();

protected:
	RECT m_DestRect;
	int m_nPoints;
	POINT* m_pPoints;
};

typedef union 
{
	DWORD dwData;
	struct
	{
		UINT HideBoxes		: 1;
		UINT HideAllBoxes	: 1;
		UINT Unused			: 3;
		UINT WeekStart		: 1;
		UINT YearStyle		: 4;
		UINT WeekStyle		: 4;
		UINT TitleSize		: 8;
		UINT DayOfWkSize	: 8;
		// leaves 2 unused bits out of 32
	};
} CALENDARSTYLES;

typedef union 
{
	DWORD dwData;
	struct
	{
		UINT TitleHorzJust	: 4;
		UINT TitleVertJust	: 4;
		UINT WeekHorzJust	: 4;
		UINT WeekVertJust	: 4;
		UINT DateHorzJust	: 4;
		UINT DateVertJust	: 4;
		// leaves 8 unused bits out of 32
	};
} CALENDARSTYLES1;

class CCalSupport;

class CAGSymCalendar : public CAGSymGraphic
{
	friend CCalSupport;

public:
	CAGSymCalendar();
	~CAGSymCalendar();

	CAGSym* Duplicate();
	void Draw(CAGDC& dc, DWORD dwFlags);
	bool HitTest(POINT Pt, DWORD dwFlags);
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput);
	void WriteFYSXml(FILE* output, int itabs);
	void GetDestInflateSize(SIZE& Size);
	const RECT& GetDestRect() const
		{ return m_DestRect; }
	void SetDestRect(const RECT& DestRect)
		{ m_DestRect = DestRect; }

	void SetMonth(int iMonth)
		{ m_iMonth = iMonth; }
	int GetMonth()
		{ return m_iMonth; }
	void SetYear(int iYear)
		{ m_iYear = iYear; }
	int GetYear()
		{ return m_iYear; }

	void SetMode(int iMode)
		{ m_iMode = iMode; }
	int GetMode()
		{ return m_iMode; }

	CAGSpec* GetActiveSpec()
		{ return (m_iMode == -1 ? NULL : (m_iMode == 0 ? &m_SpecMonth : (m_iMode == 1 ? &m_SpecWeek : &m_SpecDays))); }

	void SetSpecTextSizes(int mTextSize, int wTextSize, int dTextSize);
	void GetFonts(LOGFONTLIST& lfList);
	void SetSpecFillType(FillType Fill);
	void SetSpecFillColor(COLORREF Color);
	void SetSpecFillColor2(COLORREF Color);
	void SetSpecLineWidth(int LineWidth);
	void SetSpecLineColor(COLORREF Color);
	void SetSpecTextSize(int TextSize);
	void SetSpecTypeface(const LOGFONT& Font);
	void SetSpecEmphasis(bool bBold, bool bItalic, bool bUnderline);
	void SetSpecHorzJust(eTSJust HorzJust);
	void SetSpecVertJust(eVertJust VertJust);

	UINT GetYearStyle()		{ return m_uCalStyles.YearStyle; }
	UINT GetWeekStyle()		{ return m_uCalStyles.WeekStyle; }
	UINT GetTitleSize()		{ return m_uCalStyles.TitleSize; }
	UINT GetDayOfWkSize()	{ return m_uCalStyles.DayOfWkSize; }
	bool GetWeekStart()		{ return m_uCalStyles.WeekStart; }
	bool GetHideBoxes()		{ return m_uCalStyles.HideBoxes; }
	bool GetHideAllBoxes()	{ return m_uCalStyles.HideAllBoxes; }

	void SetYearStyle(UINT val)		{ m_uCalStyles.YearStyle = val; }
	void SetWeekStyle(UINT val)		{ m_uCalStyles.WeekStyle = val; }
	void SetTitleSize(UINT val)		{ m_uCalStyles.TitleSize = val; }
	void SetDayOfWkSize(UINT val)	{ m_uCalStyles.DayOfWkSize = val; }
	void SetWeekStart(bool bOn)		{ m_uCalStyles.WeekStart = bOn; }
	void SetHideBoxes(bool bHide)	{ m_uCalStyles.HideBoxes = bHide; }
	void SetHideAllBoxes(bool bHide){ m_uCalStyles.HideAllBoxes = bHide; }

	UINT GetVertJust();
	UINT GetHorzJust();
	CString GetActivePanelTitle();

protected:
	RECT m_DestRect;

	int m_iMode;
	int m_iMonth;
	int m_iYear;
	CALENDARSTYLES m_uCalStyles;
	CALENDARSTYLES1 m_uCalStyles1;
	CAGSpec m_SpecMonth;
	CAGSpec m_SpecWeek;
	CAGSpec m_SpecDays;
};
