<?xml version="1.0" encoding="utf-8"?>
<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="TrumpBubbles.Web._Default" EnableViewState="false" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
	<title>TrumpBubbles</title>
	<meta http-equiv="X-UA-Compatible" content="IE=8"></meta>
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
			padding: 0;
			margin: 0;
		}
	</style>

	<script type="text/javascript" src="Silverlight.js"></script>
	<script type="text/javascript">
	    function onSourceDownloadProgressChanged(sender, eventArgs) {
	        sender.findName("progressText").Text = Math.round((eventArgs.progress * 100)) + "% downloaded ...";
	        sender.findName("progressBar").Width = eventArgs.progress * sender.findName("progressBarBackground").Width;
	    }
	</script>
</head>
<body style="background: #DDC195;">
	<%--<form runat="server" style="height: 100%;">--%>
	<div style="height:100%; text-align: left;">
		<object data="data:application/x-silverlight-2," type="application/x-silverlight-2" width="100%"
			height="835"><%--275 pixels per normal bubble - height should be tall enough for the rows of normal bubbles and will be changed dynamically--%>
			<param name="initParams" value="ViewPage=/DefaultHome" /><%--Comma separated name=value pairs--%>
			<param name="background" value="#DDC195" />
			<param name="source" value="ClientBin/TrumpBubbles.xap" />
			<param name="minRuntimeVersion" value="3.0.40624.0" />
			<param name="autoUpgrade" value="true" />
			<param name="windowless" value="true" />
			<param name="splashscreensource" value="Loading.xaml" />
			<param name="onsourcedownloadprogresschanged" value="onSourceDownloadProgressChanged" />
			
			<%--<param name="onError" value="onSilverlightError" />--%>
			<div>
				<h3>Trump network Backoffice. &nbsp; The easiest way to manage your business online.</h3>
				<h4>For the full experience, install the latest version of Microsoft Silverlight 
				<a href="http://go.microsoft.com/fwlink/?LinkID=149156&amp;v=3.0.40624.0">
					here</a>.</h4>
				<a href="http://go.microsoft.com/fwlink/?LinkID=149156&amp;v=3.0.40624.0" style="text-decoration: none">
					<img width="110" src="http://go.microsoft.com/fwlink/?LinkId=108181" alt="Microsoft Silverlight" style="border-style:none;" />
				</a>
			</div>
		</object>
		<iframe id="_sl_historyFrame" style="visibility: hidden; height: 0px; width: 0px; border: 0px"></iframe>
	</div>
	<%--</form>--%>
</body>
</html>
