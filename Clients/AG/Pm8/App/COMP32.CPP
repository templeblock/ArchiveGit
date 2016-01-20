/*
// COMP32.CPP
//
// Compress/uncompress routines for Win32 version.
//
// $Header: /PM8/App/COMP32.CPP 1     3/03/99 6:04p Gbeddow $
//
// $Log: /PM8/App/COMP32.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:46   Fred
// Initial revision.
// 
//    Rev 1.2   01 Jul 1997 15:46:14   Jay
// Now checks return code from NextByte() in uncompress().
// 
//    Rev 1.1   08 Nov 1996 08:57:38   Jay
// Removed 16-bit compress.obj
// 
//    Rev 1.0   14 Mar 1996 13:45:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:10   FRED
// Initial revision.
// 
//    Rev 1.2   18 Sep 1995 08:25:40   JAY
// Made PackBits work.
// 
//    Rev 1.1   15 Sep 1995 17:04:38   JAY
// Fixed (CCITT) uncompress().
// 
//    Rev 1.0   15 Sep 1995 15:30:28   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "compress.h"

BYTE code0msk[] =
{
	0x00,
	0x80,
	0xc0,
	0xe0,
	0xf0,
	0xf8,
	0xfc,
	0xfe,
	0xff
};

BYTE code1msk[] =
{
	0xff,
	0x7f,
	0x3f,
	0x1f,
	0x0f,
	0x07,
	0x03,
	0x01,
	0x00
};

enum CCITTStateLabel
{
	eol = -32768,
	zero = -32767,
	nul = 0,
	white,
	w01,
	w011,
	w0111,
	w01111,
	w011111,
	w012,
	w0121,
	w02,
	w021,
	w0211,
	w02111,
	w03,
	w1,
	w11,
	w111,
	w1111,
	w11111,
	w111111,
	w1111111,
	w11112,
	w111121,
	w1112,
	w11121,
	w111211,
	w11122,
	w112,
	w1121,
	w11211,
	w112111,
	w1122,
	w11221,
	w112211,
	w11222,
	w12,
	w121,
	w1211,
	w12111,
	w121111,
	w1211111,
	w121112,
	w12112,
	w121121,
	w1212,
	w12121,
	w121211,
	w12122,
	w122,
	w1221,
	w12211,
	w1222,
	w12221,
	w2,
	w21,
	w211,
	w2111,
	w21111,
	w211111,
	w21112,
	w2112,
	w21121,
	w212,
	w2121,
	w21211,
	w22,
	w221,
	w2211,
	w22111,
	w2212,
	w222,
	w2221,
	w3,
	w31,
	w311,
	w3111,
	w31111,
	w3112,
	w312,
	w3121,
	w32,
	w321,
	w3211,
	w4,
	w41,
	w411,
	w4111,
	w5,
	w51,
	w511,
	w6,
	w61,
	w7,
	w71,
	w711,
	w7111,
	w7112,
	w712,
	w7121,
	w7122,
	w72,
	w721,
	w722,
	w7221,
	w7222,
	w8,
	w9,
	w10,
	we,

	black,
	b01,
	b1,
	b11,
	b2,
	b21,
	b3,
	b31,
	b311,
	b4,
	b41,
	b411,
	b42,
	b421,
	b4211,
	b42111,
	b421111,
	b4211111,
	b421112,
	b42112,
	b421121,
	b4212,
	b42121,
	b421211,
	b422,
	b4221,
	b42211,
	b422111,
	b42212,
	b4222,
	b42221,
	b5,
	b51,
	b511,
	b5111,
	b51111,
	b511111,
	b5111111,
	b511112,
	b51112,
	b511121,
	b5112,
	b51121,
	b511211,
	b51122,
	b52,
	b521,
	b5211,
	b52111,
	b521111,
	b52112,
	b5212,
	b52121,
	b522,
	b5221,
	b52211,
	b5222,
	b6,
	b61,
	b611,
	b6111,
	b61111,
	b611111,
	b6111111,
	b61112,
	b611121,
	b6112,
	b61121,
	b611211,
	b612,
	b6121,
	b61211,
	b612111,
	b6122,
	b61221,
	b62,
	b621,
	b6211,
	b62111,
	b6212,
	b62121,
	b622,
	b6221,
	b62211,
	b63,
	b631,
	b6311,
	b63111,
	b6312,
	b632,
	b6321,
	b7,
	b8,
	b9,
	b10,
	be
};

struct CCITTState
{
	short int n0State;
	short int n1State;
};

static CCITTState State[] =
{
	{ nul, nul },				// nul
	{ w1, w01 },				// white
	{ w011, w02 },				// w01
	{ w012, w0111 },			// w011
	{ w01111, -4 },			// w0111
	{ -9, w011111 },			// w01111
	{ -16, -17 },				// w011111
	{ -3, w0121 },				// w012
	{ -128, -8 },				// w0121
	{ w021, w03 },				// w02
	{ -5, w0211 },				// w021
	{ w02111, -64 },			// w0211
	{ -14, -15 },				// w02111
	{ -6, -7 },					// w03
	{ w2, w11 },				// w1
	{ w111, w12 },				// w11
	{ w112, w1111 },			// w111
	{ w11111, w1112 },		// w1111
	{ w11112, w111111 },		// w11111
	{ w1111111, -25 },		// w111111
	{ -51, -52 },				// w1111111
	{ -24, w111121 },			// w11112
	{ -49, -50 },				// w111121
	{ w11121, -192 },			// w1112
	{ w11122, w111211 },		// w11121
	{ -57, -58 },				// w111211
	{ -55, -56 },				// w11122
	{ -11, w1121 },			// w112
	{ w11211, w1122 },		// w1121
	{ -27, w112111 },			// w11211
	{ -59, -60 },				// w112111
	{ w11221, -18 },			// w1122
	{ w11222, w112211 },		// w11221
	{ -1600, -1728 },			// w112211
	{ -1472, -1536 },			// w11222
	{ w121, -2 },				// w12
	{ w122, w1211 },			// w121
	{ w12111, w1212 },		// w1211
	{ w12112, w121111 },		// w12111
	{ w1211111, w121112 },	// w121111
	{ -960, -1024 },			// w1211111
	{ -1088, -1152 },			// w121112
	{ -576, w121121 },		// w12112
	{ -832, -896 },			// w121121
	{ w12121, -256 },			// w1212
	{ w12122, w121211 },		// w12121
	{ -1344, -1408 },			// w121211
	{ -1216, -1280 },			// w12122
	{ -1664, w1221 },			// w122
	{ w12211, w1222 },		// w1221
	{ -448, -512 },			// w12211
	{ w12221, -640 },			// w1222
	{ -704, -768 },			// w12221
	{ w3, w21 },				// w2
	{ w211, w22 },				// w21
	{ w212, w2111 },			// w211
	{ w21111, w2112 },		// w2111
	{ w21112, w211111 },		// w21111
	{ -41, -42 },				// w211111
	{ -39, -40 },				// w21112
	{ w21121, -21 },			// w2112
	{ -43, -44 },				// w21121
	{ -12, w2121 },			// w212
	{ w21211, -26 },			// w2121
	{ -53, -54 },				// w21211
	{ w221, -10 },				// w22
	{ w222, w2211 },			// w221
	{ w22111, w2212 },		// w2211
	{ -63, zero },				// w22111
	{ -320, -384 },			// w2212
	{ -28, w2221 },			// w222
	{ -61, -62 },				// w2221
	{ w4, w31 },				// w3
	{ w311, w32 },				// w31
	{ w312, w3111 },			// w311
	{ w31111, w3112 },		// w3111
	{ -35, -36 },				// w31111
	{ -37, -38 },				// w3112
	{ -20, w3121 },			// w312
	{ -33, -34 },				// w3121
	{ w321, -1 },				// w32
	{ -19, w3211 },			// w321
	{ -31, -32 },				// w3211
	{ w5, w41 },				// w4
	{ w411, -13 },				// w41
	{ -23, w4111 },			// w411
	{ -47, -48 },				// w4111
	{ w6, w51 },				// w5
	{ w511, -22 },				// w51
	{ -45, -46 },				// w511
	{ w7, w61 },				// w6
	{ -29, -30 },				// w61
	{ w8, w71 },				// w7
	{ w711, w72 },				// w71
	{ w7111, w712 },			// w711
	{ -1792, w7112 },			// w7111
	{ -1984, -2048 },			// w7112
	{ w7121, w7122 },			// w712
	{ -2112, -2176 },			// w7121
	{ -2240, -2304 },			// w7122
	{ w721, w722 },			// w72
	{ -1856, -1920 },			// w721
	{ w7221, w7222 },			// w722
	{ -2368, -2432 },			// w7221
	{ -2496, nul },			// w7222
	{ w9, nul },				// w8
	{ w10, nul },				// w9
	{ we, nul },				// w10
	{ nul, eol },				// we

	{ b1, b01 },				// black
	{ -3, -2 },					// b01
	{ b2, b11 },				// b1
	{ -1, -4 },					// b11
	{ b3, b21 },				// b2
	{ -6, -5 },					// b21
	{ b4, b31 },				// b3
	{ b311, -7 },				// b31
	{ -9, -8 },					// b311
	{ b5, b41 },				// b4
	{ b411, b42 },				// b41
	{ -10, -11 },				// b411
	{ b421, -12 },				// b42
	{ b422, b4211 },			// b421
	{ b42111, b4212 },		// b4211
	{ b42112, b421111 },		// b42111
	{ b4211111, b421112 },	// b421111
	{ -36, -37 },				// b4211111
	{ -38, -39 },				// b421112
	{ -20, b421121 },			// b42112
	{ -34, -35 },				// b421121
	{ b42121, zero },			// b4212
	{ -21, b421211 },			// b42121
	{ -42, -43 },				// b421211
	{ -15, b4221 },			// b422
	{ b42211, b4222 },		// b4221
	{ b42212, b422111 },		// b42211
	{ -26, -27 },				// b422111
	{ -128, -192 },			// b42212
	{ b42221, -19 },			// b4222
	{ -28, -29 },				// b42221
	{ b6, b51 },				// b5
	{ b511, b52 },				// b51
	{ -13, b5111 },			// b511
	{ b51111, b5112 },		// b5111
	{ b51112, b511111 },		// b51111
	{ b5111111, b511112 },	// b511111
	{ -44, -45 },				// b5111111
	{ -46, -47 },				// b511112
	{ -23, b511121 },			// b51112
	{ -50, -51 },				// b511121
	{ b51121, -16 },			// b5112
	{ b51122, b511211 },		// b51121
	{ -61, -256 },				// b511211
	{ -57, -58 },				// b51122
	{ b521, -14 },				// b52
	{ b522, b5211 },			// b521
	{ b52111, b5212 },		// b5211
	{ b52112, b521111 },		// b52111
	{ -32, -33 },				// b521111
	{ -30, -31 },				// b52112
	{ b52121, -22 },			// b5212
	{ -40, -41 },				// b52121
	{ -17, b5221 },			// b522
	{ b52211, b5222 },		// b5221
	{ -48, -49 },				// b52211
	{ -62, -63 },				// b5222
	{ b7, b61 },				// b6
	{ b611, b62 },				// b61
	{ b612, b6111 },			// b611
	{ b61111, b6112 },		// b6111
	{ b61112, b611111 },		// b61111
	{ b6111111, -59 },		// b611111
	{ -1408, -1472 },			// b6111111
	{ -56, b611121 },			// b61112
	{ -1280, -1344 },			// b611121
	{ b61121, -24 },			// b6112
	{ -60, b611211 },			// b61121
	{ -1536, -1600 },			// b611211
	{ -18, b6121 },			// b612
	{ b61211, b6122 },		// b6121
	{ -52, b612111 },			// b61211
	{ -640, -704 },			// b612111
	{ b61221, -55 },			// b6122
	{ -768, -832 },			// b61221
	{ b621, b63 },				// b62
	{ b622, b6211 },			// b621
	{ b62111, b6212 },		// b6211
	{ -384, -448 },			// b62111
	{ b62121, -53 },			// b6212
	{ -512, -576 },			// b62121
	{ -25, b6221 },			// b622
	{ b62211, -320 },			// b6221
	{ -1664, -1728 },			// b62211
	{ b631, -64 },				// b63
	{ b632, b6311 },			// b631
	{ b63111, b6312 },		// b6311
	{ -1024, -1088 },			// b63111
	{ -1152, -1216 },			// b6312
	{ -54, b6321 },			// b632
	{ -896, -960 },			// b6321
	{ b8, w71 },				// b7
	{ b9, nul },				// b8
	{ b10, nul },				// b9
	{ be, nul },				// b10
	{ nul, eol }				// be
};

/*
// Uncompress some data in CCITT format.
*/

SHORT uncompress(BYTE_FUNC NextByte, LPBYTE pBuffer, SHORT nBits)
{
	BYTE bBit = 0;
	int nPutBit = 0;

	BYTE bByte;
	int nState;

	while (nBits > 0)
	{
	/*
	// Phase 1: Decompress the white run.
	*/

		do
		{
			nState = white;
			do
			{
				if (bBit == 0)
				{
				/* Load the next byte. */
					if (!NextByte(&bByte))
               {
                  goto EndState;
               }
					bBit = 0x80;
				}

				nState = (bByte & bBit) ? State[nState].n1State : State[nState].n0State;
				bBit >>= 1;
			} while (nState > 0);

			if (nState == eol)
			{
			/* We want to end. */
				goto EndState;
			}

			if (nState != zero)
			{
				if (nBits == 0)
				{
					break;
				}
			/* We want to put '-nState' zero bits. */
				nState = -nState;
				int nBitsToPut = nState;			// Get count of bits.

				if ((nBits -= nBitsToPut) < 0)
				{
					nBitsToPut += nBits;			// nBits < 0
					nBits = 0;
				}

				*pBuffer &= code0msk[nPutBit];
				nBitsToPut += nPutBit;
				nPutBit = nBitsToPut & 7;

				while (nBitsToPut >= 8)
				{
					*++pBuffer = 0;
					nBitsToPut -= 8;
				}
			}
		} while (nState >= 64 /*&& nBits != 0*/);

		if (nBits == 0)
		{
			break;
		}

	/*
	// Phase 2: Decompress the black run.
	*/

		do
		{
			nState = black;
			do
			{
				if (bBit == 0)
				{
				/* Load the next byte. */
					if (!NextByte(&bByte))
               {
                  goto EndState;
               }
					bBit = 0x80;
				}
				nState = (bByte & bBit) ? State[nState].n1State : State[nState].n0State;
				bBit >>= 1;
			} while (nState > 0);

			if (nState == eol)
			{
			/* We want to end. */
				goto EndState;
			}

			if (nState != zero)
			{
				if (nBits == 0)
				{
					break;
				}

			/* We want to put '-nState' one bits. */
				nState = -nState;
				int nBitsToPut = nState;			// Get count of bits.

				if ((nBits -= nBitsToPut) < 0)
				{
					nBitsToPut += nBits;			// nBits < 0
					nBits = 0;
				}
			/* */
				*pBuffer |= code1msk[nPutBit];
				nBitsToPut += nPutBit;
				nPutBit = nBitsToPut & 7;

				while (nBitsToPut >= 8)
				{
					*++pBuffer = 0xff;
					nBitsToPut -= 8;
				}
			}
		} while (nState >= 64 /*&& nBits != 0*/);
	}

EndState:

	return (nBits == 0);
}

/*
// Uncompress some data in PackBits format.
*/

BOOL unPackBits(BYTE_FUNC NextByte, LPBYTE pBuffer, SHORT nBytes)
{
	while (nBytes > 0)
	{
		char count;
		BYTE data;

		if (!NextByte((BYTE*)&count))
		{
			return FALSE;
		}
		if (count < 0)
		{
		/* Repeat count */
			if (!NextByte(&data))
			{
				return FALSE;
			}
			count = -count;
			while ((count >= 0) && (nBytes > 0))
			{
				*pBuffer++ = data;
				nBytes--;
				count--;
			}
		}
		else
		{
		/* Replace count. */
			while ((count >= 0) && (nBytes > 0))
			{
				if (!NextByte(&data))
				{
					return FALSE;
				}
				*pBuffer++ = data;
				nBytes--;
				count--;
			}
		}
	}
	return TRUE;
}
