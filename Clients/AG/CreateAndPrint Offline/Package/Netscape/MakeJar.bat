@echo off
set PluginFile=..\..\Source\NpCtp\Release\NpCtp.dll
set ClassFile=..\..\Source\NpCtp\NpCtp.class
set InstallScript=install.js
set InstallScriptInJar=install.js
set JarFile=NpCtp.jar
if exist %JarFile% del %JarFile%

if exist %PluginFile% goto FileOK
echo Executable file '%PluginFile%' does not exist
goto end

:FileOK
set folder=signdir
set subfolder=%folder%\PluginDir
if exist %folder%\*.* rmdir/s/q %folder%
md %folder%
md %subfolder%

echo Copying %PluginFile% to %subfolder%...
copy/v %PluginFile% %subfolder%
echo Copying %InstallScript% to %folder%\%InstallScriptInJar%...
copy/v %InstallScript% %folder%\%InstallScriptInJar%
echo Copying %ClassFile% to %subfolder%...
copy/v %ClassFile% %subfolder%
call SignTool -d"." -c9 -i%InstallScriptInJar% -k"American Greetings, Inc.'s VeriSign, Inc. ID" -Z%JarFile% %folder%

:end

rem Usage:  signtool [options] directory-tree
rem 
rem     -b"basename"                basename of .sf, .rsa files for signing
rem     -c#                         Compression level, 0-9, 0=none
rem     -d"certificate directory"   contains cert*.db and key*.db
rem     -e".ext"                    sign only files with this extension
rem     -f"filename"                read commands from file
rem     -G"nickname"                create object-signing cert with this nickname
rem     -i"installer script"        assign installer javascript
rem     -j"javascript directory"    sign javascript files in this subtree
rem     -J                          directory contains HTML files. Javascript will
rem                                 be extracted and signed.
rem     -k"cert nickname"           sign with this certificate
rem     --leavearc                  do not delete .arc directories created
rem                                 by -J option
rem     -m"metafile"                include custom meta-information
rem     --norecurse                 do not operate on subdirectories
rem     -o                          optimize - omit optional headers
rem     --outfile "filename"        redirect output to file
rem     -p"password"                for password on command line (insecure)
rem     -s keysize                  keysize in bits of generated cert
rem     -t token                    name of token on which to generate cert
rem     --verbosity #               Set amount of debugging information to generate.
rem 
rem                                 Lower number means less output, 0 is default.
rem     -x"name"                    directory or filename to exclude
rem     -z                          omit signing time from signature
rem     -Z"jarfile"                 create JAR file with the given name.
rem                                 (Default compression level is 6.)
rem 
rem signtool -l
rem   lists the signing certificates in your database
rem 
rem signtool -L
rem   lists all certificates in your database, marks object-signing certificates
rem 
rem signtool -M
rem   lists the PKCS #11 modules available to signtool
rem 
rem signtool -v file.jar
rem   show the contents of the specified jar file
rem 
rem signtool -w file.jar
rem   if valid, tries to tell you who signed the jar file
rem 
rem For more details, visit
rem   http://developer.netscape.com/library/documentation/signedobj/signtool/

rem After importing the MyCredentials.p12 certificate into Netscape Communicator,
rem I copied the Certificate DB files (key3.db and cert7.db)
rem from "C:\Program Files\Netscape\Users\jmccurdy"
rem so this folder could contain the buid entire environment

