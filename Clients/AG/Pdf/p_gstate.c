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

/* $Id: p_gstate.c,v 1.24.2.3 2002/01/16 19:05:07 tm Exp $
 *
 * PDFlib routines dealing with the graphics states
 *
 */

#include <math.h>
#include <string.h>

#include "p_intern.h"

/* ---------------------- matrix functions ----------------------------- */

pdf_bool
pdf_is_identity_matrix(pdf_matrix *m)
{
    return (m->a == (float) 1 &&
	    m->b == (float) 0 &&
	    m->c == (float) 0 &&
	    m->d == (float) 1 &&
	    m->e == (float) 0 &&
	    m->f == (float) 0);
}

/* left-multiply M to N and store the result in N */
void
pdf_matrix_mul(const pdf_matrix *M, pdf_matrix *N)
{
    pdf_matrix result;

    result.a = M->a * N->a + M->b * N->c;
    result.b = M->a * N->b + M->b * N->d;
    result.c = M->c * N->a + M->d * N->c;
    result.d = M->c * N->b + M->d * N->d;

    result.e = M->e * N->a + M->f * N->c + N->e;
    result.f = M->e * N->b + M->f * N->d + N->f;

    N->a = result.a;
    N->b = result.b;
    N->c = result.c;
    N->d = result.d;
    N->e = result.e;
    N->f = result.f;
}

void
pdf_concat_raw(PDF *p, pdf_matrix *m)
{
    if (pdf_is_identity_matrix(m))
	return;

    pdf_end_text(p);

    pdf_printf(p, "%f %f %f %f %f %f cm\n",
		    m->a, m->b, m->c, m->d, m->e, m->f);

    pdf_matrix_mul(m, &p->gstate[p->sl].ctm);
}

/* invert M and store the result in N */
static void
pdf_invert_matrix(PDF *p, pdf_matrix *N, pdf_matrix *M)
{
    float det = M->a * M->d - M->b * M->c;

    if (fabs(det) < (float) PDF_SMALLREAL)
	pdf_error(p, PDF_SystemError, "CTM not invertible");
    
    N->a = M->d/det;
    N->b = -M->b/det;
    N->c = -M->c/det;
    N->d = M->a/det;
    N->e = -(M->e * N->a + M->f * N->c);
    N->f = -(M->e * N->b + M->f * N->d);
}

/* -------------------- Special graphics state ---------------------------- */

void
pdf_init_gstate(PDF *p)
{
    pdf_gstate *gs = &p->gstate[p->sl];

    gs->ctm.a = (float) 1;
    gs->ctm.b = (float) 0;
    gs->ctm.c = (float) 0;
    gs->ctm.d = (float) 1;
    gs->ctm.e = (float) 0.0;
    gs->ctm.f = (float) 0.0;

    gs->x = (float) 0.0;
    gs->y = (float) 0.0;

    p->fillrule = pdf_fill_winding;

    gs->lwidth = (float) 1;
    gs->lcap = 0;
    gs->ljoin = 0;
    gs->miter = (float) 10;
    gs->flatness = (float) -1;	/* -1 means "has not been set" */
    gs->dashed = pdf_false;
}

PDFLIB_API void PDFLIB_CALL
PDF_save(PDF *p)
{
    static const char fn[] = "PDF_save";

    PDF_TRACE(("%s\t\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (p->sl == PDF_MAX_SAVE_LEVEL - 1)
	pdf_error(p, PDF_RuntimeError, "Too many save levels");

    pdf_end_text(p);

    pdf_puts(p, "q\n");

    /* propagate states to next level */
    p->sl++;
    memcpy(&p->gstate[p->sl], &p->gstate[p->sl - 1], sizeof(pdf_gstate));
    memcpy(&p->tstate[p->sl], &p->tstate[p->sl - 1], sizeof(pdf_tstate));
    memcpy(&p->cstate[p->sl], &p->cstate[p->sl - 1], sizeof(pdf_cstate));
}

PDFLIB_API void PDFLIB_CALL
PDF_restore(PDF *p)
{
    static const char fn[] = "PDF_restore";

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (p->sl == 0)
	pdf_error(p, PDF_RuntimeError, "Invalid restore");

    pdf_end_text(p);

    pdf_puts(p, "Q\n");

    p->sl--;
}

PDFLIB_API void PDFLIB_CALL
PDF_translate(PDF *p, float tx, float ty)
{
    static const char fn[] = "PDF_translate";
    pdf_matrix m;

    PDF_TRACE(("%s\t(pdf[%p], %f, %f);\n", fn, (void *) p, tx, ty));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (tx == (float) 0 && ty == (float) 0)
	return;

    m.a = (float) 1;
    m.b = (float) 0;
    m.c = (float) 0;
    m.d = (float) 1;
    m.e = tx;
    m.f = ty;

    pdf_concat_raw(p, &m);
}

PDFLIB_API void PDFLIB_CALL
PDF_scale(PDF *p, float sx, float sy)
{
    static const char fn[] = "PDF_scale";
    pdf_matrix m;

    PDF_TRACE(("%s\t(pdf[%p], %f, %f);\n", fn, (void *) p, sx, sy));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (sx == (float) 0 || sy == (float) 0)
	pdf_error(p, PDF_ValueError, "Zero scaling factor");

    if (sx == (float) 1 && sy == (float) 1)
	return;

    m.a = sx;
    m.b = (float) 0;
    m.c = (float) 0;
    m.d = sy;
    m.e = (float) 0;
    m.f = (float) 0;

    pdf_concat_raw(p, &m);
}

PDFLIB_API void PDFLIB_CALL
PDF_rotate(PDF *p, float phi)
{
    static const char fn[] = "PDF_rotate";
    pdf_matrix m;
    float c, s;

    PDF_TRACE(("%s\t(pdf[%p], %f);\n", fn, (void *) p, phi));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (phi == (float) 0)
	return;

    phi = (float) (phi * PDF_M_PI / 180);	/* convert to radians */

    c = (float) cos(phi);
    s = (float) sin(phi);

    m.a = (float) c;
    m.b = (float) s;
    m.c = (float) -s;
    m.d = (float) c;
    m.e = (float) 0;
    m.f = (float) 0;

    pdf_concat_raw(p, &m);
}

PDFLIB_API void PDFLIB_CALL
PDF_skew(PDF *p, float alpha, float beta)
{
    static const char fn[] = "PDF_skew";
    pdf_matrix m;

    PDF_TRACE(("%s\t\t(pdf[%p], %f, %f);\n", fn, (void *) p, alpha, beta));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (alpha == (float) 0 && beta == (float) 0)
	return;

    if (alpha > (float) 360 || alpha < (float) -360 ||
	beta > (float) 360 || beta < (float) -360 ||
	alpha == (float) -90 || alpha == (float) -270 ||
	beta == (float) -90 || beta == (float) -270 ||
	alpha == (float) 90 || alpha == (float) 270 ||
	beta == (float) 90 || beta == (float) 270) {
	pdf_error(p, PDF_ValueError, "Bad skewing values %f, %f", alpha, beta);
    }

    m.a = (float) 1;
    m.b = (float) tan((float) (alpha * PDF_M_PI / 180));
    m.c = (float) tan((float) (beta * PDF_M_PI / 180));
    m.d = (float) 1;
    m.e = (float) 0;
    m.f = (float) 0;

    pdf_concat_raw(p, &m);
}

PDFLIB_API void PDFLIB_CALL
PDF_concat(PDF *p, float a, float b, float c, float d, float e, float f)
{
    static const char fn[] = "PDF_concat";
    pdf_matrix m;
    float det = a * d - b * c;

    PDF_TRACE(("%s\t(pdf[%p], %f, %f, %f, %f, %f, %f);\n",
	fn, (void *) p, a, b, c, d, e, f));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (fabs(det) < (float) PDF_SMALLREAL)
	pdf_error(p, PDF_ValueError, "Degenerate matrix in PDF_concat");

    m.a = (float) a;
    m.b = (float) b;
    m.c = (float) c;
    m.d = (float) d;
    m.e = (float) e;
    m.f = (float) f;

    pdf_concat_raw(p, &m);
}

PDFLIB_API void PDFLIB_CALL
PDF_setmatrix(PDF *p, float a, float b, float c, float d, float e, float f)
{
    static const char fn[] = "PDF_setmatrix";
    pdf_matrix m;
    float det = a * d - b * c;

    PDF_TRACE(("%s\t(pdf[%p], %f, %f, %f, %f, %f, %f);\n",
	fn, (void *) p, a, b, c, d, e, f));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (fabs(det) < (float) PDF_SMALLREAL)
	pdf_error(p, PDF_ValueError, "Degenerate matrix in PDF_setmatrix");

    pdf_invert_matrix(p, &m, &p->gstate[p->sl].ctm);
    pdf_concat_raw(p, &m);

    m.a = (float) a;
    m.b = (float) b;
    m.c = (float) c;
    m.d = (float) d;
    m.e = (float) e;
    m.f = (float) f;

    pdf_concat_raw(p, &m);
}

/* -------------------- General graphics state ---------------------------- */

PDFLIB_API void PDFLIB_CALL
PDF_setdash(PDF *p, float b, float w)
{
    static const char fn[] = "PDF_setdash";
    pdf_gstate *gs = &p->gstate[p->sl];

    PDF_TRACE(("%s\t(pdf[%p], %f, %f);\n", fn, (void *) p, b, w));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (b < (float) 0.0 || w < (float) 0.0) {
	pdf_error(p, PDF_NonfatalError, "Negative dash value %f, %f", b, w);
	return;
    }

    if (b == 0.0 && w == 0.0)		/* both zero means solid line */
    {
	if (gs->dashed)
	{
	    pdf_puts(p, "[] 0 d\n");
	    gs->dashed = pdf_false;
	}
    }
    else
    {
	pdf_printf(p, "[%f %f] 0 d\n", b, w);
	gs->dashed = pdf_true;
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_setpolydash(PDF *p, float *darray, int length)
{
    static const char fn[] = "PDF_setpolydash";
    pdf_gstate *gs = &p->gstate[p->sl];

    int i;

    PDF_TRACE(("%s\t(pdf[%p], fn, darray[%p], %d);\n",
	fn, (void *) p, (void *) darray, length));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (length == 0 || length == 1) {	/* length == 0 or 1 means solid line */
	pdf_puts(p, "[] 0 d\n");
	return;
    }

    /* sanity checks */
    if (!darray || length < 0) {
	pdf_error(p, PDF_RuntimeError, "Bad array in PDF_setpolydash");
	return;
    }

    if (length > MAX_DASH_LENGTH) {
	pdf_error(p, PDF_NonfatalError, "Dash array too long (%d) - truncated",
		length);
	length = MAX_DASH_LENGTH;
    }

    for (i = 0; i < length; i++) {
	if (darray[i] < (float) 0.0) {
	    pdf_error(p, PDF_NonfatalError, "Negative dash value %f",
	    	darray[i]);
	    return;
	}
    }

    pdf_puts(p, "[");

    for (i = 0; i < length; i++) {
	pdf_printf(p, "%f ", darray[i]);
    }
    pdf_puts(p, "] 0 d\n");
    gs->dashed = pdf_true;
}

PDFLIB_API void PDFLIB_CALL
PDF_setflat(PDF *p, float flat)
{
    static const char fn[] = "PDF_setflat";
    pdf_gstate *gs = &p->gstate[p->sl];

    PDF_TRACE(("%s\t(pdf[%p], %f);\n", fn, (void *) p, flat));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (flat < 0.0 || flat > 100.0) {
	pdf_error(p, PDF_NonfatalError, "Illegal flat value %f", flat);
	return;
    }

    if (flat != gs->flatness)
    {
	gs->flatness = flat;
	pdf_printf(p, "%f i\n", flat);
    }
}

#define LAST_JOIN	2

PDFLIB_API void PDFLIB_CALL
PDF_setlinejoin(PDF *p, int join)
{
    static const char fn[] = "PDF_setlinejoin";
    pdf_gstate *gs = &p->gstate[p->sl];

    PDF_TRACE(("%s\t(pdf[%p], %d);\n", fn, (void *) p, join));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (join > LAST_JOIN) {
	pdf_error(p, PDF_NonfatalError, "Bogus line join value %d", join);
	return;
    }

    if (join != gs->ljoin)
    {
	gs->ljoin = join;
	pdf_printf(p, "%d j\n", join);
    }
}
#undef LAST_JOIN

#define LAST_CAP	2

PDFLIB_API void PDFLIB_CALL
PDF_setlinecap(PDF *p, int cap)
{
    static const char fn[] = "PDF_setlinecap";
    pdf_gstate *gs = &p->gstate[p->sl];

    PDF_TRACE(("%s\t(pdf[%p], %d);\n", fn, (void *) p, cap));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (cap > LAST_CAP) {
	pdf_error(p, PDF_NonfatalError, "Bogus line cap value %d", cap);
	return;
    }

    if (cap != gs->lcap)
    {
	gs->lcap = cap;
	pdf_printf(p, "%d J\n", cap);
    }
}
#undef LAST_CAP

PDFLIB_API void PDFLIB_CALL
PDF_setmiterlimit(PDF *p, float miter)
{
    static const char fn[] = "PDF_setmiterlimit";
    pdf_gstate *gs = &p->gstate[p->sl];

    PDF_TRACE(("%s\t(pdf[%p], %f);\n", fn, (void *) p, miter));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (miter < (float) 1.0) {
	pdf_error(p, PDF_NonfatalError, "Bogus miter limit %f", miter);
	return;
    }

    if (miter != gs->miter)
    {
	gs->miter = miter;
	pdf_printf(p, "%f M\n", miter);
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_setlinewidth(PDF *p, float width)
{
    static const char fn[] = "PDF_setlinewidth";
    pdf_gstate *gs = &p->gstate[p->sl];

    PDF_TRACE(("%s\t(pdf[%p], %f);\n", fn, (void *) p, width));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (width <= (float) 0.0) {
	pdf_error(p, PDF_NonfatalError, "Bogus line width %f", width);
	return;
    }

    if (width != gs->lwidth)
    {
	gs->lwidth = width;
	pdf_printf(p, "%f w\n", width);
    }
}

/* reset all gstate parameters except CTM
*/
void
pdf_reset_gstate(PDF *p)
{
    pdf_gstate *gs = &p->gstate[p->sl];

    PDF_setcolor(p, "both", "gray", (float) 0, (float) 0, (float) 0, (float) 0);

    PDF_setlinewidth(p, 1);
    PDF_setlinecap(p, 0);
    PDF_setlinejoin(p, 0);
    PDF_setmiterlimit(p, 10);
    PDF_setdash(p, 0, 0);

    if (gs->flatness != (float) -1)
	PDF_setflat(p, (float) 1.0);

    /* LATER: rendering intent and flatness */
}

PDFLIB_API void PDFLIB_CALL
PDF_initgraphics(PDF *p)
{
    static const char fn[] = "PDF_initgraphics";
    pdf_matrix inv_ctm;

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    pdf_reset_gstate(p);

    pdf_invert_matrix(p, &inv_ctm, &p->gstate[p->sl].ctm);
    pdf_concat_raw(p, &inv_ctm);

    /* This also resets the CTM which guards against rounding artifacts. */
    pdf_init_gstate(p);
}
