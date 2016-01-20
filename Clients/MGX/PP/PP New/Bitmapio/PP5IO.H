//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _PP5IO_H_
#define _PP5IO_H_

#include "mgxbm.h"

typedef enum
	{
	PPFF_ORIGHDR_OPCODE,
	PPFF_OBJECT_OPCODE,
	PPFF_DATA_OPCODE,
	PPFF_ALPHA_OPCODE,
	PPFF_TEXT_OPCODE,
	PPFF_CMDLIST_OPCODE,
	PPFF_CURRHDR_OPCODE
	} PPFF_OPCODE, FAR *LPPPFF_OPCODE;


#define PPFF_VERSION		0x0200 // 2.0
#define PPFF_HDR_SIZE (14+20)
#define PPFF_OBJECT_SIZE (54+20+32)
#define PPFF_TEXT_SIZE	(6 + 6 + 12 + 2 + 20)
#define PPFF_BYTEORDER 0x4949

//LPTIFFHEADER PP5ReadTiffHeader(LPSTR lpFileName, LPINT lpDataType);
class CReadTiff : public CReadBitmap
{
public:
	CReadTiff(CReadBitmap *pReader, CFile *pFile, BOOL bEscapable, LPCMSINFO pCmsInfo);
	~CReadTiff() {}

	// overridables
	virtual void ProgressBegin(int nPasses) {m_pReader->ProgressBegin(nPasses);}
	virtual BOOL Progress(int nPart, int nWhole, BOOL fEscapable) { return m_pReader->Progress(nPart, nWhole, fEscapable); }
	virtual void ProgressEnd() { m_pReader->ProgressEnd(); }
private:
	CReadBitmap *m_pReader;
};

class CWriteTiff : public CWriteBitmap
{
public:
	CWriteTiff(CWriteBitmap *pWriter, CFile *pFile, BOOL bEscapable);
	~CWriteTiff() {}

	// overridables
	virtual void ProgressBegin(int nPasses) {m_pWriter->ProgressBegin(nPasses);}
	virtual BOOL Progress(int nPart, int nWhole, BOOL fEscapable) { return m_pWriter->Progress(nPart, nWhole, fEscapable); }
	virtual void ProgressEnd() { m_pWriter->ProgressEnd(); }
private:
	CWriteBitmap *m_pWriter;
};



#endif // _PP5IO_H_

