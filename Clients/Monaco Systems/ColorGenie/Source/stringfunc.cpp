#include "stringfunc.h"

static void get_block(char *c, char *c2, int n)
{
	int i , len;

	i=0;
	len = strlen(c);

	while ((i <= len) && (c[i]!=' ')){
		c2[i]=c[i];
		i++;
	}

}

static char *skip_blocks(char *c,int n)
{ 
	int i,v;

	v=0;
	for (i=0; i<n; i++)
	{
		while ((v<=strlen(c)) && (c[v]!=' ')) v++;
		while ((v<=strlen(c)) && (c[v]==' ')) v++;	
		if (v>strlen(c)) return(NULL);
	}
	return (&c[v]);
}

int sscanf_n(char *c,int start,int n, double *p)
{
	int i;
	char c1[256];
	char *c2;

	c2=skip_blocks(c,start);

	for (i=0; i< n; i++)
	{
		if(!c2)	return 0;
		get_block(c2,c1,i);
		sscanf(c1,"%lf",&p[i]);
		c2 = skip_blocks(c2, 1); 
	}

	return 1;
}


int sscanf_n_i(char *c,int start,int n, int *p)
{
	int i;
	char c1[256];
	char *c2;

	c2=skip_blocks(c,start);

	for (i=0; i< n; i++)
	{
		if(!c2)	return 0;
		memset(c1, '\0', 256);
		get_block(c2,c1,i);
		sscanf(c1,"%d",&p[i]);
		c2 = skip_blocks(c2, 1); 
	}

	return 1;
}
