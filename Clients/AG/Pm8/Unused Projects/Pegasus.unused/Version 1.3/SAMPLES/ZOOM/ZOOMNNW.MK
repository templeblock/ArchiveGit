.after
 copy /b zoomnnw.exe ..\..\bin\*.*

 copy /b zoomnnw.sym ..\..\bin\*.*


project : d:\pdk\samples\zoom\zoomnnw.exe .SYMBOLIC

!include d:\pdk\samples\zoom\zoomnnw.mk1
