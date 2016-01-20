/*
** convert.c
**
** Post-processing of 24-bit imagery using the OCTREE algorithm
**
** Dean Clark
** for Dr. Dobbs Journal
** June 1995
**
*/

#include <stdio.h>
#include <math.h>
#include <alloc.h>
#include <mem.h>
#include <dos.h>
#include <conio.h>
#include <process.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "oct1.h"

extern unsigned _stklen = 16294U;

/*
** Super-VGA graphics display uses MetaWINDOW graphics library
*/
#if defined METAWINDO
#include <metawndo.h>
int GrafixCard,GrafixInput;             /* Global variables for GRQuery     */
#endif


int SaveAsPCX(FILE *fimg,
			  OctreeType *octree,
              int resx,
              int resy,
			  double nrgbr,
              double nrgbg,
              double nrgbb,
              int npal,
			  RGBType *colortable);

int npal = 256;

void main(int argc, char *argv[])
{
	double      r, g, b;
	uint        rows, cols;
	char        fname[256];
	double      colormag;
	time_t      tstart, tend;
    int         nrgbr = 63, nrgbg = 63, nrgbb = 63;
	OctreeType  *octree;
	RGBType		color;
	FILE		*f;
	char		title[40];
	char		description[128];
    ulong       i;
    uint        j;
	int			n;
	RGBType		palette[256];
	ulong		image_start;
	union REGS	regs;
	int			resx, resy;
	int			px, py;
	int			ii;
	int			cols2, rows2;
	int			k;
	int			cli;
    int         maxr=0, maxg=0, maxb=0;

#if defined METAWINDO
	rect        screen;
#endif

	printf("Image file : ");
	scanf("%s",fname);
	if ((f = fopen(fname,"rb")) == NULL) {
        printf("%s not found.\n",fname);
		exit(1);
	}

    /*
    ** Read the image file header
    */
	fgets(title,40,f);
	fgets(description,128,f);
	fscanf(f,"%d %d",&cols,&rows);
    fscanf(f,"%lf",&colormag);
	image_start = ftell(f);

	cols2 = cols/2;
	rows2 = rows/2;
	time(&tstart);

	/*
    ** Initialize the color octree
	*/
    octree = CreateOctNode(0);

	/*
    ** Loop through the image and store each unique color.
	*/
	for (i = 0L; i < (ulong)rows*(ulong)cols; i++) {
		/*
		** Show progress...
		*/
		if ((i % (ulong)cols) == 0L) printf("%ld\r",i/cols);

        fscanf(f,"%lf %lf %lf",&r,&g,&b);
		/*
		** Convert input floating point values to bytes.  NOTE: We assume that
		** all input values are between 0..1.0
		*/
        color.r = (unsigned char)(r  * nrgbr);
        color.g = (unsigned char)(g  * nrgbg);
        color.b = (unsigned char)(b  * nrgbb);
		if (color.r > nrgbr) color.r = nrgbr;
		if (color.g > nrgbg) color.g = nrgbg;
		if (color.b > nrgbb) color.b = nrgbb;

		/*
		** Insert this color into the octree
		*/
		InsertTree(&octree, &color, 0);

		/*
		** If there are too many colors in the tree as a result of this
		** insert, reduce the octree
		*/
		while (TotalLeafNodes() > npal) {
			ReduceTree();
		}
	}

	/*
	** Make a pass through the completed octree to average down the
	** rgb components.  When done, 'n' contains the actual number of
	** colors in the palette table.
	*/
	n = 0;
	MakePaletteTable(octree, palette, &n);

	/*
	** How long did it take?
	*/
	time(&tend);
	printf("Processed %ld pixels per second\ninto %d quantized colors\n",
		   ((long)rows*(long)cols)/(tend-tstart), n);

	j = 0;
	while (j != 3) {
		printf("Output to (1)monitor or (2).PCX file or (3) quit: ");
		scanf("%s",title);
		j = atoi(title);
		if (j == 2) {
			fseek(f,image_start,0);
            SaveAsPCX(f, octree, cols, rows, nrgbr, nrgbg, nrgbb, npal, palette);
		}
		else if (j == 1) {
#if defined METAWINDO
			/*
			** NOTE: This section requires MetaWINDOW graphics lib
			**
			** Let the user choose his graphics device and resolution
			*/
			MetQuery(argc,argv);
			if (InitGraphics(GrafixCard) != 0) {
				printf("\n---Error initializing graphics device---\n");
				exit(1);
			}
			SetDisplay(GrafPg0);
			BackColor(0);
			/*
			** Set the VGA palette
			*/
			for (j = 0; j < n; j++) {
				regs.h.al = 0x10;
				regs.h.ah = 0x10;
				regs.h.bl = j;
				regs.h.bh = 0;
				regs.h.ch = (int)(palette[j].g);
				regs.h.cl = (int)(palette[j].b);
				regs.h.dh = (int)(palette[j].r);
				int86(0x10,&regs,&regs);
			}

			/*
			** Center the image on the screen
			*/
			ScreenRect(&screen);
			resx = screen.Xmax;
			resy = screen.Ymax;
			px = resx/2 - npal;

			/*
			** Display a color bar at the top of the screen
			*/
			for (ii = 0; ii < npal; ii++){
				PenColor(ii);
				SetPixel(ii*2+px,1);
				SetPixel(ii*2+px+1,1);
				SetPixel(ii*2+px,2);
				SetPixel(ii*2+px+1,2);
				SetPixel(ii*2+px,3);
				SetPixel(ii*2+px+1,3);
				SetPixel(ii*2+px,4);
				SetPixel(ii*2+px+1,4);
			}

			fseek(f,image_start,0);

			py = resy/2 - rows2 - 1;
			for (ii = 0; ii < rows ; ii++) {
				px = resx/2 - cols2;
				for (k = 0; k < cols; k++) {
					if (fscanf(f,"%f %f %f",&r,&g,&b) == EOF) {
						goto pdone;
					}
					color.r = (byte)(nrgbr * r);
					color.g = (byte)(nrgbg * g);
					color.b = (byte)(nrgbb * b);
					cli = QuantizeColor(octree, &color);
					PenColor(cli);
					SetPixel(px,py);
					px++;
				}
				py++;
			}
pdone:      getch();
			SetDisplay(TextPg0);
			StopGraphics();
#endif
		}
	}
}



int encput(int byt, int cnt, FILE *fid);
int encline(unsigned char *line, int length, FILE *fp);

typedef struct {
	char        manuf;              /* Program manufacturer ID          */
    char        vers;               /* Version number                   */
	char        rle;                /* if 1 then image run-length enc   */
    char        bitpx;              /* Bits per pixel                   */
	unsigned short    x1,           /* Image bounds                     */
                y1,
				x2,
                y2,
				hres,               /* Horizontal resolution            */
                vres;               /* vertical resolution              */
	unsigned char rgb[48];          /* Palette                          */
    char        vmode;              /* Unused                           */
	char        nplanes;            /* Number of planes                 */
    unsigned short   bpline;        /* bytes per line                   */
	unsigned short   palinfo;       /* 1=color, 2=grayscale             */
    unsigned short   hscreen;
	unsigned short   vscreen;
    char        xtra[54];           /* Future use?                      */
} PCXhdr;


typedef struct {
	unsigned char r;
    unsigned char g;
	unsigned char b;
} PCXCOLOR;


/****************************************************************************
**
** SaveAsPCX
**
** Write a PCX file version of the image
*/
int SaveAsPCX(FILE *f,
			  OctreeType *octree,
			  int resx,
			  int resy,
			  double nrgbr,
			  double nrgbg,
			  double nrgbb,
			  int npal,
			  RGBType *colortable)
{
    PCXhdr          hdr;
	FILE            *pf;
    unsigned char   *buf;
	PCXCOLOR        *pal;
    int             i,j,k;
	char            pcxfile[128];
    double          r, g, b;
	RGBType			color;

	printf("\nPCX file name : ");
	scanf("%s",pcxfile);

	if ((pf = fopen(pcxfile, "wb")) == NULL) {
        fprintf(stderr, "Can't open file %s for writing\n", pcxfile);
		return 1;
    }

    /*
	** Set up the PCX palette.
    */
	pal = (PCXCOLOR *)malloc(256*sizeof(PCXCOLOR));
	for (k = 0; k < npal ; k++) {
		pal[k].r = (unsigned char)(colortable[k].r);
		pal[k].g = (unsigned char)(colortable[k].g);
		pal[k].b = (unsigned char)(colortable[k].b);
	}
    /*
	** Fill in the PCX header
    */
	hdr.manuf = (char)10;
    hdr.vers = (char)5;
	hdr.rle = (char)1;
    hdr.bitpx = 8;
	hdr.x1 = hdr.y1 = 0;
	hdr.x2 = resx - 1;
	hdr.y2 = resy - 1;
	hdr.hres = 640;
	hdr.vres = 480;
	hdr.hscreen = 0;
	hdr.vscreen = 0;
    hdr.bpline = hdr.x2;
	hdr.vmode = (char)0;
    hdr.nplanes = 1;
	hdr.palinfo = 1;
    memset(hdr.xtra,0x00,54);

    fwrite(&hdr, sizeof(PCXhdr), 1, pf);

    /*
	** Scan the image and pack pixels for the PCX image
    */
	buf = (unsigned char *)malloc(resx * sizeof(unsigned char));

	for (i = 0; i < resy; i++) {
        fprintf(stderr, "%1.0f\%\r", 100.0*(float)i/(float)resy);
		/*
		** Build a scan line of color table indices
		*/
		for (j = 0; j < resx; j++) {
            if (fscanf(f,"%lf %lf %lf",&r,&g,&b) == EOF) {
				fprintf(stderr, "\n\nUnexpected end of input file (%d,%d)\n", i, j);
				exit(1);
			}
            color.r = (byte)(r * nrgbr);
            color.g = (byte)(g * nrgbg);
            color.b = (byte)(b * nrgbb);
			if (color.r > nrgbr) color.r = nrgbr;
			if (color.g > nrgbg) color.g = nrgbg;
            if (color.b > nrgbb) color.b = nrgbb;
			buf[j] = QuantizeColor(octree, &color);
		}
		/*
		** Encode this scan line and write it to the file.  If return is non-0
		** then the write failed
		*/
		if (encline(buf, hdr.x2, pf)) {
			fclose(pf);
			free(buf);
			free(pal);
			return 1;
		}
	}
	/*
	** Store the palette
	*/
	for (i = 0; i < 256; i++) {
        pal[i].r *= 4;
        pal[i].g *= 4;
        pal[i].b *= 4;
	}
	putc(0x0c, pf);
	fwrite(pal, 768, 1, pf);
	fclose(pf);
	free(buf);
	free(pal);
	return 0;
}



/***************************************************************************
**
** encline
**
** Encode a line for RLE PCX image data
*/
int encline(unsigned char *line, int length, FILE *fp)
{
	int             this, last;
    int             srcindex;
	int             runcount;
    int             total;

    last = *line++;
	runcount = 1;
    srcindex = 1;
	total = 0;
    while (srcindex < length) {
		this = *line++;
        srcindex++;
		if (this == last) {
            runcount++;
			if (runcount == 0x3f) {
                if (encput(last, runcount, fp)) {
					return 1;
                }
				total += runcount;
                runcount = 0;
			}
        }
		else {
            if (runcount) {
				if (encput(last,runcount,fp)) {
                    return 1;
				}
                total += runcount;
			}
            last = this;
			runcount = 1;
        }
	}
    if (runcount) {
		if (encput(last, runcount, fp)) {
            return 1;
		}
        total += runcount;
	}
    return 0;
}


int encput(int byt, int cnt, FILE *fid)
{
    if (cnt) {
		if (((byt & 0x00c0) == 0x00c0) || (cnt > 1)) {
            if (putc(0xc0 | cnt, fid) == EOF) {
				return 1;
            }
		}
        if (putc(byt, fid) == EOF) {
			return 1;
        }
	}
    return 0;
}




#include "\mw43d\metquery.c"


