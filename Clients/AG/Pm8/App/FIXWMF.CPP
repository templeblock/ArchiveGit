#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#define MAGIC_OFFSET 0x0122

int main(int argc, char**argv)
{
	int nRet = 1;

/* Validate number of arguments. */

	if (argc != 2)
	{
		printf("\nUsage: fixwmf file.wmf\n");
		return nRet;			// 1
	}

/* Get the file name. */

	char* pName = argv[1];

/* Open the file. */

	int nFile;

	nRet++;				// -> 2

	if ((nFile = open(pName, O_RDWR | O_BINARY)) == -1)
	{
		printf("\nUnable to open file '%s' for reading and writing.\n", pName);
		return nRet;			// 2
	}

/* Seek to the offset of what we want to modify. */

	lseek(nFile, MAGIC_OFFSET, SEEK_SET);

	int nData[2];

	nRet++;				// -> 3

	if (read(nFile, (char*)nData, sizeof(nData)) == sizeof(nData))
	{
	/* Validate the data. */
		nRet++;				// -> 4
		if (nData[0] == 0x102 && nData[1] == 1)		// 1 = transparent
		{
			nData[1] = 2;										// 2 = opaque
			lseek(nFile, MAGIC_OFFSET, SEEK_SET);
			nRet++;				// -> 5
			if (write(nFile, (char*)nData, sizeof(nData)) == sizeof(nData))
			{
				nRet = 0;
			}
			else
			{
				printf("\nError writing data back to file '%s'.\n", pName);
			}
		}
		else
		{
			printf("\nFile '%s' does not have data to be modified.\n", pName);
		}
	}
	else
	{
		printf("\nError reading data from file '%s'.\n", pName);
	}

/* Close the file now that we are done with it. */

	close(nFile);

	return nRet;
}