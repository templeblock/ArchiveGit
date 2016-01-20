<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ Page Language="C#" CodeFile="default.aspx.cs" Inherits="Mine.MyDefault" %>

<html>
	<head>
		<title>Home</title>
	</head>

	<body>
		<h1>Hello everyone, <% =DateTime.Now.ToString() %></h1><p/>

		<asp:PlaceHolder id="ctlLoggedIn" visible="false" runat="server">
			Hello <% =Page.User.Identity.Name %><br/>
			<a href="~/Logout.aspx">Logout</a><br/>
		</asp:PlaceHolder>

		<asp:PlaceHolder id="ctlNotLoggedIn" visible="false" runat="server">
			<a href="/Login.aspx">Login</a><br/>
			<a href="Register.aspx">Create a new account</a><br/>
		</asp:PlaceHolder>

	</body>
</html> 
