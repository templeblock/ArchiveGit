#include <stdio.h>
#include <stdlib.h>


char			bfr[1558];
char			rgb[1024];
unsigned char	r, g, b, ri, gi, bi;
int				i, j, k, l;

FILE			*stream;

main ()
{

stream = fopen("DEVELOP.PAL","r+b");
if (stream == NULL)
{
	perror("Reading Palette:");
	exit(1);
}

i = fread(bfr,1,1558,stream);
if (i != 1558)
{
	printf("Could not read the palette file\n");
	exit(1);
}
printf("Palette file read and ready\n");


/*	Do solid RED */
ri = 17; gi = 0; bi = 0;
r = 0; g = 0; b = 0;

for (i=0, j=22, k=0, l=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do solid GREEN */
ri = 0; gi = 17; bi = 0;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do solid BLUE */
ri = 0; gi = 0; bi = 17;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 17; gi = 9; bi = 5;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 17; gi = 5; bi = 9;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 9; gi = 17; bi = 5;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 5; gi = 17; bi = 9;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 17; gi = 17; bi = 5;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 17; gi = 17; bi = 9;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 9; gi = 5; bi = 17;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 5; gi = 9; bi = 17;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 17; gi = 5; bi = 17;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 17; gi = 9; bi = 17;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 5; gi = 17; bi = 17;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do Shades of colors */
ri = 9; gi = 17; bi = 17;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);

/*	Do the shades of gray */
ri = 17; gi = 17; bi = 17;
r = 0; g = 0; b = 0;

for (i=0 ; i < 16 ; i++, k++)
{
	bfr[j++] = r;
	bfr[j++] = g;
	bfr[j++] = b;
	rgb[l++] = r;
	rgb[l++] = g;
	rgb[l++] = b;
	rgb[l++] = 4;
	r += ri; if (r > 255) r = 255;
	g += gi; if (g > 255) g = 255;
	b += bi; if (b > 255) b = 255;
}
printf("%d entries complete\n",k);


for (i=0 ; i < 768 ; i++) bfr[22+768+i] = bfr[22+i];

fseek(stream,22,SEEK_SET);
i = fwrite(&bfr[22],1,1536,stream);
if (i != 1536)
{
	printf("Could not write the palette file\n");
}
fclose(stream);


stream = fopen("FDRAW.PAL","wb");
fwrite(&bfr[22],1,768,stream);
fclose(stream);


rgb[3] = 0;
rgb[1023] = 0;
stream = fopen("FD.PAL","wb");
fwrite(rgb,1,1024,stream);
fclose(stream);


return;
}
