#include <stdio.h>
#include "zlib.h"
#include <string.h>
#include <stdlib.h>


void main (int argc, char *argv[])
{
	if (argc < 3)
	{
		printf ("Usage: %s [-d] input output\n", argv[0]);
		exit (0);
	}

	if (strcmp (argv[1], "-d") == 0)
	{
		FILE *input;
		FILE *output;

		if ((input = fopen (argv[2], "rb")) == NULL)
		{
			printf ("Can't open file %s\n", argv[2]);
			exit (0);
		}

		if ((output = fopen (argv[3], "wb")) == NULL)
		{
			printf ("Can't open file %s\n", argv[3]);
			fclose (input);
			exit (0);
		}

		fseek (input, 0, SEEK_END);
		uLong lFileLen = ftell (input);
		fseek (input, 0, SEEK_SET);

		Bytef *pBytesIn = (Bytef *) malloc (lFileLen);
		fread (pBytesIn, lFileLen, 1, input);
		fclose (input);

		uLong lUncompressedLen = *((long *) pBytesIn);
		Bytef *pBytesOut = (Bytef *) malloc (lUncompressedLen);
	    uncompress (pBytesOut, &lUncompressedLen, pBytesIn + 4, lFileLen - 4);

		fwrite (pBytesOut, lUncompressedLen, 1, output);
		fclose (output);

		free (pBytesIn);
		free (pBytesOut);
	}
	else
	{
		FILE *input;
		FILE *output;

		if ((input = fopen (argv[1], "rb")) == NULL)
		{
			printf ("Can't open file %s\n", argv[1]);
			exit (0);
		}

		if ((output = fopen (argv[2], "wb")) == NULL)
		{
			printf ("Can't open file %s\n", argv[2]);
			fclose (input);
			exit (0);
		}

		fseek (input, 0, SEEK_END);
		uLong lFileLen = ftell (input);
		fseek (input, 0, SEEK_SET);

		Bytef *pBytesIn = (Bytef *) malloc (lFileLen);
		fread (pBytesIn, lFileLen, 1, input);
		fclose (input);

		fwrite (&lFileLen, sizeof (lFileLen), 1, output);

		Bytef *pBytesOut = (Bytef *) malloc (lFileLen * 2);

		uLong lCompressedLen;
	    compress2 (pBytesOut, &lCompressedLen, pBytesIn, lFileLen,
		  Z_BEST_COMPRESSION);
		fwrite (pBytesOut, lCompressedLen, 1, output);
		fclose (output);

		free (pBytesIn);
		free (pBytesOut);
	}

	exit (0);
}
