<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ Page Language="C#" CodeFile="Congrats.aspx.cs" Inherits="Congrats" %>

<html>
	<head>
		<title>Congratulations!</title>
	</head>

	<body>
		An account has been created for username <%= Page.User.Identity.Name.ToString() %>.<p/>
		<a href="Default.aspx">Let's Get Started!</a>
	</body>
</html>
