/*=======================================================================*\

	CONVERT.H - Display Converter header file.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef _CONVERT_H_
#define _CONVERT_H_

#ifndef C_CODE
extern "C" {
#endif
extern BYTE Matrix[256];
#ifndef C_CODE
}
#endif
extern BYTE Pattern[64];

#define THRESH 128

#define RGB_FROM_CMYK(R, G, B, C, M, Y, K) \
{ \
	R = C^0xFF; \
	G = M^0xFF; \
	B = Y^0xFF; \
	if (K) \
	{ \
		long k = K; \
		R -= (k * R)>>8; \
		G -= (k * G)>>8; \
		B -= (k * B)>>8; \
	} \
}

/*=========================================================================*/

#define RED_0  0
#define RED_1  36
#define RED_2  72
#define RED_3  108
#define RED_4  144
#define RED_5  180

#define GRN_0  0
#define GRN_1  6
#define GRN_2  12
#define GRN_3  18
#define GRN_4  24
#define GRN_5  30

#define BLU_0  0
#define BLU_1  1
#define BLU_2  2
#define BLU_3  3
#define BLU_4  4
#define BLU_5  5

#define VALUE0	    0
#define VALUE1    51
#define VALUE2   102
#define VALUE3   153
#define VALUE4   204
#define VALUE5   255

#define HandleREDComponent(COLOR, OUTVALUE) \
	if (COLOR < 128) {                  \
		if (COLOR < 26) {               \
		/*	OUTVALUE += RED_0;       */ \
		/*	COLOR    -= VALUE0;      */ \
		} else                          \
		if (COLOR < 77) {               \
			OUTVALUE += RED_1;          \
			COLOR    -= VALUE1;         \
		} else {                        \
			OUTVALUE += RED_2;          \
			COLOR    -= VALUE2;         \
		}                               \
	} else {                            \
		if (COLOR < 179) {              \
			OUTVALUE += RED_3;          \
			COLOR    -= VALUE3;         \
		} else                          \
		if (COLOR < 230) {              \
			OUTVALUE += RED_4;          \
			COLOR    -= VALUE4;         \
		} else {                        \
			OUTVALUE += RED_5;          \
			COLOR    -= VALUE5;         \
		}                               \
	}

#define HandleGRNComponent(COLOR, OUTVALUE) \
	if (COLOR < 128) {                  \
		if (COLOR < 26) {               \
		/*	OUTVALUE += GRN_0;       */ \
		/*	COLOR    -= VALUE0;      */ \
		} else                          \
		if (COLOR < 77) {               \
			OUTVALUE += GRN_1;          \
			COLOR    -= VALUE1;         \
		} else {                        \
			OUTVALUE += GRN_2;          \
			COLOR    -= VALUE2;         \
		}                               \
	} else {                            \
		if (COLOR < 179) {              \
			OUTVALUE += GRN_3;          \
			COLOR    -= VALUE3;         \
		} else                          \
		if (COLOR < 230) {              \
			OUTVALUE += GRN_4;          \
			COLOR    -= VALUE4;         \
		} else {                        \
			OUTVALUE += GRN_5;          \
			COLOR    -= VALUE5;         \
		}                               \
	}

#define HandleBLUComponent(COLOR, OUTVALUE) \
	if (COLOR < 128) {                  \
		if (COLOR < 26) {               \
		/*	OUTVALUE += BLU_0;       */ \
		/*	COLOR    -= VALUE0;      */ \
		} else                          \
		if (COLOR < 77) {               \
			OUTVALUE += BLU_1;          \
			COLOR    -= VALUE1;         \
		} else {                        \
			OUTVALUE += BLU_2;          \
			COLOR    -= VALUE2;         \
		}                               \
	} else {                            \
		if (COLOR < 179) {              \
			OUTVALUE += BLU_3;          \
			COLOR    -= VALUE3;         \
		} else                          \
		if (COLOR < 230) {              \
			OUTVALUE += BLU_4;          \
			COLOR    -= VALUE4;         \
		} else {                        \
			OUTVALUE += BLU_5;          \
			COLOR    -= VALUE5;         \
		}                               \
	}

#endif // _CONVERT_H_

