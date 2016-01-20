#pragma once

#include "sctypes.h"
#include "scapptyp.h"
#include "scpubobj.h"
#include "scselect.h"
#include "Spell.h"

class CAGSym;
class CAGSpec;
class CAGDocIO;

typedef bool (CALLBACK * FNSPECCALLBACK)(CAGSpec* Spec, CAGSpec* pNewSpecTemplate);
typedef struct SPECSTRUCT
{
	CAGSpec* pSpec;
	CString strTxt;
} SpecStruct;
typedef CSimpleArray<SPECSTRUCT> SpecArray;
typedef CSimpleArray<CString> FontsArray;

class CAGSpec : public APPSpec
{
public:
	CAGSpec();
	CAGSpec(const LOGFONT& Font, eTSJust HorzJust, short sLineLeadPct, int LineWidth, COLORREF LineColor, FillType Fill, COLORREF FillColor, COLORREF FillColor2);
	CAGSpec(const CAGSpec& Spec);

	void operator= (const CAGSpec& Spec);
	bool operator== (const CAGSpec& Spec) const;
	bool operator!= (const CAGSpec& Spec) const
		{ return (!operator== (Spec)); }

	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput) const;
	void DrawTextAligned(CAGDC* pDC, LPCSTR pString, int* pWidths, CRect& RectAlign, eVertJust VertJust) const;
	void DrawText(CAGDC* pDC, LPCSTR pString, int* pWidths, int x, int y, CRect* pRectAlign = NULL) const;
	static bool CALLBACK CallbackChangeFillType(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);
	static bool CALLBACK CallbackChangeFillColor(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);
	static bool CALLBACK CallbackChangeFillColor2(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);
	static bool CALLBACK CallbackChangeLineWidth(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);
	static bool CALLBACK CallbackChangeLineColor(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);
	static bool CALLBACK CallbackChangeTextSize(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);
	static bool CALLBACK CallbackChangeTypeface(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);
	static bool CALLBACK CallbackChangeEmphasis(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);
	static bool CALLBACK CallbackChangeLineSpacing(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);
	static bool CALLBACK CallbackChangeCharSpacing(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate);

public:
	LOGFONTA m_Font;
	eTSJust m_HorzJust;
	TSTracking m_CharTracking;
	short m_sLineLeadPct;
	short m_sCharSpacePct;
	unsigned short m_sFlags;

	int m_LineWidth;
	COLORREF m_LineColor;

	FillType m_FillType;
	COLORREF m_FillColor;
	COLORREF m_FillColor2;
};


typedef status (_cdecl *AppCheckWordFunc)(char* pInBuffer, char* pResultBuffer, int iSizeBuffer);

class CAGText
{
public:
	CAGText();
	~CAGText();
	void BlinkCaret();
	void Create(const CRect& DestRect);
	void DrawAll(CAGDC* pDC);
protected:
	void DrawCaret(HDC hDC, const CAGMatrix& Matrix, const CRect& rect);
	void DrawSelection(HDC hDC, const CAGMatrix& Matrix, const CRect& rect);
	static void DrawHilite(const scXRect& rRect, APPDrwCtx pTextCtx, BOOL bSliverCursor);
	void DrawHilite(const scXRect& rRect, BOOL bSliverCursor);
public:
	void StartEdit(CAGDC* pDC, CAGSym* pSym);
	void EndEdit();
	bool InEditMode() const
		{ return m_pSelection != NULL; }
protected:
	void SaveSelection();
	void RestoreSelection();
public:
	void SelectAll(bool bShow = true);
	bool Delete();
	HANDLE GetClipboardData(bool bCut);
	void PasteText(char* pRawText, CAGSpec* pAGSpec);
	scColumn* GetColumn()
		{ return m_pColumn; }
	CAGDC* GetDC() const
		{ return m_pDC; }
	const CRect& GetDestRect() const
		{ return m_DestRect; }
	void SetDestRect(const CRect& DestRect);
	CAGDocIO* GetDocIO() const
		{ return m_pDocIO; }
	void GetFonts(LOGFONTLIST& lfList);
	scSelection* GetSelection() const
		{ return m_pSelection; }
	void GetSelParaTSList(scTypeSpecList& tsList) const;
	void GetSelTSList(scTypeSpecList& tsList) const;
	const scTypeSpecList& GetTSList() const
		{ return m_TSList; }
	eVertJust GetVertJust() const;
	scTypeSpecList* GetWriteTSList() const
		{ return m_pWriteTSList; }
	bool IsTextEntrySuppressed() const
		{ return m_bSuppressTextEntry;}
	void SetSuppressTextEntry(bool bSuppress)
		{ m_bSuppressTextEntry = bSuppress; }

protected:
	bool IsEmpty() const;
public:
	bool SpellCheck(CSpell& Spell, DWORD* pdwWordsChecked, DWORD* pdwWordsChanged, DWORD* pdwErrorsDetected, bool bCheckSelection);
	CString ExtractText();
	bool OnChar(UINT nChar);
	bool OnKeyDown(UINT nChar);
	void OnKeyUp(UINT nChar);
	void OnLButtonDblClk(POINT Point);
	void OnLButtonDown(POINT Point);
	bool OnLButtonUp(POINT Point);
	void OnMouseMove(POINT Point);
	void ReadColumn(CAGDocIO* pInput);

	bool IsDirty() {return m_fDirty;}
	void SetDirty(bool fDirty) {m_fDirty = fDirty;}

	void SpecChanger(FNSPECCALLBACK fnSpecCallback, CAGSpec* pNewSpecTemplate);
	void SetFillType(FillType Fill);
	void SetFillColor(COLORREF FillColor);
	void SetFillColor2(COLORREF FillColor);
	void SetLineWidth(int LineWidth);
	void SetLineColor(COLORREF FillColor);
	void SetTextSize(int TextSize, double fScale = 0.0);
	void SetTypeface(const LOGFONT& Font);
	void SetEmphasis(bool bBold, bool bItalic, bool bUnderline);
	void SetHorzJust(eTSJust HorzJust);
	void SetVertJust(eVertJust VertJust);
	void SetLineSpacing(int iUnits);
	void SetCharSpacing(int iUnits);
	void SetText(const char* pText, int nSpecs, CAGSpec** pAGSpecs, const int* pSpecOffsets);

	void ShowSelectionEx(HDC hDC, bool bCaretOnly, bool bShow, bool bDraw = true);
	void ShowSelection(bool bShow, bool bDraw = true);
	void ExtendSelection(eSelectMove movement);
	void MoveSelection(eSelectMove movement);
	void UpdateSelection();
	void WriteColumn(CAGDocIO* pOutput);
	void GetSpecStructArray(SpecArray& cSpecArray);

protected:
	void CopyStream(scColumn* pColumn);
	void SwapStreams(scColumn* pColumn);
	void Invalidate(scXRect& rRect);
	void Redisplay(scRedispList& colRedisp);
	void GetSpecArray(SpecArray& cSpecArray, CString& strText);

protected:
	CRect m_DestRect; 
	scColumn* m_pColumn;
	scSelection* m_pSelection;
	scTypeSpecList m_TSList;
	scTypeSpecList* m_pWriteTSList;
	scMuPoint m_LastMousePoint;
	scStreamLocation m_SavedSelectionMark;
	scStreamLocation m_SavedSelectionPoint;
	HDC m_hPaintDC;
	CAGSym* m_pSym;
	CAGDC* m_pDC;
	CAGDC* m_pSavedDC;
	CAGDocIO* m_pDocIO;
	bool m_bTempDCInUse;
	bool m_bSelectionIsShown;
	bool m_bAllowBlinkCaret;
	bool m_bOverstrike;
	bool m_bAutoSelect;
	bool m_fDirty;
	bool m_bSuppressTextEntry;
};
