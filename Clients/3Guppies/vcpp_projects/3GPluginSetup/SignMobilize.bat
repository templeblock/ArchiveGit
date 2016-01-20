@echo off
cd ".\CodeSigning"
call "Sign_WithP12orPfx.bat" "C:\My Projects\3gCheckouts\vcpp_projects\3GPluginSetup\Release\SetupMobilize.msi" "3Guppies Mobilizer" "http://www.3guppies.com"
