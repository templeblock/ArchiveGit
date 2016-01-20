@rem echo off
@mkdir Compressed
@for %%f in (*.ttf) do zcomp %%f Compressed\%%f
@ren Compressed\*.ttf *.ttz
