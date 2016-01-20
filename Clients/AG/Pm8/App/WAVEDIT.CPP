
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <mmsystem.h>
#include <malloc.h>
#include <ctype.h>
#include <dos.h>
#include <string.h>

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
		0L						/* To be plugged in. */
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

/*
// Scale a number by the ratio of two other numbers.
// (like muldiv).
*/

int scale_number(int x, int n, int d)
{
	int r;

	_asm
	{
		mov	ax, x
		imul	n
		idiv	d
		mov	r, ax
	}

	return r;
}

int compute_old_amplitude(FILE *sfile, DWORD dwSize, int &nAverage, DWORD __huge *pdwHistogram)
{
	long curpos = ftell(sfile);
	unsigned int uMaxAmplitude = 0;
	DWORD dwSum = 0;

	for (unsigned u = 0; u < 32768; u++)
	{
		pdwHistogram[u] = 0;
	}

	for (DWORD dwIndex = 0; dwIndex < dwSize; dwIndex++)
	{
		int nData;

		if (fread(&nData, 1, sizeof(nData), sfile) != sizeof(nData))
		{
		/* Error! */
			uMaxAmplitude = (unsigned int)-1;
			break;
		}
		if (nData == -32768)
		{
			nData = -32767;
		}

		if (nData < 0)
		{
			nData = -nData;
		}

		unsigned int uData = (unsigned int)nData;		// nData is now always >= 0

		pdwHistogram[uData]++;
		dwSum += uData;

		if (uData > uMaxAmplitude)
		{
			uMaxAmplitude = uData;
		}
	}

	fseek(sfile, curpos, SEEK_SET);

	nAverage = (int)(dwSum/dwSize);

	return (int)uMaxAmplitude;
}

/*
// Print usage.
*/

void usage(int nExitCode)
{
	printf(
"\n"
"usage: wavedit in16files [-Aamplitude] [-Ppercent] [-8] [-DOutDir] [-Eext]\n"
"where: 'in16files' are 16-bit WAV files *IN THE CURRENT DIRECTORY*\n"
"       'amplitude' is a number from 0 (silence) to 32767 (max)\n"
"       'percent' is sample percentage to use to compute old amplitude (95%)\n"
"       '-8' means convert to 8-bit\n"
"       'OutDir' is an output directory name\n"
"       'ext' is the extension to use for output files, no dot (default WAO)\n"
"\n");
	exit(nExitCode);
}

void convert_file(const char *pName,
						const char *pDestName,
						int nNewAmplitude,
						int nPercent,
						DWORD __huge *pdwHistogram,
						int nNewBits)
{

	printf("Converting '%s' to '%s'...", pName, pDestName);

/* Open the file. */

	FILE *sfile;

	if ((sfile = fopen(pName, "rb")) == NULL)
	{
		printf("\nERROR: Unable to open file '%s' for reading.\n", pName);
		return;
	}

/* Read the file header from the source file. */

	if (fread(&file_header, 1, sizeof(file_header), sfile) != sizeof(file_header))
	{
		fclose(sfile);
		printf("\nERROR: Unable to read wave file header from file '%s'.\n", pName);
		return;
	}

	if (file_header.format.nBitsPerSample != 16)
	{
		fclose(sfile);
		printf("\nERROR: This is not a 16-bit wave file we can operate on\n");
		return;
	}

/* Modify the header to have the new N-bit values. */

	file_header.format.wf.nBlockAlign = nNewBits/8;
	file_header.format.wf.nAvgBytesPerSec =
					file_header.format.wf.nSamplesPerSec *
					file_header.format.wf.nBlockAlign;
	file_header.format.nBitsPerSample = nNewBits;

	DWORD dwOldSize = file_header.data_chunk.dwSize;
	DWORD dwNewSize = dwOldSize/(16/nNewBits);

	file_header.header.dwSize += (dwNewSize - dwOldSize);
	file_header.data_chunk.dwSize = dwNewSize;

	FILE * dfile;
	if ((dfile = fopen(pDestName, "w+b")) == NULL)
	{
		fclose(sfile);
		printf("\nERROR: Unable to create file '%s' for writing.\n", pDestName);
		return;
	}

/*
// Compute how many samples are in the old file.
*/

	DWORD dwOldCount = dwOldSize/2;

/*
// Write the header to the file.
*/

	if (fwrite(&file_header, 1, sizeof(file_header), dfile) != sizeof(file_header))
	{
		fclose(sfile);
		fclose(dfile);
		printf("\nERROR: Unable to write wave file header to file '%s'.\n", pDestName);
		return;
	}

/*
// Read and copy all the data.
*/

	int nOldAverage;
	int nOldAmplitude = -1;
	int nClipAmp;

/*
// Prepare for amplitude conversion if requested.
*/

	if (nNewAmplitude != -1 && pdwHistogram != NULL)
	{
	/*
	// Generate the max amplitude and histogram.
	*/

		nOldAmplitude = compute_old_amplitude(sfile, dwOldCount, nOldAverage, pdwHistogram);

		if (nOldAmplitude == -1)
		{
			fclose(sfile);
			fclose(dfile);
			printf("\nERROR: Error reading source data to compute histogram.\n\n");
			return;
		}

	/*
	// To compute the amplitude, we count samples until we reach the desired
	// percentage. The amplitude at that point is the one we use.
	*/

		DWORD dwCount = 0;
		DWORD dwIncCount = dwOldCount*nPercent/100L;
		unsigned int uMaxAmp;

		for (uMaxAmp = 0; uMaxAmp < 32767; uMaxAmp++)
		{
			dwCount += pdwHistogram[uMaxAmp];
			if (dwCount >= dwIncCount)
			{
				break;
			}
		}

		printf("\nConvert amp (%d) from %u to %d)", nOldAmplitude, uMaxAmp, nNewAmplitude);

		nOldAmplitude = uMaxAmp;
	}

	if (nNewAmplitude > nOldAmplitude)
	{
		nClipAmp = scale_number(32766, nOldAmplitude, nNewAmplitude);
	}
	else
	{
		nClipAmp = 32766;
	}

	for (DWORD dwIndex = 0; dwIndex < dwOldCount; dwIndex++)
	{
		int nOldData;

		if (fread(&nOldData, 1, sizeof(nOldData), sfile) != sizeof(nOldData))
		{
			printf("Error reading source data during conversion.\n\n");
			fclose(sfile);
			fclose(dfile);
		}

#if 0
		int nUseData = (nOldData < 0) ? -nOldData : nOldData;

		int nDiff;

		if (nUseData < nMinAmp)
		{
			nDiff = 0;
		}
		else
		{
			nDiff = nNewAmplitude - nUseData;

			int nMaxDiff = nUseData;
			int nUseDiff = (nDiff < 0) ? -nDiff : nDiff;

			if (nUseDiff > nMaxDiff)
			{
				nUseDiff = nMaxDiff;
				nDiff = (nDiff < 0) ? -nUseDiff : nUseDiff;
			}
		}

		if (nOldData < 0)
		{
			nDiff = -nDiff;
		}

		int nNewData = nOldData + nDiff/10;

#else
		int nNewData = nOldData;

		if (nNewAmplitude != -1 && nOldAmplitude > 0)
		{
			int nTestData = (nOldData < 0) ? -nOldData : nOldData;
			if (nTestData > nClipAmp)
			{
				nOldData = (nOldData < 0) ? -nClipAmp : nClipAmp;
			}
			nNewData = scale_number(nOldData, nNewAmplitude, nOldAmplitude);
		}
#endif

		if (nNewBits == 16)
		{
		/* Write it back out. */
			fwrite(&nNewData, 1, sizeof(nNewData), dfile);
		}
		else
		{
		/* Must be 8-bit. */

			if (nNewData < 0)
			{
				nNewData = -(-nNewData & 0xFF00);
			}
			char cNewData = ((char)(nNewData >> 8)) + 128;

			fwrite(&cNewData, 1, sizeof(cNewData), dfile);
		}
	}

	fclose(sfile);
	fclose(dfile);
	printf("\nConverted successfully\n");
}

/*
// Convert all files matching the file spec passed.
*/

void convert_files(const char *pName,
						 int nNewAmplitude,
						 int nPercent,
						 DWORD __huge *pdwHistogram,
						 int nNewBits,
						 const char *pExtension,
						 const char *pDestDir)
{
	struct _find_t findbuf;

	for (unsigned uRet = _dos_findfirst(pName, _A_NORMAL|_A_RDONLY, &findbuf);
		  uRet == 0;
		  uRet = _dos_findnext(&findbuf))
	{
		char cbDestName[_MAX_PATH];
		char *p;

	/*
	// Prepend the path we want.
	*/

		if (pDestDir == NULL)
		{
			*cbDestName = '\0';
		}
		else
		{
			strcpy(cbDestName, pDestDir);
			p = cbDestName + strlen(cbDestName);
			if (p[-1] != '\\' && p[-1] != ':')
			{
				*p++ = '\\';
				*p = 0;
			}
		}

		strcat(cbDestName, findbuf.name);

	/*
	// Append the extension we want.
	*/

		if ((p = strrchr(cbDestName, '.')) == NULL)
		{
			strcat(cbDestName, pExtension);
		}
		else
		{
			strcpy(p, pExtension);
		}

		convert_file(findbuf.name,
						 cbDestName,
						 nNewAmplitude,
						 nPercent,
						 pdwHistogram,
						 nNewBits);
	}
}

int main(int argc, char **argv)
{
/* Make sure we have an argument. */

	if (argc < 2)
	{
		usage(1);
	}

/* Command line variables (with defaults). */

	int nNewAmplitude = -1;
	BOOL fConvertTo8 = FALSE;
	int nPercent = 95;
	char cbExtension[5] = ".WAO";
	char cbDestinationDir[_MAX_PATH] = "";

	char *pSourceName = NULL;

	for (int i = 1; i < argc; i++)
	{
		char *pArg = argv[i];

		if (pArg[0] == '-')
		{
			switch (pArg[1])
			{
				case 'A':
				{
					nNewAmplitude = atoi(pArg+2);
					break;
				}
				case 'P':
				{
					nPercent = atoi(pArg+2);
					break;
				}
				case 'E':
				{
					strncpy(cbExtension+1, pArg+2, 3);
					cbExtension[4] = '\0';
					break;
				}
				case 'D':
				{
					strcpy(cbDestinationDir, pArg+2);
					break;
				}
				case '8':
				{
					fConvertTo8 = TRUE;
					break;
				}
				default:
				{
					printf("\nIllegal switch '%c'\n", pArg[1]);
					usage(2);
				}
			}
		}
		else
		{
			if (pSourceName == NULL)
			{
				pSourceName = pArg;
			}
			else
			{
				printf("\nToo many filenames.\n");
				usage(3);
			}
		}
	}

	if (pSourceName == NULL)
	{
	/* No source file name! */
		usage(1);
	}

/*
// Allocate the histogram array.
*/

	DWORD __huge *pdwHistogram = NULL;

	if (nNewAmplitude != -1)
	{
		if ((pdwHistogram = (DWORD __huge *)halloc(32768, sizeof(DWORD))) == NULL)
		{
			printf("\nUnable to allocate histogram array.\n");
			exit(4);
		}
	}

	convert_files(pSourceName,
					  nNewAmplitude,
					  nPercent,
					  pdwHistogram,
					  fConvertTo8 ? 8 : 16,
					  cbExtension,
					  cbDestinationDir[0] == '\0' ? NULL : cbDestinationDir);
	exit(0);
	return 0;			// To make compiler happy...
}

