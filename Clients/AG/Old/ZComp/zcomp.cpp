#include <io.h> // for open, seek, write, close
#include <sys/stat.h> // for open pmodes
#include <fcntl.h> // for open flags
#include <stdio.h> // for printf
#include <string.h> // for strcmp
#include <stdlib.h> // for exit
#include "zlib.h"

void main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("Usage: %s [-d] input output\n", argv[0]);
		exit(0);
	}

	bool bDecompress = (!strcmp(argv[1], "-d"));
	char* pstrFileIn = (bDecompress ? argv[2] : argv[1]);
	char* pstrFileOut = (bDecompress ? argv[3] : argv[2]);

	int input = NULL;
	if ((input = _open(pstrFileIn, _O_RDONLY|_O_BINARY)) == -1)
	{
		printf("Can't open file %s\n", pstrFileIn);
		exit(0);
	}

	uLong iBytesToRead = _lseek(input, 0, SEEK_END);
	_lseek(input, 0, SEEK_SET);

	Byte* pBytesIn = (Byte*)malloc(iBytesToRead);
	if (!pBytesIn)
	{
		printf("File allocation error\n");
		_close(input);
		exit(0);
	}

	int iBytesRead = _read(input, pBytesIn, iBytesToRead);
	_close(input);

	if (iBytesRead != iBytesToRead)
	{
		printf("Can't read file %s\n", pstrFileIn);
		free(pBytesIn);
		exit(0);
	}

	uLong iBytesToWrite = (bDecompress ? *((long*)pBytesIn) : (iBytesToRead * 2));
	Byte* pBytesOut = (Byte*)malloc(iBytesToWrite);
	if (!pBytesOut)
	{
		printf("File allocation error\n");
		free(pBytesIn);
		exit(0);
	}

	int output = NULL;
	if ((output = _open(pstrFileOut, _O_TRUNC|_O_CREAT|_O_WRONLY|_O_BINARY, _S_IREAD|_S_IWRITE/*used during creates*/)) == -1)
	{
		printf("Can't open file %s\n", pstrFileOut);
		free(pBytesIn);
		free(pBytesOut);
		exit(0);
	}

	int iError = -1;
	int iBytesWritten = 0;
	if (bDecompress)
	{
		iError = uncompress(pBytesOut, &iBytesToWrite, pBytesIn + 4, iBytesToRead - 4);
		if (iError >= 0)
		{
			iBytesWritten = _write(output, pBytesOut, iBytesToWrite);
		}
	}
	else
	{
		iError = compress2(pBytesOut, &iBytesToWrite, pBytesIn, iBytesToRead, Z_BEST_COMPRESSION);
		if (iError >= 0)
		{
			iBytesWritten = _write(output, &iBytesToRead, sizeof(iBytesToRead));
			iBytesWritten = _write(output, pBytesOut, iBytesToWrite);
		}
	}

	_close(output);
	free(pBytesIn);
	free(pBytesOut);

	if (iError < 0)
		printf("Compress/Uncompress error (%d)\n", iError);

	if (iBytesWritten != iBytesToWrite)
		printf("Can't write file %s\n", pstrFileOut);

	exit(0);
}
