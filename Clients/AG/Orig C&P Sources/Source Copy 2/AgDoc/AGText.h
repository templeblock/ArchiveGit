#ifndef __AGTEXT_H_
#define __AGTEXT_H_

#include "AGDC.h"
#include "AGDoc.h"
#include "sctypes.h"
#include "scapptyp.h"
#include "scpubobj.h"
#include "scselect.h"

class CAGSpec : public stSpec
{
public:
	CAGSpec();
	CAGSpec(const LOGFONT &Font, COLORREF Color,
	  eTSJust HorzJust = eRagRight, int nAboveParaLeading = 0,
	  int nBelowParaLeading = 0, int nLineLeading = 0);
	CAGSpec(const CAGSpec &Spec);

	void operator= (const CAGSpec &Spec);
	int operator== (const CAGSpec &Spec) const;
	int operator!= (const CAGSpec &Spec) const
		{ return (! operator== (Spec)); }

	void Read(CAGDocIO *pInput);
	void Write(CAGDocIO *pOutput) const;

public:
	UFont		m_Font;
	COLORREF	m_Color;
	eTSJust 	m_HorzJust;
	int 		m_nAboveParaLeading;
	int 		m_nBelowParaLeading;
	int 		m_nLineLeading;
};


class CAGText
{
public:
	CAGText();
	~CAGText();
	void BlinkCursor();
	void Create(const RECT &DestRect);
	void DrawColumn(CAGDC &dc);
	void DrawSelection(CAGDC &dc);
	void Edit(CAGDC *pdc, int x, int y, bool bClick = false);
	void EndEdit();
	scColumn *GetColumn()
		{ return (m_pColumn); }
	CAGDC *GetDC() const
		{ return (m_pdc); }
	const RECT &GetDestRect() const
		{ return (m_DestRect); }
	CAGDocIO *GetDocIO() const
		{ return (m_pDocIO); }
	void GetFonts(LOGFONTARRAY &lfArray);
	scSelection *GetSelection() const
		{ return (m_pSelection); }
	void GetSelParaTSList(scTypeSpecList &tsList) const;
	void GetSelTSList(scTypeSpecList &tsList) const;
	const scTypeSpecList &GetTSList() const
		{ return (m_TSList); }
	eVertJust GetVertJust() const;
	scTypeSpecList *GetWriteTSList() const
		{ return (m_pWriteTSList); }
	bool IsEditing() const
		{ return (m_pSelection != NULL); }
	bool IsEmpty() const;
	bool IsLButtonDown() const
		{ return (m_bLButtonDown); }
	void OnChar(UINT nChar);
	void OnKeyDown(UINT nChar);
	void OnKeyUp(UINT nChar);
	void OnLButtonDblClk(POINT Point);
	void OnLButtonDown(POINT Point, bool bShift);
	void OnLButtonUp(POINT Point);
	void OnMouseMove(POINT Point);
	void ReadColumn(CAGDocIO *pInput);
	void SetDC(CAGDC *pdc)
		{ m_pdc = pdc; }

	void SetColor(COLORREF Color);
	void SetFont(const LOGFONT &Font);
	void SetHorzJust(eTSJust HorzJust);
	void SetNextSpec(const CAGSpec &Spec);
	void SetPtSize(int nPtSize);
	void SetText(const char *pText, int nChars, int nSpecs,
	  const CAGSpec *pAGSpecs, const int *pSpecOffsets);

	void SetTypeface(const LOGFONT &Font);
	void SetUnderline(bool bUnderline);
	void SetVertJust(eVertJust VertJust);

	void ShowSelection(bool bShow);
	void WriteColumn(CAGDocIO *pOutput);

protected:
	void ComputeRedisplay(scRedispList &colRedisp);
	void ExtendSelection(eSelectMove movement);
	void MoveSelection(eSelectMove movement);
	void UpdateSelection();


protected:
	CAGDC			*m_pdc;
	RECT			m_DestRect; 
	scColumn		*m_pColumn;
	scSelection 	*m_pSelection;
	bool			m_bSelection;
	bool			m_bOverstrike;
	bool			m_bShiftKeyDown;
	bool			m_bControlKeyDown;
	bool			m_bLButtonDown;
	CAGSpec 		*m_pNextSpec;
	scSelection 	m_SelNextSpec;
	CAGDocIO		*m_pDocIO;
	scTypeSpecList	m_TSList;
	scTypeSpecList	*m_pWriteTSList;
	scMuPoint		m_LastPt;
};

#endif //__AGTEXT_H_
