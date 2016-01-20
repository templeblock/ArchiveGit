@echo off
del /Q ref\* < yes.txt
del /Q dst\* < yes.txt
cd ctllib
del /Q obj\*.obj < ..\yes.txt
cd ..
cd superblt
del /Q obj\*.obj < ..\yes.txt
cd ..
cd imgutil
del /Q obj\*.obj < ..\yes.txt
del /Q dllobj\*.obj < ..\yes.txt
cd ..
cd apputil
del /Q obj\*.obj < ..\yes.txt
del /Q dllobj\*.obj < ..\yes.txt
cd ..
cd bitmapio
del /Q obj\*.obj < ..\yes.txt
del /Q dobj\*.obj < ..\yes.txt
cd ..
cd browser
del /Q obj\*.obj < ..\yes.txt
cd ..
cd pp\src
del /Q obj\*.obj < ..\..\yes.txt
cd ..
del /Q res\*.res < ..\yes.txt
cd ..


