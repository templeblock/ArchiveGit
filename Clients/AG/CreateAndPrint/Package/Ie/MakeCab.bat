@echo off
set file1=..\..\AxCtp2\Release\AxCtp2.dll
set file2=..\..\..\ImageControl\Release\ImageControl.dll
set file3=AxCtp2.inf
set outfile=AxCtp2.cab
if exist %outfile% del %outfile%

if exist %file1% goto file1OK
echo Executable file '%file1%' does not exist
goto end
:file1OK

if exist %file2% goto file2OK
echo Executable file '%file2%' does not exist
goto end
:file2OK

call CabArc.exe -s 6144 n %outfile% %file1% %file2% %file3%
call "C:\My Projects\AgApplications\CodeSigning\Sign.bat" %outfile% "Create & Print plugin" "http://www.americangreetings.com/printables"

:end

rem rem Usage: CABARC [[options]] [command] [cabfile] [[filelist...]] [dest_dir]
rem 
rem Commands:
rem    L   List contents of cabinet (e.g. cabarc l test.cab)
rem    N   Create new cabinet (e.g. cabarc n test.cab *.c app.mak *.h)
rem    X   Extract file(s) from cabinet (e.g. cabarc x test.cab foo*.c)
rem 
rem Options:
rem   -c   Confirm files to be operated on
rem   -o   When extracting, overwrite without asking for confirmation
rem   -m   Set compression type [MSZIP or NONE], (default is MSZIP)
rem   -p   Preserve path names (absolute paths not allowed)
rem   -P   Strip specified prefix from files when added
rem   -r   Recurse into subdirectories when adding files (see -p also)
rem   -s   Reserve space in cabinet for signing (e.g. -s 6144 reserves 6K bytes)
rem   -i   Set cabinet set ID when creating cabinets (default is 0)
rem   --   Stop option parsing
rem 
rem Notes
rem -----
rem When creating a cabinet, the plus sign (+) may be used as a filename
rem to force a folder boundary; e.g. cabarc n test.cab *.c test.h + *.bmp
rem 
rem When extracting files to disk, the [dest_dir], if provided, must end in
rem a backslash; e.g. cabarc x test.cab bar*.cpp *.h d:\test\
rem 
rem The -P (strip prefix) option can be used to strip out path information
rem e.g. cabarc -r -p -P myproj\ a test.cab myproj\balloon\*.*
rem The -P option can be used multiple times to strip out multiple paths
rem 

rem Usage:  SignCode [options] FileName
rem  -spc  [file]      Spc file containing software publishing certificates
rem  -v    [pvkFile]   Pvk file name containing the private key
rem  -t    [URL]       TimeStamp server's http address
rem
rem Options:
rem  -k    [KeyName]   Key container name
rem  -n    [name]      Text name representing content of the file to be signed
rem  -i    [info]      Place to get more info on content (usually a URL)
rem  -p    [provider]  Name of the cryptographic provider on the system
rem  -y    [type]      Cryptographic provider type to use
rem  -ky   [keytype]   Key type
rem                       [signature or exchange or [integer]]
rem  -$    [authority] Signing authority of the certificate
rem                       [individual or commercial]
rem                       Default to using certificate's highest capability
rem  -a    [algorithm] Hashing algorithm for signing
rem                       [md5 or sha1]. Default to md5
rem  -tr   [number]    The # of timestamp trial until succeeds. Default to 1
rem  -tw   [number]    The # of seconds delay between each timestamp. Default to 0
rem  -j    [dllName]   Name of the dll that provides attributes of the signature
rem  -jp   [param]     Parameter to be passed to the dll
rem  -c    [file]      X509 file containing encoded software publishing certificate
rem  -s    [store]     Cert store containing certs. Default to my store
rem  -r    [location]  Location of the cert store in the registry
rem                       [localMachine or currentUser]. Default to currentUser
rem  -sp   [policy]    Add all the certificates in the chain or add until one cert
rem                    in the chain is from the spc store.
rem                       [chain or spcstore]. Default to spcstore
rem  -cn   [name]      The common name of the certificate
rem  -x                Do not sign the file.  Only Timestamp the file
rem 
rem Note:   To sign with a SPC file, the required options are -spc and -v if
rem         your private key is in a PVK file. If your private key is in a
rem         registry key container, then -spc and -k are the required options.
