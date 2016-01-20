if not exist q:\install.exe goto :badmap
if not exist q:\readme.doc goto :badmap
if not exist q:\autorun.inf goto :badmap
if not exist q:\pm??50.id goto :badmap
goto :goodmap
:badmap
echo Q: is not mapped correctly!
goto :end
:goodmap

get pmwpfl16.txt
attrib -r q:\install\pmw16.pfl
makepfl pmwpfl16.txt q:\install\pmw16.pfl
attrib +r q:\install\pmw16.pfl
rem more < q:\install\pmw16.pfl

:end


