/*.T JPTEST.C
 * $Header: /D2J/D2JEDDW/JPACK.C 2     5/11/97 10:39a Jim $Nokeywords: $
 */

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ    Use JPTEST to compress a set of files to .PIC or .JPG files.       บ */
/*  ณ                                                                       บ */
/*  ณ    MUST be compiled with FLAT model.                                  บ */
/*  ณ                                                                       บ */
/*  รฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ */
#define PC_WATCOM	1 /* allows for wildcard processing, not ANSI */
/* Definition of NOCO must match that in jplevl2.h ! */
#define	NOCO	0  /* 0->use co-routines, 1->no co-routines */
/* If PC_WATCOM is 0 and NOCO is 0 and USE_ASSEMBLY in level2 is 0, then
		the code should be ANSI c */
#define	YUV_INPUT		1 /* 1 allows raw uyvy, yuy2 or yvyu (microsoft video media subtypes) as input format */
/* Definition of ELS_CODER must match that in jplevl2.h ! */
#define	DEFAULT_ELS_CODER	1 /* allows for possibility of using ELS entropy coding */

#if !defined(ELS_CODER)
#define ELS_CODER   DEFAULT_ELS_CODER
#endif

#if ELS_CODER == 1
	#pragma message("ELS_CODER enabled")
#else
	#pragma message("els_coder DISABLED")
#endif

#define	MIN_PUT_QUEUE_SIZE	1024
#define DEFAULT_PUT_QUEUE_SIZE 64000

#pragma pack(1)

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <ctype.h>
#include    <string.h>
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

#include    "stdtypes.h"
#include    "targa.h"
#include    "bmp.h"
#include    "errors.h"
#include	  "pic.h"

typedef struct { BYTE b, g, r; } BGR;

/*  Globals for main */
PRIVATE	DWORD   SizeImage;
PRIVATE	BYTE     *StripBuff;
PRIVATE	DWORD	StripBuffSize;
PRIVATE FILE   *src_file = NULL, *des_file = NULL;
PRIVATE LONG		image_start;
#if PC_WATCOM
PRIVATE char    src_name[100], src_path[100], des_name[100], des_path[100];
PRIVATE char    *des_ext;
PRIVATE BOOL		wild_card;
#else
PRIVATE char		*src_name,*des_name;
#endif
PRIVATE	DWORD		num_loops = 1;
PRIVATE	DWORD		get_buff_size;
PRIVATE	DWORD		put_buff_size;
PRIVATE BOOL    jpeg_file, include_jcm, dont_make_new_colors;
PRIVATE BOOL    use_YCbCr_for_colors;
PRIVATE BOOL    pic2_file;
PRIVATE	BOOL		make_new_colors, absolutely_no_output_colors;
PRIVATE BOOL    optimize_huff_codes;
PRIVATE BOOL    use_els_coder;
PRIVATE BOOL    suppress, skip_copyright;
PRIVATE BOOL		test_strips = FALSE;
#if YUV_INPUT
PRIVATE BOOL		raw_yuv_in = FALSE;
PRIVATE	DWORD		raw_yuv_in_width, raw_yuv_in_height, raw_yuv_in_format;
#endif
PRIVATE WORD    lum_opt, chrom_opt, subsample, vorient;
PRIVATE WORD    num_prim_colors_req, num_sec_colors_req;
PRIVATE BYTE    comment[256];
PRIVATE BYTE    qtable[192];
PRIVATE char    key[9], QTableName[100];
PRIVATE char    *msg[4] =
			{"", "ซ Hor chrom subsample,", "ซ Hor/Ver chrom subsample," ,
							"ซ Ver chrom subsample,"};

#if NOCO
PRIVATE	defer_status;
#endif

PRIVATE long  pack (void);
#if NOCO
PRIVATE	LONG	  defer_function(PIC_PARM *p, RESPONSE res);
#endif
PRIVATE LONG   read_file (PIC_PARM *p);
PRIVATE LONG   copy_to_file (PIC_PARM *p);
PRIVATE BOOL    pal_is_gray (PIC_PARM *p);
PRIVATE void    rgbt_to_rgbquad (RGBQUAD *des, RGBTRIPLE *src, DWORD n);
#if PC_WATCOM
PRIVATE WORD    crc (BYTE  *pp);
#endif
PRIVATE WORD    show_copyright (BOOL visible);
PRIVATE void    print_help (void);
PRIVATE void    parse_parms (INT argc, char *argv[]);
PRIVATE void    check (BOOL exp, char *fmsg, ... );
PRIVATE	LONG		file_size(FILE *fp);
PRIVATE	void 		MakeLittleEndianDword(BYTE *des, DWORD dd);
PRIVATE	void 		MakeLittleEndianWord(BYTE *des, WORD dw);

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ   Test the Pegasus routine interface.  (OP_D2S)                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
#ifdef  WINDOWS

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
#endif
	LONG    status;
	UINT    n;

  	/****/

	if (argc < 2) {
		print_help();   exit(1);
	}

	printf(
		"\nJPTEST - version 3.60, Pack a set of BMP or TARGA images to JPEG or PIC.\n"
		"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
	);

	/*  sets some entries in options structure, and command line arguments. */
	parse_parms(argc, argv);

		if ( sizeof(size_t) != sizeof(long) )
				test_strips = TRUE;     /* must do it in 16-bit */

	if (!skip_copyright)
		show_copyright(TRUE);
	if (QTableName[0] != '\0')    /*  User supplied a file name holding Q-table values */
	{ /* qtable is assumed NOT in zig_zag order (unlike what's in jpg file ) */
			FILE    *QFile;
			LONG	qq,stat = 0;
			int	len,c;
			/****/
			QFile = fopen(QTableName, "rt");
			check(QFile != NULL, "Q-table %s could not be opened", QTableName);
			memset(qtable,0,192);
			n = 0;
			while(n < 192)
			{
				len = fscanf(QFile, " %d", &qq);
				if(len != 1)
				{
					if(len == EOF)
						break;
					c = getc(QFile);
					if(c == '#')
					{ /* comment to the end of line */
						while(1)
						{
							c = getc(QFile);
							if(c == '\n'|| c == EOF)
								break;
						}
						if(c == EOF)
							break;
						continue;
					}
					else
						break;
				}
				if(qq < 1 || qq > 255)
				{
					printf("Program terminated:illegal quantization value(s)\n");
					stat = -1;
					break;
				}
				qtable[n++] = qq;
			}
			fclose(QFile);
			QFile = NULL;
			if(stat != 0)
				exit(1);
			check(n == 64 || n == 128 || n == 192, "incorrect number of quantization values");
	}

#if PC_WATCOM
	n = 0;
	if (_dos_findfirst(src_name, _A_NORMAL, &info) == 0) {
		do {
			strcpy(src_name, src_path);   strcat(src_name, info.name);
			if (wild_card) {
				strcpy(des_name, des_path);   strcat(des_name, info.name);
				strcpy(strchr(des_name, '.'), des_ext);
			}
			status = pack();
      if(status != 0)
      	break;
			n++;
		} while ( _dos_findnext(&info) == 0);
	}
	printf("\nThere were %d images packed.\n", n);
#else
	status = pack();
#endif
	if(status != 0)
		printf("status = %d\n",status);
  return(0);
}

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE long  pack (void)
{
	PIC_PARM  p;
	RESPONSE    res;
	BITMAPFILEHEADER	BF;
  BITMAPINFOHEADER    BMI;
	TARGA_HEAD  thead;
	DWORD	type,bmi_size,num_colors_from_input;
	BYTE    *pp, *q;
	DWORD       in_size, out_size, delta, len, first_len;
	LONG		loop_ct;
  BYTE	*get_buff;

		/****/

  src_file = des_file = NULL;

	/*  Open the source file */
	src_file = fopen(src_name, "rb");
	check(src_file != NULL, "Could not open %s", src_name);

	/*  Allocate Get Buff and place into it the information to be queried */
  if(num_loops > 1)
  {
	   /* allocate a full_size buffer and do not do strips */
    check(test_strips == FALSE,"Can't do strips if num_loops > 1");
		if(put_buff_size < 200000) /* large enough to avoid file output in loop */
			put_buff_size = 200000;
  }
  if(!test_strips)
		get_buff_size = file_size(src_file);
  else
		get_buff_size = 10000;
	get_buff = malloc(get_buff_size); /* big enough for query */
	check(get_buff != NULL, "Out of memory for query buffer");

	/*  read */
	first_len = fread(get_buff,sizeof(BYTE),get_buff_size,src_file);
	check(ferror(src_file) == 0, "Error reading input file");


  /* If num_loops > 1, don't do any file io during loop, for speed test */

for(loop_ct = 0;loop_ct < num_loops;++loop_ct)
{
	memset(&p, 0, sizeof(PIC_PARM));    /* important: set all default values 0 */
    /* ALSO important to set these */
	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;   p.ParmVerMinor = 1;
#if NOCO
	defer_status = 0;
	p.DeferFn = &defer_function;
#endif

	p.Get.Start = get_buff;
	p.Get.End = p.Get.Start + get_buff_size;
	p.Get.Front = p.Get.Start;
	p.Get.Rear = p.Get.Start + first_len;
	p.u.QRY.BitFlagsReq = QBIT_BICOMPRESSION;
#if YUV_INPUT
	if(!raw_yuv_in)
#endif
	{
		check(PegasusQuery(&p), "Invalid PegasusQuery");
		/* Note: the following would wipe out any pic flags that were set */
		memset(&p.u.QRY, 0, sizeof(p.u.QRY));
		check(p.Head.biCompression == BI_RGB  ||
				p.Head.biCompression == BI_BMPO ||
				p.Head.biCompression == BI_TGA1 ||
				p.Head.biCompression == BI_TGA2 ||
				p.Head.biCompression == BI_TGA3, "Must be uncompressed BMP or TARGA");
	}
#if YUV_INPUT
	else
	{
		if(raw_yuv_in)
		{
			check(subsample == SS_211 || subsample == SS_411, "output subsampling must be no finer than input.\n");
			check((raw_yuv_in_width & 1) == 0,"input width must be even for yuv with this subsampling");
			p.Head.biSizeImage = 4*((raw_yuv_in_width+1)/2)*raw_yuv_in_height;
			p.Head.biCompression = raw_yuv_in_format;
		}
		p.Head.biWidth = raw_yuv_in_width;
		p.Head.biHeight = -raw_yuv_in_height;
	}
#endif
	/*  Set up the uncompressed image characteristics. */
	if(first_len < get_buff_size || !test_strips)
		p.Get.QFlags |= Q_EOF;
#if 0
	if(num_loops == 1)
		p.u.D2S.PicFlags |= PF_YieldPut;
#endif
	p.VisualOrient = vorient;        /* 0 = O_normal; */
#if ELS_CODER == 1
	p.Op = OP_D2SE;
#else
	p.Op = OP_D2S;
#endif
	if (strlen(comment) != 0)
	{
		p.CommentLen = strlen(comment) + 1;
		p.Comment = comment;
	}
	memcpy(p.KeyField, key, 8);
	p.u.D2S.PrimClrToMake = num_prim_colors_req;
	p.u.D2S.SecClrToMake = num_sec_colors_req;
	type = p.Head.biCompression;
	num_colors_from_input = p.Head.biClrUsed;
	switch (type)
	{
		case BI_RGB  :      /* BMP RGB */
		case BI_BMPO :      /* BMP old-style RGB (uncompressed) */
			memcpy(&BF, p.Get.Start, sizeof(BITMAPFILEHEADER));
			bmi_size = (type == BI_RGB ? sizeof(BITMAPINFOHEADER)
										 : sizeof(BITMAPCOREHEADER));
			if (num_colors_from_input > 0)
			{    /* get colors */
				if (type != BI_BMPO)
				{
					memcpy(p.ColorTable,p.Get.Start + sizeof(BITMAPFILEHEADER) + bmi_size, num_colors_from_input << 2);
				}
				else
				{
					memcpy(p.ColorTable,p.Get.Start + sizeof(BITMAPFILEHEADER) + bmi_size, num_colors_from_input*3);
					pp = (BYTE *) p.ColorTable + p.Head.biClrUsed * 3;
					q = (BYTE *) p.ColorTable + p.Head.biClrUsed * 4;
					while (pp > (BYTE *) p.ColorTable)
					{
						*--q = 0;
						*--pp = *--q;   *--pp = *--q;   *--pp = *--q;
					}
				}
			}
     	p.Get.Front += BF.bfOffBits;
			p.Head.biCompression = BI_RGB;   /* Now treat as new BMP */
			if(p.Head.biBitCount <= 8  &&  pal_is_gray(&p))
				p.u.D2S.PicFlags |= PF_IsGray;
			break;
		case BI_TGA1 :      /* Targa color mapped */
		case BI_TGA2 :      /* Targa RGB */
		case BI_TGA3 :      /* Targa Gray-scale */
			memcpy(&thead, p.Get.Start,sizeof(TARGA_HEAD));
      p.Get.Front += sizeof(TARGA_HEAD);
			if (thead.id_size)
			{
				if (!suppress)
				{
					p.Comment = comment;
					memcpy(p.Comment,p.Get.Front,thead.id_size);
					p.Comment[thead.id_size] = '\0';
					p.CommentLen = thead.id_size + 1;
				}
      	p.Get.Front += thead.id_size;
			}
			if (thead.cm_type)
			{        /* read in color map */
				check(thead.cm_elt_size == 24, "Color map must be 24 bit BGR");
				memcpy(p.ColorTable,p.Get.Front,num_colors_from_input*3);
				p.Get.Front += num_colors_from_input*3;
				rgbt_to_rgbquad(p.ColorTable, (RGBTRIPLE *)p.ColorTable, num_colors_from_input);
			}
			if(thead.image_type == 3  || (thead.ips <= 8  &&  pal_is_gray(&p)))
				p.u.D2S.PicFlags |= PF_IsGray;
			p.u.D2S.PicFlags |= PF_NoDibPad; /* Targa lines are not padded */
			break;
#if YUV_INPUT
		case BI_UYVY :
		case BI_YUY2 :
/*		case BI_YVYU : */
			p.u.D2S.PicFlags |= PF_NoDibPad; /* don't think these are padded ? */
			break;
#endif
		default :
			check(FALSE, "Bad image type");
	}
	image_start = p.Get.Front - p.Get.Start; /* offset of pixel info */

	/*  Elements specified by the user as command line options */
	p.u.D2S.LumFactor = lum_opt;
	p.u.D2S.ChromFactor =  chrom_opt;
  if(p.u.D2S.PicFlags & PF_IsGray)
  	subsample = 0;
	p.u.D2S.SubSampling = subsample;
#if ELS_CODER
	if(use_els_coder)
  {
  	optimize_huff_codes = FALSE;
    jpeg_file = TRUE;
		p.u.D2S.PicFlags |= PF_ElsCoder;
  }
#endif
	if (optimize_huff_codes)
		p.u.D2S.PicFlags |= PF_OptimizeHuff;
	if (QTableName[0] != '\0')
	{
		p.u.D2S.QTable = (BYTE PICHUGE *) qtable;
		lum_opt = chrom_opt = 0;
	}
	p.u.D2S.JpegType = (jpeg_file ? JT_RAW : JT_PIC);
	if ( pic2_file )
    p.u.D2S.JpegType = JT_PIC2;


		/* Palette section. TRICKY! see usage message. */
	if(p.u.D2S.PicFlags & PF_IsGray)
  	absolutely_no_output_colors = TRUE;
  if((num_colors_from_input == 0) && (!jpeg_file || include_jcm))
  	make_new_colors = TRUE;
  if(jpeg_file && !include_jcm)
  	make_new_colors = FALSE; /* just in case user set -P without setting -M,
      for a JPEG file. No need to make colors since they will not go in
			output. See usage message. */
  if(dont_make_new_colors)
  	make_new_colors = FALSE; /* incompatible requests, dont_make_new_colors
				takes precedence. */
  if(absolutely_no_output_colors) /* this takes prcedence over all. */
  {
  	make_new_colors = FALSE;
		p.Head.biClrUsed = p.Head.biClrImportant = 0; /* NEW. Input palette,
		  if any, will NOT go into output file. */
    include_jcm = FALSE;
  }
	if(make_new_colors)
  {
		p.u.D2S.PicFlags |= PF_MakeColors;
		if(use_YCbCr_for_colors)
			p.u.D2S.PicFlags |= PF_UseYCbCrForColors;
  }

	if(include_jcm)
		p.u.D2S.PicFlags |= PF_App1Pal;

	/*  Determine if a file needs to be reversed from top-to-bottom. */
	/* If biHeight is > 0 now, the input file must be bottom-up, and */
	/*   needs to be reversed to conform to JFIF which is always top-down. */
		/*  (this must be set before the call to Setup ) */
	if (p.Head.biHeight >= 0)
	{   /* JFIF files are always top-down */
		p.Get.QFlags |= Q_REVERSE;
		p.Head.biHeight = -p.Head.biHeight; /* so .pic file will be top-down */
	}

	SizeImage = p.Head.biSizeImage;

	/*  setup output buff */

	p.Put.Start = (BYTE  *) malloc(put_buff_size);
	check(p.Put.Start != NULL, "Out of memory for put buff");
	p.Put.Front = p.Put.Rear = p.Put.Start;
	p.Put.End = p.Put.Start + put_buff_size;
		/* setup output file */
	if(num_loops == 1)
	{
		des_file = fopen(des_name, "w+b");
		check(des_file != NULL, "Can't create file %s", des_name);
	}

#if	NOCO
	/*  Initialize Jpeg compression */
	res = Pegasus(&p, REQ_INIT);
  check(defer_status >= 0, "Error %d in DeferFn.", defer_status);
	check(res != RES_ERR, "Error number %d in Pegasus.",p.Status);
	check(p.Status >= 0, "Error %d in Pegasus.", p.Status);
	if (p.Status > 0)  printf("Status %d in Pegasus.\n", p.Status);

	if(test_strips)
  {
		/*  Allocate space for a StripBuff containing at least StripSize bytes */
    /* NOTE: in general, if the Get queue is to be used as a queue, where
    the producer places bytes back at the start before consumer has used
    all the bytes to the end, then the Get queue MUST BE A MULTIPLE OF THE
    STRIP_SIZE, AND THE DATA MUST BE PUT IN IN MULTIPLES OF STRIP_SIZE,
		STARTING AT Get.Start ! If the Get queue is only used linearly, the only
		requirement is that it be at least StripSize bytes, and the data placed
		in the Get queue by the producer be in exact multiples of StripSize bytes
		(except	for the last strip, which may be less).
		The point is that the convert	routines in Pegasus consume data
		contiguously per strip. */

	  free(p.Get.Start); /* was used for just xquery */
		p.Get.Start = (BYTE  *) malloc(p.u.D2S.StripSize);
	  check(p.Get.Start != NULL, "Out of memory for strip");
	  p.Get.Rear = p.Get.Front = p.Get.Start;
		p.Get.End = p.Get.Start + p.u.D2S.StripSize;
	  if(p.Get.QFlags & Q_REVERSE)
	  {
		  if(fseek(src_file, SizeImage + image_start, SEEK_SET) != 0)
				return(ERR_BAD_SEEK);
	  }
    else
	  {
		  if(fseek(src_file,image_start, SEEK_SET) != 0)
			  return(ERR_BAD_SEEK);
	  }
  }
  else
  {
	  if(p.Get.QFlags & Q_REVERSE)
	  {
		  p.Get.Rear = p.Get.Front;
		  p.Get.Front += SizeImage;
	  }
  }

	StripBuff = p.Get.Start;
	StripBuffSize = p.u.D2S.StripSize;

  if(test_strips)
  {
		printf("\n%3d%% done", 0);
		fflush(stdout);
  }

	/*  Compress the file. */
	res = Pegasus(&p, REQ_EXEC);
  check(defer_status >= 0, "Error %d in DeferFn.", defer_status);
	check(res != RES_ERR, "Error number %d in Pegasus.",p.Status);
	check(p.Status >= 0 || p.Status == ERR_UNEXPECTED_EOF, "Error %d in Pegasus.", p.Status);
	/* go ahead and finish if premature EOF */

  /* Let Pegasus close things */
  /* res = Pegasus(&p,REQ_TERM); This can only return RES_DONE, and  */
	/* does not change p->Status. Not needed in seq. */

	if (p.Status != 0)  printf("Status %d in Pegasus.\n", p.Status);

#else

	res = Pegasus(&p, REQ_INIT); /* Does not get data. */
	while (res != RES_DONE)
	{
		if (res == RES_PUT_DATA_YIELD)
		{
				p.Status = copy_to_file(&p);
		}
		else if (res == RES_PUT_NEED_SPACE)
		{
    		if(num_loops > 1)
					check(FALSE," Put buffer too small");
        else
					p.Status = copy_to_file(&p);
		}
		else if (res == RES_YIELD)
		{
			;
		}
		else
		{
			check(res != RES_ERR, "Error number %d", p.Status);
			check(FALSE,"Unknown Response  %d",res);
		}
		res = Pegasus(&p, REQ_CONT);
	}

	if(test_strips)
  {
		/*  Allocate space for a StripBuff containing at least StripSize bytes */
	  free(p.Get.Start); /* was used for just xquery */
		p.Get.Start = (BYTE  *) malloc(p.u.D2S.StripSize);
	  check(p.Get.Start != NULL, "Out of memory for strip");
	  p.Get.Rear = p.Get.Front = p.Get.Start;
	  p.Get.End = p.Get.Start + p.u.D2S.StripSize;
	  if(p.Get.QFlags & Q_REVERSE)
	  {
		  if(fseek(src_file, SizeImage + image_start, SEEK_SET) != 0)
			  return(ERR_BAD_SEEK);
	  }
    else
	  {
			if(fseek(src_file,image_start, SEEK_SET) != 0)
			  return(ERR_BAD_SEEK);
	  }
  }
	else
  {
	  if(p.Get.QFlags & Q_REVERSE)
	  {
		  p.Get.Rear = p.Get.Front;
		  p.Get.Front += SizeImage;
	  }
  }

	StripBuff = p.Get.Start;
	StripBuffSize = p.u.D2S.StripSize;

  if(test_strips)
  {
		printf("\n%3d%% done", 0);
		fflush(stdout);
  }

	/*  Compress the file. */
	res = Pegasus(&p, REQ_EXEC);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA)
		{
    	if(test_strips)
				p.Status = read_file(&p);
      else
      	check(FALSE,"Unexpected end of input data");
		}
		else if (res == RES_PUT_DATA_YIELD)
		{
				p.Status = copy_to_file(&p);
		}
		else if (res == RES_PUT_NEED_SPACE)
		{
    		if(num_loops > 1)
					check(FALSE," Put buffer too small");
        else
					p.Status = copy_to_file(&p);
		}
    else if (res == RES_POKE)
    {
			/* write out remainder of put buffer, if any -- there won't
          be a poke until after pack has output everything.
          flushing here saves us having to remember the current
          file position so we can seek back for the final write */
      if((len = p.Put.Rear - p.Put.Front) > 0)
        {
        check(fwrite(p.Put.Front,sizeof(BYTE),len,des_file) == len,
            "output file write error");
        p.Put.Front = p.Put.Rear;     /* it's empty now */
        }
      check(fseek(des_file, p.SeekInfo, SEEK_SET) == 0,
          "output file seek error");
      len = p.Put.RearEnd - p.Put.FrontEnd;
      check(fwrite(p.Put.FrontEnd, sizeof(BYTE), len, des_file) == len,
          "output file write error");
    }
		else if (res == RES_YIELD)
		{
			;
		}
		else
		{
			check(res != RES_ERR, "Error number %d", p.Status);
			check(FALSE,"Unknown Response  %d",res);
		}
		res = Pegasus(&p, REQ_CONT);
	}
	/* in case of premature EOF, go ahead and finish */
	check(p.Status == ERR_NONE || p.Status == ERR_UNEXPECTED_EOF, "Error %d in Pegasus", p.Status);

	Pegasus(&p, REQ_TERM);
#endif
	if(loop_ct != num_loops - 1)
  {
	  free(p.Put.Start); /* get ready for another loop (speed test) */
	  p.Put.Start = NULL;
  }
}

	if(num_loops > 1)
  {
	  /* setup output file */
	  des_file = fopen(des_name, "w+b");
		check(des_file != NULL, "Can't create file %s", des_name);
  }
  /* write out put buffer, if anything left there */
	if((len = p.Put.Rear - p.Put.Front) > 0)
		if (fwrite(p.Put.Front,sizeof(BYTE),len,des_file) < len)
			return(ERR_BAD_WRITE);

	in_size = file_size(src_file);
	fclose(src_file); src_file = NULL;
	out_size = file_size(des_file);

	if (p.u.D2S.JpegType == JT_PIC )
	{
			/*  Update file header and info header for size fields */
		check(fseek(des_file, 0, SEEK_SET) == 0, "Bad seek");
		fread(&BF, sizeof(BITMAPFILEHEADER),1,des_file);
		check(ferror(des_file) == 0, "Bad read");
		fread(&BMI, sizeof(BITMAPINFOHEADER),1,des_file);
		check(ferror(des_file) == 0,"Bad read");

    MakeLittleEndianDword((BYTE *)&(BF.bfSize),out_size); /* (BMP is little-endian) */
		q = (BYTE *)&BF.bfOffBits;
		len = q[0] | (DWORD)(q[1]) << 8 | (DWORD)(q[2]) << 16 |
		 					(DWORD)(q[3]) << 24;
		delta = out_size - len;
    MakeLittleEndianDword((BYTE *)&BMI.biSizeImage, delta);

		/* If color palette will be put in BMP header, reset num. cols, in
		   case fewer were made than requested. */
      /* If not making colors, any colors would already be in output file */
		if (p.Head.biClrUsed >= 1 && !(p.u.D2S.PicFlags & PF_App1Pal) &&
					(p.u.D2S.PicFlags & PF_MakeColors) )
		{
			MakeLittleEndianDword((BYTE *)&BMI.biClrUsed, p.Head.biClrUsed);
			MakeLittleEndianDword((BYTE *)&BMI.biClrImportant, p.Head.biClrImportant);
		}
		check(fseek(des_file, 0, SEEK_SET) == 0, "Bad seek");
		len = fwrite(&BF, sizeof(BITMAPFILEHEADER),1,des_file);
		check(len == 1, "Bad write");
		len = fwrite(&BMI, sizeof(BITMAPINFOHEADER),1,des_file);
		check(len == 1,"Bad write");

		/*  Write color palette if required and not already written */
      /* If not making colors, any colors would already be in output file */
		if (p.Head.biClrUsed >= 1 && !(p.u.D2S.PicFlags & PF_App1Pal) &&
					(p.u.D2S.PicFlags & PF_MakeColors) )
		{
				/* Write out made color palette */
			check(fseek(des_file, sizeof(BITMAPFILEHEADER) + 68L, SEEK_SET) == 0,
						"Bad seek");
			len = fwrite(p.ColorTable, sizeof(BYTE),p.Head.biClrUsed << 2,des_file);
			check(len == (p.Head.biClrUsed << 2),	"Bad color palette write");
		}
	}
    /* If not making colors, any colors would already be in output file */
	if (p.Head.biClrUsed >= 1 && (p.u.D2S.PicFlags & PF_App1Pal) &&
					(p.u.D2S.PicFlags & PF_MakeColors) )
	{
			/* put color palette in JPEG APP1 marker. -  rewind first. */
		BYTE      work[256*3+4];
		RGBQUAD   *s = p.ColorTable;
		RGBTRIPLE *d = (RGBTRIPLE *) work;
		int       i;

		MakeLittleEndianWord(work, p.Head.biClrUsed); /* lobyte first - these are defined to be
				little endian. */
		MakeLittleEndianWord(work + 2, p.Head.biClrImportant);
		d = (RGBTRIPLE *)(work + 4);
		for (i = p.Head.biClrUsed;  i > 0;  i--)
		{
		  d->rgbtRed = s->rgbRed;
		  d->rgbtGreen = s->rgbGreen;
		  (d++)->rgbtBlue = (s++)->rgbBlue;
		}
		if (p.u.D2S.JpegType == JT_PIC)
		{
			check(fseek(des_file, sizeof(BITMAPFILEHEADER) + 68
						+ 40, SEEK_SET) == 0, "Bad seek");
		}
		else
		{
			check(fseek(des_file, 40, SEEK_SET) == 0, "Bad seek");
		}
		len = fwrite(work,sizeof(BYTE), (BYTE *) d - work,des_file);
		check(len == (BYTE *) d - work,	"Bad colortable write");
	}

	delta = in_size - out_size;
	printf( "\nPacked %s, (%u x %u  %u-bit), %s\n"
			"    to %s, lum=%u, chrom=%u,%s\n"
			"    original size = %lu, compressed size = %lu, save %lu (%ld%%)\n",
			src_name,  p.Head.biWidth, labs(p.Head.biHeight),
			p.Head.biBitCount, msg[subsample],
			des_name, lum_opt, chrom_opt, (p.u.D2S.PicFlags & PF_MakeColors ? " Color Table created," : ""),
			in_size, out_size, delta, (delta*100 + in_size/2)/in_size);
	if(p.u.D2S.PicFlags & PF_OptimizeHuff)
		printf( "    Optimized Huffman Codes\n");
	if(p.u.D2S.PicFlags & PF_ElsCoder)
		printf( "    Used ELS entropy coding\n");
	if (QTableName[0] != '\0')
		printf( "    User supplied quantization tables\n");

	free(p.Put.Start);   p.Put.Start = NULL;
	free(p.Get.Start);   p.Get.Start = NULL;

	fclose(des_file); des_file = NULL;
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
  {
    if(test_strips)
			defer_status = read_file(p);
    else
      check(FALSE,"Unexpected end of input data");
	}
	else if (res == RES_PUT_DATA_YIELD)
		defer_status = copy_to_file(p);
	else if (res == RES_PUT_NEED_SPACE)
  {
		if(num_loops > 1)
			defer_status = ERR_PUT_BUFF_TOO_SMALL;
    else
			defer_status = copy_to_file(p);
  }
	else if (res == RES_YIELD)
			;
	else
		defer_status = ERR_UNKNOWN_RESPONSE;
  if(defer_status)
    p->Status = defer_status; /* optional, just a place for App to record errors also if he chooses */
  return(defer_status);
}
#endif


PRIVATE LONG  read_file (PIC_PARM *p)
{
	DWORD   len;
	long	file_posn;

		/****/

	if (p->Get.QFlags & Q_REVERSE)
	{
		file_posn = ftell(src_file);
		len = file_posn - image_start;
		if(StripBuffSize < len)
			len = StripBuffSize;
		if(fseek(src_file, -len, SEEK_CUR) != 0)
			return(ERR_BAD_SEEK);
		if(len < StripBuffSize)
			p->Get.QFlags |= Q_EOF;
		fread(StripBuff,sizeof(BYTE),len,src_file);
		if(ferror(src_file))
			return(ERR_BAD_READ);
		if(fseek(src_file, -len, SEEK_CUR) != 0)
			return(ERR_BAD_SEEK);
		p->Get.Front = StripBuff + len;
		p->Get.Rear = StripBuff;
	}
	else
	{
		len = fread(StripBuff,sizeof(BYTE),StripBuffSize,src_file);
		if(ferror(src_file))
			return(ERR_BAD_READ);
		if(len < StripBuffSize)
			p->Get.QFlags |= Q_EOF;
		p->Get.Front = StripBuff;
		p->Get.Rear = p->Get.Front + len;
	}
	printf("\r%3d%% done", p->PercentDone);
	fflush(stdout);
#if defined( WINDOWS) & YIELD_TEST
	/* yield test */
	MSG msg;
	if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage((LPMSG) &msg);
		DispatchMessage((LPMSG) &msg);
	}
#endif
	return(ERR_NONE);
}


PRIVATE LONG  copy_to_file (PIC_PARM *p)
{
	DWORD	len;

  	/****/

 	/* Note that we are not using Put buffer as a queue, just a linear buffer */
	len = p->Put.Rear - p->Put.Front;
	if (fwrite(p->Put.Front,sizeof(BYTE),len,des_file) < len)
		return(ERR_BAD_WRITE);
	p->Put.Rear = p->Put.Front;
#if defined(WINDOWS) & YIELD_TEST
		/* yield test */
	MSG msg;
	if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage((LPMSG) &msg);
		DispatchMessage((LPMSG) &msg);
	}
#endif
	return(ERR_NONE);
}

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ   Determine if a palette is gray-scale or not.                        บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE BOOL    pal_is_gray (PIC_PARM *p)
{
	RGBQUAD *pp = p->ColorTable;
	DWORD    n;

  	/****/

  if(p->Head.biClrUsed != 256)
  	return(FALSE);
	for(n = 0;n < 256;++n)
	{
		if (pp->rgbBlue != n || pp->rgbGreen != n || pp->rgbRed != n)
		  return (FALSE);
		pp++;
	}
	return (TRUE);
}

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ   Convert RGB triples into RGBQUADS, possibly within the same buffer. บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    rgbt_to_rgbquad (RGBQUAD *des, RGBTRIPLE *src, DWORD n)
{
	src += n;
	des += n;
	while (n--)
	{
 		/*  The order of assignment below is crucial! Input and output
			 buffer may be the same */
		--src;
		--des;
		des->rgbReserved = 0;
		des->rgbRed = src->rgbtRed;
		des->rgbGreen = src->rgbtGreen;
		des->rgbBlue = src->rgbtBlue;
	}
}

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ   Compute the cycle redundancy check of the given string              บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
#if PC_WATCOM
PRIVATE WORD    crc (BYTE  *bp)
{
	DWORD   sum = 0;
	WORD    n = _fstrlen(bp);

	while (n--)
	{
		sum = (sum << 1) + (DWORD) 0x2941 * *bp++ + (sum >> 16);
	}
	return ((WORD) sum);
}
#endif


/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ   Show the copyright information.                                     บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE WORD    show_copyright (BOOL visible)
{
	char    s1[] = "(c) 1995, Applied Multimedia Technologies Corporation, Atlanta, Georgia";
	char    s2[] = "(c) 1995, Pegasus Imaging Corporation, Tampa, Florida";
	char    s3[] = "Bad Code";

#if PC_WATCOM
	check(crc(s1) == (WORD) 5165  &&  crc(s2) == (WORD) 58266, "%s", s3);
#endif
	if (visible)
	{
		printf("\nJPACK - Batch Pack source to JPEG files.\n");
		printf("%s\n\n", s2);
		printf("    (Voice) 813-875-7575  (Fax) 813-875-7705\n\n"
				 "Program Version 3.60\n\n"
			);
		printf("Press 'C' (ENTER) to continue..\n ");
		while ('C' != getchar()) ;    /* Await user response */
		printf("\n");
	}
	return (strlen(s2)+2);
}


/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ       Print help message.                                             บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    print_help (void)
{
	printf(
		"\nJPACK - version 3.60, Compress a set of Targa or BMP images using JPEG.\n"
		"(c) 1995-1996, Pegasus Imaging Corporation, Tampa, Florida\n"
#if PC_WATCOM
		"usage:  JPACK  source_name[.TGA]  [destination[.PIC]]  {options}\n"
#else
		"usage:  JPACK  source_name  destination_name  {options}\n"
#endif
    "     Makes either PIC or JPEG images (default is PIC).\n"
    "         For PIC images, the default color palette behavior is to put a\n"
    "     palette into the BMP-style header of the PIC file if the input is a\n"
		"     color (not grayscale) image; this palette will be the one that came\n"
		"     with the input image if there is one,\n"
    "     otherwise a new palette will be created with 236 primary and 16\n"
		"     secondary colors. (use -P option below to always make new colors).\n"
    "         For JPEG images, the default palette behavior is to not have\n"
		"     a palette in the JPEG file.\n"
    "     (These defaults can be modified with some of options below).\n"
		"The options are:(please hit ENTER)\n"
    );
		getchar();
  printf(
		"    -J     Make JPEG rather than PIC file"
#if PC_WATCOM
		"           , extension defaults to .JPG"
#endif
		"\n"
		"    -2     Make PIC2 file (overrides -J)"
#if PC_WATCOM
		"           , extension defaults to .PIC"
#endif
		"\n"
		"    -M     For a JPEG output file which is not grayscale, this means put\n"
		"           a palette in an APP1 JPEG marker, the palette coming from the\n"
		"           input file if it has a valid one (and -P is not set),\n"
		"           otherwise one will be created, just as described above for\n"
		"           a PIC file.\n"
    "               For a PIC output file, this flag means to put any palette\n"
    "           into a APP1 JPEG marker INSTEAD of putting it into the BMP\n"
    "           style header.\n"
		"    -X     Don't make any NEW color palettes for PIC or JPEG files.\n"
    "           Thus for a PIC or JPEG file, if the input has no palette,\n"
    "           neither will the output(even if -M is set for a JPEG file).\n"
    "           Note that if -X is set and the output is a PIC file, the\n"
    "           output will STILL get a palette if the input has one, unless\n"
    "           the -D option below is set.\n"
    "    -D     Don't put ANY palette in output file, no matter what.  This\n"
    "           makes it possible for PIC output files to have no palette.\n"
		"           JPEG output files would not need this option, because if -M is\n"
		"           not set, a JPEG output already would have no palette.\n"
		"    -P     If input is not grayscale, make new color palettes and put\n"
		"           them into the output for PIC and JPEG files, even\n"
		"           if input file already had a palette. Default to 236 primary\n"
		"           + 16 secondary colors.\n"
    "           If -X is set, -P will be ignored, as it is incompatible.\n"
    "           If the output is a JPEG file and -P is set and -M is not set,\n"
    "           then -P is ignored and no colors are made. If -D is set, -P\n"
    "           is ignored.\n"
		"    -N#    Set the number of palette entries to be made in any case that\n"
		"           new colors are being made; if new colors are not being made,\n"
		"           the -N option is ignored.\n"
		"               The -N# option sets number of primary colors if # is\n"
		"           positive. -N# may be entered a second time with # negative\n"
		"           to set number of secondary colors.  If num. primary colors is\n"
		"           not specified, then any specification of number of secondary\n"
		"           colors is ignored and the defaults of 236 primary, 16 \n"
		"           secondary are used.  If number of primary colors is specified\n"
		"           and number of secondary colors is not, the number of secondary\n"
		"           colors is taken to be 0. Thus 0 secondary colors is allowed.\n"
		"    -Y     Use YCbCr colorspace for making any new colors,  which\n"
		"           often makes better colors than using RGB colorspace.\n"
		"           Ignored if not making new colors.(default is RGB). \n"
		" (please hit ENTER)\n"
		);
		getchar();
	printf(
#if YUV_INPUT
		"    -U# # # Raw yuv input;first number is width, second is height, third\n"
		"             is format of input(0=UYVY,1=YUY2(microsoft media subtypes\n"
		"             with horizontal 2 to 1 downsampling of chrominance; UYUV means\n"
		"             bytes are in order u1,y1,v1,y2,u2,y3,v2,y4  etc, and YUY2 means\n"
		"             bytes are in order y1,u1,y2,v1,y3,u2,y4,v2 etc.)\n"
		"             In this case, output subsampling will be 211 by default.\n"
		"             User may specify 411 subsampling \n"
		"             for output by using -S2 option below.\n"
#endif
		"    -H     do NOT optimize huffman codes (default is optimize them)\n"
#if ELS_CODER
		"    -W     use ELS entropy coder rather than huffman codes.\n"
#endif
		"    -S#    indicates output subsampling,applies to chrominance only: 0=none(111),\n"
		"           1=horizontal only by factor of 2(211), 2=horizontal and vertical\n"
		"           by factor of 2 (411), 3 = vertical only by factor of 2 (211v)\n"
		"           (default is 2 (411)).\n"
		"    -Qname (optional) user gives up to 192 Q-table values in text file 'name'.\n"
		"           If color there must be at least 128 values;64 if grayscale.\n"
		"           The values are assumed NOT in zig-zag order, unlike in jpg file.\n"
		"           The first 64 are for Y(luminance); if color, next 64 for Cb.\n"
		"           If there are yet 64 more they are for Cr, else Cr uses same as Cb.\n"
		"           Comments are allowed: after #, rest of line is a comment.\n"
		"           Use no commas or other marks between between q values.\n"
		"           Values must be integers between 1 and 255, inclusive.\n"
		"    -L#    gives a luminance quality rating from 0 to 255 (default 24)\n"
		"    -C#    gives a chrominance quality rating from 0 to 255 (default 30)\n"
		"    -R     remove (suppress) any comment or id information\n"
		"    -Z     read file input in strips(slower, as a test or to save mem.)\n"
		"    -T#    num loops(1 <= # <= 1000)for speed timing. All but one of the\n"
		"           compressions is done buffer to buffer, no file io. Cannot be\n"
		"           used with -Z option. The output buffer will be automatically\n"
		"           set at 200000 for this case so that file output can likely\n"
		"           wait to the end.  You will have to set -E below\n"
		"           if your output will be bigger than 200000 bytes.\n"
		"    -E#  	output buffer size (default 64000).\n"
		"    -O#    sets visual Orientation (See PIC.H for values) default is 0.\n"
    "    -Kkey  Encrypts the image file with the given key, key is up to 8 chars.\n"
#if PC_WATCOM
		"The source name may be a wild card name - if so, all images are compressed\n"
		"and share the same options.  If the source is not a wild card name, the\n"
		"destination is the name of a file with the .PIC extension (or.JPG if\n"
		"the -J option is used).  If the source is a wild card, the destination is\n"
		"a path name; the destination name is the destination path followed by the\n"
		"source name with .PIC (or .JPG) extension."
#endif
	);
	exit(1);
}


/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ       Parse the parameters.                                           บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    parse_parms (INT argc, char *argv[])
{
#if PC_WATCOM
	char    ci_drive[5], ci_dir[60], ci_fname[20], ci_ext[10], *pp;
	BOOL    have_name;
#endif
	LONG    i,nc;

  	/****/

#if PC_WATCOM
	wild_card = (strchr(argv[1], '*') != NULL  ||
				 strchr(argv[1], '?') != NULL);
	_fullpath(src_name, argv[1], 100);   strupr(src_name);
	_splitpath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
	if (ci_ext[0] == '\0')  strcpy(ci_ext, ".TGA");
	_makepath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
	_makepath(src_path, ci_drive, ci_dir, "", "");
	des_ext = ".PIC";
#else
  check(argv[1][0] != '-' && argv[1][0] != '/',"need input filename");
  check(argv[2][0] != '-' && argv[2][0] != '/',"need output filename");
  src_name = argv[1];
  des_name = argv[2];
#endif

	lum_opt = 24;
	chrom_opt = 30;
	subsample = 0xFFFF; /* illegal value */
	num_prim_colors_req = 0;
	num_sec_colors_req = 0;
	vorient = 0;
	jpeg_file = include_jcm = dont_make_new_colors =
							make_new_colors = use_YCbCr_for_colors = pic2_file = FALSE;
	absolutely_no_output_colors = FALSE;
	optimize_huff_codes = TRUE;
	use_els_coder = FALSE;
	suppress = skip_copyright = FALSE;
	test_strips = FALSE;
  num_loops = 1;
	put_buff_size = DEFAULT_PUT_QUEUE_SIZE;
  memset(key, 0, sizeof(key));
	QTableName[0] = '\0';

	for (i = 2;  i < argc;  ++i)
	{
		if (argv[i][0] == '-'  ||  argv[i][0] == '/')
		{
			switch (toupper(argv[i][1]))
			{
				case 'J' :  jpeg_file = TRUE;
#if PC_WATCOM
							des_ext = ".JPG";
#endif
							break;
        case '2' :  pic2_file = TRUE;
              break;

				case 'M' :  include_jcm = TRUE;
							break;
				case 'X' :  dont_make_new_colors = TRUE;
							break;
				case 'D' :  absolutely_no_output_colors = TRUE;
							break;
				case 'P' :  make_new_colors = TRUE;
							break;
				case 'Y' :  use_YCbCr_for_colors = TRUE;
							break;
				case 'N' :  nc = atoi(argv[i]+2);
							if(nc > 0) num_prim_colors_req = nc;
							else num_sec_colors_req = labs(nc);
							break;
				case 'H' :  optimize_huff_codes = FALSE;
							break;
#if ELS_CODER
				case 'W' :  use_els_coder = TRUE;
							break;
#endif
				case 'S' :  subsample = atoi(argv[i]+2);
							break;
				case 'Q' :  strcpy(QTableName, argv[i]+2);
							break;
				case 'L' :  lum_opt = atoi(argv[i]+2);
							break;
				case 'C' :  chrom_opt = atoi(argv[i]+2);
							break;
				case 'R' :  suppress = TRUE;
							break;
				case 'O' :  vorient = atoi(argv[i]+2);
							break;
				case 'K' :  strncpy(key, argv[i]+2, 8);
							break;
				case 'Z' :
						test_strips = TRUE;
						break;
				case 'T' :
						num_loops = atoi(argv[i]+2);
						check(num_loops > 0 && num_loops <= 1000, "Bad num loops");
						break;
#if YUV_INPUT
				case 'U' :
						raw_yuv_in = TRUE;
						raw_yuv_in_width = atoi(argv[i]+2);
						raw_yuv_in_height = atoi(argv[i+1]);
						raw_yuv_in_format = atoi(argv[i+2]);
						if(raw_yuv_in_format == 0)
							raw_yuv_in_format = BI_UYVY;
						else if(raw_yuv_in_format == 1)
							raw_yuv_in_format = BI_YUY2;
/*						else if(raw_yuv_in_format == 2)
							raw_yuv_in_format = BI_YVYU; */
						else
							check(FALSE, "Unknown raw yuv format");
						i += 2;
						break;
#endif
				case 'E' :
						put_buff_size = atoi(argv[i]+2);
						check(put_buff_size >= MIN_PUT_QUEUE_SIZE, "Bad output buff size");
						break;
						/* Next line for testing only */
				case 'A' :  strncpy(comment, argv[i]+2, sizeof(comment));
							break;
				case '!' :  skip_copyright = TRUE;
							break;
				default  :  check(FALSE, "Unknown option %s", argv[i]);
							break;
			}
		}
	}

#if YUV_INPUT
	if(raw_yuv_in && (subsample == 0xFFFF))
		subsample = SS_211;
#endif
	if(subsample == 0xFFFF)
		subsample = SS_411; /* default */

	check(lum_opt < 256  &&  chrom_opt < 256  &&  subsample < 4,
		"Invalid option value");

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

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ       Check that exp is TRUE.  If it is not then print the formatted  บ */
/*  ณ   message fmsg, and terminate the program.                            บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    check (BOOL exp, char *fmsg, ... )
{
	va_list marker;

  	/****/

	if (!exp) {
		va_start(marker, fmsg);
		printf("Program terminated: ");
		vprintf(fmsg, marker);
		printf(".\n");
		if (src_file != NULL)
		  fclose(src_file); src_file = NULL;
		if (des_file != NULL)
		  fclose(des_file); des_file = NULL;
		exit(1);
	}
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

PRIVATE	void MakeLittleEndianDword(BYTE *des, DWORD dd)
{
	des[0] = dd;
	des[1] = dd >> 8;
	des[2] = dd >> 16;
	des[3] = dd >> 24;
}

PRIVATE	void MakeLittleEndianWord(BYTE *des, WORD dw)
{
	des[0] = dw;
	des[1] = dw >> 8;
}

