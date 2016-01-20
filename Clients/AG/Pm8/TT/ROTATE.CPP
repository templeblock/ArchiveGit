/*
$Header: /PM8/TT/ROTATE.CPP 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/ROTATE.CPP $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:15:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:12   Fred
// Initial revision.
// 
//    Rev 1.3   01 Jul 1997 09:08:36   Fred
// More changes for PMG 4.0
// 
//    Rev 1.2   21 Apr 1997 08:47:56   Fred
// Fixes for rotated pens and shadows.
// 
//    Rev 1.1   18 Apr 1997 12:49:50   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:42:00   FRED
// Initial revision.
// 
//    Rev 1.1   04 Oct 1995 10:59:38   FRED
// 32 bit TrueType engine
// 
//    Rev 1.0   29 Sep 1995 13:37:02   FRED
// Initial revision.
 * 
 *    Rev 1.10   13 Jul 1993 10:40:56   MACDUFF
 * Fixed shadow rotation code, though it's currently disabled with
 * #ifdef ROTATE_EFFECTS
 * 
 *    Rev 1.9   02 Dec 1992 11:36:20   MACDUFF
 * Disabled get_shadow_offset temporarily because it is wrong.
 * 
 *    Rev 1.8   05 Nov 1992 12:59:34   MACDUFF
 * Fixed fsg_MatrixPoint.
 * Added rotate_pen.
 * 
 *    Rev 1.7   21 Oct 1992 14:12:28   MACDUFF
 * Support for new shadow rotation
 * 
 *    Rev 1.6   19 Oct 1992 17:57:32   MACDUFF
 * Added rotation functions for TRUEFX.
 * 
 *    Rev 1.5   01 Sep 1992 14:50:54   FRED
 * #ifdef'ed out prototype that was not used.
 * 
 * 
 *    Rev 1.4   31 Aug 1992 23:12:22   FRED
 * Removed code and prototypes that were not being used.
 * 
 * 
 *    Rev 1.3   19 Aug 1992 15:46:42   MACDUFF
 * Uses variable precision globals.
 * 
 *    Rev 1.2   18 Aug 1992 09:57:32   MACDUFF
 * Uses _PRECISION
 * 
 *    Rev 1.1   14 Jul 1992 09:06:08   ADAM
 * fsg_Dot6XYMul was not using transformation matrix.
*/

/** FontScaler's Includes **/
#include "fserror.h"
#include "fscdefs.h"
#include "fontmath.h"
#include "sfnt.h"
#include "fnt.h"
#include "sc.h"
#include "fsglue.h"
#include "privsfnt.h"
#include <math.h>

#include "effects.h"
extern FX Fx;

#define NUMBEROFPOINTS(elementPtr)  (elementPtr->ep[elementPtr->nc - 1] + 1 + PHANTOMCOUNT)

#define LOOPDOWN(n)     for (--n; n >= 0; --n)
#define ULOOPDOWN(n)        while (n--)

#if 0 // 9/1/92 (FF) does not seem to be used
void fsg_ReverseContours (fnt_ElementType*elementPtr);
#endif

void fsg_CopyElementBackwards (fnt_ElementType *elementPtr);

#if 0 // 8/31/92 (FF) - does not seem to be used
static int8 fsg_HasPerspective (transMatrix*matrix);
static void fsg_Dot6XYMul (F26Dot6*x, F26Dot6*y, transMatrix*matrix);
#endif

static void fsg_IntelMul (int numPts, F26Dot6*x, F26Dot6*y, transMatrix *trans, Fixed xStretch, Fixed yStretch);
/*
 *  fsg_PostTransformGlyph              <3>
 */
void fsg_PostTransformGlyph (register fsg_SplineKey *key, transMatrix *trans)
{
  fnt_ElementType * elementPtr = & (key->elementInfoRec.interpreterElements[key->elementNumber]);

  fsg_IntelMul (NUMBEROFPOINTS (elementPtr), elementPtr->x, elementPtr->y, trans, key->metricScalarX, key->metricScalarY);
}

static void fsg_IntelMul (int numPts, F26Dot6*x, F26Dot6*y, transMatrix *trans, Fixed xStretch, Fixed yStretch)
{
  if (xStretch == 0L || yStretch == 0L)
  {
    LOOPDOWN (numPts)
      *y++ = *x++ = 0;
  }
  else
  {
    Fixed m00 = FixDiv (trans->transform[0][0], xStretch);
    Fixed m01 = FixDiv (trans->transform[0][1], xStretch);
    Fixed m10 = FixDiv (trans->transform[1][0], yStretch);
    Fixed m11 = FixDiv (trans->transform[1][1], yStretch);

    LOOPDOWN (numPts)
    {
      register Fixed origx = *x;
      register Fixed origy = *y;

      *x++ = (F26Dot6) (FixMul (m00, origx) + FixMul (m10, origy));
      *y++ = (F26Dot6) (FixMul (m01, origx) + FixMul (m11, origy));
    }
  }
}


#if 0 // 8/31/92 (FF) does not seem to be used

/* <3> */
/*
 *  Good for transforming scaled coordinates.  Assumes NO translate  <4>
 */
static void fsg_Dot6XYMul (F26Dot6*x, F26Dot6*y, transMatrix*matrix)
{
  register F26Dot6 xTemp, yTemp;
  register Fixed *m0, *m1;

  m0 = (Fixed *) & matrix->transform[0][0];
  m1 = (Fixed *) & matrix->transform[1][0];

  xTemp = *x;
  yTemp = *y;
  *x = (F26Dot6) (FixMul (*m0++, xTemp) + FixMul (*m1++, yTemp));
  *y = (F26Dot6) (FixMul (*m0++, xTemp) + FixMul (*m1++, yTemp));

  if (*m0 || *m1)     /* these two are Fracts */
  {
    Fixed tmp = FracMul (*m0, xTemp) + FracMul (*m1, yTemp);
	 tmp <<= 10;
    tmp += matrix->transform[2][2];
    if (tmp && tmp != ONEFIX)
    {
      *x = (F26Dot6) FixDiv (*x, tmp);
      *y = (F26Dot6) FixDiv (*y, tmp);
    }
  }
}

#endif

/*
 *      fsg_LocalPostTransformGlyph                             <3>
 *
 * (1) Inverts the stretch from the CTM
 * (2) Applies the local transformation passed in in the trans parameter
 * (3) Applies the global stretch from the root CTM
 * (4) Restores oox, ooy, oy, ox, and f.
 */
void fsg_LocalPostTransformGlyph(register fsg_SplineKey *key, transMatrix *trans)
{
#if 0 // 8/31/92 (FF) - these do not seem to be used
  register int            numPts;
  register Fixed          xScale, yScale;
  register F26Dot6*       x;
  register F26Dot6*       y;
#endif
  register int            count;
  fnt_ElementType*        elementPtr = &(key->elementInfoRec.interpreterElements[key->elementNumber]);

  count = NUMBEROFPOINTS(elementPtr);

  fsg_IntelMul (count, elementPtr->x, elementPtr->y, trans, ONEFIX, ONEFIX);
  fsg_IntelMul (count, elementPtr->oox, elementPtr->ooy, trans, ONEFIX, ONEFIX);
  fsg_CopyElementBackwards( &(key->elementInfoRec.interpreterElements[GLYPHELEMENT]) );
}

#if 0
/*
 *  fsg_LocalPostTransformGlyph             <3>
 *
 * (1) Inverts the stretch from the CTM
 * (2) Applies the local transformation passed in in the trans parameter
 * (3) Applies the global stretch from the root CTM
 * (4) Restores oox, ooy, oy, ox, and f.
 */
void fsg_LocalPostTransformGlyph (register fsg_SplineKey *key, transMatrix *trans)
{
  register int16      numPts, count;
  register Fixed      xScale = trans->transform[0][0], yScale = trans->transform[1][1];
  register F26Dot6*  x;
  register F26Dot6*  y;
  fnt_ElementType *    elementPtr = & (key->elementInfoRec.interpreterElements[key->elementNumber]);

  numPts = count = NUMBEROFPOINTS (elementPtr);

  x = elementPtr->x;
  y = elementPtr->y;

  if (xScale == 0L || yScale == 0L)
  {
    register F26Dot6 zero = 0;
    LOOPDOWN (numPts)
    *x++ = *y++ = zero;
  }
  else
  {
    LOOPDOWN (numPts)
    {
      *x = (F26Dot6) FixDiv (*x, xScale);
      *y = (F26Dot6) FixDiv (*y, yScale);
      fsg_Dot6XYMul (x++, y++, trans);
    }
  }

  fsg_CopyElementBackwards (& (key->elementInfoRec.interpreterElements[GLYPHELEMENT]));
  x = elementPtr->oox;
  y = elementPtr->ooy;
  LOOPDOWN (count)
    fsg_Dot6XYMul (x++, y++, trans);
}

#endif


/*
 * fsg_RotateScalePoint
 * fsg_RotateScaleEmX
 * fsg_RotateScaleEmY
 *
 *		These functions apply the rotation and scaling components of a
 * transform matrix to a coordinate, typically in Em Units and return the
 * rotated and scaled coordinate, typically in pixel units.  They are
 * required when using special effects under rotation.
 */


void fsg_RotateScalePoint(int *x, int *y, transMatrix *matrix, int emRes)
{
	Fixed *t = (long *)(&matrix->transform);

	Fixed temp;
	int newx;

	temp = LongMulDiv((long)*x, t[0], emRes) + LongMulDiv((long)*y, t[3], emRes);
 	newx = FIXROUND(temp);

	temp = LongMulDiv((long)*x, t[1], emRes) + LongMulDiv((long)*y, t[4], emRes);
	*y = FIXROUND(temp);
	*x = newx;
}


int fsg_RotateScaleX(int x, transMatrix *matrix, int emRes)
{
	Fixed fx;
	fx = LongMulDiv((long)x, matrix->transform[0][0], emRes);

	return FIXROUND(fx);
}

int fsg_RotateScaleY(int y, transMatrix *matrix, int emRes)
{
	Fixed fy;
	fy = LongMulDiv((long)y, matrix->transform[1][1], (long)emRes);
	return FIXROUND(fy);
}

void fsg_RotateScalePointFix(Fixed *x, Fixed *y, transMatrix *matrix, int emRes)
{
	Fixed *t = (long *)(&matrix->transform);
	long scale = (long)emRes << 16;

	Fixed newx;

	newx = LongMulDiv(*x, t[0], scale) + LongMulDiv(*y, t[3], scale);

	*y = LongMulDiv(*x, t[1], scale) + LongMulDiv(*y, t[4], scale);
	*x = newx;
}


/*
 * fsg_MatrixPoint
 * fsg_MatrixX
 * fsg_MatrixY
 *
 *		These functions are similar to the functions above except that they
 * do not use a scaling factor.  They may be used with matrices that stretch.
 * They apply only the upper-left part of the matrix (the rotate/scale part).
 */

void fsg_MatrixPoint(int *x, int *y, transMatrix *matrix)
{
	Fixed *t = (long *)(&matrix->transform);

	Fixed temp;
	int newx;

	temp = FixMul((long)*x << 16, t[0]) + FixMul((long)*y << 16, t[3]);
 	newx = FIXROUND(temp);

	temp = FixMul((long)*x << 16, t[1]) + FixMul((long)*y << 16, t[4]);
	*y = FIXROUND(temp);
	*x = newx;
}

int fsg_MatrixX(int x, transMatrix *matrix)
{
	return FIXROUND(FixMul((Fixed)x << 16, matrix->transform[0][0]));
}

int fsg_MatrixY(int y, transMatrix *matrix)
{
	return FIXROUND(FixMul((Fixed)y << 16, matrix->transform[1][1]));
}

void fsg_MatrixPointFix(Fixed *x, Fixed *y, transMatrix *matrix)
{
	Fixed *t = (long *)(&matrix->transform);

	Fixed newx;

	newx = FixMul(*x, t[0]) + FixMul(*y, t[3]);

	*y = FixMul(*x, t[1]) + FixMul(*y, t[4]);
	*x = newx;
}

static Fixed fxabs(Fixed fx)
{
	if (fx < 0)
	{
		fx = -fx;
	}
	return fx;
}

/*
 * fsg_ExtractRotationMatrix
 *
 *		This function splits the transform matrix into a pure rotation matrix
 * (with determinant=1) and a pure scaling matrix that just converts em units
 * to pixel units for the current point size and resolution.  
 */


void fsg_ExtractRotationMatrix(transMatrix *inputMatrix,
											Fixed xStretch,
											Fixed yStretch,
											fsg_SplineKey *key)
{
	Fixed *Mi;		/* Matrix in Input structure: rotation&stretching */
	Fixed *Mc;		/* currentTMatrix with point size/resolution scaling */
	Fixed *Mr;		/* pure rotation matrix */
	Fixed *Ms;		/* pure scaling matrix */
	Fixed CosTheta, SinTheta;

	Mi = (Fixed *)inputMatrix;
	Mc = (Fixed *)&key->currentTMatrix;
	Mr = (Fixed *)&key->RotationMatrix;
	Ms = (Fixed *)&key->ScalingMatrix;

	/* Divide by the larger number for better precision */
	if (fxabs(xStretch) >= fxabs(yStretch))
	{
		CosTheta = FixDiv(Mi[0], xStretch);
		SinTheta = -FixDiv(Mi[1], xStretch);
	}
	else
	{
		CosTheta = FixDiv(Mi[4], yStretch);
		SinTheta = FixDiv(Mi[3], yStretch);
	}

	Mr[0] = CosTheta;
	Mr[1] = -SinTheta;
	Mr[2] = 0;
	Mr[3] = SinTheta;
	Mr[4] = CosTheta;
	Mr[5] = 0;
	Mr[6] = 0;
	Mr[7] = 0;
	Mr[8] = 0x10000L;

	/* Divide by the larger number for better precision, and to prevent
		dividing by zero. */
	if (fxabs(CosTheta) >= fxabs(SinTheta))
	{
		Ms[0] = FixDiv(Mc[0], CosTheta);		/* XScale */
		Ms[4] = FixDiv(Mc[4], CosTheta);		/* YScale */
	}
	else
	{
		Ms[0] = FixDiv(Mc[1], -SinTheta);	/* XScale */
		Ms[4] = FixDiv(Mc[3], SinTheta);		/* YScale */
	}
	Ms[1] = 0;
	Ms[2] = 0;
	Ms[3] = 0;
	Ms[5] = 0;
	Ms[6] = 0;
	Ms[7] = 0;
	Ms[8] = 0x10000L;
}

/*
 * get_shadow_offset
 *
 *		The TRUEFX version, gets shadow offsets for an arbitrary rotation.
 * This version can only return pixel values, and must take Em units in
 * its arguments.
 *
 *	Arguments:
 *		uint16 h: Height of font in Em units
 *		uint16 w: Width of font in Em units
 * Returns:
 *		uint32
 *			Hi word: negative vertical shadow offset in pixels.
 *			Lo word: positive horizontal shadow offset in pixels.
 * Vertical offset is negative for historical reasons.
 */

#define FIXTRUNC(f)   ((f)<0?-(Fixed)((-(f))&0xffff0000):f&0xffff0000)

void get_shadow_offset(int32 h, int32 w)
{
	Fixed x0;
	Fixed y0;

	if (Fx.fx_Shadow.m_nOffsetType == ShadowFormat::Absolute)
	{
		// Absolute shadow offset.
		x0 = MulDiv64(Fx.fx_Shadow.m_lXOffset, GlobalKey->emResolution, 72);
		y0 = MulDiv64(Fx.fx_Shadow.m_lYOffset, GlobalKey->emResolution, 72);
	}
	else
	{
		// Relative shadow offset.
		x0 = w<<16;
		y0 = h<<16;

		// Scale by relative percentages.
		x0 = FixMul(x0, Fx.fx_Shadow.m_lXOffset);
		y0 = FixMul(y0, Fx.fx_Shadow.m_lYOffset);
	}

	// Convert base shadow vector to pixel units.
	fsg_RotateScalePointFix(&x0, &y0, &GlobalKey->ScalingMatrix, GlobalKey->emResolution);

	// Add in non-linear-scaling portion: 2 pixels:
	x0 += (Fx.fx_Shadow.m_lXOffset >= 0) ? 0x20000L : -0x20000L;
	y0 += (Fx.fx_Shadow.m_lYOffset >= 0) ? 0x20000L : -0x20000L;

	// Rotate the shadow vector.
	if (Fx.fx_dRotation != 0.0)
	{
		// Rotate the shadow.
		double dX = (double)x0;
		double dY = (double)y0;

		x0 = (Fixed)floor(dX*Fx.fx_dCosRotation-dY*Fx.fx_dSinRotation+0.5);
		y0 = (Fixed)floor(dX*Fx.fx_dSinRotation+dY*Fx.fx_dCosRotation+0.5);
	}

	// Invert Y.
	y0 = -y0;

	// Round to an integer number of pixels.
	Fx.fx_x_offset = FIXROUND(x0);
	Fx.fx_y_offset = FIXROUND(y0);

#if 0
	// Compute base shadow vector.
	uint16 shadow_type = Fx.fx_shadow & 0x0003;
	Fixed x0 = w;
	Fixed y0 = h;

	if (shadow_type == 0 || shadow_type == 2)
	{
		x0 = -x0;
	}
	if (shadow_type == 1 || shadow_type == 2)
	{
		y0 = -y0;
	}

	// Convert to fixed.
	x0 <<= 16;
	y0 <<= 16;

	// Convert base shadow vector to pixel units.
	fsg_RotateScalePointFix(&x0, &y0, &GlobalKey->ScalingMatrix, GlobalKey->emResolution);

	// Scale vector by 5/256 (magic MoreFonts fraction.)
	x0 = (x0 * 5L) >> 8;
	y0 = (y0 * 5L) >> 8;

	// Adjust by miminum vector size.
	Fixed x1 = 0x20000L;
	Fixed y1 = 0x20000L;

	if (shadow_type == 0 || shadow_type == 2)
	{
		x1 = -x1;
	}
	if (shadow_type == 1 || shadow_type == 2)
	{
		y1 = -y1;
	}

	x0 += x1;
	y0 += y1;

	if (Fx.fx_dRotation != 0.0)
	{
		// Rotate the shadow.
		double dX = (double)x0;
		double dY = (double)y0;

		x0 = (Fixed)floor(dX*Fx.fx_dCosRotation-dY*Fx.fx_dSinRotation+0.5);
		y0 = (Fixed)floor(dX*Fx.fx_dSinRotation+dY*Fx.fx_dCosRotation+0.5);
	}

	// Convert to MoreFonts coordinates (inverted Y).
	y0 = -y0;

	Fx.fx_x_offset = FIXROUND(x0);
	Fx.fx_y_offset = FIXROUND(y0);
#endif
}

void rotate_pen(OutlineDescriptor* pPen)
{
	if (pPen->nType == OUTLINE_diagonal)
	{
		double x, y;

		x = (double)(pPen->nXSize);
		y = (double)(pPen->nYSize);

		pPen->nXSize = (int32)floor( x*Fx.fx_dCosRotation+y*Fx.fx_dSinRotation+0.5);
		pPen->nYSize = (int32)floor(-x*Fx.fx_dSinRotation+y*Fx.fx_dCosRotation+0.5);

		x = (double)(pPen->nXOffset);
		y = (double)(pPen->nYOffset);

		pPen->nXOffset = (int32)floor( x*Fx.fx_dCosRotation+y*Fx.fx_dSinRotation+0.5);
		pPen->nYOffset = (int32)floor(-x*Fx.fx_dSinRotation+y*Fx.fx_dCosRotation+0.5);
	}
}
