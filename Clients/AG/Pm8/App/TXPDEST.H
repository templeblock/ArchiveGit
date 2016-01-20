/*
// Rich Text Format (RTF) CTxp destination
//
// $Header: /PM8/App/TXPDEST.H 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TXPDEST.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:22   Fred
// Initial revision.
// 
//    Rev 1.0   18 Dec 1996 09:10:36   Jay
// More RTF parsing
*/

class CTxpRTFDestination;

#include "rtfparse.h"
#include "ctxp.h"

#ifndef __TXPDEST_H__
#define __TXPDEST_H__

/////////////////////////////////////////////////////////////////////////////
// CTxpRTFDestination
//   A CTxp-based destination for reading RTF data.

class CTxpRTFDestination : public CRTFDestination
{
public:
	CTxpRTFDestination(CTxp* pTxp, int nBufferSize = 2048);
	virtual ~CTxpRTFDestination();

	virtual void PutChar(int c, const CRTFParser::CRTFState& State);
	virtual void Pard(const CRTFParser::CRTFState& State);
	virtual void Plain(const CRTFParser::CRTFState& State);
	virtual void SetTab(const CRTFTab* pTab, const CRTFParser::CRTFState& State);
	virtual void Flush(void);

	static CFixed TwipsToFixed(int nTwips);
	static CFixed TwipsToPoints(int nTwips);

protected:
	CTxp* m_pTxp;
	CHARACTER* m_pBuffer;
	int m_nBufferSize;
	int m_nCharactersInBuffer;

	void FlushBuffer(void);
	void UpdateStyle(const CRTFParser::CRTFState& State);
};

#endif
