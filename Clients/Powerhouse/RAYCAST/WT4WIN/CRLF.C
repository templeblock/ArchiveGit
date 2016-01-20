#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

main(argc,argv)
int argc;
char *argv[];
{
	FILE *fp;

	if (argc<2) {
		printf("Usage: crlf filename\nExplodes filename from UNIX style newline to DOS newline, spewing to stdout.\n");
		printf("Redirect to the nearest bucket.\n");
		exit(1);
	}

	fp = fopen(argv[1],"rb");
	if (fp == NULL) { 
		printf("Can't open file %s\n",argv[1]);
		exit(2);
	}

		//s l o w   but effective and easy.

//	strupr(argv[1]);
//	printf("/* (WT) %12s */\n", argv[1]);

	while (!feof(fp)) {
		char c = fgetc(fp);
		if (c == 0x0a) {
			fputchar('\n');
		} else {
			fputchar(c);
		}
	}

	close(fp);
}

