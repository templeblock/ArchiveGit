#pragma once
#include "AGDC.h"
#include "Dib.h"
#include "Matrix.h"
#include "DocCommands.h"
#include "ToolSettings.h"

class CImageObject
{
public:
	CImageObject(const CRect* pPageRect);
	CImageObject(const CRect* pPageRect, CDib pDIB);
	CImageObject(const CRect* pPageRect, HGLOBAL hMemory);
	~CImageObject();
private:
	void Init();
	void FreeDIB();

public:
	void Close();

	CImageObject* Duplicate(bool bCopyDib);
	void SetRotatedDibForDraw();
	void SetNormalDibForDraw();
	void FreeDibForDraw();
	void Draw(CAGDC& dc, bool bOverrideHidden);
	bool HitTest(CPoint Pt, bool bOverrideHidden);
private:
	bool LoadDIB(CDib Dib, bool bTakeOverMemory);
	CString DuplicateSourceFile();
	LPCSTR DetermineSourceType(BYTE* pMemory, DWORD dwSize);
public:
	void GetDestInflateSize(SIZE& Size);
	const CRect& GetDestRect() const
		{ return m_DestRect; }
	void SetDestRect(const CRect& DestRect)
	{
		m_DestRect = DestRect;
		SetModified(true);
	}
	CRect GetDestRectTransformed()
	{
		CRect DestRect = GetDestRect();
		m_Matrix.Transform(DestRect);
		return DestRect;
	}
	int GetID() const
	{
		return m_nID;
	}
	void SetID(int nID)
	{
		m_nID = nID;
	}
	bool IsHidden(bool bOverrideHidden = false)
	{
		return (m_bDeleted | m_bHidden) && !bOverrideHidden;
	}
	void SetHidden(bool bHidden)
	{
		m_bHidden = bHidden;
	}
	bool IsDeleted()
	{
		return m_bDeleted;
	}
	void SetDeleted(bool bDeleted)
	{
		m_bDeleted = bDeleted;
		SetModified(true);
	}
	const CMatrix& GetMatrix() const
	{
		return m_Matrix;
	}
	void SetMatrix(const CMatrix& Matrix)
	{
		m_Matrix = Matrix;
		SetModified(true);
	}
	void Transform(const CMatrix& Matrix)
	{
		m_Matrix *= Matrix;
	}
	const CRect& GetCropRect()
	{
		if (!m_CropRect.IsRectEmpty())
			return m_CropRect;

		return m_DestRect;
	}
	void SetCropRect(const CRect& CropRect)
	{
		m_CropRect = CropRect;
		m_CropRect.IntersectRect(&m_CropRect, &m_DestRect);
		SetModified(true);
	}
	CDib GetDib(bool bForDraw = false)
	{
		return (bForDraw ? m_pDibForDraw : m_pDibForEdit);
	}
	void SetDib(CDib pDib, bool bKeepCurrentAsUndo)
	{
		FreeDibForDraw();

		if (m_pDibForEdit)
			SetModified(true);

		if (bKeepCurrentAsUndo)
		{
			if (m_pDibForUndo != pDib)
				DibFree(m_pDibForUndo);
			m_pDibForUndo = m_pDibForEdit;
			m_pDibForEdit = pDib;
		}
		else
		{
			if (m_pDibForEdit != pDib)
				DibFree(m_pDibForEdit);
			m_pDibForEdit = pDib;
		}
	}
	bool EditStart(bool bEditInPlace)
	{
		// Since we are getting ready to edit the image, get rid on any rotated DibForDraw
		FreeDibForDraw();

		m_bEditStarted = true;
		if (!bEditInPlace && !m_pDibForUndo)
			m_pDibForUndo = DibCopy(m_pDibForEdit);

		if (!m_bMatrixForUndo)
		{
			m_MatrixForUndo = m_Matrix;
			m_bMatrixForUndo = true;
		}

		return true;
	}
	void EditApply()
	{
		if (!m_bEditStarted)
			return;

		m_bEditStarted = false;
		if (m_pDibForUndo)
		{
			DibFree(m_pDibForUndo);
			m_pDibForUndo = NULL;
		}

		if (m_bMatrixForUndo)
		{
			m_MatrixForUndo.Unity();
			m_bMatrixForUndo = false;
		}

		SetModified(true);
	}
	void EditUndo()
	{
		if (!m_bEditStarted)
			return;

		m_bEditStarted = false;
		if (m_pDibForUndo)
		{
			SetDib(m_pDibForUndo, false/*bKeepCurrentAsUndo*/);
			m_pDibForUndo = NULL;
		}

		if (m_bMatrixForUndo)
		{
			m_Matrix = m_MatrixForUndo;
			m_MatrixForUndo.Unity();
			m_bMatrixForUndo = false;
		}
	}
	bool EditCanUndo()
	{
		return m_bEditStarted; //j m_pDibForUndo || m_Matrix != m_MatrixForUndo;
	}
	long GetWidth()
	{
		if (!m_pDibForEdit)
			return 0;

		return m_pDibForEdit.Width();
	}
	long GetHeight()
	{
		if (!m_pDibForEdit)
			return 0;
			
		return m_pDibForEdit.Height();
	}
	double GetAngle()
	{
		return m_Matrix.GetAngle();
	}
	bool IsPortrait()
	{
		if (!m_pDibForEdit)
			return false;
			
		return m_pDibForEdit.Width() < m_pDibForEdit.Height();
	}
	void SetTransparent(bool bOn)
	{
		m_TransparentColor = (bOn ? m_LastHitColor : CLR_NONE);
		SetModified(true);
	}
	void SetTransparentColor(COLORREF TransparentColor)
	{
		m_TransparentColor = TransparentColor;
		SetModified(true);
	}
	COLORREF GetTransparentColor()
	{
		return m_TransparentColor;
	}
	bool IsTransparent()
	{
		return m_TransparentColor != CLR_NONE;
	}
	void SetCoverDraw(bool bCoverDraw)
	{
		m_bCoverDraw = bCoverDraw;
	}
	bool GetCoverDraw()
	{
		return m_bCoverDraw;
	}
	COLORREF GetLastHitColor()
	{
		return m_LastHitColor;
	}
	const CString& GetSourceFile()
	{
		return m_strSourceFile;
	}
	const CString& GetSourceType()
	{
		return m_strSourceType;
	}
	bool GetModified()
	{
		return m_bModified;
	}
	void SetModified(bool bModified)
	{
		m_bModified = bModified;
	}

	int ProcessEndEdit(int iValue);
	bool WriteFile(LPCTSTR strFileName, HGLOBAL hMemory, CString* pstrSourceType = NULL);
	COLORREF GetTransparentColor(LPCTSTR pFileName);
	bool Open(LPCTSTR strFileName);
	bool ReOpen();
	bool GetDibCropRect(CRect& DibCropRect);

	bool FlipRotateTab(int iValue);
	bool FlipXY(bool bFlipX, bool bFlipY);
	bool Rotate(double fAngle);

	bool CropTab(int iValue);
	bool CropApply(const CRect& CropRectInPixels);

	bool ResizeTab(int iValue, const CSize& Size);
	bool ResizeApply(const CSize& Size);

	bool EffectsTab(int iValue);
	bool EffectsExecute(int iValue);
	bool EffectsTabSubCommand(CString& strCommand);

	bool ContrastBrightnessTab(int iValue);
	bool AutoCorrect();

	bool ColorsTab(int iValue);
	bool MakeGray();
	bool Invert();

	bool RedEyeTab(int iValue);
	bool RedEyeClicked(CPoint Pt, const CToolSettings& ToolSettings);

	bool RecolorTab(int iValue);
	bool RecolorClicked(CPoint Pt, const CToolSettings& ToolSettings);

	bool BordersTab(int iValue);
	bool GenerateBorder();
	bool BorderType(LPCTSTR pstrType);
	bool BorderSize(int nSize);
	bool BorderSoftness(int nSoftness);
	bool BorderStretch(bool bStretch);
	bool BorderImageMerge(const CString& strFileName, CRect& PageRect);
	bool Merge(CImageObject* pObject);
	bool Maximize(const CRect& PageRect, bool bWidth, bool bHeight);

protected:
	int m_nID;
	bool m_bHidden;
	bool m_bDeleted;
	bool m_bModified;
	bool m_bCoverDraw;
	bool m_bSourceFileIsTemporary;
	const CRect* m_pPageRect;
	CRect m_DestRect;
	CRect m_CropRect;
	CMatrix m_Matrix;
	COLORREF m_LastHitColor;
	COLORREF m_TransparentColor;
	CString m_strSourceFile;
	CString m_strSourceType;

	// For Effects
	CString m_strEffectOption;

	// For Borders
	CString m_strBorderType;
	int m_nBorderSize;
	int m_nBorderSoftness;
	bool m_bBorderStretch;

	// For StartOver
	CDib m_pDibOrig;

	// For Edit/Undo
	bool m_bEditStarted;
	bool m_bMatrixForUndo;
	CMatrix m_MatrixForUndo;
	CDib m_pDibForUndo;

	// For DibForDraw
	CDib m_pDibForEdit;
	CDib m_pDibForDraw;
	double m_fDibAngle;
};
