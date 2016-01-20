#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include "long_fix.h"

void main ( int argc, char *argv[] )
{
    if (argc != 3)
    {
        cout << "USAGE: long_fix [first_number] [second_number]\n";
        exit(0);
    }

    long_fixed first = atof(argv[1]);
    long_fixed second = atof(argv[2]);

    if (second == 0)
    {
        cout << "**** ERROR **** the second number can not be zero!\n";
        exit(0);
    }

    cout << " -first = " << (-first) << "\n\n";
    cout << " first + second = " << (first + second) << '\n';
    cout << " first - second = " << (first - second) << '\n';
    cout << " first * second = " << (first * second) << '\n';
    cout << " first / second = " << (first / second) << "\n\n";
    cout << " abs(first) = " << abs(first) << '\n';
    cout << " round(first) = " << round(first) << '\n';
    cout << " ceil(first) = " << ceil(first) << '\n';
    cout << " floor(first) = " << floor(first) << "\n\n";
    cout << " first << round(second) = " << (first << (int)round(second)) << '\n';
    cout << " first >> round(second) = " << (first >> (int)round(second)) << "\n\n";
    cout << " first <  second = " << ((first < second) ? "TRUE" : "FALSE") << '\n';
    cout << " first <= second = " << ((first <= second) ? "TRUE" : "FALSE") << '\n';
    cout << " first >  second = " << ((first > second) ? "TRUE" : "FALSE") << '\n';
    cout << " first >= second = " << ((first >= second) ? "TRUE" : "FALSE") << '\n';
    cout << " first == second = " << ((first == second) ? "TRUE" : "FALSE") << '\n';
    cout << " first != second = " << ((first != second) ? "TRUE" : "FALSE") << "\n\n";
    cout << " first += second = " << (first += second) << '\n';
    cout << " first -= second = " << (first -= second) << '\n';
    cout << " first *= second = " << (first *= second) << '\n';
    cout << " first /= second = " << (first /= second) << "\n\n";
    cout << " first <<= round(second) = " << (first <<= (int)round(second)) << '\n';
    cout << " first >>= round(second) = " << (first >>= (int)round(second)) << '\n';
}

