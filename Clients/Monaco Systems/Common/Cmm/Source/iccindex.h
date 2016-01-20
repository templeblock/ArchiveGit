//generate icc index
#ifndef ICC_INDEX_H
#define ICC_INDEX_H

#define GRAYSCALE_INDEX (1)
#define RGB_INDEX		(2)
#define LAB_INDEX		(3)
#define XYZ_INDEX		(4)
#define CMY_INDEX		(5)
#define CMYK_INDEX		(6)

class Iccindex{

public:
Iccindex(unsigned char* table, long size, long dimen);
Iccindex(double* table, long size, long dimen);

Iccindex(long type, unsigned char* table, long size, long packsize);
Iccindex(long type, unsigned short* table, long size, long packsize);
Iccindex(long type, double* table, long size, long packsize);

~Iccindex(){;}
};


#endif