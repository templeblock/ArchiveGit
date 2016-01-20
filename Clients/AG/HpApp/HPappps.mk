
HPappps.dll: dlldata.obj HPapp_p.obj HPapp_i.obj
	link /dll /out:HPappps.dll /def:HPappps.def /entry:DllMain dlldata.obj HPapp_p.obj HPapp_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0500 /DREGISTER_PROXY_DLL \
		$<
# _WIN32_WINNT=0x0500 is for Win2000, change it to 0x0400 for NT4 or Win95 with DCOM

clean:
	@del HPappps.dll
	@del HPappps.lib
	@del HPappps.exp
	@del dlldata.obj
	@del HPapp_p.obj
	@del HPapp_i.obj
