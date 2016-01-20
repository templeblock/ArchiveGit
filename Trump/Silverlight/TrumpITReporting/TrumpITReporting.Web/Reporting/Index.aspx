<?xml version="1.0" encoding="utf-8"?>
<%@ Page Language="C#" CodeBehind="Index.aspx.cs" Inherits="TrumpITReporting.Web.Index" AutoEventWireup="True" EnableViewState="false" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
	<title>TrumpITReporting</title>
	<meta http-equiv="X-UA-Compatible" content="IE=8"></meta>
	<meta name="description" content="The easiest way to manage your business online."></meta>
	<meta name="keywords" content="Trump,Reporting"></meta>
	<style type="text/css">
		html
		{
			height: 100%;
			overflow: auto;
			margin: 0px;
		}
		body
		{
			height: 100%;
			overflow: hidden;
			font-family: Trebuchet MS;
			color: #817C90;
			padding: 0;
			margin: 0;
		}
		#silverlightControlHost
		{
			height: 100%;
			text-align: left;
		}
		#silverlightNotInstalled
		{
			height: 100%;
			overflow: auto;
			padding-top: 25px;
			padding-left: 50px;
		}
	</style>
	<asp:Literal id="x_HeadContent" runat="server" />
	<%--<asp:ContentPlaceHolder ID="x_HeadContent" runat="server" />--%>
	<script type="text/javascript" src="SplashScreen.js"></script>
	<script type="text/javascript" src="Silverlight.js"></script>
	<script type="text/javascript">
		function onSilverlightError(sender, args) {}
	</script>
</head>
<body>
	<form id="form1" runat="server" style="height:100%;">
		<asp:ScriptManager runat="server" EnablePageMethods="true" />
		<div id="silverlightControlHost">
			<object data="data:application/x-silverlight-2," type="application/x-silverlight-2" width="100%" height="100%">
				<param name="enableautozoom" value="true"/>
				<param name="source" value="ClientBin/TrumpITReporting.xap" />
				<%--<param name="onError" value="onSilverlightError" />--%>
				<param name="background" value="White" />
				<param name="minRuntimeVersion" value="3.0.40624.0" />
				<param name="autoUpgrade" value="true" />
				<param name="initParams" value="a=1,b=2" /><%--Comma separated name=value pairs--%>
				<param name="splashScreenSource" value="SplashScreen.xaml" />
				<param name="onSourceDownloadProgressChanged" value="onSourceDownloadProgressChanged" />
				<param name="onSourceDownloadComplete" value="onSourceDownloadComplete" />
				<param name="windowless" value="true" />
				<div id="silverlightNotInstalled">
					<h3>TrumpITReporting. &nbsp; The easiest way to manage your business online.</h3>
					<h4>For the full experience, install the latest version of Microsoft Silverlight 
					<a href="http://go.microsoft.com/fwlink/?LinkID=149156&amp;v=3.0.40624.0">
						here</a>.</h4>
					<a href="http://go.microsoft.com/fwlink/?LinkID=149156&amp;v=3.0.40624.0" style="text-decoration: none">
						<img width="110" src="http://go.microsoft.com/fwlink/?LinkId=108181" alt="Microsoft Silverlight" style="border-style:none;" />
					</a>
					<asp:ListView id="x_Links" runat="server">
						<LayoutTemplate>
							<ul><asp:PlaceHolder id="itemPlaceholder" runat="server" /></ul>
						</LayoutTemplate>
						<ItemTemplate>
							<li><span><a href="app<%# Eval("Command")%>"><%# Eval("Name")%></a></span></li>
						</ItemTemplate>
					</asp:ListView>
					<asp:Literal id="x_BodyContent" runat="server" />
					<%--<asp:ContentPlaceHolder ID="x_BodyContent" runat="server" />--%>
					<img height="200" src="Images/Spacer.gif" alt="" style="border-style:none;" />
				</div>
			</object>
			<iframe id="_sl_historyFrame" style="visibility: hidden; height: 0px; width: 0px; border: 0px"></iframe>
		</div>
	</form>
</body>
</html>
