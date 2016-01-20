@echo off
rem	Old MASM command line
rem	MASM /Ml /B63 /D_ML ackasm,ackasm;
rem	Needed more characters is .OBJ label size using /H64
rem	so had to change to ML
rem	For listing output file with timing info:
rem	ML /D_ML /H64 /c /Sc /Sl80 /Flackasm.lis ackasm.asm
ML /D_ML /H64 /c ackasm.asm
