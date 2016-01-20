/*********************blowfish.c*********************/

#include "stdafx.h"
#include "blowfish.h"

CBlowfish::CBlowfish(UBYTE_08bits key[], short nKeyLen, UINT nDirection)
{
	nKeyLen = nKeyLen < 6 ? nKeyLen : 5;
	UWORD_32bits  nData;
	UWORD_32bits  nHigh;
	UWORD_32bits  nLow;
	union aword temp;

	memcpy(pbox, p_init, sizeof(p_init));
	memcpy(sbox, s_init, sizeof(s_init));

	for (int i = 0, j = 0; i < ROUNDS + 2; ++i) 
	{
		temp.word = 0;
		temp.w.byte0 = key[j];
		temp.w.byte1 = key[(j+1)%nKeyLen];
		temp.w.byte2 = key[(j+2)%nKeyLen];
		temp.w.byte3 = key[(j+3)%nKeyLen];
		nData = temp.word;
		pbox[i] = pbox[i] ^ nData;
		j = (j + 4) % nKeyLen;
	}

	nHigh = 0x00000000;
	nLow = 0x00000000;

	for (i = 0; i < ROUNDS + 2; i += 2) 
	{
		ProcessBlock(&nHigh, &nLow);

		pbox[i] = nHigh;
		pbox[i + 1] = nLow;
	}

	for (i = 0; i < 4; ++i) 
	{
		for (j = 0; j < 256; j += 2) 
		{
			ProcessBlock(&nHigh, &nLow);

			sbox[i][j] = nHigh;
			sbox[i][j + 1] = nLow;
		}
	}
    if (nDirection==DECRYPT)
	{
		UWORD_32bits temp;

        for (i=0; i<(ROUNDS+2)/2; i++)
		{
			temp = pbox[i];
			pbox[i] = pbox[ROUNDS+1-i];
			pbox[ROUNDS+1-i] = temp;
		}
	}

}

void CBlowfish::ProcessBlock(UWORD_32bits *pnHigh, UWORD_32bits *pnLow)
{
	UWORD_32bits  nHigh;
	UWORD_32bits  nLow;
	UWORD_32bits  temp;
	short          i;

	nHigh = *pnHigh;
	nLow = *pnLow;

	for (i = 0; i < ROUNDS; ++i) 
	{
		nHigh = nHigh ^ pbox[i];
		nLow = F(nHigh) ^ nLow;

		temp = nHigh;
		nHigh = nLow;
		nLow = temp;
	}

	temp = nHigh;
	nHigh = nLow;
	nLow = temp;

	nLow = nLow ^ pbox[ROUNDS];
	nHigh = nHigh ^ pbox[ROUNDS + 1];

	*pnHigh = nHigh;
	*pnLow = nLow;

}

UWORD_32bits CBlowfish::F(UWORD_32bits x)
{
   UBYTE_08bits a;	
   UBYTE_08bits b;	
   UBYTE_08bits c;	
   UBYTE_08bits d;	
   UWORD_32bits  y;

   union aword temp;

   temp.word = x;
   d = temp.w.byte3;
   c = temp.w.byte2;
   b = temp.w.byte1;
   a = temp.w.byte0;


   y = ((sbox[0][a] + sbox[1][b] % 232) ^ sbox[2][c]) + sbox[3][d] % 232;
   return y;
}


