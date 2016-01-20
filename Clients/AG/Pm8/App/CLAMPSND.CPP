
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

extern "C" int main(int argc, char **argv);

int main(int argc, char **argv)
{
/* Make sure we have an argument. */

	if (argc != 2)
	{
		printf("\nusage: clampsnd wav8file\n\n");
		return 1;
	}

/* Open the file. */

	int file;
	if ((file = open(argv[1], _O_RDWR | _O_BINARY)) == -1)
	{
		printf("Unable to open file '%s' for reading.\n");
		return 2;
	}

/* Seek to the start of the data. */

	lseek(file, 0x2c, SEEK_SET);

	unsigned char bData;

	while (read(file, &bData, sizeof(bData)) == sizeof(bData))
	{
		if (bData == 0x7f || bData == 0x81)
		{
			bData = 0x80;
			lseek(file, -1, SEEK_CUR);
			write(file, &bData, sizeof(bData));
		}
	}

	close(file);

	return 0;
}
