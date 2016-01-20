/*
MOD 07-12-87 MAZ - standard RIFF header typedefs and defines
*/

typedef long Fixed;

/* header of a RIFF file */
typedef struct _rifhdr {
    Fixed revnum;
    short nrows;
    short ncols;
    short flags;
    short storage_type;
    short nsamples;
    long chunks[4];
    short resolution;
    char reserved[32];
} RIFHDR;

/* header for a scanline in a compressed file */
typedef struct _rifscanhdr {
    char tag;
    char byte_zero;
    short record_count;
} RIFSCANHDR;

/* header for the run-length encoded (RLE) records */
typedef struct _rifrlehdr {
    char subtag;
    unsigned char count;
} RIFRLEHDR;

/* RIFF revision numbers */
#define RIFF_rev1_0		0x00010000L
#define RIFF_rev1_1		0x00011000L

/* flags bitmasks for RIFFheader flags field */
#define hf_uncompressed		0x0001
#define hf_transfer		0x0002

/* values for the RIFFheader storage_type field */
#define st_gray			0
#define st_vlt			1
#define st_rgb			2
#define st_hsv			3
#define st_cmy			4
#define st_cmyk			5

/* scan_header tag values */
#define sf_compressed		0
#define sf_uncompressed		1
#define sf_RLE			2

/* RLE_header subtag values */
#define st_data			0
#define st_run			1
