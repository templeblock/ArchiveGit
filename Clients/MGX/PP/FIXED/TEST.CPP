#include <iostream.h>
#include <timer.h>
#include "long_fix.h"

class dbl_class
{
  private:
    double num;

  public:
    dbl_class () { num = 0.0; }
    dbl_class (double n) { num = n; }
    dbl_class &operator+= ( const dbl_class &sec );
    dbl_class &operator-= ( const dbl_class &sec );
    int  operator<  ( const dbl_class &sec ) const;
};

dbl_class &dbl_class::operator+= ( const dbl_class &sec )
{
    num += sec.num;
    return *this;
}

dbl_class &dbl_class::operator-= ( const dbl_class &sec )
{
   num -= sec.num;
   return *this;
}

int  dbl_class::operator<  ( const dbl_class &sec ) const
{
    return (num < sec.num);
}

long_fixed r_inc = 1.23456789;
dbl_class  d_inc = 1.23456789;

const long_fixed r_is  = 50000.0;
const dbl_class  d_is  = 50000.0;

void main (void)
{
    Timer tm;
    long_fixed ri, r_ans;
    dbl_class  di, d_ans;

    cout << "Comparison with decimal positions\n";
    tm.start();
    tm.reset();
    for (di = 0.0; di < d_is; di += d_inc)
        d_ans -= di;
    tm.stop();
    cout << "dbl_class time  = " << tm.time() << '\n';

    tm.start();
    tm.reset();
    for (ri = 0.0; ri < r_is; ri += r_inc)
        r_ans -= ri;
    tm.stop();
    cout << "long_fixed time = " << tm.time() << '\n';

    r_inc = 1.0;
    d_inc = 1.0;

    cout << "\nComparison without decimal positions\n";
    tm.start();
    tm.reset();
    for (di = 0.0; di < d_is; di += d_inc)
        d_ans -= di;
    tm.stop();
    cout << "dbl_class time  = " << tm.time() << '\n';

    tm.start();
    tm.reset();
    for (ri = 0.0; ri < r_is; ri += r_inc)
        r_ans -= ri;
    tm.stop();
    cout << "long_fixed time = " << tm.time() << '\n';
}
