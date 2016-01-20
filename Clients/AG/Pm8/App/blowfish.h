
/*********************blowfish.h********************/

/* $Id: blowfish.h,v 1.2 1994/09/22 04:28:43 cj Exp cj $*/
#ifndef Blowfish_h
#define Blowfish_h


#define UWORD_32bits  unsigned long
#define UBYTE_08bits  unsigned char
#define ROUNDS 16


class CBlowfish
{
public:
	CBlowfish(UBYTE_08bits key[], short nKeyLen, UINT nDirection);
	void ProcessBlock(UWORD_32bits *pnHigh, UWORD_32bits *pnLow);

	enum
	{
		ENCRYPT = 0,
		DECRYPT
	};
protected:
	UWORD_32bits F(UWORD_32bits x);
	union aword 
	{
		UWORD_32bits word;
		UBYTE_08bits byte [4];
		struct 
		{
			UBYTE_08bits byte3;
			UBYTE_08bits byte2;
			UBYTE_08bits byte1;
			UBYTE_08bits byte0;
		} w;
	};
    UWORD_32bits pbox[ROUNDS+2];
    UWORD_32bits sbox[4][256];
    static const UWORD_32bits p_init[ROUNDS+2];
    static const UWORD_32bits s_init[4*256];

};


class CBlowfishEncrypt : public CBlowfish
{
public:
	CBlowfishEncrypt(UBYTE_08bits key[], short nKeyLen) :
	  CBlowfish(key, nKeyLen, ENCRYPT){}
};

class CBlowfishDecrypt : public CBlowfish
{
public:
	CBlowfishDecrypt(UBYTE_08bits key[], short nKeyLen) :
	  CBlowfish(key, nKeyLen, DECRYPT){}
};

#endif
