/* TARGA file description */

/* dataBits values */
#define TYPE_8		8
#define TYPE_16		16
#define TYPE_24		24
#define TYPE_32		32
#define TYPE_M8		-8

typedef struct _tgahdr {
    char	textSize;
    char	mapType;
    char	dataType;
    int		mapOrig;
    int		mapLength;
    char	CMapBits;
    int		XOffset;
    int		YOffset;
    int		x;
    int		y;
    char	dataBits;
    char	imType;
} TGAHDR;

typedef struct _tgastate {
    FILEBUF	*ifd;
    RGBS 	*RGBmap;	/* pointer to color map */
    int		inputBytes;
    int		cnt;		/* number of pixels remaining in packet */
    RGBS	pixel;		/* pixel being repeated (run-length packet) */
    BOOL	rawPacket;	/* raw packet flag */
} TGASTATE;
