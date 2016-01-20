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

/* $Id: p_color.c,v 1.29.2.5 2002/01/07 18:26:29 tm Exp $
 *
 * PDFlib color routines
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "p_intern.h"

/* This must be kept in sync with the pdf_colorspace enum in p_intern.h. */
const char *pdf_colorspace_names[] = {
    "DeviceGray", "DeviceRGB", "DeviceCMYK", "CalGray", "CalRGB", "Lab", 
    "Indexed", "Pattern", "Separation", "Invalid_CS"
};

void
pdf_init_cstate(PDF *p)
{
    pdf_cstate *cs;

    cs = &p->cstate[p->sl];

    cs->fill.cs		= DeviceGray;
    cs->fill.val.gray	= (float) 0.0;

    cs->stroke.cs	= DeviceGray;
    cs->stroke.val.gray	= (float) 0.0;
}

/* Color operators */

/* Avoid wrong error messages due to rounding artifacts.
 * This doesn't do any harm since we truncate to 5 decimal places anyway
 * when producing PDF output.
 */
#define EPSILON	((float) (1.0 + PDF_SMALLREAL))

/* Grayscale functions */

static void
pdf_setgraycolor(PDF *p, const char *type, float g)
{
    pdf_cstate *cs = &p->cstate[p->sl];

    if (g < 0.0 || g > EPSILON ) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus gray value (%f) in PDF_setcolor", g);
	return;
    }

    if (!strcmp(type, "fill") || !strcmp(type, "both")) {
	if (cs->fill.cs != DeviceGray || cs->fill.val.gray != g)
	{
	    if (PDF_GET_STATE(p) != pdf_state_document)
		pdf_printf(p, "%f g\n", g);

	    cs->fill.cs		= DeviceGray;
	    cs->fill.val.gray	= g;
	}
    }

    if (!strcmp(type, "stroke") || !strcmp(type, "both")) {
	if (cs->stroke.cs != DeviceGray || cs->stroke.val.gray != g)
	{
	    if (PDF_GET_STATE(p) != pdf_state_document)
		pdf_printf(p, "%f G\n", g);

	    cs->stroke.cs	= DeviceGray;
	    cs->stroke.val.gray	= g;
	}
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray_fill(PDF *p, float g)
{
    static const char fn[] = "PDF_setgray_fill";

    PDF_TRACE(("%s\t(pdf[%p], %f);\n", fn, (void *) p, g));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    pdf_setgraycolor(p, "fill", g);
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray_stroke(PDF *p, float g)
{
    static const char fn[] = "PDF_setgray_stroke";

    PDF_TRACE(("%s\t(pdf[%p], %f);\n", fn, (void *) p, g));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    pdf_setgraycolor(p, "stroke", g);
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray(PDF *p, float g)
{
    static const char fn[] = "PDF_setgray";

    PDF_TRACE(("%s\t(pdf[%p], %f);\n", fn, (void *) p, g));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    pdf_setgraycolor(p, "both", g);
}

/* RGB functions */

static void
pdf_setrgbcolor2(PDF *p, const char *type, float red, float green, float blue)
{
    pdf_cstate *cs = &p->cstate[p->sl];

    if (red < 0.0 || red > EPSILON || green < 0.0 || green > EPSILON ||
	blue < 0.0 || blue > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus RGB color value (%f/%f/%f) in PDF_setcolor",
	    red, green, blue);
	return;
    }

    if (red == green && green == blue) {
	pdf_setgraycolor(p, type, red);
	return;
    }

    if (!strcmp(type, "fill") || !strcmp(type, "both")) {
	if (cs->fill.cs != DeviceRGB || cs->fill.val.rgb.r != red ||
	    cs->fill.val.rgb.g != green || cs->fill.val.rgb.b != blue)
	{
	    if (PDF_GET_STATE(p) != pdf_state_document)
		pdf_printf(p, "%f %f %f rg\n", red, green, blue);

	    cs->fill.cs		= DeviceRGB;
	    cs->fill.val.rgb.r	= red;
	    cs->fill.val.rgb.g	= green;
	    cs->fill.val.rgb.b	= blue;
	}
    }

    if (!strcmp(type, "stroke") || !strcmp(type, "both")) {
	if (cs->stroke.cs != DeviceRGB || cs->stroke.val.rgb.r != red ||
	    cs->stroke.val.rgb.g != green || cs->stroke.val.rgb.b != blue)
	{
	    if (PDF_GET_STATE(p) != pdf_state_document)
		pdf_printf(p, "%f %f %f RG\n", red, green, blue);

	    cs->stroke.cs		= DeviceRGB;
	    cs->stroke.val.rgb.r	= red;
	    cs->stroke.val.rgb.g	= green;
	    cs->stroke.val.rgb.b	= blue;
	}
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_fill(PDF *p, float red, float green, float blue)
{
    static const char fn[] = "PDF_setrgbcolor_fill";

    PDF_TRACE(( "%s\t(pdf[%p], %f, %f, %f);\n",
	fn, (void *) p, red, green, blue));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    pdf_setrgbcolor2(p, "fill", red, green, blue);
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_stroke(PDF *p, float red, float green, float blue)
{
    static const char fn[] = "PDF_setrgbcolor_stroke";

    PDF_TRACE(( "%s\t(pdf[%p], %f, %f, %f);\n",
	fn, (void *) p, red, green, blue));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    pdf_setrgbcolor2(p, "stroke", red, green, blue);
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor(PDF *p, float red, float green, float blue)
{
    static const char fn[] = "PDF_setrgbcolor";

    PDF_TRACE(("%s\t(pdf[%p], %f, %f, %f);\n",
	fn, (void *) p, red, green, blue));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    pdf_setrgbcolor2(p, "both", red, green, blue);
}

/* CMYK functions */

static void
pdf_setcmykcolor(
    PDF *p,
    const char *type,
    float cyan,
    float magenta,
    float yellow,
    float black)
{
    pdf_cstate *cs = &p->cstate[p->sl];

    if (cyan < 0.0 || cyan > EPSILON || magenta < 0.0 || magenta > EPSILON ||
	yellow < 0.0 || yellow > EPSILON || black < 0.0 || black > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus CMYK color value (%f/%f/%f/%f) in PDF_setcolor",
	    cyan, magenta, yellow, black);
	return;
    }

    if (!strcmp(type, "fill") || !strcmp(type, "both")) {
	if (cs->fill.cs != DeviceCMYK ||
	    cs->fill.val.cmyk.c != cyan || cs->fill.val.cmyk.m != magenta ||
	    cs->fill.val.cmyk.y != yellow || cs->fill.val.cmyk.k != black)
	{
	    if (PDF_GET_STATE(p) != pdf_state_document)
		pdf_printf(p, "%f %f %f %f k\n", cyan, magenta, yellow, black);

	    cs->fill.cs		= DeviceCMYK;
	    cs->fill.val.cmyk.c	= cyan;
	    cs->fill.val.cmyk.m	= magenta;
	    cs->fill.val.cmyk.y	= yellow;
	    cs->fill.val.cmyk.k	= black;
	}
    }

    if (!strcmp(type, "stroke") || !strcmp(type, "both")) {
	if (cs->stroke.cs != DeviceCMYK ||
	    cs->stroke.val.cmyk.c != cyan || cs->stroke.val.cmyk.m != magenta ||
	    cs->stroke.val.cmyk.y != yellow || cs->stroke.val.cmyk.k != black)
	{
	    if (PDF_GET_STATE(p) != pdf_state_document)
		pdf_printf(p, "%f %f %f %f K\n", cyan, magenta, yellow, black);

	    cs->stroke.cs		= DeviceCMYK;
	    cs->stroke.val.cmyk.c	= cyan;
	    cs->stroke.val.cmyk.m	= magenta;
	    cs->stroke.val.cmyk.y	= yellow;
	    cs->stroke.val.cmyk.k	= black;
	}
    }
}

/* Spot color functions */

void
pdf_init_colorspaces(PDF *p)
{
    int i;

    p->colorspaces_number = 0;
    p->colorspaces_capacity = COLORSPACES_CHUNKSIZE;

    p->colorspaces = (pdf_csresource *) 
	p->malloc(p, sizeof(pdf_csresource) * p->colorspaces_capacity,
	    "pdf_init_colorspaces");

    for (i = 0; i < p->colorspaces_capacity; i++) {
	p->colorspaces[i].used_on_current_page = pdf_false;
	p->colorspaces[i].name = NULL;
    }
}

void
pdf_grow_colorspaces(PDF *p)
{
    int i;

    p->colorspaces = (pdf_csresource *) p->realloc(p, p->colorspaces,
	sizeof(pdf_csresource) * 2 * p->colorspaces_capacity,
	"pdf_grow_colorspaces");

    for (i = p->colorspaces_capacity; i < 2 * p->colorspaces_capacity; i++) {
	p->colorspaces[i].used_on_current_page = pdf_false;
	p->colorspaces[i].name = NULL;
    }

    p->colorspaces_capacity *= 2;
}

void
pdf_write_page_colorspaces(PDF *p)
{
    int i, total = 0;

    for (i = 0; i < p->colorspaces_number; i++)
	if (p->colorspaces[i].used_on_current_page)
	    total++;

    if (total > 0) {
	pdf_puts(p, "/ColorSpace");

	pdf_begin_dict(p);			/* color space names */

	for (i = 0; i < p->colorspaces_number; i++) {
	    if (p->colorspaces[i].used_on_current_page) {
		p->colorspaces[i].used_on_current_page = pdf_false; /* reset */
		pdf_printf(p, "/CS%d %ld 0 R\n", i, p->colorspaces[i].obj_id);
	    }
	}

	pdf_end_dict(p);			/* color space names */
    }
}

void
pdf_write_doc_colorspaces(PDF *p)
{
    int i;

    pdf_csresource *cs;

    for (i = 0; i < p->colorspaces_number; i++) {
	cs = &p->colorspaces[i];

	pdf_begin_obj(p, cs->obj_id);		/* color space resource */

	/* Separation color space, or pattern based on separation */
	if (cs->cs == Separation ||
	   (cs->cs == PatternCS && cs->alternate.cs == Separation)) {
	    const char *name;

	    if (cs->cs == PatternCS) {
		pdf_printf(p, "[/Pattern");	/* pattern color space */
		name = p->colorspaces[cs->alternate.val.sep.cs].name;
	    } else
		name = cs->name;

	    pdf_puts(p, "[/Separation/");
	    pdf_puts(p, name);		/* this is already quoted */
	    pdf_puts(p, "\n");

	    if (cs->alternate.cs == DeviceGray) {
		pdf_puts(p, "/DeviceGray<<\n");
		pdf_puts(p, "/Range[0 1]\n");
		pdf_printf(p, "/C0[0]\n");
		pdf_printf(p, "/C1[%f]\n", cs->alternate.val.gray);

	    } else if (cs->alternate.cs == DeviceRGB) {
		pdf_puts(p, "/DeviceRGB<<\n");
		pdf_puts(p, "/Range[0 1 0 1 0 1]\n");
		pdf_printf(p, "/C0[0 0 0]\n");
		pdf_printf(p, "/C1[%f %f %f]\n", cs->alternate.val.rgb.r,
			cs->alternate.val.rgb.g, cs->alternate.val.rgb.b);
		    
	    } else if (cs->alternate.cs == DeviceCMYK) {
		pdf_puts(p, "/DeviceCMYK<<\n");
		pdf_puts(p, "/Range[0 1 0 1 0 1 0 1]\n");
		pdf_printf(p, "/C0[0 0 0 0]\n");
		pdf_printf(p, "/C1[%f %f %f %f]\n",
			cs->alternate.val.cmyk.c, cs->alternate.val.cmyk.m,
			cs->alternate.val.cmyk.y, cs->alternate.val.cmyk.k);

	    } else if (cs->alternate.cs != Separation)
		pdf_error(p, PDF_SystemError,
		"Unknown alternate color space in pdf_write_doc_colorspaces");

	    pdf_puts(p, "/FunctionType 2\n");
	    pdf_puts(p, "/Domain[0 1]\n");
	    pdf_puts(p, "/N 1\n");

	    pdf_end_dict(p);		/* alternate color space */
	    pdf_puts(p, "]\n");		/* separation color space */

	    if (cs->cs == PatternCS)
		pdf_puts(p, "]\n");	/* pattern color space */

	/* Pattern color space based on simple color space */
	} else if (cs->cs == PatternCS) {
	    pdf_printf(p, "[/Pattern/%s]\n",
		pdf_colorspace_names[cs->alternate.cs]);

	} else {
	    pdf_error(p, PDF_SystemError,
	"Unknown base or alternate color space in pdf_write_doc_colorspaces");
	}

	pdf_end_obj(p);			/* color space resource */
    }
}

void
pdf_cleanup_colorspaces(PDF *p)
{
    int i;

    if (!p->colorspaces)
	return;

    for (i = 0; i < p->colorspaces_number; i++) {
	if (p->colorspaces[i].name)
	    p->free(p, p->colorspaces[i].name);
    }

    p->free(p, p->colorspaces);
    p->colorspaces = NULL;
}

#define PDF_MAX_SPOTNAME	126	/* maximum length of spot color name */

int
pdf_add_colorspace(PDF *p, pdf_colorspace cs, const char *spotname)
{
    int	slot;

    /* TODO: optimize and check for known color space combinations
    if (cs == Separation)
    for (slot = 0; slot < p->colorspaces_number; slot++) {
	if (!strcmp(p->colorspaces[slot].name, spotname))
	    return slot;
    }
    */

    slot = p->colorspaces_number;

    if (p->colorspaces_number >= p->colorspaces_capacity)
	pdf_grow_colorspaces(p);

    if (cs == Separation) {
	p->colorspaces[slot].obj_id = pdf_alloc_id(p);
	p->colorspaces[slot].name = pdf_strdup(p, spotname);
	p->colorspaces[slot].cs = Separation;

	p->colorspaces[slot].alternate = p->cstate[p->sl].fill;

    } else if (cs == PatternCS) {
	p->colorspaces[slot].obj_id = pdf_alloc_id(p);
	p->colorspaces[slot].name = (char *) NULL;
	p->colorspaces[slot].cs = PatternCS;

	p->colorspaces[slot].alternate = p->cstate[p->sl].fill;

    } else {
	pdf_error(p, PDF_SystemError,
	    "Unknown color space in pdf_add_colorspace");
    }

    p->colorspaces[p->colorspaces_number].used_on_current_page = pdf_true;

    p->colorspaces_number++;

    return slot;
}

PDFLIB_API int PDFLIB_CALL
PDF_makespotcolor(PDF *p, const char *spotname, int len)
{
    static const char fn[] = "PDF_makespotcolor";
    char *quotedspotname;
    int ret;

    PDF_TRACE(("%s\t(pdf[%p], \"%s\");\n", fn, (void *) p, spotname));

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt | pdf_state_document);

    if (spotname == NULL || *spotname == 0)
	pdf_error(p, PDF_ValueError, "Empty spot color name");

    if (p->cstate[p->sl].fill.cs == PatternCS ||
	p->cstate[p->sl].fill.cs == Indexed ||
	p->cstate[p->sl].fill.cs == Separation)
	pdf_error(p, PDF_RuntimeError,
	    "Spot colors can only be based on simple color spaces");

    if (len == 0)
	len = (int) strlen(spotname);

    if (len > PDF_MAX_SPOTNAME)
	pdf_error(p, PDF_ValueError, "Spot color name too long");

    quotedspotname = (char *) p->malloc(p, (size_t) (3*len + 1),
			"PDF_makespotcolor");

    pdf_make_quoted_name(p, spotname, (size_t) len, quotedspotname);

    ret = pdf_add_colorspace(p, Separation, quotedspotname);

    p->free(p, quotedspotname);

    return ret;
}

static void
pdf_setspotcolor(PDF *p, const char *type, int spotcolor, float tint)
{
    pdf_cstate *cs = &p->cstate[p->sl];

    if (spotcolor < 0 || spotcolor >= p->colorspaces_number)
	pdf_error(p, PDF_ValueError,
	    "Invalid spot color number %d in PDF_setcolor", spotcolor);

    if (tint < 0.0 || tint > EPSILON )
	pdf_error(p, PDF_ValueError,
	    "Bogus spot color tint value (%f) in PDF_setcolor", tint);

    if (!strcmp(type, "fill") || !strcmp(type, "both")) {
	if (cs->fill.cs != Separation ||
	    cs->fill.val.sep.cs != spotcolor || cs->fill.val.sep.tint != tint)
	{
	    if (PDF_GET_STATE(p) != pdf_state_document)
		pdf_printf(p, "/CS%d cs %f scn\n", spotcolor, tint);

	    p->cstate[p->sl].fill.cs = Separation;
	    p->cstate[p->sl].fill.val.sep.cs = spotcolor;
	    p->cstate[p->sl].fill.val.sep.tint = tint;
	}
    }

    if (!strcmp(type, "stroke") || !strcmp(type, "both")) {
	if (cs->fill.cs != Separation ||
	    cs->fill.val.sep.cs != spotcolor || cs->fill.val.sep.tint != tint)
	{
	    if (PDF_GET_STATE(p) != pdf_state_document)
		pdf_printf(p, "/CS%d CS %f SCN\n", spotcolor, tint);

	    p->cstate[p->sl].stroke.cs = Separation;
	    p->cstate[p->sl].stroke.val.sep.cs = spotcolor;
	    p->cstate[p->sl].stroke.val.sep.tint = tint;
	}
    }

    p->colorspaces[spotcolor].used_on_current_page = pdf_true;
}

PDFLIB_API void PDFLIB_CALL
PDF_setcolor(
    PDF *p,
    const char *fstype,
    const char *colorspace,
    float c1, float c2, float c3, float c4)
{
    static const char fn[] = "PDF_setcolor";

    PDF_TRACE(("%s\t(pdf[%p], \"%s\", \"%s\", %f, %f, %f, %f);\n",
    	fn, (void *) p, fstype, colorspace, c1, c2, c3, c4));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt | pdf_state_document);

    if (!fstype || !*fstype)
	pdf_error(p, PDF_ValueError, "Missing type in PDF_setcolor");

    if (strcmp(fstype, "fill") && strcmp(fstype, "stroke") &&
	strcmp(fstype, "both"))
	pdf_error(p, PDF_ValueError, "Unknown type in PDF_setcolor");

    if (!colorspace || !*colorspace)
	pdf_error(p, PDF_ValueError, "Missing color space in PDF_setcolor");

    if (!strcmp(colorspace, "gray"))
	pdf_setgraycolor(p, fstype, c1);

    else if (!strcmp(colorspace, "rgb"))
	pdf_setrgbcolor2(p, fstype, c1, c2, c3);

    else if (!strcmp(colorspace, "cmyk"))
	pdf_setcmykcolor(p, fstype, c1, c2, c3, c4);

    else if (!strcmp(colorspace, "spot"))
	pdf_setspotcolor(p, fstype, (int) c1, c2);

    else if (!strcmp(colorspace, "pattern"))
	pdf_setpatterncolor(p, fstype, (int) c1);

    else
	pdf_error(p, PDF_ValueError,
	    "Unknown color space '%s' in PDF_setcolor", colorspace);
}
