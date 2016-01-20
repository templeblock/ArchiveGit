<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ Page Language="C#" CodeFile="Register.aspx.cs" Inherits="Test.Register" %>

<html>
	<head>
		<title>Register for a new account</title>

    <style type="text/css">
	    .redtext {font:8pt Verdana; color:#ff0000;}
	    .headertext {font:10pt Arial;}
	    .label {font:10pt Arial;width:120;}
	    .zip {font:10pt Arial;}
	    .data {width:220;}
    </style>

	</head>

	<body style="margin:10,10,10,10">
		<form runat="server">
			<table border="0" width="100%" cellspacing="0" cellpadding="0">
				<tr align="left">
					<td class="headertext">
						Please complete the following required fields to create a new user profile. 
						When you are finished, click the "Create Account" button to proceed.<br/><br/>
					</td>
				</tr>
			</table>
			<table border="0" width="100%">
				<tr>
					<td colspan="3">
						<table border="0" cellpadding="0" cellspacing="0" width="100%">
							<tr>
								<td class="headertext">
									<b>Sign-In Information</b><i> (Required)</i>
								</td>
							</tr>
						</table>
					</td>
				</tr>
				<tr>
					<td align="right" class="label">
						Email Address
					</td>
					<td class="data">
						<asp:textbox id="ctlEmailAddress" width="200px" maxlength="255" runat="server" />
					</td>
					<td width="*">
						<asp:label id="ctlErrorMsg" text="" enableviewstate="false" visible="false" CssClass="redtext" runat="server" />
						<asp:requiredfieldvalidator id="ctlEmailAddressReqVal" controltovalidate="ctlEmailAddress" display="Dynamic" CssClass="redtext" runat="server">
							An email address is required
						</asp:requiredfieldvalidator>
						<asp:regularexpressionvalidator id="EmailAddressRegExValidator" controltovalidate="ctlEmailAddress"
							validationexpression="\S+@\S+\.\S{2,3}" display="Dynamic" CssClass="redtext" runat="server">
							Invalid email address
						</asp:regularexpressionvalidator>
					</td>
				</tr>
				<tr>
					<td align="right" class="label">
						Password
					</td>
					<td class="data">
						<asp:textbox id="ctlPassword" textmode="password" maxlength="20" runat="server" />
					</td>
					<td width="*">
						<asp:requiredfieldvalidator id="ctlPasswordReqVal" controltovalidate="ctlPassword" errormessage="Password" display="Dynamic" CssClass="redtext" runat="server">
							A password is required
						</asp:requiredfieldvalidator>
						<asp:regularexpressionvalidator id="PasswordRegExValidator" controltovalidate="ctlPassword"
							validationexpression="\w{4,20}" display="Dynamic" CssClass="redtext" runat="server">
							Passwords must be 4-20 characters long
						</asp:regularexpressionvalidator>
					</td>
				</tr>
				<tr>
					<td align="right" class="label">
						Re-enter Password
					</td>
					<td class="data">
						<asp:textbox id="ctlPassword2" textmode="password" maxlength="20" runat="server" />
					</td>
					<td width="*">
						<asp:requiredfieldvalidator id="ctlPassword2ReqVal" controltovalidate="ctlPassword2" errormessage="Password" display="Dynamic" CssClass="redtext" runat="server">
							This field is required
						</asp:requiredfieldvalidator>
						<asp:comparevalidator id="CompareValidator1" controltovalidate="ctlPassword2" controltocompare="ctlPassword" errormessage="Re-enter Password" display="Dynamic" CssClass="redtext" runat="server">
							The password fields don't match
						</asp:comparevalidator>
					</td>
				</tr>
				<tr>
					<td colspan="3">&nbsp;</td>
				</tr>
				<!-- personalization information -->
				<tr>
					<td colspan="3">
						<table border="0" cellpadding="0" cellspacing="0" width="100%">
							<tr>
								<td class="headertext"><b>Personal Information</b><i>   (Optional)</i>
								</td>
							</tr>
						</table>
					</td>
				</tr>
				<tr>
					<td align="right" class="label">
						First Name
					</td>
					<td class="data">
						<asp:textbox id="ctlFirstName" maxlength="20" width="200px" runat="server" />
					</td>
					<td>
					</td>
				</tr>
				<tr>
					<td align="right" class="label">
						Last Name
					</td>
					<td class="data">
						<asp:textbox id="ctlLastName" maxlength="20" width="200px" runat="server" />
					</td>
					<td>
					</td>
				</tr>
				<tr>
					<td align="right" class="label">
						Address
					</td>
					<td class="data">
						<asp:textbox id="ctlAddress" maxlength="50" width="200px" runat="server" />
					</td>
					<td>
					</td>
				</tr>
				<tr>
					<td align="right" class="label">
						City
					</td>
					<td class="data">
						<asp:textbox id="ctlCity" maxlength="50" width="200px" runat="server" />
					</td>
					<td>
					</td>
				</tr>
				<tr>
					<td align="right" class="label">
						State
					</td>
					<td class="data">
						<asp:textbox id="ctlState" width="40px" maxlength="2" runat="server" />
						<span class="zip">&nbsp;Zip&nbsp;</span>
						<asp:textbox id="ctlZip" width="75px" maxlength="5" runat="server" />
					</td>
					<td width="*">
						<asp:regularexpressionvalidator id="RegExValidator1" controltovalidate="ctlZip"
							validationexpression="\d{5}" display="Dynamic" CssClass="redtext" runat="server">
							Zip code must be 5 digits
						</asp:regularexpressionvalidator>
					</td>
				</tr>
				<tr>
					<td align="right" class="label">
						Phone
					</td>
					<td class="data">
						<asp:textbox id="ctlPhone" maxlength="20" runat="server" />
					</td>
				</tr>
			</table>
			<br>
			<table width="100%">
				<tr>
					<td style="padding:15,15,15,15;border-color:black;border-style:solid;border-width:1;border-right:0;border-left:0">
						<input type="submit" value="Create Account" onserverclick="Create_User" id="Submit1" runat="server" /><br/>
						<a href="Login.aspx">Login</a><br/>
						<a href="Default.aspx">Home</a><br/>
					</td>
				</tr>
			</table>
		</form>
	</body>
</html>
