if not exist q:\install.exe goto :badmap
REM if not exist q:\readme.doc goto :badmap
if not exist q:\autorun.inf goto :badmap
goto :success

:badmap
@echo Network drive Q: is NOT mapped correctly or missing files!
seterror 1
goto :end

:success
seterror 0
@echo Verified files on Network drive Q:

:end
