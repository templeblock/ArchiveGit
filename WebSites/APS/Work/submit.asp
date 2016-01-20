<%
' This is used to control whether or not we empty the guestbook at
' the first hit after midnight.  We do it just to keep the list
' short.  You'll probably want to set this to False
Const bDeleteEntries = True

' Allows us to easily clear the guestbook if we notice someone is
' getting rude!  All you need to do is pass it ?force=anything
' Possibly something else you might not want. To disable it comment
' out the Request.QueryString line and uncomment the "" one.
Dim bForce
bForce = Request.QueryString("force")
'bForce = ""

' Now that we're done implementing features you probably won't want,
' let's get to the actual script...
Dim strFile ' String variable to store the path / file we write to
' I use MapPath here to make the script somewhat location independent.
' If I specified the physical path, you'd need to edit it to run this
' on your own server.  This way it should work fine as long as it's in
' the same directory as the guestbook file.  The include line also
' needs to be changed if you change this!  This file needs to exist
' BEFORE you run this script!
strFile = Server.MapPath("guestbook.txt")

' If the script doesn't have anything posted to it we display the form
' otherwise we process the input and append it to the guestbook file.
If Request.Form.Count = 0 Then
	' Display the entry form.
	%>

<head>
<meta name="GENERATOR" content="Microsoft FrontPage 4.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
</head>

<h3>Sign Our Guestbook:</h3>
<form action="guestbook.asp" method="post">
	<table>
		<tr>
			<td align="right"><b>Name:</b></td>
			<td><input type="text" name="name" size="15"></td>
		</tr>
		<tr>
			<td align="right"><b>Comment:</b></td>
			<td><input type="text" name="comment" size="35"></td>
		</tr>
	</table>
	<input type="submit" value="Sign Guestbook!">
</form>
<br>
<h3>Today's Comments:</h3>
	<!-- Instead of doing this in script, I simply include the guestbook file as is -->
	<!--#INCLUDE FILE="guestbook.txt"-->
	<%
Else
	' Log the entry to the guestbook file
	Dim objFSO  'FileSystemObject Variable
	Dim objFile 'File Object Variable
	
	' Create an instance of the FileSystemObject
	Set objFSO = Server.CreateObject("Scripting.FileSystemObject")
	' Open the TextFile (FileName, ForAppending, AllowCreation)
	Set objFile = objFSO.OpenTextFile(strFile, 8, True)

	' Log the results: I simply bold the name and do a <BR>.
	objFile.Write "<B>"
	objFile.Write Server.HTMLEncode(Request.Form("name"))
	objFile.Write ":</B> "
	objFile.Write Server.HTMLEncode(Request.Form("comment"))
	objFile.Write "<BR>"
	objFile.WriteLine ""
	
	' Close the file and dispose of our objects
	objFile.Close
	Set objFile = Nothing
	Set objFSO = Nothing

	' Tell people we've written their info
%>
<h3>Your comments have been written to the file!</h3>
<a href="guestbook.asp">Back to the guestbook</a>
<%
End If

' We do this to delete the file every day to keep it managable.
' If you were doing this for real you probably wouldn't want to
' do this so we have defined a const named bDeleteEntries at the
' top of the script that you can set to False to prevent this
' section from running.  You could also delete this whole
' If Then....End If block if you'd like.  Just be sure to leave
' the script delimiter at the bottom!
If bDeleteEntries Then
	Set objFSO = Server.CreateObject("Scripting.FileSystemObject")
	Set objFile = objFSO.GetFile(strFile)
	If DateDiff("d", objFile.DateLastModified, Date()) <> 0 Or bForce <> "" Then
		Set objFile = Nothing		
		' Can't use delete because we need the file to exist for
		' the include the next time the script is run!
		'objFile.Delete
		
		' Create a file overwriting old one.
		Set objFile = objFSO.CreateTextFile(strFile, True)

		' The include barks if the file's empty!
		objFile.Write "<B>John:</B> "
		objFile.WriteLine "I hope you like our guestbook!<BR>"
		objFile.Close
	End If
	Set objFile = Nothing
	Set objFSO = Nothing
End If
%>
