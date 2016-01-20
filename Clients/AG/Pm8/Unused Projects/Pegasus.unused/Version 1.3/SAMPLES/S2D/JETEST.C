/*.T  JETEST.C
 * $Header: /PEGASUS/J2D/JE2DDDW/JETEST.C 5     8/03/97 10:58a Jim $Nokeywords: $
 */

/*   Copyright (c) 1995, Pegasus Imaging Corporation */
#define PC_WATCOM	1 /* allows for wildcard processing, not ANSI */
/* Definition of NOCO must match that in jelevl2.h ! */
#define	NOCO	0  /* 0->use co-routines, 1->no co-routines */
/* If PC_WATCOM is 0 and NOCO is 1 and USE_ASSEMBLY in level2 is 0, then
		the code should be ANSI c */
/* Definition of DEFAULT_ELS_CODER must match that in jelevl2.h ! */
#define	DEFAULT_ELS_CODER	1 /* allows for possibility of using ELS entropy coding */
#define	DEFAULT_PIC2	1 /* allows for possibility of PIC2 input */
#define YUV_OUTPUT	1 /* allows for possibility of uyvy or yuy2 output. Must
				match the definition in jelevl2.h  */

#if !defined(ELS_CODER)
#define ELS_CODER       DEFAULT_ELS_CODER
#endif

#if !defined(PIC2)
#define PIC2       DEFAULT_PIC2
#endif

#if ELS_CODER == 1
	#pragma message("ELS_CODER enabled")
#else
	#pragma message("els_coder DISABLED")
#endif

#if PIC2 == 1
	#pragma message("PIC2 enabled")
#else
	#pragma message("pic2 DISABLED")
#endif

#define	USE_PEG_QRY	1  /* PegasusQuery is not ANSI so set this to 0 to get ANSI compliance */
#define	DEFAULT_GET_BUFF_SIZE	20000

#pragma pack(1) /* If not WATCOM, replace this with some equivalent. The
	structures must be packed with no space, for reading and writing file
	headers, and also for consistency with asm stuctures in some cases. */

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <string.h>
#include    <ctype.h>
#include	<time.h>  /* needed for timing */
#if PC_WATCOM
#include    <dos.h>    /* needed for wildcards. Not ANSI */
#endif

#define YIELD_TEST 0    /* 1/0  => Yield/no yield to other window tasks */
#if defined(WINDOWS) && !defined(_CONSOLE)
		int printf(const char *f, ...) { return 0; }
		int vprintf(const char *f, va_list v) { return 0; }
#undef getchar
		int getchar(void) { return 'C'; }
		int fflush(FILE *f) { return 0; }
		#define EXPORT  __export
#else
		#define EXPORT
#endif

/* The next are from pegasus\include  directory */
#include    "stdtypes.h"
#include    "pic.h"
#include    "errors.h"
#include    "bmp.h"

#if PC_WATCOM
PRIVATE CHAR    src_name[100], src_path[100], des_name[100], des_path[100];
PRIVATE CHAR    *des_ext;
PRIVATE BOOL		wild_card;
#else
PRIVATE CHAR		*src_name,*des_name;
#endif
PRIVATE FILE	*src_file = NULL, *des_file = NULL;
PRIVATE DWORD  bit_count;
PRIVATE DWORD  thumbnail;
PRIVATE BOOL   dither;
PRIVATE BOOL   fast_dither;
PRIVATE BOOL	make_colors;
PRIVATE BOOL	no_smooth = FALSE;
PRIVATE BOOL	make_top_down_dib = FALSE;
PRIVATE BOOL	user_colors = FALSE;
PRIVATE BOOL	test_strips = FALSE;
PRIVATE BOOL	return_y_if_first = FALSE;
PRIVATE BOOL	y_done_first;
PRIVATE LONG  nprimcolors;
PRIVATE LONG  nseccolors;
PRIVATE BOOL    make_gray;
PRIVATE DWORD   gray_levels;
PRIVATE BOOL    skip_copyright;
PRIVATE CHAR    key[9];
PRIVATE CHAR    comment[256];
PRIVATE	DWORD	image_size;
PRIVATE	DWORD	get_buff_size;
PRIVATE	DWORD	width_pad;
PRIVATE	DWORD	num_loops = 1;
PRIVATE	DWORD	crop_x, crop_y, crop_w, crop_h;
PRIVATE	BOOL	crop_flg;
#if YUV_OUTPUT
PRIVATE	DWORD	yuv_output;
PRIVATE BOOL	raw_yuv_output;
#endif
#if NOCO
PRIVATE	LONG	defer_status;
#endif

/*  Local routines */
PRIVATE LONG    expand (void);
#if NOCO
PRIVATE	LONG	  defer_function(PIC_PARM *p, RESPONSE res);
#endif
PRIVATE LONG    read_file (PIC_PARM *p);
PRIVATE LONG    copy_to_file (PIC_PARM *p);
PRIVATE LONG    write_bmp_header (PIC_PARM *p);
PRIVATE LONG    write_color_table (PIC_PARM *p);
PRIVATE LONG    copy_whole_buf_to_file (PIC_PARM *p);
PRIVATE LONG    write_bmp_header_and_col_table(PIC_PARM *p);
PRIVATE	LONG		file_size(FILE *fp);
PRIVATE void    check (BOOL exp, char *fmsg, ... );
PRIVATE LONG    check1 (BOOL exp, char *fmsg, ... );
PRIVATE void    show_copyright (void);
PRIVATE void    print_help (void);
PRIVATE void    parse_parms (int argc, char *argv[]);
#if !PC_WATCOM
PRIVATE	void	ChangeEndianBmpFileHead(BITMAPFILEHEADER *des,BITMAPFILEHEADER *src);
PRIVATE	void	ChangeEndianBmpInfoHead(BITMAPINFOHEADER *des,BITMAPINFOHEADER *src);
#endif

#if 0
#if PC_WATCOM
extern	DWORD	__cdecl my_clock(void); /* for Pentium timing */
#endif
#endif

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ   Test the Pegasus routine interface.                                 บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
#if defined(WINDOWS) && !defined(_CONSOLE)
#define argc _argc
#define argv _argv
extern  INT  argc;
extern  char **argv;
INT     PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrevInst,
						char *szCmdLine, INT nCmdShow)
#else
INT     main (INT argc, char *argv[])
#endif
{
#if PC_WATCOM
	struct  find_t info;
	UINT	n;
#endif
	LONG   status = 0;

			/****/

		if (argc < 2)
		{
				print_help();
				exit(1);
		}

		printf(
				"\nJETEST - Test seq. JPEG expand by converting images to .BMP files.\n"
				"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
		);

		/*  Get command line arguments. */
		parse_parms(argc, argv);

		if ( sizeof(size_t) != sizeof(long) )
			test_strips = TRUE;	/* must do it in 16-bit */

		if (!skip_copyright)
			show_copyright();

#if PC_WATCOM
		n = 0;
		if (_dos_findfirst(src_name, _A_NORMAL, &info) == 0) {
				do {
						strcpy(src_name, src_path);   strcat(src_name, info.name);
						if (wild_card) {
								strcpy(des_name, des_path);   strcat(des_name, info.name);
								strcpy(strchr(des_name, '.'), des_ext);
						}
						status = expand();
#if 0
						if(status < 0)
							break;
#endif
						n++;
				} while ( _dos_findnext(&info) == 0);
		}
		else
			printf("\n image not found.\n", n);
		printf("\nThere were %d images expanded or attempted.\n", n);
#else
		status = expand();
#endif
		if(status)
			printf("status on last image = %d\n",status);
		return (0);
}

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE LONG  expand (void)
{
	PIC_PARM  p;
	RESPONSE    res;
	LONG	len;
	LONG	ct;
	LONG	local_status;
	BOOL	peg_qry_status = FALSE;
	LONG		start_time, end_time;
#if 0
	LONG	clocks_per_millisec = (133*1000)/30;
#endif
	BYTE *temp;

		/****/

	src_file = des_file = NULL;
	temp = NULL;
	y_done_first = FALSE;

	memset(&p, 0, sizeof(PIC_PARM));    /* important: set all default values 0 */
		/* ALSO important -- following must be set even for PegasusQuery */
		/* beginning version 13 */
	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;
	p.ParmVerMinor = 1;
#if NOCO
	p.DeferFn = &defer_function;
#endif

#if YUV_OUTPUT
	if(yuv_output)
	{
		make_top_down_dib = ~make_top_down_dib;
	}
#endif

	/*  Open the JPEG file */
	src_file = fopen(src_name, "rb");
	if(check1(src_file != NULL, "Could not open %s", src_name)) goto quit;

	/*  Allocate GetBuff and place into it the information to be queried */
	if(num_loops > 1)
	{ /* allocate a full_size buffer and do not do strips */
		if(check1(test_strips == FALSE,"Can't do strips if num_loops > 1")) goto quit;
		if(check1(return_y_if_first == FALSE,"Can't do return_y_if_first if num_loops > 1")) goto quit;
		get_buff_size = file_size(src_file) + 2;
	}
	if(return_y_if_first)
	{
		if(check1(test_strips == FALSE,"Can't do strips if return_y_if_first")) goto quit;
	}
	p.Get.Start = (BYTE *) malloc(get_buff_size);
	if(check1(p.Get.Start != NULL, "Out of memory for get buffer")) goto quit;
	p.Get.End = p.Get.Start + get_buff_size;

	/*  read in header data */
	len = fread(p.Get.Start, sizeof(BYTE), get_buff_size, src_file);
	if(check1(ferror(src_file) == 0, "Error reading input file")) goto quit;
	if(len < get_buff_size)
		p.Get.QFlags |= Q_EOF;

#if 0
	start_time = my_clock();
#endif
#if 1
	start_time = clock();
#endif

for(ct = 0;ct < num_loops;++ct)
{
	p.Get.Rear = p.Get.Start + len;
	p.Get.Front = p.Get.Start;
	peg_qry_status = FALSE;
#if USE_PEG_QRY
	/* note: PegasusQuery is not ANSI */
	p.u.QRY.BitFlagsReq = QBIT_BICOMPRESSION;
	peg_qry_status = PegasusQuery(&p);
	memset(&p.u.QRY, 0, sizeof(p.u.QRY));
	if(peg_qry_status)
	{
	if(check1(p.Head.biCompression == BI_PICJ  ||
#if PIC2
				p.Head.biCompression == BI_PC2J ||
#endif
#if ELS_CODER
				p.Head.biCompression == BI_JPGE	||
#if PIC2
				p.Head.biCompression == BI_PC2E	||
#endif
#endif
				p.Head.biCompression == BI_JPEG, "Must be JPEG compressed")) goto quit;
	}
#endif

#if 0
	/* This is to skip to FFD8 in case there is garbage up front */
	while(p.Get.Front < p.Get.End)
	{
		if(*(p.Get.Front) == 0xFF && *(p.Get.Front + 1) == 0xD8)
			break;
		++(p.Get.Front);
	}
	if(check1(p.Get.Front < p.Get.End,"No JPEG start of image found")) goto quit;
#endif



	/*  Set up the uncompressed image characteristics. */

	/*  Here is where a user's palette could be substituted for the file */
	/*  image's palette. If you do this, you must set p.Head.biClrUsed */
	/* and p.Head.biClrImportant, and load p.ColorTable. Note that */
	/* make_colors will later override user_colors, so this is pointless */
	/* if make_colors is set. */

	if(user_colors)
	{
		; /* stub */
	}

	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;   p.ParmVerMinor = 1;
#if ELS_CODER == 1
	p.Op = OP_SE2D;
#else
	p.Op = OP_S2D;
#endif
	p.Comment = comment;
	p.CommentSize = sizeof(comment);
	p.u.S2D.PicFlags = 0;
#if YUV_OUTPUT
	if(yuv_output)
	{
		p.u.S2D.PicFlags |= PF_YuvOutput;
		p.u.S2D.YuvOutputType = yuv_output;
		bit_count = 16;
		make_gray = FALSE;
	}
#endif
	p.u.S2D.DibSize = bit_count;
	if(width_pad != 0)
	{
		if(check1(p.Head.biWidth * ((bit_count + 7)/8) <= width_pad, "width_pad too small")) goto quit;
		p.u.S2D.PicFlags |= PF_WidthPadKnown;
		p.u.S2D.WidthPad = (width_pad + 3) & (-4); /* just to make a BMP file */
	}
	if(test_strips)
		p.u.S2D.PicFlags |= PF_YieldPut;
	if(return_y_if_first)
		p.u.S2D.PicFlags |= PF_ReturnYIfFirst;
	if (dither)
		p.u.S2D.PicFlags |= PF_Dither;
	if (fast_dither)
		p.u.S2D.DitherType = 1; /* only applies to indexed color mode for now */
	if (make_gray)
		p.u.S2D.PicFlags |= PF_ConvertGray;
	memcpy(p.KeyField, key, 8);
	p.u.S2D.Thumbnail = thumbnail;

	if(no_smooth)
		p.u.S2D.PicFlags |= PF_NoCrossBlockSmoothing;

	if((make_gray || p.Head.biBitCount <= 8) && gray_levels)
	{
		p.u.S2D.GraysToMake = gray_levels;
		/*p.u.S2D.PicFlags |= PF_MakeColors; */
	}

	p.u.S2D.PrimClrToMake = p.u.S2D.SecClrToMake = 0;
	if(p.u.S2D.DibSize == 8 && !make_gray)
		p.u.S2D.PrimClrToMake = nprimcolors;
	else if(p.u.S2D.DibSize == 4 && !make_gray)
		p.u.S2D.SecClrToMake = nseccolors;

	/************************************************************* */
	/* if (no good (colors in file or user colors) or make_colors set) AND dibsize <=8 AND */
	/* output is not gray AND input is not gray, */
	/* set PF_MakeColors flag and set PF_CreateDibWithMadeColors flag */
	/* so we will make and use optimal colors. */
	/************************************************************* */
#if 0
	if(!peg_qry_status)
		p.Head.biBitCount = 24; /* just assume this (a guess) if no peg qry */
#endif

#if 1
	if( (make_colors || p.Head.biClrUsed == 0 ||
		(p.u.S2D.DibSize == 4 && p.Head.biClrImportant > 16 &&
		(p.Head.biClrUsed - p.Head.biClrImportant == 0))) &&
		p.u.S2D.DibSize <=8 &&
		!make_gray && p.Head.biBitCount > 8) /* latter says input is not gray */
			p.u.S2D.PicFlags |= (PF_MakeColors | PF_CreateDibWithMadeColors |
							PF_OnlyUseMadeColors);
#endif

#if	USE_PEG_QRY
	if(peg_qry_status)
	{
		if(p.Head.biHeight <= 0)
		{
			if(make_top_down_dib == FALSE)
				p.Put.QFlags |= Q_REVERSE;
		}
		else
		{
			if(make_top_down_dib == TRUE)
				p.Put.QFlags |= Q_REVERSE;
		}
	}
	else
#endif
		if(make_top_down_dib == FALSE)
			p.Put.QFlags |= Q_REVERSE;


	/* ************************************************************** */
	/* ** Now p.Head.biClrUsed and p.Head.biClrImportant MUST be set to */
	/* zero UNLESS there are user colors, because a non_zero value */
	/* of p.Head.biClrUsed is the way of signaling to level 2 that */
	/* there are user colors. */

	if(!user_colors)
	{
		p.Head.biClrUsed = p.Head.biClrImportant = 0;
	}

	p.u.S2D.StripSize = 0; /* let level 2 figure this out */
		/* (if it is non-zero here, level 2 will change this if necessary */
		/*  to the nearest value which makes sense). */
	/* Note: StripSize is set only by INIT for sequential. The */
	/*       progressive routine recalculates this in EXEC. */
  if(crop_flg)
  {
		p.Flags |= F_Crop;
    if(crop_w == 0)
			crop_w = 65535; /* init will cut this back to full width */
		if(crop_h == 0)
			crop_h = 65535;
    p.CropWidth = crop_w;
		p.CropHeight = crop_h;
    p.CropXoff = crop_x;
    p.CropYoff = crop_y;
  }

#if	NOCO
	/*  Initialize Jpeg decompression */
	defer_status = 0;
	res = Pegasus(&p, REQ_INIT);
	/* BMP info header for output dib is set during init. */
	if(check1(defer_status >= 0, "Error %d in DeferFn.", defer_status)) goto quit;
	if(check1(res != RES_ERR, "Error number %d in Pegasus.",p.Status)) goto quit;
	if(check1(p.Status >= 0, "Error %d in Pegasus.", p.Status)) goto quit;


	/*  Allocate DIB output buffer during EXEC. Set to NULL now (important!) */
	if(ct == 0)
	{
		temp = NULL;
		p.Put.Start = NULL; /* This will cause RES_PUT_NEED_SPACE response later. */
	}

	else
	{
		p.Put.End = p.Put.Start + image_size;
		if(p.Put.QFlags & Q_REVERSE)
		{
			p.Put.Rear = p.Put.Front = p.Put.End; /*put buff runs backwards */
		}
		else
			p.Put.Rear = p.Put.Front = p.Put.Start;
	}

	/*  Decompress the file. */
	res = Pegasus(&p, REQ_EXEC);
	/* check(defer_status >= 0, "Error %d in DeferFn.", defer_status); */
/*	check(res != RES_ERR, "Error number %d in Pegasus.",p.Status); output what you can */

	/* Let Pegasus close things(in seq. jpeg, everything is already closed, */
	/* but this would not necessarily be true for progressive) */
	/* res = Pegasus(&p,REQ_TERM); // This can only return RES_DONE, and */
	/* does not change p->Status. Not needed in seq. */


#else

	/*  Initialize Jpeg decompression */
	local_status = 0;
	res = Pegasus(&p, REQ_INIT);
	/* BMP info header for output dib is set during init. */
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA) {
			local_status = read_file(&p);
		} else if (res == RES_PUT_DATA_YIELD) {
			local_status = copy_to_file(&p);
		} else if (res == RES_ERR) {
			if(check1(FALSE, "Error number %d ",p.Status)) goto quit;
		} else {
			if(check1(FALSE, "Unexpected or unknown response %d ", res)) goto quit;
		}
		if(check1(p.Status >= 0, "Error %d in Pegasus", p.Status)) goto quit;
		if(check1(local_status >= 0, "Error %d in Pegasus", local_status)) goto quit;
		res = Pegasus(&p, REQ_CONT);
	}
	if(check1(p.Status >= 0, "Error %d in Pegasus", p.Status)) goto quit;

	/*  Allocate DIB output buffer during EXEC. Set to NULL now (important!) */
	if(ct == 0)
	{
		temp = NULL;
		p.Put.Start = NULL; /* This will cause RES_PUT_NEED_SPACE response. */
	}
	else
	{ /* reset, but do not reallocate, put buffer, for speed test loops. */
		p.Put.End = p.Put.Start + image_size;
		if(p.Put.QFlags & Q_REVERSE)
		{
			p.Put.Rear = p.Put.Front = p.Put.End; /*put buff runs backwards */
		}
		else
			p.Put.Rear = p.Put.Front = p.Put.Start;
	}

	/*  Decompress the file. */
	res = Pegasus(&p, REQ_EXEC);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA)
			local_status = read_file(&p);
		else if (res == RES_PUT_NEED_SPACE)
		{
			if(p.Put.Start == NULL)
			{
				/*  Allocate DIB output buffer now */

				image_size = (( (p.Head.biWidth * p.u.S2D.DibSize + 31) >> 3 ) & -4) *
								 labs(p.Head.biHeight);
				if(test_strips)
				{
					des_file = fopen(des_name, "wb");
					if(check1(des_file != NULL,"Could not create %s", des_name)) goto quit;
					if(check1(write_bmp_header(&p) == 0,"write error")) goto quit;
					printf("\n%3d%% done", 0);
					fflush(stdout);
					p.Put.Start = (BYTE *) malloc(p.u.S2D.StripSize);
					if(check1(p.Put.Start != NULL,"Out of space for put buffer")) goto quit;
					p.Put.End = p.Put.Start + p.u.S2D.StripSize;
					if(p.Put.QFlags & Q_REVERSE)
					{
						p.Put.Rear = p.Put.Front = p.Put.End; /*put buff runs backwards */
						if(check1(fseek(des_file, image_size, SEEK_CUR) == 0,"Bad des file seek")) goto quit;
					}
					else
						p.Put.Rear = p.Put.Front = p.Put.Start;
				}
				else
				{
					temp = (BYTE *) malloc(image_size + 4);
					p.Put.Start = temp;
/*					p.Put.Start = (BYTE *) malloc(image_size); */
					/*p.Put.Start = (BYTE *) calloc(image_size,sizeof(char)); */
					if(check1(p.Put.Start != NULL, "Out of space for put buffer")) goto quit;
					p.Put.End = p.Put.Start + image_size;
					if(p.Put.QFlags & Q_REVERSE)
					{
						p.Put.Rear = p.Put.Front = p.Put.End; /*put buff runs backwards */
					}
					else
						p.Put.Rear = p.Put.Front = p.Put.Start;
				}
			}
		}
		else if (res == RES_PUT_DATA_YIELD)
		{
			if(test_strips)
				local_status = copy_to_file(&p);
		}
		else if (res == RES_YIELD)
			;
		else if (res == RES_COLORS_MADE)
		{
			if(p.u.S2D.PicFlags & PF_CreateDibWithMadeColors)
			{
				/* one could set the colors now */
				;
			}
		}
		else if (res == RES_Y_DONE_FIRST)
		{
			y_done_first = TRUE;
			des_file = fopen(des_name, "wb");
			if(check1(des_file != NULL, "Could not create %s", des_name)) goto quit;
			p.Head.biHeight *= 2;
			p.Head.biSizeImage *= 2;
			if(check1(write_bmp_header_and_col_table(&p) >= 0, "error writing file header or color table")) goto quit;
			if(check1(copy_whole_buf_to_file(&p) >= 0, "error writing file")) goto quit;
			if(p.Put.QFlags & Q_REVERSE)
			{
				p.Put.Rear = p.Put.Front = p.Put.End; /*put buff runs backwards */
			}
			else
				p.Put.Rear = p.Put.Front = p.Put.Start;
		}
		else if (res == RES_ERR)
			break; /* output what you can. check(FALSE, "Error number %d ",p.Status); */
		else
			if(check1(FALSE, "Unexpected or unknown response %d ", res)) goto quit;
		if(local_status < 0)
			break;
		res = Pegasus(&p, REQ_CONT);
	}
	Pegasus(&p, REQ_TERM);

#endif

}
#if 1
	end_time = clock();
	printf("start time was %lu clocks\n", start_time );
	printf("end time was %lu clocks\n", end_time );
	printf("execution time was %lu millisec.\n",
			((end_time - start_time)*1000)/CLOCKS_PER_SEC );
#endif
#if 0
	end_time = my_clock();
	printf("start time was %lu clocks\n", start_time );
	printf("end time was %lu clocks\n", end_time );
	printf("execution time was %lu millisec.\n",
			(end_time - start_time)/clocks_per_millisec );
#endif

	if (p.Status)  printf("Status %d in Pegasus.\n", p.Status);
	if(p.Status == ERR_JUNK_BYTES_IN_HEADER)
		printf("Junk bytes found in header were skipped over.\n");
	if(p.Status == ERR_JUNK_BYTES_BEFORE_RESTART)
		printf("Junk bytes found before restart marker(s) were skipped over.\n");
	if(p.Status == ERR_RESTART_ATTEMPT_AFTER_BAD_DATA)
		printf("Bad data or unexpected end of data in entropy segment, restart attempted.\n");
#if NOCO
	if (defer_status)  printf("defer status %d in Pegasus.\n", defer_status);
#else
	if (local_status)  printf("local status %d in Pegasus.\n", local_status);
#endif

	/*  Finish .BMP file */

	if(test_strips)
	{
		if(check1(write_color_table(&p) >= 0, "error writing color table")) goto quit;
	}
	else
	{
		if(!y_done_first)
		{
			printf("filled buffer... now writing out file\n");
			des_file = fopen(des_name, "wb");
			if(check1(des_file != NULL, "Could not create %s", des_name)) goto quit;
			if(check1(write_bmp_header_and_col_table(&p) >= 0, "error writing file header or color table")) goto quit;
		}
		if(check1(copy_whole_buf_to_file(&p) >= 0, "error writing file")) goto quit;
	}

	printf("\n");
	if (p.CommentLen > 0)
	{
		if(p.CommentLen < p.CommentSize)
			p.Comment[p.CommentLen] = '\0';
		else
			p.Comment[p.CommentSize-1] = '\0';
		printf("Comment: %s\n", p.Comment);
	}
	if (p.u.S2D.AppFieldLen > 0)
		printf("Application field length is %d\n", p.u.S2D.AppFieldLen);

	printf("Expanded file %s into %s.\n", src_name, des_name);
	if(p.u.S2D.PicFlags & PF_UsedMMX)
		printf("Used MMX processing.\n");

quit:
	/*  Close file, free memory. */
	if (src_file != NULL)
		fclose(src_file);
	if (des_file != NULL)
		fclose(des_file);

#if 0
	if(p.Put.Start != NULL)
	{
		free(p.Put.Start);
		p.Put.Start = NULL;
	}
#endif
#if 1
	if(temp != NULL)
	{
		free(temp);
		temp = NULL;
	}
#endif
	if(p.Get.Start != NULL)
	{
		free(p.Get.Start);
		p.Get.Start = NULL;
	}
	return (p.Status);
}


/* The following function will be used by Pegasus to ask for data,space, etc.
 It should return non-zero number if the App wants to terminate,
 otherwise return 0.  In case of an error here, the App may set p->Status
 if he chooses in addition to returning a non-zero number; Pegasus will then
 terminate without resetting p->Status, and without looking at p->Status. */
#if NOCO
PRIVATE	LONG	defer_function(PIC_PARM *p, RESPONSE res)
{

	if (res == RES_GET_NEED_DATA)
		defer_status = read_file(p);
	else if (res == RES_PUT_NEED_SPACE)
	{
		if(p->Put.Start == NULL)
		{
			/*  Allocate DIB output buffer now */

			image_size = (( (p->Head.biWidth * p->u.S2D.DibSize + 31) >> 3 ) & -4) *
							 labs(p->Head.biHeight);
			if(test_strips)
			{
				des_file = fopen(des_name, "wb");
				if(des_file == NULL)
				{
					printf("Could not create %s", des_name);
					defer_status = ERR_FILE_OPEN_ERROR;
					p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
					return(defer_status);
				}
				defer_status = write_bmp_header(p);
				if(defer_status)
				{
					p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
					return(defer_status);
				}
				printf("\n%3d%% done", 0);
				fflush(stdout);
				p->Put.Start = (BYTE *) malloc(p->u.S2D.StripSize);
				if(p->Put.Start == NULL)
				{
					defer_status = ERR_OUT_OF_SPACE;
					p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
					return(defer_status);
				}
				p->Put.End = p->Put.Start + p->u.S2D.StripSize;
				if(p->Put.QFlags & Q_REVERSE)
				{
					p->Put.Rear = p->Put.Front = p->Put.End; /*put buff runs backwards */
					if( fseek(des_file, image_size, SEEK_CUR) != 0)
					{
						defer_status = ERR_BAD_SEEK;
						p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
						return(defer_status);
					}
				}
				else
					p->Put.Rear = p->Put.Front = p->Put.Start;
			}
			else
			{
				p->Put.Start = (BYTE *) malloc(image_size);
				if(p->Put.Start == NULL)
				{
					defer_status = ERR_OUT_OF_SPACE;
					p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
					return(defer_status);
				}
				p->Put.End = p->Put.Start + image_size;
				if(p->Put.QFlags & Q_REVERSE)
				{
					p->Put.Rear = p->Put.Front = p->Put.End; /*put buff runs backwards */
				}
				else
					p->Put.Rear = p->Put.Front = p->Put.Start;
			}
		}
		else
		{ /* need to empty put buffer */
			if(test_strips)
				defer_status = copy_to_file(p);
			else
				defer_status = ERR_NOT_ENOUGH_BUFF_SPACE;
		}
	}
	else if (res == RES_PUT_DATA_YIELD)
	{
		if(test_strips)
			defer_status = copy_to_file(p);
	}
	else if (res == RES_YIELD)
		;
	else if (res == RES_COLORS_MADE)
	{
		if(p->u.S2D.PicFlags & PF_CreateDibWithMadeColors)
		{
			/* one could set the colors now */
			;
		}
	}
	else if (res == RES_Y_DONE_FIRST)
	{
		y_done_first = TRUE;
		des_file = fopen(des_name, "wb");
		if(des_file == NULL)
		{
			printf("Could not create %s", des_name);
			defer_status = ERR_FILE_OPEN_ERROR;
			p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
			return(defer_status);
		}
		p->Head.biHeight *= 2;
		p->Head.biSizeImage *= 2;
		defer_status = write_bmp_header_and_col_table(p);
		if(defer_status < 0)
		{
			p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
			return(defer_status);
		}
		defer_status = copy_whole_buf_to_file(p);
		if(defer_status < 0)
		{
			p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
			return(defer_status);
		}
		if(p->Put.QFlags & Q_REVERSE)
		{
			p->Put.Rear = p->Put.Front = p->Put.End; /*put buff runs backwards */
		}
		else
			p->Put.Rear = p->Put.Front = p->Put.Start;
	}
	else
		defer_status = ERR_UNKNOWN_RESPONSE;

	if(defer_status)
		p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
	return(defer_status);
}
#endif

PRIVATE LONG   read_file (PIC_PARM *p)
{
	LONG    avail,len;

		/****/

	if ( p->Get.Rear == p->Get.Front) /* empty buffer */
		p->Get.Front = p->Get.Rear = p->Get.Start;

	if ( p->Get.Rear >= p->Get.Front)
	{
		avail = p->Get.End - p->Get.Rear;
		if(avail > 0)
		{
			len = fread(p->Get.Rear, sizeof(BYTE), avail, src_file);
			if(ferror(src_file))
				return(ERR_BAD_READ);
			if(len < avail)
			{
				p->Get.QFlags |= Q_EOF;
				p->Get.Rear += len;
			}
			else
			{
				if(p->Get.Front > p->Get.Start)
					p->Get.Rear = p->Get.Start;
				else
					p->Get.Rear = p->Get.End;
			}
		}
		else
		{
			if(p->Get.Front > p->Get.Start)
				p->Get.Rear = p->Get.Start;
		}
	}
	if(p->Get.Rear < p->Get.Front)
	{
		avail = p->Get.Front - p->Get.Rear - 1;
		len = fread(p->Get.Rear, sizeof(BYTE), avail, src_file);
		if(ferror(src_file))
			return(ERR_BAD_READ);
		p->Get.Rear += len;
		if(len < avail)
			p->Get.QFlags |= Q_EOF;
	}
#if defined( WINDOWS) & YIELD_TEST
	/* yield test */
		MSG msg;
		if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage((LPMSG) &msg);
				DispatchMessage((LPMSG) &msg);
	}
#endif
	return(0); /* no error */
}


PRIVATE LONG  copy_to_file (PIC_PARM *p)
{
	long siz;

		/****/

		/*  Usually the JPEG image is top-down, a BMP is bottom-up so must reverse */

	if (p->Put.QFlags & Q_REVERSE)       /* Need to reverse */
	{
		siz = p->Put.End - p->Put.Rear;

		if (fseek(des_file, -siz, SEEK_CUR) != 0)
			return(ERR_BAD_SEEK);
		if (fwrite(p->Put.Rear, sizeof(BYTE), siz, des_file) < siz)
			return(ERR_BAD_WRITE);
		if (fseek(des_file, -siz, SEEK_CUR) != 0)
			return(ERR_BAD_SEEK);

		p->Put.Rear = p->Put.End;
	}
	else
	{
		siz = p->Put.Rear - p->Put.Start;
		if (fwrite(p->Put.Start, sizeof(BYTE), siz, des_file) < siz)
			return(ERR_BAD_WRITE);
		p->Put.Rear = p->Put.Start;
	}
	printf("\r%3d%% done", p->PercentDone);
	fflush(stdout);
#if defined(WINDOWS) & YIELD_TEST
		/* yield test */
	MSG msg;
	if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE)) {
						TranslateMessage((LPMSG) &msg);
						DispatchMessage((LPMSG) &msg);
	}
#endif
  return(0); /*no error */
}

PRIVATE LONG   copy_whole_buf_to_file (PIC_PARM *p)
{

		/****/

	if(fwrite(p->Put.Start, sizeof(BYTE), image_size, des_file) != image_size)
		return(ERR_BAD_WRITE);
	return(0); /* no error */
}


/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ   Create an uncompressed BMP file.  Fill in the information using     บ */
/*  ณ   data from the compressed BMP image and from user parameters.        บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE LONG    write_bmp_header (PIC_PARM *p)
{
	BITMAPFILEHEADER   des_fhead;
	BITMAPINFOHEADER	*pbif;
#if !PC_WATCOM
	BYTE test_bytes[4];
	DWORD	*dp;
	BITMAPINFOHEADER	bif;
#endif

		/****/

#if YUV_OUTPUT
	if(raw_yuv_output)
		return(0);
#endif

	des_fhead.bfType = 0x4D42;      /* "BM" */
	des_fhead.bfOffBits = sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER) + p->Head.biClrUsed*4;
	des_fhead.bfSize = des_fhead.bfOffBits + p->Head.biSizeImage;
	des_fhead.bfReserved1 = 0;
	des_fhead.bfReserved2 = 0;
	pbif = &(p->Head);
#if !PC_WATCOM
	dp = (DWORD *)test_bytes;
	*dp = 0;
	test_bytes[0] = 1;
	if(((*dp) >> 24) != 0) /* see if a big endian machine */
	{ /* it's BigEndian, need to flip words and dwords */
		pbif = &bif;
		ChangeEndianBmpFileHead(&des_fhead, &des_fhead);
		ChangeEndianBmpInfoHead(pbif,&(p->Head));
	}
#endif
	if(fwrite(&des_fhead, sizeof(BYTE), sizeof(BITMAPFILEHEADER), des_file) !=  sizeof(BITMAPFILEHEADER)  ||
		 fwrite(pbif, sizeof(BYTE), sizeof(BITMAPINFOHEADER), des_file) !=  sizeof(BITMAPINFOHEADER) )
		return(ERR_BAD_WRITE);



	fseek(des_file, p->Head.biClrUsed*4, SEEK_CUR);
	return(0); /* no error */
}

PRIVATE LONG    write_color_table (PIC_PARM *p)
{

#if YUV_OUTPUT
	if(raw_yuv_output)
		return(0);
#endif

	if (p->Head.biClrUsed > 0)
	{
		fseek(des_file, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
							SEEK_SET);
		if(fwrite(p->ColorTable, sizeof(BYTE), p->Head.biClrUsed*4, des_file) !=  p->Head.biClrUsed*4)
			return(ERR_BAD_WRITE);
	}
	return(0); /* no error */
}

PRIVATE LONG    write_bmp_header_and_col_table(PIC_PARM *p)
{
	BITMAPFILEHEADER    des_fhead;
	BITMAPINFOHEADER	*pbif;
#if !PC_WATCOM
	BYTE test_bytes[4];
	DWORD	*dp;
	BITMAPINFOHEADER	bif;
#endif

		/****/

#if YUV_OUTPUT
	if(raw_yuv_output)
		return(0);
#endif

	des_fhead.bfType = 0x4D42;      /* "BM" */
	des_fhead.bfOffBits = sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER) + p->Head.biClrUsed*4;
	des_fhead.bfSize = des_fhead.bfOffBits + p->Head.biSizeImage;
	des_fhead.bfReserved1 = 0;
	des_fhead.bfReserved2 = 0;
	pbif = &(p->Head);
#if !PC_WATCOM
	dp = (DWORD *)test_bytes;
	*dp = 0;
  test_bytes[0] = 1;
	if(((*dp) >> 24) != 0) /* see if a big endian machine */
  { /* it's BigEndian, need to flip words and dwords */
  	pbif = &bif;
    ChangeEndianBmpFileHead(&des_fhead, &des_fhead);
		ChangeEndianBmpInfoHead(pbif,&(p->Head));
  }
#endif
	if(fwrite(&des_fhead, sizeof(BYTE), sizeof(BITMAPFILEHEADER), des_file) !=  sizeof(BITMAPFILEHEADER)  ||
		 fwrite(&(p->Head), sizeof(BYTE), sizeof(BITMAPINFOHEADER), des_file) !=  sizeof(BITMAPINFOHEADER) )
     return(ERR_BAD_WRITE);
	if (p->Head.biClrUsed > 0)
	{
		fseek(des_file, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
				SEEK_SET);
		if(fwrite(p->ColorTable, sizeof(BYTE), p->Head.biClrUsed*4, des_file) !=  p->Head.biClrUsed*4)
			return(ERR_BAD_WRITE);
	}
  return(0); /* no error */
}


/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ       Check that exp is TRUE.  If it is not then print the formatted  บ */
/*  ณ   message fmsg, and return -1.                            บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE LONG    check1 (BOOL exp, char *fmsg, ... )
{
	LONG	status = 0;
	va_list marker;

		/****/

	if (!exp)
	{
		va_start(marker, fmsg);
		printf("Image terminated: ");
		vprintf(fmsg, marker);
		printf(".\n");
		status = -1;
	}
	return(status);
}

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ       Check that exp is TRUE.  If it is not then print the formatted  บ */
/*  ณ   message fmsg, and terminate program.                            บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    check (BOOL exp, char *fmsg, ... )
{
	va_list marker;

		/****/

	if (!exp)
	{
		va_start(marker, fmsg);
		printf("Image terminated: ");
		vprintf(fmsg, marker);
		printf(".\n");
		exit(1);
	}
}


/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ   Show the copyright information.                                     บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    show_copyright (void)
{
		printf("\nJETEST - Test seq. JPEG expand by converting images to .BMP files.\n"
					 "(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n\n"
			 "    (Voice) 813-875-7575  (Fax) 813-875-7705\n\n"
					 "Program Version 3.00\n\n"
				);
		printf("Press 'C'(ENTER) to continue.. ");
		fflush(stdout);
		while ('C' != getchar()) ;    /* Await user response */
		printf("\n");
}


/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ       Print help message.                                             บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    print_help (void)
{
	printf(
	"\nJETEST - Test seq. JPEG expand by converting images to .BMP files.\n"
	"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
#if PC_WATCOM
	"\nusage:  J32TEST  input_filename[.PIC]  [destination[.BMP]]  {options}\n"
#else
	"\nusage:  J32TEST  input_filename  output_filename  {options}\n"
#endif
	"where options are:\n"
	"  -B#    output bits-per-pixel (# = 4, 8, 16, 24 or 32 - default is 24)\n"
#if YUV_OUTPUT
	"  -U#    create yuv output;0 = UYVY, 1 = YUY2 (microsoft media subtypes\n"
	"         with horizontal 2 to 1 downsampling of chrominance; UYVY means\n"
	"         bytes are in order u1,y1,v1,y2,u2,y3,v2,y4  etc, and YUY2 means\n"
	"         bytes are in order y1,u1,y2,v1,y3,u2,y4,v2 etc.)\n"
	"         The -B and -G options are ignored.\n"
	"         If -H (below)is not set, for this test program the output file will\n"
	"         have a (not legitimate) bmp header in which the biCompression field\n"
	"         will be BI_UYVY or BI_YUY2 (see pic.h) and the biBitCount field\n"
	"         will be 16.\n"
	"         For this format, the default will be a top-down dib; this can be\n"
	"         reversed using the -D option below.\n"
	"  -H     (for yuv output only). No header, just a raw file output.\n"
#endif
	"  -G[#]  convert image to # gray-scale levels (2 ๓ # ๓ 256) - default is 256\n"
	"  -N     no dithering of the image\n"
	"  -F     use fast error-diffusion dither for indexed color modes\n"
	"         note: this is ignored for now except in all-c code version.\n"
	"  -M     make colors for colormapped image, don't use ones in input file\n"
	"  -P#    use # colors in palette (2 ๓ |#| ๓ 256), # < 0 if secondary palette\n"
	"  -T[#]  create thumbnail;3 => 1/64(default),2 => 1/16 size,1 -> 1/4 size.\n"
	"  -C     no cross-block smoothing(default is to smooth)\n"
	"  -D     Causes all output dibs except yuv to be top-down; the default is\n"
	"         to make a bottom-up dib(because of windows). For yuv output, however,\n"
	"         the default is top-down and -D reverses this to bottom up.\n"
	"  -Z     write file output in strips(slower, as a test or to save mem.)\n"
	"  -E#    input buffer size, min. 1024(default 20000,ignored if -L used).\n"
	"  -W#    width_pad(Must be at least true image width*bytesperpixel.).\n"
	"         For this test program,rounded up to mult. of 4 to make valid BMP.\n"
	"  -L#    num loops(1 <= # <= 1000)for speed test,not to be used with -Z option\n"
	"  -RX#   x coord. of cropped rectangle (default 0)\n"
	"  -RY#   y coord. of cropped rectangle(default 0)\n"
	"  -RW#   width of cropped rectangle(default full image)\n"
	"  -RH#   height of cropped rectangle(default full image)\n"
	"  -Kkey  Decrypts the image file with the given key, key is up to 8 chars.\n"
	"  -Y     As a test, if image is multiscan with only Y as first scan, return\n"
	"         that in beginning of output file. Can't do this with -Z or -L options.\n"
	"   (press return) \n"
	);
	getchar();
	printf(
	"Choosing the -B (bit_length) option  4 or 8 creates a color-mapped palette;\n"
	"If the image is color and a palette is not available, one will be created;\n"
	"The -P option is only meaningful if colors are being made for a colormapped\n"
	"image, either because -M is chosen or the image has no valid colors.\n"
	"If -P is not set and colors are being made, the default is 236 colors for an\n"
	"8-bit dib and 16 colors for a 4-bit dib. If image is monochrome then 16 or 236\n"
	"gray levels are used. Choosing the -B option 16 or 24 creates an RGB image; if\n"
	"the source is gray-scale then the bit size is set to 8 with 256 gray levels.\n"
	"The -G option forces the image to gray-scale and sets the number of gray\n"
	"levels.  If no number is given, the default number is 16 or 236 depending\n"
	"on the bit levels being 4 or 8. The -N option surpresses dithering; dithering\n"
	"is only done if bits per pixel <= 16.\n"
#if PC_WATCOM
	"The file_name may be a wild card name - if so, all images are compressed\n"
	"sharing the same options and the destination name is the destination path\n"
	"followed by the file_name with .BMP extension.  If the source is not a wild\n"
	"card name, the destination name is the destination with default .BMP extension."
#endif
	);
}


/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ       Parse the parameters.                                           บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    parse_parms (int argc, char *argv[])
{
	LONG     i, nc;
#if PC_WATCOM
	char    ci_drive[5], ci_dir[60], ci_fname[20], ci_ext[10], *pp;
	BOOL    have_name;
#endif

  	/****/

#if PC_WATCOM
	wild_card = (strchr(argv[1], '*') != NULL  ||
							 strchr(argv[1], '?') != NULL);
	_fullpath(src_name, argv[1], sizeof(src_name));   strupr(src_name);
	_splitpath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
	if (ci_ext[0] == '\0')  strcpy(ci_ext, ".PIC");
	_makepath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
	_makepath(src_path, ci_drive, ci_dir, "", "");
	des_ext = ".BMP";
#else
	check(argv[1][0] != '-' && argv[1][0] != '/',"need input filename");
	check(argv[2][0] != '-' && argv[2][0] != '/',"need output filename");
	src_name = argv[1];
	des_name = argv[2];
#endif

	bit_count = 24;   gray_levels = 256;   thumbnail =  0;
	nprimcolors = 0; nseccolors = 0; make_colors = FALSE;
	test_strips = FALSE;
	skip_copyright = make_gray = FALSE;   dither = TRUE;
	fast_dither = FALSE;
	user_colors = FALSE; /* Just a stub for now. */
	no_smooth = FALSE;
	make_top_down_dib = FALSE;
	get_buff_size = DEFAULT_GET_BUFF_SIZE;
	width_pad = 0;
	num_loops = 1;
	crop_x = crop_y = crop_w = crop_h = 0;
	crop_flg = FALSE;
	return_y_if_first = FALSE;
#if YUV_OUTPUT
	yuv_output = 0;
	raw_yuv_output = FALSE;
#endif
	memset(key, 0, sizeof(key));

	for (i = 2;  i < argc;  ++i)
	{
		if (argv[i][0] == '-'  ||  argv[i][0] == '/')
		{
			switch (toupper(argv[i][1]))
			{
				case 'B' :
						bit_count = atoi(argv[i]+2);
						break;
#if YUV_OUTPUT
				case 'U' :
						yuv_output = atoi(argv[i]+2);
						if(yuv_output == 0)
							yuv_output = BI_UYVY;
						else if(yuv_output == 1)
							yuv_output = BI_YUY2;
						break;
				case 'H' :
						raw_yuv_output = TRUE;
						break;
#endif
				case 'G' :
						make_gray = TRUE;
						gray_levels = atoi(argv[i]+2);
						if(gray_levels == 0)
							gray_levels = 256;
						break;
				case 'K' :
						strncpy(key, argv[i]+2, 8);
						break;
				case 'M' :
						make_colors = TRUE;
						break;
				case 'N' :
						dither = FALSE;
						break;
				case 'F' :
						fast_dither = TRUE;
						break;
				case 'C' :
						no_smooth = TRUE;
						break;
				case 'D' :
						make_top_down_dib = TRUE;
						break;
				case 'Y' :
						return_y_if_first = TRUE;
						break;
				case 'Z' :
						test_strips = TRUE;
						break;
				case 'P' :
						nc = atoi(argv[i]+2);
						if(nc > 0)
							nprimcolors = nc;
						else
							nseccolors = nc;
						break;
				case 'T' :
						thumbnail = atoi(argv[i]+2);
						if (thumbnail == 0)  thumbnail = 3;
						check(thumbnail >= 1  &&  thumbnail <= 3, "Bad thumbnail");
						break;
				case 'E' :
						get_buff_size = atoi(argv[i]+2);
						check(get_buff_size >= 128, "Bad input buff size");
						break;
				case 'W' :
						width_pad = atoi(argv[i]+2);
						check(width_pad > 0, "Bad width_pad size");
						break;
				case 'L' :
						num_loops = atoi(argv[i]+2);
						check(num_loops > 0 && num_loops <= 1000, "Bad num loops");
						break;
				case 'R' :
						crop_flg = TRUE;
						switch (toupper(argv[i][2]))
						{
							case 'X' :
									crop_x = atoi(argv[i]+3);
									break;
							case 'Y' :
									crop_y = atoi(argv[i]+3);
									break;
							case 'W' :
									crop_w = atoi(argv[i]+3);
									break;
							case 'H' :
									crop_h = atoi(argv[i]+3);
									break;
							default :
									check(FALSE, "Unknown crop option %s", argv[i]);
									break;
						}
						break;
				case '!' :
						skip_copyright = TRUE;
						break;
				default  :
						check(FALSE, "Unknown option %s", argv[i]);
						break;
			}
		}
	}
#if PC_WATCOM
	have_name = (argc > 2  &&  argv[2][0] != '-'  &&  argv[2][0] != '/');
	if (have_name) {
					_fullpath(des_name, argv[2], 100);   strupr(des_name);
  } else {
		strcpy(des_name, src_name);
  }
  if (wild_card) {
      if (have_name) {
          pp = des_name + strlen(des_name);
					if (pp[-1] != '\\'  &&  pp[-1] != ':')  *((WORD *) pp) = 0x005C;
      }
      _splitpath(des_name, ci_drive, ci_dir, ci_fname, ci_ext);
			_makepath(des_path, ci_drive, ci_dir, "", "");
  } else {
      pp = strchr(des_name, '.');
      if (have_name) {
					if (pp == NULL)  strcat(des_name, des_ext);
			} else {
					strcpy(pp, des_ext);
			}
	}
#endif
}


PRIVATE	LONG	file_size(FILE *fp)
{
	LONG	save_pos, size_of_file;
		/****/

  save_pos = ftell(fp);
  fseek(fp,0L,SEEK_END);
  size_of_file = ftell(fp);
  fseek(fp, save_pos, SEEK_SET);
  return(size_of_file);
}
#if !PC_WATCOM
PRIVATE	void	ChangeEndianBmpFileHead(BITMAPFILEHEADER *des,BITMAPFILEHEADER *src)
{
	WORD dw;
	DWORD	dd;

		/****/
/* Note: des and src are allowed to be same */

	dw = src->bfType;
	des->bfType = (dw << 8) | (dw >> 8);
	dd = src->bfSize;
	des->bfSize = (dd << 24) | ((dd & 0xFF00) << 8) | ((dd & 0xFF0000) >> 8) |
									(dd >> 24);
	dw = src->bfReserved1;
	des->bfReserved1 = (dw << 8) | (dw >> 8);
	dw = src->bfReserved2;
	des->bfReserved2 = (dw << 8) | (dw >> 8);
	dd = src->bfOffBits;
	des->bfOffBits = (dd << 24) | ((dd & 0xFF00) << 8) | ((dd & 0xFF0000) >> 8) |
									(dd >> 24);
}
PRIVATE	void	ChangeEndianBmpInfoHead(BITMAPINFOHEADER *des,BITMAPINFOHEADER *src)
{
	WORD dw;
	DWORD	dd;
	LONG	dl;

		/****/
/* Note: des and src are allowed to be same */

	dd = src->biSize;
	des->biSize = (dd << 24) | ((dd & 0xFF00) << 8) | ((dd & 0xFF0000) >> 8) |
									(dd >> 24);
	dl = src->biWidth;
	des->biWidth = (dl << 24) | ((dl & 0xFF00) << 8) | ((dl & 0xFF0000) >> 8) |
									(dl >> 24);
	dd = (DWORD)src->biHeight;
	des->biHeight = (LONG)((dd << 24) | ((dd & 0xFF00) << 8) | ((dd & 0xFF0000) >> 8) |
									(dd >> 24));
	dw = src->biPlanes;
	des->biPlanes = (dw << 8) | (dw >> 8);
	dw = src->biBitCount;
	des->biBitCount = (dw << 8) | (dw >> 8);
	dd = src->biCompression;
  des->biCompression = (dd << 24) | ((dd & 0xFF00) << 8) | ((dd & 0xFF0000) >> 8) |
									(dd >> 24);
	dd = src->biSizeImage;
	des->biSizeImage = (dd << 24) | ((dd & 0xFF00) << 8) | ((dd & 0xFF0000) >> 8) |
									(dd >> 24);
	dl = src->biXPelsPerMeter;
  des->biXPelsPerMeter = (dl << 24) | ((dl & 0xFF00) << 8) | ((dl & 0xFF0000) >> 8) |
									(dl >> 24);
	dl = src->biYPelsPerMeter;
  des->biYPelsPerMeter = (dl << 24) | ((dl & 0xFF00) << 8) | ((dl & 0xFF0000) >> 8) |
									(dl >> 24);
	dd = src->biClrUsed;
  des->biClrUsed = (dd << 24) | ((dd & 0xFF00) << 8) | ((dd & 0xFF0000) >> 8) |
									(dd >> 24);
	dd = src->biClrImportant;
  des->biClrImportant = (dd << 24) | ((dd & 0xFF00) << 8) | ((dd & 0xFF0000) >> 8) |
									(dd >> 24);
}
#endif
