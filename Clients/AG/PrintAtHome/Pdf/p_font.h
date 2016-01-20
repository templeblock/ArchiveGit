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

/* $Id: p_font.h,v 1.15.2.2 2002/01/07 18:26:29 tm Exp $
 *
 * Header file for the PDFlib font subsystem
 *
 */

#ifndef P_FONT_H
#define P_FONT_H

/* Ligature definition is a linked list since any character can have
 * any number of ligatures.
 */
typedef struct _t_ligature {
    char *succ, *lig;
    struct _t_ligature *next;
} Ligature;

/* Character Metric Information. This structure is used only if ALL 
 * character metric information is requested. If only the character
 * widths is requested, then only an array of the character x-widths
 * is returned.
 */
typedef struct {
    int code, 		/* key: C */
        wx,		/* key: WX */
        wy;		/* together wx and wy are associated with key: W */
    char *name; 	/* key: N */
    pdf_rectangle charBBox;	/* key: B */
    Ligature *ligs;	/* key: L (linked list; not a fixed number of Ls */
} CharMetricInfo;

/* Track kerning data structure.  */
typedef struct {
    int degree;  
    float minPtSize, 
          minKernAmt, 
          maxPtSize, 
          maxKernAmt;
} TrackKernData;

/* Pair Kerning data structure. */
typedef struct {
    char *name1;
    char *name2;
    int xamt,
        yamt;
} PairKernData;

/* PCC is a piece of a composite character. This is a sub structure of a
 * compCharData described below.
 */
typedef struct {
    char *pccName;
    int deltax,
        deltay;
} Pcc;

/* Composite Character Information data structure. 
 * The fields ccName and numOfPieces are filled with the values associated
 * with the key CC. The field pieces points to an array (size = numOfPieces)
 * of information about each of the parts of the composite character. That
 * array is filled in with the values from the key PCC.
 */
typedef struct {
    char *ccName;
    int numOfPieces;
    Pcc *pieces;
} CompCharData;

/* Predefined character collections */
typedef enum {
    cc_simplified_chinese, cc_traditional_chinese, cc_japanese, cc_korean,
    cc_identity, cc_none
} pdf_charcoll;

typedef enum { Type1, Type1C, TrueType, MMType1, Type3, Type0, CIDFontType0
} pdf_fonttype;

/*
 * Symbolic names for predefined font encodings. 0 and above are used
 * as indices in the pdf_encoding_vector array.
 */
typedef enum {
    invalidenc = -3,
    cid = -2, builtin = -1, winansi = 0, macroman = 1, ebcdic = 2
} pdf_encoding;

/* The core PDFlib font structure */
struct pdf_font_s {
    char	*name;			/* PostScript or TrueType name */
    int		encoding;		/* PDFlib font encoding shortcut */
    int		used_on_current_page;	/* this font is in use on current p. */
    int		embed;			/* whether or not to embed this font */
    char	*fontfilename;		/* name of external font file */
    id		obj_id;			/* object id of this font */

    unsigned long flags;		/* font flags for font descriptor */
    pdf_fonttype	type;		/* type of font */

    char	*ttname;		/* names[6] in the TT name table */
    char	*encodingScheme;	/* AFM key: EncodingScheme or CMap */
    pdf_charcoll charcoll;		/* CID character collection supported */
    float	italicAngle;		/* AFM key: ItalicAngle */
    int		isFixedPitch;		/* AFM key: IsFixedPitch */
    float	llx;			/* AFM key: FontBBox */
    float	lly;			/* AFM key: FontBBox */
    float	urx;			/* AFM key: FontBBox */
    float	ury;			/* AFM key: FontBBox */
    int		underlinePosition;  	/* AFM key: UnderlinePosition */
    int		underlineThickness; 	/* AFM key: UnderlineThickness */
    int		capHeight;		/* AFM key: CapHeight */
    int		xHeight;		/* AFM key: XHeight */
    int		ascender;		/* AFM key: Ascender */
    int		descender;		/* AFM key: Descender */
    int		StdVW;			/* AFM key: StdVW */
    int		StdHW;			/* AFM key: StdHW */

    int			numOfChars;	/* # of entries in char metrics */
    CharMetricInfo	*cmi;		/* ptr to char metrics array */
    int 		numOfTracks;	/* # of entries in track kerning array*/
    TrackKernData	*tkd;		/* ptr to track kerning array */
    int			numOfPairs;	/* # of entries in pair kerning array */
    PairKernData	*pkd;		/* ptr to pair kerning array */
    int			numOfComps;	/* # of entries in comp char array */
    CompCharData	*ccd;		/* ptr to comp char array */

    int        		widths[256];	/* From AFM char metrics and encoding */
    size_t		filelen;	/* length of uncompressed font data */
    pdf_byte		*img;		/* font (or CFF table) data in memory */
    long		cff_offset;	/* start of CFF table in font */
    size_t		cff_length;	/* length of CFF table in font */
};

struct pdf_encodingvector_s {
    char *apiname;	/* PDFlib's name of the encoding at the API */
    char *pdfname;	/* PDF's official name of the encoding */
    unsigned short *codes;/* 256 unicode values for TrueType fonts */
    char *chars[256];	/* the actual character names */
};

/* these defaults are used when the stem value must be derived from the name */
#define PDF_STEMV_MIN		50	/* minimum StemV value */
#define PDF_STEMV_LIGHT		71	/* light StemV value */
#define PDF_STEMV_NORMAL	109	/* normal StemV value */
#define PDF_STEMV_SEMIBOLD	135	/* semibold StemV value */
#define PDF_STEMV_BOLD		165	/* bold StemV value */
#define PDF_STEMV_EXTRABOLD	201	/* extrabold StemV value */
#define PDF_STEMV_BLACK		241	/* black StemV value */

/* Bit positions for the font descriptor flag */
#define FIXEDWIDTH	(long) (1L<<0)
#define SERIF		(long) (1L<<1)
#define SYMBOL		(long) (1L<<2)
#define SCRIPT		(long) (1L<<3)
#define ADOBESTANDARD	(long) (1L<<5)
#define ITALIC		(long) (1L<<6)
#define SMALLCAPS	(long) (1L<<17)
#define FORCEBOLD	(long) (1L<<18)

/* ---------------- font function prototypes -------------------------- */

/* p_truetype.c */
void		pdf_put_ttfont(PDF *p, int slot);
pdf_bool	pdf_get_metrics_tt(PDF *p, pdf_font *font,
		const char *fontname, int enc, const char *filename, int embed);

/* p_afm.c */
pdf_bool	pdf_get_metrics_afm(PDF *p, pdf_font *font,
		    const char *fontname, int enc, const char *filename);

/* p_pfm.c */
pdf_bool 	pdf_get_metrics_pfm(PDF *p, pdf_font *font,
		    const char *fontname, int enc, const char *filename);

/* p_font.c */
void	pdf_write_encoding(PDF *p, pdf_encodingvector *enc, id encoding_id);
void	pdf_write_fontdescriptor(PDF *p, pdf_font *font, id desc_id,id file_id);
void	pdf_cleanup_enc(PDF *p, pdf_encodingvector *enc);
void	pdf_make_fontflag(PDF *p, pdf_font *font);

/* p_unicode.c */
unsigned int pdf_adobe2unicode(PDF *p, const char *name);
const char * pdf_unicode2adobe(PDF *p, unsigned int uv);

#endif	/* P_FONT_H */
