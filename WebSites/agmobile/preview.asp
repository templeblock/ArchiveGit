<%@ Language=JavaScript %>
<% Response.Buffer = true; %>
<!-- #include file="script.asp" -->
<% 
	var Agent = String(Request.servervariables('HTTP_USER_AGENT'));
	var bWireless = (Agent.indexOf('Mozilla') < 0);
	var bWML = false; //j

	function output(Text)
	{
		Response.write(Text + '\n');
	}
	
	function outputReturnCategories()
	{
		output('	<a accesskey="8" href="index.asp?cid=categories" style="font-weight:normal">Return to Categories</a><br/>');
	}

	function outputReturnHome()
	{
		output('	<a accesskey="9" href="index.asp" style="font-weight:normal">Return Home</a><br/>');
	}

//j	Response.Expires = 0; // Be sure this pages always refreshes
	var pid = (!Request.QueryString('pid').Count ? 'NotFound' : String(Request.QueryString('pid')));
	var bBadRequest = !Request.QueryString('pid').Count;
	var pname = pid + '/graphic.gif';
	if (!FileExists(pname))
	{
		pname = pid + '/graphic.jpg';
		if (!FileExists(pname))
		{
			bBadRequest = true;
			pid = 'NotFound';
			pname = pid + '/graphic.gif';
		}
	}

	var bPickup = (Request.QueryString('x').Count > 0);
	var Title = (bPickup ? 'AG Greeting ' : 'AG Greeting ') + pid;
	if (bWML)
	{
		Response.ContentType = 'text/vnd.wap.wml';
		output('<?xml version=""1.0""?>');
		output('<!DOCTYPE wml PUBLIC ""-//WAPFORUM//DTD WML 1.1//EN"" ""http://www.wapforum.org/DTD/wml_1_1.dtd"">');
		output('<wml>');
		output('<template>');
		output('	<do type="prev" label="Back">');
		output('		<prev />');
		output('	</do>');
		output('	<do type="options" label="Home to ag.com">');
		output('		<go href="index.xml"/>');
		output('	</do>');
		output('</template>');
		output('<card id="preview" title="' + Title + '">');
	}
	else
	{
		Response.ContentType = 'text/html';
		output('<?xml version="1.0" encoding="UTF-8"?>');
		output('<!DOCTYPE html PUBLIC "-//WAPFORUM//DTD XHTML Mobile 1.0//EN" "http://www.wapforum.org/DTD/xhtml-mobile10.dtd">');
		output('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">');
		output('<head>');
//		output('	<link rel="stylesheet" href="style.css" type="text/css"/>');
		output('	<style>');
		output('		body, img, form, p, a, table, tr, td {padding:0px; margin:0px}');
		output('		img.bug {width:24px; height:24px; float:left}');
		output('		p {color:black; font-size:medium; font-family:sans-serif}');
		output('		a {color:navy; font-size:medium; font-family:sans-serif; font-weight:bold}');
		output('		a.nav {background-color:#ffffff}');
		output('		br {font-size:small; font-weight:normal}');
//		output('		table {background-color:#ffffff; width:100%}');
//		output('		table, tr, td {border:0px}');
		output('		span.hilite {color:#400000; font-size:large; font-weight:bold}');
		output('		span.hilite2 {color:#400000; font-size:medium; font-weight:bold}');
		output('	</style>');
		output('	<title>' + Title + '</title>');
		output('</head>');
		output('<body style="background-color:#ff8080">');
	}

		// Build the body of the greeting
		output('<form action="index.asp" method="get">');
		output('<input type="hidden" name="pid" value="' + pid + '"/>');
		output('<input type="hidden" name="cid" value="sendform"/>');
		output('<p align="center">');
		output('	<img src="' + pname + '" alt=" Please wait..."/><br />');
		output('	<b>');
		var TextFileContents = GetFileContents(pid + '/text.txt');
		if (TextFileContents)
			output(TextFileContents + '<br/>');
		var bSending = (!bPickup && !bBadRequest);
		if (bSending)
		{
			output('	<br/>');
			output('	<input accesskey="1" type="submit" value="Continue to Send"/><br/>');
		}
		output('	</b>');
		output('	<br/>');
		if (!bPickup)
			outputReturnCategories();
		outputReturnHome();
		output('	<br/>');
		if (bPickup)
		{
			output('	<br/>');
			output('	<br/>');
			output('	<br/>');
		}
		output('</p>');
		output('</form>');

		var audio = pid + '/music.mid';
		if (FileExists(audio))
		{
			if (bWireless)
				output('<object data="' + audio + '" type="audio/midi"/>');
			else
			if (bWML)
				output('<audio src="' + audio + '" loop="infinite"/>');
			else
				output('<bgsound src="' + audio + '" loop="infinite"/>');
		}

	if (bWML)
	{
		output('</card>');
		output('</wml>');
	}
	else
	{
		output('</body>');
		output('</html>');
	}
%>
