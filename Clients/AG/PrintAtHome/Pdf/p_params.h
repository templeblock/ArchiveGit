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

/* $Id: p_params.h,v 1.5.2.7 2002/01/07 18:26:29 tm Exp $
 *
 * PDFlib parameter table
 *
 */

#if	pdf_gen_parm_enum
#define pdf_gen1(code, name, zero, scope)		PDF_PARAMETER_##code,
#define pdf_gen2(code, name, zero, gscope, sscope)	PDF_PARAMETER_##code,
#elif	pdf_gen_parm_descr
#define pdf_gen1(code, name, zero, scope)		\
	{ name, zero, scope, scope },
#define pdf_gen2(code, name, zero, gscope, sscope)	\
	{ name, zero, gscope, sscope },
#else
#error	invalid inclusion of generator file
#endif

/* 
   List of unsupported debug parameters:
   a    ASCIIHex output for fonts and images
   c    trace calloc calls
   f    trace free calls
   F    throw an exception if a font is unavailable (instead of returning -1)
   h    disable host font processing
   i    throw an exception when an image is unavailable
   m    trace malloc calls
   P    disable pass-through mode for TIFF images
   p    throw an exception if an imported PDF is unavailable
   r    trace realloc calls
   s    print document statistics
   t	trace API calls
   u    unbuffered output
   w    throw an exception for non-fatal errors (=warnings); on by default
*/

/* setup */
pdf_gen1(COMPRESS,	"compress",	1, pdf_state_all)
pdf_gen1(FLUSH,		"flush",	1, pdf_state_all)
pdf_gen1(RESOURCEFILE,	"resourcefile",	1, pdf_state_all)
pdf_gen1(DEBUG,		"debug",	1, pdf_state_all)
pdf_gen1(NODEBUG,	"nodebug",	1, pdf_state_all)
pdf_gen1(COMPATIBILITY,	"compatibility",1, pdf_state_object)
pdf_gen1(PREFIX,	"prefix",	1, pdf_state_all)
pdf_gen1(WARNING,	"warning",	1, pdf_state_all)
pdf_gen1(BINDING,	"binding",	1, pdf_state_all)
pdf_gen1(INERROR,	"inerror",	1, pdf_state_all)
pdf_gen1(FLOATDIGITS,	"floatdigits",	1, pdf_state_all)
pdf_gen1(SERIAL,	"serial",	1, pdf_state_all)

/* version control */
pdf_gen1(MAJOR,		"major",	1, pdf_state_all)
pdf_gen1(MINOR,		"minor",	1, pdf_state_all)
pdf_gen1(REVISION,	"revision",	1, pdf_state_all)
pdf_gen1(VERSION,	"version",	1, pdf_state_all)

/* page */
pdf_gen1(PAGEWIDTH,	"pagewidth",	1, pdf_state_page)
pdf_gen1(PAGEHEIGHT,	"pageheight",	1, pdf_state_page)
pdf_gen1(CROPBOX_LLX,	"CropBox/llx",	1, pdf_state_page)
pdf_gen1(CROPBOX_LLY,	"CropBox/lly",	1, pdf_state_page)
pdf_gen1(CROPBOX_URX,	"CropBox/urx",	1, pdf_state_page)
pdf_gen1(CROPBOX_URY,	"CropBox/ury",	1, pdf_state_page)
pdf_gen1(BLEEDBOX_LLX,	"BleedBox/llx",	1, pdf_state_page)
pdf_gen1(BLEEDBOX_LLY,	"BleedBox/lly",	1, pdf_state_page)
pdf_gen1(BLEEDBOX_URX,	"BleedBox/urx",	1, pdf_state_page)
pdf_gen1(BLEEDBOX_URY,	"BleedBox/ury",	1, pdf_state_page)
pdf_gen1(TRIMBOX_LLX,	"TrimBox/llx",	1, pdf_state_page)
pdf_gen1(TRIMBOX_LLY,	"TrimBox/lly",	1, pdf_state_page)
pdf_gen1(TRIMBOX_URX,	"TrimBox/urx",	1, pdf_state_page)
pdf_gen1(TRIMBOX_URY,	"TrimBox/ury",	1, pdf_state_page)
pdf_gen1(ARTBOX_LLX,	"ArtBox/llx",	1, pdf_state_page)
pdf_gen1(ARTBOX_LLY,	"ArtBox/lly",	1, pdf_state_page)
pdf_gen1(ARTBOX_URX,	"ArtBox/urx",	1, pdf_state_page)
pdf_gen1(ARTBOX_URY,	"ArtBox/ury",	1, pdf_state_page)

/* font and text */
pdf_gen1(FONTAFM,	"FontAFM",	1, pdf_state_all)
pdf_gen1(FONTPFM,	"FontPFM",	1, pdf_state_all)
pdf_gen1(FONTOUTLINE,	"FontOutline",	1, pdf_state_all)
pdf_gen1(ENCODING,	"Encoding",	1, pdf_state_all)
pdf_gen1(TEXTX,		"textx",	1, pdf_state_ppt)
pdf_gen1(TEXTY,		"texty",	1, pdf_state_ppt)
pdf_gen1(FONT,		"font",		1, pdf_state_ppt)
pdf_gen1(FONTSIZE,	"fontsize",	1, pdf_state_ppt)
pdf_gen1(LEADING,	"leading",	1, pdf_state_ppt)
pdf_gen1(TEXTRISE,	"textrise",	1, pdf_state_ppt)
pdf_gen1(HORIZSCALING,	"horizscaling",	1, pdf_state_ppt | pdf_state_document)
pdf_gen1(TEXTRENDERING,	"textrendering",1, pdf_state_ppt)
pdf_gen1(CHARSPACING,	"charspacing",	1, pdf_state_ppt | pdf_state_document)
pdf_gen1(WORDSPACING,	"wordspacing",	1, pdf_state_ppt | pdf_state_document)
pdf_gen1(UNDERLINE,	"underline",	1, pdf_state_ppt)
pdf_gen1(OVERLINE,	"overline",	1, pdf_state_ppt)
pdf_gen1(STRIKEOUT,	"strikeout",	1, pdf_state_ppt)
pdf_gen1(FONTNAME,	"fontname",	1, pdf_state_ppt)
pdf_gen1(FONTENCODING,	"fontencoding",	1, pdf_state_ppt)
pdf_gen1(CAPHEIGHT,	"capheight",	0, pdf_state_all)
pdf_gen1(ASCENDER,	"ascender",	0, pdf_state_all)
pdf_gen1(DESCENDER,	"descender",	0, pdf_state_all)
pdf_gen1(NATIVEUNICODE,	"nativeunicode",1, pdf_state_all)
pdf_gen1(FONTWARNING,	"fontwarning",	1, pdf_state_all)

/* graphics */
pdf_gen1(CURRENTX,	"currentx",	1, pdf_state_ppt)
pdf_gen1(CURRENTY,	"currenty",	1, pdf_state_ppt)
pdf_gen1(FILLRULE,	"fillrule",	1, pdf_state_ppt)

/* image */
pdf_gen1(IMAGEWARNING,	"imagewarning",	1, pdf_state_all)
pdf_gen1(INHERITGSTATE,	"inheritgstate",1, pdf_state_all)
pdf_gen1(PASSTHROUGH,	"passthrough",	1, pdf_state_all)
pdf_gen1(IMAGEWIDTH,	"imagewidth",	0,
	    pdf_state_path | pdf_state_ppt | pdf_state_document)
pdf_gen1(IMAGEHEIGHT,	"imageheight",	0,
	    pdf_state_path | pdf_state_ppt | pdf_state_document)
pdf_gen1(RESX,		"resx",		0,
	    pdf_state_path | pdf_state_ppt | pdf_state_document)
pdf_gen1(RESY,		"resy",		0,
	    pdf_state_path | pdf_state_ppt | pdf_state_document)

/* PDI */
pdf_gen1(PDI,		"pdi",		1, pdf_state_all)
pdf_gen1(PDIWARNING,	"pdiwarning",	1, pdf_state_all)
pdf_gen1(PDISTRICT,	"pdistrict",	1, pdf_state_all)

/* hypertext */
pdf_gen1(OPENACTION,	"openaction",	1, pdf_state_ppt | pdf_state_document)
pdf_gen1(OPENMODE,	"openmode",	1, pdf_state_ppt | pdf_state_document)
pdf_gen1(BOOKMARKDEST,	"bookmarkdest",	1, pdf_state_ppt | pdf_state_document)
pdf_gen1(TRANSITION,	"transition",	1, pdf_state_all)
pdf_gen1(DURATION,	"duration",	1, pdf_state_all)
pdf_gen1(BASE,		"base",		1, pdf_state_ppt | pdf_state_document)

#undef	pdf_gen1
#undef	pdf_gen2
