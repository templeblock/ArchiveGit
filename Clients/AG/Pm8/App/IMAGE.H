/*
// $Header: /PM8/App/IMAGE.H 1     3/03/99 6:06p Gbeddow $
//
// Image definitions.
//
// $Log: /PM8/App/IMAGE.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     4/07/98 8:43a Jayn
// Expanded size capabilities.
// 
//    Rev 1.0   14 Aug 1997 15:21:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:12   Fred
// Initial revision.
// 
//    Rev 1.4   09 Dec 1996 10:19:44   Jay
// More image stuff.
// 
//    Rev 1.3   05 Dec 1996 12:30:32   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.2   01 Nov 1996 17:11:50   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.1   16 Oct 1996 13:05:16   Jay
// Fixed direct 24-bit graphic with color.
// 
//    Rev 1.0   14 Mar 1996 13:46:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:12   FRED
// Initial revision.
// 
//    Rev 2.1   09 Oct 1995 15:15:04   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.0   07 Feb 1995 15:48:40   JAY
// Initial revision.
// 
//    Rev 1.4   04 Aug 1994 07:33:50   JAY
// Large model and 32-bit changes
// 
//    Rev 1.3   22 Feb 1994 15:02:28   JAY
// New rasterizing code for CGMs which uses GDI calls.
// 
//    Rev 1.2   13 Jan 1994 17:01:14   JAY
//  
// 
//    Rev 1.1   30 Dec 1993 16:11:04   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:25:56   JAY
//  
*/

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "paint.h"
#include "imagedrv.h"

typedef class ImageContext far *ImageContextPtr;

#include "imagebuf.h"
#include "imagesrc.h"

#define MAX_COLOR_PLANES 20

class CBitmapObjectImage;

/*
// An info structure holding all relevant image (bitmap) info.
*/

class ImageContext : public PaintContext
{
private:

	VOID free_render_buffers(VOID);
	BOOL init_render_buffers(VOID);

	BOOL init_smoothing(VOID);
	VOID free_smoothing(VOID);

	VOID xflip_strip(VOID);

	virtual VOID compute_source_extent(VOID);

protected:

	VOID process_errdiff_smoothed(int plane, LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width);

	VOID process_errdiff_unsmoothed(int plane, LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width);

	ImageSourcePtr source;

/*
// Actual processing elements.
// Depends on the image type.
*/

	virtual IMAGE_CODE init_block(VOID);
	virtual IMAGE_CODE process_line(SHORT *skip) = 0;
	virtual VOID deinit_block(VOID);

	virtual void ProcessMaskLine(void);

public:

	ImageContext(ImageSourcePtr s);
	virtual ~ImageContext();

/*
// Our image info.
*/

	IMAGE_INFO	image_info;

/*
// Filled in by init_color_stuff().
*/

	LPBYTE 		source_row;
	LPBYTE 		m_pMaskRow;

	PALETTE_INFO palette;

	UCHAR far	*translate_table[MAX_COLOR_PLANES];

/*
// Variables set by init_smoothing().
*/

/* Vertical smoothing variables. */

	struct
	{
		LPBYTE	value;
		LPBYTE	delta;
		LPBYTE	fraction;
		LPBYTE	final;
		LPSTR		inc;				// signed
	} smooth;

	USHORT smoothing_size;


/* The object this is */

	GraphicObjectPtr		object;

/*
// For use whilst we're building an image.
*/

	CBitmapObjectImage*		object_image;	/* The object image when recording */

/*
// The top origin when making an object image.
// If bitmap painting is interrupted, the dest top may move down; this will
// confuse the object image code which needs the same top the whole time.
// So we remember the top the first time and use it from then on.
*/

	int image_top;

/*
// Methods.
*/

	VOID setup_translate_tables(COLOR_COMPONENT far *rgbdata, SHORT data_size, BOOL fReversed = FALSE);

	VOID set_current_strip(VOID);

/*
// Update notify routines.
*/

	VOID notify_paint_image_update(VOID);

	VOID show_graphic_map(BOOL interrupted);

/*
// Initialization routines.
*/

	VOID init_palette(VOID);
	BOOL init_render_machine(VOID);

/*
// Interrupt handling routines.
*/

	IMAGE_CODE check_interrupt(VOID);

/*
// Process a block.
// This is where (almost) all of it happens.
*/
	virtual IMAGE_CODE process_block(VOID);
};

/*
// A palette to bilevel rendering context.
*/

class PaletteContext : public ImageContext
{
private:
	VOID (ImageContext::* process_error_diffusion)(int plane, LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width);
public:
	PaletteContext(ImageSourcePtr s) : ImageContext(s) {}
	virtual IMAGE_CODE init_block(VOID);
	virtual IMAGE_CODE process_line(SHORT *skip);
};

/*
// A full-color to bilevel rendering context.
*/

class FullColorContext : public ImageContext
{
private:
	VOID (ImageContext::* process_error_diffusion)(int plane, LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width);
public:
	FullColorContext(ImageSourcePtr s) : ImageContext(s) {}
	virtual IMAGE_CODE init_block(VOID);
	virtual IMAGE_CODE process_line(SHORT *skip);
};

/*
// A context for rendering palette to a palette device
*/

class PalPaletteContext : public ImageContext
{
private:
	VOID process_errdiff_unsmoothed(LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width);

	VOID process_errdiff_smoothed(LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width);

protected:
	VOID convertPalette(LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
 							int row,
							int x_offset, int width);

	VOID (PalPaletteContext::* process_error_diffusion)(LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width);
public:
	PalPaletteContext(ImageSourcePtr s) : ImageContext(s) {}
	virtual IMAGE_CODE init_block(VOID);
	virtual IMAGE_CODE process_line(SHORT *skip);
};

/*
// A full-color rendering context.
*/

class Bit24Context : public PalPaletteContext
{
public:
	Bit24Context(ImageSourcePtr s) : PalPaletteContext(s) {}

	virtual IMAGE_CODE init_block(VOID);
	virtual IMAGE_CODE process_line(SHORT *skip);
};

/*
// A direct rendering context.
*/

class DirectBitmapContext : public ImageContext
{
public:
	DirectBitmapContext(ImageSourcePtr s) : ImageContext(s) {}

	virtual IMAGE_CODE init_block(void);
	virtual IMAGE_CODE process_line(SHORT *skip);
	virtual void ProcessMaskLine(void);
};

class ImageUpdateState : public UpdateState
{
public:
	RECT clip;
	SHORT image_top;
};

typedef ImageUpdateState far *ImageUpdateStatePtr;

/*
// Paint a bitmap image.
*/

extern UpdateStatePtr paint_bitmap_image(GraphicObjectPtr object, LPRECT clip,
					GraphicPtr record, ImageDriverPtr driver,
					RedisplayContextPtr rc, UpdateStatePtr ustate);
#endif					// #ifndef __IMAGE_H__
