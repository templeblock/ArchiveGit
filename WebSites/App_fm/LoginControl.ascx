<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ Language="C#" CodeFile="LoginControl.cs" Inherits="LoginControl" %>

<table width="205" cellpadding=5 cellspacing=0 style="font: 10pt Arial">
	<tr bgcolor="<%= Session["HeadColor"]%>">
		<td align="left" height="25" style="border-color:black;border-style:solid; border-width:1;">
			<b>Please Login</b>
		</td>
	</tr>
	<tr bgcolor="<%= Session["LeftColor"]%>">
		<td align="center" height="25" style="border-color:black;border-style:solid; border-width:1; border-top:0;">
			<table width="100%">
				<tr>
					<td>Email Address:</td>
					<td><asp:textbox id="ctlEmailAddress" width="200px" maxlength="255" runat="server" /> </td>
				</tr>
				<tr>
					<td>Password:</td>
					<td><asp:textbox id="ctlPassword" textmode="password" width="200px" maxlength="20" runat="server" /></td>
				</tr>
				<tr>
					<asp:panel id="panelPassword2" visible="false" runat="server">
						<td>Re-enter Password:</td>
						<td><asp:textbox id="ctlPassword2" textmode="password" width="200px" maxlength="20" runat="server" /></td>
					</asp:panel>
				</tr>
				<tr>
					<td colspan="2" style="color:black;font:8pt verdana, arial">
						<asp:label id="ctlErrorMsg" text="" enableviewstate="false" visible="false" runat="server" />
						<asp:requiredfieldvalidator id="ctlEmailAddressReqVal" controltovalidate="ctlEmailAddress" display="Dynamic" runat="server">
							An email address is required
						</asp:requiredfieldvalidator>
						<asp:regularexpressionvalidator id="EmailAddressRegExValidator" controltovalidate="ctlEmailAddress"
							validationexpression="\S+@\S+\.\S{2,3}" display="Dynamic" runat="server">
							Invalid email address
						</asp:regularexpressionvalidator>
					</td>
				</tr>
				<tr>
					<td colspan="2" >
						<input type="submit" value="Sign In" onserverclick="LoginBtn_Click" runat="server" />
						<input type="submit" value="I Forgot my password" onserverclick="ForgotBtn_Click" runat="server" />
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center">
						<a href="Register.aspx"><span style="color:black;font:8pt verdana, arial" >Create a new account</span></a>
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table> 
