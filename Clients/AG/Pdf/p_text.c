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

/* $Id: p_text.c,v 1.25.2.16 2002/01/29 10:10:48 tm Exp $
 *
 * PDFlib text routines
 *
 */

#include <string.h>
#include <ctype.h>
#include <math.h>

#include "p_intern.h"
#include "p_font.h"

/* ------------------------ Text object operators ------------------------ */

static void
pdf_begin_text(PDF *p, pdf_bool setpos)
{
    pdf_matrix *m;

    if (p->contents != c_text) {
	p->procset	|= Text;
	p->contents	= c_text;
	p->textparams_done = pdf_false;

	pdf_puts(p, "BT\n");

	/* BT resets the current point, text matrix, and line matrix */
	p->gstate[p->sl].x = (float) 0.0;
	p->gstate[p->sl].y = (float) 0.0;
    }

    /* output the matrix. this will result in one superfluous Tm
     * iff the first action on a new page is eg. a show_xy(0, 0).
     */
    m = &p->tstate[p->sl].m;
    if (setpos) {
	pdf_printf(p, "%f %f %f %f %f %f Tm\n",
			  m->a, m->b, m->c, m->d, m->e, m->f);
    }
}

void
pdf_end_text(PDF *p)
{
    if (p->contents != c_text)
	return;

    p->contents	= c_page;

    pdf_puts(p, "ET\n");
}

/* ------------------------ Text state operators ------------------------ */

/* Initialize the text state at the beginning of each page */
void
pdf_init_tstate(PDF *p)
{
    pdf_tstate *ts;

    ts = &p->tstate[p->sl];

    ts->c	= (float) 0;
    ts->w	= (float) 0;
    ts->h	= (float) 1;
    ts->l	= (float) 0;
    ts->f	= -1;
    ts->fs	= (float) 0;

    ts->m.a	= (float) 1;
    ts->m.b	= (float) 0;
    ts->m.c	= (float) 0;
    ts->m.d	= (float) 1;
    ts->m.e	= (float) 0;
    ts->m.f	= (float) 0;

    ts->mode	= 0;
    ts->rise	= (float) 0;

    ts->lm.a	= (float) 1;
    ts->lm.b	= (float) 0;
    ts->lm.c	= (float) 0;
    ts->lm.d	= (float) 1;
    ts->lm.e	= (float) 0;
    ts->lm.f	= (float) 0;

    p->underline	= pdf_false;
    p->overline		= pdf_false;
    p->strikeout	= pdf_false;
    p->textparams_done	= pdf_false;
}

/* character spacing for justified lines */
void
pdf_set_char_spacing(PDF *p, float spacing)
{
    if (PDF_GET_STATE(p) == pdf_state_document)
    {
	p->tstate[p->sl].c = spacing;
	return;
    }

    /*
     * We take care of spacing values != 0 in the text output functions,
     * but must explicitly reset here.
     */
    if (spacing == (float) 0) {
	pdf_begin_text(p, pdf_false);
	pdf_printf(p, "%f Tc\n", spacing);
    }

    p->tstate[p->sl].c = spacing;
    p->textparams_done = pdf_false;
}

/* word spacing for justified lines */
void
pdf_set_word_spacing(PDF *p, float spacing)
{
    if (PDF_GET_STATE(p) == pdf_state_document)
    {
	p->tstate[p->sl].w = spacing;
	return;
    }

    /*
     * We take care of spacing values != 0 in the text output functions,
     * but must explicitly reset here.
     */
    if (spacing == (float) 0) {
	pdf_begin_text(p, pdf_false);
	pdf_printf(p, "%f Tw\n", spacing);
    }

    p->tstate[p->sl].w = spacing;
    p->textparams_done = pdf_false;
}

void
pdf_set_horiz_scaling(PDF *p, float scale)
{
    if (scale == (float) 0.0)
	pdf_error(p, PDF_ValueError, "Bad horizontal text scaling 0");
	
    if (PDF_GET_STATE(p) == pdf_state_document)
    {
	p->tstate[p->sl].h = scale / (float) 100.0;
	return;
    }

    if (scale == 100 * p->tstate[p->sl].h)
	return;

    pdf_begin_text(p, pdf_false);
    pdf_printf(p, "%f Tz\n", scale);

    p->tstate[p->sl].h = scale / (float) 100.0;
}

void
pdf_set_leading(PDF *p, float l)
{
    if (l == p->tstate[p->sl].l)
	return;

    p->tstate[p->sl].l = l;
    p->textparams_done = pdf_false;
}

#define LAST_MODE	7

void
pdf_set_text_rendering(PDF *p, int mode)
{
    if (mode < 0 || mode > LAST_MODE) {
	pdf_error(p, PDF_NonfatalError, "Bogus text rendering mode %d", mode);
	return;
    }
		
    if (mode == p->tstate[p->sl].mode)
	return;

    pdf_begin_text(p, pdf_false);
    pdf_printf(p, "%d Tr\n", mode);

    p->tstate[p->sl].mode = mode;
}
#undef LAST_MODE

void
pdf_set_text_rise(PDF *p, float rise)
{
    if (PDF_GET_STATE(p) == pdf_state_document)
    {
	p->tstate[p->sl].rise = rise;
	return;
    }

    if (rise == p->tstate[p->sl].rise)
	return;

    pdf_begin_text(p, pdf_false);
    pdf_printf(p, "%f Ts\n", rise);

    p->tstate[p->sl].rise = rise;
}

/* Text positioning operators */

PDFLIB_API void PDFLIB_CALL
PDF_set_text_matrix(PDF *p,
float a, float b, float c, float d, float e, float f)
{
    static const char fn[] = "PDF_set_text_matrix";

    PDF_TRACE(("%s\t(pdf[%p], %f, %f, %f, %f, %f, %f);\n",
	fn, (void *) p, a, b, c, d, e, f));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt | pdf_state_document);

    p->tstate[p->sl].m.a = a;
    p->tstate[p->sl].m.b = b;
    p->tstate[p->sl].m.c = c;
    p->tstate[p->sl].m.d = d;
    p->tstate[p->sl].m.e = e;
    p->tstate[p->sl].m.f = f;

    if (PDF_GET_STATE(p) != pdf_state_document)
	pdf_begin_text(p, pdf_true);
}

PDFLIB_API void PDFLIB_CALL
PDF_set_text_pos(PDF *p, float x, float y)
{
    static const char fn[] = "PDF_set_text_pos";

    PDF_TRACE(("%s\t(pdf[%p], %f, %f);\n", fn, (void *) p, x, y));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    p->tstate[p->sl].m.e = x;
    p->tstate[p->sl].m.f = y;

    p->tstate[p->sl].lm.e = x;
    p->tstate[p->sl].lm.f = y;

    pdf_begin_text(p, pdf_true);
}

/* String width calculations */

static float
pdf_str_width(PDF *p, const char *text, int len, int font, float size)
{
    const char *cp;
    int i;
    float width = (float) 0.0;

    if (len == 0)
	len = (int) strlen(text);

    /* We cannot handle CID fonts */
    if (p->fonts[font].encoding == cid) {
	PDF_TRACE((" [%f]\n", width));
	return width;
    }

    for (i = 0, cp = text; i < len; cp++, i++) {
	/* take word spacing parameter into account at each blank */
	if (*cp == PDF_SPACE)
	    width += p->tstate[p->sl].w;

	/* individual character width plus character spacing parameter */
	width += size * p->fonts[font].widths[(unsigned char) *cp]/((float)1000)
		 + p->tstate[p->sl].c;
    }

    /* take current text matrix and horizontal scaling factor into account */
    width = width * p->tstate[p->sl].m.a * p->tstate[p->sl].h;

    return width;
}

PDFLIB_API float PDFLIB_CALL
PDF_stringwidth(PDF *p, const char *text, int font, float size)
{
    static const char fn[] = "PDF_stringwidth";
    float result;

    PDF_TRACE(("%s\t(pdf[%p], \"%s\", %d, %f);\n",
	fn, (void *) p, text, font, size));

    if (PDF_SANITY_CHECK_FAILED(p)) {
	PDF_TRACE((" [%f]\n", (float)0));
	return 0;
    }

    PDF_CHECK_SCOPE(p, fn,
	pdf_state_document | pdf_state_ppt | pdf_state_path);

    /* Check parameters */
    if (text == NULL || *text == '\0') {
	PDF_TRACE((" [%f]\n", (float)0));
	return 0;
    }

    if (font < 0 || font >= p->fonts_number)
	pdf_error(p, PDF_ValueError, "Bad font descriptor %d in %s", font, fn);
	
    if (size == (float) 0.0)
	pdf_error(p, PDF_ValueError, "Bad font size 0 in %s", fn);

    result = pdf_str_width(p, text, 0, font, size);
    PDF_TRACE((" [%f]\n", result));
    return result;
}

PDFLIB_API float PDFLIB_CALL
PDF_stringwidth2(PDF *p, const char *text, int len, int font, float size)
{
    static const char fn[] = "PDF_stringwidth2";
    float result;

    PDF_TRACE(("%s\t(pdf[%p], \"%s\", %d, %d, %f);\n",
	fn, (void *) p, text, len, font, size));

    if (PDF_SANITY_CHECK_FAILED(p)) {
	PDF_TRACE((" [%f]\n", 0));
	return 0;
    }

    PDF_CHECK_SCOPE(p, fn,
	pdf_state_document | pdf_state_ppt | pdf_state_path);

    /* Check parameters */
    if (text == NULL || *text == '\0') {
	PDF_TRACE((" [%f]\n", 0));
	return 0;
    }

    if (font < 0 || font >= p->fonts_number)
	pdf_error(p, PDF_ValueError, "Bad font descriptor %d in %s", font, fn);
	
    if (size == (float) 0.0)
	pdf_error(p, PDF_ValueError, "Bad font size 0 in %s", fn);

    result = pdf_str_width(p, text, len, font, size);
    PDF_TRACE((" [%f]\n", result));
    return result;
}

/* ------------------------ Text control functions ------------------------ */

static void
pdf_underline(PDF *p, const char *text, int len, float x, float y)
{
    float length, delta_y, linewidth, xscale, yscale;
    pdf_tstate *ts;

    ts = &p->tstate[p->sl];

    /* We can't do underline for CID fonts */
    if (p->fonts[ts->f].encoding == cid)
	return;

    xscale = (float) fabs((double) ts->m.a);
    yscale = (float) fabs((double) ts->m.d);

    linewidth = ts->fs * p->fonts[ts->f].underlineThickness / 1000 *
		ts->h * (xscale > yscale ?  xscale : yscale);

    /* the font size may be negative, resulting in bad line width */
    linewidth = (float) fabs((double) linewidth);

    length = pdf_str_width(p, text, len, ts->f, ts->fs);

    if (length == (float) 0)
	return;

    delta_y = (ts->fs * p->fonts[ts->f].underlinePosition / 1000 + ts->rise) *
	    (float) fabs((double) ts->h) * (xscale > yscale ?  xscale : yscale);

    PDF_save(p);

    PDF_setlinewidth(p, linewidth);
    PDF_setlinecap(p, 0);
    PDF_setdash(p, 0, 0);
    PDF_moveto(p, x,          y + delta_y);
    PDF_lineto(p, x + length, y + delta_y);
    PDF_stroke(p);

    PDF_restore(p);
}

static void
pdf_overline(PDF *p, const char *text, int len, float x, float y)
{
    float length, delta_y, linewidth, xscale, yscale, lineheight;
    pdf_tstate *ts;

    ts = &p->tstate[p->sl];

    /* We can't do overline for CID fonts */
    if (p->fonts[ts->f].encoding == cid)
	return;

    xscale = (float) fabs((double) ts->m.a);
    yscale = (float) fabs((double) ts->m.d);

    linewidth = ts->fs * p->fonts[ts->f].underlineThickness / 1000 *
		ts->h * (xscale > yscale ?  xscale : yscale);

    /* the font size may be negative, resulting in bad line width */
    linewidth = (float) fabs((double) linewidth);

    lineheight = ((float) p->fonts[ts->f].ascender/1000) * ts->fs;

    length = pdf_str_width(p, text, len, ts->f, ts->fs);

    if (length == (float) 0)
	return;

    delta_y = (ts->fs * p->fonts[ts->f].underlinePosition / 1000 + ts->rise) *
	    (float) fabs((double) ts->h) * (xscale > yscale ?  xscale : yscale);

    PDF_save(p);

    PDF_setlinewidth(p, linewidth);
    PDF_setlinecap(p, 0);
    PDF_setdash(p, 0, 0);
    PDF_moveto(p, x,          y + lineheight - delta_y);
    PDF_lineto(p, x + length, y + lineheight - delta_y);
    PDF_stroke(p);

    PDF_restore(p);
}

static void
pdf_strikeout(PDF *p, const char *text, int len, float x, float y)
{
    float length, delta_y, linewidth, xscale, yscale, lineheight;
    pdf_tstate *ts;

    ts = &p->tstate[p->sl];

    /* We can't do strikeout for CID fonts */
    if (p->fonts[ts->f].encoding == cid)
	return;

    xscale = (float) fabs((double) ts->m.a);
    yscale = (float) fabs((double) ts->m.d);

    linewidth = ts->fs * p->fonts[ts->f].underlineThickness / 1000 *
		ts->h * (xscale > yscale ?  xscale : yscale);

    /* the font size may be negative, resulting in bad line width */
    linewidth = (float) fabs((double) linewidth);

    lineheight = ((float) p->fonts[ts->f].ascender/1000) * ts->fs;

    length = pdf_str_width(p, text, len, ts->f, ts->fs);

    if (length == (float) 0)
	return;

    delta_y = (ts->fs * p->fonts[ts->f].underlinePosition / 1000 + ts->rise) *
	    (float) fabs((double) ts->h) * (xscale > yscale ?  xscale : yscale);

    PDF_save(p);

    PDF_setlinewidth(p, linewidth);
    PDF_setlinecap(p, 0);
    PDF_setdash(p, 0, 0);
    PDF_moveto(p, x,          y + lineheight/2 + delta_y);
    PDF_lineto(p, x + length, y + lineheight/2 + delta_y);
    PDF_stroke(p);

    PDF_restore(p);
}

/* ------------------------ font operator ------------------------ */

PDFLIB_API void PDFLIB_CALL
PDF_setfont(PDF *p, int font, float fontsize)
{
    static const char fn[] = "PDF_setfont";

    PDF_TRACE(("%s\t(pdf[%p], %d, %f);\n", fn, (void *) p, font, fontsize));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    /* Check parameters */
    if (font < 0 || font >= p->fonts_number)
	pdf_error(p, PDF_ValueError, "Bad font descriptor %d", font);
	
    if (fontsize == (float) 0.0)
	pdf_error(p, PDF_ValueError, "Invalid font size 0 in PDF_setfont");

    /* make font the current font */

    p->fonts[font].used_on_current_page = pdf_true;
    p->tstate[p->sl].fs = fontsize;
    p->tstate[p->sl].f = font;

    pdf_begin_text(p, pdf_false);
    pdf_printf(p, "/F%d %f Tf\n", font, fontsize);
    pdf_set_leading(p, fontsize);
}

/* ------------------------ Text rendering operators ------------------------ */

#define PDF_RENDERMODE_FILLCLIP 4

PDFLIB_API void PDFLIB_CALL
PDF_show(PDF *p, const char *text)
{
    PDF_show2(p, text, 0);
}

PDFLIB_API void PDFLIB_CALL
PDF_show2(PDF *p, const char *text, int len)
{
    PDF_TRACE(("PDF_show2\t\t(pdf[%p], \"%s\", %d);\n", (void *) p, text, len));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, "PDF_show", pdf_state_ppt);

    if (p->tstate[p->sl].f == -1) {			/* no font set */
        pdf_error(p, PDF_RuntimeError,
                "Must set font before calling PDF_show");
    }

    if (text == NULL)
	return;

    if (len == 0) {
	len = (int) strlen(text);
	if (len == 0)
	    return;
    }

    if (p->underline)
	pdf_underline(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);
    if (p->overline)
	pdf_overline(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);
    if (p->strikeout)
	pdf_strikeout(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);


    pdf_begin_text(p, (p->contents != c_text));

    /* set leading, word, and character spacing if required */
    if (!p->textparams_done) {
	pdf_printf(p, "%f TL\n", p->tstate[p->sl].l);

	if (p->tstate[p->sl].w != (float) 0)
	    pdf_printf(p,"%f Tw\n", p->tstate[p->sl].w);

	if (p->tstate[p->sl].c != (float) 0)
	    pdf_printf(p,"%f Tc\n", p->tstate[p->sl].c);

	p->textparams_done = pdf_true;
    }

    pdf_quote_string2(p, text, len);
    pdf_puts(p, "Tj\n");

    if (p->tstate[p->sl].mode >= PDF_RENDERMODE_FILLCLIP)
	pdf_end_text(p);

    p->tstate[p->sl].m.e += 
    	pdf_str_width(p, text, len, p->tstate[p->sl].f, p->tstate[p->sl].fs);
}

PDFLIB_API void PDFLIB_CALL
PDF_show_xy(PDF *p, const char *text, float x, float y)
{
    PDF_show_xy2(p, text, 0, x, y);
}

PDFLIB_API void PDFLIB_CALL
PDF_show_xy2(PDF *p, const char *text, int len, float x, float y)
{
    PDF_TRACE(("PDF_show_xy2\t(pdf[%p], \"%s\", %d, %f, %f);\n",
	(void *) p, text, len, x, y));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, "PDF_show_xy", pdf_state_ppt);

    if (text == NULL) {
	p->tstate[p->sl].lm.e = x;
	p->tstate[p->sl].lm.f = y;

	p->tstate[p->sl].m.e = x;
	p->tstate[p->sl].m.f = y;
	return;
    }

    if (len == 0) {
	len = (int) strlen(text);
	if (len == 0)
	    return;
    }

    if (p->tstate[p->sl].f == -1) {			/* no font set */
        pdf_error(p, PDF_RuntimeError,
                "Must set font before calling PDF_show_xy");
    }

    if (p->underline)
	pdf_underline(p, text, len, x, y);
    if (p->overline)
	pdf_overline(p, text, len, x, y);
    if (p->strikeout)
	pdf_strikeout(p, text, len, x, y);

    p->tstate[p->sl].m.e = x;
    p->tstate[p->sl].m.f = y;

    p->tstate[p->sl].lm.e = x;
    p->tstate[p->sl].lm.f = y;

    pdf_begin_text(p, pdf_true);

    /* set leading, word, and character spacing if required */
    if (!p->textparams_done) {
	pdf_printf(p, "%f TL\n", p->tstate[p->sl].l);

	if (p->tstate[p->sl].w != (float) 0)
	    pdf_printf(p,"%f Tw\n", p->tstate[p->sl].w);

	if (p->tstate[p->sl].c != (float) 0)
	    pdf_printf(p,"%f Tc\n", p->tstate[p->sl].c);

	p->textparams_done = pdf_true;
    }

    pdf_quote_string2(p, text, len);
    pdf_puts(p, "Tj\n");

    if (p->tstate[p->sl].mode >= PDF_RENDERMODE_FILLCLIP)
	pdf_end_text(p);

    p->tstate[p->sl].m.e +=
    	pdf_str_width(p, text, len, p->tstate[p->sl].f, p->tstate[p->sl].fs);
}

PDFLIB_API void PDFLIB_CALL
PDF_continue_text(PDF *p, const char *text)
{
    PDF_continue_text2(p, text, 0);
}

PDFLIB_API void PDFLIB_CALL
PDF_continue_text2(PDF *p, const char *text, int len)
{
    PDF_TRACE(("PDF_continue_text2(pdf[%p], \"%s\", %d);\n",
	(void *) p, text, len));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, "PDF_continue_text", pdf_state_ppt);

    p->tstate[p->sl].m.e  = p->tstate[p->sl].lm.e;
    p->tstate[p->sl].m.f -= p->tstate[p->sl].l;

    if (text == NULL)
	text = "";

    if (len == 0)
	len = (int) strlen(text);

    if (p->tstate[p->sl].f == -1) {			/* no font set */
        pdf_error(p, PDF_RuntimeError,
                "Must set font before calling PDF_continue_text");
    }

    if (p->underline)
	pdf_underline(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);
    if (p->overline)
	pdf_overline(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);
    if (p->strikeout)
	pdf_strikeout(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);

    if (p->contents != c_text) {
	pdf_begin_text(p, pdf_true);

	if (!p->textparams_done) {
	    pdf_printf(p, "%f TL\n", p->tstate[p->sl].l);

	    if (p->tstate[p->sl].w != (float) 0)
		pdf_printf(p,"%f Tw\n", p->tstate[p->sl].w);

	    if (p->tstate[p->sl].c != (float) 0)
		pdf_printf(p,"%f Tc\n", p->tstate[p->sl].c);

	    p->textparams_done = pdf_true;
	}

	pdf_quote_string2(p, text, len);
	pdf_puts(p, "Tj\n");

    } else {
	if (!p->textparams_done) {
	    pdf_printf(p, "%f TL\n", p->tstate[p->sl].l);

	    if (p->tstate[p->sl].w != (float) 0)
		pdf_printf(p, "%f Tw\n", p->tstate[p->sl].w);

	    if (p->tstate[p->sl].c != (float) 0)
		pdf_printf(p, "%f Tc\n", p->tstate[p->sl].c);

	    p->textparams_done = pdf_true;
	}

	pdf_quote_string2(p, text, len);
	pdf_puts(p, "'\n");
    }

    if (p->tstate[p->sl].mode >= PDF_RENDERMODE_FILLCLIP)
	pdf_end_text(p);

    p->tstate[p->sl].m.e +=
    	pdf_str_width(p, text, len, p->tstate[p->sl].f, p->tstate[p->sl].fs);
}

/* ----------------------- Text formatting routines ------------------------ */

/* text alignment modes */
typedef enum { pdf_align_left, pdf_align_right, pdf_align_center,
	pdf_align_justify, pdf_align_fulljustify
} pdf_alignment;

/* this helper function returns the width of the null-terminated string
** 'text' for the current font and size EXCLUDING the last character's
** additional charspacing.
*/
static float
pdf_swidth(PDF *p, const char *text)
{
    pdf_tstate	*ts = &p->tstate[p->sl];

    return pdf_str_width(p, text, 0, ts->f, ts->fs) - ts->c * ts->m.a * ts->h;
}

static void
pdf_show_aligned(PDF *p, const char *text, float x, float y, pdf_alignment mode)
{
    switch (mode) {
	case pdf_align_left:
	case pdf_align_justify:
	case pdf_align_fulljustify:
	    /* nothing extra here... */
	    break;

	case pdf_align_right:
	    x -= pdf_swidth(p, text);
	    break;

	case pdf_align_center:
	    x -= pdf_swidth(p, text) / 2;
	    break;
    }

    PDF_show_xy(p, text, x, y);
}

PDFLIB_API int PDFLIB_CALL
PDF_show_boxed(
    PDF *p,
    const char *text,
    float left,
    float bottom,
    float width,
    float height,
    const char *hmode,
    const char *feature)
{
    static const char fn[] = "PDF_show_boxed";

    float	textwidth, old_word_spacing, curx, cury;
    pdf_bool	prematureexit;	/* return because box is too small */
    int		curTextPos;	/* character currently processed */
    int		totallen;	/* total length of input string */
    int		lastdone;	/* last input character processed */
    pdf_alignment mode = pdf_align_left;
    pdf_bool	blind = pdf_false;
    pdf_tstate	*ts;

    PDF_TRACE(("%s\t(pdf[%p], \"%s\", %f, %f, %f, %f, \"%s\", \"%s\");\n",
    	fn, (void *) p, text, left, bottom, width, height, hmode, feature));

    if (PDF_SANITY_CHECK_FAILED(p))
	return 0;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (text == NULL || *text == '\0')
	return 0;

    if (hmode == NULL || *hmode == '\0')
	pdf_error(p, PDF_ValueError,
	    "Missing alignment mode in PDF_show_boxed");

    if (!strcmp(hmode, "left"))
	mode = pdf_align_left;
    else if (!strcmp(hmode, "right"))
	mode = pdf_align_right;
    else if (!strcmp(hmode, "center"))
	mode = pdf_align_center;
    else if (!strcmp(hmode, "justify"))
	mode = pdf_align_justify;
    else if (!strcmp(hmode, "fulljustify"))
	mode = pdf_align_fulljustify;
    else
	pdf_error(p, PDF_ValueError,
	    "Bad alignment mode '%s' in PDF_show_boxed", hmode);

    if (feature != NULL && *feature != '\0') {
	if (!strcmp(feature, "blind"))
	    blind = pdf_true;
	else
	    pdf_error(p, PDF_ValueError,
		"Unknown feature parameter '%s' in PDF_show_boxed", feature);
    }

    ts = &p->tstate[p->sl];

    if (ts->f == -1) {			/* no font set */
        pdf_error(p, PDF_RuntimeError,
                "Must set font before calling PDF_show_boxed");
    }

    /* we cannot handle CID fonts nor EBCDIC encoding */
    if ((width != 0 || height != 0) &&
	(p->fonts[ts->f].encoding == cid || p->fonts[ts->f].encoding == ebcdic))
    {
        pdf_error(p, PDF_RuntimeError,
                "PDF_show_boxed not supported for CID or EBCDIC encoding");
    }

    /* special case for a single aligned line */
    if (width == 0 && height == 0) {
	if (!blind)
	    pdf_show_aligned(p, text, left, bottom, mode);
	return 0;
    }

    if (width == 0 || height == 0)
	pdf_error(p, PDF_ValueError, "Bad size of text box in PDF_show_boxed");

    old_word_spacing = ts->w;
    totallen = (int) strlen(text);

    curx = left;
    cury = bottom + height;
    prematureexit = pdf_false;	
    curTextPos = 0;
    lastdone = 0;

    /* switch curx for right and center justification */
    if (mode == pdf_align_right)
	curx += width;
    else if (mode == pdf_align_center)
	curx += (width / 2);

#define	MAX_CHARS_IN_LINE	1024

    /* loop until all characters processed, or box full */

    while ((curTextPos < totallen) && !prematureexit) {
	/* buffer for constructing the line */
	char	linebuf[MAX_CHARS_IN_LINE];
	int	curCharsInLine = 0;	/* # of chars in constructed line */
	int	lastWordBreak = 0;	/* the last seen space char */
	int	wordBreakCount = 0;	/* # of blanks in this line */
	
	/* loop over the input string */
	while (curTextPos < totallen) {
	    if (curCharsInLine >= MAX_CHARS_IN_LINE) {
		pdf_error(p, PDF_SystemError,
		    "Line too long in PDF_show_boxed()");
	    }

	    /* abandon DOS line-ends */
	    if (text[curTextPos] == PDF_RETURN &&
		text[curTextPos+1] == PDF_NEWLINE)
		    curTextPos++;

	    /* if it's a forced line break draw the line */
	    if (text[curTextPos] == PDF_NEWLINE ||
		text[curTextPos] == PDF_RETURN) {

		cury -= ts->l;			/* adjust cury by leading */

		if (cury < bottom) {
		    prematureexit = pdf_true;	/* box full */
		    break;
		}

		linebuf[curCharsInLine] = 0;	/* terminate the line */
		
		/* check whether the line is too long */
		ts->w = (float) 0.0;
		textwidth = pdf_swidth(p, linebuf);

		/* the forced break occurs too late for this line */
		if (textwidth > width) {
		    if (wordBreakCount == 0) {	/* no blank found */
			prematureexit = pdf_true;
			break;
		    }
		    linebuf[lastWordBreak] = 0;	/* terminate at last blank */
		    if (curTextPos > 0 && text[curTextPos-1] == PDF_RETURN)
			--curTextPos;
		    curTextPos -= (curCharsInLine - lastWordBreak);

		    if (!blind) {
			textwidth = pdf_swidth(p, linebuf);
			if (wordBreakCount != 1 &&
				(mode == pdf_align_justify ||
				mode == pdf_align_fulljustify)) {
			    pdf_set_word_spacing(p,
				(width - textwidth) /
				((wordBreakCount - 1) * ts->h * ts->m.a));
			}
			else
			    pdf_set_word_spacing(p, (float) 0.0);
			pdf_show_aligned(p, linebuf, curx, cury, mode);
		    }

		} else if (!blind) {

		    if (mode == pdf_align_fulljustify && wordBreakCount > 0) {
			pdf_set_word_spacing(p,
			    (width - textwidth) /
			    (wordBreakCount * ts->h * ts->m.a));
		    } else {
			pdf_set_word_spacing(p, (float) 0.0);
		    }

		    pdf_show_aligned(p, linebuf, curx, cury, mode);
		}

		lastdone = curTextPos;
		curCharsInLine = lastWordBreak = wordBreakCount = 0;
		curTextPos++;

	    } else if (text[curTextPos] == PDF_SPACE) {
		linebuf[curCharsInLine] = 0;	/* terminate the line */
		ts->w = (float) 0.0;

		/* line too long ==> break at last blank */
		if (pdf_swidth(p, linebuf) > width) {
		    cury -= ts->l;		/* adjust cury by leading */

		    if (cury < bottom) {
			prematureexit = pdf_true; 	/* box full */
			break;
		    }

		    linebuf[lastWordBreak] = 0;	/* terminate at last blank */
		    curTextPos -= (curCharsInLine - lastWordBreak - 1);

		    if (lastWordBreak == 0)
			curTextPos--;
		    
		    /* LATER: * force break if wordBreakCount == 1,
		     * i.e., no blank
		     */
		    if (wordBreakCount == 0) {
			prematureexit = pdf_true;
			break;
		    }

		    /* adjust word spacing for full justify */
		    if (wordBreakCount != 1 && (mode == pdf_align_justify ||
					    mode == pdf_align_fulljustify)) {
			ts->w = (float) 0.0;
			textwidth = pdf_swidth(p, linebuf);
			if (!blind) {
			    pdf_set_word_spacing(p,
				(width - textwidth) /
				((wordBreakCount - 1) * ts->h * ts->m.a));
			}
		    }

		    lastdone = curTextPos;
		    if (!blind)
			pdf_show_aligned(p, linebuf, curx, cury, mode);
		    curCharsInLine = lastWordBreak = wordBreakCount = 0;

		} else {
		    /* blank found, and line still fits */
		    wordBreakCount++;
		    lastWordBreak = curCharsInLine;
		    linebuf[curCharsInLine++] = text[curTextPos++];
		}

	    } else {
		/* regular character ==> store in buffer */
		linebuf[curCharsInLine++] = text[curTextPos++];
	    }
	}

	if (prematureexit) {
	    break;		/* box full */
	}
	
	/* if there is anything left in the buffer, draw it */
	if (curTextPos >= totallen && curCharsInLine != 0) {
	    cury -= ts->l;		/* adjust cury for line height */

	    if (cury < bottom) {
		prematureexit = pdf_true; 	/* box full */
		break;
	    }

	    linebuf[curCharsInLine] = 0;	/* terminate the line */
	    
	    /* check if the last line is too long */
	    ts->w = (float) 0.0;
	    textwidth = pdf_swidth(p, linebuf);

	    if (textwidth > width) {
		if (wordBreakCount == 0)
		{
		    prematureexit = pdf_true;
		    break;
		}

		linebuf[lastWordBreak] = 0;	/* terminate at last blank */
		curTextPos -= (curCharsInLine - lastWordBreak - 1);

		/* recalculate the width */
		textwidth = pdf_swidth(p, linebuf);

		/* adjust word spacing for full justify */
		if (wordBreakCount != 1 && (mode == pdf_align_justify ||
					    mode == pdf_align_fulljustify)) {
		    if (!blind) {
			pdf_set_word_spacing(p,
			(width - textwidth) /
			    ((wordBreakCount - 1) * ts->h * ts->m.a));
		    }
		}

	    } else if (!blind) {

		if (mode == pdf_align_fulljustify && wordBreakCount) {
		    pdf_set_word_spacing(p,
			    (width - textwidth) /
			    (wordBreakCount * ts->h * ts->m.a));
		} else {
		    pdf_set_word_spacing(p, (float) 0.0);
		}
	    }

	    lastdone = curTextPos;
	    if (!blind)
		pdf_show_aligned(p, linebuf, curx, cury, mode);
	    curCharsInLine = lastWordBreak = wordBreakCount = 0;
	}
    }

    if (!blind)
	pdf_set_word_spacing(p, old_word_spacing);

    /* return number of remaining characters */

    while (text[lastdone] == PDF_SPACE)
	++lastdone;

    if ((text[lastdone] == PDF_RETURN ||
	text[lastdone] == PDF_NEWLINE) && text[lastdone+1] == 0)
	    ++lastdone;

    return (int) (totallen - lastdone);
}
