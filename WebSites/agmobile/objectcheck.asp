<%@ LANGUAGE="VBSCRIPT" %><%
'Due to the high number of components, the server may run into Timeout - increase this value then.
Server.ScriptTimeOut=240

Response.Expires = 0
Response.buffer=false
On Error Resume Next
'ObjCheck V0.25 by Kevin Kempfer
'contact me at objcheck@kevinkempfer.de
'    This program is free software; you can redistribute it and/or modify
'    it under the terms of the GNU General Public License as published by
'    the Free Software Foundation; either version 2 of the License, or
'    (at your option) any later version.
'
'    This program is distributed in the hope that it will be useful,
'    but WITHOUT ANY WARRANTY; without even the implied warranty of
'    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
'    GNU General Public License for more details.
'
'    You should have received a copy of the GNU General Public License
'    along with this program; if not, write to the Free Software
'    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
'
'History:
'v0.25  - added descriptions and links for components, now you'll get a short description of the components
'		  as well as a link to both the authors of the component and google.
'		- you can now edit descriptions and links if there's none yet available.
'		- enter your own new components! click on "Add new component"
'		- all new descriptions/links/components will be reviewed before going online.
'		- changed some colors
'v0.24  - changed method for automatic updates to XML-Stream
'v0.23	- added detection for misspelled or incomplete ProgIDs
'v0.221	- this script is now under the GPL, see licence.txt
'v0.22	- added link to Google when a component is installed, so you can get a little bit more 
'		 information on how to work with it.
'v0.21	- added check for URLs in the input box, because people always enter their URLs instead of 
'		 downloading the script...
'v0.2	- fixed some bugs, now first display information, then try to fetch the list. Sometimes
'		 the XMLHTTP-Object doesn't work, maybe due to high traffic on my site. Should work on your site.
'v0.1  - inital release January 2002

'This is the current version. These values are used for update-checking and to tell the list-server how to react,
'will be checked later.
objcheckversion=3
version="0.25"

Dim XMLHTTPError
Dim ServerXMLHTTPError
Dim classID

if request.form("descriptions")="True" then
ShowDescriptionsWhenInstalled = true
end if
if trim(request("classID"))<>"" then
	classID = cint(trim(request("classID")))
	AddDescription=true
elseif trim(request("AddNew"))<>"" then
	AddDescription=true
else
	AddDescription=false
end if
	  sub SayError  %>
      Error: This script only works with the "MSXML2.ServerXMLHTTP" Component 
      or "Microsoft.XMLHTTP" Component installed on the server. This should be 
      done by default at IIS installation. So check your IIS or contact your provider.<br>
	  Maybe this Error was made by me, so please also contact me, the author: <a href="mailto:objcheck@kevinkempfer.de?subject=Error%20in%20ObjCheck&body=<%=replace("Errormessage: " & ServerXMLHTTPError&" / " &XMLHTTPError," ","%20")%>">Kevin Kempfer</a>. (Errormessage: <%=ServerXMLHTTPError%>/<%=XMLHTTPError%>)
      <% 
	  end sub

Function IsObjInstalled(strClassString)
 ' initialize default values
 IsObjInstalled = False
 Err = 0
 ' testing code
 Dim TestObj
 Set TestObj = Server.CreateObject(strClassString)
 If 0 = Err Then 
 	IsObjInstalled = True
 end if
 ' cleanup
 Set TestObj = Nothing
 Err = 0
End Function
	  
function Alternatives(strList,arrList)
	'This function is used to find alternative Component-Prog-IDs, maybe the user input was incomplete
	for i=0 to ubound(arrList,2)
		item=trim(lcase(arrlist(1,i)))
		strList=trim(lcase(strList))
		if instr(item,strList)>0 and not item=strList and not instr(arrlist(1,i),"KevinKempfer")>0 then
			if not ThereAreAlternatives then
				Output = Output & "<br><br><strong><em>Did you mean:</em></strong><br>"
			end if
				Output = Output & "<a href=""" & request.servervariables("script_name") & "?classname=" & arrlist(1,i) & """>" & "<b>" & arrlist(1,i) & "</b></a> <br>"
			ThereAreAlternatives=true
		end if
	next
	if ThereAreAlternatives then
		Output = Output & "<br>Do you think <b>" & strList & "</b> should be listed in the components list? <a href=""mailto:objcheck@kevinkempfer.de?subject=" & Server.urlencode("Please add " & arrlist(1,i)) & """>Contact me</a>!"		
	end if
end function
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
<meta http-equiv="Content-Language" content="en">
<meta name="description" content="This ASP script will check your IIS for installed components. Currently supporting hundreds of known components. The list is still growing while you use the script - no update required.">
<meta name="publisher" content="Kevin Kempfer">
<meta name="copyright" content="Free software under GPL">
<meta name="robots" content="follow,index">
<meta name="author" content="Kevin Kempfer">
<meta name="keywords" content="asp,component,components,object,objects,IIS,Microsoft IIS,installed,check,scanner,scan,jmail,cdonts,mswc,mailer">
<META NAME="revisit-after" CONTENT="7 days">
<META NAME="page-topic" CONTENT="ASP, Scripting, VB-Script, IIS, Components">
<META NAME="audience" CONTENT="All">
<style>
	td {
		font-family: Verdana,Arial;
		font-size: 10pt;
	}
	.status {
		font-family: Verdana,Arial;
		font-size: 10pt;
	}
	tr.even {
		background-color : #EFEFEF;
	}
	tr.odd {
		background-color : White;
	}
	tr.eveninstalled {
		background-color : #FF9933;
	}
	tr.oddinstalled {
		background-color : #FFAA50;
	}
</style>
	<title>Installed Objects Scanner</title>
</head>

<body bgcolor="#FFFFFF" leftmargin="40" marginwidth="40" marginheight="40">
<!-- 
==========================================
ObjCheck V<%=version%> by Kevin Kempfer (objcheck@kevinkempfer.de)
http://www.bier-voting.de/objcheck
<%'Please do not remove this comment. %>==========================================
--> 

<TABLE width="100%" border="1" cellspacing="0" cellpadding="5" bgcolor="#FF9933" bordercolor="#000000">
  <TR>
    <TD> <FONT color="#990000" size="5"><B>Component Check</B></FONT><FONT color="#990000" size="2"> v<%=version%></font><BR>
      <%if not AddDescription then%>Components for IP <font color="#990000"><strong><%= Request.ServerVariables("LOCAL_ADDR") %></strong><%else%>Add new description<%end if%></FONT> 
      </TD>
  </TR>
</TABLE>
<BR>
<BR><font face="Verdana,Arial,'Microsoft Sans Serif'">
<%if AddDescription then%>
Please enter a description, all fields but name are optional, so leave them empty if you don't know everything. Thanks for your support!
<%else%>
Enter a component's ProgID or ClassID to check 
if it's installed on this server (<%= Request.ServerVariables("LOCAL_ADDR") %>). 
Leave the input field empty to check all listed components.
<% End If %>
</FONT>
<BR>
<BR>
<BR>
<%if not AddDescription then%>
<font face="Verdana,Arial,'Microsoft Sans Serif'">If you want to check your own server, you'll have to <a href="http://www.bier-voting.de/objcheck/query.zip">download the script</a> and run it from your site.</font>
<% End If %>
<BR><br><br>
<%
response.write("<div class=""status"" id=""status"">Please wait...<br>")

Dim strList
Dim Output
Dim strClass
Dim myObjectReviewed
Dim myObject
Dim myObjectdesc
Dim myObjectID
Dim myObjectlink

'the following function gets the components names from my webserver
'Leave this URL as is!
ListURL = "http://www.bier-voting.de/objcheck/objects.asp"

Function GetURL2(URL)
Set objHTTP = Server.CreateObject("Microsoft.XMLHTTP")
	objHTTP.open "GET", URL, false
	objHTTP.send
	strList = objHTTP.ResponseStream
Set objHTTP = Nothing
End Function

Function GetURL(URL)
    set xmlhttp = server.CreateObject("MSXML2.ServerXMLHTTP") 
    xmlhttp.open "GET", url, false 
    xmlhttp.send() 
    strList = xmlhttp.responseStream
	set xmlhttp = nothing 
End Function

dim even
function trclass
	if even then
		trclass = "even"
		even = false
	else
		trclass = "odd"
		even = true
	end if
end function

sub showDescription(myObjectID)
	if myObjectReviewed=false and myObjectdesc<>"" then
		Output = Output & "<br>" & myObjectdesc & " <font color=""#FF0000"">(not yet reviewed)</font>"
	elseif myObjectdesc<>"" then
		Output = Output & "<br>" & myObjectdesc 
	else
		Output = Output & "<br><a href=""" & request.servervariables("script_name") & "?classID=" & myObjectID & """> Write a description</a>"
	end if
	if myObjectReviewed=false and myObjectlink<>"" then
		Output = Output & "<br><a target=""_blank"" href=""" & myObjectlink & """>details...</a> <font color=""#FF0000"">(not yet reviewed)</font>"
		Output = Output & " or <a target=""_blank"" href=""http://www.google.com/search?q=" & server.urlencode(myObject) & """>search Google</a>"
	elseif myObjectlink<>"" then
		Output = Output & "<br><a target=""_blank"" href=""" & myObjectlink & """>details...</a>"
		Output = Output & " or <a target=""_blank"" href=""http://www.google.com/search?q=" & server.urlencode(myObject) & """>search Google</a>"
	else
		Output=Output &"<br><a href=""" & request.servervariables("script_name") & "?classID=" & myObjectID & "&addlink=true"">add link</a>"
		Output = Output & " or <a target=""_blank"" href=""http://www.google.com/search?q=" & server.urlencode(myObject) & """>search Google</a>"
	end if
end sub
'Try to use MSXML2.ServerXMLHTTP to retrieve the list, if it's not available, use Microsoft.XMLHTTP
'Why? Because Microsoft.XMLHTTP wasn't build for server use - it's quite buggy...

if IsObjInstalled("MSXML2.ServerXMLHTTP") then
	method = 1
elseif IsObjInstalled("Microsoft.XMLHTTP") then
	method = 2
else
	method = 0
end if

Err = 0

'for adding new classnames to the list and for statistical things and to deliver the right version, and for my pleasure ;-)
ListURL = ListURL & "?site=" & Server.URLEncode(Request.ServerVariables("SERVER_NAME") & Request.ServerVariables("URL"))
'Leave the version string as is! Otherwise, the server will return wrong data
ListURL = ListURL & "&ver=" & version
if request("classname") <> "" then
	ListURL = ListURL & "&classname=" & request("classname")
end if
randomize
'Fake the browser, so it doesn't display cached sites.
ListURL = ListURL & "&nocache=" & Server.URLEncode(rnd*10000)
if method=1 OR method=2 then
	response.write("Getting list...<br>")
	GetURL(ListURL)
	
	If Not 0 = Err Then
 		response.write("MSXML2.ServerXMLHTTP not available, using Microsoft.XMLHTTP ...<br>")
		ServerXMLHTTPError = Err.description
		GetURL2(ListURL)
		
	end if
	if 0 = Err then
		Err = 0
	
		Set objRS = Server.CreateObject("ADODB.Recordset")
		objRS.Open strList
		
		
		AnzahlComponenten = objRS.RecordCount - 1
		
		strClass = Trim(Request("classname"))
		If "" <> strClass then
			arrlist=objRS.getrows
			response.write("Testing for " & strClass & " ...<br></div>")
			If left(strClass, 7) = "http://" or (left(strClass,4)="www." and (right(strClass,4)=".com" or right(strClass,4)=".net" or right(strClass,4)=".org" or right(strClass,4)=".edu" or right(strClass,3)=".de") or right(strClass,4)=".com" or right(strClass,4)=".net" or right(strClass,4)=".org" or right(strClass,4)=".edu" or right(strClass,3)=".br" or right(strClass,3)=".de") then
				Output = Output&"<div class=""status"">" & strClass & " looks like an URL! Remember, this script can only check the server it was started from. If you want to test your own server, <a href=""http://www.bier-voting.de/objcheck/query.zip"">download the script</a>, run it from your server and enter a ProgID (<strong>no URL!</strong>).</div><br>"
			end if
			Output = Output & "<br><div class=""status"" align=""center"">" & strClass & " is " 
		    If Not IsObjInstalled(strClass) then 
			    Output = Output & "not installed!"
				if "" <> strClass then
					Output = Output & Alternatives(strClass,arrList)
				end if
				Output = output & "<br> <br>If you want to add <strong>&quot;" & strClass & "&quot;</strong> to the database, please <a href=""" & request.servervariables("script_name") & "?AddNew=true&Adddescription=true&newClassname=" & strClass & """>click here</a>."
			Else
			    Output = Output & "<strong>installed!</strong> (<a target=""_blank"" href=""http://www.google.com/search?q=" & server.urlencode(strClass) & """>details</a>)"
			End If
			Output = Output & "</div><P>" & vbCrLf
		Else
			if not AddDescription then
				response.write("Testing for " & AnzahlComponenten & " components... (this may take a minute)<br>")
				response.write("<input type=""text"" id=""statuscheck"" size=""40"">")
				' Default: Check the whole list
				icount = 0
				Output = Output & "<table align=""center"" border=""0"" width=""95%"">"
				Do while not objRS.EOF
					myObject = objRS("Object")
					myObjectdesc = objRS("Beschreibung")
					myObjectlink = objRS("link")
					myObjectID = objRS("ID")
					myObjectReviewed = objRS("Online")
					response.write("<s"&"cript>document.getElementById(""statuscheck"").value = ""Checking "&myObject&" ..."";</s"&"cript>")
					If not (left(myObject,12) = "KevinKempfer") then
						'response.write("Testing: "&myObject&"<br>")
						Installed = IsObjInstalled(myObject)
						If Not Installed Then
							If Not Request.Form("nurpositiv") = "True" Then
						    	Output = Output & "<TR class=""" & trclass & """><TD width=""200"">" & myObject & "</TD><TD>is not installed!"
								if not ShowDescriptionsWhenInstalled then
									showDescription myObjectID
								end if
							End If
						Else
						    Output = Output & "<TR class=""" & trclass & "installed""><TD width=""200"">" & myObject & "</TD><TD>is <strong>installed!</strong>"
							showDescription myObjectID
							icount = icount + 1
						End If
						Output = Output & "</TD></TR>" & vbCrLf
					else
						'check version
			  			if cint(right(myObject,1)) > objcheckversion then
							UpdateAvailable = true
						end if
					end if
					Installed=false
					objRS.MoveNext
				loop
				response.write("</div>")
			else
				response.write("</div>")
				bingo=false
				Do while not (objRS.EOF or bingo or request("AddNew")="true")
					myObject = objRS("Object")
					myObjectdesc = objRS("Beschreibung")
					myObjectlink = objRS("link")
					myObjectID = objRS("ID")
					if myObjectID = classID then
						bingo=true
					else
						objRS.MoveNext
						bingo=false
					end if
				loop
			end if
			Set objRS=Nothing
		End If 
	else
		XMLHTTPError = Err.Description
		SayError
		response.end
	end if

%>
<script language="JavaScript">
document.getElementById("status").style.display = "none";
</script>
<%if not AddDescription then%>
<% if UpdateAvailable or trim(request("Thank"))="you" then %>
<TABLE width="70%" border="1" cellspacing="0" cellpadding="5" align="center" bgcolor="#CCCCCC" bordercolor="#000000">
  <TR>
    <TD>
  		<CENTER>
<strong><font color="#ff0000"><% If not trim(request("Thank"))="you" then %>Webmaster! There's an <blink>update</blink> available! Please <a href="http://www.bier-voting.de/objcheck/">check the download-site</a>!<% Else %>Thank you! Your submission will be reviewed as soon as possible.<% End If %></font></strong></CENTER>
	</TD>
  </TR>
</TABLE>
<%end if%>
<table border="0" width="100%">
<tr><td width="15%">&nbsp;</td>
<td width="70%">
<TABLE width="100%" border="1" cellspacing="0" cellpadding="5" align="right" bgcolor="#CCCCCC" bordercolor="#000000">
  <TR>
    <TD>
	<FORM action=<%=Request.ServerVariables("SCRIPT_NAME")%> method="post">
  		
		
		<CENTER>Enter a ProgID (like <strong>JMail.SMTPMail</strong>). If you want to check your own server, <br>
		<a href="http://www.bier-voting.de/objcheck/query.zip">download the script</a>!<br>
		          <BR>
          <table border="0"><tr><td align="left"><input type="text" value="<%=Request("Classname")%>" name="classname" size=40>
		    <BR><INPUT type="checkbox" name="nurpositiv" value="True">Show installed components only
		    <BR><INPUT type="checkbox" name="descriptions" value="True"<%if ShowDescriptionsWhenInstalled then response.write(" checked")%>>Show descriptions for installed components only<br>(where available)
			</td></tr></table><BR><BR><INPUT type=submit value="&gt;&gt;&nbsp;Test <%=Request.ServerVariables("SERVER_NAME")%>&nbsp;&lt;&lt;">
			<br><br>			
		Note: Do <b>not</b> enter your website URL or IP, <a href="http://www.bier-voting.de/objcheck/query.zip">download the script</a> instead.
	  	</CENTER>
	</FORM>
	</TD></tr></table>
</td>
<td width="15%">
	<table align=right border=0 cellpadding=1 cellspacing=0 bgcolor=000000>
		<tr><td align=center>
			<table border=0 cellpadding=3 cellspacing=0 bgcolor=eeeedd>
				<tr><td align=center nowrap>
					<font style="font-size:10pt;font-family:Arial;"><b>Rated:</b>
					 <a href="http://www.Aspin.com/func/review?id=4222310"><img src="http://ratings.Aspin.com/getstars?id=4222310" border=0></a>
					<font style="font-size:8pt;"><br>by <a href="http://www.Aspin.com">Aspin.com</a> users<br></font></font>
					</td></tr><tr nowrap><td align=center><form action="http://www.Aspin.com/func/review/write?id=4222310" method=post>
				<font style="font-size:10pt;font-family:Arial;">What do you think?</font><br>
				<select name="VoteStars"><option>5 Stars<option>4 Stars<option>3 Stars<option>2 Stars<option>1 Star</select><input type=submit value="Vote">
	</td></form></tr></table>
</td></tr></table>
</td>
  </TR>
</TABLE>


<BR>
<div align="center"><font face="Verdana,Arial,'Microsoft Sans Serif'">Currently checking for <strong><%= AnzahlComponenten %></strong> components.<br>
<% If icount > 0 then %><strong><%= icount %></strong> components installed.<br>
<a href="<%=request.servervariables("script_name")%>?AddNew=true&Adddescription=true">Click here to add a new component to this list.</a><% End If %></font></div>
<%= Output %> <br>
<br>
<%else%><form action="<%=ListURL%>&adddescription=true" method="post"><input type="hidden" name="classID" value="<%=MyObjectID%>">
<div align="center">
<table>

<tr>
	<td>ClassID:<%if trim(request("AddNew"))<>"" then%><input type="hidden" name="AddNew" value="true"><br>
					(like "JMail.SMTP")<% End If %></td>
	<td><%if trim(request("AddNew"))<>"" then%><input type="text" name="newClassname" value="<%=request("newClassname")%>" size="20"><% Else %><%=MyObject%><% End If %></td>
</tr>

<tr>
	<td valign="top">Description:</td>
	<td><%if isempty(trim(cstr(myObjectdesc))) or myObjectdesc="" then%><textarea cols="20" rows="5" name="description"></textarea><% Else %><%=myObjectdesc%><% End If %></td>
</tr>
<tr>
	<td>URL:</td>
	<td><%if isempty(trim(cstr(myObjectlink))) or myObjectlink="" then%><input type="text" name="url" value="http://" size="20"><% Else %><%=myObjectlink%><% End If %></td>
</tr>
<tr>
	<td></td>
	<td><input type="submit" name="submit" value="submit"></td>
</tr>
</table></div>
</form>
<%end if%>
<TABLE width="100%" border="1" cellspacing="0" cellpadding="5" bgcolor="#FF9933" bordercolor="#000000">
  <TR>
    <TD align="center">
	<font face="Verdana,Arial,'Microsoft Sans Serif'">This script is freeware by <a href="mailto:objcheck@kevinkempfer.de">Kevin Kempfer</a>. If you think your components should be listed here, <a href="mailto:objcheck@kevinkempfer.de">contact me</a> or <a href="<%=request.servervariables("script_name")%>?AddNew=true&Adddescription=true">click here</a>.
	  </FONT>

      <% Else
	  	  sayerror
	  End If %>
    </TD>
  </TR>
</TABLE>
</body>
</html>
