@rem echo off
@mkdir Decompressed
@for %%f in (*.ttz) do zcomp -d %%f Decompressed\%%f
@ren Decompressed\*.ttz *.ttf
