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

/* $Id: p_intern.h,v 1.62.10.17 2002/01/28 09:07:18 tm Exp $
 *
 * PDFlib internal definitions
 *
 */

#ifndef P_INTERN_H
#define P_INTERN_H

#include <stdarg.h>
#include <stdio.h>

#include "pdflib.h"
#include "p_config.h"

/* ------------------------ PDFlib feature configuration  ------------------- */

/* changing the following is not recommended, and not supported */

/* GIF image support */
#define PDF_GIF_SUPPORTED

/* JPEG image support */
#define PDF_JPEG_SUPPORTED

/* PNG image support, requires HAVE_LIBZ */
#define HAVE_LIBPNG

/* TIFF image support */
//j #define HAVE_LIBTIFF

/* zlib compression support */
#define HAVE_LIBZ

/* TrueType font support */
#define PDF_TRUETYPE_SUPPORTED

/* Support for host fonts
 * Requires PDF_TRUETYPE_SUPPORTED and WIN32.
 */
#if defined(PDF_TRUETYPE_SUPPORTED) && defined(WIN32) && !defined(WINCE)
#define PDF_HOSTFONTS_SUPPORTED
#endif

/* Support for pulling encodings from the host system */
#define PDF_HOSTENCODING_SUPPORTED

/* ----------------------------------- macros ------------------------------- */

#define PDFLIBSERIAL		"PDFLIBSERIAL"		/* name of env var. */
#define RESOURCEFILE		"PDFLIBRESOURCE"	/* name of env var. */

#ifndef MVS
#define DEFAULTRESOURCEFILE	"pdflib.upr"
#else
#define DEFAULTRESOURCEFILE	"upr"
#endif

#define	PDF_1_2			12		/* PDF 1.2 = Acrobat 3 */
#define	PDF_1_3			13		/* PDF 1.3 = Acrobat 4 */
#define	PDF_1_4			14		/* PDF 1.4 = Acrobat 5 */

/*
 * Allocation chunk sizes. These don't affect the generated documents
 * in any way. In order to save initial memory, however, you can lower 
 * the values. Increasing the values will bring some performance gain
 * for large documents, but will waste memory for small ones.
 */
#define PAGES_CHUNKSIZE		512		/* pages */
#define PNODES_CHUNKSIZE	64		/* page tree nodes */
#define ID_CHUNKSIZE		2048		/* object ids */
#define CONTENTS_CHUNKSIZE	64		/* page content streams */
#define FONTS_CHUNKSIZE		16		/* document fonts */
#define XOBJECTS_CHUNKSIZE	128		/* document xobjects */
#define IMAGES_CHUNKSIZE	128		/* document images */
#define OUTLINE_CHUNKSIZE	256		/* document outlines */
#define STREAM_CHUNKSIZE	65536		/* output buffer for in-core */
#define PDI_CHUNKSIZE		16		/* PDI instances */
#define COLORSPACES_CHUNKSIZE	4		/* separation color spaces */
#define PATTERN_CHUNKSIZE	4		/* pattern */

/* The following shouldn't require any changes */
#define ENC_MAX			128		/* max number of encodings */
#define PDF_MAX_SAVE_LEVEL	10		/* max number of save levels */
#define PDF_FILENAMELEN		1024		/* maximum file name length */
#define PDF_MAX_PARAMSTRING	128		/* image parameter string */

#ifdef HAVE_LIBZ
#define PDF_DEFAULT_COMPRESSION	6		/* default zlib level */
#else
#define PDF_DEFAULT_COMPRESSION	0		/* no compression */
#endif

#define PDF_LINEBUFLEN		256		/* len of pdf_printf() buffer */

/* configurable flush points */
#define PDF_FLUSH_NONE		0		/* end of document */
#define PDF_FLUSH_PAGE		(1<<0)		/* after page */
#define PDF_FLUSH_CONTENT	(1<<1)		/* font, xobj, annots */
#define PDF_FLUSH_RESERVED1	(1<<2)		/* reserved */
#define PDF_FLUSH_RESERVED2	(1<<3)		/* reserved */
#define PDF_FLUSH_HEAVY		(1<<4)		/* before realloc attempt */

/*
 * An arbitrary number used as a sanity check for the PDF pointer.
 * Actually, we use the hex representation of pi in order to avoid
 * the more common patterns.
 */
#define PDF_MAGIC	((unsigned long) 0x126960A1)

/* Unfortunately M_PI causes porting woes, so we use a private name */
#define PDF_M_PI		3.14159265358979323846		/* pi */

/* Acrobat viewers change absolute values < 1/65536 to zero */
#define PDF_SMALLREAL	(0.000015)

/* Acrobat viewers have an upper limit on real and integer numbers */
#define PDF_BIGREAL		(32768.0)
#define PDF_BIGINT		(2147483647.0)

/* Acrobat viewers have limits on page dimensions */
#define PDF_ACRO3_MINPAGE	((float) 72)	/* 1 inch = 2.54 cm */
#define PDF_ACRO3_MAXPAGE	((float) 3240)	/* 45 inch = 114,3 cm */
#define PDF_ACRO4_MINPAGE	((float) 3)	/* 1/24 inch = 0.106 cm */
#define PDF_ACRO4_MAXPAGE	((float) 14400)	/* 200  inch = 508 cm */ 

/*
 * When there's no PDF pointer we don't have access to an error handler.
 * Since the client completely screwed up, we refuse to do
 * anything in the hope the client will recognize his trouble.
 */

#define PDF_SANITY_CHECK_FAILED(p)	(p == NULL || p->magic != PDF_MAGIC)

#define NEW_ID		0L
#define BAD_ID		-1L

/* Note: pdf_begin_obj() is a function */
#define pdf_end_obj(p)		pdf_puts(p, "endobj\n")

#define pdf_begin_dict(p)	pdf_puts(p, "<<")
#define pdf_end_dict(p)		pdf_puts(p, ">>\n")

#define pdf_begin_stream(p)	pdf_puts(p, "stream\n")
#define pdf_end_stream(p)	pdf_puts(p, "endstream\n")

#define pdf_false	0
#define pdf_true	1

/* ------------------------ character-set related stuff --------------------- */

/* some ASCII characters and strings, deliberately defined as hex/oct codes */
#define PDF_NEWLINE		((char) 0x0a)		/* ASCII '\n' */
#define PDF_RETURN		((char) 0x0d)		/* ASCII '\r' */
#define PDF_SPACE		((char) 0x20)		/* ASCII ' '  */
#define PDF_HASH		((char) 0x23)		/* ASCII '#'  */
#define PDF_SLASH		((char) 0x2f)		/* ASCII '/'  */
#define PDF_BACKSLASH		((char) 0x5c)		/* ASCII '\\' */
#define PDF_PARENLEFT		((char) 0x28)		/* ASCII '('  */
#define PDF_PARENRIGHT		((char) 0x29)		/* ASCII ')'  */
#define PDF_EXCLAM		((char) 0x21)		/* ASCII '!'  */
#define PDF_COMMA		((char) 0x2c)		/* ASCII ','  */
#define PDF_SEMICOLON		((char) 0x3b)		/* ASCII ';'  */
#define PDF_O			((char) 0x4f)		/* ASCII 'O'  */
#define PDF_T			((char) 0x54)		/* ASCII 'T'  */
#define PDF_n			((char) 0x6e)		/* ASCII 'n'  */
#define PDF_r			((char) 0x72)		/* ASCII 'r'  */
#define PDF_z			((char) 0x7a)		/* ASCII 'z'  */
#define PDF_0			((char) 0x30)		/* ASCII '0'  */

#define PDF_STRING_0123456789ABCDEF	\
	"\060\061\062\063\064\065\066\067\070\071\101\102\103\104\105\106"

#ifdef PDFLIB_EBCDIC
#define pdf_make_ascii(s)		pdf_ebcdic2ascii(s)
#define pdf_make_ascii_len(s, l)	pdf_ebcdic2ascii_len(s, l)
#define pdf_make_ebcdic(s)		pdf_ascii2ebcdic(s)
#endif

/* The Unicode byte order mark (BOM) signals Unicode strings */
#define PDF_BOM0		0376		/* '\xFE' */
#define PDF_BOM1		0377		/* '\xFF' */

/* tracing of PDF API calls */
#if defined(DEBUG) && !defined(DEBUG_TRACE)
# define DEBUG_TRACE
#endif

#ifdef DEBUG_TRACE
# ifndef DEBUG_TRACE_FILE
#  if defined(MVS)
#   define DEBUG_TRACE_FILE	"pdftrace"
#  elif defined(MAC)
#   define DEBUG_TRACE_FILE	"PDFlib.trace"
#  elif defined(WIN32)
#   define DEBUG_TRACE_FILE	"/PDFlib.trace"
#  else
#   define DEBUG_TRACE_FILE     "/tmp/PDFlib.trace"
#  endif
# endif
# define PDF_TRACE(ARGS)	pdf_trace ARGS
#else
# define PDF_TRACE(ARGS)	/* */
#endif

/*
 * check whether the string is plain C or unicode by looking for the BOM
 * s must not be NULL.
 */
#define pdf_is_unicode(s) \
	(((unsigned char *)(s))[0] == PDF_BOM0 &&	\
	 ((unsigned char *)(s))[1] == PDF_BOM1)

/* --------------------------- typedefs and enums --------------------------- */

typedef int pdf_bool;
typedef long id;
typedef unsigned char pdf_byte;
typedef enum { ImageB = 1, ImageC = 2, ImageI = 4, Text = 8 } pdf_procset;
typedef enum {pdf_fill_winding, pdf_fill_evenodd} pdf_fillrule;
typedef enum { c_none, c_page, c_path, c_text } pdf_content_type;

/* Transition types for page transition effects */
typedef enum {
    trans_none, trans_split, trans_blinds, trans_box, 
    trans_wipe, trans_dissolve, trans_glitter, trans_replace
} pdf_transition;

/* The color spaces are PDF standard except ImageMask, which we use
 * internally.
 */
typedef enum {
    DeviceGray = 0, DeviceRGB, DeviceCMYK, 
    CalGray, CalRGB, Lab, 
    Indexed, PatternCS, Separation,
    ImageMask, LastCS
} pdf_colorspace;

/* Document open modes */
typedef enum {
    open_auto, open_none, open_bookmarks, open_thumbnails, open_fullscreen
} pdf_openmode;

/* Destination types for internal and external links */
typedef enum { retain, fitpage, fitwidth, fitheight, fitbbox } pdf_desttype;

/* Border styles for links */
typedef enum {
    border_solid, border_dashed, border_beveled,
    border_inset, border_underlined
} pdf_border_style;

/* Internal PDFlib states for error checking */
typedef enum {
    pdf_state_object = 1<<0,		/* outside any document */
    pdf_state_document = 1<<1,		/* document */
    pdf_state_page = 1<<2,		/* page description in a document */
    pdf_state_pattern = 1<<3,		/* pattern in a document */
    pdf_state_template = 1<<4,		/* template in a document */
    pdf_state_path = 1<<5,		/* path in a page description */
    pdf_state_error = 1<<6		/* in error cleanup */
} pdf_state;

#define pdf_state_ppt	\
	(pdf_state_page | pdf_state_pattern | pdf_state_template)

#define PDF_STATE_STACK_SIZE	4

#define PDF_GET_STATE(p)						\
	((p)->state_stack[(p)->state_sp])

#define PDF_SET_STATE(p, fn, s)						\
	((p)->state_stack[(p)->state_sp] = (s))

#define PDF_CHECK_SCOPE(p, fn, s)					\
	if (((p)->state_stack[(p)->state_sp] & (s)) == 0)		\
	    pdf_scope_error(p, fn)

#define PDF_PUSH_STATE(p, fn, s)					\
	if ((p)->state_sp == PDF_STATE_STACK_SIZE - 1)			\
	    pdf_error(p, PDF_SystemError,				\
		"state stack overflow in function '%s'", fn);		\
	else								\
	    (p)->state_stack[++(p)->state_sp] = (s)

#define PDF_POP_STATE(p, fn)						\
	if ((p)->state_sp == 0)						\
	    pdf_error(p, PDF_SystemError,				\
		"state stack underflow in function '%s'", fn);		\
	else								\
	    --(p)->state_sp


/* -------------------------------- structs -------------------------------- */

#define PDI_DEFINED
typedef struct PDI_s PDI;	/* The opaque PDI type */

/* Destination structure for bookmarks and links */
typedef struct {
    pdf_desttype type;
    int page;
} pdf_dest;

typedef struct { float llx, lly, urx, ury; } pdf_rectangle;
typedef struct { float a, b, c, d, e, f; } pdf_matrix;

/* Opaque types which are detailed in the respective modules */
typedef struct pdf_csresource_s pdf_csresource;
typedef struct pdf_xobject_s pdf_xobject;
typedef struct pdf_res_s pdf_res;
typedef struct pdf_category_s pdf_category;
typedef struct pdf_annot_s pdf_annot;
typedef struct pdf_info_s pdf_info;
typedef struct pdf_outline_s pdf_outline;
typedef struct pdf_image_s pdf_image;
typedef struct pdf_font_s pdf_font;
typedef struct pdf_encodingvector_s pdf_encodingvector;
typedef struct pdf_stream_s pdf_stream;
typedef struct pdf_pattern_s pdf_pattern;

/* -------------------- special graphics state -------------------- */
typedef struct {
    pdf_matrix	ctm;		/* current transformation matrix */
    float	x;		/* current x coordinate */
    float	y;		/* current y coordinate */

    float	startx;		/* starting x point of the subpath */
    float	starty;		/* starting y point of the subpath */

    float	lwidth;		/* line width */
    int		lcap;		/* line cap style */
    int		ljoin;		/* line join style */
    float	miter;		/* line join style */
    float	flatness;	/* line join style */
    pdf_bool	dashed;		/* line dashing in effect */

    /* LATER: rendering intent and flatness */
} pdf_gstate;

/* ------------------------ text state ---------------------------- */
typedef struct {
    float	c;		/* character spacing */
    float	w;		/* word spacing */
    float	h;		/* horizontal scaling */
    float	l;		/* leading */
    int		f;		/* slot number of the current font */
    float	fs;		/* font size */
    pdf_matrix	m;		/* text matrix */
    int		mode;		/* text rendering mode */
    float	rise;		/* text rise */
    pdf_matrix	lm;		/* text line matrix */
} pdf_tstate;

/* ----------------------- color space --------------------------- */
typedef struct {
    pdf_colorspace	cs;
    union {
	float		gray;		/* for DeviceGray */
	int		pattern;	/* for Pattern */
	struct {			/* for DeviceRGB */
	    float	r;
	    float	g;
	    float	b;
	} rgb; 
	struct {			/* for DeviceCMYK */
	    float	c;
	    float	m;
	    float	y;
	    float	k;
	} cmyk; 
	struct {			/* for Separation */
	    float	tint;
	    int		cs;	/* index into color space resource list */
	} sep; 
    } val;
} pdf_color;
    
typedef struct {
    pdf_color	fill;
    pdf_color	stroke;
} pdf_cstate;

/* A color space resource */
struct pdf_csresource_s {
    id		obj_id;			/* object id of this resource */
    pdf_bool	used_on_current_page;	/* this resource used on current page */
    char	*name;			/* quoted separation name */
    pdf_colorspace cs;			/* color space type */
    pdf_color	alternate;		/* alternate or base color space */
};

/*
 * *************************************************************************
 * The core PDF document descriptor
 * *************************************************************************
 */

struct PDF_s {
    /* -------------------------- general stuff ------------------------ */
    unsigned long	magic;		/* poor man's integrity check */

    char	*binding;		/* name of the language binding */
    char	*prefix;		/* prefix for resource file names */
    int		compatibility;		/* PDF version number * 10 */

    pdf_state	state_stack[PDF_STATE_STACK_SIZE];
    int		state_sp;		/* state stack pointer */

    /* ------------------- PDF Info dictionary entries ----------------- */
    char	*Keywords;
    char	*Subject;
    char	*Title;
    char	*Creator;
    char	*Author;
    pdf_info	*userinfo;		/* list of user-defined entries */

    /* ------------------------- PDF output ---------------------------- */
    FILE	*fp;
    char	*filename;
    pdf_stream	*stream;
    int		flush;			/* flushing strategy */
    /* writeproc == NULL means we're generating PDF in-core */
    size_t	(*writeproc)(PDF *p, void *data, size_t size);
    int		floatdigits;		/* floating point output precision */

    /* -------------- error handling and memory management ------------- */
    void 	(*errorhandler)(PDF *p, int level, const char* msg);
    void	*(*malloc)(PDF *p, size_t size, const char *caller);
    void	*(*calloc)(PDF *p, size_t size, const char *caller);
    void	*(*realloc)(PDF *p, void *mem, size_t size, const char *caller);
    void	 (*free)(PDF *p, void *mem);
    void	*opaque;		/* user-specific, opaque data */

    /* ------------------------- PDF import ---------------------------- */
    PDI		**pdi;			/* PDI context array */
    int		pdi_capacity;		/* currently allocated size */
    int		pdi_number;		/* next available PDI number */
    pdf_bool	pdi_strict;		/* strict parsing enabled */
    char	*pdi_strbuf;		/* string buffer for pdi parms */
    int		pdi_strbuflen;		/* current size of pdi_strbuf */

    /* ------------------------ resource stuff ------------------------- */
    pdf_category *resources;		/* anchor for the resource list */
    char	*resourcefilename;	/* name of the resource file */
    pdf_bool	resourcefile_loaded;	/* already loaded the resource file */

    /* ------------------- object and id bookkeeping ------------------- */
    id		root_id;
    id		info_id;

    pdf_openmode open_mode;		/* document open mode */
    pdf_desttype open_action;		/* open action/first page display */
    pdf_desttype bookmark_dest;		/* destination type for bookmarks */
    char	*base;			/* document base URI */

    long	*file_offset;		/* the objects' file offsets */
    int		file_offset_capacity;
    id		currentobj;

    id		*pages;			/* page ids */
    int		pages_capacity;
    int 	current_page;		/* current page number (1-based) */

    id		*pnodes;		/* page tree node ids */
    int		pnodes_capacity;	/* current # of entries in pnodes */
    int		current_pnode;		/* current node number (0-based) */
    int		current_pnode_kids;	/* current # of kids in current node */

    /* ------------------- document resources ------------------- */
    pdf_font	*fonts;			/* all fonts in document */
    int		fonts_capacity;		/* currently allocated size */
    int		fonts_number;		/* next available font number */

    pdf_xobject *xobjects;		/* all xobjects in document */
    int		xobjects_capacity;	/* currently allocated size */
    int		xobjects_number;	/* next available xobject slot */

    pdf_csresource *colorspaces;	/* all color space resources */
    int		colorspaces_capacity;	/* currently allocated size */
    int		colorspaces_number;	/* next available xobject number */

    pdf_pattern *pattern;		/* all pattern resources */
    int		pattern_capacity;	/* currently allocated size */
    int		pattern_number;		/* next available pattern number */

    pdf_image  *images;			/* all images in document */
    int		images_capacity;	/* currently allocated size */

    /* ------------------- document outline tree ------------------- */
    int		outline_capacity;	/* currently allocated size */
    int		outline_count;		/* total number of outlines */
    pdf_outline	*outlines;		/* dynamic array of outlines */

    /* ------------------- page specific stuff ------------------- */
    id		res_id;			/* id of this page's res dict */
    id		contents_length_id;	/* id of current cont section's length*/
    id		*contents_ids;		/* content sections' chain */
    int		contents_ids_capacity;	/* # of content sections */
    id		next_content;		/* # of current content section */
    pdf_content_type	contents;	/* type of current content section */
    pdf_transition	transition;	/* type of page transition */
    float	duration;		/* duration of page transition */

    pdf_annot	*annots;		/* annotation chain */

    int		procset;		/* procsets for this page */
    long	start_contents_pos;	/* start offset of contents section */

    float	width;			/* MediaBox: current page's width */
    float	height;			/* MediaBox: current page's height */
    pdf_rectangle	CropBox;
    pdf_rectangle	BleedBox;
    pdf_rectangle	TrimBox;
    pdf_rectangle	ArtBox;
    id		thumb_id;		/* id of this page's thumb, or BAD_ID */

    int		sl;				/* current save level */
    pdf_gstate	gstate[PDF_MAX_SAVE_LEVEL];	/* graphics state */
    pdf_tstate	tstate[PDF_MAX_SAVE_LEVEL];	/* text state */
    pdf_cstate	cstate[PDF_MAX_SAVE_LEVEL];	/* color state */

    /* ------------------- template stuff ------------------- */
    int		templ;			/* current template if in templ. state*/

    /* ------------ other text and graphics-related stuff ------------ */
    /* leading, word, and character spacing are treated as a group */
    pdf_bool	textparams_done;	/* text parameters already set */
    pdf_bool	underline;		/* underline text */
    pdf_bool	overline;		/* overline text */
    pdf_bool	strikeout;		/* strikeout text */
    pdf_bool	inheritgs;		/* inherit gstate in templates */

    pdf_fillrule	fillrule;	/* nonzero or evenodd fill rule */

    /* -------------- annotation border style and color -------------- */
    pdf_border_style	border_style;
    float		border_width;
    float		border_red;
    float		border_green;
    float		border_blue;
    float		border_dash1;
    float		border_dash2;

    /* ------------------------ miscellaneous ------------------------ */
    int		chars_on_this_line;
    int		compresslevel;		/* zlib compression level */
    char	debug[256];		/* debug flags */
    void	*md5;			/* MD5 digest context for file ID */

    /* the list of known encoding vectors */
    pdf_encodingvector *encodings[ENC_MAX];
};

/* Data source for images, compression, ASCII encoding, fonts, etc. */
typedef struct PDF_data_source_s PDF_data_source;
struct PDF_data_source_s {
    pdf_byte 		*next_byte;
    size_t		bytes_available;
    void		(*init)(PDF *, PDF_data_source *src);
    int			(*fill)(PDF *, PDF_data_source *src);
    void		(*terminate)(PDF *, PDF_data_source *src);

    pdf_byte		*buffer_start;
    size_t		buffer_length;
    void		*private_data;
    long		offset;		/* start of data to read */
    long		length;		/* length of data to read */
    long		total;		/* total bytes read so far */
};

/* ------ Private functions for library-internal use only --------- */

/* p_basic.c */
void	pdf_begin_contents_section(PDF *p);
void	pdf_end_contents_section(PDF *p);
void	pdf_error(PDF *, int level, const char *fmt, ...);
#ifdef DEBUG_TRACE
void	pdf_trace(const char *fmt, ...);
#endif
void	pdf_scope_error(PDF *p, const char *funame);
const char *pdf_current_scope(PDF *p);
id	pdf_begin_obj(PDF *p, id obj_id);
id	pdf_alloc_id(PDF *p);
void	pdf_grow_pages(PDF *p);
void	pdf_write_page_procsets(PDF *p);

/* p_color.c */
extern const char *pdf_colorspace_names[];
void	pdf_init_cstate(PDF *p);
void	pdf_init_colorspaces(PDF *p);
void	pdf_write_page_colorspaces(PDF *p);
void	pdf_write_doc_colorspaces(PDF *p);
int	pdf_add_colorspace(PDF *p, pdf_colorspace cs, const char *spotname);
void	pdf_grow_colorspaces(PDF *p);
void	pdf_cleanup_colorspaces(PDF *p);

/* p_text.c */
void	pdf_init_tstate(PDF *p);
void	pdf_end_text(PDF *p);
void	pdf_set_leading(PDF *p, float leading);
void	pdf_set_text_rise(PDF *p, float rise);
void	pdf_set_horiz_scaling(PDF *p, float scale);
void	pdf_set_text_rendering(PDF *p, int mode);
void	pdf_set_char_spacing(PDF *p, float spacing);
void	pdf_set_word_spacing(PDF *p, float spacing);
const char *pdf_get_fontname(PDF *p);
const char *pdf_get_fontencoding(PDF *p);
float	pdf_get_fontsize(PDF *p);
int	pdf_get_font(PDF *p);

/* p_gstate.c */
void	pdf_init_gstate(PDF *p);
void	pdf_matrix_mul(const pdf_matrix *M, pdf_matrix *N);
pdf_bool pdf_is_identity_matrix(pdf_matrix *m);
void	pdf_concat_raw(PDF *p, pdf_matrix *m);
void	pdf_reset_gstate(PDF *p);

#ifdef HAVE_PDI
/* p_pdi.c */
void	pdf_init_pdi(PDF *p);
void	pdf_cleanup_pdi(PDF *p);
void	pdf_reset_pdi(PDF *p);
#endif	/* HAVE_PDI */


/* p_image.c */
void	pdf_init_images(PDF *p);
void	pdf_cleanup_images(PDF *p);
void	pdf_cleanup_image(PDF *p, int im);
void	pdf_init_xobjects(PDF *p);
void	pdf_write_xobjects(PDF *p);
void	pdf_cleanup_xobjects(PDF *p);

/* p_filter.c */
void	pdf_ASCII85Encode(PDF *p, PDF_data_source *src);
void	pdf_ASCIIHexEncode(PDF *p, PDF_data_source *src);

void	pdf_data_source_file_init(PDF *p, PDF_data_source *src);
int	pdf_data_source_file_fill(PDF *p, PDF_data_source *src);
void	pdf_data_source_file_terminate(PDF *p, PDF_data_source *src);

void	pdf_copy(PDF *p, PDF_data_source *src);
void	pdf_compress(PDF *p, PDF_data_source *src);

/* p_font.c */
void	pdf_init_fonts(PDF *p);
void	pdf_init_font_struct(PDF *p, pdf_font *font);
void	pdf_write_page_fonts(PDF *p);
void	pdf_write_doc_fonts(PDF *p);
void	pdf_cleanup_fonts(PDF *p);
void	pdf_cleanup_font_struct(PDF *p, pdf_font *font); /* compile_metrics.c */

/* p_annots.c */
void	pdf_init_annots(PDF *p);
void	pdf_write_annots_root(PDF *p);
void	pdf_init_page_annots(PDF *p);
void	pdf_write_page_annots(PDF *p);
void	pdf_cleanup_page_annots(PDF *p);

/* p_hyper.c */
void	pdf_init_outlines(PDF *p);
void	pdf_write_outlines(PDF *p);
void	pdf_write_outline_root(PDF *p);
void	pdf_cleanup_outlines(PDF *p);

void	pdf_init_transition(PDF *p);
void	pdf_write_page_transition(PDF *p);

void	pdf_init_info(PDF *p);
void	pdf_write_info(PDF *p);
void	pdf_cleanup_info(PDF *p);

void	pdf_set_transition(PDF *p, const char *type);
void	pdf_set_duration(PDF *p, float t);

/* p_stream.c */
pdf_bool pdf_buffer_not_empty(PDF *p);
long	pdf_tell(PDF *p);
void	pdf_write(PDF *p, const void *data, size_t size);
void	pdf_puts(PDF *p, const char *s);
void	pdf_putc(PDF *p, char c);
void	pdf_printf(PDF *p, const char *fmt, ...);
int	pdf_vsprintf(PDF *p, char *buf, const char *format, va_list args);

void	pdf_boot_stream(PDF *);
void	pdf_init_stream(PDF *p);
void	pdf_flush_stream(PDF *p);
void	pdf_close_stream(PDF *p);

#ifdef PDFLIB_EBCDIC
void	pdf_ebcdic2ascii(char *s);
void	pdf_ebcdic2ascii_len(char *s, size_t len);
void	pdf_ascii2ebcdic(char *s);
#endif

void	pdf_compress_init(PDF *p);
void	pdf_compress_end(PDF *p);
int	pdf_compresslevel(PDF *p);

/* p_util.c */
char   *pdf_strdup(PDF *p, const char *text);
size_t	pdf_strlen(const char *text);
char   *pdf_getenv(const char *name);
void	pdf_quote_string(PDF *p, const char *string);
void	pdf_quote_string2(PDF *p, const char *text, int len);
void	pdf_quote_name(PDF *p, const char *text, size_t len);
char   *pdf_make_quoted_name(PDF *p, const char *text, size_t len, char *buf);

char   *pdf_find_resource(PDF *p, const char *category,
		const char *resourcename, pdf_bool loadupr);
void	pdf_add_resource(PDF *p, const char *category, const char *resource,
		const char *filename, const char *prefix);
void	pdf_cleanup_resources(PDF *p);
int	pdf_load_encoding(PDF *p, const char *filename, const char *encoding);

/* p_pattern.c */
void	pdf_init_pattern(PDF *p);
void	pdf_write_page_pattern(PDF *p);
void	pdf_cleanup_pattern(PDF *p);
void	pdf_setpatterncolor(PDF *p, const char *type, int pattern);

/* p_filter.c */
void	pdf_noop(PDF *p, PDF_data_source *src);
int	pdf_data_source_buf_fill(PDF *p, PDF_data_source *src);
#endif	/* P_INTERN_H */
