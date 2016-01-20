/*.T XPG test ver 3.00
 * $Header: /PEGASUS/XETST/XETSTDDW/XETEST.C 3     8/04/97 12:28p Jim $
 * $Nokeywords: $
 */

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ    Copyright (c) 1994, Pegasus Imaging Corporation                    บ
//  ณ    All rights reserved.                                               บ
//  ณ                                                                       บ
//  ณ    Use PicQueryType, PicExpandJpegSetup, PicExpandJpeg, and           บ
//  ณ    PicExpandJpegTerm to decompress a progressive JPEG image to a BMP file.        บ
//  ณ                                                                       บ
//  รฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#pragma pack(1)

#ifndef PC_WATCOM
#define PC_WATCOM   1
#endif

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <dos.h>
#include    <string.h>
#include    <io.h>
#include    <conio.h>
#include    <ctype.h>
#include    <fcntl.h>
#include    <sys\types.h>
#include    <sys\stat.h>

#define MAX_NUM_SCANS 22

// NOTE: PegasusQuery assumes whole bmp header is in buffer, so packet
//  size must be at least large enough for that. Even worse, it assumes
// in raw JPEG case that up through frame header is all in first packet.
// It needs to be rewritten to fix this.

#define YIELD_TEST 0    // 1/0  => Yield/no yield to other window tasks
#if defined(WINDOWS) && !defined(_CONSOLE)         // Needed for Windows
        int printf(const char *f, ...) { return 0; }
		int vprintf(const char *f, va_list v) { return 0; }
		int getch(void) { return 'C'; }
        int fflush(FILE *f) { return 0; }
		#define EXPORT  __export

#else
        #define EXPORT
#endif


#include    "stdtypes.h"
#include    "errors.h"
#include    "pic.h"
#include    "bmp.h"


//  Globals for main
PRIVATE PIC_PARM  p;
PRIVATE char    src_name[100], src_path[100], des_name[100], des_path[100];
PRIVATE char    *des_ext;
PRIVATE int     src_file = -1, des_file = -1;
PRIVATE WORD    bit_count;          // specified on command line
PRIVATE WORD    thumbnail;          // "
PRIVATE WORD    num_scans;          		// "
PRIVATE BOOL	multiple_scans = FALSE; // "
PRIVATE BOOL    dither;             // "
PRIVATE BOOL	make_colors;		// "
PRIVATE BOOL	user_colors = FALSE;		// "
PRIVATE BOOL	no_gets = FALSE;		// "
PRIVATE BOOL	decode_to_data_end = FALSE;		// "
PRIVATE BOOL	no_smooth = FALSE;		// "
PRIVATE BOOL	make_top_down_dib = FALSE;
PRIVATE BOOL	test_strips = FALSE;
PRIVATE short   nprimcolors;            // "
PRIVATE short   nseccolors;            // "
PRIVATE BOOL    make_gray;          // "
PRIVATE WORD    gray_levels;        // "
PRIVATE BOOL    wild_card, skip_copyright;
PRIVATE	DWORD	image_size;
PRIVATE	DWORD	packet_size = 63*1024;
PRIVATE char    comment[256];
PRIVATE	DWORD	width_pad;
PRIVATE	DWORD	num_loops = 1;
PRIVATE	DWORD	crop_x, crop_y, crop_w, crop_h;
PRIVATE	BOOL	crop_flg;


//  Local routines
PRIVATE short   expand (char *src_name, char *des_name);
PRIVATE void    read_file (PIC_PARM *p);
PRIVATE void    copy_to_file (PIC_PARM *p);
PRIVATE void    write_bmp_header (void);
PRIVATE void    write_color_table (void);
PRIVATE void    check (BOOL exp, char *fmsg, ... );
PRIVATE void    show_copyright (void);
PRIVATE void    print_help (void);
PRIVATE void    parse_parms (int argc, char *argv[]);
PRIVATE void    copy_whole_buf_to_file (PIC_PARM *p);
PRIVATE void    write_bmp_header_and_col_table(void);

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ   Test the Pegasus routine interface.                                 บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
#if defined(WINDOWS) && !defined(_CONSOLE)         // Needed for Windows

#define argc _argc
#define argv _argv
extern  int  argc;
extern  char **argv;

int     PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrevInst,
								char *szCmdLine, int nCmdShow)
#else

int     main (int argc, char *argv[])

#endif
{
#if PC_WATCOM
		struct  find_t info;
		UINT    n;
#endif
		short   status;

		if (argc < 2) {
				print_help();   exit(1);
		}

		printf(
				"\nJ32TEST - Test prgressive JPEG by converting images to .BMP files.\n"
				"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
		);

		//  Get command line arguments.
		parse_parms(argc, argv);
		if ( sizeof(size_t) != sizeof(long) )
			test_strips = TRUE;	// must do it in 16-bit

		if (!skip_copyright)  show_copyright();

#if PC_WATCOM
		n = 0;
		if (_dos_findfirst(src_name, _A_NORMAL, &info) == 0) {
				do {
						strcpy(src_name, src_path);   strcat(src_name, info.name);
						if (wild_card) {
								strcpy(des_name, des_path);   strcat(des_name, info.name);
								strcpy(strchr(des_name, '.'), des_ext);
						}
						status = expand(src_name, des_name);
						if(status != 0)
							break;
						n++;
				} while ( _dos_findnext(&info) == 0);
		}
		printf("\nThere were %d images expanded.\n", n);
#else
    status = expand(src_name, des_name);
#endif

		if(status)
			printf("status = %d\n",status);
		return (0);
}


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE short   expand (char *src_name, char *des_name)
{
	RESPONSE    res;
	DWORD		get_buff_size;
	long		len,siz;
	char	des_namep[100], *des_name_plus;
	char    ui_drive[5], ui_dir[60], ui_fname[20], ui_ext[10],fname[20];
	char	num_buf[3];
	DWORD	scan, start_scan, last_scan, scan_inc;
	DWORD	ct; // ** Speed test uses this.


	memset(&p, 0, sizeof(PIC_PARM));    // important: set all default values 0
    // ALSO important to set the following
	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;   p.ParmVerMinor = 1;


		//  Open the PICX file
	src_file = open(src_name, O_BINARY | O_RDONLY);
	check(src_file != -1, "Could not open %s", src_name);

	//  Allocate GetBuff and place into it the information to be queried
	if(num_loops > 1)
		check(test_strips == FALSE,"Can't do strips if num_loops > 1");
	get_buff_size = filelength(src_file);
	p.Get.Start = (BYTE *) malloc(get_buff_size);
	check(p.Get.Start != NULL, "Out of memory for get buffer");
	p.Get.End = p.Get.Start + get_buff_size;
	siz = min(get_buff_size,packet_size);
	if(num_loops > 1)
		siz = get_buff_size;
	len = (unsigned)read(src_file, p.Get.Start, siz);
	check(len != (unsigned)-1, "Error reading input file");
	if(len < siz)
		p.Get.QFlags |= Q_EOF;
	p.Get.Rear = p.Get.Start + len;
	p.Get.Front = p.Get.Start;

	p.u.QRY.BitFlagsReq = QBIT_BICOMPRESSION;
	check(PegasusQuery(&p), "Invalid PegasusQuery");
	memset(&p.u.QRY, 0, sizeof(p.u.QRY));
	check(p.Head.biCompression == BI_PJPG, "Must be progressive JPEG");

		//  Set up the uncompressed image characteristics.

		//  Here is where a user's palette could be substituted for the file
		//  image's palette. If you do this, you must set p.Head.biClrUsed
		// and p.Head.biClrImportant, and load p.ColorTable. Note that
		// make_colors will later override user_colors, so this is pointless
		// if make_colors is set.

	if(user_colors)
	{
		; // stub
	}

	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;   p.ParmVerMinor = 1;
	p.Op = OP_P2D;
	p.Comment = comment;
	p.CommentSize = sizeof(comment);
	p.u.P2D.PicFlags = 0;

	p.u.P2D.DibSize = bit_count;   // or SrcBMI.biBitCount or 0 to default
	if(width_pad != 0)
	{
		check(p.Head.biWidth * ((bit_count + 7)/8) <= width_pad, "width_pad too small");
		p.u.P2D.PicFlags |= PF_WidthPadKnown;
		p.u.P2D.WidthPad = (width_pad + 3) & (-4); /* just to make a BMP file */
	}

	if(test_strips)
		p.u.P2D.PicFlags |= PF_YieldPut;
	if (dither)     p.u.P2D.PicFlags |= PF_Dither;
	if (make_gray)  p.u.P2D.PicFlags |= PF_ConvertGray;
	p.u.P2D.Thumbnail = thumbnail;

	if(no_gets)
		p.u.P2D.PicFlags |= PF_NoGets;
	else if(decode_to_data_end) //can't do this and no_gets
		p.u.P2D.PicFlags |= PF_DecodeToDataEnd;
	if(no_smooth)
		p.u.P2D.PicFlags |= PF_NoCrossBlockSmoothing;

	if((make_gray || p.Head.biBitCount <= 8) && gray_levels)
	{
		p.u.P2D.GraysToMake = gray_levels;
		/* p.u.P2D.PicFlags |= PF_MakeColors; */
	}

	p.u.P2D.PrimClrToMake = p.u.P2D.SecClrToMake = 0;
	if(p.u.P2D.DibSize == 8 && !make_gray)
		p.u.P2D.PrimClrToMake = nprimcolors;
	else if(p.u.P2D.DibSize == 4 && !make_gray)
		p.u.P2D.SecClrToMake = nseccolors;

	//************************************************************
	// if (no good (colors in file or user colors) or make_colors set) AND dibsize <=8 AND
	// output is not gray AND input is not gray,
	// set PF_MakeColors flag and set PF_CreateDibWithMadeColors flag
	// so we will make and use optimal colors.
	//************************************************************

	if( (make_colors || p.Head.biClrUsed == 0 ||
			(p.u.P2D.DibSize == 4 && p.Head.biClrImportant > 16 &&
			(p.Head.biClrUsed - p.Head.biClrImportant == 0))) &&
		p.u.P2D.DibSize <=8 &&
		!make_gray && p.Head.biBitCount > 8) // latter says input is not gray
		p.u.P2D.PicFlags |= (PF_MakeColors | PF_CreateDibWithMadeColors |
							PF_OnlyUseMadeColors);

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


	// **************************************************************
	// ** Now p.Head.biClrUsed and p.Head.biClrImportant MUST be set to
	// zero UNLESS there are user colors, because a non_zero value
	// of p.Head.biClrUsed is the way of signaling to level 2 that
	// there are user colors.

	if(!user_colors)
	{
		p.Head.biClrUsed = p.Head.biClrImportant = 0;
	}

	p.u.P2D.StripSize = 0; /* let level 2 figure this out */

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

	//  Initialize Jpeg decompression
	res = Pegasus(&p, REQ_INIT);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA)
			read_file(&p);
		else if (res == RES_ERR)
			check(FALSE, "Error number %d ",p.Status);
		else
			check(FALSE, "Unexpected or unknown response %d ", res);
		res = Pegasus(&p, REQ_CONT);
	}

		//  Allocate DIB output buffer during EXEC. Set to NULL now (important!).
	p.Put.Start = NULL;
	p.u.P2D.StripSize = 0; // let level 2 figure this out before putting first strip
	// Note: in progressive, unlike sequential, this is figured out by
	//         level 2 during EXEC (it was also calculated during INIT, but
	//         since several EXECS may be called without re-init, we redo it
	//         during EXEC).
	// 	(if it is non-zero here, level 2 will change this if necessary
	//  	to the nearest value which makes sense).


	//  Decompress the file.
	// scan is numbered starting with 1 at minimum, not 0
	// (0 is a flag to do all scans)

	des_name_plus = des_name;
	start_scan = last_scan = num_scans;
	scan_inc = 1;
	if(multiple_scans)
	{
		if(p.u.P2D.PicFlags & PF_NoGets ||
				p.u.P2D.PicFlags & PF_DecodeToDataEnd)
				start_scan = 0;
		else
			start_scan = 1;
		if(num_scans == 0)
			last_scan = MAX_NUM_SCANS;
		des_name_plus = des_namep;
		_splitpath(des_name, ui_drive, ui_dir, ui_fname, ui_ext);
	}

	scan = start_scan;
	while(scan <= last_scan)
	{
		p.u.P2D.NumScansReq = scan;
		if(multiple_scans)
		{
			if(scan == 0)
				scan = 1;
			strcpy(fname,ui_fname);
			if(scan <= 9)
				fname[7] = '\0';
			else
				fname[6] = '\0';
			itoa(scan,num_buf,10);
			strcat(fname,num_buf);
			_makepath(des_name_plus, ui_drive, ui_dir, fname, ui_ext);
		}

	for(ct = 0;ct < num_loops;++ct)
	{
		if(ct != 0)
		{ /* reset, but do not reallocate, put buffer, for speed test loops. */
			p.Put.End = p.Put.Start + image_size;
			if(p.Put.QFlags & Q_REVERSE)
			{
				p.Put.Rear = p.Put.Front = p.Put.End; /*put buff runs backwards */
			}
			else
				p.Put.Rear = p.Put.Front = p.Put.Start;
		}
		res = Pegasus(&p, REQ_EXEC);
		while (res != RES_DONE)
		{
			if (res == RES_GET_NEED_DATA)
				read_file(&p);
			else if (res == RES_PUT_NEED_SPACE)
			{
				if(p.Put.Start == NULL)
				{
					 //	if(! (p.u.P2D.PicFlags & PF_CreateDibWithMadeColors) )
					 //	{
						//  Create the BMP file
						// Create BMP header and allow for the color table if not making colors.
						// If making colors, this should not be done until after colors are made,
						// to allow for the possibility that fewer colors will be used than
						// were asked for. If you don't mind having space allowed for colors
						// which might not be used, you could write_bmp_header() here even
						// when making colors.
					 //	}

					//  Allocate DIB output buffer now

					image_size = (( (p.Head.biWidth * p.u.P2D.DibSize + 31) >> 3 ) & -4) *
									 p.Head.biHeight;
					if(test_strips)
					{
						des_file = open(des_name_plus, O_BINARY | O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
						check(des_file != -1, "Could not create %s", des_name_plus);
						write_bmp_header();
						printf("\n%3d%% done", 0);
						fflush(stdout);
						p.Put.Start = (BYTE *) malloc(p.u.P2D.StripSize);
						check(p.Put.Start != NULL, "Out of space for put buffer");
						p.Put.End = p.Put.Start + p.u.P2D.StripSize;
						if(p.Put.QFlags & Q_REVERSE)
						{
							p.Put.Rear = p.Put.Front = p.Put.End; //put buff runs backwards
							check(lseek(des_file, image_size, SEEK_CUR) != -1, "Bad des file seek");
						}
						else
							p.Put.Rear = p.Put.Front = p.Put.Start;
					}
					else
					{
						p.Put.Start = (BYTE *) malloc(image_size);
						check(p.Put.Start != NULL, "Out of space for put buffer");
						p.Put.End = p.Put.Start + image_size;
						if(p.Put.QFlags & Q_REVERSE)
						{
							p.Put.Rear = p.Put.Front = p.Put.End; //put buff runs backwards
						}
						else
							p.Put.Rear = p.Put.Front = p.Put.Start;
					}
				}
			}
			else if (res == RES_PUT_DATA_YIELD)
			{
				if(test_strips)
					copy_to_file(&p);
      }
			else if (res == RES_YIELD)
				;
			else if (res == RES_COLORS_MADE)
			{
				if(p.u.P2D.PicFlags & PF_CreateDibWithMadeColors)
				{
					// one could set the colors now
					;
				}
      }
			else if (res == RES_ERR)
				check(FALSE, "Error number %d ",p.Status);
			else
				check(FALSE, "Unexpected or unknown response %d ", res);
			res = Pegasus(&p, REQ_CONT);
		}

	} /* end of ct loop */

		printf("Scans done = %d\n", p.u.P2D.NumScansDone);
		printf("Bytes done = %d\n", p.u.P2D.NumBytesDone);
		if(p.Status == ERR_SCAN_NON_EXIST) // non-terminal "error"( >0 )
				// num scans req. more than scans in file
		{
			printf("Num scans requested more than scans that exist/n");
			scan = last_scan = 0;
			continue;
		}
		if(p.Status == ERR_REQ_SCAN_NOT_IN_BUF) // non-terminal "error"( >0 )
		{
				// num scans req. more than scans in buffer, for no-gets
			break;
		}

			//  Finish .BMP file

		if(test_strips)
		{
			write_color_table();
		}
		else
		{
			printf("filled buffer... now writing out file\n");
			des_file = open(des_name_plus, O_BINARY | O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
			check(des_file != -1, "Could not create %s", des_name_plus);
			write_bmp_header_and_col_table();
			copy_whole_buf_to_file(&p);
		}
		printf("\n");
		if (p.CommentLen > 0)
		{
			p.Comment[min(p.CommentLen, p.CommentSize-1)] = '\0';
			printf("Comment: %s\n", p.Comment);
		}
		printf("Expanded file %s into %s.\n", src_name, des_name_plus);

			//  Close out file, free out memory.
		close(des_file);
		free(p.Put.Start);   p.Put.Start = NULL;
		if(p.u.P2D.PicFlags & PF_EOIFound)
			break;
		if(	p.u.P2D.PicFlags & PF_NoGets ||
				p.u.P2D.PicFlags & PF_DecodeToDataEnd)
			scan = p.u.P2D.NumScansDone + 1;
		else
			scan += scan_inc;
	}

	Pegasus(&p, REQ_TERM);

		// Close src file and memory

	close(src_file);
	free(p.Get.Start);   p.Get.Start = NULL;
	return (p.Status);
}

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ   This is a callback routine invoked by PicExpandJpegStartup and by   บ
//  ณ   PicExpandJpeg to load a buffer pointed to by BuffPtr from the next  บ
//  ณ   successive *BuffLen bytes in the src_file.  In the case that        บ
//  ณ   fewer than *BuffLen bytes can be read, *BuffLen is set to the       บ
//  ณ   actual number of bytes read.  An error is returned if read-error    บ
//  ณ   or end-of-file is encountered, otherwise 0 is returned.             บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#if 1

PRIVATE void   read_file (PIC_PARM *p)
{
	long    avail,len,siz;

		/****/

	if ( p->Get.Rear >= p->Get.Front)
	{
		if( (avail = p->Get.End - p->Get.Rear) == 0)
		{
			p->Get.QFlags |= Q_EOF;
			return;
		}
		siz = min(avail,packet_size);
		len = (unsigned)read(src_file, p->Get.Rear, siz);
		if (len == (unsigned)-1)     // read error
			p->Get.QFlags |= Q_IO_ERR;
		else
		{
			p->Get.Rear += len;
			if(len < siz)
				p->Get.QFlags |= Q_EOF;
		}
	}
	else  // Front not before Rear for PICX
		p->Get.QFlags |= Q_IO_ERR;


#if defined( WINDOWS) & YIELD_TEST
	// yield test
		MSG msg;
		if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage((LPMSG) &msg);
				DispatchMessage((LPMSG) &msg);
	}
#endif
}

#endif

#if 0

PRIVATE void   read_file (PIC_PARM *p)
{
	long    avail,len;

		/****/

	if ( p->Get.Rear >= p->Get.Front)
	{
		avail = p->Get.End - p->Get.Rear;
		if(avail > 0)
		{
			len = (unsigned)read(src_file, p->Get.Rear, avail);
			if (len == (unsigned)-1)
			{    // read error
    			p->Get.QFlags |= Q_IO_ERR;
				return;
			}
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
		len = (unsigned)read(src_file, p->Get.Rear, avail);
		if (len == (unsigned)-1)
		{    // read error
			p->Get.QFlags |= Q_IO_ERR;
			return;
		}
		p->Get.Rear += len;
		if(len < avail)
			p->Get.QFlags |= Q_EOF;
	}


#if defined( WINDOWS) & YIELD_TEST
	// yield test
		MSG msg;
		if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage((LPMSG) &msg);
				DispatchMessage((LPMSG) &msg);
	}
#endif
}

#endif


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ   This is a callback routine invoked by PicExpandJpeg to save the     บ
//  ณ   DIB strip in StripBuff of BuffLen bytes out to the des_file.  The   บ
//  ณ   PercentComplete is 100 when the image has been completely sent.     บ
//  ณ   Return a non-zero result to abort the decompression, any other      บ
//  ณ   value will be passed back from PicExpandJpeg.                       บ
//  ณ                                                                       บ
//  ณ   Note that StripBuffPtr is a pointer to a pointer to a buffer.  This บ
//  ณ   allows the user to change the output buffer loaction after each     บ
//  ณ   strip has been decompressed.  The next strip will be decompressed   บ
//  ณ   at the new location - an application does not have to copy a buffer บ
//  ณ   to another location, rather by adjusting *StripBuffPtr the copy     บ
//  ณ   is done automatically to the proper location.  (The initial call    บ
//  ณ   to PicExpandJpeg specified where the first strip went.)             บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void   copy_to_file (PIC_PARM *p)
{
	long siz;

		/****/

		//  Usually the JPEG image is top-down, a BMP is bottom-up so must reverse
	if(p->Put.QFlags & Q_REVERSE)
	{      // Need to reverse
		siz = p->Put.End - p->Put.Rear;

		if (lseek(des_file, -siz, SEEK_CUR) == -1) {
			p->Put.QFlags |= Q_IO_ERR;
			return;
		}
		if (write(des_file, p->Put.Rear, siz) < siz) {
			p->Put.QFlags |= Q_IO_ERR;
			return;
		}
		if (lseek(des_file, -siz, SEEK_CUR) == -1) {
			p->Put.QFlags |= Q_IO_ERR;
			return;
		}

		p->Put.Rear = p->Put.End;

	}
	else
	{
		siz = p->Put.Rear - p->Put.Start;

		if (write(des_file, p->Put.Start, siz) < siz) {
			p->Put.QFlags |= Q_IO_ERR;
			return;
		}
		p->Put.Rear = p->Put.Start;
	}
	printf("\r%3d%% done", p->PercentDone);
	fflush(stdout);
#if defined(WINDOWS) & YIELD_TEST
		// yield test
				MSG msg;
	if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE)) {
						TranslateMessage((LPMSG) &msg);
						DispatchMessage((LPMSG) &msg);
	}
#endif
}

PRIVATE void   copy_whole_buf_to_file (PIC_PARM *p)
{
		/****/

	if (write(des_file, p->Put.Start, image_size) < image_size)
	{
		p->Status = ERR_BAD_WRITE;
		return;
	}
}


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ   Create an uncompressed BMP file.  Fill in the information using     บ
//  ณ   data from the compressed BMP image and from user parameters.        บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void    write_bmp_header (void)
{
		BITMAPFILEHEADER    des_fhead;

		/****/

		des_fhead.bfType = 0x4D42;      // "BM"
		des_fhead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
										 p.Head.biClrUsed*4;
		des_fhead.bfSize = des_fhead.bfOffBits + p.Head.biSizeImage;
		des_fhead.bfReserved1 = 0;
		des_fhead.bfReserved2 = 0;

	check(write(des_file, &des_fhead, sizeof(BITMAPFILEHEADER)) != -1  &&
			write(des_file, &p.Head, sizeof(BITMAPINFOHEADER)) != -1,
			"Error writing file header");
	lseek(des_file, p.Head.biClrUsed*4, SEEK_CUR);
}

PRIVATE void    write_color_table (void)
{

	if (p.Head.biClrUsed > 0) {
		lseek(des_file, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
				SEEK_SET);
		check(write(des_file, p.ColorTable, p.Head.biClrUsed*4) != -1,
					"Error writing color table");
	}
}

PRIVATE void    write_bmp_header_and_col_table(void)
{
		BITMAPFILEHEADER    des_fhead;

		/****/

		des_fhead.bfType = 0x4D42;      // "BM"
		des_fhead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
										 p.Head.biClrUsed*4;
		des_fhead.bfSize = des_fhead.bfOffBits + p.Head.biSizeImage;
		des_fhead.bfReserved1 = 0;
		des_fhead.bfReserved2 = 0;

	check(write(des_file, &des_fhead, sizeof(BITMAPFILEHEADER)) != -1  &&
			write(des_file, &p.Head, sizeof(BITMAPINFOHEADER)) != -1,
			"Error writing file header");
	if (p.Head.biClrUsed > 0) {
		lseek(des_file, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
				SEEK_SET);
		check(write(des_file, p.ColorTable, p.Head.biClrUsed*4) != -1,
					"Error writing color table");
	}
}

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ       Check that exp is TRUE.  If it is not then print the formatted  บ
//  ณ   message fmsg, and terminate the program.                            บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void    check (BOOL exp, char *fmsg, ... )
{
        va_list marker;

		if (!exp) {
                va_start(marker, fmsg);
				printf("Program terminated: ");  vprintf(fmsg, marker);  printf(".\n");
                if (src_file != -1)  close(src_file);
                if (des_file != -1)  close(des_file);
                exit(1);
        }
}


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ   Show the copyright information.                                     บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void    show_copyright (void)
{
		printf("\nJ32TEST - Test 32-bit PJPEG by converting images to .BMP files.\n"
				   "(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n\n"
				   "    (Voice) 813-875-7575  (Fax) 813-875-7705\n\n"
				   "Program Version 3.00\n\n"
                );
        printf("Press 'C' to continue.. ");
        fflush(stdout);
        while ('C' != getch()) ;    // Await user response
        printf("\n");
}


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ       Print help message.                                             บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void    print_help (void)
{
        printf(
				"\nJ32TEST - Test Progressive JPEG by converting images to .BMP files.\n"
				"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
				"\nusage:  J32TEST  file_name [destination[.BMP]]  {options}\n"
				"where options are:\n"
				"    -B#    bit length (# = 4, 8, 16, or 24 - default is 24)\n"
				"    -G[#]  convert image to # gray-scale levels (2 ๓ # ๓ 256) - default is 256\n"
				"    -N     no dithering of the image\n"
				"    -M     make colors for colormapped image, don't use ones in input file\n"
				"    -P#    use # colors in palette (2 ๓ |#| ๓ 256), # < 0 if secondary palette\n"
				"    -H     no data to be read after init.(just use what's in buffer) \n"
				"    -E     decode to end of data in buf and keep going(get data as needed) \n"
				"    -C     no cross-block smoothing(default is to smooth)\n"
				"    -D     make top-down dib rather than bottom up(for windows)\n"
				"    -S     separately output each scan up to last one requested\n"
				"    -Z     write file output in strips(slower, as a test or to save mem.)\n"
				"    -A#    packet size in bytes(default 64K)\n"
		"    -W#  width_pad(Must be at least true image width*bytesperpixel.).\n"
		"           For this test program,rounded up to mult. of 4 to make valid BMP.\n"
		"    -T[#] make thumbnail image;3 => 1/64(default),2 => 1/16,1 -> 1/4 size.\n"
		"    -F[#]  num. scans to do(default:all). ( 1 => dc only).\n"
		"    -L#  num loops(1 <= # <= 1000)for speed test,not to be used with -Z option\n"
		"    -RX#   x coord. of cropped rectangle (default 0)\n"
		"    -RY#   y coord. of cropped rectangle(default 0)\n"
		"    -RW#   width of cropped rectangle(default full image)\n"
		"    -RH#   height of cropped rectangle(default full image)\n"
		"     (press any key) \n"
		);
		getch();
		printf(
				"Choosing the -S option causes scan-numbers to be appended to dest. name.\n"
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
				"is only done if there are 256 colors or less.\n"
				"The file_name may be a wild card name - if so, all images are compressed\n"
				"sharing the same options and the destination name is the destination path\n"
		"followed by the file_name with .BMP extension.  If the source is not a wild\n"
		"card name, the destination name is the destination with default .BMP extension."
		);
}


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ       Parse the parameters.                                           บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void    parse_parms (int argc, char *argv[])
{
		char    ci_drive[5], ci_dir[60], ci_fname[20], ci_ext[10], *p;
		INT     i, nc;
		BOOL    have_name;

		wild_card = (strchr(argv[1], '*') != NULL  ||
								 strchr(argv[1], '?') != NULL);
		_fullpath(src_name, argv[1], sizeof(src_name));   strupr(src_name);
		_splitpath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
		if (ci_ext[0] == '\0')  strcpy(ci_ext, ".PIC");
		_makepath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
		_makepath(src_path, ci_drive, ci_dir, "", "");

		des_ext = ".BMP";
		bit_count = 24;   gray_levels = 256;   thumbnail =  0;
		num_scans = 0; // this means do ALL scans
		nprimcolors = 0; nseccolors = 0; make_colors = FALSE;
		multiple_scans = FALSE;
		test_strips = FALSE;
		skip_copyright = make_gray = FALSE;   dither = TRUE;
		user_colors = FALSE; // Just a stub for now.
		no_gets = FALSE;
		decode_to_data_end = FALSE;
		no_smooth = FALSE;
		make_top_down_dib = FALSE;
		width_pad = 0;
		num_loops = 1;
		crop_x = crop_y = crop_w = crop_h = 0;
		crop_flg = FALSE;

		for (i = 2;  i < argc;  ++i) {
				if (argv[i][0] == '-'  ||  argv[i][0] == '/') {
						switch (toupper(argv[i][1])) {
								case 'B' :
										bit_count = atoi(argv[i]+2);
										break;
								case 'G' :
										make_gray = TRUE;
										gray_levels = atoi(argv[i]+2);
										if(gray_levels == 0)
											gray_levels = 256;
										break;
								case 'M' :
										make_colors = TRUE;
										break;
								case 'N' :
										dither = FALSE;
										break;
								case 'H' :
										no_gets = TRUE;
										break;
								case 'E' :
										decode_to_data_end = TRUE;
										break;
								case 'C' :
										no_smooth = TRUE;
										break;
								case 'D' :
										make_top_down_dib = TRUE;
										break;
								case 'S' :
										multiple_scans = TRUE;
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
								case 'A' :
										packet_size = atoi(argv[i]+2);
										if(packet_size < 128)
											packet_size = 128;
										break;
								case 'T' :
										thumbnail = atoi(argv[i]+2);
										if (thumbnail == 0)  thumbnail = 3;
										check(thumbnail >= 1  &&  thumbnail <= 4, "Bad thumbnail");
										break;
								case 'F' :
										num_scans = atoi(argv[i]+2);
										check(num_scans <= MAX_NUM_SCANS, "Bad num scans");
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
		have_name = (argc > 2  &&  argv[2][0] != '-'  &&  argv[2][0] != '/');
		if (have_name) {
				_fullpath(des_name, argv[2], 100);   strupr(des_name);
		} else {
				strcpy(des_name, src_name);
		}
		if (wild_card) {
                if (have_name) {
						p = des_name + strlen(des_name);
                        if (p[-1] != '\\'  &&  p[-1] != ':')  *((WORD *) p) = 0x005C;
                }
                _splitpath(des_name, ci_drive, ci_dir, ci_fname, ci_ext);
                _makepath(des_path, ci_drive, ci_dir, "", "");
        } else {
                p = strchr(des_name, '.');
                if (have_name) {
                        if (p == NULL)  strcat(des_name, des_ext);
                } else {
                        strcpy(p, des_ext);
								}
        }
}
