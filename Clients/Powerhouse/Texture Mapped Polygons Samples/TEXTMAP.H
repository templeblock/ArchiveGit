typedef struct
    {
     int x,y;
     int sx, sy;	/* Source coordinates from the texture image */
    } tpoint;

extern unsigned char far *textureimage;

extern void tmapline (int x1, int tmapx1, int tmapy1, int x2, int tmapx2,
		      int tmapy2, int y);

extern void texturedpoly (tpoint *vertexlist, int numvertex);
