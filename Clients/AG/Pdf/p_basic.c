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

/* $Id: p_basic.c,v 1.61.2.19 2002/01/07 18:26:29 tm Exp $
 *
 * PDFlib general routines
 *
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifndef WINCE
#include <time.h>
#include "p_intern.h"
#else
#include "p_intern.h"
#include <winbase.h>
#endif

#include "p_font.h"
#include "p_md5.h"

#if !defined(WIN32) && !defined(OS2)
#include <unistd.h>
#endif

#if (defined(WIN32) || defined(OS2)) && !defined(WINCE)
#include <fcntl.h>
#include <io.h>
#endif

#if defined(MAC) && defined(__MWERKS__)
#include <Files.h>
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#undef WIN32_LEAN_AND_MEAN
#endif

PDFLIB_API void PDFLIB_CALL
PDF_boot(void)
{
    PDF_TRACE(("PDF_boot\t\t();\n"));

    /* nothing yet */
}

PDFLIB_API void PDFLIB_CALL
PDF_shutdown(void)
{
    PDF_TRACE(("PDF_shutdown\t();\n"));

    /* nothing yet */
}

#if (defined(WIN32) || defined(__CYGWIN)) && defined(PDFLIB_EXPORTS)

/*
 * DLL entry function as required by Visual C++.
 * It is currently not necessary on Windows, but will eventually 
 * be used to boot thread-global resources for PDFlib
 * (mainly font-related stuff).
 */
BOOL WINAPI
DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    (void) hModule;
    (void) lpReserved;

    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	    PDF_boot();
	    break;
	case DLL_THREAD_ATTACH:
	    break;
	case DLL_THREAD_DETACH:
	    break;
	case DLL_PROCESS_DETACH:
	    PDF_shutdown();
	    break;
    }

    return TRUE;
}
#endif	/* WIN32 && PDFLIB_EXPORT */

PDFLIB_API int PDFLIB_CALL
PDF_get_majorversion(void)
{
    PDF_TRACE(("PDF_get_majorversion\t(); [%s]\n", PDFLIB_VERSIONSTRING));

    return PDFLIB_MAJORVERSION;
}

PDFLIB_API int PDFLIB_CALL
PDF_get_minorversion(void)
{
    PDF_TRACE(("PDF_get_minorversion\t(); [%s]\n", PDFLIB_VERSIONSTRING));

    return PDFLIB_MINORVERSION;
}

/* This list must be kept in sync with the corresponding #defines in pdflib.h */
static const char *pdf_error_names[] = {
    NULL,
    "memory error",
    "I/O error",
    "runtime error",
    "index error",
    "type error",
    "division by zero error",
    "overflow error",
    "syntax error",
    "value error",
    "system error",
    "warning (ignored)",
    "unknown error"
};

/* The default error handler for C and C++ clients */
static void
pdf_c_errorhandler(PDF *p, int type, const char* shortmsg)
{
    char msg[2048];

    /*
     * A line like this allows custom error handlers to supply their 
     * own program name
     */
    sprintf(msg, "PDFlib %s: %s\n", pdf_error_names[type], shortmsg);

    switch (type) {
	/* Issue a warning message and continue */
	case PDF_NonfatalError:
	    (void) fprintf(stderr, msg);
	    return;

	/* give up in all other cases */
	case PDF_MemoryError:
	case PDF_IOError:
	case PDF_RuntimeError:
	case PDF_IndexError:
	case PDF_TypeError:
	case PDF_DivisionByZero:
	case PDF_OverflowError:
	case PDF_SyntaxError:
	case PDF_ValueError:
	case PDF_SystemError:
	case PDF_UnknownError:
	default:
	    (void) fprintf(stderr, msg);	/* print message */
	    PDF_delete(p);			/* clean up PDFlib */
	    exit(99);				/* good-bye */
    }
}

void
pdf_error(PDF *p, int type, const char *fmt, ...)
{
    static const char fn[] = "pdf_error";
    char msg[2048];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(msg, fmt, ap);

    PDF_TRACE(("\n[%s\t(pdf(%p), %d (%s), \"%s\");]\n",
	fn, (void *) p, type, pdf_error_names[type], msg));

    /*
     * We catch non-fatals here since user-supplied error handlers
     * don't know about the debug level.
     */
    if (PDF_GET_STATE(p) != pdf_state_error	/* avoid recursive errors */
	&& (type != PDF_NonfatalError || ((PDF *)p)->debug['w']))
    {
	PDF_PUSH_STATE(p, fn, pdf_state_error);
	(p->errorhandler)(p, type, msg);

	/* If the error handler returns the error was non-fatal */
	PDF_POP_STATE(p, fn);
    }

    PDF_TRACE(("[error handler returned]\n"));
    va_end(ap);
}

#ifdef DEBUG_TRACE

/* Include informational stuff in [] brackets, and use %s/\[[^]]*\]//g
 * to get rid of it :-)
 */

void
pdf_trace(const char *fmt, ...)
{
    FILE *fp;
    const char *filename = DEBUG_TRACE_FILE;
#ifndef WINCE
    time_t	timer;
    struct tm	ltime;
#else
    SYSTEMTIME  st;
#endif
    va_list ap;

    if ((fp = fopen(filename, APPENDMODE)) == NULL)
	return;

#ifndef WINCE
    time(&timer);
    ltime = *localtime(&timer);
    fprintf(fp, "[%04d/%02d/%02d %02d:%02d:%02d] ",
	ltime.tm_year + 1900, ltime.tm_mon + 1, ltime.tm_mday,
	ltime.tm_hour, ltime.tm_min, ltime.tm_sec);

#else

    GetLocalTime (&st);
    fprintf(fp, "[%04d/%02d/%02d %02d:%02d:%02d] ",
	 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
#endif	/* !WINCE */

    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    fclose(fp);

}
#endif

const char *
pdf_current_scope(PDF *p)
{
    static const char *scope_names[] =
    {
	"object",
	"document",
	"page",
	"pattern",
	"template",
	"path"
    };

    int i;

    for (i = 0; i < 6; ++i)
	if (PDF_GET_STATE(p) == (1 << i))
	    return scope_names[i];

    pdf_error(p, PDF_SystemError, "illegal state 0x%08X", PDF_GET_STATE(p));

    return (char *) 0;	/* be happy, compiler! */
}

void
pdf_scope_error(PDF *p, const char *funame)
{
    if (PDF_GET_STATE(p) == pdf_state_error)
	return;

    pdf_error(p, PDF_RuntimeError,
	"function '%s' must not be called in '%s' scope",
					    funame, pdf_current_scope(p));
}

static void
pdf_init_document(PDF *p)
{
    static const char *fn = "pdf_init_document";
    id i;

    p->file_offset_capacity = ID_CHUNKSIZE;
    p->file_offset = (long *) p->malloc(p, 
	    sizeof(long) * p->file_offset_capacity, fn);

    p->contents_ids_capacity = CONTENTS_CHUNKSIZE;
    p->contents_ids = (id *) p->malloc(p, 
	    sizeof(id) * p->contents_ids_capacity, fn);

    p->pages_capacity = PAGES_CHUNKSIZE;
    p->pages = (id *) p->malloc(p,
	    sizeof(id) * p->pages_capacity, fn);

    /* mark ids to allow for pre-allocation of page ids */
    for (i = 0; i < p->pages_capacity; i++)
	p->pages[i] = BAD_ID;

    p->currentobj	= (id) 0;

    p->pnodes_capacity = PNODES_CHUNKSIZE;
    p->pnodes = (id *) p->malloc(p, sizeof(id) * p->pnodes_capacity, fn);

    p->pnodes[0]	= pdf_alloc_id(p);
    p->current_pnode	= 0;
    p->current_pnode_kids = 0;

    p->filename		= NULL;
    p->fp		= NULL;
    p->current_page	= 0;
    p->root_id		= pdf_alloc_id(p);
    p->open_action	= retain;
    p->open_mode	= open_auto;
    p->bookmark_dest	= retain;
    p->base		= NULL;

    p->md5 = p->malloc(p, sizeof(MD5_CTX), fn);
    MD5_Init(p->md5);

    PDF_SET_STATE(p, fn, pdf_state_document);
}

/* Free all page-related resources */
static void
pdf_cleanup_page(PDF *p)
{
    pdf_cleanup_page_annots(p);
}

/* p may be NULL on the first call - we don't use it anyway */
static void *
pdf_malloc(PDF *p, size_t size, const char *caller)
{
    void *ret;

    /* the behavior of malloc(0) is undefined in ANSI C, and may
     * result in a NULL pointer return value which makes PDFlib bail out.
     */
    if (size == (size_t) 0) {
	size = (size_t) 1;
	if (p != NULL) {
	    pdf_error(p, PDF_NonfatalError,
		    "Tried to allocate 0 bytes in %s!\n", caller);
	}
    }

    ret = malloc(size);

#ifdef DEBUG
    if (p != NULL && p->debug['m'])
	fprintf(stderr, "%x malloced, size %d from %s, page %d\n",
		(int)ret, size, caller, p->current_page);
#endif

    /* Special error handling at first allocation (for p itself) */
    if (p != NULL) {
	if (ret == NULL) {
	    pdf_error(p, PDF_MemoryError,
		    "Couldn't allocate memory in %s!\n", caller);
	}
    }

    return ret;
}

/* We cook up our own calloc routine, using the caller-supplied 
 * malloc and memset.
 */
static void *
pdf_calloc(PDF *p, size_t size, const char *caller)
{
    void *ret;

    /* see pdf_malloc() */
    if (size == (size_t) 0) {
	size = (size_t) 1;
	if (p != NULL) {
	    pdf_error(p, PDF_NonfatalError,
		    "Tried to (c)allocate 0 bytes in %s!\n", caller);
	}
    }

    if ((ret = p->malloc(p, size, caller)) == NULL) {
	pdf_error(p, PDF_MemoryError,
		"Couldn't (c)allocate memory in %s!\n", caller);
    }
    memset(ret, 0, size);

#ifdef DEBUG
    if (p->debug['c'])
	fprintf(stderr, "%x calloced, size %d from %s, page %d\n",
		(int) ret, size, caller, p->current_page);
#endif

    return ret;
}

static void *
pdf_realloc(PDF *p, void *mem, size_t size, const char *caller)
{
    void *ret;

    if ((ret = realloc(mem, size)) == NULL) {
	pdf_error(p, PDF_MemoryError,
		"Couldn't reallocate memory in %s!\n", caller);
    }

#ifdef DEBUG
    if (p->debug['r'])
	fprintf(stderr, "%x realloced to %x, %d from %s, page %d\n",
		(int) mem, (int) ret, (int) size, caller, p->current_page);
#endif
    return ret;
}

static void
pdf_free(PDF *p, void *mem)
{
#ifdef DEBUG
    if (p->debug['f'])
	fprintf(stderr, "%x freed, page %d\n", (int) mem, p->current_page);
#endif

    /* We mustn't raise a fatal error here to avoid potential recursion */
    if (mem == NULL) {
	pdf_error(p, PDF_NonfatalError,
		"(Internal) Tried to free null pointer");
	return;
    }

    free(mem);
}

static size_t
pdf_writeproc_file(PDF *p, void *data, size_t size)
{
    return fwrite(data, 1, (size_t) size, p->fp);
}

/* This is the easy version with the default handlers */
PDFLIB_API PDF * PDFLIB_CALL
PDF_new(void)
{
    return PDF_new2(pdf_c_errorhandler, NULL, NULL, NULL, NULL);
}

/* This is the spiced-up version with user-defined error and memory handlers */

PDFLIB_API PDF * PDFLIB_CALL
PDF_new2(
    void  (*errorhandler)(PDF *p, int type, const char *msg),
    void* (*allocproc)(PDF *p, size_t size, const char *caller),
    void* (*reallocproc)(PDF *p, void *mem, size_t size, const char *caller),
    void  (*freeproc)(PDF *p, void *mem),
    void   *opaque)
{
    PDF *p;

    PDF_TRACE(("[ ------------------------------------------------------ ]\n"));
#ifdef HAVE_PDI
    PDF_TRACE(("[ PDFlib %s + PDI on %s ]\n",
	PDFLIB_VERSIONSTRING, PDF_PLATFORM));
#else
    PDF_TRACE(("[ PDFlib %s on %s ]\n", PDFLIB_VERSIONSTRING, PDF_PLATFORM));
#endif
    PDF_TRACE(("[ ------------------------------------------------------ ]\n"));
    PDF_TRACE(("PDF_new2\t\t(eh[%p], ap[%p], rp[%p], fp[%p], op[%p]);",
	(void *) errorhandler, (void *) allocproc, (void *) reallocproc,
	(void *) freeproc, opaque));

    if (errorhandler == NULL)
	errorhandler = pdf_c_errorhandler;

    /* If allocproc is NULL, all entries are supplied internally by PDFlib */
    if (allocproc == NULL) {
	allocproc	= pdf_malloc;
	reallocproc	= pdf_realloc;
	freeproc	= pdf_free;
    }

    p = (PDF *) (*allocproc) (NULL, sizeof(PDF), "PDF_new");

    if (p == NULL)
    {
	PDF pdf;	/* dummy PDF struct for error handler. */

	/* let PDF_delete() know that there's nothing to do. */
	pdf.errorhandler = NULL;

	/* these two are required by PDF_get_opaque() */
	pdf.magic = PDF_MAGIC;
	pdf.opaque = opaque;

	(*errorhandler)(&pdf, PDF_MemoryError,
	    "Couldn't allocate PDFlib object");

	return NULL;	/* Won't happen because of the error handler */
    }

    /*
     * Guard against crashes when PDF_delete is called without any
     * PDF_open_*() in between.
     */
    memset((void *)p, 0, (size_t) sizeof(PDF));

    p->magic		= PDF_MAGIC;
    p->compatibility	= PDF_1_3;
    p->errorhandler	= errorhandler;
    p->malloc		= allocproc;
    p->realloc		= reallocproc;
    p->free		= freeproc;

    p->flush		= PDF_FLUSH_PAGE;
    p->writeproc 	= NULL;
    p->floatdigits	= 4;

    p->opaque		= opaque;
    p->calloc		= pdf_calloc;

    p->compresslevel	= PDF_DEFAULT_COMPRESSION;
    p->inheritgs	= pdf_true;

    p->resourcefilename	= NULL;
    p->resources	= NULL;
    p->resourcefile_loaded = pdf_false;
    p->filename		= NULL;
    p->binding		= NULL;
    p->prefix		= NULL;
    p->xobjects		= NULL;
    p->state_sp		= 0;
    PDF_SET_STATE(p, "PDF_new", pdf_state_object);

    /* all debug flags are cleared by default because of the above memset... */

    /* ...but warning messages for non-fatal errors should be set,
     * as well as font warnings -- the client must explicitly disable these.
     */
    p->debug['w'] = pdf_true;
    p->debug['F'] = pdf_true;

#ifdef DEBUG
    /* a debug build will trace by default, but the client may disable it */
    p->debug['t'] = pdf_true;
#endif

#ifdef HAVE_PDI
    /* The PDI chain survives all PDF documents */
    pdf_init_pdi(p);
#endif
    pdf_boot_stream(p);


    PDF_TRACE((" [%p]\n", p));

    return p;
}

PDFLIB_API void * PDFLIB_CALL
PDF_get_opaque(PDF *p)
{
    PDF_TRACE(("PDF_get_opaque\t(pdf[%p]);", (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return ((void *) NULL);

    PDF_TRACE((" [%p]\n", p->opaque));
    return p->opaque;
}

/* close the output file, if opened with PDF_open_file();
 * close the output stream if opened 
 */
static void
pdf_close_file(PDF *p)
{
#if defined(MAC) && defined(__MWERKS__)
    FCBPBRec	fcbInfo;
    Str32	name;
    FInfo	fInfo;
    FSSpec	fSpec;
#endif

    if (p->fp == NULL)
	return;

#if defined(MAC) && defined(__MWERKS__)
/*  Contributed by Leonard Rosenthol:
 *  On the MacOS, files are not associated with applications by extensions.
 *  Instead, it uses a pair of values called the type & creator.
 *  This block of code sets those values for PDF files.
 */
    memset(&fcbInfo, 0, sizeof(FCBPBRec));
    fcbInfo.ioRefNum = (short) p->fp->handle;
    fcbInfo.ioNamePtr = name;
    if (!PBGetFCBInfoSync(&fcbInfo) &&
	FSMakeFSSpec(fcbInfo.ioFCBVRefNum, fcbInfo.ioFCBParID, name, &fSpec)
		== noErr) {
	    FSpGetFInfo(&fSpec, &fInfo);
	    fInfo.fdType = 'PDF ';
	    fInfo.fdCreator = 'CARO';
	    FSpSetFInfo(&fSpec, &fInfo);
	}
#endif

    /*
     * If filename is set, we started with PDF_open_file; therefore
     * we also close the file ourselves.
     */
    if (p->filename && p->writeproc) {
	if (strcmp(p->filename, "-"))
	    fclose(p->fp);
	p->free(p, p->filename);
    }

    /* mark fp as dead in case the error handler jumps in later */
    p->fp = NULL;
}

static void pdf_cleanup_document(PDF *p);

/* 
 * PDF_delete must be called for cleanup in case of error,
 * or when the client is done producing PDF.
 * It should never be called more than once for a given PDF, although
 * we try to guard against duplicated calls.
 *
 * Note: all pdf_cleanup_*() functions may safely be called multiple times.
 */

PDFLIB_API void PDFLIB_CALL
PDF_delete(PDF *p)
{
    static const char fn[] = "PDF_delete";

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->errorhandler == NULL)
	return;	/* nothing to do with dummy PDF struct (see PDF_new2()) */

    PDF_CHECK_SCOPE(p, fn, pdf_state_object);

    /* check whether the caller forgot to fetch the last chunk of data */
    if (PDF_GET_STATE(p) != pdf_state_error && pdf_buffer_not_empty(p)) {
	    pdf_error(p, PDF_RuntimeError,
		"Must call PDF_get_buffer() after PDF_close()");
    }

    /*
     * Clean up page-related stuff if necessary. Do not raise
     * an error here since we may be called from the error handler.
     */
    if (PDF_GET_STATE(p) != pdf_state_object) {
	pdf_cleanup_document(p);
    }

    /* close the output stream.
     * This can't be done in PDF_close() because the caller may fetch
     * the buffer only after PDF_close()ing the document.
     */
    pdf_close_stream(p);

    pdf_cleanup_resources(p);		/* release the resources tree */

#ifdef HAVE_PDI
    pdf_cleanup_pdi(p);
#endif

    if (p->binding)
	p->free(p, p->binding);

    if (p->prefix)
	p->free(p, p->prefix);

    /* free the PDF structure and try to protect against duplicated calls */

    p->magic = 0L;		/* we don't reach this with the wrong magic */
    p->free(p, (void *)p);
}

static void
pdf_init_all(PDF *p)
{
    pdf_init_document(p);
    pdf_init_info(p);
    pdf_init_images(p);
    pdf_init_xobjects(p);
    pdf_init_fonts(p);
    pdf_init_transition(p);
    pdf_init_outlines(p);
    pdf_init_annots(p);
    pdf_init_stream(p);
    pdf_init_colorspaces(p);
    pdf_init_pattern(p);

    /* clients may set char/word spacing and horizontal scaling outside pages
    ** for PDF_stringwidth() calculations, and they may set a color for use
    ** in PDF_makespotcolor(). here we set the defaults.
    */
    p->sl = 0;			
    pdf_init_tstate(p);
    pdf_init_cstate(p);

#ifdef HAVE_PDI
    pdf_reset_pdi(p);		/* reset all PDI contexts */
#endif
}

/* Must be defined octal to protect it from EBCDIC compilers */
#define PDF_MAGIC_BINARY "\045\344\343\317\322\012"

static void
pdf_write_header(PDF *p)
{
    /* Write the document header */
    /*
     * Although the %PDF-1.3 header also works with older Acrobat viewers,
     * we need a 1.2 header for special applications (controlled by the
     * client).
    */

    if (p->compatibility == PDF_1_4)
	pdf_puts(p, "%PDF-1.4\n");	      	/* PDF 1.4 header */
    else if (p->compatibility == PDF_1_3)
	pdf_puts(p, "%PDF-1.3\n");	      	/* PDF 1.3 header */
    else
	pdf_puts(p, "%PDF-1.2\n");	      	/* PDF 1.2 header */

    /* binary magic number */
    pdf_write(p, PDF_MAGIC_BINARY, sizeof(PDF_MAGIC_BINARY) - 1);
}

PDFLIB_API int PDFLIB_CALL
PDF_open_fp(PDF *p, FILE *fp)
{
    static const char fn[] = "PDF_open_fp";

    PDF_TRACE(("%s\t(pdf[%p], fp[%p]);", fn, (void *) p, (void *) fp));

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    PDF_CHECK_SCOPE(p, fn, pdf_state_object);

    if (fp == NULL) {
	PDF_TRACE((" [%d]\n", -1));
	return -1;
    }

/*
 * It is the callers responsibility to open the file in binary mode,
 * but it doesn't hurt to make sure it really is.
 * The Intel version of the Metrowerks compiler doesn't have setmode().
 */
#if !defined(__MWERKS__) && (defined(WIN32) || defined(OS2))
#if defined WINCE
    _setmode(fileno(fp), _O_BINARY);
#else
    _setmode(_fileno(fp), O_BINARY);
#endif
#endif

    pdf_init_all(p);
    p->writeproc	= pdf_writeproc_file;
    p->fp		= fp;
    p->filename		= NULL;	/* marker to remember not to close fp */

    pdf_write_header(p);

    PDF_TRACE((" [%d]\n", pdf_true));
    return pdf_true;
}

PDFLIB_API int PDFLIB_CALL
PDF_open_file(PDF *p, const char *filename)
{
    static const char fn[] = "PDF_open_file";

    PDF_TRACE(("%s\t(pdf[%p], \"%s\");", fn, (void *) p, filename));

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    PDF_CHECK_SCOPE(p, fn, pdf_state_object);

    pdf_init_all(p);

    if (filename == NULL || *filename == '\0') {
	/* no file name supplied ==> in-core PDF generation requested */
	p->writeproc = NULL;
	p->filename = NULL;

    } else {
#if !(defined(MAC) && defined(__MWERKS__))
	if (filename && !strcmp(filename, "-")) {
	    p->filename = NULL;
	    p->fp = stdout;
#if !defined(__MWERKS__) && (defined(WIN32) || defined(OS2))
#if defined WINCE
	    _setmode(fileno(stdout), _O_BINARY);
#else
	    _setmode(_fileno(stdout), O_BINARY);
#endif /* !WINCE */
#endif
	} else
#endif /* MAC */

	if ((p->fp = fopen(filename, WRITEMODE)) == NULL) {
	    PDF_SET_STATE(p, "PDF_open_file", pdf_state_object);
	    PDF_TRACE((" [%d]\n", -1));
	    return -1;
	}

	p->writeproc = pdf_writeproc_file;
	p->filename = pdf_strdup(p, filename);
    }

    pdf_write_header(p);

    PDF_TRACE((" [%d]\n", pdf_true));
    return pdf_true;
}

PDFLIB_API void PDFLIB_CALL
PDF_open_mem(PDF *p, size_t (*writeproc)(PDF *p, void *data, size_t size))
{
    static const char fn[] = "PDF_open_mem";

    PDF_TRACE(("%s\t(pdf[%p], wp[%p]);\n", fn, (void *) p, (void *) writeproc));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_object);

    if (writeproc == NULL)
	pdf_error(p, PDF_ValueError, "NULL write procedure in PDF_open_mem()");
	
    p->writeproc = writeproc;
    p->filename = NULL;

    pdf_init_all(p);
    pdf_write_header(p);
}

#define N_KIDS	10

static id
pdf_get_pnode_id(PDF *p)
{
    static const char fn[] = "pdf_get_pnode_id";

    if (p->current_pnode_kids == N_KIDS)
    {
	if (++p->current_pnode == p->pnodes_capacity)
	{
	    p->pnodes_capacity *= 2;
	    p->pnodes = (id *) p->realloc(p, p->pnodes,
			sizeof (id) * p->pnodes_capacity, fn);
	}

	p->pnodes[p->current_pnode] = pdf_alloc_id(p);
	p->current_pnode_kids = 1;
    }
    else
	++p->current_pnode_kids;

    return p->pnodes[p->current_pnode];
}

static void
pdf_write_pnode(PDF *p,
	id node_id,
	id parent_id,
	id *kids,
	int n_kids,
	int n_pages)
{
    pdf_begin_obj(p, node_id);
    pdf_begin_dict(p);
    pdf_puts(p, "/Type/Pages\n");
    pdf_printf(p, "/Count %d\n", n_pages);

    if (parent_id != BAD_ID)
	pdf_printf(p, "/Parent %d 0 R\n", parent_id);

    pdf_puts(p, "/Kids[");

    do
    {
	pdf_printf(p, "%ld 0 R", *kids++);
	pdf_putc(p, PDF_NEWLINE);
    } while (--n_kids > 0);

    pdf_puts(p, "]");
    pdf_end_dict(p);
    pdf_end_obj(p);
}

static id
pdf_make_tree(PDF *p, id parent_id, id *pnodes, id *pages, int n_pages)
{
    if (n_pages <= N_KIDS)
    {
	/* this is a near-to-leaf node. use the pre-allocated id
	** from p->pnodes.
	*/
	pdf_write_pnode(p, *pnodes, parent_id, pages, n_pages, n_pages);
	return *pnodes;
    }
    else
    {
	id node_id = pdf_alloc_id(p);
	id kids[N_KIDS];
	int n_kids, rest;
	int pow = N_KIDS;
	int i;

	/* pow < n_pages <= pow*N_KIDS
	*/
	while (pow * N_KIDS < n_pages)
	    pow *= N_KIDS;

	n_kids = n_pages / pow;
	rest = n_pages % pow;

	for (i = 0; i < n_kids; ++i, pnodes += pow / N_KIDS, pages += pow)
	{
	    kids[i] = pdf_make_tree(p, node_id, pnodes, pages, pow);
	}

	if (rest)
	{
	    kids[i] = pdf_make_tree(p, node_id, pnodes, pages, rest);
	    ++n_kids;
	}

	pdf_write_pnode(p, node_id, parent_id, kids, n_kids, n_pages);
	return node_id;
    }
}

/* Write all pending document information up to the xref table and trailer */
static void
pdf_wrapup_document(PDF *p)
{
    static const char bin2hex[] = PDF_STRING_0123456789ABCDEF;

    long	pos;
    id		i;
    id		pages_id;
    unsigned char digest[MD5_DIGEST_LENGTH];

    pdf_write_info(p);

    pdf_write_doc_fonts(p);			/* font objects */

    pdf_write_doc_colorspaces(p);		/* color space resources */

    pages_id = pdf_make_tree(p, BAD_ID, p->pnodes, p->pages+1, p->current_page);

    pdf_begin_obj(p, p->root_id);		/* Catalog or Root object */
    pdf_begin_dict(p);
    pdf_puts(p, "/Type/Catalog\n");

    /* 
     * specify the open action (display of the first page) 
     * default = retain: top of the first page at default zoom level
     */
    if (p->open_action == fitpage) {
	pdf_printf(p, "/OpenAction[%ld 0 R/Fit]\n", p->pages[1]);

    } else if (p->open_action == fitwidth) {
	pdf_printf(p, "/OpenAction[%ld 0 R/FitH]\n", p->pages[1]);

    } else if (p->open_action == fitheight) {
	pdf_printf(p, "/OpenAction[%ld 0 R/FitV 0]\n", p->pages[1]);

    } else if (p->open_action == fitbbox) {
	pdf_printf(p, "/OpenAction[%ld 0 R/FitB]\n", p->pages[1]);
    }

    /* 
     * specify the document's open mode
     * default = open_none: open document with neither bookmarks nor 
     * thumbnails visible
     */
    if (p->open_mode == open_bookmarks) {
	pdf_printf(p, "/PageMode/UseOutlines\n");

    } else if (p->open_mode == open_thumbnails) {
	pdf_printf(p, "/PageMode/UseThumbs\n");

    } else if (p->open_mode == open_fullscreen) {
	pdf_printf(p, "/PageMode/FullScreen\n");
    }

    if (p->base) {
	pdf_printf(p, "/URI<</Base", p->base);
        pdf_quote_string(p, p->base);
	pdf_printf(p, ">>\n", p->base);
    }
    						/* Pages object */
    pdf_printf(p, "/Pages %ld 0 R\n", pages_id);

    pdf_write_outline_root(p);

    pdf_end_dict(p);				/* Catalog */
    pdf_end_obj(p);

    pdf_write_outlines(p);

#ifdef DEBUG
    if (p->debug['s']) {
	fprintf(stderr, "PDF document statistics:\n");
	fprintf(stderr, "    %d pages\n", p->current_page);
	fprintf(stderr, "    %d fonts\n", p->fonts_number);
	fprintf(stderr, "    %d xobjects\n", p->xobjects_number);
	fprintf(stderr, "    %ld objects\n", p->currentobj + 1);
    }
#endif

    /* Don't write any object after this check! */
    for (i = 1; i <= p->currentobj; i++) {
	if (p->file_offset[i] == BAD_ID) {
	    pdf_error(p, PDF_NonfatalError,
		"Object %ld allocated but not used", i);
	    pdf_begin_obj(p, i);
	    pdf_end_obj(p);
	}
    }

    pos = pdf_tell(p);				/* xref table */
    pdf_puts(p, "xref\n");
    pdf_printf(p, "0 %ld\n", p->currentobj + 1);
    pdf_puts(p, "0000000000 65535 f \n");
    for (i = 1; i <= p->currentobj; i++) {
	pdf_printf(p, "%010ld 00000 n \n", p->file_offset[i]);
    }

    pdf_puts(p, "trailer\n");

    pdf_begin_dict(p);				/* trailer */
    pdf_printf(p, "/Size %ld\n", p->currentobj + 1);
    pdf_printf(p, "/Info %ld 0 R\n", p->info_id);
    pdf_printf(p, "/Root %ld 0 R\n", p->root_id);

    /* put some more data into the MD5 digest */
    MD5_Update(p->md5, (unsigned char *) &pos, sizeof pos);
    MD5_Update(p->md5, (unsigned char *) p, sizeof (PDF));
    if (p->filename && strcmp(p->filename, "-") != 0)
	MD5_Update(p->md5, (unsigned char *) p->filename, strlen(p->filename));

    /* write the digest to the ID array */
    MD5_Final(digest, p->md5);
    pdf_puts(p, "/ID[<");
    for (i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
	pdf_putc(p, bin2hex[digest[i] >> 4]);
	pdf_putc(p, bin2hex[digest[i] & 0x0F]);
    }
    pdf_puts(p, "><");
    for (i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
	pdf_putc(p, bin2hex[digest[i] >> 4]);
	pdf_putc(p, bin2hex[digest[i] & 0x0F]);
    }
    pdf_puts(p, ">]\n");

    pdf_end_dict(p);				/* trailer */

    pdf_puts(p, "startxref\n");
    pdf_printf(p, "%ld\n", pos);
    pdf_puts(p, "%%EOF\n");
}

static void
pdf_cleanup_document(PDF *p)
{
    /* close the output file if writing to file... */
    if (p->writeproc)
	pdf_close_file(p);
    
    /*
     * ...but do not close the output stream since the caller will have to
     * fetch the buffer after PDF_close().
     */

    /* clean up all document-related stuff */
    if (p->file_offset) {
	p->free(p, p->file_offset);
	p->file_offset = NULL;
    }
    if (p->contents_ids) {
	p->free(p, p->contents_ids);
	p->contents_ids = NULL;
    }
    if (p->pages) {
	p->free(p, p->pages);
	p->pages = NULL;
    }
    if (p->resourcefilename) {
	p->free(p, p->resourcefilename);
	p->resourcefilename = NULL;
    }
    if (p->base) {
	p->free(p, p->base);
	p->base = NULL;
    }
    if (p->pnodes) {
	p->free(p, p->pnodes);
	p->pnodes = NULL;
    }
    if (p->md5) {
	p->free(p, p->md5);
	p->md5 = NULL;
    }
    pdf_cleanup_info(p);
    pdf_cleanup_fonts(p);
    pdf_cleanup_outlines(p);
    pdf_cleanup_colorspaces(p);
    pdf_cleanup_pattern(p);
    pdf_cleanup_images(p);
    pdf_cleanup_xobjects(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_close(PDF *p)
{
    static const char fn[] = "PDF_close";

    PDF_TRACE(("%s\t\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_document);

    if (PDF_GET_STATE(p) != pdf_state_error) {
	if (p->current_page == 0)
	    pdf_error(p, PDF_RuntimeError, "Empty document");

	pdf_wrapup_document(p);	/* dump the remaining PDF structures */
    }

    pdf_flush_stream(p);

    pdf_cleanup_document(p);

    /* UPR stuff not released here but in PDF_delete() */

    PDF_SET_STATE(p, fn, pdf_state_object);
}

void
pdf_begin_contents_section(PDF *p)
{
    if (p->contents != c_none)
	return;

    if (p->next_content >= p->contents_ids_capacity) {
	p->contents_ids_capacity *= 2;
	p->contents_ids = (id *) p->realloc(p, p->contents_ids,
			    sizeof(long) * p->contents_ids_capacity,
			    "pdf_begin_contents_section");
    }

    p->contents_ids[p->next_content] = pdf_begin_obj(p, NEW_ID);
    p->contents	= c_page;
    pdf_begin_dict(p);
    p->contents_length_id = pdf_alloc_id(p);
    pdf_printf(p, "/Length %ld 0 R\n", p->contents_length_id);

    if (p->compresslevel)
	pdf_puts(p, "/Filter/FlateDecode\n");

    pdf_end_dict(p);

    pdf_begin_stream(p);

    pdf_compress_init(p);
    							/* Contents object */
    p->start_contents_pos = pdf_tell(p);
    p->next_content++;
}

void
pdf_end_contents_section(PDF *p)
{
    long length;

    if (p->contents == c_none)
	return;

    pdf_end_text(p);
    p->contents = c_none;

    pdf_compress_end(p);
						/* Contents object */
    length = pdf_tell(p) - p->start_contents_pos;
    pdf_end_stream(p);
    pdf_end_obj(p);

    pdf_begin_obj(p, p->contents_length_id);	/* Length object */
    pdf_printf(p, "%ld\n", length);
    pdf_end_obj(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_begin_page(PDF *p, float width, float height)
{
    static const char fn[] = "PDF_begin_page";

    PDF_TRACE(("%s\t(pdf[%p], %f, %f);\n", fn, (void *) p, width, height));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_document);
    
    if (width <= 0 || height <= 0)
	pdf_error(p, PDF_ValueError, "Page size must be positive");

    if (p->compatibility >= PDF_1_3 &&
	(height < PDF_ACRO4_MINPAGE || width < PDF_ACRO4_MINPAGE ||
	height > PDF_ACRO4_MAXPAGE || width > PDF_ACRO4_MAXPAGE))
	pdf_error(p, PDF_NonfatalError,
	    "Page size incompatible with Acrobat 4");

    else if (p->compatibility == PDF_1_2 &&
	(height < PDF_ACRO3_MINPAGE || width < PDF_ACRO3_MINPAGE ||
	height > PDF_ACRO3_MAXPAGE || width > PDF_ACRO3_MAXPAGE))
	pdf_error(p, PDF_RuntimeError, "Page size incompatible with Acrobat 3");

    if (++(p->current_page) >= p->pages_capacity)
	pdf_grow_pages(p);

    /* no id has been preallocated */
    if (p->pages[p->current_page] == BAD_ID)
	p->pages[p->current_page] = pdf_alloc_id(p);

    p->height		= height;
    p->width		= width;
    p->thumb_id		= BAD_ID;
    p->next_content	= 0;
    p->contents 	= c_none;
    p->procset		= 0;
    p->sl		= 0;

    p->CropBox.llx	= (float) 0;
    p->CropBox.lly	= (float) 0;
    p->CropBox.urx	= (float) 0;
    p->CropBox.ury	= (float) 0;

    p->BleedBox.llx	= (float) 0;
    p->BleedBox.lly	= (float) 0;
    p->BleedBox.urx	= (float) 0;
    p->BleedBox.ury	= (float) 0;

    p->TrimBox.llx	= (float) 0;
    p->TrimBox.lly	= (float) 0;
    p->TrimBox.urx	= (float) 0;
    p->TrimBox.ury	= (float) 0;

    p->ArtBox.llx	= (float) 0;
    p->ArtBox.lly	= (float) 0;
    p->ArtBox.urx	= (float) 0;
    p->ArtBox.ury	= (float) 0;

    PDF_SET_STATE(p, fn, pdf_state_page);

    pdf_init_page_annots(p);
    pdf_init_tstate(p);
    pdf_init_gstate(p);
    pdf_init_cstate(p);

    pdf_begin_contents_section(p);
}

void
pdf_write_page_procsets(PDF *p)
{
    pdf_puts(p, "/ProcSet[/PDF");

    if (p->procset & ImageB)	pdf_puts(p, "/ImageB");
    if (p->procset & ImageC)	pdf_puts(p, "/ImageC");
    if (p->procset & ImageI)	pdf_puts(p, "/ImageI");
    if (p->procset & Text)	pdf_puts(p, "/Text");

    pdf_puts(p, "]\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_end_page(PDF *p)
{
    static const char fn[] = "PDF_end_page";
    int index;


    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_page);

    /* check whether PDF_save() and PDF_restore() calls are balanced */
    if (p->sl > 0)
	pdf_error(p, PDF_RuntimeError, "Unmatched save level at end of page");


    /* restore text parameter and color defaults for out-of-page usage.
    */
    pdf_init_tstate(p);
    pdf_init_cstate(p);

    pdf_end_contents_section(p);

    /* Page object */
    pdf_begin_obj(p, p->pages[p->current_page]);

    pdf_begin_dict(p);
    pdf_puts(p, "/Type/Page\n");
    pdf_printf(p, "/Parent %ld 0 R\n", pdf_get_pnode_id(p));

    p->res_id = pdf_alloc_id(p);
    pdf_printf(p, "/Resources %ld 0 R\n", p->res_id);

    pdf_printf(p, "/MediaBox[0 0 %f %f]\n", p->width, p->height);

    if (p->CropBox.llx != (float) 0 || p->CropBox.lly != (float) 0 ||
        p->CropBox.urx != (float) 0 || p->CropBox.ury != (float) 0 )
    {
	if (p->CropBox.urx <= p->CropBox.llx ||
	    p->CropBox.ury <= p->CropBox.lly)
		pdf_error(p, PDF_ValueError, "Illegal CropBox dimensions");

	pdf_printf(p, "/CropBox[%f %f %f %f]\n",
	    p->CropBox.llx, p->CropBox.lly, p->CropBox.urx, p->CropBox.ury);
    }

    if (p->BleedBox.llx != (float) 0 || p->BleedBox.lly != (float) 0 ||
        p->BleedBox.urx != (float) 0 || p->BleedBox.ury != (float) 0 )
    {
	if (p->BleedBox.urx <= p->BleedBox.llx ||
	    p->BleedBox.ury <= p->BleedBox.lly)
		pdf_error(p, PDF_ValueError, "Illegal BleedBox dimensions");

	pdf_printf(p, "/BleedBox[%f %f %f %f]\n",
	    p->BleedBox.llx, p->BleedBox.lly, p->BleedBox.urx, p->BleedBox.ury);
    }

    if (p->TrimBox.llx != (float) 0 || p->TrimBox.lly != (float) 0 ||
        p->TrimBox.urx != (float) 0 || p->TrimBox.ury != (float) 0 )
    {
	if (p->TrimBox.urx <= p->TrimBox.llx ||
	    p->TrimBox.ury <= p->TrimBox.lly)
		pdf_error(p, PDF_ValueError, "Illegal TrimBox dimensions");

	pdf_printf(p, "/TrimBox[%f %f %f %f]\n",
	    p->TrimBox.llx, p->TrimBox.lly, p->TrimBox.urx, p->TrimBox.ury);
    }

    if (p->ArtBox.llx != (float) 0 || p->ArtBox.lly != (float) 0 ||
        p->ArtBox.urx != (float) 0 || p->ArtBox.ury != (float) 0 )
    {
	if (p->ArtBox.urx <= p->ArtBox.llx ||
	    p->ArtBox.ury <= p->ArtBox.lly)
		pdf_error(p, PDF_ValueError, "Illegal ArtBox dimensions");

	pdf_printf(p, "/ArtBox[%f %f %f %f]\n",
	    p->ArtBox.llx, p->ArtBox.lly, p->ArtBox.urx, p->ArtBox.ury);
    }

    /*
     * The duration can be placed in the transition dictionary (/D)
     * or in the page dictionary (/Dur). We put it here so it can
     * be used without setting a transition effect.
     */

    if (p->duration > 0)
	pdf_printf(p, "/Dur %f\n", p->duration);

    pdf_write_page_transition(p);

    pdf_puts(p, "/Contents[");
    for (index = 0; index < p->next_content; index++) {
	pdf_printf(p, "%ld 0 R", p->contents_ids[index]);
	pdf_putc(p, (char) (index + 1 % 8 ? PDF_SPACE : PDF_NEWLINE));
    }
    pdf_puts(p, "]\n");

    /* Thumbnail image */
    if (p->thumb_id != BAD_ID)
	pdf_printf(p, "/Thumb %ld 0 R\n", p->thumb_id);

    pdf_write_annots_root(p);

    pdf_end_dict(p);				/* Page object */
    pdf_end_obj(p);

    pdf_write_page_annots(p);			/* Annotation dicts */
    
    pdf_begin_obj(p, p->res_id);		/* resource object */
    pdf_begin_dict(p);				/* resource dict */

    pdf_write_page_procsets(p);			/* ProcSet resources */

    pdf_write_page_fonts(p);			/* Font resources */

    pdf_write_page_colorspaces(p);		/* Color space resources */

    pdf_write_page_pattern(p);			/* Pattern resources */

    pdf_write_xobjects(p);			/* XObject resources */

    pdf_end_dict(p);				/* resource dict */
    pdf_end_obj(p);				/* resource object */

    pdf_cleanup_page(p);

    PDF_SET_STATE(p, fn, pdf_state_document);

    if (p->flush & PDF_FLUSH_PAGE)
	pdf_flush_stream(p);
}

id
pdf_begin_obj(PDF *p, id obj_id)
{
    if (obj_id == NEW_ID)
	obj_id = pdf_alloc_id(p);

    p->file_offset[obj_id] = pdf_tell(p); 
    pdf_printf(p, "%ld 0 obj\n", obj_id);
    return obj_id;
}

id
pdf_alloc_id(PDF *p)
{
    p->currentobj++;

    if (p->currentobj >= p->file_offset_capacity) {
	p->file_offset_capacity *= 2;
	p->file_offset = (long *) p->realloc(p, p->file_offset,
		    sizeof(long) * p->file_offset_capacity, "pdf_alloc_id");
    }

    /* only needed for verifying obj table in PDF_close() */
    p->file_offset[p->currentobj] = BAD_ID;

    return p->currentobj;
}

void
pdf_grow_pages(PDF *p)
{
    int i;

    p->pages_capacity *= 2;
    p->pages = (id *) p->realloc(p, p->pages,
		sizeof(id) * p->pages_capacity, "pdf_grow_pages");
    for (i = p->pages_capacity/2; i < p->pages_capacity; i++)
	p->pages[i] = BAD_ID;
}
