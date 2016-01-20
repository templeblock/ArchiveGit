/**********************************************************************/
/* crypt_eve.h														  */
/* encrypt and decrypt the passwords to remotly program the dongle    */
/* James Vogh														  */
/**********************************************************************/

#ifndef IN_CRYPT_EVE
#define IN_CRYPT_EVE

#include <stdio.h>

//#include "stdafx.h"

#define M1	(674538965)
#define N1	(846539530)
#define P1	(53)

#define M2	(267754384)
#define N2	(585349339)
#define P2	(38)

// for codes 1

#define FLAG_REFL	32  // reflective
#define FLAG_4X5	128	// 4x5 transparent
#define FLAG_35MM	64	// 35mm transparent
#define FLAG_BATCH	16	// MonacoBatch flag
#define FLAG_MC2	4	// MonacoColor flag
#define FLAG_MP_L	2 	// MonacoPrint 2.0
#define FLAG_MS		1	// MonacoSCAN
#define FLAG_DD		8	// Dealer Demo

// for codes 2

#define FLAG_MP_M	1	// unused
#define FLAG_MP_P	64	// MonacoPrint 2.5
#define FLAG_MV		8	// MonacoVIEW
#define FLAG_MSC	16	// Selective color in MonacoCOLOR


void encrypt(long sn,unsigned long &p1,unsigned long &p2,
	unsigned char codes1,unsigned char codes2,unsigned char count1,unsigned char count2);
long decrypt(long sn,unsigned long p1,unsigned long p2,
	unsigned char &codes1,unsigned char &codes2,unsigned char &count1,unsigned char &count2);

#endif