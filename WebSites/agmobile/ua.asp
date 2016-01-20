<%@ Language=JavaScript %>
<% Response.ContentType = 'text/html' %>
<% 
	function output(strText)
	{
		Response.write(strText + '\n');
	}
	
	output(Request.Cookies() + ' a<br/>');
	output(Request.Cookies('mycookie').HasKeys + ' b<br/>');
	output(Request.Cookies('mycookie') + ' c<br/>');
	output(Request.Cookies('mycookie')('two') + ' d<br/>');
	output(Request.Cookies('mycookie')('three') + ' e<br/>');


	var Item;
	for (Item in Request.Cookies())
	{
		Response.write(Item + 's<br/>');
		var item;
		for (item in Request.Cookies)
		{
			if (Request.Cookies[item].haskeys)
			{
				Response.write(item + ' has the following keys: <br/>');
				var subitem;
				for (subitem in Request.Cookies[item])
					Response.write(subitem + ' = ' + Request.Cookies[item][subitem] + '<br/>');
			}
			else
				Response.write(item + ' = ' + Request.Cookies[item]);
		}
	}

	Response.Cookies('mycookie')('two') = 'yesterday';
	Response.Cookies('mycookie')('three') = 'tomorrow';
	Response.Cookies('mycookie').Expires = "July 31, 2003";

	var strAgent = String(Request.servervariables('HTTP_USER_AGENT'));
	var strAccept = String(Request.servervariables('ALL_HTTP'));

// XHTML-MP-WAP
//	output('<?xml version="1.0" encoding="UTF-8"?>');
//	output('<!DOCTYPE html PUBLIC "-//WAPFORUM//DTD XHTML Mobile 1.0//EN" "http://www.wapforum.org/DTD/xhtml-mobile10.dtd">');
//	output('<html xmlns="http://www.w3.org/1999/xhtml">'); // Doesn't work with ie
// XHTML 1.0
//	output('<?xml version="1.0"?>');
//	output('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "DTD/xhtml1-strict.dtd">');
//	output('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">'); // Doesn't work with ie
	output('<html>');
	output('<head>');
	output('	<title>AG UA</title>');
	output('</head>');
	output('<body>');

	output('<p align="center" mode="wrap">');
	output('	<strong>HTTP_USER_AGENT:</strong><br/>');
	output(strAgent);
	output('	<br/><br/>');
	output('	<strong>ALL_HTTP:</strong><br/>');
	output(strAccept);
	output('</p>');

	output('</body>');
	output('</html>');
%>
