
/************************************************************************
 * HEADER gif.h
 *	Constants and types for Compuserve 'GIF' images.
 */


#define GIFImSep	','
#define GIFExtBlk	'!'
#define GIFTerm		';'


typedef struct _gifmap {
    WORD	Length;		/* entries in color map */
    RGBS	Map[256];	/* color map */
} GIFMAP;

typedef struct _gifhdr {
    BYTE		Signiture[6];
    WORD		ScnWidth;
    WORD		ScnHeight;
    BYTE		RawImInfo;
    BYTE		Background;
    BYTE		Zero;

/* the following is generated from the RawImInfo field */
    BOOL		GlobalMap;	/* true if global color map */
    BYTE		ColorRes;	/* color resolution */
    BYTE		bpp;		/* bits per pixel */
} GIFHDR;

/* Image Descriptor */
typedef struct _gifdesc {
    WORD	ImLeft;
    WORD	ImTop;
    WORD	ImWidth;
    WORD	ImHeight;
    BYTE	RawImInfo;
    BYTE	Zero;

/* the following is generated from the RawImInfo field */
    BOOL	LocalMap;	/* true if local color map */
    BOOL	Interlaced;	/* true if image is interlaced */
    BYTE	bpp;		/* bits per pixel */
} GIFDESC;
