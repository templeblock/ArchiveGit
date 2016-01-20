
README for HOTSPOT EDITOR
Wayne Radinsky, Microsoft Developer Relations, 4 October 1993

The hotspot editor was written using Visual C/C++.  The makefile
is AVIHED.MAK and is backward compatible with previous versions
of microsoft C/C++, however it uses complete pathnames and you
may need to go in and edit the pathname to point to your build
directory in order to run nmake successfully.

I left some commented-out code in dlg.c for checking if everything
entered is OK.  I did this so that if you need a template for code
that does such checking, the code would be right there.  AVIHED
doesn't need it anymore.

Building in Visual C/C++ causes 4 warnings which you can safely
ignore.
