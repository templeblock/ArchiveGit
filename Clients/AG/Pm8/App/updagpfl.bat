@echo off
CALL CHECKMAP.BAT
if errorlevel 1 goto :end

CALL GET.BAT agpfl.txt
attrib -r q:\install\install.pfl
makepfl32 agpfl.txt q:\install\install.pfl
attrib +r q:\install\install.pfl
rem more < q:\install\install.pfl

:end
@echo on

