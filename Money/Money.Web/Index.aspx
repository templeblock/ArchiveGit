<?xml version="1.0" encoding="utf-8"?>
<%@ Page Language="C#" CodeBehind="Index.aspx.cs" Inherits="Money.Web.Index" AutoEventWireup="True" EnableViewState="false"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
	<title>YinYangMoney</title>
	<meta http-equiv="X-UA-Compatible" content="IE=8"></meta>
	<meta name="description" content="The easiest way to build a financial plan online."></meta>
	<meta name="keywords" content="money,easy,retirement,planning,personal,financial,advisor,balance,budgeting,software,calculators,goals,spending,income,investments,saving,future,life,lifetime,flow,free,online,tool,yin,yang,ying"></meta>
	<!-- Google Webmaster Tools site verification-->
	<meta name="google-site-verification" content="bcZkfeVYjxvyuFXT831Zr56Ee_GgTbiZcaBKVyEr77M"></meta>
	<!-- Bing Webmaster Tools site verification-->
	<meta name="msvalidate.01" content="419FBC9C538DB5B6E230350E69C7C25D"></meta>
	<!-- Yahoo Webmaster Tools site verification-->
	<meta name="y_key" content="d969e99248989562"></meta>
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

	<%-- Google Analytics --%>
	<script type="text/javascript">
		var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
		document.write(unescape("%3Cscript src='" + gaJsHost + "google-analytics.com/ga.js' type='text/javascript'%3E%3C/script%3E"));
	</script>
	<script type="text/javascript">
		try
		{
			var pageTracker = _gat._getTracker("UA-11388284-1");
			pageTracker._trackPageview();
		} catch (err) { }
	</script>

	<%--
	<script type="text/javascript">
		function onSilverlightLoad(sender, args)
		{
		}

		function onSilverlightError(sender, args)
		{
			var appSource = "";
			if (sender != null && sender != 0)
				appSource = sender.getHost().Source;
			
			var errorType = args.ErrorType;
			var iErrorCode = args.ErrorCode;

			if (errorType == "ImageError" || errorType == "MediaError")
				return;

			var errMsg = "Unhandled Error in Silverlight Application " + appSource + "\n" ;
			errMsg += "Code: "+ iErrorCode + "\n";
			errMsg += "Category: " + errorType + "\n";
			errMsg += "Message: " + args.ErrorMessage + "\n";

			if (errorType == "ParserError")
			{
				errMsg += "File: " + args.xamlFile + "\n";
				errMsg += "Line: " + args.lineNumber + "\n";
				errMsg += "Position: " + args.charPosition + "\n";
			}
			else
			if (errorType == "RuntimeError")
			{
				if (args.lineNumber != 0)
				{
					errMsg += "Line: " + args.lineNumber + "\n";
					errMsg += "Position: " + args.charPosition + "\n";
				}
				errMsg += "MethodName: " + args.methodName + "\n";
			}

			throw new Error(errMsg);
		}
	</script>
	--%>
</head>
<body>
	<form id="form1" runat="server" style="height:100%;">
		<asp:ScriptManager runat="server" EnablePageMethods="true" />
		<div id="silverlightControlHost">
			<object data="data:application/x-silverlight-2," type="application/x-silverlight-2" width="100%" height="100%">
				<param name="source" value="ClientBin/YinYangMoney.xap" />
				<param name="initParams" value="a=1,b=2" /><%--Comma separated name=value pairs--%>
				<param name="background" value="White" />
				<param name="minRuntimeVersion" value="4.0.50826.0" />
				<param name="autoUpgrade" value="true" />
				<param name="windowless" value="false" />
				<param name="EnableGPUAcceleration" value="true" />
				<param name="splashScreenSource" value="SplashScreen.xaml" />
				<param name="onSourceDownloadProgressChanged" value="onSourceDownloadProgressChanged" />
				<param name="onSourceDownloadComplete" value="onSourceDownloadComplete" />
				<%--<param name="onError" value="onSilverlightError" />--%>
				<%--<param name="onLoad" value="onSilverlightLoad" />--%>
				<div id="silverlightNotInstalled">
					<h3>YinYangMoney. &nbsp; The easiest way to build a financial plan online.</h3>
					<h4>For the full experience, install the latest version of Microsoft Silverlight 
					<a href="http://go.microsoft.com/fwlink/?LinkID=149156&amp;v=4.0.50826.0">
						here</a>.</h4>
					<a href="http://go.microsoft.com/fwlink/?LinkID=149156&amp;v=4.0.50826.0" style="text-decoration: none">
						<img width="110" src="http://go.microsoft.com/fwlink/?LinkId=161376" alt="Microsoft Silverlight" style="border-style: none;" />
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
