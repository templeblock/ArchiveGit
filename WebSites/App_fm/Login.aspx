<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ Page Language="C#" CodeFile="Login.aspx.cs" Inherits="Login" %>
<%@ Register Src="LoginControl.ascx" TagPrefix="MyCommon" TagName="LoginControl" %>

<html>
	<head>
		<title>Login</title>
	</head>

	<body style="margin:0,0,0,0">
		<table border=0 width="100%" cellspacing=5 cellpadding=0 style="padding:5,0,0,0">
			<tr>
				<td>
					<form runat="server">
                        <MyCommon:LoginControl runat="server" />
                   	</form>
				</td>
			</tr>
		</table>
	</body>
</html>
