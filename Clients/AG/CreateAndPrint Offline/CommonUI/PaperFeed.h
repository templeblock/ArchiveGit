#pragma once

#include "resource.h"

// The iPaperFeedCode is a combination of three fields, totaling 5 bits (0-31)
// Bits 16|8 [Pos], Bit 4 [Face Up?], Bits 2|1 [Edge]

// The iPaperFeedCode is a combination of three fields, totaling 6 bits (0-63)
// Bits 32 [Feed], Bits 16|8 [Pos], Bit 4 [Face Up?], Bits 2|1 [Edge]

#define FEED_MASK   (32)
#define POS_MASK	(16|8)
#define FACE_MASK	(4)
#define EDGE_MASK	(2|1)

#define FEED_SHIFT  (5)
#define POS_SHIFT	(3)
#define FACE_SHIFT	(2)
#define EDGE_SHIFT	(0)

class CPaperFeed
{
public:
	//////////////////////////////////////////////////////////////////////
	static int Default()
	{
		int iPaperFeedCode = 0;
		SetPos(iPaperFeedCode, IDC_PAPERFEED_POS_RIGHT);
		SetFace(iPaperFeedCode, IDC_PAPERFEED_FACE_DOWN);
		SetEdge(iPaperFeedCode, IDC_PAPERFEED_EDGE_RIGHT);
		SetFeed(iPaperFeedCode, IDC_PAPERFEED_BOTTOM);
		return iPaperFeedCode;
	}

	//////////////////////////////////////////////////////////////////////
	static void Verify(int& iPaperFeedCode)
	{
		iPaperFeedCode &= (FEED_MASK | POS_MASK | FACE_MASK | EDGE_MASK);
	}

	//////////////////////////////////////////////////////////////////////
	static void SetPos(int& iPaperFeedCode, WORD wID)
	{
		iPaperFeedCode &= ~POS_MASK;
		iPaperFeedCode |= (((wID - IDC_PAPERFEED_POS_LEFT) << POS_SHIFT) & POS_MASK);
	}

	//////////////////////////////////////////////////////////////////////
	static void SetFace(int& iPaperFeedCode, WORD wID)
	{
		int iFaceUp = (wID == IDC_PAPERFEED_FACE_UP ? 1 : 0);
		iPaperFeedCode &= ~FACE_MASK;
		iPaperFeedCode |= (iFaceUp << FACE_SHIFT);
	}

	//////////////////////////////////////////////////////////////////////
	static void SetEdge(int& iPaperFeedCode, WORD wID)
	{
		iPaperFeedCode &= ~EDGE_MASK;
		iPaperFeedCode |= (((wID - IDC_PAPERFEED_EDGE_LEFT) << EDGE_SHIFT) & EDGE_MASK);
	}

	//////////////////////////////////////////////////////////////////////
	static void SetFeed(int& iPaperFeedCode, WORD wID)
	{
		int iFeedBottom = (wID == IDC_PAPERFEED_BOTTOM ? 0 : 1);
		iPaperFeedCode &= ~FEED_MASK;
		iPaperFeedCode |= (iFeedBottom << FEED_SHIFT);
	}

	//////////////////////////////////////////////////////////////////////
	static int GetPos(int iPaperFeedCode)
	{
		Verify(iPaperFeedCode);
		return IDC_PAPERFEED_POS_LEFT + ((iPaperFeedCode & POS_MASK) >> POS_SHIFT);
	}

	//////////////////////////////////////////////////////////////////////
	static int GetFace(int iPaperFeedCode)
	{
		Verify(iPaperFeedCode);
		bool bFaceUp = (iPaperFeedCode & FACE_MASK ? true : false);
		return (bFaceUp ? IDC_PAPERFEED_FACE_UP : IDC_PAPERFEED_FACE_DOWN);
	}

	//////////////////////////////////////////////////////////////////////
	static int GetEdge(int iPaperFeedCode)
	{
		Verify(iPaperFeedCode);
		return IDC_PAPERFEED_EDGE_LEFT + ((iPaperFeedCode & EDGE_MASK) >> EDGE_SHIFT);
	}

	//////////////////////////////////////////////////////////////////////
	static int GetFeed(int iPaperFeedCode)
	{
		Verify(iPaperFeedCode);
		bool bFeedBottom = (iPaperFeedCode & FEED_MASK ? false : true);
		return (bFeedBottom ? IDC_PAPERFEED_BOTTOM : IDC_PAPERFEED_TOP);
	}

	//////////////////////////////////////////////////////////////////////
	static int GetEnvelopeIconEx(int iPaperFeedCode)
	{
		Verify(iPaperFeedCode);
		const int iIconArray[] = {
		/*   D P  F E	*/
		/* 0 0 00 0 00*/	IDB_ENV_BLDL,
		/* 1 0 00 0 01*/	IDB_ENV_BLDR,
		/* 2 0 00 0 10*/	IDB_ENV_BLDT,
		/* 3 0 00 0 11*/	0,
		/* 4 0 00 1 00*/	IDB_ENV_BLUL,
		/* 5 0 00 1 01*/	IDB_ENV_BLUR,
		/* 6 0 00 1 10*/	IDB_ENV_BLUT,
		/* 7 0 00 1 11*/	0,
		/* 8 0 01 0 00*/	IDB_ENV_BCDL,
		/* 9 0 01 0 01*/	IDB_ENV_BCDR,
		/*10 0 01 0 10*/	IDB_ENV_BCDT,
		/*11 0 01 0 11*/	0,
		/*12 0 01 1 00*/	IDB_ENV_BCUL,
		/*13 0 01 1 01*/	IDB_ENV_BCUR,
		/*14 0 01 1 10*/	IDB_ENV_BCUT,
		/*15 0 01 1 11*/	0,
		/*16 0 10 0 00*/	IDB_ENV_BRDL,
		/*17 0 10 0 01*/	IDB_ENV_BRDR,
		/*18 0 10 0 10*/	IDB_ENV_BRDT,
		/*19 0 10 0 11*/	0,
		/*20 0 10 1 00*/	IDB_ENV_BRUL,
		/*21 0 10 1 01*/	IDB_ENV_BRUR,
		/*22 0 10 1 10*/	IDB_ENV_BRUT,
		/*23 0 10 1 11*/	0,
		/*24 0 11 0 00*/	0,
		/*25 0 11 0 01*/	0,
		/*26 0 11 0 10*/	0,
		/*27 0 11 0 11*/	0,
		/*28 0 11 1 00*/	0,
		/*29 0 11 1 01*/	0,
		/*30 0 11 1 10*/	0,
		/*31 0 11 1 11*/	0,
		/*32 1 00 0 00*/	IDB_ENV_TLDL,
		/*33 1 00 0 01*/	IDB_ENV_TLDR,
		/*34 1 00 0 10*/	IDB_ENV_TLDT,
		/*35 1 00 0 11*/	0,
		/*36 1 00 1 00*/	IDB_ENV_TLUR,
		/*37 1 00 1 01*/	IDB_ENV_TLUL,
		/*38 1 00 1 10*/	IDB_ENV_TLUT,
		/*39 1 00 1 11*/	0,
		/*40 1 01 0 00*/	IDB_ENV_TCDL,
		/*41 1 01 0 01*/	IDB_ENV_TCDR,
		/*42 1 01 0 10*/	IDB_ENV_TCDT,
		/*43 1 01 0 11*/	0,
		/*44 1 01 1 00*/	IDB_ENV_TCUR,
		/*45 1 01 1 01*/	IDB_ENV_TCUL,
		/*46 1 01 1 10*/	IDB_ENV_TCUT,
		/*47 1 01 1 11*/	0,
		/*48 1 10 0 00*/	IDB_ENV_TRDL,
		/*49 1 10 0 01*/	IDB_ENV_TRDR,
		/*50 1 10 0 10*/	IDB_ENV_TRDT,
		/*51 1 10 0 11*/	0,
		/*52 1 10 1 00*/	IDB_ENV_TRUR,
		/*53 1 10 1 01*/	IDB_ENV_TRUL,
		/*54 1 10 1 10*/	IDB_ENV_TRUT,
		/*55 1 10 1 11*/	0,
		/*56 1 11 0 00*/	0,
		/*57 1 11 0 01*/	0,
		/*58 1 11 0 10*/	0,
		/*59 1 11 0 11*/	0,
		/*60 1 11 1 00*/	0,
		/*61 1 11 1 01*/	0,
		/*62 1 11 1 10*/	0,
		/*63 1 11 1 11*/	0,
		};

		return iIconArray[iPaperFeedCode];
	}

	//////////////////////////////////////////////////////////////////////
	//static int GetEnvelopeIcon(int iPaperFeedCode)
	//{
	//	Verify(iPaperFeedCode);
	//	const int iIconArray[] = {
	//	/*   P  F E	*/
	//	/* 0 00 0 00*/	IDB_ENV_LDL,
	//	/* 1 00 0 01*/	IDB_ENV_LDR,
	//	/* 2 00 0 10*/	IDB_ENV_LDT,
	//	/* 3 00 0 11*/	0,
	//	/* 4 00 1 00*/	IDB_ENV_LUL,
	//	/* 5 00 1 01*/	IDB_ENV_LUR,
	//	/* 6 00 1 10*/	IDB_ENV_LUT,
	//	/* 7 00 1 11*/	0,
	//	/* 8 01 0 00*/	IDB_ENV_CDL,
	//	/* 9 01 0 01*/	IDB_ENV_CDR,
	//	/*10 01 0 10*/	IDB_ENV_CDT,
	//	/*11 01 0 11*/	0,
	//	/*12 01 1 00*/	IDB_ENV_CUL,
	//	/*13 01 1 01*/	IDB_ENV_CUR,
	//	/*14 01 1 10*/	IDB_ENV_CUT,
	//	/*15 01 1 11*/	0,
	//	/*16 10 0 00*/	IDB_ENV_RDL,
	//	/*17 10 0 01*/	IDB_ENV_RDR,
	//	/*18 10 0 10*/	IDB_ENV_RDT,
	//	/*19 10 0 11*/	0,
	//	/*20 10 1 00*/	IDB_ENV_RUL,
	//	/*21 10 1 01*/	IDB_ENV_RUR,
	//	/*22 10 1 10*/	IDB_ENV_RUT,
	//	/*23 10 1 11*/	0,
	//	/*24 11 0 00*/	0,
	//	/*25 11 0 01*/	0,
	//	/*26 11 0 10*/	0,
	//	/*27 11 0 11*/	0,
	//	/*28 11 1 00*/	0,
	//	/*29 11 1 01*/	0,
	//	/*30 11 1 10*/	0,
	//	/*31 11 1 11*/	0,
	//	};

	//	return iIconArray[iPaperFeedCode];

	//}

	//////////////////////////////////////////////////////////////////////
	static int GetPositionIcon(int iPaperFeedCode)
	{
		Verify(iPaperFeedCode);
		bool bBottomFeed = (GetFeed(iPaperFeedCode) == IDC_PAPERFEED_BOTTOM);

		if (GetPos(iPaperFeedCode) == IDC_PAPERFEED_POS_LEFT)
			return (bBottomFeed ? IDB_FEED_LEFT : IDB_TOP_FEED_LEFT);
		if (GetPos(iPaperFeedCode) == IDC_PAPERFEED_POS_CENTER)
			return (bBottomFeed ? IDB_FEED_CENTER : IDB_TOP_FEED_CENTER);
		if (GetPos(iPaperFeedCode) == IDC_PAPERFEED_POS_RIGHT)
			return (bBottomFeed ? IDB_FEED_RIGHT : IDB_TOP_FEED_RIGHT);

		return 0;
	}
};
