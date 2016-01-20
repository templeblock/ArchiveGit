#include <stdlib.h>
int main(int argc, char *argv[])
   {
      int nRetVal = 0;

      if(argc > 1)
         {
            nRetVal = atoi(argv[1]);
         }
      return nRetVal;
   }
