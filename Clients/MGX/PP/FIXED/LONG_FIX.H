#ifndef LONG_FIXED__H_
#define LONG_FIXED__H_

#include <iostream.h>

class long_fixed
{
  private:
    unsigned long num[2];
    long_fixed ( const long n0, const long n1 ) { num[0] = n0; num[1] = n1; }
    friend long trunc ( long_fixed num );

  public:
   ~long_fixed () { }

    long_fixed () {num[0] = num[1] = 0L;}

    long_fixed ( const long_fixed &in) {num[0] = in.num[0]; num[1] = in.num[1];}

    long_fixed ( const long double in);
    long_fixed ( const double in);
    long_fixed ( const long in);
    long_fixed ( const int in);

    long double value(void);

    long_fixed operator- ( void ) const;

    long_fixed operator+ ( const long_fixed &sec ) const;
    long_fixed operator- ( const long_fixed &sec ) const;
    long_fixed operator* ( const long_fixed &sec ) const;
    long_fixed operator/ ( const long_fixed &sec ) const;

    long_fixed operator<< ( const int &shift ) const;
    long_fixed operator>> ( const int &shift ) const;

    long_fixed &operator=  ( const long_fixed &sec );

    long_fixed &operator+= ( const long_fixed &sec );
    long_fixed &operator-= ( const long_fixed &sec );
    long_fixed &operator*= ( const long_fixed &sec );
    long_fixed &operator/= ( const long_fixed &sec );

    long_fixed &operator<<= ( const int &shift );
    long_fixed &operator>>= ( const int &shift );

    int  operator<  ( const long_fixed &sec ) const;
    int  operator<= ( const long_fixed &sec ) const;
    int  operator>  ( const long_fixed &sec ) const;
    int  operator>= ( const long_fixed &sec ) const;
    int  operator== ( const long_fixed &sec ) const;
    int  operator!= ( const long_fixed &sec ) const;

    friend long_fixed operator* ( long double fst, long_fixed sec);
    friend long_fixed operator/ ( long double fst, long_fixed sec);
    friend long_fixed operator+ ( long double fst, long_fixed sec);
    friend long_fixed operator- ( long double fst, long_fixed sec);

    friend long_fixed abs   ( long_fixed num );

    friend long round ( long_fixed num );
    friend long ceil  ( long_fixed num );
    friend long floor ( long_fixed num );

    friend ostream& operator<< (ostream& os, long_fixed& sec);
};

#endif
