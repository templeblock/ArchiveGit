// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

/*****************************************************************************
 Replacement WINSTUB.EXE for windows apps. This stub will start windows and
 the application simply by typing the application's name at the DOS prompt.

 compile and link line - uses C6 W4 and either real or implict real lib.

 cl -AS -W[4, 3] -c stub.c
 link stub, , , slibce[r], nul

*****************************************************************************/
extern int execv(char *, char **);
extern int execvp(char *, char **);
extern int execlp(char *, char *, ...);

#define WINDOWS "WIN "

/*****************************************************************************
 main - works out how much memory is in the argv array of char pointers as
 strings plus 'WIN' plus all the spaces and a NULL terminator. calls
 malloc to allocate the memory then adds the strings 'WIN' and argv strings
 to it. adds a null to the end of it, and causes windows
 to start with the app and arguments passed in lpszCmdLine to the windows
 app. finally it frees the memory.
*****************************************************************************/
int main( int argc, char **argv )
{
int nStrlen;
int i;
char *lpMem;
char *szChar;

if ( !(lpMem = ( char * )malloc( 5 )) )
	{
	printf( "Alloc error\n" );
	return( 1 );
	}

lpMem[0] = 'w';
lpMem[1] = 'i';
lpMem[2] = 'n';
lpMem[3] = ' ';
lpMem[4] = 0;
if ( execlp( "WIN", *argv, NULL ) < 0 )
	printf( "This Application requires Windows\n" );
printf("huh!");
return;

nStrlen = 0;
for ( i = 0; i<argc; i++ )
	nStrlen += strlen( *( argv+i ) );
nStrlen += argc;
nStrlen += strlen( WINDOWS );
nStrlen++;

if ( !(lpMem = ( char * )malloc( nStrlen )) )
	{
	printf( "This Application requires Windows\n" );
	return( 1 );
	}

strcpy( lpMem, WINDOWS );
for ( i = 0; i<argc; i++ )
	{
	strcat( lpMem, *(argv+i) );
	strcat( lpMem, " " );
	}

//if ( system( lpMem ) )
//	printf( "This Application requires Windows\n" );
free( lpMem );
return( 0 );
}
