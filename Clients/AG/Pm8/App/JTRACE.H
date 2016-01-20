//#ifndef  JTRACE
#ifdef   JTRACE_ON
   #ifdef _DEBUG
      #pragma message ("Turning JTRACE on\n")
   #endif
   #define  JTRACE   TRACE
   #define  JTRACE0  TRACE0
   #define  JTRACE1  TRACE1
   #define  JTRACE2  TRACE2
   #define  JTRACE3  TRACE3
#else
   //#pragma message ("Turning JTRACE off\n")
   inline   void CDECL TraceBeGone(LPCSTR, ...){}
   #define  JTRACE				1 ? (void)0 :  ::TraceBeGone  // Disable tracing
   #define  JTRACE0           JTRACE
   #define  JTRACE1				JTRACE
   #define  JTRACE2				JTRACE
   #define  JTRACE3				JTRACE
#endif

//#endif   
