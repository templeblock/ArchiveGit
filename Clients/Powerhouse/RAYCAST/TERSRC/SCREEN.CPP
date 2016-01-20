#include <mem.h>
#include <malloc.h>

char *CreateBitmap(int w, int h)
{
        return (char *)malloc(w * h);
}

void ClearScreen(char clr, int w, int h, char *video)
{
    memset(video, clr, w*h);
}

void Display(char *src, int w, int h)
{
        char *video = (char *)0xA0000 + (320 - w)/2;
        for (short j=0; j<h; j++)
        {
            memcpy(video, src, w);
            video += 320;
            src += w;
        }
}


