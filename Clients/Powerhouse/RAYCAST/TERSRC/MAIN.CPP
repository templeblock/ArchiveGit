#include <stdio.h>
#include <conio.h>
#include <graph.h>
#include "Mouse.hpp"

void CloseGraphix()
{
    if (!getch()) getch();
    _setvideomode(_TEXTC80);    
}

void OpenGraphix()
{
    _setvideomode(_MRES256COLOR);
}

extern void Terrain();
void main()
{
    /*
    int x, y;
    while (!kbhit())
    {
        MouseMove(&x, &y);
        printf("%d, %d\n", x, y);
    }
    return;
    */
              
    OpenGraphix();

    Terrain();
    
    CloseGraphix();        
}


