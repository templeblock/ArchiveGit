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

/* $Id: p_cid.h,v 1.10.10.2 2002/01/07 18:26:29 tm Exp $
 *
 * Header file for PDFlib CID font information
 *
 */

#ifndef P_CID_H
#define P_CID_H

typedef struct {
    const char *name;
    pdf_charcoll charcoll;
    int compatibility;
} pdf_cmap;

typedef struct {
    const char *ordering;
    int supplement;
} pdf_charcoll_names;

pdf_charcoll_names charcoll_names[] = {
    { "GB1",		0 },	/* cc_simplified_chinese */
    { "CNS1",		1 },	/* cc_traditional_chinese */
    { "Japan1",		2 },	/* cc_japanese */
    { "Korea1",		1 },	/* cc_korean */
    { "Identity",	0 }	/* cc_identity (not used) */
};

/* predefined CMaps and the corresponding character collection */

pdf_cmap cmaps[] = {
    { "GB-EUC-H",	cc_simplified_chinese, PDF_1_2 },
    { "GB-EUC-V",	cc_simplified_chinese, PDF_1_2 },
    { "GBpc-EUC-H",	cc_simplified_chinese, PDF_1_2 },
    { "GBpc-EUC-V",	cc_simplified_chinese, PDF_1_3 },
    { "GBK-EUC-H",	cc_simplified_chinese, PDF_1_3 },
    { "GBK-EUC-V",	cc_simplified_chinese, PDF_1_3 },
    { "GBKp-EUC-H",	cc_simplified_chinese, PDF_1_4 },
    { "GBKp-EUC-V",	cc_simplified_chinese, PDF_1_4 },
    { "GBK2K-H",	cc_simplified_chinese, PDF_1_4 },
    { "GBK2K-V",	cc_simplified_chinese, PDF_1_4 },
    { "UniGB-UCS2-H",	cc_simplified_chinese, PDF_1_3 },
    { "UniGB-UCS2-V",	cc_simplified_chinese, PDF_1_3 },

    { "B5pc-H",		cc_traditional_chinese, PDF_1_2 },
    { "B5pc-V",		cc_traditional_chinese, PDF_1_2 },
    { "HKscs-B5-H",	cc_traditional_chinese, PDF_1_4 },
    { "HKscs-B5-V",	cc_traditional_chinese, PDF_1_4 },
    { "ETen-B5-H",	cc_traditional_chinese, PDF_1_2 },
    { "ETen-B5-V",	cc_traditional_chinese, PDF_1_2 },
    { "ETenms-B5-H",	cc_traditional_chinese, PDF_1_3 },
    { "ETenms-B5-V",	cc_traditional_chinese, PDF_1_3 },
    { "CNS-EUC-H",	cc_traditional_chinese, PDF_1_2 },
    { "CNS-EUC-V",	cc_traditional_chinese, PDF_1_2 },
    { "UniCNS-UCS2-H",	cc_traditional_chinese, PDF_1_3 },
    { "UniCNS-UCS2-V",	cc_traditional_chinese, PDF_1_3 },

    { "83pv-RKSJ-H",	cc_japanese, PDF_1_2 },
    { "90ms-RKSJ-H",	cc_japanese, PDF_1_2 },
    { "90ms-RKSJ-V",	cc_japanese, PDF_1_2 },
    { "90msp-RKSJ-H",	cc_japanese, PDF_1_3 },
    { "90msp-RKSJ-V",	cc_japanese, PDF_1_3 },
    { "90pv-RKSJ-H",	cc_japanese, PDF_1_2 },
    { "Add-RKSJ-H",	cc_japanese, PDF_1_2 },
    { "Add-RKSJ-V",	cc_japanese, PDF_1_2 },
    { "EUC-H",		cc_japanese, PDF_1_3 },
    { "EUC-V",		cc_japanese, PDF_1_3 },
    { "Ext-RKSJ-H",	cc_japanese, PDF_1_2 },
    { "Ext-RKSJ-V",	cc_japanese, PDF_1_2 },
    { "H",		cc_japanese, PDF_1_2 },
    { "V",		cc_japanese, PDF_1_2 },
    { "UniJIS-UCS2-H",	cc_japanese, PDF_1_3 },
    { "UniJIS-UCS2-V",	cc_japanese, PDF_1_3 },
    { "UniJIS-UCS2-HW-H",	cc_japanese, PDF_1_3 },
    { "UniJIS-UCS2-HW-V",	cc_japanese, PDF_1_3 },

    { "KSC-EUC-H",	cc_korean, PDF_1_2 },
    { "KSC-EUC-V",	cc_korean, PDF_1_2 },
    { "KSCms-UHC-H",	cc_korean, PDF_1_2 },
    { "KSCms-UHC-V",	cc_korean, PDF_1_2 },
    { "KSCms-UHC-HW-H",	cc_korean, PDF_1_3 },
    { "KSCms-UHC-HW-V",	cc_korean, PDF_1_3 },
    { "KSCpc-EUC-H",	cc_korean, PDF_1_2 },
    { "UniKS-UCS2-H",	cc_korean, PDF_1_3 },
    { "UniKS-UCS2-V",	cc_korean, PDF_1_3 },

    { "Identity-H",	cc_identity, PDF_1_2 },
    { "Identity-V",	cc_identity, PDF_1_2 }
};

/* Font descriptors for the preinstalled CID fonts */

const pdf_font pdf_cid_metrics[] = {
/* ---------------------------------------------------------- */
{	"HeiseiKakuGo-W5",		/* FontName */
	cid,				/* encoding */
	0,				/* used on current page */
	0,				/* embed */
	(char *) NULL,			/* font file */
	0L,				/* Object id */
	4L,				/* Font flags */
	CIDFontType0,			/* font type */
	NULL,				/* TT name */
	(char *) NULL,			/* Native encoding */
	cc_japanese,			/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-92,				/* llx */
	-250,				/* lly */
	1010,				/* urx */
	922,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-221,				/* Descender */
	114,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"HeiseiMin-W3",			/* FontName */
	cid,				/* encoding */
	0,				/* used on current page */
	0,				/* embed */
	(char *) NULL,			/* font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	CIDFontType0,			/* font type */
	NULL,				/* TT name */
	(char *) NULL,			/* Native encoding */
	cc_japanese,			/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-123,				/* llx */
	-257,				/* lly */
	1001,				/* urx */
	910,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	709,				/* CapHeight */
	450,				/* xHeight */
	723,				/* Ascender */
	-241,				/* Descender */
	69,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"HYGoThic-Medium",		/* FontName */
	cid,				/* encoding */
	0,				/* used on current page */
	0,				/* embed */
	(char *) NULL,			/* font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	CIDFontType0,			/* font type */
	NULL,				/* TT name */
	(char *) NULL,			/* Native encoding */
	cc_korean,			/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-6,				/* llx */
	-145,				/* lly */
	1003,				/* urx */
	880,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-271,				/* Descender */
	58,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"HYSMyeongJo-Medium",		/* FontName */
	cid,				/* encoding */
	0,				/* used on current page */
	0,				/* embed */
	(char *) NULL,			/* font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	CIDFontType0,			/* font type */
	NULL,				/* TT name */
	(char *) NULL,			/* Native encoding */
	cc_korean,			/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-0,				/* llx */
	-148,				/* lly */
	1001,				/* urx */
	880,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-271,				/* Descender */
	58,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"MHei-Medium",		/* FontName */
	cid,				/* encoding */
	0,				/* used on current page */
	0,				/* embed */
	(char *) NULL,			/* font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	CIDFontType0,			/* font type */
	NULL,				/* TT name */
	(char *) NULL,			/* Native encoding */
	cc_traditional_chinese,		/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-45,				/* llx */
	-250,				/* lly */
	1015,				/* urx */
	887,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-271,				/* Descender */
	58,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"MSung-Light",			/* FontName */
	cid,				/* encoding */
	0,				/* used on current page */
	0,				/* embed */
	(char *) NULL,			/* font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	CIDFontType0,			/* font type */
	NULL,				/* TT name */
	(char *) NULL,			/* Native encoding */
	cc_traditional_chinese,		/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-160,				/* llx */
	-259,				/* lly */
	1015,				/* urx */
	888,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-271,				/* Descender */
	58,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"STSong-Light",			/* FontName */
	cid,				/* encoding */
	0,				/* used on current page */
	0,				/* embed */
	(char *) NULL,			/* font file */
	0L,				/* Object id */
	4L,				/* Font flags */
	CIDFontType0,			/* font type */
	NULL,				/* TT name */
	(char *) NULL,			/* Native encoding */
	cc_simplified_chinese,		/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-250,				/* llx */
	-143,				/* lly */
	600,				/* urx */
	857,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	857,				/* CapHeight */
	599,				/* xHeight */
	857,				/* Ascender */
	-143,				/* Descender */
	91,				/* StdVW */
	91,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
}

};

#define SIZEOF_CID_METRICS	\
	((int) (sizeof(pdf_cid_metrics)/sizeof(pdf_font)))
#define NUMBER_OF_CMAPS		((int) (sizeof(cmaps)/sizeof(pdf_cmap)))

#endif /* P_CID_H */
