//sub7tst.c

//QuickWin test program for opcode 82, subcode 7 (color reduction [palettization] to
// an optimal palette) -- compiled using Microsoft Visual C++ 1.52c

//NOTE that there is little or no error checking on reads, writes, seeks, etc.

//NOTE that a 24-bmp file with an included palette is not properly handled
// by this sample program, as well as any weird bmp file that has extra
// information between the BITMAPINFO structure and the dib bits area.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dos.h>
#include <string.h>
#include <io.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <stdtypes.h>
#include <errors.h>
#include <pic.h>

//local routines
static long palettize_file (char *file1, char *file2, int do_bpp, int do_colors);
static long bytes_in_queue (QUEUE *q);
static long bytes_free_in_queue (QUEUE *q);
static void getdata (PIC_PARM *pp);
static void putdata (PIC_PARM *pp);
static void writedata (BYTE PICHUGE *address, long nbytes, PIC_PARM *pp);
static void readdata (BYTE PICHUGE *address, long nbytes, PIC_PARM *pp);
static void fail (PIC_PARM *pp, long error_code);

static PIC_PARM pp;
	//for the opcode

char infilelist[300], outdirectory[300], answer[50];
	//for user data entry

BYTE PICHUGE *input_queue;
	//the address of the allocated input queue
BYTE PICHUGE *output_queue;
	//the address of the allocated output queue
long inbufsize, outbufsize;
	//size of input queue and output queue

struct _find_t fileinfo;
char x_drive[_MAX_DRIVE+1];
char x_dir[_MAX_DIR+1];
char x_fname[_MAX_FNAME+1];
char x_ext[_MAX_EXT+1];
	//for _dos_findxx

char infilename[300], outfilename[300];
	//the built-up input and output file names
int do_dither;
	//1 if we are doing dithering, 0 otherwise
int do_bpp;
	//bits per pixel output: 1, 4, or 8
int do_colors;
	//number of colors: 2 -- (1 << bpp)

int filecount;	//number of files processed

int main (int argc, char *argv[])
{
	long iresult;
	int done;
	              
	printf ("Enter source files (wildcards allowed): ");
	gets (infilelist);
	printf ("Enter destination directory: ");
	gets (outdirectory);
getdither:
	printf ("Do you want to dither the images? ");
	gets (answer);
	if (strlen (answer) > 1)
	{
		goto getdither;
	}
	if (answer[0] == 'y' || answer[0] == 'Y')
	{
		do_dither = 1;
	}
	else if (answer[0] == 'n' || answer[0] == 'N')
	{
		do_dither = 0;
	}
	else
	{
		goto getdither;
	}
get_bpp:
	printf ("Enter bits per pixel output (1, 4, or 8): ");
	gets (answer);
	if (strlen (answer) != 1)
	{
		goto get_bpp;
	}
	if (answer[0] != '1' && answer[0] != '4' && answer[0] != '8')
	{
		goto get_bpp;
	}
	do_bpp = answer[0] - '0';
get_colors:
	printf ("Enter number of colors: ");
	gets (answer);
	do_colors = atoi (answer);
	if (do_colors < 2 || do_colors > (1 << do_bpp))
	{
		goto get_colors;
	}

	//allocate the queues
	inbufsize = 10000;	//input buffer -- can be just about anything
	outbufsize = 12000;	//output buffer -- can be just about anything
	if ((input_queue = malloc (inbufsize + 1)) == NULL)
	{
		printf ("***** unable to allocate input queue\n");
		return 1;
	}
	if ((output_queue = malloc (outbufsize + 1)) == NULL)
	{
		printf ("***** unable to allocate output queue\n");
		free (input_queue);
		return 1;
	}

	//split the input file(s) argument into path info
	_splitpath (infilelist, x_drive, x_dir, x_fname, x_ext);

	filecount = 0;	//number of files processed
	done = _dos_findfirst (infilelist, _A_NORMAL, &fileinfo);
	while (done == 0)
	{
		//build the input and output file names
		strcpy (infilename, x_drive);
		strcat (infilename, x_dir);
		if (infilename[strlen(infilename)-1] != '\\')
		{
			strcat (infilename, "\\");
		}
		strcat (infilename, fileinfo.name);
		strcpy (outfilename, outdirectory);
		if (outfilename[strlen(outfilename)-1] != '\\')
		{
			strcat (outfilename, "\\");
		}
		strcat (outfilename, fileinfo.name);

		//do the work
		printf ("Converting %s to %s\n", infilename, outfilename);
		iresult = palettize_file (infilename, outfilename, do_bpp, do_colors);
		if (iresult)
		{
			printf ("palettize_file failed on file %s-- error code %ld\n",
				infilename, iresult);
		}
		done = _dos_findnext (&fileinfo);
	}
	printf ("%d file(s) converted", filecount);
	free (input_queue);
	free (output_queue);
	return 0;
}

static int inhandle = -1;
static int outhandle = -1;
	//the file handles

static long infilesize;
	//these are needed by the getdata callback
static int getdata_already_called;
	//when we put the entire file in the input buffer, this makes
	// sure getdata is not called again asking for more data!
static long total_bytes_read = 0;
	//how many bytes we getdata'd from the input file
static long seek_adder;
	//input file offset
	
static char palbuf[1024];	//the resulting palette

static long palettize_file (char *file1, char *file2, int do_bpp, int do_colors)
{
	long error = 1;
	long n_input_colors;
	RESPONSE res;
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	inhandle = -1;
	outhandle = -1;

	//open the files
	inhandle = open (file1, O_RDONLY | O_BINARY);
	if (inhandle == -1)
	{
		printf ("***** unable to open input image %s\n", file1);
		goto ehandler;
	}
	outhandle = open (file2, O_RDWR | O_BINARY | O_CREAT | O_TRUNC,
		S_IREAD | S_IWRITE);
	if (outhandle == -1)
	{
		printf ("***** unable to create output file %s\n", file2);
		goto ehandler;
	}

	infilesize = filelength (inhandle);
	total_bytes_read = 0;

	//write a dummy bitmapfileheader for now
	write (outhandle, &bf, sizeof (BITMAPFILEHEADER));
	//write a dummy bitmapinfoheader for now
	write (outhandle, &bi, sizeof (BITMAPINFOHEADER));
	//write the new palette
	memset (palbuf, 0, 1024);
	write (outhandle, palbuf, do_colors << 2);
		//note that when we palettize the image, we may actually use
		// les than this number of colors.

	//now set up for opcode 82
	memset (&pp, 0, sizeof (pp));	//VERY IMPORTANT!
	pp.Op = OP_UTL;		//Utility opcode (82)
	pp.ParmSize = sizeof (pp);
	pp.ParmVer = CURRENT_PARMVER;	//from pic.h

	//get the input file's bitmapinfoheader into pp.Head
	// -- skip the BITMAPFILEHEADER
	lseek (inhandle, sizeof (BITMAPFILEHEADER), SEEK_SET);
	read (inhandle, &pp.Head, sizeof (BITMAPINFOHEADER));
	seek_adder = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER);
	//if the input file is < 24bpp, read in that palette
	if (pp.Head.biBitCount <= 8)
	{
		if (pp.Head.biClrUsed == 0)
		{
			//0 means max colors
			n_input_colors = (1 << pp.Head.biBitCount);
		}
		else
		{
			n_input_colors = pp.Head.biClrUsed;
		}
		if (n_input_colors < 2 || n_input_colors > (1 << pp.Head.biBitCount))
		{
			printf ("***** input palette has bad color count\n");
			goto ehandler;
		}
		read (inhandle, pp.ColorTable, (unsigned short)n_input_colors << 2);
		seek_adder += (n_input_colors << 2);
	}
	//NOTE: at this point, seek_adder is the offset in the input file of
	// the dib bits
	pp.u.UTL.Subcode = 7;	//"Palettize to given palette"

	pp.u.UTL.ptr1 = NULL;	//actually, already null
	pp.u.UTL.ptr2 = NULL;   //ditto
	pp.u.UTL.OutBpp = do_bpp;	//output bit depth
	if (do_dither)
	{
		pp.u.UTL.PicFlags = PF_Dither;
	}
	pp.u.UTL.NumColors = do_colors;	//number of colors to reduce to
    	pp.u.UTL.AllocType = 0;	//we will not allocate full buffers for the
		//input and output files -- we will just process in serial "chunks"

	pp.Get.Start = input_queue;
	pp.Get.Front = pp.Get.Rear = pp.Get.Start;
	pp.Get.End = pp.Get.Start + inbufsize + 1;

	pp.Put.Start = output_queue;
	pp.Put.Front = pp.Put.Rear = pp.Put.Start;
	pp.Put.End = pp.Put.Start + outbufsize + 1;

	getdata_already_called = 0;
	lseek (inhandle, seek_adder, SEEK_SET);
		//seek to the beginning of the dib bits

	//the Pegasus loops
	res = Pegasus (&pp, REQ_INIT);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA || res == RES_GET_DATA_YIELD)
		{
			getdata (&pp);
		}
		else if (res == RES_PUT_DATA_YIELD || res == RES_PUT_NEED_SPACE)
		{
			putdata (&pp);
		}
		//NOTE: RES_SEEK is never requested at init for subcode 7
		else
		{
			if (res != RES_ERR)
			{
				pp.Status = ERR_UNKNOWN_RESPONSE;
			}
			fail (&pp, pp.Status);
			return FALSE;
		}
		//actually, for this subcode, INIT does basicallay nothing
		res = Pegasus (&pp, REQ_CONT);
	}

	res = Pegasus (&pp, REQ_EXEC);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA || res == RES_GET_DATA_YIELD)
		{
			getdata (&pp);
		}
		else if (res == RES_PUT_DATA_YIELD || res == RES_PUT_NEED_SPACE)
		{
			putdata (&pp);
		}
		else if (res == RES_SEEK)
		{
			//seeking is only on the input file for this subcode
			if (pp.SeekInfo & SEEK_FILE)
			{
				//seek on output file -- not required by this subcode
				pp.Status = ERR_UNKNOWN_RESPONSE;
				fail (&pp, pp.Status);
				return FALSE;
			}
			if (pp.SeekInfo & SEEK_DIRECTION)
			{
				//I'm lazy -- this subcode only seeks from beg. of file
				pp.Status = ERR_UNKNOWN_RESPONSE;
				fail (&pp, pp.Status);
				return FALSE;
			}
			lseek (inhandle, seek_adder + (pp.SeekInfo & SEEK_OFFSET),
				SEEK_SET);
			getdata_already_called = 0;
			total_bytes_read = 0;
		}
		else
		{
			if (res != RES_ERR)
			{
				pp.Status = ERR_UNKNOWN_RESPONSE;
			}
			fail (&pp, pp.Status);
			return FALSE;
		}
		res = Pegasus (&pp, REQ_CONT);
	}

	//flush the output
	if (pp.Put.Front != pp.Put.Rear)
	{
		putdata (&pp);
	}

	//write the bitmapfileheader out
	lseek (outhandle, 0, SEEK_SET);
	bf.bfType = 'B' + ('M' << 8);
	bf.bfReserved1 = 0;
	bf.bfReserved2 = 0;
	bf.bfSize = filelength (outhandle);
	//where the bitmap image is:
	bf.bfOffBits = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER)
		+ (do_colors << 2);
	write (outhandle, &bf, sizeof (BITMAPFILEHEADER));

	//write the bitmapinfoheader out
	//but first, set the number of colors -- color reduction might use
	// less than we request, and bi must match the palette we previously
	// output
	pp.u.UTL.BiOut.biClrUsed = pp.u.UTL.BiOut.biClrImportant
		= pp.u.UTL.NumColors;
	write (outhandle, &pp.u.UTL.BiOut, sizeof (BITMAPINFOHEADER));
	write (outhandle, pp.ColorTable, do_colors << 2);

	Pegasus (&pp, REQ_TERM);
	error = 0;

ehandler:
	if (inhandle != -1)
	{
		close (inhandle);
		inhandle = -1;
	}
	if (outhandle != -1)
	{
		close (outhandle);
		outhandle = -1;
	}
	if (error)
	{
		remove (file2);
	}
	else
	{
		filecount++;
	}
	return error;
}

#define READ_AT_A_TIME 30000
	//max we will read at a time when a get-data is requested

static void getdata (PIC_PARM *pp)
{
	//reads more data into the Get queue
	//As written, this routine FILLs the Get queue.
	//This is not necessary; you can put in as much or as little as you like
	// each time.

	long ndo, ix;
	QUEUE *q;

	q = &pp->Get;
	if (total_bytes_read >= infilesize - seek_adder)
	{
		//the opcode requested too much data
		printf ("***** getdata -- attempt to read past end of file\n");
		fail (pp, 1);
	}
	ndo = bytes_free_in_queue (q);
	if (ndo <= 0)
	{
		return;	//presumably a yield call
	}
	ndo = min (ndo, infilesize - seek_adder - total_bytes_read);
	if (ndo + q->Rear <= q->End)
	{
		//all at once
		readdata (q->Rear, ndo, pp);
		q->Rear += ndo;
		if (q->Rear >= q->End)
		{
			q->Rear = q->Start;
		}
	}
	else
	{
		//in 2 chunks
		readdata (q->Rear, ix = (q->End - q->Rear), pp);
		readdata (q->Start, ix = (ndo - ix), pp);
		q->Rear = q->Start + ix;
	}
	total_bytes_read += ndo;
}

static void putdata (PIC_PARM *pp)
{
	//write data from the put queue to the output file

	long avail, ix;
	QUEUE *q;

	q = &pp->Put;
	if ((avail = bytes_in_queue(q)) <= 0)
	{
		return;	//presumably due to yield
	}
	//something there -- it may wrap
	if (avail + q->Front <= q->End)
	{
		//all in one chunk
		writedata (q->Front, avail, pp);
		q->Front += avail;
		if (q->Front >= q->End)
		{
			q->Front = q->Start;
		}
	}
	else
	{
		writedata (q->Front, ix = (q->End - q->Front), pp);
		writedata (q->Start, ix = (avail - ix), pp);
		q->Front = q->Start + ix;
	}
	return;
}

static void writedata (BYTE PICHUGE *address, long nbytes, PIC_PARM *pp)
{
	//do ~30K at a time
	short ndo;

	while (nbytes > 0)
	{
		if (nbytes > 30000)
		{
			ndo = 30000;
		}
		else
		{
			ndo = (short)nbytes;
		}
		if (write (outhandle, address, ndo) != ndo)
		{
			printf ("***** writedata -- error writing output file\n");
			fail (pp, 1);
		}
		nbytes -= ndo;
		address += ndo;
	}
	return;
}

static void readdata (BYTE PICHUGE *address, long nbytes, PIC_PARM *pp)
{
	//do ~30K at a time
	short ndo;

	while (nbytes > 0)
	{
		if (nbytes > 30000)
		{
			ndo = 30000;
		}
		else
		{
			ndo = (short)nbytes;
		}
		if (read (inhandle, address, ndo) != ndo)
		{
			printf ("***** readdata -- error reading input file\n");
			fail (pp, 1);
		}
		nbytes -= ndo;
		address += ndo;
	}
	return;
}

static void fail (PIC_PARM *pp, long error_code)
{
	//terminates Pegasus and sets error code

	Pegasus (pp, REQ_TERM);
	pp->Status = error_code;
	if (inhandle != -1)
	{
		close (inhandle);
		inhandle = -1;
	}
	if (outhandle != -1)
	{
		close (outhandle);
		outhandle = -1;
	}
	remove (outfilename);
	printf ("!!!!Error -- code %ld\n", error_code);
	return;
}

//FIXUP is necessary because all routines in this module always assume
// that the Front and Rear pointers point to a location WITHIN the ring,
// but it is not terribly hard to imagine someone leaving one of those
// pointers pointing just past the end of the buffer

#define FIXUP(q) \
	if (q->Front >= q->End) \
	{ \
		q->Front = q->Start; \
	} \
	if (q->Rear >= q->End) \
	{ \
		q->Rear = q->Start; \
	}

PUBLIC long bytes_in_queue (QUEUE *q)
{
	//returns number of bytes in a queue

	long ix;

	FIXUP (q);
	if (q->Front == q->Rear)
	{
		return 0;
	}
	if ((ix = (q->Rear - q->Front)) > 0)
	{
		return ix;
	}
	return (q->End - q->Front) + (q->Rear - q->Start);
}

PUBLIC long bytes_free_in_queue (QUEUE *q)
{
	//returns amount of space available in a queue

	long ix;

	FIXUP (q);
	ix = q->Rear - q->Front;
	if (ix >= 0)
	{
		return (q->End - q->Start) - ix - 1;
	}
	else
	{
		return -ix - 1;
	}
}
