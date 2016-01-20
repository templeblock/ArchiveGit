/***************************************************************************
**
**      Filename: dcmpbin.c
**
**      Description: This file contains source code for the ccitt group
**              three decompression routines.
**
**
**      Creation Date: 3/2/1987
**
**      Modification History:
**
**									*/
/*     This source code is provided courtesy of Hewlett-Packard     */
/*     Company, Greeley Division.  It will need to be modified for  */
/*     data transfer applications.                                  */
/*                                                                  */
/*     Licensed by Hewlett-Packard and Microsoft to developers for  */
/*     use with Microsoft Windows.  Please consult your license     */
/*     agreement concerning your rights to modify and distribute.   */
/*								    */
/*     Copyright Hewlett-Packard Company, 1987.			    */
/********************************************************************/


/*
* This program decompresses data with the one dimensional CCITT
* standard algorithm.
*/

#include "ccitt.h"
#include <malloc.h>
#include <stdio.h>

extern int DBUG_LEV;

TREE *TreeArray[2] = {NULL,NULL}; /* Trees for white and black decoding. */

struct record           /* Keep track of color changes. */
{
                int BitCount,Color;
};

/******************************************************************************
*                                   DCMPBIN                                   *
*******************************************************************************
*                                                                             *
* Status = dcmpbin(InputBuffer,OutputBuffer,ScratchBuffer,LineCount,FirstLine,*
*               ImageWidth,BufferSize,File,WhiteValue);                       *
*                                                                             *
*       int Status Zero if the operation was completed with no errors,        *
*               negative if an error occured                                  *
*       char *InputBuffer Pointer to a buffer for holding the compressed      *
*               data as it is read from the file                              *
*       char *OutputBuffer Pointer to a buffer for holding the decompressed   *
*               data                                                          *
*       struct record *ScratchBuffer Pointer to a buffer to hold the runs of  *
*               bits as they are decoded from the file. This buffer must be a *
*               maximum of 4 * bits per line of the image bytes in size       *
*       int LineCount Number of lines to decomppress from the file into the   *
*               output buffer                                                 *
*       int FirstLine Number of lines to decomppress from the file before     *
*               starting to store the decompressed data to the output buffer  *
*       int ImageWidth Number of pixels in a line of the image                *
*       int BufferSize Number of bytes in the input buffer                    *
*       int File DOS File handle for the file containing the data to be       *
*               decompressed. The file pointer for the file should be pointing*
*               to the first byte of the first line to be decompressed        *
*       int WhiteValue This is used to determine what value shoud be written  *
*               in the output buffer to represent white                       *
*                       0: 0 is white, 1 is black                             *
*                       1: 1 is white, 0 is black                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*               This routine is used to decompress data encoded by the TIFF   *
*       standard adaptation of CCITT Group 3 FAX Encoding (TIFF Compression   *
*       method 2). The data is decompressed from the current location of the  *
*       file into the output buffer. FirstLine lines of data are decompressed *
*       discarding the data to advance the file pointer to the desired line,  *
*       then LineCount lines are decompressed and the data placed in the      *
*       output buffer                                                         *
*                                                                             *
*******************************************************************************
*/

dcmpbin(InputBuffer,OutputBuffer,ScratchBuffer,LineCount,FirstLine,ImageWidth,
                BufferSize,File,WhiteValue)
        char *InputBuffer,*OutputBuffer;
        struct record ScratchBuffer[];
        int LineCount,FirstLine,ImageWidth,BufferSize,File,WhiteValue;
{
        int CurrentLine,Status,OutBuffPosition;

        if (DBUG_LEV >= 2) printf("entering dcmpbin()\n");

        if (TreeArray[WHITE] == NULL) {
                TreeArray[WHITE] = ReadTree(&WhiteTreeTable);
                if (TreeArray[WHITE] == NULL) return(CANTBUILDCODETREE);
        }
        if (TreeArray[BLACK] == NULL) {
                TreeArray[BLACK] = ReadTree(&BlackTreeTable);
                if (TreeArray[BLACK] == NULL) return(CANTBUILDCODETREE);
        }

        OutBuffPosition = 0;

        /* Intialize the input buffer handling */
        Status = ParseBits(ScratchBuffer,File,ImageWidth,InputBuffer,
                BufferSize,-1);

        for (CurrentLine = 0; CurrentLine < (LineCount+FirstLine);
                                                           CurrentLine++) {
                Status = ParseBits(ScratchBuffer,File,ImageWidth,
                        InputBuffer,BufferSize,WHITE);
                if (Status < 0) return(Status);
                if (Status != ImageWidth) return(LINESIZEMISCOMPARE);

                if ((OutputBuffer != NULL) && (CurrentLine >= FirstLine)) {
                        Status = PutLine(ScratchBuffer,OutputBuffer,
                                &OutBuffPosition,BufferSize,WhiteValue);
                        if (Status < 0) return(Status);
                        if (Status != ImageWidth) return(LINESIZEMISCOMPARE);
                }
        }

        if (DBUG_LEV >= 2) printf("leaving dcmpbin()\n");

        return(0);
}

/******************************************************************************
*                                PARSE BITS                                   *
*******************************************************************************
*                                                                             *
* BitsRead = ParseBits(ScratchBuff,File,ImageWidth,InBuff,BuffSize,Start);    *
*                                                                             *
*       int BitsRead Number of bits wide of the decoded line, negative if an  *
*               error occured                                                 *
*       struct record *ScratchBuffer Pointer to a buffer to hold the runs of  *
*               bits as they are decoded from the file. This buffer must be a *
*               maximum of 4 * bits per line of the image bytes in size       *
*       int File DOS File handle for the file containing the data to be       *
*               decompressed.                                                 *
*       int ImageWidth Number of pixels in a line of the image                *
*       char *InBuff Pointer to a buffer for holding the compressed data as it*
*                is read from the file                                        *
*       char *OutputBuffer Pointer to a buffer for holding the decompressed   *
*               data                                                          *
*       int BuffSize Number of bytes in the input buffer                      *
*       int Start Which tree to began using for the decompression, a value of *
*               -1 is used to initialize the buffer management                *
*                                                                             *
*******************************************************************************
*                                                                             *
*               This routine is used to parse a line of an image from the     *
*       compressed input stream. It keeps parsing and recording runs of bits  *
*       until eneuph bits to fill the line have been decoded                  *
*                                                                             *
*******************************************************************************
*/

ParseBits(ScratchBuff,File,ImageWidth,InBuff,BuffSize,Start)
        char *ScratchBuff;
        int File,ImageWidth;
        char InBuff[];
        int BuffSize,Start;
{
        TREE *usetree;
        register struct node *current;
        register int i,newtree,index;
        int Length,LastRun;
        static int BytesInBuffer = 0;
        static int BufferPosition = 0;

        if (DBUG_LEV >= 3) printf("entering ParseBits(%lX,%d,%d,%lX,%d,%d)\n",
                ScratchBuff,File,ImageWidth,InBuff,BuffSize,Start);

        if (Start == -1) { /* Intialize the input buffer handleing */
                BytesInBuffer = 0;
                BufferPosition = 0;
                return(0);
        }

        usetree = TreeArray[Start];
        current = usetree->root;
        Length = 0;
        LastRun = 0;

        while ((Length <  ImageWidth) || (LastRun > 63)){
                if (BytesInBuffer == 0) {
                        BytesInBuffer = DosRead(File,InBuff,BuffSize);
                        if (BytesInBuffer <= 0) return(FILEIOERROR);
                        BufferPosition = 0;
                }
                for (i = 0; i<8; i++) {
                        index = (InBuff[BufferPosition] & mask[i]) ? 1 : 0;
                        if ((current = current->next[index]) == NULL) {
                                printf("Parsing Error\n");
                                return(PARSINGERROR);
                        }
                        if (current->token >= 0) {
                                Length += current->token;
                                LastRun = current->token;
                                newtree =
                                    DoToken((current->token),ScratchBuff);
                                if (newtree < 0) return(newtree);
                                usetree = TreeArray[newtree];
                                current = usetree->root;
                        }
                }
                BytesInBuffer--;
                BufferPosition++;
        }

        newtree = DoToken(EOL,ScratchBuff);
        if (newtree < 0) return(newtree);

        if (DBUG_LEV >= 3) printf("leaving ParseBits(%lX,%d,%d,%lX,%d,%d )= %d\n",
                ScratchBuff,File,ImageWidth,InBuff,BuffSize,Start,Length);

        return(Length);
}


/******************************************************************************
*                                  DO TOKEN                                   *
*******************************************************************************
*                                                                             *
* CurrentColor = DoToken(Token,ScratchBuffer);                                *
*                                                                             *
*       int CurrentColor The color of the next run (the next tree to use for  *
*               parsing the compressed data)                                  *
*       int Token The length of the decoded run                               *
*       struct record *ScratchBuffer Pointer to a buffer to hold the runs of  *
*               bits as they are decoded from the file. This buffer must be a *
*               maximum of 4 * bits per line of the image bytes in size       *
*                                                                             *
*******************************************************************************
*                                                                             *
*               This routine records a run of bits of the current color in    *
*       the scratch buffer and determines the color of the next run           *
*                                                                             *
*******************************************************************************
*/

int DoToken(Token,ScratchBuffer)
        int Token;
        struct record ScratchBuffer[];
{
        static CurrentColor = WHITE;
        static Index = 0;   /* Number of words to get from w/b tables. */
        int error;

        if (DBUG_LEV >= 4) printf("entering DoToken(%d,%lX)\n",
                Token,ScratchBuffer);

        if (Token == EOL) {
                ScratchBuffer[Index].Color = WHITE;
                ScratchBuffer[Index].BitCount = EOL;
                CurrentColor = WHITE;
                Index = 0;
        }
        else {
                ScratchBuffer[Index].Color = CurrentColor;
                ScratchBuffer[Index].BitCount = Token;
                if (Token < 64)     /* If a terminating code word. */
                        CurrentColor = 1 - CurrentColor;
                Index++;
        }

        if (DBUG_LEV >= 4) printf("leaving DoToken(%d,%lX) = %d\n",
                Token,ScratchBuffer,CurrentColor);

        return(CurrentColor);
}

/******************************************************************************
*                                  PUT LINE                                   *
*******************************************************************************
*                                                                             *
* BitsPlaced = PutLine(ScratchBuffer,OutBuff,BuffPosition,WhiteValue)         *
*                                                                             *
*       int BitsPlaced The width in bits of the line placed in the output     *
*               buffer*
*       struct record *ScratchBuffer Pointer to a buffer to holding the runs  *
*               of bits decoded from the file                                 *
*       char *OutBuff Pointer to a buffer for holding the decompressed data   *
*       int *BuffPosition The location in the output buffer to place the next *
*               byte                                                          *
*       int WhiteValue This is used to determine what value shoud be written  *
*               in the ouput buffer to represent white                        *
*                       0: 0 is white, 1 is black                             *
*                       1: 1 is white, 0 is black                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*               This routine takes the runs of color an length recorded in    *
*       the scratch buffer and converts it to bytes in the output buffer. If  *
*       the line does not end on a byte boundry, the line is padded with      *
*       white bits to end on a byte boundry                                   *
*                                                                             *
*******************************************************************************
*/

PutLine(ScratchBuffer,OutBuff,BuffPosition,WhiteValue)
        struct record ScratchBuffer[];
        char OutBuff[];
        int  *BuffPosition,WhiteValue;
{
        int Index,Length,BitsOnEnd;

        if (DBUG_LEV >= 3) printf("entering PutLine(%lX,%lX,%d,%d)\n",
                ScratchBuffer,OutBuff,*BuffPosition,WhiteValue);

        Index = 0;
        Length = 0;
        while (ScratchBuffer[Index].BitCount != EOL) {
                BitsOut(ScratchBuffer[Index].BitCount,
                        ScratchBuffer[Index].Color,OutBuff,BuffPosition,
                                                                WhiteValue);
                Length += ScratchBuffer[Index].BitCount;
                Index++;
        }

        /* Make sure that the line ends on a byte boundry */

        BitsOnEnd = 8 - (Length - ((Length/8)*8));
        if (BitsOnEnd != 0) BitsOut(BitsOnEnd,WHITE,OutBuff,BuffPosition,
                                                                WhiteValue);

        if (DBUG_LEV >= 3) printf("leaving PutLine(%lX,%lX,%d) = %d\n",
                ScratchBuffer,OutBuff,WhiteValue,Length);

        return(Length);
}

/******************************************************************************
*                                   BITS OUT                                  *
*******************************************************************************
*                                                                             *
* BitsOut(n,Color,OutBuff,BuffPosition,WhiteValue);                  *
*                                                                             *
*       int n The number of bits to write                                     *
*       int Color The color of the bits                                       *
*       char *OutBuff Pointer to a buffer for holding the decompressed data   *
*       int *BuffPosition The location in the output buffer to place the next *
*               byte                                                          *
*       int WhiteValue This is used to determine what value shoud be written  *
*               in the ouput buffer to represent white                        *
*                       0: 0 is white, 1 is black                             *
*                       1: 1 is white, 0 is black                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*               This routine takes a run of a given color and writes the      *
*       correct bits into the buffer                                          *
*                                                                             *
*******************************************************************************
*/

BitsOut(n,Color,OutBuff,BuffPosition,WhiteValue)
/* Actually write out the proper number of bits. */
        register int n;
        int Color;
        char OutBuff[];
        int *BuffPosition,WhiteValue;
{
        static int bitsin = 0;
        static unsigned char old = 0;
        unsigned char usual;
        register int width,field;
        static unsigned char fieldm[9] = {0,0x1,0x3,0x7,0xf,0x1f,0x3f,0x7f,0xff};

        if (DBUG_LEV >= 4) printf("entering BitsOut(%d,%d,%lX,%d,%d)\n",
                n,Color,OutBuff,BuffPosition);

        usual = Color ? 0xff : 0x00;
        if (WhiteValue == 0) usual = ~usual;
        while (n > 0) {
                width = n<(8-bitsin) ? n : 8-bitsin;
                field = Color ? fieldm[width] : 0;
                old  = old | (field << (8 - bitsin - width));
                bitsin += width;
                if (bitsin == 8) {
                        OutBuff[(*BuffPosition)++] = old;
                        old = 0;
                        bitsin = 0;
                }
                n -= width;

                /* Now - if n is still large then bitsin is aligned on
                        a byte boundary, and we can save time by doing a byte
                        at a time. */

                while (n >= 8) {
                        OutBuff[(*BuffPosition)++] = usual;
                        n -= 8;
                }
        }

        if (DBUG_LEV >= 4) printf("leaving BitsOut(%d,%d,%lX,%d,%d) = 0\n",
                n,Color,OutBuff,BuffPosition);

        return(0);
}


/***************************************************************************
**
**      Filename: tree.c
**
**      Description: This file contains source code for the code tree
**              building routines for the ccitt group three decompression
**              routines.
**
**
**      Creation Date: 3/2/1987
**
**      Modification History:
**
**									*/
/*     This source code is provided courtesy of Hewlett-Packard     */
/*     Company, Greeley Division.                                  */
/*                                                                  */
/*     Licensed by Hewlett-Packard and Microsoft to developers for  */
/*     use with Microsoft Windows.  Please consult your license     */
/*     agreement concerning your rights to modify and distribute.   */
/*								    */
/*     Copyright Hewlett-Packard Company, 1987.			    */					
/********************************************************************/

#include "tree.h"
#include <stdio.h>

#define ASIZE 100

extern int DBUG_LEV;

struct node *npr;

/******************************************************************************
*                                  NEW NODE                                   *
*******************************************************************************
*                                                                             *
* NodePointer = newnode();                                                    *
*                                                                             *
*       struct node *NodePointer Pointer to a new elememnt of type node. Null *
*               if an error occured                                           *
*                                                                             *
*******************************************************************************
*                                                                             *
*               This routine is used to allocate space for the nodes of a     *
*       code tree. To conserve space, nodes are allocated from a large block  *
*       of memory that is expanded as space is required                       *
*                                                                             *
*******************************************************************************
*/

static struct node *newnode()

{
        static int nalloc = 0, position = 0;
        char *malloc(),*realloc();

        if (DBUG_LEV >= 8) printf("entering newnode()\n");

        if (!nalloc) {
                if ((npr = (struct node *) malloc(ASIZE*sizeof(struct node))) == NULL) {
                        printf("error in memory allocation\n");
                        return(NULL);
                }
                nalloc = ASIZE;
        }
        if (position >= nalloc) {
                nalloc += ASIZE;
                if ((npr = (struct node *) realloc(npr,nalloc*sizeof(struct node))) == NULL) {
                        printf("error in memory allocation\n");
                        return(NULL);
                }
        }
        npr[position].token = UNINIT;
        npr[position].next[0] = NULL;
        npr[position].next[1] = NULL;

        if (DBUG_LEV >= 8) printf("leaving newnode() = %lX\n",&npr[position]);

        return(&npr[position++]);
}

/******************************************************************************
*                                   READ TREE                                 *
*******************************************************************************
*                                                                             *
* TreePointer = ReadTree(Table);                                                 *
*                                                                             *
*       TREE *NodePointer Pointer to a node of the newly constructed code     *
*               tree. Null if an error occured                                *
*                                                                             *
*******************************************************************************
*                                                                             *
*               This routine is used to allocate space for the nodes of a     *
*       code tree. To conserve space, nodes are allocated from a large block  *
*       of memory that is expanded as space is required                       *
*                                                                             *
*******************************************************************************
*/

TREE *ReadTree(Table)
        TABLE *Table;
{
        int Token,Index,Position;
        char *str,*ptr;
        struct node *p;
        TREE *t;

        if (DBUG_LEV >= 6) printf("entering readtree(%lX)\n",Table);

        if ((t = (TREE *) malloc(sizeof(TREE))) == NULL) {
                return(NULL);
        }
        t->id = Table->id;
        t->root = newnode();
        for (Position = 0, Token = Table->pair[0].token; Token >= 0;
                            Position++, Token = Table->pair[Position].token) {
                str = Table->pair[Position].str;
                ptr = str;
                p = t->root;
                while (*ptr) {
                        Index = *ptr++ - '0';
                        if (Index && (Index != 1)) {
                                free(t);
                                return(NULL);
                        }
                        if (p->next[Index] == NULL) p->next[Index] = newnode();
                        p->token = NOTLEAF;
                        p = p->next[Index];
                        if (p->token >= 0) {
                                free(t);
                                return(NULL);
                        }
                }
                if (p->token != UNINIT) {
                        free(t);
                        return(NULL);
                }
                p->token = Token;
        }

        if (DBUG_LEV >= 6) printf("entering readtree(%lX) = %lX\n",Table,t);

        return(t);
}

