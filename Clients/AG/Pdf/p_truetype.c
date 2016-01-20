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

/* $Id: p_truetype.c,v 1.33.2.23 2002/01/26 18:32:18 tm Exp $
 *
 * PDFlib TrueType handling routines
 *
 */

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p_intern.h"
#include "p_font.h"
#include "p_truetype.h"

#ifdef PDF_TRUETYPE_SUPPORTED

#if defined(PDF_HOSTFONTS_SUPPORTED)
#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(macintosh)
#include <Files.h>
#include <Fonts.h>
#include <Resources.h>
#include <TextUtils.h>
#endif
#endif

static void pdf_cleanup_tt(PDF *p, tt_file *ttf);

static void
tt_error(PDF *p, tt_file *ttf, int level, const char *msg)
{
    const char *filename = ttf->filename;

    pdf_cleanup_tt(p, ttf);	/* this will not free the filename */
    if (filename)
	pdf_error(p, level, "Error reading TrueType font file '%s': %s",
	    filename, msg);
    else
	pdf_error(p, level, "Error reading TrueType font: %s", msg);
} /* tt_error */

#define TT_ASSERT(p, ttf, cond)		\
	((cond) ? (void) 0 : tt_error(p, ttf, PDF_SystemError, \
		"Internal error"))

#define TT_IOCHECK(p, ttf, cond)	\
	((cond) ? (void) 0 : tt_error(p, ttf, PDF_IOError, \
		"Corrupt TrueType font file"))

#if	TT_INCORE

static void
tt_seek(PDF *p, tt_file *ttf, long offset)
{
    TT_IOCHECK(p, ttf, ttf->img + (tt_ulong) offset <= ttf->end);
    ttf->pos = ttf->img + (tt_ulong) offset;
}

static void
tt_read(PDF *p, tt_file *ttf, void *buf, int nbytes)
{
    TT_IOCHECK(p, ttf, ttf->pos + (tt_ulong) nbytes <= ttf->end);
    memcpy(buf, ttf->pos, (size_t) nbytes);
    ttf->pos += (tt_ulong) nbytes;
}

static long
tt_tell(tt_file *ttf)
{
    return (long) (ttf->pos - ttf->img);
}

#if	TT_MACROS

#define tt_get_ushort(p, ttf) (						\
    TT_IOCHECK(p, ttf, (ttf->pos += 2) <= ttf->end),			\
    (tt_ushort) (((tt_ushort) ttf->pos[-2] << 8) | ttf->pos[-1])	\
)

#define tt_get_short(p, ttf) (						\
    TT_IOCHECK(p, ttf, (ttf->pos += 2) <= ttf->end),			\
    (tt_short) (((tt_short) (tt_char) ttf->pos[-2] << 8) | ttf->pos[-1])\
)

#else

static tt_ushort
tt_get_ushort(PDF *p, tt_file *ttf)
{
    tt_byte *pos = ttf->pos;

    TT_IOCHECK(p, ttf, (ttf->pos += 2) <= ttf->end);

    return (tt_ushort) (((tt_ushort) pos[0] << 8) | pos[1]);
}

static tt_short
tt_get_short(PDF *p, tt_file *ttf)
{
    tt_byte *pos = ttf->pos;

    TT_IOCHECK(p, ttf, (ttf->pos += 2) <= ttf->end);

    return (tt_short) (((tt_short) (tt_char) pos[0] << 8) | pos[1]);
}

#endif	/* !TT_MACROS */

static tt_ulong
tt_get_ulong(PDF *p, tt_file *ttf)
{
    tt_byte *pos = ttf->pos;

    TT_IOCHECK(p, ttf, (ttf->pos += 4) <= ttf->end);

    return  ((tt_ulong) pos[0] << 24) |
	    ((tt_ulong) pos[1] << 16) |
	    ((tt_ulong) pos[2] <<  8) |
			pos[3];
}

static tt_long
tt_get_long(PDF *p, tt_file *ttf)
{
    tt_byte *pos = ttf->pos;

    TT_IOCHECK(p, ttf, (ttf->pos += 4) <= ttf->end);

    return  ((tt_long) (tt_char)   pos[0] << 24) |
	    ((tt_long)		   pos[1] << 16) |
	    ((tt_long)		   pos[2] <<  8) |
				   pos[3];
}

#else

static void
tt_seek(PDF *p, tt_file *ttf, long offset)
{
    TT_IOCHECK(p, ttf, fseek(ttf->fp, offset, SEEK_SET) == 0);
}

static void
tt_read(PDF *p, tt_file *ttf, void *buf, int nbytes)
{
    TT_IOCHECK(p, ttf, fread(buf, 1, nbytes, ttf->fp) == nbytes);
}

static long
tt_tell(tt_file *ttf)
{
    return ftell(ttf->fp);
}

static tt_ushort
tt_get_ushort(PDF *p, tt_file *ttf)
{
    tt_byte buf[2];

    TT_IOCHECK(p, ttf, fread(buf, 1, 2, ttf->fp) == 2);

    return (tt_ushort) (((tt_ushort) buf[0] << 8) | buf[1]);
}

static tt_short
tt_get_short(PDF *p, tt_file *ttf)
{
    tt_byte buf[2];

    TT_IOCHECK(p, ttf, fread(buf, 1, 2, ttf->fp) == 2);

    return (tt_short) (((tt_short) (tt_char) buf[0] << 8) | buf[1]);
}

static tt_ulong
tt_get_ulong(PDF *p, tt_file *ttf)
{
    tt_byte buf[4];

    TT_IOCHECK(p, ttf, fread(buf, 1, 4, ttf->fp) == 4);

    return  ((tt_ulong) buf[0] << 24) |
	    ((tt_ulong) buf[1] << 16) |
	    ((tt_ulong) buf[2] <<  8) |
			buf[3];
}

static tt_long
tt_get_long(PDF *p, tt_file *ttf)
{
    tt_byte buf[4];

    TT_IOCHECK(p, ttf, fread(buf, 1, 4, ttf->fp) == 4);

    return  ((tt_long) (tt_char) buf[0] << 24) |
	    ((tt_long)		   buf[1] << 16) |
	    ((tt_long)		   buf[2] <<  8) |
				   buf[3];
}

#endif	/* !TT_INCORE */

static void
tt_get_dirent(PDF *p, tt_dirent *dirent, tt_file *ttf)
{
    tt_read(p, ttf, dirent->tag, 4);
    dirent->tag[4] = 0;
    dirent->checksum = tt_get_ulong(p, ttf);
    dirent->offset = tt_get_ulong(p, ttf);
    dirent->length = tt_get_ulong(p, ttf);
} /* tt_get_dirent */


static int
tt_tag2idx(tt_file *ttf, char *tag)
{
    int i;

    for (i = 0; i < ttf->n_tables; ++i)
	if (strcmp(ttf->dir[i].tag, tag) == 0)
	    return i;

    return -1;
} /* tt_tag2idx */

static void
tt_get_cmap0(PDF *p, tt_file *ttf, tt_cmap0_6 *cm0_6)
{
    cm0_6->length	= tt_get_ushort(p, ttf);
    cm0_6->language	= tt_get_ushort(p, ttf);

    /* These are not used in format 0 */
    cm0_6->firstCode	= 0;
    cm0_6->entryCount	= 256;

    tt_read(p, ttf, cm0_6->glyphIdArray, 256);
} /* tt_get_cmap0 */

static void
tt_get_cmap6(PDF *p, tt_file *ttf, tt_cmap0_6 *cm0_6)
{
    tt_ushort c, last;

    cm0_6->length	= tt_get_ushort(p, ttf);
    cm0_6->language	= tt_get_ushort(p, ttf);
    cm0_6->firstCode	= tt_get_ushort(p, ttf);
    cm0_6->entryCount	= tt_get_ushort(p, ttf);

    /* default for codes outside the range specified in this table */
    for (c = 0; c < 256; c++)
	cm0_6->glyphIdArray[c] = 0;

    last = (tt_ushort) (cm0_6->firstCode + cm0_6->entryCount);
    TT_IOCHECK(p, ttf, last <= 256);

    for (c = cm0_6->firstCode; c < last; c++)
	cm0_6->glyphIdArray[c] = (unsigned char) tt_get_ushort(p, ttf);

} /* tt_get_cmap6 */

static int
tt_code2gidx0_6(PDF *p, tt_file *ttf, int code)
{
    TT_ASSERT(p, ttf, ttf->tab_cmap != (tt_tab_cmap *) 0);
    TT_ASSERT(p, ttf, ttf->tab_cmap->mac != (tt_cmap0_6 *) 0);
    TT_ASSERT(p, ttf, 0 <= code && code <= 255);

    return ttf->tab_cmap->mac->glyphIdArray[code];
} /* tt_code2gidx0_6 */

static void
tt_get_cmap4(PDF *p, tt_file *ttf, tt_cmap4 *cm4, tt_ushort encID)
{
    static const char *fn = "tt_get_cmap4";
    int		i, segs;

    cm4->encodingID	= encID;

    /* the instruction order is critical for cleanup after exceptions!
    */
    cm4->endCount	= (tt_ushort *) 0;
    cm4->startCount	= (tt_ushort *) 0;
    cm4->idDelta	= (tt_short *)  0;
    cm4->idRangeOffs	= (tt_ushort *) 0;
    cm4->glyphIdArray	= (tt_ushort *) 0;

    cm4->format		= tt_get_ushort(p, ttf);
    TT_IOCHECK(p, ttf, cm4->format == 4);
    cm4->length		= tt_get_ushort(p, ttf);
    cm4->version	= tt_get_ushort(p, ttf);
    cm4->segCountX2	= tt_get_ushort(p, ttf);
    cm4->searchRange	= tt_get_ushort(p, ttf);
    cm4->entrySelector	= tt_get_ushort(p, ttf);
    cm4->rangeShift	= tt_get_ushort(p, ttf);

    segs = cm4->segCountX2 / 2;
    cm4->numGlyphIds = (cm4->length - (16 + 8 * segs)) / 2;
    TT_IOCHECK(p, ttf,
	0 <= cm4->numGlyphIds && cm4->numGlyphIds < cm4->length);

    cm4->endCount =
	(tt_ushort *) p->malloc(p, (size_t) (sizeof (tt_ushort) * segs), fn);
    cm4->startCount =
	(tt_ushort *) p->malloc(p, (size_t) (sizeof (tt_ushort) * segs), fn);
    cm4->idDelta =
	(tt_short *)  p->malloc(p, (size_t) (sizeof (tt_ushort) * segs), fn);
    cm4->idRangeOffs =
	(tt_ushort *) p->malloc(p, (size_t) (sizeof (tt_ushort) * segs), fn);
    
    if (cm4->numGlyphIds) {
	cm4->glyphIdArray = (tt_ushort *)
	    p->malloc(p, (size_t) (sizeof (tt_ushort) * cm4->numGlyphIds), fn);
    }

    for (i = 0; i < segs; ++i)
	cm4->endCount[i] = tt_get_ushort(p, ttf);

    TT_IOCHECK(p, ttf, cm4->endCount[segs - 1] == 0xFFFF);

    (void) tt_get_ushort(p, ttf);	/* padding */
    for (i = 0; i < segs; ++i)	cm4->startCount[i] = tt_get_ushort(p, ttf);
    for (i = 0; i < segs; ++i)	cm4->idDelta[i] = tt_get_short(p, ttf);
    for (i = 0; i < segs; ++i)	cm4->idRangeOffs[i] = tt_get_ushort(p, ttf);

    for (i = 0; i < cm4->numGlyphIds; ++i)
	cm4->glyphIdArray[i] = tt_get_ushort(p, ttf);
} /* tt_get_cmap4 */

/* translate character code to glyph index using cmap4.  */

static int
tt_code2gidx4(PDF *p, tt_file *ttf, int code)
{
    tt_cmap4 *	cm4;
    int		segs;
    int		i;

    TT_ASSERT(p, ttf, ttf->tab_cmap != (tt_tab_cmap *) 0);
    TT_ASSERT(p, ttf, ttf->tab_cmap->win != (tt_cmap4 *) 0);

    cm4 = ttf->tab_cmap->win;
    segs = cm4->segCountX2 / 2;

    for (i = 0; i < segs; ++i)
	if (code <= cm4->endCount[i])
	    break;

    TT_IOCHECK(p, ttf, i != segs);
    if (code < cm4->startCount[i])
	return 0;

    if (cm4->idRangeOffs[i] == 0)
	return (code + cm4->idDelta[i]) & 0xFFFF;
    else
    {
	int idx = cm4->idRangeOffs[i] / 2
			    + (code - cm4->startCount[i]) - (segs - i);

	TT_IOCHECK(p, ttf, 0 <= idx && idx < cm4->numGlyphIds);

	if (cm4->glyphIdArray[idx] == 0)
	    return 0;
	else
	    return (cm4->glyphIdArray[idx] + cm4->idDelta[i]) & 0xFFFF;
    }
} /* tt_code2gidx4 */


static void
tt_get_tab_cmap(PDF *p, tt_file *ttf)
{
    static const char *fn = "tt_get_tab_cmap";
    tt_tab_cmap *tp = (tt_tab_cmap *) p->malloc(p, sizeof (tt_tab_cmap), fn);
    int		idx = tt_tag2idx(ttf, pdf_str_cmap);
    int		i;
    tt_ulong	offset;
    tt_ushort	numEncTabs;
    tt_ushort	tableFormat;

    /* the instruction order is critical for cleanup after exceptions!
    */
    tp->win = (tt_cmap4 *) 0;
    tp->mac = (tt_cmap0_6 *) 0;
    ttf->tab_cmap = tp;

    TT_IOCHECK(p, ttf, idx != -1);
    offset = ttf->dir[idx].offset;
    tt_seek(p, ttf, (long) offset);

    (void) tt_get_ushort(p, ttf);	/* version */
    numEncTabs = tt_get_ushort(p, ttf);

    for (i = 0; i < numEncTabs; ++i)
    {
	tt_ushort platformID = tt_get_ushort(p, ttf);
	tt_ushort encodingID = tt_get_ushort(p, ttf);
	tt_ulong offsetEncTab = tt_get_ulong(p, ttf);
	tt_long pos = tt_tell(ttf);

	tt_seek(p, ttf, (long) (offset + offsetEncTab));

	if (platformID == tt_pfid_mac && encodingID == 0)
	{
	    tableFormat = tt_get_ushort(p, ttf);

	    /* we currently do not support cmaps
	    ** other than format 0 and 6 for Macintosh cmaps.
	    */

	    if (tableFormat == 0 && tp->mac == (tt_cmap0_6 *) 0)
	    {
		tp->mac = (tt_cmap0_6 *) p->malloc(p, sizeof (tt_cmap0_6), fn);
		tp->mac->format = 0;
		tt_get_cmap0(p, ttf, tp->mac);
	    }
	    if (tableFormat == 6 && tp->mac == (tt_cmap0_6 *) 0)
	    {
		tp->mac = (tt_cmap0_6 *) p->malloc(p, sizeof (tt_cmap0_6), fn);
		tp->mac->format = 6;
		tt_get_cmap6(p, ttf, tp->mac);
	    }
	}
	else if (platformID == tt_pfid_win &&
		(encodingID == tt_wenc_symbol || encodingID == tt_wenc_text))
	{
	    TT_IOCHECK(p, ttf, tp->win == (tt_cmap4 *) 0);
	    tp->win = (tt_cmap4 *) p->malloc(p, sizeof (tt_cmap4), fn);
	    tt_get_cmap4(p, ttf, tp->win, encodingID);
	}

	tt_seek(p, ttf, pos);
    } /* for */
} /* tt_get_tab_cmap */
 
static void
tt_get_tab_head(PDF *p, tt_file *ttf)
{
    static const char *fn = "tt_get_tab_head";
    tt_tab_head *tp = (tt_tab_head *) p->malloc(p, sizeof (tt_tab_head), fn);
    int		idx = tt_tag2idx(ttf, pdf_str_head);

    ttf->tab_head = tp;
    TT_IOCHECK(p, ttf, idx != -1);
    tt_seek(p, ttf, (long) ttf->dir[idx].offset);

    tp->version			= tt_get_fixed(p, ttf);
    tp->fontRevision		= tt_get_fixed(p, ttf);
    tp->checkSumAdjustment	= tt_get_ulong(p, ttf);
    tp->magicNumber		= tt_get_ulong(p, ttf);
    tp->flags			= tt_get_ushort(p, ttf);
    tp->unitsPerEm		= tt_get_ushort(p, ttf);
    tp->created[1]		= tt_get_ulong(p, ttf);
    tp->created[0]		= tt_get_ulong(p, ttf);
    tp->modified[1]		= tt_get_ulong(p, ttf);
    tp->modified[0]		= tt_get_ulong(p, ttf);
    tp->xMin			= tt_get_fword(p, ttf);
    tp->yMin			= tt_get_fword(p, ttf);
    tp->xMax			= tt_get_fword(p, ttf);
    tp->yMax			= tt_get_fword(p, ttf);
    tp->macStyle		= tt_get_ushort(p, ttf);
    tp->lowestRecPPEM		= tt_get_ushort(p, ttf);
    tp->fontDirectionHint	= tt_get_short(p, ttf);
    tp->indexToLocFormat	= tt_get_short(p, ttf);
    tp->glyphDataFormat		= tt_get_short(p, ttf);
} /* tt_get_tab_head */

static void
tt_get_tab_hhea(PDF *p, tt_file *ttf)
{
    static const char *fn = "tt_get_tab_hhea";
    tt_tab_hhea *tp = (tt_tab_hhea *) p->malloc(p, sizeof (tt_tab_hhea), fn);
    int		idx = tt_tag2idx(ttf, pdf_str_hhea);

    ttf->tab_hhea = tp;
    TT_IOCHECK(p, ttf, idx != -1);
    tt_seek(p, ttf, (long) ttf->dir[idx].offset);

    tp->version			= tt_get_fixed(p, ttf);
    tp->ascender		= tt_get_fword(p, ttf);
    tp->descender		= tt_get_fword(p, ttf);
    tp->lineGap			= tt_get_fword(p, ttf);
    tp->advanceWidthMax		= tt_get_ufword(p, ttf);
    tp->minLeftSideBearing	= tt_get_fword(p, ttf);
    tp->minRightSideBearing	= tt_get_fword(p, ttf);
    tp->xMaxExtent		= tt_get_fword(p, ttf);
    tp->caretSlopeRise		= tt_get_short(p, ttf);
    tp->caretSlopeRun		= tt_get_short(p, ttf);
    tp->res1			= tt_get_short(p, ttf);
    tp->res2			= tt_get_short(p, ttf);
    tp->res3			= tt_get_short(p, ttf);
    tp->res4			= tt_get_short(p, ttf);
    tp->res5			= tt_get_short(p, ttf);
    tp->metricDataFormat	= tt_get_short(p, ttf);
    tp->numberOfHMetrics	= tt_get_ushort(p, ttf);
} /* tt_get_tab_hhea */

static void
tt_get_tab_hmtx(PDF *p, tt_file *ttf)
{
    static const char *fn = "tt_get_tab_hmtx";
    tt_tab_hmtx *tp = (tt_tab_hmtx *) p->malloc(p, sizeof (tt_tab_hmtx), fn);
    int		idx = tt_tag2idx(ttf, pdf_str_hmtx);
    int		n_metrics;
    int		n_lsbs;
    int		i;

    TT_ASSERT(p, ttf, ttf->tab_hhea != 0);
    TT_ASSERT(p, ttf, ttf->tab_maxp != 0);

    /* the instruction order is critical for cleanup after exceptions!
    */
    tp->metrics = 0;
    tp->lsbs = 0;
    ttf->tab_hmtx = tp;

    TT_IOCHECK(p, ttf, idx != -1);
    tt_seek(p, ttf, (long) ttf->dir[idx].offset);

    n_metrics = ttf->tab_hhea->numberOfHMetrics;
    n_lsbs = ttf->tab_maxp->numGlyphs - n_metrics;
    TT_IOCHECK(p, ttf, n_metrics != 0);
    TT_IOCHECK(p, ttf, n_lsbs >= 0);
    tp->metrics = (tt_metric *)
	p->malloc(p, n_metrics * sizeof (tt_metric), fn);

    for (i = 0; i < n_metrics; ++i)
    {
	tp->metrics[i].advanceWidth = tt_get_ufword(p, ttf);
	tp->metrics[i].lsb = tt_get_fword(p, ttf);
    }

    if (n_lsbs == 0)
	tp->lsbs = (tt_fword *) 0;
    else
    {
	tp->lsbs = (tt_fword *) p->malloc(p, n_lsbs * sizeof (tt_fword), fn);
	for (i = 0; i < n_lsbs; ++i)
	    tp->lsbs[i] = tt_get_fword(p, ttf);
    }
} /* tt_get_tab_hmtx */

static void
tt_get_tab_CFF_(PDF *p, tt_file *ttf)
{
    static const char *fn = "tt_get_tab_CFF_";
    int		idx = tt_tag2idx(ttf, pdf_str_CFF_);

    if (idx != -1) {
	/* CFF table found */
	ttf->tab_CFF_ = (tt_tab_CFF_ *) p->malloc(p, sizeof (tt_tab_CFF_), fn);
	ttf->tab_CFF_->offset = ttf->dir[idx].offset;
	ttf->tab_CFF_->length = ttf->dir[idx].length;
    }

} /* tt_get_tab_CFF_ */

static int
tt_gidx2width(PDF *p, tt_file *ttf, int gidx)
{
    TT_ASSERT(p, ttf, ttf->tab_hmtx != (tt_tab_hmtx *) 0);
    TT_ASSERT(p, ttf, ttf->tab_head != (tt_tab_head *) 0);
    TT_ASSERT(p, ttf, ttf->tab_hhea != (tt_tab_hhea *) 0);

    {
	int		n_metrics = ttf->tab_hhea->numberOfHMetrics;
	tt_ushort	unitsPerEm = ttf->tab_head->unitsPerEm;

	if (n_metrics <= gidx)
	    gidx = n_metrics - 1;

/*
	return (int) ROUND(
	(ttf->tab_hmtx->metrics[gidx].advanceWidth * 1000.0f) / unitsPerEm);
*/
	return (int) 
	((ttf->tab_hmtx->metrics[gidx].advanceWidth * 1000L) / unitsPerEm);
    }
} /* tt_gidx2width */


static void
tt_get_tab_maxp(PDF *p, tt_file *ttf)
{
    static const char *fn = "tt_get_tab_maxp";
    tt_tab_maxp *tp = (tt_tab_maxp *) p->malloc(p, sizeof (tt_tab_maxp), fn);
    int		idx = tt_tag2idx(ttf, pdf_str_maxp);

    ttf->tab_maxp = tp;
    TT_IOCHECK(p, ttf, idx != -1);
    tt_seek(p, ttf, (long) ttf->dir[idx].offset);

    tp->version			= tt_get_fixed(p, ttf);
    tp->numGlyphs		= tt_get_ushort(p, ttf);
    tp->maxPoints		= tt_get_ushort(p, ttf);
    tp->maxContours		= tt_get_ushort(p, ttf);
    tp->maxCompositePoints	= tt_get_ushort(p, ttf);
    tp->maxCompositeContours	= tt_get_ushort(p, ttf);
    tp->maxZones		= tt_get_ushort(p, ttf);
    tp->maxTwilightPoints	= tt_get_ushort(p, ttf);
    tp->maxStorage		= tt_get_ushort(p, ttf);
    tp->maxFunctionDefs		= tt_get_ushort(p, ttf);
    tp->maxInstructionDefs	= tt_get_ushort(p, ttf);
    tp->maxStackElements	= tt_get_ushort(p, ttf);
    tp->maxSizeOfInstructions	= tt_get_ushort(p, ttf);
    tp->maxComponentElements	= tt_get_ushort(p, ttf);
    tp->maxComponentDepth	= tt_get_ushort(p, ttf);
} /* tt_get_tab_maxp */

typedef struct
{
    tt_ushort	platform;
    tt_ushort	length;
    tt_ushort	offset;
} tt_nameref;

static void
tt_get_tab_name(PDF *p, tt_file *ttf)
{
    static const char *fn = "tt_get_tab_name";
    tt_tab_name *tp = (tt_tab_name *) p->malloc(p, sizeof (tt_tab_name), fn);
    int		idx = tt_tag2idx(ttf, pdf_str_name);
    int		i;
    tt_nameref	names[TT_NUM_NAMES];
    tt_ulong	offs;

    /* the instruction order is critical for cleanup after exceptions!
    */
    for (i = 0; i < TT_NUM_NAMES; ++i)
    {
	names[i].length = 0;
	tp->names[i] = (char *) 0;
    }

    ttf->tab_name = tp;
    TT_IOCHECK(p, ttf, idx != -1);
    tt_seek(p, ttf, (long) ttf->dir[idx].offset);

    tp->format = tt_get_ushort(p, ttf);
    TT_IOCHECK(p, ttf, tp->format == 0);
    tp->numNameRecords = tt_get_ushort(p, ttf);
    tp->offsetStrings = tt_get_ushort(p, ttf);
    offs = ttf->dir[idx].offset + tp->offsetStrings;

    for (i = 0; i < tp->numNameRecords; ++i)
    {
	tt_ushort platformID	= tt_get_ushort(p, ttf);
	tt_ushort encodingID	= tt_get_ushort(p, ttf);
	tt_ushort languageID	= tt_get_ushort(p, ttf);
	tt_ushort nameID	= tt_get_ushort(p, ttf);
	tt_ushort stringLength	= tt_get_ushort(p, ttf);
	tt_ushort stringOffset	= tt_get_ushort(p, ttf);

	(void)  encodingID;	/* avoid compiler warning */

	/* get the names from platform Win(3) or Mac(1).
	*/
	if (nameID < TT_NUM_NAMES && names[nameID].length == 0)
	{
	    if (platformID == tt_pfid_win && languageID == 0x0409)
	    {
		names[nameID].platform = platformID;
		names[nameID].length = stringLength;
		/* length must be even: */
		TT_IOCHECK(p, ttf, (names[nameID].length & 1) == 0);
		names[nameID].offset = stringOffset;
	    }
	    else if (platformID == tt_pfid_mac)
	    {
		names[nameID].platform = platformID;
		names[nameID].length = stringLength;
		names[nameID].offset = stringOffset;
	    }
	}
    } /* for i */

    for (i = 0; i < TT_NUM_NAMES; ++i)
    {
	/* brain-dead fonts with empty name entries actually exist! */
    	if (names[i].length == 0)
	    continue;

	tp->names[i] = (char *) p->calloc(p, (size_t) names[i].length + 1, fn);
	tt_seek(p, ttf, (long) (offs + names[i].offset));
	tt_read(p, ttf, tp->names[i], names[i].length);

	if (names[i].platform == tt_pfid_win)
	{
	    int k;

	    for (k = 0; k < names[i].length / 2; ++k)
		tp->names[i][k] = tp->names[i][2*k + 1];
	    tp->names[i][k] = 0;
	}
    } /* for i */

    /* If these checks ever fail, we can consider constructing the
    ** missing name 4 from 1+2.
    */
    TT_IOCHECK(p, ttf, tp->names[4] != (char *) 0);
} /* tt_get_tab_name */
 
static void
tt_get_tab_OS_2(PDF *p, tt_file *ttf)
{
    static const char *fn = "tt_get_tab_OS_2";
    tt_tab_OS_2 *tp = (tt_tab_OS_2 *) p->malloc(p, sizeof (tt_tab_OS_2), fn);
    int		idx = tt_tag2idx(ttf, pdf_str_OS_2);

    ttf->tab_OS_2 = tp;
    TT_IOCHECK(p, ttf, idx != -1);
    tt_seek(p, ttf, (long) ttf->dir[idx].offset);

    tp->version = tt_get_ushort(p, ttf);
    tp->xAvgCharWidth = tt_get_short(p, ttf);
    tp->usWeightClass = tt_get_ushort(p, ttf);
    tp->usWidthClass = tt_get_ushort(p, ttf);
    tp->fsType = tt_get_ushort(p, ttf);
    tp->ySubscriptXSize = tt_get_short(p, ttf);
    tp->ySubscriptYSize = tt_get_short(p, ttf);
    tp->ySubscriptXOffset = tt_get_short(p, ttf);
    tp->ySubscriptYOffset = tt_get_short(p, ttf);
    tp->ySuperscriptXSize = tt_get_short(p, ttf);
    tp->ySuperscriptYSize = tt_get_short(p, ttf);
    tp->ySuperscriptXOffset = tt_get_short(p, ttf);
    tp->ySuperscriptYOffset = tt_get_short(p, ttf);
    tp->yStrikeoutSize = tt_get_short(p, ttf);
    tp->yStrikeoutPosition = tt_get_short(p, ttf);
    tp->sFamilyClass = tt_get_ushort(p, ttf);

    tt_read(p, ttf, tp->panose, 10);

    tp->ulUnicodeRange1 = tt_get_ulong(p, ttf);
    tp->ulUnicodeRange2 = tt_get_ulong(p, ttf);
    tp->ulUnicodeRange3 = tt_get_ulong(p, ttf);
    tp->ulUnicodeRange4 = tt_get_ulong(p, ttf);

    tt_read(p, ttf, tp->achVendID, 4);

    tp->fsSelection = tt_get_ushort(p, ttf);
    tp->usFirstCharIndex = tt_get_ushort(p, ttf);
    tp->usLastCharIndex = tt_get_ushort(p, ttf);
    tp->sTypoAscender = tt_get_short(p, ttf);
    tp->sTypoDescender = tt_get_short(p, ttf);
    tp->sTypoLineGap = tt_get_short(p, ttf);
    tp->usWinAscent = tt_get_ushort(p, ttf);
    tp->usWinDescent = tt_get_ushort(p, ttf);

    if (tp->version >= 1)
    {
	tp->ulCodePageRange[0] = tt_get_ulong(p, ttf);
	tp->ulCodePageRange[1] = tt_get_ulong(p, ttf);
    }
    else
    {
	tp->ulCodePageRange[0] = 0;
	tp->ulCodePageRange[1] = 0;
    }

    if (tp->version >= 2)
    {
	tp->sxHeight = tt_get_short(p, ttf);
	tp->sCapHeight = tt_get_short(p, ttf);
	tp->usDefaultChar = tt_get_ushort(p, ttf);
	tp->usBreakChar = tt_get_ushort(p, ttf);
	tp->usMaxContext = tt_get_ushort(p, ttf);
    }
    else
    {
	tp->sxHeight = 0;
	tp->sCapHeight = 0;
	tp->usDefaultChar = 0;
	tp->usBreakChar = 0;
	tp->usMaxContext = 0;
    }
} /* tt_get_tab_OS_2 */

static void
tt_get_tab_post(PDF *p, tt_file *ttf)
{
    static const char *fn = "tt_get_tab_post";
    tt_tab_post *tp = (tt_tab_post *) p->malloc(p, sizeof (tt_tab_post), fn);
    int		idx = tt_tag2idx(ttf, pdf_str_post);

    ttf->tab_post = tp;
    TT_IOCHECK(p, ttf, idx != -1);
    tt_seek(p, ttf, (long) ttf->dir[idx].offset);

    tp->formatType = tt_get_fixed(p, ttf);
    tp->italicAngle = tt_get_fixed(p, ttf) / (float) 65536.0;
    tp->underlinePosition = tt_get_fword(p, ttf);
    tp->underlineThickness = tt_get_fword(p, ttf);
    tp->isFixedPitch = tt_get_ulong(p, ttf);
    tp->minMemType42 = tt_get_ulong(p, ttf);
    tp->maxMemType42 = tt_get_ulong(p, ttf);
    tp->minMemType1 = tt_get_ulong(p, ttf);
    tp->maxMemType1 = tt_get_ulong(p, ttf);
} /* tt_get_tab_post */

static void
pdf_cleanup_tt(PDF *p, tt_file *ttf)
{

    if (ttf->fp != (FILE *) 0)
	fclose(ttf->fp);

    if (ttf->dir != (tt_dirent *) 0)	p->free(p, ttf->dir);

    if (ttf->tab_head != (tt_tab_head *) 0)	p->free(p, ttf->tab_head);
    if (ttf->tab_hhea != (tt_tab_hhea *) 0)	p->free(p, ttf->tab_hhea);
    if (ttf->tab_maxp != (tt_tab_maxp *) 0)	p->free(p, ttf->tab_maxp);
    if (ttf->tab_OS_2 != (tt_tab_OS_2 *) 0)	p->free(p, ttf->tab_OS_2);
    if (ttf->tab_CFF_ != (tt_tab_CFF_ *) 0)	p->free(p, ttf->tab_CFF_);
    if (ttf->tab_post != (tt_tab_post *) 0)	p->free(p, ttf->tab_post);

    if (ttf->tab_cmap != (tt_tab_cmap *) 0)
    {
	if (ttf->tab_cmap->mac != (tt_cmap0_6 *) 0)
	    p->free(p, ttf->tab_cmap->mac);

	if (ttf->tab_cmap->win != (tt_cmap4 *) 0)
	{
	    tt_cmap4 *cm4 = (tt_cmap4 *) ttf->tab_cmap->win;

	    if (cm4->endCount != 0)	p->free(p, cm4->endCount);
	    if (cm4->startCount != 0)	p->free(p, cm4->startCount);
	    if (cm4->idDelta != 0)	p->free(p, cm4->idDelta);
	    if (cm4->idRangeOffs != 0)	p->free(p, cm4->idRangeOffs);
	    if (cm4->glyphIdArray != 0)	p->free(p, cm4->glyphIdArray);

	    p->free(p, cm4);
	}

	p->free(p, ttf->tab_cmap);
    }

    if (ttf->tab_hmtx != (tt_tab_hmtx *) 0)
    {
	if (ttf->tab_hmtx->metrics != (tt_metric *) 0)
	    p->free(p, ttf->tab_hmtx->metrics);
	if (ttf->tab_hmtx->lsbs != (tt_fword *) 0)
	    p->free(p, ttf->tab_hmtx->lsbs);
	p->free(p, ttf->tab_hmtx);
    }

    if (ttf->tab_name != (tt_tab_name *) 0)
    {
	int i;

	for (i = 0; i < TT_NUM_NAMES; ++i)
	    if (ttf->tab_name->names[i] != (char *) 0)
		p->free(p, ttf->tab_name->names[i]);
	p->free(p, ttf->tab_name);
    }

    /* Note: do not clean up ttf->img since the data belongs to font->img
    */

    p->free(p, ttf);

} /* pdf_cleanup_tt */

static void
pdf_init_tt(PDF *p, tt_file *ttf)
{
    int i;

    ttf->dir = (tt_dirent *) 0;

    ttf->tab_cmap = (tt_tab_cmap *) 0;
    ttf->tab_head = (tt_tab_head *) 0;
    ttf->tab_hhea = (tt_tab_hhea *) 0;
    ttf->tab_hmtx = (tt_tab_hmtx *) 0;
    ttf->tab_maxp = (tt_tab_maxp *) 0;
    ttf->tab_name = (tt_tab_name *) 0;
    ttf->tab_post = (tt_tab_post *) 0;
    ttf->tab_OS_2 = (tt_tab_OS_2 *) 0;
    ttf->tab_CFF_ = (tt_tab_CFF_ *) 0;

    (void) tt_get_ulong(p, ttf);		/* version */

    /* set up table directory */
    ttf->n_tables = tt_get_ushort(p, ttf);
    ttf->dir = (tt_dirent *) p->malloc(p,
	(size_t) (ttf->n_tables * sizeof (tt_dirent)), "pdf_init_tt");

    tt_seek(p, ttf, 12);

    for (i = 0; i < ttf->n_tables; ++i)
	tt_get_dirent(p, ttf->dir + i, ttf);

    /* These are all required TrueType tables; optional tables are not read.
    **
    ** There's no error return; pdf_cleanup_tt() is called by all subroutines
    ** before exceptions are thrown. LATER: It is a known bug that p->malloc()
    ** exceptions can't be caught and will lead to resource leaks in this
    ** context!
    */
    tt_get_tab_cmap(p, ttf);
    tt_get_tab_head(p, ttf);
    tt_get_tab_hhea(p, ttf);
    tt_get_tab_maxp(p, ttf);
    tt_get_tab_hmtx(p, ttf);	/* MUST be read AFTER hhea & maxp! */
    tt_get_tab_name(p, ttf);
    tt_get_tab_post(p, ttf);
    tt_get_tab_OS_2(p, ttf);

    /* this is an optional table, present only in OpenType fonts */
    tt_get_tab_CFF_(p, ttf);
} /* pdf_init_tt */

#ifdef PDF_UNUSED

static pdf_bool
tt_supports_cp(char *name, unsigned *cp_range)
{
    typedef struct
    {
	const char *	s;
	int		i;
    } str_int;

    static const str_int n2i[] =
    {
	{ "1250",	TT_CP1250 },
	{ "1251",	TT_CP1251 },
	{ "1252",	TT_CP1252 },
	{ "1253",	TT_CP1253 },
	{ "1254",	TT_CP1254 },
	{ "1255",	TT_CP1255 },
	{ "1256",	TT_CP1256 },
	{ "1257",	TT_CP1257 },
	{ "1258",	TT_CP1258 },
	{ "1361",	TT_CP1361 },
	{ "437",	TT_CP437  },
	{ "708",	TT_CP708  },
	{ "737",	TT_CP737  },
	{ "775",	TT_CP775  },
	{ "850",	TT_CP850  },
	{ "852",	TT_CP852  },
	{ "855",	TT_CP855  },
	{ "857",	TT_CP857  },
	{ "860",	TT_CP860  },
	{ "861",	TT_CP861  },
	{ "862",	TT_CP862  },
	{ "863",	TT_CP863  },
	{ "864",	TT_CP864  },
	{ "865",	TT_CP865  },
	{ "866",	TT_CP866  },
	{ "869",	TT_CP869  },
	{ "874",	TT_CP874  },
	{ "932",	TT_CP932  },
	{ "936",	TT_CP936  },
	{ "949",	TT_CP949  },
	{ "950",	TT_CP950  },
    };

    int lo = 0;
    int hi = (sizeof n2i) / (sizeof (str_int));

    while (lo != hi)
    {
	int idx = (lo + hi) / 2;
	int cmp = strcmp(name, n2i[idx].s);

	if (cmp == 0)
	{
	    int i = n2i[idx].i;

	    return (cp_range[i > 31] & (1 << (i & 0x1F))) != 0;
	}

	if (cmp < 0)
	    hi = idx;
	else
	    lo + idx + 1;
    }

    return pdf_false;
} /* tt_supports_cp */

#endif	/* PDF_UNUSED */


#if defined(PDF_HOSTFONTS_SUPPORTED)
#if defined(WIN32)

static pdf_bool
pdf_get_hostfont(PDF *p, const char *fontname, pdf_font *font)
{
    LOGFONT	lf;
    HFONT 	hf;
    HDC		hdc;
    static const char *fn = "pdf_get_hostfont";

    lf.lfHeight = 0;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = 0;
    lf.lfItalic = 0;
    lf.lfUnderline = 0;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

    if (strlen(fontname) >= 32) {
	if (p->debug['F']) {
	    pdf_error(p, PDF_ValueError,
		"Font name '%s' contains more than 31 characters", fontname);
	} else {
	    return pdf_false;
	}
    }

    strcpy((char *)lf.lfFaceName, fontname);

    if ((hf = CreateFontIndirect(&lf)) == NULL)
	return pdf_false;

    if ((hdc = GetDC(0)) == NULL) {
	DeleteObject(hf);
	return pdf_false;
    }

    SelectObject(hdc, hf);

    font->filelen = GetFontData(hdc, 0, 0, 0, 0);

    if (font->filelen == GDI_ERROR) {
	ReleaseDC(0, hdc);
	DeleteObject(hf);
	return pdf_false;
    }

    font->img = (tt_byte *) p->malloc(p, (size_t) font->filelen, fn);

    if (GetFontData(hdc, 0, 0, font->img, (size_t) font->filelen) ==
	    GDI_ERROR) {
	p->free(p, font->img);
	ReleaseDC(0, hdc);
	DeleteObject(hf);
	return pdf_false;
    }

    ReleaseDC(0, hdc);
    DeleteObject(hf);

    if (font->filelen >= 10)	/* we need 10 bytes for plausibility checks */
    {
#ifdef PDF_HOSTFONT_PS_NYI
/* later we will check for PostScript fonts, too */
#define PS_START "\045\041\120\123"	/* "%!PS" */
	if (font->img[0] == 0x80 && font->img[1] == 0x01 &&
	    strncmp(PS_START, img + 6, 4) == 0)
	    return pdf_true;
#endif 
	/* The TrueType (including OpenType/TT) "version" is always 0x00010000
	*/
	if (font->img[0] == 0x00 && font->img[1] == 0x01 &&
	    font->img[2] == 0x00 && font->img[3] == 0x00)
	    return pdf_true;

	/* The OpenType/CFF version is always 'OTTO' */
	if (font->img[0] == PDF_O && font->img[1] == PDF_T &&
	    font->img[2] == PDF_T && font->img[3] == PDF_O)
	    return pdf_true;
    }

    p->free(p, font->img);
    return pdf_false;
} /* pdf_get_hostfont - WIN32*/

#endif	/* WIN32 */
#endif /* PDF_HOSTFONTS_SUPPORTED */


static pdf_bool
pdf_parse_tt(
    PDF *p,
    const char *filename,
    const char *fontname,	/* only used if filename == NULL */
    pdf_font *font,
    int enc,
    int embed)
{
    static const char *fn = "pdf_parse_tt";
    int		i;
    float	w;
    tt_cmap4	*win;
    tt_cmap0_6	*mac;
    tt_file	*ttf;

#if defined(PDF_HOSTFONTS_SUPPORTED)
#if !defined(WIN32) && !defined(macintosh)
    if(!filename) 			/* host font requested... */
	return pdf_false;		/* ...but no support for host fonts */
#endif
#endif

    ttf = (tt_file *) p->malloc(p, (size_t) sizeof (tt_file), fn); /* LEAK */
    ttf->img = 0;
    ttf->fp = 0;
    ttf->filename = NULL;

    pdf_init_font_struct(p, font);	/* initialize font struct */

    if (filename) {		/* read the font from file */
	ttf->filename = filename;

	if ((ttf->fp = fopen(filename, READMODE)) == NULL) {
	    p->free(p, ttf);

	    if (p->debug['F']) {
		pdf_error(p, PDF_IOError,
		    "Couldn't open font file '%s' for TrueType font", filename);
	    } else
		return pdf_false;
	}

	fseek(ttf->fp, 0L, SEEK_END);
	font->filelen = (size_t) ftell(ttf->fp);
	fseek(ttf->fp, 0L, SEEK_SET);

#if TT_INCORE
	font->img = ttf->img = (tt_byte *) p->malloc(p, font->filelen, fn);
	ttf->pos = ttf->img;
	ttf->end = ttf->img + font->filelen;

	if (font->filelen == 0 ||
	    fread(ttf->img, 1, font->filelen, ttf->fp) != font->filelen) {
	    fclose(ttf->fp);
	    p->free(p, ttf->img);
	    p->free(p, ttf);
	    if (p->debug['F']) {
		pdf_error(p, PDF_IOError,
		    "Couldn't read data from TrueType font file '%s'",
		    filename);
	    } else
		return pdf_false;
	}

	fclose(ttf->fp);
	ttf->fp = 0;
#endif
    }

#if defined(PDF_HOSTFONTS_SUPPORTED)
#if defined(WIN32) || defined(macintosh)
    else {				/* check host fonts */
	if (!pdf_get_hostfont(p, fontname, font)) {
	    p->free(p, ttf);
	    return pdf_false;
	}

	ttf->img = font->img;
	ttf->pos = ttf->img;
	ttf->end = ttf->img + font->filelen;
    }
#endif
#endif /* PDF_HOSTFONTS_SUPPORTED */

    pdf_init_tt(p, ttf);		/* read font file */

    if (ttf->tab_CFF_) {
	font->type = Type1C;
	font->cff_offset = (long) ttf->tab_CFF_->offset;
	font->cff_length = ttf->tab_CFF_->length;
    } else {
	font->type = TrueType;
    }

    /* check if all the tables required by Acrobat are contained in the font.
    */
/*  TT_IOCHECK(p, ttf, tt_tag2idx(ttf, pdf_str_cvt_) != -1);  c0d0gb12 */
/*  TT_IOCHECK(p, ttf, tt_tag2idx(ttf, pdf_str_fpgm) != -1);  c0d0gb12 */
/*  TT_IOCHECK(p, ttf, tt_tag2idx(ttf, pdf_str_prep) != -1);  8940hg-symbol */

    /* These tables are not present in OT fonts */
    if (font->type != Type1C) {
	TT_IOCHECK(p, ttf, tt_tag2idx(ttf, pdf_str_glyf) != -1);
	TT_IOCHECK(p, ttf, tt_tag2idx(ttf, pdf_str_loca) != -1);
    }

    font->numOfChars	= 256;

    /* check whether our font is compatible with the requested encoding */
    win = ttf->tab_cmap->win;
    mac = ttf->tab_cmap->mac;

    if (win == 0 && mac == 0) {
	pdf_cleanup_tt(p, ttf);
	if (p->debug['F']) {
	    pdf_error(p, PDF_IOError,
		"Font '%s' contains unknown encodings only", fontname);
	} else
	    return pdf_false;
    }

    switch (enc) {
	case builtin:
	    if (win != 0) {
		if (win->encodingID == tt_wenc_symbol) {
		    font->flags |= SYMBOL;
		    break;

		} else if (win->encodingID == tt_wenc_text) {
		    enc = winansi;
		    pdf_error(p, PDF_NonfatalError,
		"Using 'winansi' encoding instead of 'builtin' for font '%s'",
			fontname);
		    break;
		}

	    } else if (mac != 0) {
		enc = macroman;
		pdf_error(p, PDF_NonfatalError,
		"Using 'macroman' encoding instead of 'builtin' for font '%s'",
		    fontname);
		break;

	    } else {
		pdf_cleanup_tt(p, ttf);
		if (p->debug['F']) {
		    pdf_error(p, PDF_RuntimeError,
			"Can't use 'builtin' encoding for font '%s'",
			fontname);
		} else
		    return pdf_false;
	    }
	    break;

	case winansi:
	    if (win == 0) {
		pdf_cleanup_tt(p, ttf);
		if (p->debug['F']) {
		    pdf_error(p, PDF_RuntimeError,
		    "Font '%s' doesn't support 'winansi' encoding", fontname);
		} else
		    return pdf_false;
	    }

	    if (win->encodingID == tt_wenc_symbol) {
		font->flags |= SYMBOL;
		enc = builtin;
		pdf_error(p, PDF_NonfatalError,
		"Using 'builtin' encoding instead of 'winansi' for font '%s'",
			fontname);
	    }

	    break;

	case macroman:
	    if (mac == 0) {
		pdf_cleanup_tt(p, ttf);
		if (p->debug['F']) {
		    pdf_error(p, PDF_RuntimeError,
			"Font '%s' doesn't support 'macroman' encoding",
			fontname);
		} else
		    return pdf_false;

	    }
	    break;

	default:
	    /* construct a Unicode-compatible encoding later */
	    if (win == 0) {
		pdf_cleanup_tt(p, ttf);
		if (p->debug['F']) {
		    pdf_error(p, PDF_RuntimeError,
			"Font '%s' doesn't support requested encoding",
			fontname);
		} else
		    return pdf_false;
	    }
	    break;
    } /* switch (enc) */

#define PDF_RESTRICTED_TT_EMBEDDING	0x02
    /* check embedding flags */
    if (embed && ttf->tab_OS_2->fsType == PDF_RESTRICTED_TT_EMBEDDING) {
	pdf_cleanup_tt(p, ttf);
	if (p->debug['F']) {
	    pdf_error(p, PDF_RuntimeError,
		"Cannot embed TrueType font '%s' due to licensing restrictions",
		fontname);
	} else
	    return pdf_false;
    }

    font->encoding	= enc;

    /* The name which is exposed in Windows/Mac OS and at the PDFlib API... */

#if (defined(WIN32) || defined(macintosh)) && defined(PDF_HOSTFONTS_SUPPORTED)

    /* we take the user-supplied name (which may be localized) for host fonts */
    if (!filename) {
	font->name	= pdf_strdup(p, fontname);
    } else {
	/* ...and the name from the font file otherwise */
	font->name	= pdf_strdup(p, ttf->tab_name->names[4]);
    }

#else

    font->name	= pdf_strdup(p, ttf->tab_name->names[4]);

#endif

    if (ttf->tab_name->names[6] == (char *) 0)
	font->ttname	= pdf_strdup(p, ttf->tab_name->names[4]);
    else
	font->ttname	= pdf_strdup(p, ttf->tab_name->names[6]);

#ifdef PDFLIB_EBCDIC
    pdf_make_ebcdic(font->name);
    pdf_make_ebcdic(font->ttname);
#endif

    w			= ttf->tab_OS_2->usWeightClass / (float) 65;
    font->StdVW		= (int) (PDF_STEMV_MIN + w * w);

    font->italicAngle	= (float) ttf->tab_post->italicAngle;
    font->isFixedPitch	= (pdf_bool) ttf->tab_post->isFixedPitch;

#define PDF_TT2PDF(v)	(int) ROUND(v * 1000.0f / ttf->tab_head->unitsPerEm)

    font->llx		= (float) PDF_TT2PDF(ttf->tab_head->xMin);
    font->lly		= (float) PDF_TT2PDF(ttf->tab_head->yMin);
    font->urx		= (float) PDF_TT2PDF(ttf->tab_head->xMax);
    font->ury		= (float) PDF_TT2PDF(ttf->tab_head->yMax);

    font->underlinePosition	= PDF_TT2PDF(ttf->tab_post->underlinePosition);
    font->underlineThickness	= PDF_TT2PDF(ttf->tab_post->underlineThickness);

    if (ttf->tab_OS_2->sCapHeight) {
	font->capHeight	= PDF_TT2PDF(ttf->tab_OS_2->sCapHeight);
    } else {
	font->capHeight	= 666;	/* an educated guess */
    }

    font->xHeight	= PDF_TT2PDF(ttf->tab_OS_2->sxHeight);
    font->ascender	= PDF_TT2PDF(ttf->tab_OS_2->sTypoAscender);
    font->descender	= PDF_TT2PDF(ttf->tab_OS_2->sTypoDescender);

    if (ttf->tab_cmap->win && ttf->tab_cmap->win->encodingID == tt_wenc_text)
	font->encodingScheme = pdf_strdup(p, "AdobeStandardEncoding");
    else
	font->encodingScheme = pdf_strdup(p, "FontSpecific");

    /*
     * If we have a PostScript encoding with no Unicode table attached,
     * convert it now. The Unicode table will survive for use with
     * another font later. Do this only for table-driven encodings,
     * i.e., not builtin.
     */
    if (enc >= 0 && !p->encodings[enc]->codes && !(font->flags & SYMBOL)) {
	p->encodings[enc]->codes = (unsigned short *)
	    p->malloc(p, 256 * sizeof (unsigned short), fn);

	for (i = 0; i < 256; i++) {
	    char *name = p->encodings[enc]->chars[i];
	    p->encodings[enc]->codes[i] =
		(unsigned short) pdf_adobe2unicode(p, name);
	}
    }

    for (i = 0; i < 256; i++) {
	int gidx, uv;

	if (enc == macroman)
	    gidx = tt_code2gidx0_6(p, ttf, i);

    	else if (font->flags & SYMBOL)
	{
	    /* e.g. WingDings has usFirstChar = 0xF020, but we must start
	    ** at 0xF000. I haven't seen non-symbol fonts with a usFirstChar
	    ** like that; perhaps we have to apply similar tricks then...
	    */
	    uv = i + (ttf->tab_OS_2->usFirstCharIndex & 0xFF00);
	    gidx = tt_code2gidx4(p, ttf, uv);
	}
	else
	{
	    /* fetch the Unicode value from the encoding table */
	    uv = (int) p->encodings[enc]->codes[i];
	    gidx = tt_code2gidx4(p, ttf, uv);
	}

	/* if there's no glyph for this code
	** this is the width of the "missingGlyph".
	*/
	font->widths[i] = tt_gidx2width(p, ttf, gidx);
    }

    /* LATER: raise an exception if no character was found
    if (enc != macroman && enc != winansi && no_character_found) {
	pdf_cleanup_tt(p, ttf);
	if (p->debug['F']) {
	    pdf_error(p, PDF_RuntimeError,
		"Font '%s' can't be used with encoding '%s'",
		font->ttname, p->encodings[enc].apiname);
	} else
	    return pdf_false;
    }
    */

    pdf_cleanup_tt(p, ttf);

    return pdf_true;
} /* pdf_parse_tt */

pdf_bool
pdf_get_metrics_tt(
    PDF *p,
    pdf_font *font,
    const char *fontname,
    int enc,
    const char *filename,
    int embed)
{
    /* parse TrueType file */
    if (pdf_parse_tt(p, filename, fontname, font, enc, embed) == pdf_false) {
	if (filename) {
	    if (p->debug['F'])
		pdf_error(p, PDF_RuntimeError,
		    "Error parsing TrueType font '%s'", fontname);
	    else
		return pdf_false;
	} else {
	    /* looking for host font -- silently ignore error */
	    return pdf_false;
	}
    }

#ifdef NYI
    /* validate the font name if read from file */
    if (filename && strcmp(font->name, fontname)) {
	if (p->debug['F']) {
	    pdf_error(p, PDF_ValueError,
		"Name mismatch in TrueType font (use '%s' instead of '%s')",
		font->ttname, fontname);
	} else
	    return pdf_false;
    }
#endif

    pdf_make_fontflag(p, font);

    return pdf_true;
} /* pdf_get_metrics_tt */

void
pdf_put_ttfont(PDF *p, int slot)
{
    int		i, j;
    id		encoding_id = BAD_ID, fontdescriptor_id, fontfile_id;
    id		length_id, length1_id = BAD_ID;
    long	length, stream_start;
    pdf_font	*font;
#ifdef PDFLIB_EBCDIC
    char	*afontname;
#endif

    font	= &p->fonts[slot];

    pdf_begin_obj(p, font->obj_id);		/* font attributes	*/
    pdf_begin_dict(p);
    pdf_puts(p, "/Type/Font\n");

    if (font->type == Type1C)
	pdf_puts(p, "/Subtype/Type1\n");	/* NOT /Type1C !!! */
    else
	pdf_puts(p, "/Subtype/TrueType\n");

    pdf_puts(p, "/FirstChar 0\n");
    pdf_puts(p, "/LastChar 255\n");

    pdf_puts(p, "/Widths[\n");

    for (i = 0; i < 16; i++) {
	for (j = 0; j < 16; j++)
	    pdf_printf(p, " %d", font->widths[16*i + j]);
	pdf_putc(p, PDF_NEWLINE);
    }

    pdf_puts(p, "]\n");

    if (font->encoding != builtin) {
	if (p->encodings[font->encoding]->pdfname)
	    pdf_printf(p, "/Encoding/%s\n",
	    	p->encodings[font->encoding]->pdfname);
	else {
	    encoding_id = pdf_alloc_id(p);
	    pdf_printf(p, "/Encoding %ld 0 R\n", encoding_id);
	}
    }

    /* TrueType fonts use a different name at the API level than in the PDF */
    pdf_puts(p, "/BaseFont/");
#ifndef PDFLIB_EBCDIC
    pdf_quote_name(p, font->ttname, strlen(font->ttname));
#else
    afontname = pdf_strdup(p, font->ttname);
    pdf_make_ascii(afontname);
    pdf_quote_name(p, afontname, strlen(afontname));
#endif
    pdf_puts(p, "\n");

    fontdescriptor_id = pdf_alloc_id(p);
    pdf_printf(p, "/FontDescriptor %ld 0 R\n", fontdescriptor_id);

    pdf_end_dict(p);
    pdf_end_obj(p);			/* font attributes */

    if (font->embed == 1)
	fontfile_id = pdf_alloc_id(p);
    else
	fontfile_id = BAD_ID;

    pdf_write_fontdescriptor(p, font, fontdescriptor_id, fontfile_id);

    if (font->encoding != builtin && !p->encodings[font->encoding]->pdfname)
	pdf_write_encoding(p, p->encodings[font->encoding], encoding_id);

    /* embed TrueType font file */
    if (font->embed == 1) {
	PDF_data_source src;

	pdf_begin_obj(p, fontfile_id);
	pdf_begin_dict(p);

	length_id = pdf_alloc_id(p);
	pdf_printf(p, "/Length %ld 0 R\n", length_id);

	if (font->type == Type1C)
	    pdf_puts(p, "/Subtype/Type1C\n");

	if (font->type == TrueType) {
	    length1_id = pdf_alloc_id(p);
	    pdf_printf(p, "/Length1 %ld 0 R\n", length1_id);
	}

	if (p->compresslevel && font->filelen != 0L)
	    pdf_puts(p, "/Filter/FlateDecode\n");

	pdf_end_dict(p);

	pdf_begin_stream(p);			/* font data stream */
	stream_start = pdf_tell(p);

	/* write the font file to the PDF output */
	src.private_data = (void *) font->fontfilename;

	if (font->fontfilename) {
	    /* read the font from file */
	    src.init        = pdf_data_source_file_init;
	    src.fill        = pdf_data_source_file_fill;
	    src.terminate   = pdf_data_source_file_terminate;

	    if (font->type == TrueType) {
		src.offset      = (long) 0;
		src.length      = (long) 0;	/* the complete file */

	    } else {	/* font->type == Type1C */

		src.offset      = font->cff_offset;
		src.length      = (long) font->cff_length;
	    }

	} else {

	    /* read the font from memory */
	    src.init        = pdf_noop;
	    src.fill        = pdf_data_source_buf_fill;
	    src.terminate   = pdf_noop;

	    if (font->type == TrueType) {
		src.buffer_start = font->img;
		src.buffer_length = font->filelen;

	    } else {	/* font->type == Type1C */

		src.buffer_start = font->img + font->cff_offset;
		src.buffer_length = font->cff_length;
	    }

	    src.bytes_available = 0;
	    src.next_byte	= NULL;
	}

	if (p->compresslevel && font->filelen != 0L)
	    pdf_compress(p, &src);
	else
	    pdf_copy(p, &src);

	length = pdf_tell(p) - stream_start;
	pdf_end_stream(p);
	pdf_end_obj(p);				/* font file object */

	pdf_begin_obj(p, length_id);		/* Length object */
	pdf_printf(p, "%ld\n", length);
	pdf_end_obj(p);

	if (font->type == TrueType) {	/* not for Type1C fonts */
	    pdf_begin_obj(p, length1_id);		/* Length1 object */
	    pdf_printf(p, "%ld\n",
		(font->filelen != 0) ? font->filelen : length);
	    pdf_end_obj(p);
	}
    }

    if (p->flush & PDF_FLUSH_CONTENT)
	pdf_flush_stream(p);
} /* pdf_put_ttfont */

#endif /* PDF_TRUETYPE_SUPPORTED */
