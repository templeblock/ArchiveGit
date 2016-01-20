path > ppath.bat

:make32

set MSDevDir=v:\msdev

set PATH=%MSDevDir%\BIN
set INCLUDE=%MSDevDir%\INCLUDE;%MSDevDir%\MFC\INCLUDE
set LIB=%MSDevDir%\LIB;%MSDevDir%\MFC\LIB

cd install
nmake /Finst32.mak CFG="Inst32 - Win32 Release"
cd ..

nmake /Fmsfree32.mak CFG="msfree32 - Win32 Release"
nmake /Fmsreg32.mak CFG="msreg32 - Win32 Release"
nmake /Fmsregr32.mak CFG="Msregr32 - Win32 Release"
nmake /Fmsregx32.mak CFG="msregx32 - Win32 Release"
nmake /Fmsrun32.mak CFG="msrun32 - Win32 Release"
nmake /Ftest32.mak CFG="Test32 - Win32 Release"
nmake /Funreg32.mak CFG="Unreg32 - Win32 Release"

cd msfree
nmake /Fmsfree32.mak CFG="msfree32 - Win32 Debug"
nmake /Fmsfree32.mak CFG="msfree32 - Win32 Release"
cd ..

:make16

set MSDevDir=m:\msvc152

set PATH=%MSDevDir%\BIN
set INCLUDE=%MSDevDir%\INCLUDE;%MSDevDir%\MFC\INCLUDE;%MSDevDir%\MFC\SRC
set LIB=%MSDevDir%\LIB;%MSDevDir%\MFC\LIB

cd install
nmake /Finst16.mak DEBUG=0
cd ..

nmake /Fmsfree16.mak DEBUG=0
nmake /Fmsreg16.mak DEBUG=0
nmake /Fmsregr16.mak DEBUG=0
nmake /Fmsregx16.mak DEBUG=0
nmake /Fmsrun16.mak DEBUG=0
nmake /Ftest16.mak DEBUG=0
nmake /Funreg16.mak DEBUG=0

cd msfree
nmake /Fmsfree16.mak DEBUG=0
cd ..

:end

ppath

