/*
// $Header: /PM8/App/OLDGRAF.H 1     3/03/99 6:08p Gbeddow $
//	Old Definitions for graphic objects and data.
//
*/

#ifndef __OLDGRAF_H__
#define __OLDGRAF_H__

/*
// Structures.
*/

typedef enum
{
	OLD_GRAPHIC_TYPE_UNKNOWN,
	OLD_GRAPHIC_TYPE_SHP,			/* PrintMaster, NewsMaster */
	OLD_GRAPHIC_TYPE_TIFF,			/* Tag Image File Format */
	OLD_GRAPHIC_TYPE_PCX,			/* Paintbrush, etc. */
	OLD_GRAPHIC_TYPE_LOTUS,			/* Lotus Pic. Not supported here... */
	OLD_GRAPHIC_TYPE_POG,			/* New PrintShop - "old" format */
	OLD_GRAPHIC_TYPE_PNG,			/* New PrintShop - "new" format */
	OLD_GRAPHIC_TYPE_CNG,			/* New PrintShop - "new" color format */
	OLD_GRAPHIC_TYPE_DAT,			/* PrintShop */
	OLD_GRAPHIC_TYPE_GIF,			/* Graphics Interchange Format file. */
	OLD_GRAPHIC_TYPE_HALO,			/* Dr. Halo file. */
	OLD_GRAPHIC_TYPE_EPS,			/* Encapsulated PostScript file. */
	OLD_GRAPHIC_TYPE_BMP,			/* Windows bitmap file. */
	OLD_GRAPHIC_TYPE_CGM,			/* Computer Graphics Metafile format. */
	OLD_GRAPHIC_TYPE_WPG,			/* Word Perfect Graphic file. */

/*
// Border types.
// Technically, a OLD_GRAPHIC_TYPE should never be one of these.
*/

	OLD_BORDER_TYPE_UNKNOWN = 0,		/* unknown type */
	OLD_BORDER_TYPE_BIC = 100,			/* PrintMaster border */
	OLD_BORDER_TYPE_PBD,					/* Print Shop border (old format) */
	OLD_BORDER_TYPE_POB,					/* Print Shop border (new format) */
	OLD_BORDER_TYPE_PNB					/* New Print Shop border */
} OLD_GRAPHIC_TYPE, OLD_BORDER_TYPE;

typedef enum
{
	OLD_GRAPHIC_STORAGE_UNKNOWN,		/* totally unknown */
	OLD_GRAPHIC_STORAGE_MEMORY,		/* in memory */
	OLD_GRAPHIC_STORAGE_FILE,			/* in the source file */
	OLD_GRAPHIC_STORAGE_IMAGE,			/* an image is in memory */
	OLD_GRAPHIC_STORAGE_MISSING,		/* on disk but in an unknown place */
	OLD_GRAPHIC_STORAGE_BITMAP			/* in the document at low resolution */
} OLD_GRAPHIC_STORAGE;

typedef struct
{
	unsigned int	line;
	unsigned long	offset;
} OLD_LINE_LOCATOR;

/*
// Possible return types from a method.
*/

typedef enum
{
	OLD_UL_CORNER,
	OLD_UR_CORNER,
	OLD_LR_CORNER,
	OLD_LL_CORNER,
	OLD_UPPER_EDGE,
	OLD_LOWER_EDGE,
	OLD_LEFT_EDGE,
	OLD_RIGHT_EDGE,
	NUMBER_OF_OLD_BORDER_PIECES
} OLD_BORDER_PIECES;

typedef struct
{
	UCHAR	width;							/* width of this piece */
	UCHAR	height;							/* height of this piece */
	SHORT	size;								/* The size of the piece */
	SHORT	offset;							/* The offset of the piece in the data */
} OLD_BORDER_PIECE;

typedef struct
{
	BOOL		valid;
	SHORT		file;
	WORD		byte_order;
	SHORT		width;
	SHORT		height;
	SHORT		byte_width;
	SHORT		bits_per_sample;
	SHORT		rows_per_strip;
	SHORT		compression;
	SHORT		photometric;
	LONG		strip_offsets;
	SHORT		last_row_read;
	LONG		last_strip_read;
	LONG		next_line_offset;
	LONG		palette_offset;
	SHORT		gray_response_length;
	SHORT		gray_response_unit;
	SHORT		samples_per_pixel;
	SHORT		planar_configuration;
	SHORT		predictor;
	ULONG		offset;
/* All the fields! */
//	TIFF_ENTRY	*entries;
	LPBYTE	entries;
} OLD_TIFF_INFO;

typedef struct
{
	LPVOID			next;					/* pointer to next graphic */
	LPSTR				OldName;					/* The viewed name of the graphic */
	LPSTR				OldFileName;			/* The file name of the graphic */
	unsigned int	reference_count;	/* How many references there are */
	OLD_GRAPHIC_TYPE	type;					/* What type of graphic this is */
	unsigned int	x_resolution;		/* The x resolution of the data */
	unsigned int	y_resolution;		/* The y resolution of the data */
	unsigned int	x_size;				/* The x dimension of the data */
	unsigned int	y_size;				/* The y dimension of the data */
	OLD_GRAPHIC_STORAGE	storage;			/* Where the data is located. */
	int				number;				/* Which number (in library, etc.) */
	LPBYTE 			bitmap;				/* An actual pointer to the data */
	union
	{
		struct
		{
			SHORT				file;					/* file handle */
			ULONG				offset;				/* Offset into file */
		} file_info;
		struct
		{
			SHORT				row_bytes;			/* bytes in a row */
			UCHAR				nplanes;				/* number of planes */
			UCHAR				bits_per_pixel;	/* number of bits per pixel */
			OLD_LINE_LOCATOR	lines[8+1];			/* start of lines */
		} pcx_info;
		struct
		{
			UCHAR				bits_per_pixel;	/* number of bits per pixel */
			BOOL				has_color_map;		/* has a global color map */
		} gif_info;
		struct
		{
			BOOL				has_pal_file;		/* the pal file exists */
			UCHAR				bits_per_pixel;	/* number of bits per pixel */
			OLD_LINE_LOCATOR	lines[8+1];			/* start of lines */
		} halo_info;
		struct
		{
			DWORD				header_size;		/* size of BMP header */
			UCHAR				bits_per_pixel;	/* number of bits per pixel */
			UCHAR				compression;		/* type of compression */
			DWORD				bitmap_offset;		/* offset to the bitmap */
		} bmp_info;
		struct
		{
			SHORT					border_x_thick;
			SHORT					border_y_thick;
			SHORT					border_h_edge_length;
			SHORT					border_v_edge_length;
			OLD_BORDER_PIECE	piece[NUMBER_OF_OLD_BORDER_PIECES];
		} border_info;
		OLD_TIFF_INFO	tiff_info;
		struct
		{
			POINT				metafile_min;
			POINT				metafile_max;
			SHORT				number_of_colors;
			POINT				extended_min;
			POINT				extended_max;
		} cgm_info;
		struct
		{
			SHORT				version;
			ULONG				data_start;
		} wpg_info;
	} where;
} OLD_GRAPHIC_DATA;

typedef OLD_GRAPHIC_DATA far *OLD_GRAPHIC_DATA_PTR;

#endif						/* #ifndef __OLDGRAF_H__ */
