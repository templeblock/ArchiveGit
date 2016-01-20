".\Flex 2 SDK\bin\mxmlc.exe" -compiler.as3 -compiler.strict %1 %2 %3 %4 %5 %6 %7 %8 %9
@if errorlevel 0 move /y *.swf ..
