
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <mmsystem.h>

extern "C" int main(int argc, char **argv);

#define FOURCC_fmt		mmioFOURCC('f', 'm', 't', ' ')
#define FOURCC_data		mmioFOURCC('d', 'a', 't', 'a')

/*
// A RIFF chunk.
*/

typedef struct
{
	FOURCC	fccChunkType;
	DWORD		dwSize;
} RIFF_CHUNK;

typedef struct
{
	WAVEFORMAT	wf;
	UINT			nBitsPerSample;
} PCM_FORMAT;

/*
// Our header.
*/

typedef struct
{
	RIFF_CHUNK	header;
	char			wave[4];
	RIFF_CHUNK	fmt_chunk;
	PCM_FORMAT	format;
	RIFF_CHUNK	data_chunk;
} FILE_HEADER;

FILE_HEADER file_header;
#if 0
dest_file_header =
{
/* header */
	{
		FOURCC_RIFF,
v		0L						/* To be plugged in. */
	},
/* wave */
	"WAVE",
/* fmt_chunk */
	{
		FOURCC_fmt,
		sizeof(PCM_FORMAT)
	},
/* format */
	{
	/* WAVEFORMAT wf */
		{
			1,
			1,
			11025,
			11025,
			1,
		},
	/* nBitsPerSample */
		8
	},
/* data_chunk */
	{
		FOURCC_data,
		0L							/* To be plugged in */
	}
};
#endif

int main(int argc, char **argv)
{
/* Make sure we have an argument. */

	if (argc != 3)
	{
		printf("\nusage: wavcvt wav16file wav8file\n\n");
		return 1;
	}

/* Open the file. */

	FILE *sfile;

	if ((sfile = fopen(argv[1], "rb")) == NULL)
	{
		printf("Unable to open file '%s' for reading.\n", argv[1]);
		return 2;
	}

/* Read the file header from the source file. */

	if (fread(&file_header, 1, sizeof(file_header), sfile) != sizeof(file_header))
	{
		fclose(sfile);
		printf("Unable to read wave file header from file '%s'.\n", argv[1]);
		return 3;
	}

	if (file_header.format.nBitsPerSample != 16)
	{
		fclose(sfile);
		printf("This is not an 16-bit wave file we can operate on\n");
		return 4;
	}

/* Modify the header to have the new 8-bit values. */

	file_header.format.wf.nBlockAlign = 1;
	file_header.format.wf.nAvgBytesPerSec = file_header.format.wf.nSamplesPerSec;
	file_header.format.nBitsPerSample = 8;

	DWORD dwOldSize = file_header.data_chunk.dwSize;
	DWORD dwNewSize = dwOldSize/2;
	DWORD dwOldCount = dwNewSize/2;

	file_header.header.dwSize += (dwNewSize - dwOldSize);
	file_header.data_chunk.dwSize = dwNewSize;

	FILE * dfile;
	if ((dfile = fopen(argv[2], "w+b")) == NULL)
	{
		fclose(sfile);
		printf("Unable to create file '%s' for writing.\n", argv[2]);
		return 5;
	}

/*
// Write the header to the file.
*/

	if (fwrite(&file_header, 1, sizeof(file_header), dfile) != sizeof(file_header))
	{
		fclose(sfile);
		fclose(dfile);
		printf("Unable to write wave file header to file '%s'.\n", argv[2]);
		return 6;
	}

/*
// Read and copy all the data.
*/

	int nOldData;
//	int nError = 0;

	for (DWORD dwIndex = 0; dwIndex < dwOldCount; dwIndex++)
	{
		if (fread(&nOldData, 1, sizeof(nOldData), sfile) != sizeof(nOldData))
		{
			printf("Error reading source data during conversion.\n\n");
			return 8;
		}

		char cNewData;

//		nOldData += nError/16;
		int nNewData;

		if (nOldData < 0)
		{
			nNewData = -(-nOldData & 0xFF00);
		}
		else
		{
			nNewData = nOldData;
		}
//		nError = nOldData - nNewData;
//		printf("error: %d\n", nError);
#if 1
		cNewData = ((char)(nNewData >> 8)) + 128;
#else
		cNewData = (nOldData >> 8) + 128;
#endif

//		printf("oldData: %d (%04x), newData: %02x, old+128: %04x\n",
//					nOldData, nOldData, cNewData,
//						nOldData + ((nOldData < 0) ? -128 : 128));

		fwrite(&cNewData, 1, sizeof(cNewData), dfile);
	}

	fclose(sfile);
	fclose(dfile);

	return 0;
}
