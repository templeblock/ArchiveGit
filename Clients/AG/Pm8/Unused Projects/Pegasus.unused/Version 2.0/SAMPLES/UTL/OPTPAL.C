//optpal.c

//QuickWin test program for generating optimal palette for a group of
// images -- uses opcode 82, subcodes 1 & 2.  The resulting output is
// a .bmp file of size 1x1 pixels that has the optimal palette.

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
static long histogram_file (char *file1);
static long bytes_in_queue (QUEUE *q);
static long bytes_free_in_queue (QUEUE *q);
static void getdata (PIC_PARM *pp);
static void readdata (BYTE PICHUGE *address, long nbytes, PIC_PARM *pp);
static void fail (PIC_PARM *pp, long error_code);

static PIC_PARM pp;
        //for the opcode

char infilelist[300], palfilename[300], answer[50];
        //for user data entry

BYTE PICHUGE *input_queue;
        //the address of the allocated input queue
BYTE PICHUGE *output_queue;
        //the address of the allocated output queue
long inbufsize, outbufsize;
        //size of input queue and output queue

BYTE PICHUGE *histogram_buffer;
        //alloc'ed space for the histogram

struct _find_t fileinfo;
char x_drive[_MAX_DRIVE+1];
char x_dir[_MAX_DIR+1];
char x_fname[_MAX_FNAME+1];
char x_ext[_MAX_EXT+1];
        //for _dos_findxx

char infilename[300];
        //the built-up input file names

int filecount;  //number of files processed
int do_colors;  //number of colors (max) in the output palette

int main (int argc, char *argv[])
{
        long iresult, zed;
        int done;
        BITMAPFILEHEADER bf;
        BITMAPINFOHEADER bi;
        int palhandle;
        RESPONSE res;

        //allocate the queues
        inbufsize = 100000;     //input buffer -- can be just about anything
        outbufsize = 1000;      //output buffer -- this is not actually used
                // by these subcodes, but they should be allocated nevertheless
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
        if ((histogram_buffer = malloc (65536)) == NULL)
        {
                printf ("***** unable to allocate histogram buffer\n");
                free (input_queue);
                free (output_queue);
                return 1;
        }
        //NOTE: GHND clears this buffer, which is extremely important!!
                      
        while (1)
        {
                printf ("Enter .bmp file(s) (wildcards allowed)\n   or <Enter> for no more files: ");
                gets (infilelist);
                if (strlen (infilelist) == 0)   //enter hit
                        break;

                //split the input file(s) argument into path info
                _splitpath (infilelist, x_drive, x_dir, x_fname, x_ext);

                filecount = 0;  //number of files processed
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

                        //do the work
                        printf ("    Processing %s\n", infilename);
                        iresult = histogram_file (infilename);
                        if (iresult)
                        {
                                printf ("palettize_file failed on file %s-- error code %ld\n",
                                        infilename, iresult);
                        }
                        done = _dos_findnext (&fileinfo);
                }
                printf ("    %d file(s) processed\n", filecount);
        }

        printf ("Enter ouput palette file name: ");
        gets (palfilename);

get_colors:
        printf ("Enter number of colors for optimal palette: ");
        gets (answer);
        do_colors = atoi (answer);
        if (do_colors < 2 || do_colors > 256)
        {
                goto get_colors;
        }
        //ok -- call subcode 2 to compute the optimal palette from the histogram
        memset (&pp, 0, sizeof (pp));   //VERY IMPORTANT!
        pp.Op = OP_UTL;         //Utility opcode (82)
        pp.ParmSize = sizeof (pp);
        pp.ParmVer = CURRENT_PARMVER;   //from pic.h
        pp.u.UTL.HistogramSize = 32768; //only valid value for now
        pp.u.UTL.NumColors = do_colors;
        pp.u.UTL.ptr1 = histogram_buffer;
        pp.u.UTL.Subcode = 2;   //build palette from histogram

        //although we don't use the queues, it is a good idea to make the
        // pointers valid
        pp.Get.Start = input_queue;
        pp.Get.Front = pp.Get.Rear = pp.Get.Start;
        pp.Get.End = pp.Get.Start + inbufsize + 1;

        pp.Put.Start = output_queue;
        pp.Put.Front = pp.Put.Rear = pp.Put.Start;
        pp.Put.End = pp.Put.Start + outbufsize + 1;

        //the Pegasus loops -- special case --
        // should not request data to read or write !!
        res = Pegasus (&pp, REQ_INIT);
        if (res != RES_DONE)
        {
                if (res != RES_ERR)
                {
                        pp.Status = ERR_UNKNOWN_RESPONSE;
                }
                fail (&pp, pp.Status);
                return FALSE;
        }

        res = Pegasus (&pp, REQ_EXEC);
        if (res != RES_DONE)
        {
                if (res != RES_ERR)
                {
                        pp.Status = ERR_UNKNOWN_RESPONSE;
                }
                fail (&pp, pp.Status);
                return FALSE;
        }
        Pegasus (&pp, REQ_TERM);

        //ok -- now the palette is in ColorTable, and the resulting number
        // of colors is in NumColors

        //create the palette file and build the minimal .bmp file
        palhandle = open (palfilename, O_RDWR | O_BINARY | O_TRUNC | O_CREAT,
                S_IREAD | S_IWRITE);
        if (palhandle == -1)
        {
                printf ("***** Error creating output palette file %s\n", palfilename);
        }
        else
        {
                //build up the bitmapfileheader
                bf.bfType = 'B' + ('M' << 8);
                bf.bfReserved1 = 0;
                bf.bfReserved2 = 0;
                bf.bfSize = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER)
                        + (pp.u.UTL.NumColors << 2) + 4;
                        //the 4 is for the 1x1 bitmap -- remember that scanlines
                        // must be padded to a dword.
                //where the bitmap image is:
                bf.bfOffBits = bf.bfSize - 4;
                write (palhandle, &bf, sizeof (BITMAPFILEHEADER));
        
                //write the bitmapinfoheader out
                
                bi.biSize = sizeof (BITMAPINFOHEADER);
                bi.biWidth = 1;
                bi.biHeight = 1;
                bi.biPlanes = 1;
                if (pp.u.UTL.NumColors <= 2)
                {
                        bi.biBitCount = 1;
                }
                else if (pp.u.UTL.NumColors <= 16)
                {
                        bi.biBitCount = 4;
                }
                else
                {
                        bi.biBitCount = 8;
                }
                bi.biCompression = BI_RGB;      //0
                bi.biSizeImage = 4;
                bi.biXPelsPerMeter = 0;
                bi.biYPelsPerMeter = 0;
                bi.biClrUsed = bi.biClrImportant = pp.u.UTL.NumColors;
                write (palhandle, &bi, sizeof (BITMAPINFOHEADER));

                //write the palette out
                write (palhandle, pp.ColorTable, (short)pp.u.UTL.NumColors << 2);

                //write out a dummy scan line of 1 pixel
                zed = 0;
                write (palhandle, &zed, 4);

                //done
                close (palhandle);
                printf ("Palette file written successfully.\n");
        }

        free (input_queue);
        free (output_queue);
        free (histogram_buffer);
        return 0;
}

static int inhandle = -1;
        //the file handle

static long infilesize;
        //these are needed by the getdata callback
static int getdata_already_called;
        //when we put the entire file in the input buffer, this makes
        // sure getdata is not called again asking for more data!
static long total_bytes_read = 0;
        //how many bytes we getdata'd from the input file
static long seek_adder;
        //input file offset

static long histogram_file (char *file1)
{
        long error = 1;
        long n_input_colors;
        RESPONSE res;

        inhandle = -1;

        inhandle = open (file1, O_RDONLY | O_BINARY);
        if (inhandle == -1)
        {
                printf ("***** unable to open input image %s\n", file1);
                goto ehandler;
        }

        infilesize = filelength (inhandle);
        total_bytes_read = 0;

        //now set up for opcode 82
        memset (&pp, 0, sizeof (pp));   //VERY IMPORTANT!
        pp.Op = OP_UTL;         //Utility opcode (82)
        pp.ParmSize = sizeof (pp);
        pp.ParmVer = CURRENT_PARMVER;   //from pic.h
        pp.u.UTL.HistogramSize = 32768; //only valid value for now

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
        pp.u.UTL.Subcode = 1;   //"Add to histogram"

        pp.u.UTL.ptr1 = histogram_buffer;

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
                //NOTE: there is no output queue data for subcode 1
                //NOTE: RES_SEEK is never requested for subcode 1
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
                //NOTE: there is no output queue data for subcode 1
                //NOTE RES_SEEK is never requested for subcode 1
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

        Pegasus (&pp, REQ_TERM);
        error = 0;

ehandler:
        if (inhandle != -1)
        {
                close (inhandle);
                inhandle = -1;
        }
        if (error == 0)
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
                return; //presumably a yield call
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
