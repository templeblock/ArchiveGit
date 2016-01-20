#ifndef __AGSYM_H_
#define __AGSYM_H_

#include "AGDoc.h"
#include "AGDC.h"
#include "AGText.h"

enum SYMTYPE
{
	ST_ANY = -1,
	ST_IMAGE,
	ST_TEXT
};


class CAGSym  
{
public:
	CAGSym (SYMTYPE SymType);
	virtual ~CAGSym ();

	SYMTYPE GetSymType () const
		{ return (m_SymType); }
	virtual void Draw (CAGDC &dc) = 0;
	const CAGMatrix &GetMatrix () const
		{ return (m_Matrix); }
	virtual bool HitTest (POINT Pt) const = 0;
	virtual bool Read (CAGDocIO *pInput);
	void SetMatrix (const CAGMatrix &Matrix)
		{ m_Matrix = Matrix; }
	void Transform (const CAGMatrix &Matrix)
		{ m_Matrix *= Matrix; }
	virtual bool Write (CAGDocIO *pOutput);

protected:
	CAGMatrix	m_Matrix;

private:
	SYMTYPE		m_SymType;
};


class CAGSymImage : public CAGSym  
{
public:
	CAGSymImage ();
	~CAGSymImage ();

	void Draw (CAGDC &dc);
	void Free ();
	const RECT &GetDestRect () const
		{ return (m_DestRect); }
	bool HitTest (POINT Pt) const;
	void LoadDIB (const BITMAPINFOHEADER *pHdr, const BYTE *pBits);
	bool Read (CAGDocIO *pInput);
	void SetDestRect (const RECT &DestRect)
		{ m_DestRect = DestRect; }
	bool Write (CAGDocIO *pOutput);

protected:
	RECT				m_DestRect;
	BITMAPINFOHEADER	*m_pDIB;

};


class CAGSymText : public CAGSym, public CAGText
{
public:
	CAGSymText ();
	~CAGSymText ();

	void Draw (CAGDC &dc);
	bool HitTest (POINT Pt) const;
	bool Read (CAGDocIO *pInput);
	bool Write (CAGDocIO *pOutput);
};

#endif //__AGSYM_H_
