// Implement pascal equivalents of c string processing
// pascal_string.c
// Copied from colorfix.c
//
// James

#include "think_bugs.h"
#include <stdio.h>

// add a pascal string into a pascal string signaled by a % character

void add_string(unsigned char *s1, unsigned char *s2)
{
int i,p;

if (s1[0] + s2[0] > 255) return;
for (i=0; i<s1[0]; i++) if (s1[i+1] == '%') break;
p = i;
for (i=s1[0]; i>p; i--)
	{
	s1[i+s2[0]] = s1[i+1];
	}
s1[0] += s2[0]-1;
for (i=0; i<s2[0]; i++)
	{
	s1[p+1+i] = s2[i+1];
	}
}

// Copy a pascal string.  copy s2 into s1

void copy_str(unsigned char *s1, unsigned char *s2)
{
int i;

for (i=0; i<s2[0]; i++) s1[i+1] = s2[i+1];
s1[0] = s2[0];
}

// Compare strings s2 and s1

int cmp_str(unsigned char *s1, unsigned char *s2)
{
int i;

if (s1[0] != s2[0]) return 0;

for (i=0; i<s2[0]; i++) if (s1[i+1] != s2[i+1]) return 0;
return 1;
}


// Cat a pascal string

void cat_str(unsigned char *s1, unsigned char *s2)
{
int i;
int	start;

start = s1[0]+1;
for (i=0; i<s2[0]; i++) 
	{
	s1[i+start] = s2[i+1];
	s1[0]++;
	}
}

// Remove everything but numbers from a string

void clean_text(unsigned char *s1)
{
int i,j;

for (i=0; i<s1[0]; i++) if ((s1[i+1] < 0x030) || (s1[i+1] > 0x039))
	{
	for (j=i; j<s1[0]-1; j++)
		{
		s1[j+1] = s1[j+2];
		}
	s1[0]--;
	} 
}

// get a floating point number from a pascal string

void StringToDouble(unsigned char *s1,double *d)
{
char *s2;

s2 = ptocstr(s1);
sscanf(s2,"%lf",d);
ctopstr(s2);
}


// get a pascal string from a floating point number with the desired percision

void DoubleToString(double d,unsigned char *s1,int dperc)
{
char *s2;
char format[255];

sprintf(format,"%%.%df",dperc);

s2 = ptocstr(s1);
sprintf(s2,format,d);
//sprintf(s2,"%.0f",d);
ctopstr(s2);
}
