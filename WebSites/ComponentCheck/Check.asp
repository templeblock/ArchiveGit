<%@ LANGUAGE="VBSCRIPT" %><%
'Due to the high number of components, the script may run into Timeout - increase this value then.
Server.ScriptTimeOut=240

Response.Expires = 0
Response.buffer=false
'ObjCheck V0.41 by Kevin Kempfer
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
'v0.41
'		- bugfixes
'v0.40 beta
'		- completly rewritten, maybe contains bugs, please feel free to send feedback to beta@kevinkempfer.de
'		- the database is now downloaded only when needed and then stored locally. you may force an update by clicking
'		  "update database now" in the upper right corner
'		- new update mechanism: the script checks for available updates and downloads them. 
'		  Set update_enabled = false to disable.
'		- removed "Skip currently tested component"-Link - you can now select which components you want to check.
'		- details on not installed components are now hidden until you move your mouse over the red *
'v0.30	- Changed the backend database to MySql due to high traffic on the list server. Some changes had to be 
'		  made on the client script, too.
'v0.29	- Added two links above the table 
'			- 'Show installed components only' will hide all not installed components
'			- 'Show all' will show them all again.
'v0.28	- You can now skip components while testing. Either click on the link next to the status field when the 
'		  component appears, or call the script like http://yourserver/objcheck/default.asp?skip=ProgID and replace
'	 	  ProgID with the ProgID of the component that will be skipped
'		- Some internal changes, more Error-checking, optimized texts
'v0.27	- Changed handling of different methods to get the list of components from my server. Now the script
'		  will test every method until one is successfull, because i.e. ServerXMLHTTP.4.0 seems to be buggy when
'		  behind a firewall or proxy.
'v0.26  - if there's no link yet available for a component, Google's number one search result will be provided. 
'		  Updated daily, thanks to Google for this great web service.
'		- added support for more components for automatic update downloads and proxy support.
'		  Because I don't use proxies, I can't test the proxy support. Please give me some feedback on this.
'		  If you need to use a proxy, enter the proxy information below.
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

'This is the current version. These values are used for update-checking and to tell the list-server 
'how to react, will be checked later.
dim strversion
objcheckversion=7
strversion="0.41"
version=0.41

'Set proxy here
ProxyAddress=""
proxyusername=""
proxypassword=""

'Set this to false, if you don't want updates or have no write access (not recommended)
update_enabled = true

Dim updateurl
dim update_link
Dim ListURL
Dim update_desc
Dim page
Dim single_object
Dim XMLHTTPError
Dim strTXT
Dim strRS
Dim resultset
Dim resultset2
Dim resultset3
dim CountAll
Dim CountInstalled
Dim jsarray
Dim strRsXML
Dim stream
Dim reloadDatabase
Dim errorcode
Dim skip

CountInstalled = 0
Set objRS = Server.CreateObject("ADODB.Recordset")
Set objRSdata = Server.CreateObject("ADODB.Recordset")
if request.querystring("debug")<>"" then
	session("debug")=request.querystring("debug")
end if

'Leave those URLs as they are - needed for retrieving updates and the database
'distributed on two servers due to high traffic
updateurl = "http://www.kempfer-online.de/objupdate.txt"
ListURL = "http://objcheck.bier-voting.de/objcheck/db.php"
'Fake the browser, so it doesn't display cached sites.
ListURL = ListURL & "?nocache=" & Server.URLEncode(rnd*10000)
Dim methods(3)



'functions for later use
function WriteToApplication
	Set conn = CreateObject("ADODB.Connection")
	Response.Write "ADO-Version: " & conn.Version & "<br><br>"
	If (conn.Version) >= "2.5" Then
		Set stream = Server.CreateObject("ADODB.Stream")
		set rs = Server.CreateObject("ADODB.Recordset")
		rs.open strRS
		rs.Save stream, 1
		strRsXML = stream.ReadText(-1)
		rs.Close
		Set rs = nothing
		stream.Close
		Set stream = Nothing
		
		Application.Lock
		Application("objcheckDatabase") = strRsXML
		Application.UnLock		
		Response.Write Replace(Server.HTMLEncode(strRsXML),VbCrLf,"<br>"&VbCrLf)
		'Response.Write "<br><br>" & VbCrLf
	else
		response.write("Falsche ADO-Version")
	end if
end function

function WriteXMLToApplication(XML)
	Set conn = CreateObject("ADODB.Connection")
	'Response.Write "ADO-Version: " & conn.Version & "<br><br>"
	If (conn.Version) >= "2.5" Then
		Application.Lock
		Application("objcheckDatabase") = XML
		Application.UnLock		
	else
		sayerror("Error while saving the database. You need ADO version 2.5 or higher.")
	end if
	set conn = Nothing
end function

sub ReadFromApplication
	set objRS = nothing
	Set objRS = Server.CreateObject ("ADODB.Recordset")
	objRS.CursorLocation = 3  ' 3 = adUseClient
	objRS.LockType = 4      ' 4 = adLockBatchOptimistic
	objRS.CursorType = 3    ' 3 = adOpenStatic
	
	strRsXML = Application("objcheckDatabase")
		
	Set stream = Server.CreateObject("ADODB.Stream")
	stream.open
	stream.WriteText strRsXML, 0  ' 0 = adWriteChar
	stream.Position = 0
	
	objRS.Open stream
	Set objRS.ActiveConnection = Nothing 
	stream.close
	Set stream = Nothing
end sub
sub ReadData(data)
	set objRSdata = nothing
	Set objRSdata = Server.CreateObject ("ADODB.Recordset")
	objRS.CursorLocation = 3  ' 3 = adUseClient
	objRS.LockType = 4      ' 4 = adLockBatchOptimistic
	objRS.CursorType = 3    ' 3 = adOpenStatic
	
	strRsXML = data
	
	Set stream = Server.CreateObject("ADODB.Stream")
	stream.open
	stream.WriteText strRsXML, 0  ' 0 = adWriteChar
	stream.Position = 0
	
	objRSdata.Open stream
	Set objRSdata.ActiveConnection = Nothing
	stream.close
	Set stream = Nothing
end sub


function getURL(url,mode)
'get the database via XML-String
	On Error Resume Next
	getURL = false
	methods(0)="MSXML2.ServerXMLHTTP.4.0"
	methods(1)="MSXML2.ServerXMLHTTP.3.0"
	methods(2)="MSXML2.ServerXMLHTTP"
	methods(3)="Microsoft.XMLHTTP"
	
	for each method in methods
		err=0
		set xmlhttp = server.CreateObject(method) 
		if err.number=0 then
			if method="MSXML2.ServerXMLHTTP.4.0" and ProxyAddress<>"" then
				xmlhttp.setProxy 0,ProxyAddress,""
				if proxyusername<>"" then
				xmlhttp.setProxyCredentials proxyusername, proxypassword
				end if
			end if
			if instr(URL,"?")=0 then
				URL =URL & "?"
			end if
			URL = URL & "&ver="&strversion&"&site="&server.urlencode(request.servervariables("server_name")&request.servervariables("url"))
		    xmlhttp.open "GET", URL, false
			if method="Microsoft.XMLHTTP" then
				xmlhttp.send
			else
			    xmlhttp.send() 
			end if
			if mode=0 then
				WriteXMLToApplication xmlhttp.ResponseText
				debug(URL)
			elseif mode=1 then
				strRS = xmlhttp.ResponseText
				strTXT = strRS
				debug(URL)
			end if
			if err.number <> 0 then 
				set xmlhttp = nothing 
		    	XMLHTTPError=XMLHTTPError& "<hr><b> Using "&method&"</b><br>"&xmlhttp.parseError.URL & _ 
		            "<br>" & xmlhttp.parseError.Reason 
		    else
				if xmlhttp.status<>200 then
					set xmlhttp = nothing 
					exit for
				else
					set xmlhttp = nothing 
					getURL=true
					exit for
				end if
		    end if 
		else
			debug(method & " is not installed.")
		end if
	next
	On Error Goto 0
end function 

function debug(text)
	if session("debug")="1" then
		response.write text & "<br>"
	end if
end function

function checkUpdates
	debug("function checkUpdates")
	checkUpdates = false
	if update_enabled and not session("update_check_done") then
		if getURL(ListURL&"&checkupdate="&strversion,1) then
			debug("Update Check")
			ReadData(strRS)
			if Err <> 0 then
				sayerror("Could not check for update. The script needs to have access to the listserver. Please <a href=""http://www.bier-voting.de/objcheck/"">download updates</a> yourself.")
				session("update_check_done")=true
			else
				if not objRSdata.EOF then
					if clng(objRSdata.Fields("current_version"))>clng(objcheckversion) then
						debug(clng(objRSdata.Fields("current_version"))&">clng("&objcheckversion&")")
						checkUpdates = true
						update_desc = objRSdata.Fields("current_desc")
						update_link = objRSdata.Fields("link")
					end if 
					session("update_check_done")=true
				end if
			end if
		else
			debug("could not get update")
			sayerror("Could not check for updates.")
		end if
	else
		debug("No update check")
	end if
end function

function getUpdates
	if checkUpdates then
		if getURL(updateurl,1) then
			set fso = Server.Createobject("Scripting.FileSystemObject")
			set f=fso.OpenTextFile(Server.MapPath(request.servervariables("script_name")),2,true)
			if 0=Err.number then
				f.Write strTXT
				f.Close
			else
				sayerror("Could not update engine. The script must have write permissions on itself!<br>Please <a href="""&update_link&""">download the update</a> yourself.")
				
			end if
			set f=nothing
			set fso=nothing
			getUpdates=(0=Err.number)
		else
			sayerror("Could not download update. Please <a href="""&update_link&""">download the update</a> yourself.")
		end if
	end if
end function

Function IsObjInstalled(strClassString)
On Error Resume Next
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
 TestObj.Close
 Set TestObj = Nothing
 Err = 0

End Function

function PrintHeader
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
	<html>
		<head>
			<meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
			<meta http-equiv="Content-Language" content="en">
			<meta name="description" content="This ASP script will check your IIS for installed components. Currently supporting hundreds of known components. The list is still growing while you use the script - no update required.">
			<meta name="publisher" content="Kevin Kempfer">
			<meta name="copyright" content="Free software under GPL">
			<meta name="robots" content="index">
			<meta name="author" content="Kevin Kempfer">
			<meta name="keywords" content="asp,component,components,object,objects,IIS,Microsoft IIS,installed,check,scanner,scan,jmail,cdonts,mswc,mailer">
			<META NAME="revisit-after" CONTENT="7 days">
			<META NAME="page-topic" CONTENT="ASP, Scripting, IIS, Components, COM">
			<META NAME="audience" CONTENT="All">
			<style>
				body {
					margin:30px;
				}
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
					background-color : #FFFFFF;
				}
				tr.eveninstalled {
					background-color : #FF9933;
				}
				tr.oddinstalled {
					background-color : #FFAA50;
				}
				tr.oddnotinstalled {
					background-color : #FFFFFF;
				}
				tr.evennotinstalled {
					background-color : #EFEFEF;
				}
				.header,.footer{
					background-color : #FF9933;
					border: 2px solid black;
					width:100%;
				}
				.header {
					margin-bottom: 30px;
					font-family: Verdana,Arial;
					font-size:12pt;
					padding: 5px;
				}
				.footer {
					margin-top: 30px;
					font-family: Verdana,Arial;
					font-size:8pt;
					padding: 2px;
				}
				.form {
					width:600px;
				}
				.formtext,.formarea {
					width:400px;
				}
				.desc{
					margin-left:70px;
				}
				.smalllinks{
					font-size:7pt;
				}
				.toolspan{
					color:red;
				}
				fieldset{
					font-family: Verdana,Arial;
					font-size: 10pt;				
				}
				.error{
					color:red;
					font-family: Verdana,Arial;
					font-size: 10pt;
					font-weight:bold;
				}
				#status{
					font-family: Verdana,Arial;
					font-size: 10pt;
				
				}
</style>
			<script language="JavaScript">
			function u(strobject)
			{
				document.getElementById("statuscheck").value=strobject;
			}
			function show(what)
			{
				var count = document.getElementsByName(what).length;
				for(i=0;i<count;i++)
				{
					tr = document.getElementsByName(what)[i];
					tr.style.display='block';
				}
			}
			function hide(what)
			{
				var count = document.getElementsByName(what).length;
				for(i=0;i<count;i++)
				{
					tr = document.getElementsByName(what)[i];
					tr.style.display='none';
				}
			}
			function hide_status()
			{
				document.getElementById("status").style.display = "none";
			}
			function checkAll()
			{
				for (i=0;i<document.getElementsByName("id").length;i++)
					document.getElementsByName("id")[i].checked = true;
			}
			function uncheckAll()
			{
				for (i=0;i<document.getElementsByName("id").length;i++)
					document.getElementsByName("id")[i].checked = false;
			}
			
			function showdesc(id)
			{
				document.getElementById('d'+id).style.display="block";
			}
			function hidedesc(id)
			{
				document.getElementById('d'+id).style.display="none";
			}
			function check(strRadio)
			{
				document.getElementById(strRadio).checked=true;
			}
			function checkbox(strRadio)
			{
				document.getElementById(strRadio).checked=(!(document.getElementById(strRadio).checked));
			}			
			</script>
				<title>Installed Components Scanner @ <%=request.servervariables("SERVER_NAME")%></title>
		</head>
		<body bgcolor="#FFFFFF">
		<!-- 
		==========================================
		ObjCheck V<%=version%> (c) Kevin Kempfer (objcheck@kevinkempfer.de)
		http://www.bier-voting.de/objcheck
		<%'Please do not remove this comment. %>==========================================
		--> 
<div class="header"><B>Component Check</B> v<%=strversion%><br>
<%
	select case page
		case "","0","2"
			Response.Write "Components for IP <strong>" & Request.ServerVariables("LOCAL_ADDR") & "</strong>"
		case "1"
			Response.Write "Scan for single component"
		case "3"
			Response.Write "Add a new description for <strong>" & single_object & "</strong>"
		case "4"
			Response.Write "Add a component"
		case "5"
			Response.Write "Thank you"
		case "6"
			Response.Write "Select components to scan for"
		case "7"
			Response.Write "Updating database"
		case else
			Response.Write "nothing here"
	end select
	response.write("<div align=""right""><a href="""&request.servervariables("script_name")&"?page=7"">update database now</a></div>")
	Response.Write "</div>"
end function

function PrintFooter
%>
		<div class="footer">
			The <a href="http://www.bier-voting.de/objcheck/">Installed Components Scanner</a> is freeware by <a href="mailto:objcheck@kevinkempfer.de">Kevin Kempfer</a>. If you think your components should be listed here, <a href="mailto:objcheck@kevinkempfer.de">contact me</a> or <a href="<%=request.servervariables("script_name")%>?page=4">click here</a>.
		</div>
	</body>
</html>
<%
end function

function PrintFormMain
%>
<form name="form1" id="form1" action="<%=Request.Servervariables("script_name")%>" method="get" style="display:inline;">
<table border="0">
	<tr>
		<td valign="top"><fieldset class="form" style="width:650px;">
		<legend class="form_legend">Scan</legend>
		<input type="radio" name="page" id="radio1" value="2"<%if page="2" or page="" or isnull(page) then%> checked checked="checked"<%end if%>><span onClick="check('radio1')">Scan for all known components</span><br>
		<input type="radio" name="page" id="radio2" value="1"<%if page="1" then%> checked checked="checked"<%end if%>><span onClick="check('radio2')">Scan for this component: <input type="text" name="single_object" value="<%=single_object%>"></span><br>
		<input type="radio" name="page" id="radio3" value="6"<%if page="6" then%> checked checked="checked"<%end if%>><span onClick="check('radio3')">Select components to scan for</span><br><br>
		
		<input type="checkbox" name="show_installed" id="radio4" value="1"<%if show_installed then%> checked<%end if%>><span onClick="checkbox('radio4')"> Only show components that are installed</span><br>
		<input type="checkbox" name="show_desc" id="radio5" value="1"<%if show_desc then%> checked<%end if%>><span onClick="checkbox('radio5')"> Show descriptions where available</span><br>
		<input type="submit" name="submit" value="Start"><br>
		<br>
		Note: If you want to scan your own server, just <a href="http://www.bier-voting.de/objcheck/">download the script</a> and place it on your server.
	</fieldset></td>
		<td valign="top" style="padding-left:20px;padding-top:10px;"><%=votecode%><span id="ad"><script type="text/javascript"><!--
google_ad_client = "pub-7029288353889431";
google_ad_width = 200;
google_ad_height = 90;
google_ad_format = "200x90_0ads_al_s";
google_ad_channel ="5244822408";
google_color_border = "FF9933";
google_color_bg = "FFFFFF";
google_color_link = "006699";
google_color_url = "006699";
google_color_text = "000000";
//Please leave the advertisement code as is, it's the only very very small income from this work.
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script></span>
</td>
	</tr>
</table>

</form>
<%
end function

function FormAddDesc(ID_object,ObjectName,ObjectDesc,ObjectLink)

%>
<form name="form1" id="form1" action="<%=ListURL%>" method="post">
<input type="hidden" name="page" value="3">
<input type="hidden" name="return" value="<%=Server.URLEncode("http://"&Request.ServerVariables("SERVER_NAME") & Request.ServerVariables("URL") &"?page=5&reloadDatabase=1")%>">
<input type="hidden" name="ID_object" value="<%=ID_object%>">
	<fieldset class="form">
		<legend class="form_legend">Add description/link for <strong><%=ObjectName%></strong></legend>
		<%if ObjectDesc="" then%>
		Please enter a description for the ProgID <strong><%=ObjectName%></strong>. The text should describe
		what the component is good for. (no advertising, no websites, no email-addresses etc.)
		<%else%>
		Description:
		<%end if%>
		<br><br>
		<%if ObjectDesc="" then%><textarea class="formarea" cols="30" rows="5" name="desc"><%end if%><%=ObjectDesc%><%if ObjectDesc="" then%></textarea><%end if%><br><br>
		If you have a link to the developer of <strong><%=ObjectName%></strong>, enter it here. <strong>Do not enter your 
		own website or your email address or anything else.</strong><br><br>
		<input class="formtext" type="text" name="url"<%if ObjectLink<>"" then%> disabled<%end if%> value="http://<%=ObjectLink%>"><br>
		<br>
		<input type="submit" name="submit" value="Submit">
		<input type="button" name="cancel" value="Cancel" onClick="history.back();">
	</fieldset>
</form>
<%
end function

function FormAddObject
%>
<form name="form1" id="form1" action="<%=ListURL%>" method="post">
<input type="hidden" name="page" value="4">
<input type="hidden" name="return" value="<%=Server.URLEncode("http://"&Request.ServerVariables("SERVER_NAME") & Request.ServerVariables("URL") & "?page=5&reloadDatabase=1")%>">
	<fieldset class="form">
		<legend class="form_legend">Add a new component</legend>
		Please enter the component's ProgID (e.g. &quot;JMail.Message&quot;)<br>
		<input type="text" name="progid" class="formtext"><br><br>
		Please enter a description for the new component. The text should describe
		what the component is good for. (no advertising, no websites, no email-addresses etc.)<br><br>
		<textarea class="formarea" cols="30" rows="5" name="desc"></textarea><br><br>
		If you have a link to the developer<%if ObjectName<>"" then%> of <strong><%=ObjectName%></strong><%end if%>, enter it here. <strong>Do not enter your 
		own website or your email address or anything else.</strong><br><br>
		<input class="formtext" type="text" name="url" value="http://"><br>
		<br>
		<input type="submit" name="submit" value="Submit"> All submissions will be reviewed!
	</fieldset>
</form>
<%
end function

function PrintThankYou

errorcode = request.querystring("errorcode")
%>
	<fieldset class="form">
		<legend class="form_legend">Thank You</legend>
		<%select case errorcode
			case 0,""
			if reloadDatabase then
				response.write("<div id=""status"">")
				sayStatus "Downloading database...",0
				if getDatabase then
					sayStatus "done",1
					ReadFromApplication
				else
					sayStatus "failed",1
					sayerror("Could not download database:" & XMLHTTPError)
				end if
				response.write("</div><s"&"cript>hide_status();</s"&"cript>")
			end if
		%>
		Thank You! Your submission will be reviewed as soon as possible. <a href="<%=request.servervariables("script_name")%>">Click here to go back</a>.
		<%	case 1,"1"%>
		Error! The ProgID you entered is not in form <strong>xxxx.yyyy</strong><br>
		Make sure you enter a valid ProgID.
		<input type="button" value="Try again" onClick="location.href='<%=request.servervariables("script_name")%>?page=4';"> <input type="button" value="Cancel" onClick="<%=request.servervariables("script_name")%>">
		<%	case 2,"2"%>
		Error! The ProgID you entered is already listed in the database.<br>
		<input type="button" value="OK" onClick="location.href='<%=request.servervariables("script_name")%>';">
		<%end select%>
	</fieldset>
<%
end function

function CheckSingle(progID)
is_url = (left(progID,7)="http://" or (left(progID,4)="www." and instrrev(progID,".")>4))

%>
	<fieldset class="results">
	<legend>Results</legend>
	<%if is_url then%>
	&quot;<%=progID%>&quot; looks like an URL! Be sure not to enter a website address. If you want to check another webserver for installed components, <a href="http://www.bier-voting.de/objcheck/query.zip">download this script</a>, put it on your server and run it from there.
	<%else
		update = getURL(ListURL & "&page=1&progid="&progID,2)
	%>
	The component &quot;<%=ProgID%>&quot; is
	<%
		if not isObjInstalled(ProgID) then
			response.write "<strong>not</strong> "
		end if
	%>
	installed on this server!
	<%end if%>
	</fieldset>
<%
end function

function PrintReloadDatabase
	response.write("<div id=""status"">")
	sayStatus "Downloading database...",0
	if getDatabase then
		sayStatus "done",1
		ReadFromApplication
		sayStatus "Database successfully updated. <input type=""button"" value=""OK"" onClick=""location.href='"&request.servervariables("script_name")&"';"">",1
	else
		sayStatus "failed",1
		sayerror("Could not download database:" & XMLHTTPError)
	end if
	response.write("</div>")
	
end function

function SetStatus(text)
			response.write("<s"&"cript>u('"&text&"');</s"&"cript>")
end function

function sayStatus(text,br)
	response.write text 
	if br then
		response.write "<br>"
	end if
end function

function output(byref RS,installed)
		objectname=RS.Fields("object")
		if isnull(RS.Fields("Beschreibung")) or trim(RS.Fields("Beschreibung"))="" then
			objectDesc = ""
		else
			objectDesc = RS.Fields("Beschreibung")
		end if
		objectLink=RS.Fields("link")
		if isnull(objectDesc) then
			objectDesc=""
		end if
		if isnull(objectLink) then
			objectLink=""
		end if
		
		if RS.Fields("link")<>"" then
			objectlinks = objectlinks & " <a href="""&RS.Fields("link")&""">more information</a>"
		else
			moreInfoLink=true
			objectlinks = objectlinks&" <a href="""&request.servervariables("script_name")&"?page=3&ID_object="&RS.Fields("ID")&"&objectName="&Server.URLEncode(objectname)&"&ObjectDesc="&Server.URLEncode(objectDesc)&"&ObjectLink="&Server.URLEncode(objectLink)&""">add description/link</a>"
		end if
		if RS.Fields("googlelink")<>"" then
			objectlinks = objectlinks & " <a href="""&RS.Fields("googlelink")&""">Google's #1</a>"
		end if
		if show_desc then
			if (not moreInfoLink) and objectDesc = "" then
				objectlinks = objectlinks&" <a href="""&request.servervariables("script_name")&"?page=3&ID_object="&RS.Fields("ID")&"&objectName="&Server.URLEncode(objectname)&""">add description/link</a>"
			end if
		else
			objectDesc = ""
		end if	
		if RS.Fields("online")=0 then
			objectDesc = objDesc & "(not yet reviewed)"
		end if
	if installed then
		CountInstalled = CountInstalled + 1

		resultset = resultset &"<span><strong>"& RS.Fields("object") & "</strong>"
		resultset = resultset &"<div class=""smalllinks"">"&objectlinks&"</div>"
		resultset = resultset &"<div class=""desc"">"&objectDesc&"</div>"
		resultset = resultset &"</span><br>" & vbcrlf
	else
		resultset2 = resultset2 &RS.Fields("object")
		if objectDesc<>"" or RS.Fields("link")<>"" or RS.Fields("googlelink")<>"" then
			resultset2 = resultset2 & "<span id=""o"&RS.Fields("ID")&""""
			resultset2 = resultset2 &" onMouseOver=""showdesc("&RS.Fields("ID")&")"" onMouseOut=""hidedesc("&RS.Fields("ID")&")"" class=""toolspan""> *"
			resultset2 = resultset2 & "</span>"
		end if
		resultset2 = resultset2 & "<br>"

		if objectDesc<>"" or RS.Fields("link")<>"" or RS.Fields("googlelink")<>"" then
			resultset2 = resultset2 & "<span id=""d"&RS.Fields("ID")&""" onMouseOver=""showdesc("&RS.Fields("ID")&")"" onMouseOut=""hidedesc("&RS.Fields("ID")&")"" style=""display:none;border:thin solid black;background-color:#FFFF99""><br>" & objectlinks & "<br>" &objectDesc & "</span>"
		end if

		
	end if
end function

function getDatabase
	getDatabase = getURL(ListURL & "&getDatabase=1",0)
end function

function CheckAllComponents
	Response.write "Testing: <input type=""text"" id=""statuscheck"" size=""40"">"
	if request.form("id")<>"" then
		'check selected components only
		checkselected = true
		ObjectsList = ","&replace(request("id")," ","")&","
	end if
	do while not objRS.EOF
		if (not checkselected OR instr(ObjectsList,","&objRS.Fields("ID")&",")>0) AND not instr(objRS.Fields("Object"),"KevinKempfer")>0 then
			if not objRS.Fields("object") = skip then
				SetStatus(objRS.Fields("object"))
				output objRS,IsObjInstalled(objRS.Fields("object"))
				CountAll = CountAll + 1
			end if
		end if
		if objRS.Fields("Object")="KevinKempfer.objcheckvote" then
			votecode = objRS.Fields("Beschreibung")
		end if
		objRS.MoveNext
	loop
	objRS.Close
end function

function CheckAll
	response.write "<fieldset class=""results"">"
	response.write "<legend class=""legend_results"">Results</legend>"
	
	if show_desc then
		ListURL = ListURL & "&all=1&show_desc=1"
	else
		ListURL = ListURL & "&all=1&show_desc=0"
	end if
	response.write("<div id=""status"">")
	if application("objcheckDatabase")="" or reloadDatabase then
		sayStatus "Downloading database...",0
		if getDatabase then
			sayStatus "done",1
			ReadFromApplication
			CheckAllComponents
		else
			sayStatus "failed",1
			sayerror("Could not download database:" & XMLHTTPError)
		end if
	else

		sayStatus "Database already downloaded",1
		ReadFromApplication
		strTXT = session("objcheckDatabaseTXT")
		CheckAllComponents

	end if
	response.write("</div><s"&"cript>hide_status();</s"&"cript>")
	response.write "Scanned for " & CountAll & " components, " & CountInstalled & " installed.<br>"
	if count>0 then
		response.write "<a href="""&request.servervariables("script_name")&"?page=2&count="&count&"&countstart="&countend&""">Scan next " & count & " components</a><br>"
	end if
	response.write "<table border=""0"" width=""100%""><tr><td valign=""top"">"
	response.write "<fieldset><legend>Installed Components</legend>"
	response.write resultset
	response.write "</fieldset>"
	if not show_installed then
		response.write "</td>"
		response.write "<td valign=""top"" width=""250"">"
		response.write "<fieldset><legend>Not Installed Components</legend>"
		response.write resultset2
		response.write "</fieldset>"
	end if
	response.write "</td></tr></table>"	
	response.write "</fieldset>"
end function

sub PrintListSelect
	i=0
	if show_desc then
		showdesc = 1
	end if
	response.write "<form method=""post"" action="""&request.servervariables("script_name")&"?page=2&show_desc="&showdesc&""">"
	response.write "<input type=""submit"" name=""submit"" value=""Scan for selected""> <input type=""button"" onclick=""history.back();"" value=""Cancel"">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <input type=""button"" onclick=""checkAll();"" value=""select all""> <input type=""button"" onclick=""uncheckAll();"" value=""select none"">"
	response.write "<table border=""0""><tr>"
	do while not objRS.EOF
		if not instr(objRS.Fields("object"),"KevinKempfer")>0 then
			if i mod 3 = 0 and i>0 then
				response.write "</tr><tr>"
			end if
			response.write "<td><input type=""checkbox"" name=""id"" value="""&objRS.Fields("ID")&"""> " & objRS.Fields("Object") & "</td>"
			i=i+1
		end if
		objRS.MoveNext
	loop
	response.write "</tr></table>"
	response.write "<input type=""submit"" name=""submit"" value=""Scan for selected""> <input type=""button"" onclick=""history.back();"" value=""Cancel"">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <input type=""button"" onclick=""checkAll();"" value=""select all""> <input type=""button"" onclick=""uncheckAll();"" value=""select none"">"
	response.write "</form>"
end sub

function PrintSelectObjects
		response.write "<fieldset class=""results"">"
	response.write "<legend class=""legend_results"">Results</legend>"
	
	if show_desc then
		ListURL = ListURL & "&select=1&show_desc=1"
	else
		ListURL = ListURL & "&select=1&show_desc=0"
	end if
	response.write("<div id=""status"">")
	if application("objcheckDatabase")="" or reloadDatabase then
		sayStatus "Downloading database...",0
		if getDatabase then
			sayStatus "done",1
			ReadFromApplication
			PrintListSelect
		else
			sayStatus "failed",1
			sayerror("Could not download database:" & XMLHTTPError)
		end if
	else
		sayStatus "Database already downloaded",1
		response.write("</div><s"&"cript>hide_status();</s"&"cript>")		
		ReadFromApplication
		strTXT = session("objcheckDatabaseTXT")
		PrintListSelect
	end if
	
end function

function sayerror(text)
	response.write "<span class=""error"">" & text & "</span><br>"
end function
'First, check for updates
if getUpdates then
	response.redirect(request.servervariables("script_name"))
end if

'Main functionality
page = request.querystring("page")
single_object = request.querystring("single_object")
show_installed = (request.querystring("show_installed")="1")
reloadDatabase =(request.querystring("reloadDatabase")="1")
skip = request.querystring("skip")
if page="" and request.querystring("show_desc")="" then
	show_desc=true
else
	show_desc = (request.querystring("show_desc")="1")
end if

select case page
	case "2","1","0",""
		'default page
		if page="" and single_object<>"" then
			page = "1"
		end if

		PrintHeader
		PrintFormMain
		select case page
			case "1"
				if single_object<>"" then
					CheckSingle(single_object)
				else
					sayerror("No ProgID specified.")
				end if
			case "2"
				CheckAll
		end select
		PrintFooter
	case "3"
		'add a description
		PrintHeader
		if isnumeric(request.querystring("ID_object")) then
			FormAddDesc clng(request.querystring("ID_object")),request.querystring("ObjectName"),request.querystring("ObjectDesc"),request.querystring("ObjectLink")
		else
			sayerror("Wrong data submitted.")
		end if
		PrintFooter
	case "4"
		'add new component
		PrintHeader
		FormAddObject
		PrintFooter
	case "5"
		'Thank You - Page
		PrintHeader
		PrintThankYou
		PrintFooter
	case "6"
		'select components to scan for
		PrintHeader
		PrintSelectObjects
		PrintFooter
	case "7"
		'reload the database
		PrintHeader
		PrintReloadDatabase
		PrintFooter
	case else

end select

Set objRS = Nothing
%>
