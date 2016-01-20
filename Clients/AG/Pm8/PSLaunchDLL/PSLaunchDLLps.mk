
PSLaunchDLLps.dll: dlldata.obj PSLaunchDLL_p.obj PSLaunchDLL_i.obj
	link /dll /out:PSLaunchDLLps.dll /def:PSLaunchDLLps.def /entry:DllMain dlldata.obj PSLaunchDLL_p.obj PSLaunchDLL_i.obj kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib 

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL $<

clean:
	@del PSLaunchDLLps.dll
	@del PSLaunchDLLps.lib
	@del PSLaunchDLLps.exp
	@del dlldata.obj
	@del PSLaunchDLL_p.obj
	@del PSLaunchDLL_i.obj
