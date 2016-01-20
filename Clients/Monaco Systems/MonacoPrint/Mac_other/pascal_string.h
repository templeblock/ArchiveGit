// Implement pascal equivalents of c string processing
// pascal_string.h
// Copied from colorfix.h
//
// James

void add_string(unsigned char *s1, unsigned char *s2);
// Copy a pascal string.  copy s2 into s1
void copy_str(unsigned char *s1, unsigned char *s2);
// Cat a pascal string
void cat_str(unsigned char *s1, unsigned char *s2);
// Remove everything but numbers from a string
void clean_text(unsigned char *s1);
// Compare strings s2 and s1
int cmp_str(unsigned char *s1, unsigned char *s2);
// get a floating point number from a pascal string
void StringToDouble(unsigned char *s1,double *d);
// convert floating point number to string
void DoubleToString(double d,unsigned char *s1,int perc);
