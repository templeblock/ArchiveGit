<%
'******** CONSTANTS ********
Const LINKS_FILE = "links.htm"

'**********************************************
Function GetLinksFileAsString()
	Dim objFSO, objInputFile
	Dim strTemp ' as String

	Set objFSO = Server.CreateObject("Scripting.FileSystemObject")
	Set objInputFile = objFSO.OpenTextFile(LINKS_FILE)

	strTemp = objInputFile.ReadAll

	objInputFile.Close
	Set objInputFile = Nothing
	Set objFSO = Nothing
	
	GetLinksFileAsString = strTemp
End Function

'**********************************************
Function GetLinkCount(strFullLinksFileText)
	Dim I ' as Integer
	I = 1
	Do While Instr(1, strFullLinksFileText, "<!-- Begin Link " & I & " -->") <> 0
		I = I + 1
	Loop
	GetLinkCount = I - 1
End Function

'**********************************************
Function GetHTMLBetweenStrings(strFullLinksFileText, strStart, strEnd)
	Dim iStart, iEnd
	
	iStart = Instr(1, strFullLinksFileText, strStart) + Len(strStart)
	iEnd = Instr(iStart, strFullLinksFileText, strEnd)
	
	GetHTMLBetweenStrings = Mid(strFullLinksFileText, iStart, iEnd - iStart)
End Function

'**********************************************
Function GetLinkHTML(iLinkNumber, strFullLinksFileText)
	GetLinkHTML = GetHTMLBetweenStrings(strFullLinksFileText, "<!-- Begin Link " & iLinkNumber & " -->", "<!-- End Link " & iLinkNumber & " -->")
End Function

'**********************************************
Function GetCommentHTML(iLinkNumber, strFullLinksFileText)
	GetCommentHTML = GetHTMLBetweenStrings(strFullLinksFileText, "<!-- Begin Comment " & iLinkNumber & " -->", "<!-- End Comment " & iLinkNumber & " -->")
End Function

'**********************************************
Function GetHTMLBetweenLinkAndComment(iLinkNumber, strFullLinksFileText)
	GetHTMLBetweenLinkAndComment = GetHTMLBetweenStrings(strFullLinksFileText, "<!-- End Link " & iLinkNumber & " -->", "<!-- Begin Comment " & iLinkNumber & " -->")
End Function

'**********************************************
Function GetHTMLBetweenLinks(iPreviousLinkNumber, strFullLinksFileText)
	GetHTMLBetweenLinks = GetHTMLBetweenStrings(strFullLinksFileText, "<!-- End Comment " & iPreviousLinkNumber & " -->", "<!-- Begin Link " & iPreviousLinkNumber + 1 & " -->")
End Function

'**********************************************
Function GetHeaderHTML(strFullLinksFileText)
	Dim strTemp
	Dim strEnd
	Dim iStart, iEnd
	strTemp = strFullLinksFileText
	
	strEnd = "<!-- Begin Link 1 -->"
	
	iStart = 1
	iEnd = Instr(iStart, strFullLinksFileText, strEnd)
	
	strTemp = Mid(strTemp, iStart, iEnd - iStart)

	GetHeaderHTML = strTemp
End Function

'**********************************************
Function GetFooterHTML(strFullLinksFileText, iLinkCount)
	Dim strTemp
	Dim strStart
	Dim iStart, iEnd
	strTemp = strFullLinksFileText
	
	strStart = "<!-- End Comment " & iLinkCount & " -->"
	
	iStart = Instr(1, strFullLinksFileText, strStart) + Len(strStart)
	iEnd = Len(strFullLinksFileText) + 1

	strTemp = Mid(strTemp, iStart, iEnd - iStart)

	GetFooterHTML = strTemp
End Function

'**********************************************
Sub WriteLinksFileToDisk(strFullLinksFileText)
	Dim objFSO, objLinksFile
	Dim strTemp ' as String

	Set objFSO = Server.CreateObject("Scripting.FileSystemObject")
	Set objLinksFile = objFSO.CreateTextFile(LINKS_FILE, True)

	objLinksFile.Write strFullLinksFileText

	objLinksFile.Close
	Set objLinksFile = Nothing
	Set objFSO = Nothing
End Sub


'******** BEGIN RUNTIME CODE ********
Dim strInput ' as String - will contain the whole links html page!
Dim strOutput ' as String - will contain modified version!
Dim strTemp ' as String - used for string building etc.
Dim iLinkCount ' as Integer - number of links we're dealing with
Dim iLink ' as Integer - stores what link # we're performing the operation on
Dim I ' as Integer - standard loop controller

' Get the current links page and place it into strInput
strInput = GetLinksFileAsString()

' Get the current number of links on the links page
iLinkCount = GetLinkCount(strInput)

iLink = Request.QueryString("link")
If IsNumeric(iLink) Then
	iLink = CInt(iLink)
Else
	iLink = 0
End If

' Debugging Lines!
Response.Write "="
Response.Write strInput
Response.Write iLinkCount
Response.Write GetHeaderHTML(strInput)
Response.Write GetLinkHTML(1, strInput)
Response.Write GetCommentHTML(1, strInput)
Response.Write GetFooterHTML(strInput, iLinkCount)
Response.Write "="

' Figure out what we're supposed to do with the file now that we have it!
' Options implemented are add, edit, delete, and the default case "view"
Select Case Request.QueryString("action")
	Case "add" ' Add a new link - This could be done in less code using the Replace function
		' If we've already entered new link, write links to links file o/w show add form
		If Request.Form("write") = "yes" Then
			' Start the new string with the everthing up to the first link
			strOutput = GetHeaderHTML(strInput)
		
			' Loop through existing links placing them into the new string
			For I = 1 to iLinkCount
				strOutput = strOutput & "<!-- Begin Link " & I & " -->" & GetLinkHTML(I, strInput) & "<!-- End Link " & I & " -->"
				strOutput = strOutput & GetHTMLBetweenLinkAndComment(I, strInput)
				strOutput = strOutput & "<!-- Begin Comment " & I & " -->" & GetCommentHTML(I, strInput) & "<!-- End Comment " & I & " -->"
				If I <> iLinkCount Then
					strOutput = strOutput & GetHTMLBetweenLinks(I, strInput)
				Else
					strOutput = strOutput & GetHTMLBetweenLinks(I - 1, strInput)
				End If
			Next 'I
		
			' Add the new link to the end of the new string
			strOutput = strOutput & "<!-- Begin Link " & iLinkCount + 1 & " -->" & Request.Form("link") & "<!-- End Link " & iLinkCount + 1 & " -->"
			strOutput = strOutput & GetHTMLBetweenLinkAndComment(iLinkCount, strInput)
			strOutput = strOutput & "<!-- Begin Comment " & iLinkCount + 1 & " -->" & Request.Form("comment") & "<!-- End Comment " & iLinkCount + 1 & " -->"
		
			' Tack on the rest of the html from the page
			strOutput = strOutput & GetFooterHTML(strInput, iLinkCount)
		
			' Save the new links file to the disk
			WriteLinksFileToDisk(strOutput)				

			' Show options for navigation
			Response.Write Request.Form("link") & " <B>has been added!</B><BR>" & vbCrLf
			Response.Write "<BR>" & vbCrLf
		Else
%>
<FORM ACTION="links.asp?action=add" METHOD="post">
<input type="hidden" name="write" value="yes">

<head>
<meta name="GENERATOR" content="Microsoft FrontPage 4.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
</head>

<table border="0" cellspacing="0" cellpadding="0">
	<tr>
		<td align="right"><b>Link:</b></td>
		<td><input type="text" size="80" name="link" value="<%= Server.HTMLEncode("<A HREF=""http://www.asp101.com""><IMG SRC=""../images/logochalk.gif""></A>") %>"></td>
	</tr>
	<tr>
		<td align="right"><b>Comment:</b></td>
		<td><input type="text" size="80" name="comment" value="The best Active Server Pages site on the net!"></td>
	</tr>
</table>
<input type="reset" value="Reset Form"><input type="submit" value="Add Link"></FORM>
<%
		End If

		' Show options for navigation
		Response.Write "<A HREF=""./links.asp"">Back to Link List</A><BR>" & vbCrLf
	Case "edit" ' Edit an existing link
		' If we've already made changes then write links to links file o/w show change form
		If Request.Form("write") = "yes" Then
			' Start the new string with the everthing up to the first link
			strOutput = GetHeaderHTML(strInput)
		
			' Loop through existing links placing them into the new string except for link to be changed
			For I = 1 to iLinkCount
				If I = iLink Then
					strOutput = strOutput & "<!-- Begin Link " & I & " -->" & Request.Form("link") & "<!-- End Link " & I & " -->"
					strOutput = strOutput & GetHTMLBetweenLinkAndComment(I, strInput)
					strOutput = strOutput & "<!-- Begin Comment " & I & " -->" & Request.Form("comment") & "<!-- End Comment " & I & " -->"
				Else
					strOutput = strOutput & "<!-- Begin Link " & I & " -->" & GetLinkHTML(I, strInput) & "<!-- End Link " & I & " -->"
					strOutput = strOutput & GetHTMLBetweenLinkAndComment(I, strInput)
					strOutput = strOutput & "<!-- Begin Comment " & I & " -->" & GetCommentHTML(I, strInput) & "<!-- End Comment " & I & " -->"
				End If
				If I <> iLinkCount Then
					strOutput = strOutput & GetHTMLBetweenLinks(I, strInput)
				End If
			Next 'I
		
			' Tack on the rest of the html from the page
			strOutput = strOutput & GetFooterHTML(strInput, iLinkCount)
		
			' Save the new links file to the disk
			WriteLinksFileToDisk(strOutput)				

			Response.Write Request.Form("link") & " <B>has been updated!</B><BR>" & vbCrLf
			Response.Write "<BR>" & vbCrLf
		Else
			%>
<form action="links.asp?action=edit&amp;link=<%= iLink %>" method="post">
	<input type="hidden" name="write" value="yes">
	<table border="0" cellspacing="0" cellpadding="0">
		<tr>
			<td align="right"><b>Link:</b></td>
			<td><input type="text" size="80" name="link" value="<%= Server.HTMLEncode(GetLinkHTML(iLink, strInput)) %>"></td>
		</tr>
		<tr>
			<td align="right"><b>Comment:</b></td>
			<td><input type="text" size="80" name="comment" value="<%= Server.HTMLEncode(GetCommentHTML(iLink, strInput)) %>"></td>
		</tr>
	</table>
	<input type="reset" value="Reset Form"><input type="submit" value="Update Link">
</form>
<p>
			<%
		Else
			' Start the new string with the everthing up to the first link
			strOutput = GetHeaderHTML(strInput)
		
			' Loop through existing links placing them into the new string except for link to be deleted
			' Note we also need to decrement the ids of the links after the one removed!
			For I = 1 to iLink - 1
				strOutput = strOutput & "<!-- Begin Link " & I & " -->" & GetLinkHTML(I, strInput) & "<!-- End Link " & I & " -->"
				strOutput = strOutput & GetHTMLBetweenLinkAndComment(I, strInput)
				strOutput = strOutput & "<!-- Begin Comment " & I & " -->" & GetCommentHTML(I, strInput) & "<!-- End Comment " & I & " -->"
				If I <> iLinkCount - 1 Then
					strOutput = strOutput & GetHTMLBetweenLinks(I, strInput)
				End If
			Next 'I
			' Notice we never hit iLink!
			For I = iLink + 1 to iLinkCount
				strOutput = strOutput & "<!-- Begin Link " & I - 1 & " -->" & GetLinkHTML(I, strInput) & "<!-- End Link " & I - 1 & " -->"
				strOutput = strOutput & GetHTMLBetweenLinkAndComment(I, strInput)
				strOutput = strOutput & "<!-- Begin Comment " & I - 1 & " -->" & GetCommentHTML(I, strInput) & "<!-- End Comment " & I - 1 & " -->"
				If I <> iLinkCount Then
					strOutput = strOutput & GetHTMLBetweenLinks(I, strInput)
				End If
			Next 'I
		
			' Tack on the rest of the html from the page
			strOutput = strOutput & GetFooterHTML(strInput, iLinkCount)
		
			' Save the new links file to the disk
			WriteLinksFileToDisk(strOutput)				

			' Show options for navigation
			Response.Write GetLinkHTML(iLink, strInput) & " <B>has been deleted!</B><BR>" & vbCrLf
			Response.Write "<BR>" & vbCrLf
		End If
		
		Response.Write "<A HREF=""./links.asp"">Back to Link List</A><BR>" & vbCrLf
	Case "sort" ' Sort existing links (Yeah Right!?!)
		' We'll leave this for those experts out ther who feel like tackling it!
		' Hint: add a column to the view table where the user can enter
		' integers representing the order they want to put them in.  You could
		' also just force an alphabetic sort if you exclusively use text links,
		' but like I said... this one is for ya'll to solve!  (We're just too
		' tired of looking at this code at this point!)
	Case Else ' Default - "view"
		%>
</p>
<table border="0">
	<tr>
		<td>
			<table border="1">
				<tr>
					<td><b>Current Links</b></td>
					<td><b>Edit</b></td>
					<td><b>Delete</b></td>
				</tr>
				<% For I = 1 to iLinkCount %>
				<tr>
					<td><%= GetLinkHTML(I, strInput) %></td>
					<td><a href="links.asp?action=edit&amp;link=<%= I %>">Edit This Link</a></td>
					<td><a href="links.asp?action=delete&amp;link=<%= I %>">Delete This Link</a></td>
				</tr>
				<% Next 'I %>
				<tr>
					<td>; 
			</table>
		</td>
	</tr>
	<tr>
		<td align="right"><a href="links.asp?action=add">Add a New Link</a></td>
	</tr>
</table>
		<%
End Select
%>
<a href="links.htm">View the generated links page!</a>