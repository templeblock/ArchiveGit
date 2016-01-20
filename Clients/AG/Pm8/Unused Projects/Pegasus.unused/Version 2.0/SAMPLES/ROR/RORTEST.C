/*.T  RORTEST.C
 */

/*   Copyright (c) 1995, Pegasus Imaging Corporation */
#if !defined(PC_WATCOM)
#define PC_WATCOM 1 /* allows for wildcard processing, not ANSI */
#endif
/* Definition of NOCO must match that in jelevl2.h ! */
#define	NOCO	0  /* 0->use co-routines, 1->no co-routines */
/* If PC_WATCOM is 0 and NOCO is 0 and USE_ASSEMBLY in level2 is 0, then
		the code should be ANSI c */
/* Definition of ELS_CODER_IN and ELS_CODER_OUT must match that in jelevl2.h ! */
/* Definition of PIC2_IN and PIC2_OUT must match that in jelevl2.h ! */
#define	DEFAULT_ELS_CODER_IN	1 /* allows for possibility of using ELS entropy coding for input */
#define DEFAULT_ELS_CODER_OUT 1 /* allows for possibility of using ELS entropy coding for output */
#define DEFAULT_REQUANT 1
#define	DEFAULT_PIC2_IN	1 /* allows for possibility of PIC2 input format */
#define	DEFAULT_PIC2_OUT	0 /* allows for possibility of PIC2 output format */

#define CNT	0

#if !defined(ELS_CODER_IN)
#define ELS_CODER_IN       DEFAULT_ELS_CODER_IN
#endif

#if !defined(ELS_CODER_OUT)
#define ELS_CODER_OUT       DEFAULT_ELS_CODER_OUT
#endif

#if !defined(REQUANT)
#define REQUANT       DEFAULT_REQUANT
#endif

#if !defined(PIC2_IN)
#define PIC2_IN       DEFAULT_PIC2_IN
#endif

#if !defined(PIC2_OUT)
#define PIC2_OUT       DEFAULT_PIC2_OUT
#endif

#if ELS_CODER_IN == 1
	#pragma message("ELS_CODER_IN enabled")
#else
	#pragma message("ELS_CODER_IN disabled")
#endif

#if ELS_CODER_OUT == 1
	#pragma message("ELS_CODER_OUT enabled")
#else
	#pragma message("ELS_CODER_OUT disabled")
#endif

#if PIC2_IN == 1
	#pragma message("PIC2_IN enabled")
#else
	#pragma message("PIC2_IN disabled")
#endif

#if PIC2_OUT == 1
	#pragma message("PIC2_OUT enabled")
#else
	#pragma message("PIC2_OUT disabled")
#endif

#define	USE_PEG_QRY	1  /* PegasusQuery is not ANSI so set this to 0 to get ANSI compliance */
#define	DEFAULT_GET_BUFF_SIZE	20000
#define	DEFAULT_PUT_BUFF_SIZE	20000

#pragma pack(1) /* If not WATCOM, replace this with some equivalent. The
	structures must be packed with no space, for reading and writing file
	headers, and also for consistency with asm stuctures in some cases. */

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <string.h>
#include    <ctype.h>
#if PC_WATCOM
#include    <dos.h>    /* needed for wildcards. Not ANSI */
#include    <share.h>  /* needed for _fsopen sharing constants */
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

#if CNT
/* Temp! */
PUBLIC DWORD OverCt = 0, UnderCt = 0, L1OverCt = 0, TotalCt = 0, DcOverCt = 0, AcOverCt = 0;
#endif

/*  Globals for main */
PRIVATE CHAR    *des_ext;
#if PC_WATCOM
PRIVATE CHAR    src_name[100], src_path[100], des_name[100], des_path[100];
PRIVATE BOOL		wild_card;
#else
PRIVATE CHAR		*src_name,*des_name;
#endif
PRIVATE FILE	*src_file = NULL, *src_file_join = NULL, *des_file = NULL;
PRIVATE CHAR		*src_name_join = NULL;
PRIVATE BOOL    skip_copyright;
PRIVATE	CHAR	QTableName[100];
PRIVATE	DWORD	get_buff_size;
PRIVATE	DWORD	put_buff_size;
PRIVATE	DWORD	num_loops = 1;
PRIVATE	LONG		orient;
PRIVATE	BOOL		keep_colors;
PRIVATE LONG		pad;
PRIVATE DWORD   lum_opt, chrom_opt;
PRIVATE BYTE    qtable[2*192];
PRIVATE	WORD    *region_qtable;
PRIVATE	DWORD		requant;
PRIVATE DWORD   insert_transparency_lum;
PRIVATE DWORD   insert_transparency_chrom;
PRIVATE	DWORD   num_tables;
#if PIC2_OUT
PRIVATE	BOOL	pic2_out = FALSE;
#endif
#if ELS_CODER_OUT
PRIVATE	BOOL	els_coder_out = FALSE;
#endif
PRIVATE	DWORD	crop_x, crop_y, crop_w, crop_h;
PRIVATE	BOOL	crop_flg;
#if NOCO
PRIVATE	LONG defer_status;
#endif
PRIVATE	DWORD	apps_to_keep;
PRIVATE	DWORD	remove_comments;
PRIVATE	BOOL	join_left_right;
PRIVATE	BOOL	join_insert;
PRIVATE	BOOL	join_second_on_top_left;
PRIVATE	BOOL	join_subsampling_from_second;
PRIVATE	BOOL	join_quantization_from_second;
PRIVATE	BOOL	join_use_requested_quantization;
PRIVATE	BOOL	regions;
PRIVATE	LONG	gray_or_color_out;
PRIVATE	SHORT	YShift,YScale,CbShift,CbScale,CrShift,CrScale;

PRIVATE CHAR    key[9];
PRIVATE CHAR    outputkey[9];

/*  Local routines */
PRIVATE LONG    expand (void);
#if NOCO
PRIVATE	LONG	  defer_function(PIC_PARM *p, RESPONSE res);
#endif
PRIVATE LONG    read_file (PIC_PARM *p);
PRIVATE LONG    copy_to_file (PIC_PARM *p);
PRIVATE	LONG		file_size(FILE *fp);
PRIVATE void    check (BOOL exp, char *fmsg, ... );
PRIVATE LONG    check1 (BOOL exp, char *fmsg, ... );
PRIVATE void    show_copyright (void);
PRIVATE void    print_help (void);
PRIVATE void    parse_parms (int argc, char *argv[]);

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
#endif
	UINT	n;
	LONG   status = 0;

			/****/

		if (argc < 2)
		{
				print_help();
				exit(1);
		}

		printf(
				"\nRORTEST - reorient, requant, crop, join or els-code seq. jpg and pic files.\n"
				"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
		);

		/*  Get command line arguments. */
		parse_parms(argc, argv);

		if (!skip_copyright)
			show_copyright();

	num_tables = 0;
	if (QTableName[0] != '\0')    /*  User supplied a file name holding Q-table values */
	{ /* qtable is assumed NOT in zig_zag order (unlike what's in jpg file ) */
			FILE    *QFile;
			LONG	qq,u,stat = 0;
			int	len,c;
			/****/
			QFile = fopen(QTableName, "rt");
			check(QFile != NULL, "Q-table %s could not be opened", QTableName);
			u = 192;
			if(regions)
			{
				u += u;
				region_qtable = (WORD *)qtable;
			}
			memset(qtable,0,u);
			n = 0;
			u = 255;
			if(regions)
				u = 65535;
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
				if(qq < 1 || qq > u)
				{
					printf("Program terminated:illegal quantization value(s)\n");
					stat = -1;
					break;
				}
				if(regions)
					region_qtable[n++] = qq;
				else
					qtable[n++] = qq;
			}
			fclose(QFile);
			QFile = NULL;
			if(stat != 0)
				exit(1);
			check(n == 64 || n == 128 || n == 192, "incorrect number of quantization values");
			num_tables = n/64;
	}

#if PC_WATCOM
		n = 0;
		if (_dos_findfirst(src_name, _A_NORMAL, &info) == 0) {
				do {
						strcpy(src_name, src_path);
						strcat(src_name, info.name);
						if (wild_card) {
								strcpy(des_name, des_path);
								strcat(des_name, info.name);
								strcpy(strchr(des_name, '.'), des_ext);
						}
						status = expand();
						n++;
				} while ( _dos_findnext(&info) == 0);
		}
		else
			printf("\n image not found.\n", n);
		printf("\nThere were %d images converted or attempted.\n", n);
#else
		status = expand();
#endif
		if(status != 0)
			printf("status = %d\n",status);
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
	LONG	join_offset = 0;
	LONG	k, region_x,region_y,region_endx,region_endy, region_w,region_h;
	BYTE	*bptr;
	BYTE	*region_info_ptr;
	REGION_INFO	*region_info;

		/****/

	src_file = src_file_join = des_file = NULL;
	join_offset = 0;

	memset(&p, 0, sizeof(PIC_PARM));    /* important: set all default values 0 */
		/* ALSO important -- following must be set even for PegasusQuery */
		/* beginning version 13 */
	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;
	p.ParmVerMinor = 1;
#if NOCO
	defer_status = 0;
	p.DeferFn = &defer_function;
#endif

	if(src_name_join != NULL)
		if(check1(!orient && !requant && !crop_flg,"Can't reorient, requant or crop while joining")) goto quit;


	/*  Open the JPEG file */
	src_file = fopen(src_name, "rb");
	if(check1(src_file != NULL, "Could not open %s", src_name)) goto quit;
	printf("Opened file %s.\n", src_name);


	/*  Open other JPEG file if joining */
	if(src_name_join != NULL)
	{
		src_file_join = fopen(src_name_join, "rb");
		if(check1(src_file_join != NULL, "Could not open %s", src_name_join)) goto quit;
		printf("Opened file %s.\n", src_name_join);
	}

	/*  Allocate GetBuff and place into it the information to be queried */
	if(num_loops > 1)
	{ /* allocate a full_size buffer  */
		get_buff_size = file_size(src_file) + 2;
		if(put_buff_size < get_buff_size << 1)
			put_buff_size = get_buff_size << 1;
	}
	if(src_name_join != NULL)
	{
		join_offset = file_size(src_file_join);
		get_buff_size += join_offset;
	}
	p.Get.Start = (BYTE *) malloc(get_buff_size);
	if(check1(p.Get.Start != NULL, "Out of memory for get buffer")) goto quit;
	p.Get.End = p.Get.Start + get_buff_size;

	/* read in first file if joining */
	if(src_name_join != NULL)
	{
		len = fread(p.Get.Start, sizeof(BYTE), get_buff_size, src_file_join);
		if(check1(ferror(src_file_join) == 0, "Error reading join file")) goto quit;
		if(check1(len == join_offset, "Error reading join file")) goto quit;
	}

	/*  read in header data, at least (of second file if joining) */
	len = fread(p.Get.Start + join_offset, sizeof(BYTE), get_buff_size - join_offset, src_file);
	if(check1(ferror(src_file) == 0, "Error reading input file")) goto quit;
	if(len < get_buff_size - join_offset)
		p.Get.QFlags |= Q_EOF;

	/*  setup output buff */
	p.Put.Start = (BYTE  *) malloc(put_buff_size);
	if(check1(p.Put.Start != NULL, "Out of memory for put buff")) goto quit;
	p.Put.End = p.Put.Start + put_buff_size;

	/* setup output file */
#if PC_WATCOM
	des_file = _fsopen(des_name, "wb", SH_DENYWR);
					/* ^ use _fsopen in watcom so we can make sure the input file
						isn't the same file as the output file.  If it is the same,
						_fsopen will fail with a sharing violation error */
#else
	des_file = fopen(des_name, "wb");
#endif
	if(check1(des_file != NULL,"Could not create %s", des_name)) goto quit;

	p.u.ROR.RegionInfo = NULL;
for(ct = 0;ct < num_loops;++ct)
{
	p.Get.Rear = p.Get.Start + len + join_offset;
	p.Get.Front = p.Get.Start;
	p.Put.Front = p.Put.Rear = p.Put.Start;

#if USE_PEG_QRY
	/* note: PegasusQuery is not ANSI */
	p.u.QRY.BitFlagsReq = QBIT_BICOMPRESSION;
	if(check1(PegasusQuery(&p), "Invalid PegasusQuery")) goto quit;
	memset(&p.u.QRY, 0, sizeof(p.u.QRY));
	if(check1(p.Head.biCompression == BI_PICJ ||
#if PIC2_IN
				p.Head.biCompression == BI_PC2J	||
#endif
#if ELS_CODER_IN
				p.Head.biCompression == BI_JPGE	||
#if PIC2_IN
				p.Head.biCompression == BI_PC2E	||
#endif
#endif
				p.Head.biCompression == BI_JPEG, "Unsupported file type")) goto quit;
#endif

	/*  Set up the output image characteristics. */

	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;   p.ParmVerMinor = 1;
#if ELS_CODER_OUT
	p.Op = OP_RORE;
#else
	p.Op = OP_ROR;
#endif
	p.u.ROR.PicFlags = 0;
	p.VisualOrient = orient;
	p.u.ROR.PicFlags = 0;
	if(num_loops == 1)
		p.u.ROR.PicFlags |= PF_YieldPut;
	p.u.ROR.Pad = pad;
	p.u.ROR.KeepColors = keep_colors;
#if ELS_CODER_OUT
	if(els_coder_out)
		p.u.ROR.PicFlags |= PF_ElsCoder;
#endif
	p.u.ROR.JpegType = JT_RAW;
#if PIC2_OUT
	if(pic2_out)
		p.u.ROR.JpegType = JT_PIC2;
#endif
	memcpy(p.KeyField, key, 8);
		memcpy(p.u.ROR.OutputKeyField, outputkey, 8);
	if(requant || ((src_name_join != NULL) && (join_use_requested_quantization)))
	{
		p.u.ROR.Requantize = requant;
		if (QTableName[0] != '\0')
			p.u.ROR.QTableReq = (BYTE PICHUGE *) qtable;
		else
		{
			p.u.ROR.LumFactorReq = lum_opt;
			p.u.ROR.ChromFactorReq =  chrom_opt;
		}
	}
	p.u.ROR.AppsToKeep = apps_to_keep;
	p.u.ROR.RemoveComments = remove_comments;

	if(src_name_join != NULL)
	{
		p.u.ROR.JoinFlags |= JF_DoJoin;
		p.u.ROR.JoinOffset = join_offset;
		if(join_use_requested_quantization)
			p.u.ROR.JoinFlags |= JF_UseRequestedQuantization;
		if(join_left_right)
			p.u.ROR.JoinFlags |= JF_LeftRight;
		if(join_insert)
		{
			p.u.ROR.JoinFlags |= JF_Insert;
			p.u.ROR.InsertTransparencyLum = insert_transparency_lum;
			p.u.ROR.InsertTransparencyChrom = insert_transparency_chrom;
		}
		if(join_second_on_top_left)
			p.u.ROR.JoinFlags |= JF_SecondOnTopLeftInsert;
		if(join_subsampling_from_second)
			p.u.ROR.JoinFlags |= JF_UseSecondSubsampling;
		if(join_quantization_from_second)
			p.u.ROR.JoinFlags |= JF_UseSecondQuantization;
	}
#if(USE_PEG_QRY)
	if(p.Head.biHeight < 1)
	{
		p.Head.biHeight = -p.Head.biHeight;
		/* from now on,p.Head.biHeight is positive. */
	}
	if(regions)
	{
		p.u.ROR.NumRegions = 2;
		p.u.ROR.RegionMapWidth = (p.Head.biWidth + 7) >> 3;
		p.u.ROR.RegionMapHeight = (p.Head.biHeight + 7) >> 3;
		region_x = crop_x >> 3;
		region_y = crop_y >> 3;
		region_endx = (crop_x + crop_w + 7) >> 3;
		region_endy = (crop_y + crop_h + 7) >> 3;
		if(check1(region_x < p.u.ROR.RegionMapWidth &&
							region_y < p.u.ROR.RegionMapHeight &&
							region_endx <= p.u.ROR.RegionMapWidth &&
							region_endy <= p.u.ROR.RegionMapHeight,
			 "Bad region rectangle")) goto quit;
		region_w = region_endx - region_x;
		region_h = region_endy - region_y;
		p.u.ROR.RegionInfo = malloc(8*p.u.ROR.NumRegions + 128*num_tables +
				p.u.ROR.RegionMapWidth * p.u.ROR.RegionMapHeight );
		if(check1(p.u.ROR.RegionInfo != NULL, "Out of memory for RegionInfo")) goto quit;
		region_info_ptr = p.u.ROR.RegionInfo;
		region_info = (REGION_INFO *)region_info_ptr;
		region_info->NumTbls = 255; /* indicates to use qtables from input jpeg for region 0 */
		region_info_ptr += 8; /* word for Lum.,word for Chrom, dword for NumTbls */
		region_info = (REGION_INFO *)region_info_ptr;
		region_info->LumFactor = lum_opt;
		region_info->ChromFactor = chrom_opt;
		region_info->NumTbls = num_tables;
		if (num_tables != 0)
			memcpy(&(region_info->QTbls[0]),region_qtable,num_tables*128);
		p.u.ROR.RegionMapOffset = 8*p.u.ROR.NumRegions + 128*num_tables;
		bptr = p.u.ROR.RegionInfo + p.u.ROR.RegionMapOffset;
		memset(bptr,1,p.u.ROR.RegionMapWidth * p.u.ROR.RegionMapHeight); /* background has index 1 */
		bptr += (region_y * p.u.ROR.RegionMapWidth) + region_x;
		for(k = 0;k < region_h;++k)
		{
			memset(bptr,0,region_w); /* rectangle of interest has index 0 */
			bptr += p.u.ROR.RegionMapWidth;
		}
	}
	p.u.ROR.YShift = YShift;
	p.u.ROR.YScale = YScale;
	p.u.ROR.CbShift = CbShift;
	p.u.ROR.CbScale = CbScale;
	p.u.ROR.CrShift = CrShift;
	p.u.ROR.CrScale = CrScale;
#endif



/*p.Head.biBitCount = 24; // temp, while not using PegasusQuery */



	if(crop_flg)
	{
		p.Flags |= F_Crop;
		if(crop_w == 0)
			crop_w = 65535; /* init will cut this back to full width */
		if(crop_h == 0)
			crop_h = 65535;
		p.CropWidth = crop_w;
		p.CropHeight = crop_h;
	}
	p.CropXoff = crop_x; /* used also for inserting */
	p.CropYoff = crop_y;

	if(gray_or_color_out == 1)
		p.u.ROR.PicFlags |= PF_ConvertGray;
	else if(gray_or_color_out == 2)
		p.u.ROR.PicFlags |= PF_ConvertToColor;


#if	NOCO
	/*  Initialize Jpeg decompression */
	res = Pegasus(&p, REQ_INIT);
	if(check1(defer_status >= 0, "Error %d in DeferFn.", defer_status)) goto quit;
	if(check1(res != RES_ERR, "Error number %d in Pegasus.",p.Status)) goto quit;
	if(check1(p.Status >= 0, "Error %d in Pegasus.", p.Status)) goto quit;
	if (p.Status > 0)  printf("Status %d in Pegasus Init.\n", p.Status);
	if(p.Status == ERR_JUNK_BYTES_IN_HEADER)
		printf("Junk bytes found in header were skipped over.\n");

	/*  Decompress the file. */
	res = Pegasus(&p, REQ_EXEC);
/*	if(check1(defer_status >= 0, "Error %d in DeferFn.", defer_status)) goto quit; */
/*	check1(res != RES_ERR, "Error number %d in Pegasus.",p.Status); output what you can */

	/* Let Pegasus close things(in seq. jpeg, everything is already closed, */
	/* but this would not necessarily be true for progressive) */
	/* res = Pegasus(&p,REQ_TERM); // This can only return RES_DONE, and */
	/* does not change p->Status. Not needed in seq. */

	if (p.Status)  printf("Status %d in Pegasus.\n", p.Status);

#else

	/*  Initialize Jpeg decompression */
	local_status = 0;
	res = Pegasus(&p, REQ_INIT);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA) {
			local_status = read_file(&p);
		} else if (res == RES_PUT_DATA_YIELD) {
			if(num_loops == 1)
				local_status = copy_to_file(&p);
		} else if (res == RES_PUT_NEED_SPACE) {
			if(check1(num_loops == 1,"output buffer too small to do loops")) goto quit;
			local_status = copy_to_file(&p);
		} else if (res == RES_ERR) {
			if(check1(FALSE, "Error number %d ",p.Status)) goto quit;
		} else {
			if(check1(FALSE, "Unexpected or unknown response %d ", res)) goto quit;
		}
		if(check1(p.Status >= 0, "Error %d in Pegasus", p.Status)) goto quit;
		if(check1(local_status >= 0, "Error %d in Pegasus", local_status)) goto quit;
		if (p.Status > 0)  printf("Status %d in Pegasus.\n", p.Status);
		res = Pegasus(&p, REQ_CONT);
	}
	if(check1(p.Status >= 0, "Error %d in Pegasus", p.Status)) goto quit;
	if (p.Status > 0)
		printf("Status %d in Pegasus Init.\n", p.Status);
	if(p.Status == ERR_JUNK_BYTES_IN_HEADER)
		printf("Junk bytes found in header were skipped over.\n");


	/*  Convert the file. */
	res = Pegasus(&p, REQ_EXEC);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA)
			local_status = read_file(&p);
		else if (res == RES_PUT_NEED_SPACE)
		{
			if(check1(num_loops == 1,"output buffer too small to do loops")) goto quit;
			local_status = copy_to_file(&p);
		}
		else if (res == RES_PUT_DATA_YIELD)
		{
			if(num_loops == 1)
				local_status = copy_to_file(&p);
		}
		else if (res == RES_POKE)
		{
			/* write out remainder of put buffer, if any -- there won't
					be a poke until after pack has output everything.
					flushing here saves us having to remember the current
					file position so we can seek back for the final write */
			if ( p.Put.Rear > p.Put.Front )
			{
				len = p.Put.Rear - p.Put.Front;
				if(check1(fwrite(p.Put.Front,sizeof(BYTE),len,des_file) == len,
						"output file write error")) goto quit;
				p.Put.Front = p.Put.Rear;     /* it's empty now */
			}
			else if ( p.Put.Rear < p.Put.Front )
				{
				len = p.Put.End - p.Put.Front;
				if(check1(fwrite(p.Put.Front,sizeof(BYTE),len,des_file) == len,
						"output file write error")) goto quit;
				len = p.Put.Rear - p.Put.Start;
				if(check1(fwrite(p.Put.Start,sizeof(BYTE),len,des_file) == len,
						"output file write error")) goto quit;
        p.Put.Front = p.Put.Rear;     /* it's empty now */
        }
      /* else Rear == Front and there's nothing left to write */
			if(check1(fseek(des_file, p.SeekInfo, SEEK_SET) == 0,
					"output file seek error")) goto quit;
			len = p.Put.RearEnd - p.Put.FrontEnd;
			if(check1(fwrite(p.Put.FrontEnd, sizeof(BYTE), len, des_file) == len,
					"output file write error")) goto quit;
		}
		else if (res == RES_YIELD)
			;
		else if (res == RES_ERR)
			break; /* output what you can. check(FALSE, "Error number %d ",p.Status); */
		else
			if(check1(FALSE, "Unexpected or unknown response %d ", res)) goto quit;
		if(local_status < 0)
    	break;
		res = Pegasus(&p, REQ_CONT);
	}
	if (p.Status)  printf("Status %d in Pegasus.\n", p.Status);
	Pegasus(&p, REQ_TERM);
#endif

	if(p.u.ROR.RegionInfo != NULL)
	{
		free(p.u.ROR.RegionInfo);
		p.u.ROR.RegionInfo = NULL;
	}
}
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
#if REQUANT
	if(p.u.ROR.Requantize && !p.u.ROR.RequantizationDone)
		printf("\nFile was not requantized.\n");
#endif

	/*  copy output buffer to file (if not already done) */
	/* We are not wrapping queue, so Rear >= Front always. */

	if( (len = p.Put.Rear - p.Put.Front) > 0)
		if (fwrite(p.Put.Front, sizeof(BYTE), len, des_file) < len)
			if(check1(FALSE, "error writing output file")) goto quit;

	printf("Converted file %s to %s.\n", src_name, des_name);

	/*  Close files, free memory. */
quit:
	if (src_file != NULL)
		fclose(src_file);
	if (src_file_join != NULL)
		fclose(src_file_join);
	if (des_file != NULL)
		fclose(des_file);

	if(p.Put.Start != NULL)
	{
		free(p.Put.Start);
		p.Put.Start = NULL;
	}
	if(p.Get.Start != NULL)
	{
		free(p.Get.Start);
		p.Get.Start = NULL;
	}
	if(p.u.ROR.RegionInfo != NULL)
	{
		free(p.u.ROR.RegionInfo);
		p.u.ROR.RegionInfo = NULL;
	}

#if CNT
	/* Temp! */
	printf("OverCt = %d UnderCt = %d L1OverCt = %d TotalCt = %d DcOverCt = %d AcOverCt = %d\n",OverCt,UnderCt,L1OverCt,TotalCt,DcOverCt,AcOverCt);
#endif
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
	LONG	len;

	if (res == RES_GET_NEED_DATA)
		defer_status = read_file(p);
	else if (res == RES_PUT_NEED_SPACE)
	{
		/* need to empty put buffer */
		if(num_loops == 1)
			defer_status = copy_to_file(p);
		else
			defer_status = ERR_NOT_ENOUGH_BUFF_SPACE;
	}
    else if (res == RES_POKE)
    {
      /* write out remainder of put buffer, if any -- there won't
					be a poke until after pack has output everything.
					flushing here saves us having to remember the current
					file position so we can seek back for the final write */
			if ( p->Put.Rear > p->Put.Front )
			{
				len = p->Put.Rear - p->Put.Front;
				if(fwrite(p->Put.Front,sizeof(BYTE),len,des_file) != len)
					return(ERR_BAD_WRITE);
				p->Put.Front = p->Put.Rear;     /* it's empty now */
			}
			else if ( p->Put.Rear < p->Put.Front )
				{
				len = p->Put.End - p->Put.Front;
				if(fwrite(p->Put.Front,sizeof(BYTE),len,des_file) != len)
					return(ERR_BAD_WRITE);
				len = p->Put.Rear - p->Put.Start;
				if(fwrite(p->Put.Start,sizeof(BYTE),len,des_file) != len)
					return(ERR_BAD_WRITE);
				p->Put.Front = p->Put.Rear;     /* it's empty now */
				}
			/* else Rear == Front and there's nothing left to write */
			if(fseek(des_file, p->SeekInfo, SEEK_SET) != 0)
				return(ERR_BAD_SEEK);
			len = p->Put.RearEnd - p->Put.FrontEnd;
			if(fwrite(p->Put.FrontEnd,sizeof(BYTE),len,des_file) != len)
				return(ERR_BAD_WRITE);
		}
	else if (res == RES_PUT_DATA_YIELD)
	{
		if(num_loops == 1)
			defer_status = copy_to_file(p);
	}
	else if (res == RES_YIELD)
	{
		;
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
	long    avail,len;

		/****/

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
	long len;

		/****/

	len = p->Put.Rear - p->Put.Front;
	if (fwrite(p->Put.Front, sizeof(BYTE), len, des_file) < len)
			return(ERR_BAD_WRITE);
	p->Put.Rear = p->Put.Start; /* empty buffer */
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
/*  ณ   message fmsg, and terminate the program.                            บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    check (BOOL exp, char *fmsg, ... )
{
	va_list marker;

		/****/

	if (!exp)
	{
		va_start(marker, fmsg);
		printf("Program terminated: ");
		vprintf(fmsg, marker);
		printf(".\n");
		if (src_file != NULL)
			fclose(src_file);
		if (src_file_join != NULL)
			fclose(src_file_join);
		if (des_file != NULL)
			fclose(des_file);
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
		printf("\nRORTEST - Test reorient, requant, crop , join or els-code \n"
		"                   seq. jpg and pic files.\n"
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
	"\nRORTEST - Test reorient, requant, crop join or els_code seq. jpg and pic files.\n"
	"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
#if PC_WATCOM
	"\nusage:  RORTEST  input_filename[.PIC]  [destination[.JPG]]  {options}\n"
#else
	"\nusage:  RORTEST  input_filename  output_filename  {options}\n"
#endif
	"where options are:\n"
	"    -R[#]  new orientation(between 0 and 7, incl;default 0.) .\n"
#if ELS_CODER_OUT
	"    -W     use ELS entropy coder rather than huffman codes for output.\n"
#endif
#if PIC2_OUT
	"    -2     Make PIC2 output"
#if PC_WATCOM
	", extension defaults to .PIC"
#endif
	"\n"
#endif
	"    -P     1 = Pad width and height if needed to fill MCU,\n"
	"           2 = Pad width only, 3 = Pad height only.  If -P is given\n"
	"           with no value, it will be assumed 1(pad both).  If -P is not\n"
	"           given, width and height are not padded, they are trimmed\n"
	"           as needed.\n"
	"    -C     Keep any colors found in a PIC file in a PIC APP1 segment\n"
	"           (default is discard the colors).\n"
	"    -E#  input buffer size, min. 1024(default 20000,ignored if -L used).\n"
	"    -F#  output buffer size, min. 1024(default 20000,ignored if -L used\n"
	"            unless more than twice the input file size).\n"
	"    -T#  num loops(1 <= # <= 1000)for speed test(default 1)\n"
	" The following 4 options are also used to define rectangle of interest\n"
	" for regions (see -G option below.)  If these are all 0, when -G is set, there\n"
	" is no rectangle of interest and whole image gets 'background' quality. This\n"
	" can be used to give images with quality lower than baseline within a\n"
	" baseline file since regions allows quantization values > 255. \n"
	"    -VX#   x coord. of cropped or inserted rectangle (default 0)\n"
	"    -VY#   y coord. of cropped or inserted rectangle(default 0)\n"
	"    -VW#   width of cropped rectangle(ignored for insert)(default full image)\n"
	"    -VH#   height of cropped rectangle(ignored for insert)(default full image)\n"
	"    -Kkey  Decrypts the image file with the given key, key is up to 8 chars.\n"
#if PIC2_OUT
		"    -Xkeyx  Encrypts the output image with keyx, keyx is up to 8 chars.\n"
/*NOTE: ^ ultimately this option is independent of PIC2_OUT'ness, but
		currently it only works for PIC2 output files.  This is the only place
		which depends on PIC2_OUT -- it's just that the OP_RORE opcode will ignore
		u.ROR.OutputKeyField unless u.ROR.JpegType == JT_PIC2 */
#endif
#if REQUANT
	"    -B[#]  Requantize according to L and M ( or Qtable) below.\n"
	"           If #=1(default), use odd multiple of existing quantization\n"
	"           which is nearest requested. This produces the same result as\n"
	"           compressing the raw image with this quantization.\n"
	"           If #=2, use requested quantization when larger than existing\n"
	"           (this does not necessarily produce the same result as\n"
	"           compressing the raw image with this quantization!\n"
	"           If #=3, use requested quantization regardless.\n"
	"           (this does not necessarily produce the same result as\n"
	"           compressing the raw image with this quantization!\n"
#endif
	"    -Qname (optional) user gives up to 192 Q-table values in text file 'name'.\n"
	"           If color there must be at least 128 values;64 if grayscale.\n"
	"           The values are assumed NOT in zig-zag order, unlike in jpg file.\n"
	"           The first 64 are for Y(luminance); if color, next 64 for Cb.\n"
	"           If there are yet 64 more they are for Cr, else Cr uses same as Cb.\n"
	"           Comments are allowed: after #, rest of line is a comment.\n"
	"           Use no commas or other marks between between q values.\n"
	"           Values must be integers between 1 and 255, inclusive.\n"
	"           However, when used for Region quantization, values may be up to\n"
	"           two bytes.\n"
	" (please hit ENTER)\n"
	);
	getchar();
	printf(
	"    -L#    gives a luminance quality rating from 0 to 255 (default 24)\n"
	"           When used for region background quality, can be up to two bytes.\n"
	"    -M#    gives a chrominance quality rating from 0 to 255 (default 30)\n"
	"           When used for region background quality, can be up to two bytes.\n"
	"    -JIname  name of jpeg file to join to the other input jpeg file. If this\n"
	"             is present, join operation is assumed. This one goes FIRST in\n"
	"             the input buffer!.\n"
	"             NOTE! Join may not be done while reorienting, cropping \n"
	"             or requantizing.\n"
	"    -JL    join left-right (default is top-bottom)\n"
	"    -JN    join by inserting rather than side-by-side.  If this is\n"
	"           done, must also specify -VX and -VY above.\n"
	"    -JTL#  This has meaning only if -JN is set.# is from 0 to 256 and gives\n"
	"           luminance transparency value for the inserted image; 0 (the\n"
	"           default)means the luminance comes entirely from the inserted image\n"
	"           in its rectangle, and 256 means that the luminance comes\n"
	"           entirely from the original image. \n"
	"    -JTC#  This has meaning only if -JN is set.# is from 0 to 256 and gives\n"
	"           chrominance transparency value for the inserted image.\n"
	"           If this not specified, it defaults to being the same as the\n"
	"           luminance transparency value.\n"
	"    -J2T   join with second image in buffer on top or left or insert\n"
	"              (default: first image in buffer).\n"
	"    -J2S   join using subsampling of second image in buffer(default: first).\n"
	"    -J2Q   join using quantization of second image in buffer(default: first).\n"
	"    -JQ    join using requested quantization (either -Qname above is\n"
	"           given or -L and -M values are used.) If this is on, -J2Q is ignored.\n"
	"    -G     The region defined by -V options above uses the quant. tables\n"
	"           from jpeg input, but the background uses quant tables defined by\n"
	"           -Q if present, or -L and -M otherwise. L and M may have values\n"
	"           bigger than 255 in this case, up to two bytes.  May not use -G if\n"
	"           doing rotate, crop, join, or requantize.\n"
	"    -ZYA#  Amount to shift luminance (Y), -256 ... 255(0 if not transforming Y).\n"
	"    -ZYM#  Amount to scale luminance (Y), -128 ... 127(0 if not transforming Y).\n"
	"    -ZBA#  Amount to shift Cb, -256 ... 255 (0 if not transforming chrominance).\n"
	"    -ZBM#  Amount to scale Cb, -128... 127 (0 if not transforming chrominance).\n"
	"    -ZRA#  Amount to shift Cr, -256 ... 255 (0 if not transforming chrominance).\n"
	"    -ZRM#  Amount to scale Cr, -128... 127 (0 if not transforming chrominance).\n"
	" (please hit ENTER)\n"
	);
	getchar();
	printf(
	"    -A#    Tell which App data segments to keep (the default is to remove\n"
	"           all App data segments except JFIF and PIC. JFIF is a special App0\n"
	"           data segment and PIC is a special App1 data segment). # may be\n"
	"           entered in Hex by prepending 0x.  The bit positions on in # \n"
	"           (numbered from least significant to most)\n"
	"           tell which App data segments to keep, other than JFIF and PIC,\n"
	"           for positions 0,1,...15 . For example, if #=0xC (1100 binary),\n"
	"           this means keep App2 and App3 data segments.  PIC is removed\n"
	"           only if bit position 17 is ON. Note that if bit 17 is on, then\n"
	"           the -C option (keep colors in APP1 PIC segment) is ignored.\n"
	"    -N     Remove existing Comment segments(the default is to keep them).\n"
	"    -S     Do not add Pegasus Imaging Corp comment (the default is to add\n"
	"           this comment(even if -N is on) if there is not already a comment\n"
	"           remaining in the file which starts with the 15 characters\n"
	"           Pegasus Imaging\n"
	"    -D#    force gray or color output.1=gray,2=color.\n"
	);
#if PC_WATCOM
	printf(
	"The file_name may be a wild card name - if so, all images are compressed\n"
	"sharing the same options and the destination name is the destination path\n"
	"followed by the file_name with .JPG or .PIC extension.  If the source is not a wild\n"
	"card name, the destination name is the destination with default .JPG or PIC extension."
	);
#endif
}

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท */
/*  ณ                                                                       บ */
/*  ณ       Parse the parameters.                                           บ */
/*  ณ                                                                       บ */
/*  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ */
PRIVATE void    parse_parms (int argc, char *argv[])
{
	LONG     i;
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
	des_ext = ".JPG";
#else
	check(argc >= 3, "need output filename. ");
	check(argv[1][0] != '-' && argv[1][0] != '/',"need input filename");
	check(argv[2][0] != '-' && argv[2][0] != '/',"need output filename");
	src_name = argv[1];
	des_name = argv[2];
#endif

	pad = 0;
	keep_colors = FALSE;
	orient = 0;
	skip_copyright = FALSE;
	get_buff_size = DEFAULT_GET_BUFF_SIZE;
	put_buff_size = DEFAULT_PUT_BUFF_SIZE;
	num_loops = 1;
	lum_opt = 24;
	chrom_opt = 30;
	requant = 0;
	QTableName[0] = '\0';
#if PIC2_OUT
	pic2_out = FALSE;
#endif
#if ELS_CODER_OUT
	els_coder_out = FALSE;
#endif
	crop_x = crop_y = crop_w = crop_h = 0;
	crop_flg = FALSE;
	memset(key, 0, sizeof(key));
	apps_to_keep = 0;
	remove_comments = 0;
	src_name_join = NULL;
	join_left_right = FALSE;
	join_insert = FALSE;
	join_second_on_top_left = FALSE;
	join_subsampling_from_second = FALSE;
	join_quantization_from_second = FALSE;
	join_use_requested_quantization = FALSE;
	regions = FALSE;
	insert_transparency_lum = insert_transparency_chrom = 0;
	YShift = YScale = CbShift = CbScale = CrShift = CrScale = 0;
	gray_or_color_out = 0;

	for (i = 2;  i < argc;  ++i)
	{
		if (argv[i][0] == '-'  ||  argv[i][0] == '/')
		{
			switch (toupper(argv[i][1]))
			{
#if PIC2_OUT
				case '2' :  pic2_out = TRUE;
							des_ext = ".PIC";
							break;
#endif
#if ELS_CODER_OUT
				case 'W' :  els_coder_out = TRUE;
							break;
#endif
				case 'K' :
						strncpy(key, argv[i]+2, 8);
						break;
								case 'X' :
						strncpy(outputkey, argv[i]+2, 8);
												break;
				case 'C' :
						keep_colors = TRUE;
						break;
				case 'P' :
						pad = atoi(argv[i]+2);
						if(pad == 0)
							pad = 1;
						check(pad <= 3 && pad >= 0, "Bad pad value");
						break;
				case 'E' :
						get_buff_size = atoi(argv[i]+2);
						check(get_buff_size >= 128, "Bad input buff size");
						break;
				case 'F' :
						put_buff_size = atoi(argv[i]+2);
						check(put_buff_size >= 128, "Bad output buff size");
						break;
				case 'T' :
						num_loops = atoi(argv[i]+2);
						check(num_loops > 0 && num_loops <= 1000, "Bad num loops");
						break;
				case 'R' :
						orient = atoi(argv[i]+2);
						check(orient >= 0 && orient <= 7, "Bad orientation");
						break;
				case 'D' :
						gray_or_color_out = atoi(argv[i]+2);
						check(gray_or_color_out >= 0 && gray_or_color_out <= 2, "Bad gray or color output value");
						break;
				case 'Q' :  strcpy(QTableName, argv[i]+2);
							break;
				case 'L' :  lum_opt = atoi(argv[i]+2);
							break;
				case 'M' :  chrom_opt = atoi(argv[i]+2);
							break;
#if REQUANT
				case 'B' :
							requant = atoi(argv[i]+2);
							if(requant == 0)
								requant = 1;
							check(requant <= 3, "Bad requant value");
							break;
#endif
				case 'A' :
							apps_to_keep = strtoul(argv[i]+2, NULL,0);
							break;
				case 'N' :
							remove_comments |= 1;
							break;
				case 'S' :
							remove_comments |= 2;
							break;
				case 'V' :
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
				case 'J' :
						switch (toupper(argv[i][2]))
						{
							case 'I' :
									src_name_join = argv[i]+3;
									break;
							case 'L' :
									join_left_right = TRUE;
									break;
							case 'N' :
									join_insert = TRUE;
									break;
							case 'T' :
									switch (toupper(argv[i][3]))
									{
											case 'L' :
													insert_transparency_lum = atoi(argv[i]+4);
													break;
											case 'C' :
													insert_transparency_chrom = atoi(argv[i]+4);
													break;
											default :
													check(FALSE, "Unknown join option %s", argv[i]);
													break;
									}
									break;
							case 'Q' :
									join_use_requested_quantization = TRUE;
									break;
							case '2' :
									switch (toupper(argv[i][3]))
									{
											case 'T' :
													join_second_on_top_left = TRUE;
													break;
											case 'Q' :
													join_quantization_from_second = TRUE;
													break;
											case 'S' :
													join_subsampling_from_second = TRUE;
													break;
											default :
													check(FALSE, "Unknown join option %s", argv[i]);
													break;
									}
									break;
							default :
									check(FALSE, "Unknown join option %s", argv[i]);
									break;
						}
						break;
				case 'G' :
						regions = TRUE;
						break;
				case 'Z' :
						switch (toupper(argv[i][2]))
						{
							case 'Y' :
								switch (toupper(argv[i][3]))
								{
									case 'A' :
										YShift = atoi(argv[i]+4);
										break;
									case 'M' :
										YScale = atoi(argv[i]+4);
										break;
									default :
											check(FALSE, "Unknown transform option %s", argv[i]);
											break;
								}
								break;
							case 'B' :
								switch (toupper(argv[i][3]))
								{
									case 'A' :
										CbShift = atoi(argv[i]+4);
										break;
									case 'M' :
										CbScale = atoi(argv[i]+4);
										break;
									default :
											check(FALSE, "Unknown transform option %s", argv[i]);
											break;
								}
								break;
							case 'R' :
								switch (toupper(argv[i][3]))
								{
									case 'A' :
										CrShift = atoi(argv[i]+4);
										break;
									case 'M' :
										CrScale = atoi(argv[i]+4);
										break;
									default :
											check(FALSE, "Unknown transform option %s", argv[i]);
											break;
								}
								break;
							default :
									check(FALSE, "Unknown transform option %s", argv[i]);
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
	if(join_insert || regions)
		crop_flg = FALSE;
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
