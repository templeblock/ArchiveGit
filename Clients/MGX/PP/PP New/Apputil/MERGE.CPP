//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "merge.h"

//#define C_CODE 1
#define CMYK_C_CODE 1

static void ProcessNormal8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessNormal8P(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessNormal24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);

static LPPROCESSPROC GetProcessProc1(MERGE_MODE MergeMode);
static LPPROCESSPROC GetProcessProc8(MERGE_MODE MergeMode);
static LPPROCESSPROC GetProcessProc8P(MERGE_MODE MergeMode);
static LPPROCESSPROC GetProcessProc24(MERGE_MODE MergeMode);
static LPPROCESSPROC GetProcessProc32(MERGE_MODE MergeMode);
static void ProcessInvert1(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessAdditive8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessSubtractive8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessRed8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessGreen8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessBlue8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessCyan8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessMagenta8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessYellow8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessHueSat8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessHue8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessSat8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessLum8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessIfLighter8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessIfDarker8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessMultiply8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessScreen8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessDifference8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessInvert8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
static void ProcessAdditive24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessSubtractive24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessRed24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessGreen24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessBlue24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessCyan24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessMagenta24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessYellow24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessHueSat24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessHue24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessSat24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessLum24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessIfLighter24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessIfDarker24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessMultiply24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessScreen24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessDifference24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessTexturize24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessInvert24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount);
static void ProcessNormal32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessAdditive32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessSubtractive32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessRed32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessGreen32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessBlue32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessCyan32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessMagenta32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessYellow32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessBlack32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessHueSat32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessHue32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessSat32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessLum32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessIfLighter32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessIfDarker32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessMultiply32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessScreen32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessDifference32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessTexturize32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);
static void ProcessInvert32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount);

/************************************************************************/
LPPROCESSPROC GetProcessProc(MERGE_MODE MergeMode, FRMDATATYPE Type)
/************************************************************************/
{
switch (Type)
	{
	case FDT_CMYKCOLOR:
		return(GetProcessProc32(MergeMode));
	case FDT_RGBCOLOR:
		return(GetProcessProc24(MergeMode));
	case FDT_LINEART:
		return(GetProcessProc1(MergeMode));
	case FDT_PALETTECOLOR:
		return(GetProcessProc8P(MergeMode));
	case FDT_GRAYSCALE:
	default:
		break;
	}
return(GetProcessProc8(MergeMode));
}

/************************************************************************/
static LPPROCESSPROC GetProcessProc1(MERGE_MODE MergeMode)
/************************************************************************/
{
switch (MergeMode)
	{
	case MM_INVERT:
		return((LPPROCESSPROC)ProcessInvert1);
	default:
		break;
	}
return((LPPROCESSPROC)ProcessNormal8P);
}

/************************************************************************/
static LPPROCESSPROC GetProcessProc8(MERGE_MODE MergeMode)
/************************************************************************/
{
switch (MergeMode)
	{
	case MM_ADDITIVE:
		return((LPPROCESSPROC)ProcessAdditive8);
	case MM_SUBTRACTIVE:
		return((LPPROCESSPROC)ProcessSubtractive8);
	case MM_IFLIGHTER:
		return((LPPROCESSPROC)ProcessIfLighter8);
	case MM_IFDARKER:
		return((LPPROCESSPROC)ProcessIfDarker8);
	case MM_MULTIPLY:
		return((LPPROCESSPROC)ProcessMultiply8);
	case MM_SCREEN:
		return((LPPROCESSPROC)ProcessScreen8);
	case MM_DIFFERENCE:
		return((LPPROCESSPROC)ProcessDifference8);
	case MM_TEXTURIZE:
		return((LPPROCESSPROC)ProcessMultiply8);
	case MM_INVERT:
		return((LPPROCESSPROC)ProcessInvert8);
	default:
		break;
	}
return((LPPROCESSPROC)ProcessNormal8);
}

/************************************************************************/
static LPPROCESSPROC GetProcessProc8P(MERGE_MODE MergeMode)
/************************************************************************/
{
return((LPPROCESSPROC)ProcessNormal8P);
}

/************************************************************************/
static LPPROCESSPROC GetProcessProc24(MERGE_MODE MergeMode)
/************************************************************************/
{
switch (MergeMode)
	{
	case MM_ADDITIVE:
		return((LPPROCESSPROC)ProcessAdditive24);
	case MM_SUBTRACTIVE:
		return((LPPROCESSPROC)ProcessSubtractive24);
	case MM_RED:
		return((LPPROCESSPROC)ProcessRed24);
	case MM_GREEN:
		return((LPPROCESSPROC)ProcessGreen24);
	case MM_BLUE:
		return((LPPROCESSPROC)ProcessBlue24);
	case MM_CYAN:
		return((LPPROCESSPROC)ProcessCyan24);
	case MM_MAGENTA:
		return((LPPROCESSPROC)ProcessMagenta24);
	case MM_YELLOW:
		return((LPPROCESSPROC)ProcessYellow24);
	case MM_HUESAT:
		return((LPPROCESSPROC)ProcessHueSat24);
	case MM_HUE:
		return((LPPROCESSPROC)ProcessHue24);
	case MM_SAT:
		return((LPPROCESSPROC)ProcessSat24);
	case MM_LUM:
		return((LPPROCESSPROC)ProcessLum24);
	case MM_IFLIGHTER:
		return((LPPROCESSPROC)ProcessIfLighter24);
	case MM_IFDARKER:
		return((LPPROCESSPROC)ProcessIfDarker24);
	case MM_MULTIPLY:
		return((LPPROCESSPROC)ProcessMultiply24);
	case MM_SCREEN:
		return((LPPROCESSPROC)ProcessScreen24);
	case MM_DIFFERENCE:
		return((LPPROCESSPROC)ProcessDifference24);
	case MM_TEXTURIZE:
		return((LPPROCESSPROC)ProcessTexturize24);
	case MM_INVERT:
		return((LPPROCESSPROC)ProcessInvert24);
	default:
		break;
	}
return((LPPROCESSPROC)ProcessNormal24);
}

/************************************************************************/
static LPPROCESSPROC GetProcessProc32(MERGE_MODE MergeMode)
/************************************************************************/
{
switch (MergeMode)
	{
	case MM_ADDITIVE:
		return((LPPROCESSPROC)ProcessAdditive32);
	case MM_SUBTRACTIVE:
		return((LPPROCESSPROC)ProcessSubtractive32);
	case MM_RED:
		return((LPPROCESSPROC)ProcessRed32);
	case MM_GREEN:
		return((LPPROCESSPROC)ProcessGreen32);
	case MM_BLUE:
		return((LPPROCESSPROC)ProcessBlue32);
	case MM_CYAN:
		return((LPPROCESSPROC)ProcessCyan32);
	case MM_MAGENTA:
		return((LPPROCESSPROC)ProcessMagenta32);
	case MM_YELLOW:
		return((LPPROCESSPROC)ProcessYellow32);
	case MM_BLACK:
		return((LPPROCESSPROC)ProcessBlack32);
	case MM_HUESAT:
		return((LPPROCESSPROC)ProcessHueSat32);
	case MM_HUE:
		return((LPPROCESSPROC)ProcessHue32);
	case MM_SAT:
		return((LPPROCESSPROC)ProcessSat32);
	case MM_LUM:
		return((LPPROCESSPROC)ProcessLum32);
	case MM_IFLIGHTER:
		return((LPPROCESSPROC)ProcessIfLighter32);
	case MM_IFDARKER:
		return((LPPROCESSPROC)ProcessIfDarker32);
	case MM_MULTIPLY:
		return((LPPROCESSPROC)ProcessMultiply32);
	case MM_SCREEN:
		return((LPPROCESSPROC)ProcessScreen32);
	case MM_DIFFERENCE:
		return((LPPROCESSPROC)ProcessDifference32);
	case MM_TEXTURIZE:
		return((LPPROCESSPROC)ProcessTexturize32);
	case MM_INVERT:
		return((LPPROCESSPROC)ProcessInvert32);
	default:
		break;
	}
return((LPPROCESSPROC)ProcessNormal32);
}

#define STACK_FRAME	\
	RGBS rgb;			\
	CMYKS cmyk;			\
	HSLS hsl;

#define PD_CSETUPMASK												\
	if (sm > 127)													\
		++sm;														\
	dm = 256-sm;

#define PD_CPROCESSRGB												\
	s = (WORD)rgb.red * sm;											\
	d = (WORD)lpDst->red * dm;										\
	lpDst->red = (d + s + 128) >> 8;										\
	s = (WORD)rgb.green * sm;										\
	d = (WORD)lpDst->green * dm;									\
	lpDst->green = (d + s + 128) >> 8;									\
	s = (WORD)rgb.blue * sm;										\
	d = (WORD)lpDst->blue * dm;										\
	lpDst->blue = (d + s + 128) >> 8;

#define PD_CPROCESSRGBN												\
	s = (WORD)lpSrc->red * sm;										\
	d = (WORD)lpDst->red * dm;										\
	lpDst->red = (d + s + 128) >> 8;										\
	s = (WORD)lpSrc->green * sm;									\
	d = (WORD)lpDst->green * dm;									\
	lpDst->green = (d + s + 128) >> 8;									\
	s = (WORD)lpSrc->blue * sm;										\
	d = (WORD)lpDst->blue * dm;										\
	lpDst->blue = (d + s + 128) >> 8;

#define PD_CPROCESSCMYKN											\
	s = (WORD)lpSrc->c * sm;										\
	d = (WORD)lpDst->c * dm;										\
	lpDst->c = (d + s + 128) >> 8;										\
	s = (WORD)lpSrc->m * sm;										\
	d = (WORD)lpDst->m * dm;										\
	lpDst->m = (d + s + 128) >> 8;										\
	s = (WORD)lpSrc->y * sm;										\
	d = (WORD)lpDst->y * dm;										\
	lpDst->y = (d + s + 128) >> 8;										\
	s = (WORD)lpSrc->k * sm;										\
	d = (WORD)lpDst->k * dm;										\
	lpDst->k = (d + s + 128) >> 8;

#define PD_CPROCESSCMYK												\
	s = (WORD)cmyk.c * sm;											\
	d = (WORD)lpDst->c * dm;										\
	lpDst->c = (d + s + 128) >> 8;										\
	s = (WORD)cmyk.m * sm;											\
	d = (WORD)lpDst->m * dm;										\
	lpDst->m = (d + s + 128) >> 8;										\
	s = (WORD)cmyk.y * sm;											\
	d = (WORD)lpDst->y * dm;										\
	lpDst->y = (d + s + 128) >> 8;										\
	s = (WORD)cmyk.k * sm;											\
	d = (WORD)lpDst->k * dm;										\
	lpDst->k = (d + s + 128) >> 8;

#define PD_BEGIN													\
	{																\
	__asm mov	edi,DWORD PTR lpDst							\
	}

#define PD_END

#define PD_SETUP													\
	{																\
	__asm mov esi,DWORD PTR lpSrc								\
	__asm mov	cx,ax					/* cx = mask value */		\
	}
	
#define PD_SETSRCPTR												\
	{																\
	__asm mov DWORD PTR lpSrc,esi 									\
	}

#define PD_SETDSTPTR												\
	{																\
	__asm mov DWORD PTR lpDst,edi									\
	}

#define PD_GETMASK													\
	{																\
	__asm mov esi,DWORD PTR lpMsk									\
	__asm lodsb														\
	__asm mov DWORD PTR lpMsk,esi										\
	__asm or	al,al												\
	__asm jne	PD_HAVEMASK											\
	__asm add DWORD PTR lpSrc,ebx										\
	__asm add	edi,ebx												\
	__asm jmp	PD_NEXT												\
__asm PD_HAVEMASK:													\
	__asm sub	ah,ah												\
	__asm cmp	ax,127												\
	__asm jbe	PD_NOINC											\
	__asm inc	ax													\
__asm PD_NOINC:														\
	}

#define PD_GETMASK8P 												\
	{																\
	__asm mov esi,DWORD PTR lpMsk									\
	__asm lodsb														\
	__asm mov DWORD PTR lpMsk,esi									\
	__asm or	al,al												\
	__asm jne	PD_HAVEMASK											\
	__asm add DWORD PTR lpSrc,1 									\
	__asm inc	edi													\
	__asm jmp	PD_NEXT												\
__asm PD_HAVEMASK:													\
	}

#define PD_GETMASK1													\
	{																\
	__asm mov esi,DWORD PTR lpMsk									\
	__asm lodsb														\
	__asm mov DWORD PTR lpMsk,esi										\
	__asm or	al,al												\
	__asm jne	PD_HAVEMASK											\
	__asm add DWORD PTR lpSrc,ebx										\
	__asm add DWORD PTR lpDst,ebx										\
	__asm jmp	PD_NEXT												\
__asm PD_HAVEMASK:													\
	__asm sub	ah,ah												\
	__asm cmp	ax,127												\
	__asm jbe	PD_NOINC											\
	__asm inc	ax													\
__asm PD_NOINC:													\
	}

#define PD_PROCESSRGB												\
	PD_GETRED														\
	PD_COMBINE_SOURCE												\
	PD_GETGREEN														\
	PD_COMBINE_SOURCE												\
	PD_GETBLUE														\
	PD_COMBINE_SOURCE

#define PD_PROCESSRGBN												\
	PD_GETSOURCE													\
	PD_COMBINE_SOURCE												\
	PD_GETSOURCE													\
	PD_COMBINE_SOURCE												\
	PD_GETSOURCE													\
	PD_COMBINE_SOURCE

#define PD_PROCESSCMYK												\
	PD_GETCYAN														\
	PD_COMBINE_SOURCE												\
	PD_GETMAGENTA													\
	PD_COMBINE_SOURCE												\
	PD_GETYELLOW													\
	PD_COMBINE_SOURCE												\
	PD_GETBLACK														\
	PD_COMBINE_SOURCE	

#define PD_PROCESSCMYKN												\
	PD_GETSOURCE													\
	PD_COMBINE_SOURCE												\
	PD_GETSOURCE													\
	PD_COMBINE_SOURCE												\
	PD_GETSOURCE													\
	PD_COMBINE_SOURCE												\
	PD_GETSOURCE													\
	PD_COMBINE_SOURCE

#define PD_COPYRGB													\
{																	\
	__asm mov al,rgb.red											\
	__asm stosb														\
	__asm mov al,rgb.green											\
	__asm stosb														\
	__asm mov al,rgb.blue											\
	__asm stosb														\
}

#define PD_COPYRGBN													\
{																	\
	__asm movsw														\
	__asm movsb														\
}

#define PD_COPYCMYK													\
{																	\
	__asm mov al,cmyk.c												\
	__asm stosb														\
	__asm mov al,cmyk.m												\
	__asm stosb														\
	__asm mov al,cmyk.y												\
	__asm stosb														\
	__asm mov al,cmyk.k												\
	__asm stosb														\
}

#define PD_COPYCMYKN												\
{																	\
	__asm movsw														\
	__asm movsw														\
}

#define PD_GETSOURCE												\
	{																\
	__asm lodsb														\
	}

#define PD_GETRED													\
	{																\
	__asm mov al,rgb.red											\
	}

#define PD_GETGREEN													\
	{																\
	__asm mov al,rgb.green											\
	}

#define PD_GETBLUE													\
	{																\
	__asm mov al,rgb.blue											\
	}

#define PD_GETCYAN													\
	{																\
	__asm mov al,cmyk.c												\
	}

#define PD_GETMAGENTA												\
	{																\
	__asm mov al,cmyk.m												\
	}

#define PD_GETYELLOW												\
	{																\
	__asm mov al,cmyk.y												\
	}

#define PD_GETBLACK													\
	{																\
	__asm mov al,cmyk.k												\
	}

#define PD_COMBINE_SOURCE											\
	/* AL = (s)source, CX = (m)ask [edi] = (d)estination */		\
	{																\
	__asm sub	ah,ah 		/* AX = convert s to WORD */			\
	__asm mul	cx			/* CX = s * sm */						\
	__asm mov	bx,ax		/* BX = (s * sm) */						\
	__asm mov	dx,256												\
	__asm sub	dx,cx		/* DX = 256-m */						\
	__asm mov	al,BYTE PTR [edi]	/* AL = d */					\
	__asm sub	ah,ah		/* AX = convert d to WORD */			\
	__asm mul	dx			/* DX = (d * (256-m)) */				\
	__asm add	bx,ax		/* BX = (s * m) + (d * (256-m))	*/ 		\
	__asm add	bx,128	/* add 128 for rounding */			\
	__asm mov	al,bh		/* AL = HIBYTE */						\
	__asm stosb				/* store d */							\
	}

#define PD_COMBINE_SOURCE1											\
	/* AL = (s)source, CX = (m)ask [edi] = (d)estination */		\
	{																\
	__asm cmp	cx,256		/* CX - full opacity */					\
	__asm je	PD_SET_SOURCE1										\
	__asm sub	ah,ah 		/* AX = convert s to WORD */			\
	__asm mul	cx			/* CX = s * sm */						\
	__asm mov	bx,ax		/* BX = (s * sm) */						\
	__asm mov	dx,256												\
	__asm sub	dx,cx		/* DX = 256-m */						\
	__asm mov	al,BYTE PTR [edi]	/* AL = d */					\
	__asm sub	ah,ah		/* AX = convert d to WORD */			\
	__asm mul	dx			/* DX = (d * (256-m)) */				\
	__asm add	bx,ax		/* BX = (s * m) + (d * (256-m))	*/ 		\
	__asm add	bx,128	/* add 128 for rounding */			\
	__asm mov	al,bh		/* AL = HIBYTE */						\
__asm PD_SET_SOURCE1:												\
	__asm stosb				/* store d */							\
	}

#define PD_COMBINE_SOURCE2											\
	/* AL = (s)source, CX = (m)ask [edi] = (d)estination */		\
	{																\
	__asm cmp	cx,256		/* CX - full opacity */					\
	__asm je	PD_SET_SOURCE2										\
	__asm sub	ah,ah 		/* AX = convert s to WORD */			\
	__asm mul	cx			/* CX = s * sm */						\
	__asm mov	bx,ax		/* BX = (s * sm) */						\
	__asm mov	dx,256												\
	__asm sub	dx,cx		/* DX = 256-m */						\
	__asm mov	al,BYTE PTR [edi]	/* AL = d */					\
	__asm sub	ah,ah		/* AX = convert d to WORD */			\
	__asm mul	dx			/* DX = (d * (256-m)) */				\
	__asm add	bx,ax		/* BX = (s * m) + (d * (256-m))	*/ 		\
	__asm add	bx,128	/* add 128 for rounding */			\
	__asm mov	al,bh		/* AL = HIBYTE */						\
__asm PD_SET_SOURCE2:												\
	__asm stosb				/* store d */							\
	}

#define PD_COMBINE_SOURCE3											\
	/* AL = (s)source, CX = (m)ask [edi] = (d)estination */		\
	{																\
	__asm cmp	cx,256		/* CX - full opacity */					\
	__asm je	PD_SET_SOURCE3										\
	__asm sub	ah,ah 		/* AX = convert s to WORD */			\
	__asm mul	cx			/* CX = s * sm */						\
	__asm mov	bx,ax		/* BX = (s * sm) */						\
	__asm mov	dx,256												\
	__asm sub	dx,cx		/* DX = 256-m */						\
	__asm mov	al,BYTE PTR [edi]	/* AL = d */					\
	__asm sub	ah,ah		/* AX = convert d to WORD */			\
	__asm mul	dx			/* DX = (d * (256-m)) */				\
	__asm add	bx,ax		/* BX = (s * m) + (d * (256-m))	*/ 		\
	__asm add	bx,128	/* add 128 for rounding */			\
	__asm mov	al,bh		/* AL = HIBYTE */						\
__asm PD_SET_SOURCE3:												\
	__asm stosb				/* store d */							\
	}

#define PD_DSTRGBLUM												\
	{	/* al = r, ah = g, bl = b */								\
	__asm mov ax,[edi]											\
	__asm mov bl,[edi+2]											\
	__asm cmp al,ah													\
	__asm jb PD_R_BELOW_G1											\
	__asm cmp ah,bl													\
	__asm ja PD_AVG1												\
	__asm cmp al,bl													\
	__asm jb PD_R_BELOW_B1											\
	__asm mov bl,ah													\
	__asm jmp PD_AVG1												\
__asm PD_R_BELOW_B1:												\
	__asm mov al,ah													\
	__asm jmp PD_AVG1												\
__asm PD_R_BELOW_G1:												\
	__asm cmp al,bl													\
	__asm ja PD_R_ABOVE_B1											\
	__asm cmp bl,ah													\
	__asm ja PD_AVG1												\
	__asm mov bl,ah													\
	__asm jmp PD_AVG1												\
__asm PD_R_ABOVE_B1:												\
	__asm mov al,ah													\
__asm PD_AVG1:														\
	__asm sub ah,ah													\
	__asm sub bh,bh													\
	__asm add ax,bx													\
	__asm shr ax,1													\
	}

#define PD_SRCRGBLUM												\
	{	/* al = r, ah = g, bl = b */								\
	__asm mov ax,[esi]											\
	__asm mov bl,[esi+2]											\
	__asm cmp al,ah													\
	__asm jb PD_R_BELOW_G2											\
	__asm cmp ah,bl													\
	__asm ja PD_AVG2												\
	__asm cmp al,bl													\
	__asm jb PD_R_BELOW_B2											\
	__asm mov bl,ah													\
	__asm jmp PD_AVG2												\
__asm PD_R_BELOW_B2:												\
	__asm mov al,ah													\
	__asm jmp PD_AVG2												\
__asm PD_R_BELOW_G2:												\
	__asm cmp al,bl													\
	__asm ja PD_R_ABOVE_B2											\
	__asm cmp bl,ah													\
	__asm ja PD_AVG2												\
	__asm mov bl,ah													\
	__asm jmp PD_AVG2												\
__asm PD_R_ABOVE_B2:												\
	__asm mov al,ah													\
__asm PD_AVG2:														\
	__asm sub ah,ah													\
	__asm sub bh,bh													\
	__asm add ax,bx													\
	__asm shr ax,1													\
	}

#define PD_PUSHDSTPTR												\
	{																\
	__asm push DWORD PTR lpDst										\
	}

#define PD_PUSHSRCPTR												\
	{																\
	__asm push DWORD PTR lpSrc										\
	}

#define PD_PUSHSRCRGB												\
	{																\
	__asm mov esi,DWORD PTR lpSrc									\
	__asm mov al,[esi+2]											\
	__asm push ax													\
	__asm mov al,[esi+1]											\
	__asm push ax													\
	__asm mov al,[esi]											\
	__asm push ax													\
	__asm add esi,3													\
	__asm mov DWORD PTR lpSrc,esi										\
	}

#define PD_PUSHSRCCMY												\
	{																\
	__asm mov esi,DWORD PTR lpSrc									\
	__asm mov al,[esi+2]											\
	__asm push ax													\
	__asm mov al,[esi+1]											\
	__asm push ax													\
	__asm mov al,[esi]											\
	__asm push ax													\
	__asm add esi,4													\
	__asm mov DWORD PTR lpSrc,esi										\
	}

#define PD_PUSHDSTRGB												\
	{																\
	__asm mov edi,DWORD PTR lpDst									\
	__asm mov al,[edi+2]											\
	__asm push ax													\
	__asm mov al,[edi+1]											\
	__asm push ax													\
	__asm mov al,[edi]											\
	__asm push ax													\
	}

#define PD_PUSHHSLPTR												\
	{																\
	__asm lea eax, DWORD PTR [hsl]									\
	__asm push eax													\
	}

#define PD_PUSHRGBPTR												\
	{																\
	__asm lea eax, DWORD PTR [rgb]									\
	__asm push eax													\
	}

#define PD_PUSHCMYKPTR												\
	{																\
	__asm lea eax, DWORD PTR [cmyk]									\
	__asm push eax													\
	}

#define PD_PUSHHSL													\
	{																\
	__asm mov al,hsl.lum											\
	__asm push ax													\
	__asm mov al,hsl.sat											\
	__asm push ax													\
	__asm mov al,hsl.hue											\
	__asm push ax													\
	}

#define PD_POP5														\
	{																\
	__asm add sp,10													\
	}

#define PD_POP4														\
	{																\
	__asm add sp,8													\
	}

#define PD_POP3														\
	{																\
	__asm add sp,6													\
	}

#define PD_POP2														\
	{																\
	__asm add sp,4													\
	}

/************************************************************************/
static void ProcessInvert1(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
	while (--iCount >= 0)
	{
		if (*lpMsk)
		{
			*lpDst ^= *lpSrc;
		}
		lpDst++;
		lpSrc++;
		lpMsk++;
	}
}

/************************************************************************/
static void ProcessNormal8P(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (*lpMsk)
	{
		*lpDst = *lpSrc;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	PD_GETMASK8P
	PD_SETUP
	movsb
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessNormal8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		if (sm > 127)
			++sm;
		dm = 256-sm;
		if (dm)
		{
			s = (WORD)*lpSrc * sm;
			d = (WORD)*lpDst * dm;
			*lpDst = (d + s + 128) >> 8;
		}
		else
			*lpDst = *lpSrc;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,1
	PD_GETMASK
	PD_SETUP
	cmp	cx,256
	je	PD_COPYSOURCE
; Process the gray channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE
	PD_SETSRCPTR
	jmp PD_NEXT
PD_COPYSOURCE:
	movsb
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessAdditive8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		d = *lpDst;
		s = (WORD)*lpSrc + d;
		if (s > 255)
			s = 255;
		if (dm)
		{
			s = s * sm;
			d *= dm;
			*lpDst = (d + s + 128) >> 8;
		}
		else
			*lpDst = s;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,1
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	PD_SETSRCPTR
	add al,[edi]
	jnc GRAY_OK
	mov	al,255
GRAY_OK:

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_COMBINE_SOURCE
	jmp PD_NEXT
PD_COPYSOURCE:
	stosb
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessSubtractive8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;
int p;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		d = *lpDst;
		p = (int)*lpSrc + (int)d - 255;
		if (p < 0)
			p = 0;
		if (dm)
		{
			s = (WORD)p * sm;
			d *= dm;
			*lpDst = (d + s + 128) >> 8;
		}
		else
			*lpDst = p;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,1
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	PD_SETSRCPTR
	sub ah,ah
	mov bl,[edi]
	sub bh,bh
	add ax,bx
	sub ax,255
	jge GRAY_OK
	mov	al,0
GRAY_OK:

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_COMBINE_SOURCE
	jmp PD_NEXT
PD_COPYSOURCE:
	stosb
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessIfLighter8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		s = *lpSrc;
		d = *lpDst;
		if (s >= d) // It's lighter
		{
			if (dm)
			{
				s *= sm;
				d *= dm;
				*lpDst = (d + s + 128) >> 8;
			}
			else
				*lpDst = s;
		}
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,1
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	PD_SETSRCPTR
	cmp al,[edi]
	jae PD_LIGHTER
	inc	di
	jmp PD_NEXT
PD_LIGHTER:
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_COMBINE_SOURCE
	jmp PD_NEXT
PD_COPYSOURCE:
	stosb
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessIfDarker8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		s = *lpSrc;
		d = *lpDst;
		if (s < d) // It's lighter
			{
			if (dm)
				{
				s *= sm;
				d *= dm;
				*lpDst = (d + s + 128) >> 8;
				}
			else
				*lpDst = s;
			}
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,1
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	PD_SETSRCPTR
	cmp al,[edi]
	jb PD_DARKER
	inc	di
	jmp PD_NEXT
PD_DARKER:
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_COMBINE_SOURCE
	jmp PD_NEXT
PD_COPYSOURCE:
	stosb
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessMultiply8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		d = *lpDst;
		s = ((WORD)*lpSrc * d)/255;
		if (dm)
			{
			s *= sm;
			d *= dm;
			*lpDst = (d + s + 128) >> 8;
			}
		else
			*lpDst = s;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,1
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	PD_SETSRCPTR

	sub ah,ah
	cmp ax,127
	jbe	PD_MULNOINC
	inc ax
PD_MULNOINC:
	mov bl,[edi]
	sub bh,bh
	mul bx
	add ax,128
	mov al,ah

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_COMBINE_SOURCE
	jmp PD_NEXT
PD_COPYSOURCE:
	stosb
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessScreen8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
	
		s = *lpSrc;
		d = *lpDst;
		s = d + ((s * s) / 255);
		if (s > 255)
			s = 255;
		if (dm)
			{
			s *= sm;
			d *= dm;
			*lpDst = (d + s + 128) >> 8;
			}
		else
			*lpDst = s;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,1
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	PD_SETSRCPTR
	sub ah,ah
	mov bx,ax
	inc ax
	mul bx
	add ah,[edi]
	jnc GRAY_OK
	mov	ah,255
GRAY_OK:
	mov al,ah

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_COMBINE_SOURCE
	jmp PD_NEXT
PD_COPYSOURCE:
	stosb
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessDifference8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;
int p;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		d = *lpDst;
		p = (int)d - (int)*lpSrc;
		if (p < 0)
			p = -p;
		if (dm)
			{
			s = (WORD)p * sm;
			d = d * dm;
			*lpDst = (d + s + 128) >> 8;
			}
		else
			*lpDst = p;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,1
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	PD_SETSRCPTR
	sub ah,ah
	mov bl,[edi]
	sub bh,bh
	sub bx,ax
	jge GRAY_OK
	neg	bl
GRAY_OK:
	mov al,bl

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_COMBINE_SOURCE
	jmp PD_NEXT
PD_COPYSOURCE:
	stosb
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessInvert8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
	WORD sm, dm;
	WORD s, d;

	while (--iCount >= 0)
	{
		if (sm = *lpMsk)
		{
			PD_CSETUPMASK
			d = *lpDst;
			s = (*lpSrc ^ (BYTE)d);
			if (dm)
			{
				s *= sm;
				d *= dm;
				*lpDst = (d + s + 128) >> 8;
			}
			else
				*lpDst = s;
		}
		lpDst++;
		lpSrc++;
		lpMsk++;
	}
}

/************************************************************************/
static void ProcessNormal24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
//#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		if (dm)
			{
			PD_CPROCESSRGBN
			}
		else
			*lpDst = *lpSrc;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#ifdef UNUSED
//#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGBN
	PD_SETSRCPTR
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGBN
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessAdditive24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		s = (WORD)lpSrc->red + (WORD)lpDst->red;
		if (s > 255)
			s = 255;
		rgb.red = s;
		s = (WORD)lpSrc->green + (WORD)lpDst->green;
		if (s > 255)
			s = 255;
		rgb.green = s;
		s = (WORD)lpSrc->blue + (WORD)lpDst->blue;
		if (s > 255)
			s = 255;
		rgb.blue = s;
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	add al,[edi]
	jnc RED_OK
	mov	al,255
RED_OK:
	mov rgb.red,al
	PD_GETSOURCE
	add al,[edi+1]
	jnc GREEN_OK
	mov al,255
GREEN_OK:
	mov rgb.green,al
	PD_GETSOURCE
	add al,[edi+2]
	jnc BLUE_OK
	mov al,255
BLUE_OK:
	mov rgb.blue,al
	PD_SETSRCPTR

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGB
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGB
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessSubtractive24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;
int p;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		p = (int)lpSrc->red + (int)lpDst->red - 255;
		if (p < 0)
			p = 0;
		rgb.red = p;
		p = (int)lpSrc->green + (int)lpDst->green - 255;
		if (p < 0)
			p = 0;
		rgb.green = p;
		p = (int)lpSrc->blue + (int)lpDst->blue - 255;
		if (p < 0)
			p = 0;
		rgb.blue = p;
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi]
	sub bh,bh
	add ax,bx
	sub ax,255
	jge RED_OK
	mov	al,0
RED_OK:
	mov rgb.red,al
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi+1]
	sub bh,bh
	add ax,bx
	sub ax,255
	jge GREEN_OK
	mov al,0
GREEN_OK:
	mov rgb.green,al
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi+2]
	sub bh,bh
	add ax,bx
	sub ax,255
	jge BLUE_OK
	mov al,0
BLUE_OK:
	mov rgb.blue,al
	PD_SETSRCPTR

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGB
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGB
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessRed24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		if (dm)
			{
			s = (WORD)lpSrc->red * sm;
			d = (WORD)lpDst->red * dm;
			lpDst->red = (d + s + 128) >> 8;
			}
		else
			lpDst->red = lpSrc->red;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
; Process the red channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
	add si,2
	add di,2
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	jns	PD_LOOP

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessGreen24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		if (dm)
			{
			s = (WORD)lpSrc->green * sm;
			d = (WORD)lpDst->green * dm;
			lpDst->green = (d + s + 128) >> 8;
			}
		else
			lpDst->green = lpSrc->green;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
	inc	si
	inc	di
; Process the green channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
	inc	si
	inc	di
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	jns	PD_LOOP

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessBlue24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		if (dm)
			{
			s = (WORD)lpSrc->blue * sm;
			d = (WORD)lpDst->blue * dm;
			lpDst->blue = (d + s + 128) >> 8;
			}
		else
			lpDst->blue = lpSrc->blue;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
	add si,2
	add di,2
; Process the blue channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	jns	PD_LOOP

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessCyan24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		if (dm)
			{
			s = (WORD)lpSrc->green * sm;
			d = (WORD)lpDst->green * dm;
			lpDst->green = (d + s + 128) >> 8;
			s = (WORD)lpSrc->blue * sm;
			d = (WORD)lpDst->blue * dm;
			lpDst->blue = (d + s + 128) >> 8;
			}
		else
			{
			lpDst->green = lpSrc->green;
			lpDst->blue = lpSrc->blue;
			}
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
; Process the red channel
	inc si
	inc di
; Process the green channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
; Process the blue channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE2
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	jns	PD_LOOP

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessMagenta24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		if (dm)
			{
			s = (WORD)lpSrc->red * sm;
			d = (WORD)lpDst->red * dm;
			lpDst->red = (d + s + 128) >> 8;
			s = (WORD)lpSrc->blue * sm;
			d = (WORD)lpDst->blue * dm;
			lpDst->blue = (d + s + 128) >> 8;
			}
		else
			{
			lpDst->red = lpSrc->red;
			lpDst->blue = lpSrc->blue;
			}
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
; Process the red channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
; Process the green channel
	inc si
	inc di
; Process the blue channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE2
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	jns	PD_LOOP

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessYellow24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		if (dm)
			{
			s = (WORD)lpSrc->red * sm;
			d = (WORD)lpDst->red * dm;
			lpDst->red = (d + s + 128) >> 8;
			s = (WORD)lpSrc->green * sm;
			d = (WORD)lpDst->green * dm;
			lpDst->green = (d + s + 128) >> 8;
			}
		else
			{
			lpDst->red = lpSrc->red;
			lpDst->green = lpSrc->green;
			}
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
; Process the red channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
; Process the green channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE2
; Process the blue channel
	inc si
	inc di
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	jns	PD_LOOP

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessHueSat24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		hsl.lum = TOLUM(lpDst->red, lpDst->green, lpDst->blue);
		RGBtoHS(lpSrc->red, lpSrc->green, lpSrc->blue, &hsl);
		HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, &rgb);
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK1
	push ax					; save mask value

	PD_PUSHHSLPTR
	PD_PUSHSRCRGB
	call RGBtoHSL
	PD_POP5

	les di,lpDst
	PD_DSTRGBLUM

	mov hsl.lum,al
	PD_PUSHRGBPTR
	PD_PUSHHSL
	call HSLtoRGB
	PD_POP5

	les di,lpDst
	pop cx
	cmp	cx,256
	je	PD_COPYSOURCE
; Process the red channel
	PD_PROCESSRGB
	jmp PD_SETUPDST
PD_COPYSOURCE:
	PD_COPYRGB
PD_SETUPDST:
	PD_SETDSTPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop	ds
	}
#endif
}

/************************************************************************/
static void ProcessHue24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		RGBtoSL(lpDst->red, lpDst->green, lpDst->blue, &hsl);
		RGBtoH(lpSrc->red, lpSrc->green, lpSrc->blue, &hsl);
		HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, &rgb);
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK1
	push ax					; save mask value

	PD_PUSHHSLPTR
	PD_PUSHSRCRGB
	call RGBtoH
	PD_POP3

	PD_PUSHDSTRGB
	call RGBtoSL
	PD_POP5

	PD_PUSHRGBPTR
	PD_PUSHHSL
	call HSLtoRGB
	PD_POP5

	les di,lpDst
	pop cx
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGB
	jmp PD_SETUPDST
PD_COPYSOURCE:
	PD_COPYRGB
PD_SETUPDST:
	PD_SETDSTPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop	ds
	}
#endif
}

/************************************************************************/
static void ProcessSat24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		RGBtoHL(lpDst->red, lpDst->green, lpDst->blue, &hsl);
		RGBtoS(lpSrc->red, lpSrc->green, lpSrc->blue, &hsl);
		HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, &rgb);
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK1
	push ax					; save mask value

	PD_PUSHHSLPTR
	PD_PUSHSRCRGB
	call RGBtoS
	PD_POP3

	PD_PUSHDSTRGB
	call RGBtoHL
	PD_POP5

	PD_PUSHRGBPTR
	PD_PUSHHSL
	call HSLtoRGB
	PD_POP5

	les di,lpDst
	pop cx
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGB
	jmp PD_SETUPDST
PD_COPYSOURCE:
	PD_COPYRGB
PD_SETUPDST:
	PD_SETDSTPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop	ds
	}
#endif
}

/************************************************************************/
static void ProcessLum24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		RGBtoHS(lpDst->red, lpDst->green, lpDst->blue, &hsl);
		hsl.lum = TOLUM(lpSrc->red, lpSrc->green, lpSrc->blue);
		HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, &rgb);
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3				; 3 bytes per pixel
	PD_GETMASK1				; get mask value
	push ax					; save mask value

	lds	si,lpSrc			; save source buffer pointer
	PD_SRCRGBLUM			; calculate luminance
	push ax					; save luminance
	add	si,3				; bump source pointer
	PD_SETSRCPTR			; save source pointer

	PD_PUSHHSLPTR		 	; push pointer to hsl on stack
	PD_PUSHDSTRGB			; push lpDst r,g, and b on stack
	call RGBtoHS			; convert to h and s
	PD_POP5					; for params

	pop	ax		  			; restore luminance
	mov	hsl.lum,al			; set luminance

	PD_PUSHRGBPTR	 		; push pointer to rgb on stack
	PD_PUSHHSL				; push h, s, l on stack
	call HSLtoRGB			; convert hsl to rgb
	PD_POP5					; for params

	les di,lpDst	 		; get destinations pointer
	pop cx					; get mask
	cmp	cx,256				; max value mask?
	je	PD_COPYSOURCE		; yes, copy rgb directly
	PD_PROCESSRGB			; use mask to combine source and dest
	jmp PD_SETUPDST	
PD_COPYSOURCE:
	PD_COPYRGB				; copy from rgb to lpDst
PD_SETUPDST:
	PD_SETDSTPTR			; bump lpDst pointer
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop	ds
	}
#endif
}

#ifdef UNUSED
/************************************************************************/
static void ProcessLum24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		RGBtoHS(lpDst->red, lpDst->green, lpDst->blue, &hsl);
		hsl.lum = TOLUM(lpSrc->red, lpSrc->green, lpSrc->blue);
		HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, &rgb);
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
; Process the red channel
	PD_SRCRGBLUM
	add si,3
	PD_SETSRCPTR
	mov	dl,al
	PD_DSTRGBLUM
	push cx

	or	al,al
	je	PD_NODSTLUM
	cmp dl,255
	je  PD_NODSTLUM
	sub	dh,dh
	mov	si,dx
	mov	bl,al
	sub	bh,bh
	mov	cl,al
	shr cl,1
	sub ch,ch
	mov	al,[edi]
	mul si
	add	ax,cx
	sub	dx,dx
	div bx
	cmp ax,255
	jbe RED_OK
	mov al,255
RED_OK:
	mov rgb.red,al
	mov	al,[edi+1]
	mul si
	add	ax,cx
	sub	dx,dx
	div bx
	cmp ax,255
	jbe GREEN_OK
	mov al,255
GREEN_OK:
	mov rgb.green,al
	mov	al,[edi+2]
	mul si
	add	ax,cx
	sub dx,dx
	div bx
	cmp ax,255
	jbe BLUE_OK
	mov al,255
BLUE_OK:
	mov rgb.blue,al
	jmp PD_GOTRGB
PD_NODSTLUM:
	mov	rgb.red,dl
	mov rgb.green,dl
	mov rgb.blue,dl
PD_GOTRGB:

	pop cx
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGB
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGB
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}
#endif

/************************************************************************/
static void ProcessIfLighter24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;
BYTE slum, dlum;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		dlum = TOLUM(lpDst->red, lpDst->green, lpDst->blue);
		slum = TOLUM(lpSrc->red, lpSrc->green, lpSrc->blue);
		if (slum >= dlum) // It's lighter
			{
			if (dm)
				{
				PD_CPROCESSRGBN
				}
			else
				*lpDst = *lpSrc;
			}
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
; Process the red channel
	PD_SRCRGBLUM
	mov	dl,al
	PD_DSTRGBLUM
	cmp dl,al
	jae PD_LIGHTER
	add	di,3
	add	si,3
	PD_SETSRCPTR
	jmp PD_NEXT
PD_LIGHTER:
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGBN
	PD_SETSRCPTR
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGBN
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessIfDarker24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;
BYTE slum, dlum;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		dlum = TOLUM(lpDst->red, lpDst->green, lpDst->blue);
		slum = TOLUM(lpSrc->red, lpSrc->green, lpSrc->blue);
		if (slum < dlum) // It's darker
			{
			if (dm)
				{
				PD_CPROCESSRGBN
				}
			else
				*lpDst = *lpSrc;
			}
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
; Process the red channel
	PD_SRCRGBLUM
	mov	dl,al
	PD_DSTRGBLUM
	cmp dl,al
	jb PD_DARKER
	add	di,3
	add	si,3
	PD_SETSRCPTR
	jmp PD_NEXT
PD_DARKER:
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGBN
	PD_SETSRCPTR
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGBN
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessMultiply24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		rgb.red = ((WORD)lpSrc->red * (WORD)lpDst->red)/255;
		rgb.green = ((WORD)lpSrc->green * (WORD)lpDst->green)/255;
		rgb.blue = ((WORD)lpSrc->blue * (WORD)lpDst->blue)/255;
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	sub ah,ah
	cmp ax,127
	jbe PD_1MULNOINC
	inc ax
PD_1MULNOINC:
	mov bl,[edi]
	sub bh,bh
	mul bx
	add ax,128
	mov rgb.red,ah
	PD_GETSOURCE
	sub ah,ah
	cmp ax,127
	jbe PD_2MULNOINC
	inc ax
PD_2MULNOINC:
	mov bl,[edi+1]
	sub bh,bh
	mul bx
	add ax,128
	mov rgb.green,ah
	PD_GETSOURCE
	sub ah,ah
	cmp ax,127
	jbe PD_3MULNOINC
	inc ax
PD_3MULNOINC:
	mov bl,[edi+2]
	sub bh,bh
	mul bx
	add ax,128
	mov rgb.blue,ah
	PD_SETSRCPTR

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGB
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGB
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessScreen24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
	
		s = lpSrc->red;
		s = (WORD)lpDst->red + ((s * s) / 255);
		if (s > 255)
			s = 255;
		rgb.red = s;
		s = lpSrc->green;
		s = (WORD)lpDst->green + ((s * s) / 255);
		if (s > 255)
			s = 255;
		rgb.green = s;
		s = lpSrc->blue;
		s = (WORD)lpDst->blue + ((s * s) / 255);
		if (s > 255)
			s = 255;
		rgb.blue = s;
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	sub ah,ah
	mov bx,ax
	inc ax
	mul bx
	add ah,[edi]
	jnc RED_OK
	mov	ah,255
RED_OK:
	mov rgb.red,ah
	PD_GETSOURCE
	sub ah,ah
	mov bx,ax
	inc ax
	mul bx
	add ah,[edi+1]
	jnc GREEN_OK
	mov ah,255
GREEN_OK:
	mov rgb.green,ah
	PD_GETSOURCE
	sub ah,ah
	mov bx,ax
	inc ax
	mul bx
	add ah,[edi+2]
	jnc BLUE_OK
	mov ah,255
BLUE_OK:
	mov rgb.blue,ah
	PD_SETSRCPTR

	cmp	cx,256
	je	PD_COPYSOURCE
; Process the red channel
	PD_PROCESSRGB
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGB
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessDifference24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;
int p;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		p = (int)lpDst->red - (int)lpSrc->red;
		if (p < 0)
			p = -p;
		rgb.red = p;
		p = (int)lpDst->green - (int)lpSrc->green;
		if (p < 0)
			p = -p;
		rgb.green = p;
		p = (int)lpDst->blue - (int)lpSrc->blue;
		if (p < 0)
			p = -p;
		rgb.blue = p;
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi]
	sub bh,bh
	sub bx,ax
	jge RED_OK
	neg	bl
RED_OK:
	mov rgb.red,bl
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi+1]
	sub bh,bh
	sub bx,ax
	jge GREEN_OK
	neg	bl
GREEN_OK:
	mov rgb.green,bl
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi+2]
	sub bh,bh
	sub bx,ax
	jge BLUE_OK
	neg bl
BLUE_OK:
	mov rgb.blue,bl
	PD_SETSRCPTR

	cmp	cx,256
	je	PD_COPYSOURCE
; Process the red channel
	PD_PROCESSRGB
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGB
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessTexturize24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;
WORD slum;

while (--iCount >= 0)
	{
	if (sm = *lpMsk)
		{
		PD_CSETUPMASK
		slum = TOLUM(lpSrc->red, lpSrc->green, lpSrc->blue);
		rgb.red = (slum * (WORD)lpDst->red)/255;
		rgb.green = (slum * (WORD)lpDst->green)/255;
		rgb.blue = (slum * (WORD)lpDst->blue)/255;
		if (dm)
			{
			PD_CPROCESSRGB
			}
		else
			*lpDst = rgb;
		}
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,3
	PD_GETMASK
	PD_SETUP
	PD_SRCRGBLUM
	add	si,3
	PD_SETSRCPTR
	mov	bl,al
	sub	bh,bh
	inc bx
	mov al,[edi]
	sub ah,ah
	mul bx
	mov rgb.red,ah
	mov al,[edi+1]
	sub ah,ah
	mul bx
	mov rgb.green,ah
	mov al,[edi+2]
	sub ah,ah
	mul bx
	mov rgb.blue,ah

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSRGB
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYRGB
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessInvert24(LPRGB lpDst, LPRGB lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
	WORD sm, dm;
	WORD s, d;
	RGBS rgb;

	while (--iCount >= 0)
	{
		if (sm = *lpMsk)
		{
			PD_CSETUPMASK
			rgb.red = (lpSrc->red ^ lpDst->red);
			rgb.green = (lpSrc->green ^ lpDst->green);
			rgb.blue = (lpSrc->blue ^ lpDst->blue);
			if (dm)
			{
				PD_CPROCESSRGB
			}
			else
				*lpDst = rgb;
		}
		lpDst++;
		lpSrc++;
		lpMsk++;
	}
}

/************************************************************************/
static void ProcessNormal32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		if (dm)
		{
			PD_CPROCESSCMYKN
		}
		else
			*lpDst = *lpSrc;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSCMYKN
	PD_SETSRCPTR
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYCMYKN
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessAdditive32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
	STACK_FRAME
	WORD sm, dm;
	WORD s, d;
	int p;

	while (--iCount >= 0)
	{
		if (sm = *lpMsk)
		{
			PD_CSETUPMASK
			p = ((int)lpSrc->c^0xFF) + ((int)lpDst->c^0xFF);
			if (p > 255)
				p = 255;
			cmyk.c = p^0xFF;
			p = ((int)lpSrc->m^0xFF) + ((int)lpDst->m^0xFF);
			if (p > 255)
				p = 255;
			cmyk.m = p^0xFF;
			p = ((int)lpSrc->y^0xFF) + ((int)lpDst->y^0xFF);
			if (p > 255)
				p = 255;
			cmyk.y = p^0xFF;
			p = ((int)lpSrc->k^0xFF) + ((int)lpDst->k^0xFF);
			if (p > 255)
				p = 255;
			cmyk.k = p^0xFF;
			if (dm)
			{
				PD_CPROCESSCMYK
			}
			else
				*lpDst = cmyk;
		}
		lpDst++;
		lpSrc++;
		lpMsk++;
	}
}

/************************************************************************/
static void ProcessSubtractive32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
	STACK_FRAME
	WORD sm, dm;
	WORD s, d;
	int p;

	while (--iCount >= 0)
	{
		if (sm = *lpMsk)
		{
			PD_CSETUPMASK

			p = ((WORD)lpSrc->c^0xFF) + ((WORD)lpDst->c^0xFF) - 255;
			if (p < 0)
				p = 0;
			cmyk.c = p^0xFF;

			p = ((WORD)lpSrc->m^0xFF) + ((WORD)lpDst->m^0xFF) - 255;
			if (p < 0)
				p = 0;
			cmyk.m = p^0xFF;

			p = ((WORD)lpSrc->y^0xFF) + ((WORD)lpDst->y^0xFF) - 255;
			if (p < 0)
				p = 0;
			cmyk.y = p^0xFF;

			p = ((WORD)lpSrc->k^0xFF) + ((WORD)lpDst->k^0xFF) - 255;
			if (p < 0)
				p = 0;
			cmyk.k = p^0xFF;

			if (dm)
			{
				PD_CPROCESSCMYK
			}
			else
				*lpDst = cmyk;
		}
		lpDst++;
		lpSrc++;
		lpMsk++;
	}
}

/************************************************************************/
static void ProcessRed32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		if (dm)
		{
			s = (WORD)lpSrc->m * sm;
			d = (WORD)lpDst->m * dm;
			lpDst->m = (d + s + 128) >> 8;
			s = (WORD)lpSrc->y * sm;
			d = (WORD)lpDst->y * dm;
			lpDst->y = (d + s + 128) >> 8;
			s = (WORD)lpSrc->k * sm;
			d = (WORD)lpDst->k * dm;
			lpDst->k = (d + s + 128) >> 8;
		}
		else
		{
			lpDst->m = lpSrc->m;
			lpDst->y = lpSrc->y;
			lpDst->k = lpSrc->k;
		}
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
; process the c channel
	inc	si
	inc di
; Process the m channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
; Process the y channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE2
; Process the k channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE3
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessGreen32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		if (dm)
		{
			s = (WORD)lpSrc->c * sm;
			d = (WORD)lpDst->c * dm;
			lpDst->c = (d + s + 128) >> 8;
			s = (WORD)lpSrc->y * sm;
			d = (WORD)lpDst->y * dm;
			lpDst->y = (d + s + 128) >> 8;
			s = (WORD)lpSrc->k * sm;
			d = (WORD)lpDst->k * dm;
			lpDst->k = (d + s + 128) >> 8;
		}
		else
		{
			lpDst->c = lpSrc->c;
			lpDst->y = lpSrc->y;
			lpDst->k = lpSrc->k;
		}
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
; Process the cyan channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
; Process the magenta channel
	inc	si
	inc	di
; Process the yellow channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE2
; Process the black channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE3
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessBlue32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		if (dm)
		{
			s = (WORD)lpSrc->c * sm;
			d = (WORD)lpDst->c * dm;
			lpDst->c = (d + s + 128) >> 8;
			s = (WORD)lpSrc->m * sm;
			d = (WORD)lpDst->m * dm;
			lpDst->m = (d + s + 128) >> 8;
			s = (WORD)lpSrc->k * sm;
			d = (WORD)lpDst->k * dm;
			lpDst->k = (d + s + 128) >> 8;
		}
		else
		{
			lpDst->c = lpSrc->c;
			lpDst->m = lpSrc->m;
			lpDst->k = lpSrc->k;
		}
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
; Process the cyan channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
; Process the magenta channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE2
; Process the yellow channel
	inc si
	inc di
; Process the black channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE3
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessCyan32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		if (dm)
		{
			s = (WORD)lpSrc->c * sm;
			d = (WORD)lpDst->c * dm;
			lpDst->c = (d + s + 128) >> 8;
		}
		else
			lpDst->c = lpSrc->c;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
; Process the c channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
	add si,3
	add di,3
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessMagenta32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		if (dm)
		{
			s = (WORD)lpSrc->m * sm;
			d = (WORD)lpDst->m * dm;
			lpDst->m = (d + s + 128) >> 8;
		}
		else
			lpDst->m = lpSrc->m;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
; Process the c channel
	inc si
	inc di
; Process the magenta channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
	add si,2
	add di,2
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessYellow32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		if (dm)
		{
			s = (WORD)lpSrc->y * sm;
			d = (WORD)lpDst->y * dm;
			lpDst->y = (d + s + 128) >> 8;
		}
		else
			lpDst->y = lpSrc->y;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
; Process the c channel
; Process the magenta channel
	add si,2
	add di,2
; Process the yellow channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
; Process the black channel
	inc si
	inc di
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessBlack32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifndef _X86_
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		if (dm)
		{
			s = (WORD)lpSrc->k * sm;
			d = (WORD)lpDst->k * dm;
			lpDst->k = (d + s + 128) >> 8;
		}
		else
			lpDst->k = lpSrc->k;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
; Process the c channel
; Process the magenta channel
; Process the yellow channel
	add si,3
	add di,3
; Process the black channel
	PD_GETSOURCE
	PD_COMBINE_SOURCE1
	PD_SETSRCPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:

	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessHueSat32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifdef CMYK_C_CODE
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		hsl.lum = CMYKtoL( lpDst );
		CMYKtoHS(lpSrc, &hsl);
		HSLtoCMYK(hsl.hue, hsl.sat, hsl.lum, &cmyk);
		if (dm)
		{
			PD_CPROCESSCMYK
		}
		else
			*lpDst = cmyk;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK1
	push ax					; save mask value

	PD_PUSHDSTPTR
	call CMYKtoL
	PD_POP2
	mov hsl.lum,al;

	PD_PUSHHSLPTR
	PD_PUSHSRCPTR
	call CMYKtoHS
	PD_POP4

	PD_PUSHCMYKPTR
	PD_PUSHHSL
	call HSLtoCMYK
	PD_POP5

	add WORD PTR lpSrc,4
	les di,lpDst
	pop cx
	cmp	cx,256
	je	PD_COPYSOURCE
; Process the c channel
	PD_PROCESSCMYK
	jmp PD_SETUPDST
PD_COPYSOURCE:
	PD_COPYCMYK
PD_SETUPDST:
	PD_SETDSTPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop ds
	}
#endif
}

/************************************************************************/
static void ProcessHue32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifdef CMYK_C_CODE
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		CMYKtoSL( lpDst, &hsl );
		CMYKtoH( lpSrc, &hsl );
		HSLtoCMYK( hsl.hue, hsl.sat, hsl.lum, &cmyk );
		if (dm)
		{
			PD_CPROCESSCMYK
		}
		else
			*lpDst = cmyk;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK1
	push ax					; save mask value

	PD_PUSHHSLPTR
	PD_PUSHDSTPTR
	call CMYKtoSL
	PD_POP4
	
	PD_PUSHHSLPTR
	PD_PUSHSRCPTR
	call CMYKtoH
	PD_POP4
	
	PD_PUSHCMYKPTR
	PD_PUSHHSL
	call HSLtoCMYK
	PD_POP5

	add WORD PTR lpSrc,4
	les di,lpDst
	pop cx
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSCMYK
	jmp PD_SETUPDST
PD_COPYSOURCE:
	PD_COPYCMYK
PD_SETUPDST:
	PD_SETDSTPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop	ds
	}
#endif
}

/************************************************************************/
static void ProcessSat32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifdef CMYK_C_CODE
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		CMYKtoHL(lpDst, &hsl);
		CMYKtoS (lpSrc, &hsl);
		HSLtoCMYK(hsl.hue, hsl.sat, hsl.lum, &cmyk);
		if (dm)
		{
			PD_CPROCESSCMYK
		}
		else
			*lpDst = cmyk;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK1
	push ax					; save mask value

	PD_PUSHHSLPTR
	PD_PUSHDSTPTR
	call CMYKtoHL
	PD_POP4
	
	PD_PUSHHSLPTR
	PD_PUSHSRCPTR
	call CMYKtoS
	PD_POP4
	
	PD_PUSHCMYKPTR
	PD_PUSHHSL
	call HSLtoCMYK
	PD_POP5

	add WORD PTR lpSrc,4
	les di,lpDst
	pop cx
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSCMYK
	jmp PD_SETUPDST
PD_COPYSOURCE:
	PD_COPYCMYK
PD_SETUPDST:
	PD_SETDSTPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop	ds
	}
#endif
}

/************************************************************************/
static void ProcessLum32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifdef CMYK_C_CODE
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		CMYKtoHS(lpDst, &hsl);
		hsl.lum = CMYKtoL( lpSrc );
		HSLtoCMYK(hsl.hue, hsl.sat, hsl.lum, &cmyk);
		if (dm)
		{
			PD_CPROCESSCMYK
		}
		else
			*lpDst = cmyk;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK1
	push ax					; save mask value

	PD_PUSHHSLPTR
	PD_PUSHDSTPTR
	call CMYKtoHS
	PD_POP4
	
	PD_PUSHSRCPTR
	call CMYKtoL
	PD_POP2
	mov hsl.lum,al
	
	PD_PUSHCMYKPTR
	PD_PUSHHSL
	call HSLtoCMYK
	PD_POP5

	add WORD PTR lpSrc,4
	les di,lpDst
	pop cx
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSCMYK
	jmp PD_SETUPDST
PD_COPYSOURCE:
	PD_COPYCMYK
PD_SETUPDST:
	PD_SETDSTPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop	ds
	}
#endif
}

/************************************************************************/
static void ProcessIfLighter32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifdef CMYK_C_CODE
WORD sm, dm;
WORD s, d;
BYTE slum, dlum;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		dlum = CMYKtoL(lpDst);
		slum = CMYKtoL(lpSrc);
		if (slum >= dlum) // It's lighter
		{
			if (dm)
			{
				PD_CPROCESSCMYKN
			}
			else
				*lpDst = *lpSrc;
		}
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK1
	push ax			; save the mask value

	PD_PUSHSRCPTR
	call CMYKtoL
	PD_POP2
	push ax

	PD_PUSHDSTPTR
	call CMYKtoL
	PD_POP2

	pop dx
	pop cx
	cmp dl,al
	jae PD_LIGHTER
	add	WORD PTR lpDst,4
	add	WORD PTR lpSrc,4
	jmp PD_NEXT
PD_LIGHTER:
	lds si,lpSrc
	les di,lpDst
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSCMYKN
	jmp PD_SETUPPTRS
PD_COPYSOURCE:
	PD_COPYCMYKN
PD_SETUPPTRS:
	PD_SETSRCPTR
	PD_SETDSTPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop	ds
	}
#endif
}

/************************************************************************/
static void ProcessIfDarker32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
#ifdef CMYK_C_CODE
WORD sm, dm;
WORD s, d;
BYTE slum, dlum;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		dlum = CMYKtoL(lpDst);
		slum = CMYKtoL(lpSrc);
		if (slum < dlum) // It's darker
		{
			if (dm)
			{
				PD_CPROCESSCMYKN
			}
			else
				*lpDst = *lpSrc;
		}
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	push	ds
	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK1
	push ax			; save the mask value

	PD_PUSHSRCPTR
	call CMYKtoL
	PD_POP2
	push ax

	PD_PUSHDSTPTR
	call CMYKtoL
	PD_POP2

	pop dx
	pop cx
	cmp dl,al
	jb PD_DARKER
	add	WORD PTR lpDst,4
	add	WORD PTR lpSrc,4
	jmp PD_NEXT
PD_DARKER:
	lds si,lpSrc
	les di,lpDst
	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSCMYKN
	jmp PD_SETUPPTRS
PD_COPYSOURCE:
	PD_COPYCMYKN
PD_SETUPPTRS:
	PD_SETSRCPTR
	PD_SETDSTPTR
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	pop	ds
	}
#endif
}

/************************************************************************/
static void ProcessMultiply32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifdef CMYK_C_CODE
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		cmyk.c = (((WORD)(lpSrc->c^0xFF) * (WORD)(lpDst->c^0xFF))/255)^0xFF;
		cmyk.m = (((WORD)(lpSrc->m^0xFF) * (WORD)(lpDst->m^0xFF))/255)^0xFF;
		cmyk.y = (((WORD)(lpSrc->y^0xFF) * (WORD)(lpDst->y^0xFF))/255)^0xFF;
		cmyk.k = (((WORD)(lpSrc->k^0xFF) * (WORD)(lpDst->k^0xFF))/255)^0xFF;
		if (dm)
		{
			PD_CPROCESSCMYK
		}
		else
			*lpDst = cmyk;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	sub ah,ah
	cmp ax,127
	jbe PD_1MULNOINC
	inc ax
PD_1MULNOINC:
	mov bl,[edi]
	sub bh,bh
	mul bx
	add ax,128
	mov cmyk.c,ah
	PD_GETSOURCE
	sub ah,ah
	cmp ax,127
	jbe PD_2MULNOINC
	inc ax
PD_2MULNOINC:
	mov bl,[edi+1]
	sub bh,bh
	mul bx
	add ax,128
	mov cmyk.m,ah
	PD_GETSOURCE
	sub ah,ah
	cmp ax,127
	jbe PD_3MULNOINC
	inc ax
PD_3MULNOINC:
	mov bl,[edi+2]
	sub bh,bh
	mul bx
	add ax,128
	mov cmyk.y,ah
	PD_GETSOURCE
	sub ah,ah
	cmp ax,127
	jbe PD_4MULNOINC
	inc ax
PD_4MULNOINC:
	mov bl,[edi+3]
	sub bh,bh
	mul bx
	add ax,128
	mov cmyk.k,ah
	PD_SETSRCPTR

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSCMYK
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYCMYK
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessScreen32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
	STACK_FRAME
	WORD sm, dm;
	WORD s, d;

	while (--iCount >= 0)
	{
		if (sm = *lpMsk)
		{
			PD_CSETUPMASK
	
			s = lpSrc->c^0xFF;
			s = ((WORD)lpDst->c^0xFF) + ((s * s) / 255);
			if (s > 255)
				s = 255;
			cmyk.c = s^0xFF;

			s = lpSrc->m^0xFF;
			s = ((WORD)lpDst->m^0xFF) + ((s * s) / 255);
			if (s > 255)
				s = 255;
			cmyk.m = s^0xFF;

			s = lpSrc->y^0xFF;
			s = ((WORD)lpDst->y^0xFF) + ((s * s) / 255);
			if (s > 255)
				s = 255;
			cmyk.y = s^0xFF;

			s = lpSrc->k^0xFF;
			s = ((WORD)lpDst->k^0xFF) + ((s * s) / 255);
			if (s > 255)
				s = 255;
			cmyk.k = s^0xFF;

			if (dm)
			{
				PD_CPROCESSCMYK
			}
			else
				*lpDst = cmyk;
		}
		lpDst++;
		lpSrc++;
		lpMsk++;
	}
}

/************************************************************************/
static void ProcessDifference32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifndef _X86_
WORD sm, dm;
WORD s, d;
int p;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK

		p = (int)(lpDst->c^0xFF) - (int)(lpSrc->c^0xFF);
		if (p < 0)
			p = -p;
		cmyk.c = p^0xFF;

		p = (int)(lpDst->m^0xFF) - (int)(lpSrc->m^0xFF);
		if (p < 0)
			p = -p;
		cmyk.m = p^0xFF;

		p = (int)(lpDst->y^0xFF) - (int)(lpSrc->y^0xFF);
		if (p < 0)
			p = -p;
		cmyk.y = p^0xFF;

		p = (int)(lpDst->k^0xFF) - (int)(lpSrc->k^0xFF);
		if (p < 0)
			p = -p;
		cmyk.k = p^0xFF;

		if (dm)
		{
			PD_CPROCESSCMYK
		}
		else
			*lpDst = cmyk;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK
	PD_SETUP
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi]
	sub bh,bh
	sub bx,ax
	jge CYAN_OK
	neg	bl
CYAN_OK:
	mov cmyk.c,bl
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi+1]
	sub bh,bh
	sub bx,ax
	jge MAGENTA_OK
	neg bl
MAGENTA_OK:
	mov cmyk.m,bl
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi+2]
	sub bh,bh
	sub bx,ax
	jge YELLOW_OK
	neg bl
YELLOW_OK:
	mov cmyk.y,bl
	PD_GETSOURCE
	sub ah,ah
	mov bl,[edi+3]
	sub bh,bh
	sub bx,ax
	jge BLACK_OK
	neg bl
BLACK_OK:
	mov cmyk.k,bl
	PD_SETSRCPTR

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSCMYK
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYCMYK
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessTexturize32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
STACK_FRAME
#ifdef CMYK_C_CODE
WORD sm, dm;
WORD s, d;

while (--iCount >= 0)
{
	if (sm = *lpMsk)
	{
		PD_CSETUPMASK
		s = CMYKtoL(lpSrc);
		cmyk.c = ((s * (WORD)(lpDst->c^0xFF))/255)^0xFF;
		cmyk.m = ((s * (WORD)(lpDst->m^0xFF))/255)^0xFF;
		cmyk.y = ((s * (WORD)(lpDst->y^0xFF))/255)^0xFF;
		cmyk.k = ((s * (WORD)(lpDst->k^0xFF))/255)^0xFF;
		if (dm)
		{
			PD_CPROCESSCMYK
		}
		else
			*lpDst = cmyk;
	}
	lpDst++;
	lpSrc++;
	lpMsk++;
}
#else
__asm
	{
	PD_BEGIN

	jmp	PD_NEXT
	
PD_LOOP:
	mov ebx,4
	PD_GETMASK1
	push ax			; save the mask value

	PD_PUSHSRCPTR
	call CMYKtoL
	PD_POP2
	mov	bl,al		; get L into BX
	sub	bh,bh
	inc bx
	add	WORD PTR lpSrc,4

	pop	cx			; restore the mask value

	les di,lpDst	; get destination pointer

	mov al,[edi]
	sub ah,ah
	mul bx
	mov cmyk.c,ah

	mov al,[edi+1]
	sub ah,ah
	mul bx
	mov cmyk.m,ah

	mov al,[edi+2]
	sub ah,ah
	mul bx
	mov cmyk.y,ah

	mov al,[edi+3]
	sub ah,ah
	mul bx
	mov cmyk.k,ah

	cmp	cx,256
	je	PD_COPYSOURCE
	PD_PROCESSCMYK
	jmp PD_NEXT
PD_COPYSOURCE:
	PD_COPYCMYK
PD_NEXT:
	dec	iCount			;iCount
	js  PD_DONE
	jmp	PD_LOOP
PD_DONE:
	PD_END
	}
#endif
}

/************************************************************************/
static void ProcessInvert32(LPCMYK lpDst, LPCMYK lpSrc, LPTR lpMsk, int iCount)
/************************************************************************/
{
	WORD sm, dm;
	WORD s, d;
	CMYKS cmyk;

	while (--iCount >= 0)
	{
		if (sm = *lpMsk)
		{
			PD_CSETUPMASK
			cmyk.c = lpSrc->c ^ lpDst->c;
			cmyk.m = lpSrc->m ^ lpDst->m;
			cmyk.y = lpSrc->y ^ lpDst->y;
			cmyk.k = lpSrc->k ^ lpDst->k;
			if (dm)
			{
				PD_CPROCESSCMYK
			}
			else
				*lpDst = cmyk;
		}
		lpDst++;
		lpSrc++;
		lpMsk++;
	}
}


