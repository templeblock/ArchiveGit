/*---------------------------------------------------------------------------*
 |              PDFlib - A library for generating PDF on the fly             |
 +---------------------------------------------------------------------------+
 | Copyright (c) 1997-2002 PDFlib GmbH and Thomas Merz. All rights reserved. |
 +---------------------------------------------------------------------------+
 |    This software is NOT in the public domain.  It can be used under two   |
 |    substantially different licensing terms:                               |
 |                                                                           |
 |    The commercial license is available for a fee, and allows you to       |
 |    - ship a commercial product based on PDFlib                            |
 |    - implement commercial Web services with PDFlib                        |
 |    - distribute (free or commercial) software when the source code is     |
 |      not made available                                                   |
 |    Details can be found in the file PDFlib-license.pdf.                   |
 |                                                                           |
 |    The "Aladdin Free Public License" doesn't require any license fee,     |
 |    and allows you to                                                      |
 |    - develop and distribute PDFlib-based software for which the complete  |
 |      source code is made available                                        |
 |    - redistribute PDFlib non-commercially under certain conditions        |
 |    - redistribute PDFlib on digital media for a fee if the complete       |
 |      contents of the media are freely redistributable                     |
 |    Details can be found in the file aladdin-license.pdf.                  |
 |                                                                           |
 |    These conditions extend to ports to other programming languages.       |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* $Id: p_draw.c,v 1.15.2.2 2002/01/07 18:26:29 tm Exp $
 *
 * PDFlib drawing routines
 *
 */

#include <math.h>

#include "p_intern.h"

/* Path segment operators */

static void
pdf_begin_path(PDF *p)
{
    if (PDF_GET_STATE(p) == pdf_state_path)
	return;

    pdf_end_text(p);
    p->contents = c_path;
    PDF_PUSH_STATE(p, "pdf_begin_path", pdf_state_path);
}

static void
pdf_end_path(PDF *p)
{
    p->contents = c_page;
    PDF_POP_STATE(p, "pdf_end_path");

    p->gstate[p->sl].x = (float) 0.0;
    p->gstate[p->sl].y = (float) 0.0;
}

PDFLIB_API void PDFLIB_CALL
PDF_moveto(PDF *p, float x, float y)
{
    static const char fn[] = "PDF_moveto";

    PDF_TRACE(("%s\t(pdf[%p], %f, %f);\n", fn, (void *) p, x, y));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt | pdf_state_path);

    /* 'm' starts a new subpath
    */
    p->gstate[p->sl].startx = p->gstate[p->sl].x = x;
    p->gstate[p->sl].starty = p->gstate[p->sl].y = y;

    pdf_begin_path(p);

    pdf_printf(p, "%f %f m\n", x, y);
}

PDFLIB_API void PDFLIB_CALL
PDF_lineto(PDF *p, float x, float y)
{
    static const char fn[] = "PDF_lineto";

    PDF_TRACE(("%s\t(pdf[%p], %f, %f);\n", fn, (void *) p, x, y));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    pdf_printf(p, "%f %f l\n", x, y);

    p->gstate[p->sl].x = x;
    p->gstate[p->sl].y = y;
}

PDFLIB_API void PDFLIB_CALL
PDF_curveto(PDF *p, float x1, float y1, float x2, float y2, float x3, float y3)
{
    static const char fn[] = "PDF_curveto";

    PDF_TRACE(("%s\t(pdf[%p], %f, %f, %f, %f, %f, %f);\n",
	fn, (void *) p, x1, y1, x2, y2, x3, y3));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    if (x2 == x3 && y2 == y3)	/* second c.p. coincides with final point */
	pdf_printf(p, "%f %f %f %f y\n", x1, y1, x3, y3);

    else			/* general case with four distinct points */
	pdf_printf(p, "%f %f %f %f %f %f c\n", x1, y1, x2, y2, x3, y3);

    p->gstate[p->sl].x = x3;
    p->gstate[p->sl].y = y3;
}

/* 4/3 * (1-cos 45°)/sin 45° = 4/3 * sqrt(2) - 1 */
#define ARC_MAGIC	((float) 0.552284749)

static void
pdf_short_arc(PDF *p, float x, float y, float r, float alpha, float beta)
{
    float bcp;
    float cos_alpha, cos_beta, sin_alpha, sin_beta;

    alpha = (float) (alpha * PDF_M_PI / 180);	/* convert to radians */
    beta  = (float) (beta * PDF_M_PI / 180);	/* convert to radians */

    /* This formula yields ARC_MAGIC for alpha == 0, beta == 90 degrees */
    bcp = (float) (4.0/3 * (1 - cos((beta - alpha)/2)) / sin((beta - alpha)/2));
    
    sin_alpha = (float) sin(alpha);
    sin_beta = (float) sin(beta);
    cos_alpha = (float) cos(alpha);
    cos_beta = (float) cos(beta);

    PDF_curveto(p, 
		x + r * (cos_alpha - bcp * sin_alpha),		/* p1 */
		y + r * (sin_alpha + bcp * cos_alpha),
		x + r * (cos_beta + bcp * sin_beta),		/* p2 */
		y + r * (sin_beta - bcp * cos_beta),
		x + r * cos_beta,				/* p3 */
		y + r * sin_beta);
} /* pdf_short_arc */

PDFLIB_API void PDFLIB_CALL
PDF_arc(PDF *p, float x, float y, float r, float alpha, float beta)
{
    static const char fn[] = "PDF_arc";

    float rad_a = (float) (alpha * PDF_M_PI / 180);
    float startx = (float) (x + r * cos(rad_a));
    float starty = (float) (y + r * sin(rad_a));

    PDF_TRACE(("%s\t\t(pdf[%p], %f, %f, %f, %f, %f);\n",
	fn, (void *) p, x, y, r, alpha, beta));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt | pdf_state_path);

    if (r < 0)
	pdf_error(p, PDF_ValueError, "Negative arc radius %f in PDF_arc", r);

    if (p->contents != c_path)
	PDF_moveto(p, startx, starty);
    else if ((p->gstate[p->sl].x != startx || p->gstate[p->sl].y != starty))
	PDF_lineto(p, startx, starty);

    while (beta < alpha)
	beta += 360;

    if (alpha == beta)
	return;

    while (beta - alpha > 90)
    {
	pdf_short_arc(p, x, y, r, alpha, alpha + 90);
	alpha += 90;
    }

    if (alpha != beta)
	pdf_short_arc(p, x, y, r, alpha, beta);
} /* PDF_arc */

PDFLIB_API void PDFLIB_CALL
PDF_arcn(PDF *p, float x, float y, float r, float alpha, float beta)
{
    static const char fn[] = "PDF_arcn";

    float rad_a = (float) (alpha * PDF_M_PI / 180);
    float startx = (float) (x + r * cos(rad_a));
    float starty = (float) (y + r * sin(rad_a));

    PDF_TRACE(("%s\t\t(pdf[%p], %f, %f, %f, %f, %f);\n",
	fn, (void *) p, x, y, r, alpha, beta));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt | pdf_state_path);

    if (r < 0)
	pdf_error(p, PDF_ValueError, "Negative arc radius %f in PDF_arcn", r);

    if (p->contents != c_path)
	PDF_moveto(p, startx, starty);
    else if ((p->gstate[p->sl].x != startx || p->gstate[p->sl].y != starty))
	PDF_lineto(p, startx, starty);

    while (alpha < beta)
	alpha += 360;

    if (alpha == beta)
	return;

    while (alpha - beta > 90)
    {
	pdf_short_arc(p, x, y, r, alpha, alpha - 90);
	alpha -= 90;
    }

    if (alpha != beta)
	pdf_short_arc(p, x, y, r, alpha, beta);
} /* PDF_arcn */

PDFLIB_API void PDFLIB_CALL
PDF_circle(PDF *p, float x, float y, float r)
{
    static const char fn[] = "PDF_circle";

    PDF_TRACE(("%s\t(pdf[%p], %f, %f, %f);\n", fn, (void *) p, x, y, r));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt | pdf_state_path);

    if (r < 0)
	pdf_error(p, PDF_ValueError, "Negative circle radius %f", r);

    /*
     * pdf_begin_path() not required since we descend to other
     * path segment functions.
     */

    /* draw four Bezier curves to approximate a circle */
    PDF_moveto(p, x + r, y);
    PDF_curveto(p, x + r, y + r*ARC_MAGIC, x + r*ARC_MAGIC, y + r, x, y + r);
    PDF_curveto(p, x - r*ARC_MAGIC, y + r, x - r, y + r*ARC_MAGIC, x - r, y);
    PDF_curveto(p, x - r, y - r*ARC_MAGIC, x - r*ARC_MAGIC, y - r, x, y - r);
    PDF_curveto(p, x + r*ARC_MAGIC, y - r, x + r, y - r*ARC_MAGIC, x + r, y);

    PDF_closepath(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_rect(PDF *p, float x, float y, float width, float height)
{
    static const char fn[] = "PDF_rect";

    PDF_TRACE(("%s\t\t(pdf[%p], %f, %f, %f, %f);\n",
	fn, (void *) p, x, y, width, height));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt | pdf_state_path);

    /* 're' creates a complete subpath
    */
    p->gstate[p->sl].startx = p->gstate[p->sl].x = x;
    p->gstate[p->sl].starty = p->gstate[p->sl].y = y;

    pdf_begin_path(p);

    pdf_printf(p, "%f %f %f %f re\n", x, y, width, height);
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath(PDF *p)
{
    static const char fn[] = "PDF_closepath";

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    pdf_puts(p, "h\n");

    p->gstate[p->sl].x = p->gstate[p->sl].startx;
    p->gstate[p->sl].y = p->gstate[p->sl].starty;
}

/* Path painting operators */

PDFLIB_API void PDFLIB_CALL
PDF_endpath(PDF *p)
{
    static const char fn[] = "PDF_endpath";

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    pdf_puts(p, "n\n");
    pdf_end_path(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_stroke(PDF *p)
{
    static const char fn[] = "PDF_stroke";

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    pdf_puts(p, "S\n");
    pdf_end_path(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath_stroke(PDF *p)
{
    static const char fn[] = "PDF_closepath_stroke";

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    pdf_puts(p, "s\n");
    pdf_end_path(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_fill(PDF *p)
{
    static const char fn[] = "PDF_fill";

    PDF_TRACE(("%s\t\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    switch (p->fillrule) {
	case pdf_fill_winding:
	    pdf_puts(p, "f\n");
	    break;

	case pdf_fill_evenodd:
	    pdf_puts(p, "f*\n");
	    break;

	default:
	    pdf_error(p, PDF_SystemError,
		"Inconsistent fill rule value in PDF_fill");
    }

    pdf_end_path(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_fill_stroke(PDF *p)
{
    static const char fn[] = "PDF_fill_stroke";

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    switch (p->fillrule) {
	case pdf_fill_winding:
	    pdf_puts(p, "B\n");
	    break;

	case pdf_fill_evenodd:
	    pdf_puts(p, "B*\n");
	    break;

	default:
	    pdf_error(p, PDF_SystemError,
		"Inconsistent fill rule value in PDF_fill_stroke");
    }

    pdf_end_path(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath_fill_stroke(PDF *p)
{
    static const char fn[] = "PDF_closepath_fill_stroke";

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    switch (p->fillrule) {
	case pdf_fill_winding:
	    pdf_puts(p, "b\n");
	    break;

	case pdf_fill_evenodd:
	    pdf_puts(p, "b*\n");
	    break;

	default:
	    pdf_error(p, PDF_SystemError,
		"Inconsistent fill rule value in PDF_closepath_fill_stroke");
    }

    pdf_end_path(p);
}

/* Path clipping operators */

PDFLIB_API void PDFLIB_CALL
PDF_clip(PDF *p)
{
    static const char fn[] = "PDF_clip";

    PDF_TRACE(("%s\t\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_path);

    switch (p->fillrule) {
	case pdf_fill_winding:
	    pdf_puts(p, "W n\n");
	    break;

	case pdf_fill_evenodd:
	    pdf_puts(p, "W* n\n");
	    break;

	default:
	    pdf_error(p, PDF_SystemError,
		"Inconsistent fill rule value in PDF_clip");
	    return;
    }

    pdf_end_path(p);
}
