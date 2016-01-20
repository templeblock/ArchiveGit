rem -DOEM
set cl=-Ires -Iinc -DOEM
cd browser
nmake OEM
cd ..
cd res
nmake OEM
cd ..
set cl=
copy pp.exe ppoem.exe
rc -31 -lim32 res\ppoem.res ppoem.exe

goto DONE

rem -DOEM -DOEM_NOSCAN
set cl=-Ires -Iinc -DOEM -DOEM_NOSCAN
cd browser
nmake OEM
cd ..
cd res
nmake OEM
cd ..
set cl=
copy pp.exe ppoemxs.exe
rc -31 -lim32 res\ppoem.res ppoemxs.exe

rem -DOEM -DOEM_NOTWAIN
set cl=-Ires -Iinc -DOEM -DOEM_NOTWAIN
cd browser
nmake OEM
cd ..
cd res
nmake OEM
cd ..
set cl=
copy pp.exe ppoemxt.exe
rc -31 -lim32 res\ppoem.res ppoemxt.exe

:DONE

