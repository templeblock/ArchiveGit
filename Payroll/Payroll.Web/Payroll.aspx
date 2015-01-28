<%@ Page Title="Payroll | DiscountPayroll.net" Language="C#" MasterPageFile="SiteBase.Master" AutoEventWireup="true" CodeBehind="Payroll.aspx.cs" ClientIDMode="Static" Inherits="Payroll.Web.Payroll" %>

<asp:Content ContentPlaceHolderID="x_HeadContent" runat="server">
	<style type="text/css">
		body
		{
			overflow: hidden;
		}
		.frameReset
		{
			position: absolute;
			margin: 0;
			padding: 0;
			border: 0;
			outline: 0;
			margin-top: -40px;
			width: 100%;
			height: 0px;
			z-index: -100;
			/*overflow: scroll;*/
		}
		.frameTint
		{
			/*background: #cfc;*/
			background: transparent;
		}
	</style>
	<script type="text/javascript">
		function OnBodyLoad()
		{
			//OnFrameLoad();
			window.onresize = OnWindowResize;
			OnWindowResize();
		}

		function OnWindowResize()
		{
			var newHeight = $(window).height();
			$('iframe').css('height', newHeight); // this will apply to all iframes on the page

			var newWidth = $(window).width();
			$('#header').css('width', newWidth - 16); // subtract the width of a scrollbar
		}
	</script>
	<script type="text/javascript">
//		function OnFrameLoad()
//		{
//			setTimeout("OnTimerTick()", 1000);

//			var frame = document.getElementById("_frame");
//			if (frame == null)
//				return;
//			var frameDocument = frame.document;
//			if (frameDocument == null)
//				return;
//			//frameDocument.attachEvent("onreadystatechange", OnFrameDocUnload);
//			//document.attachEvent("onreadystatechange", OnFrameDocUnload);
//			frameDocument.onreadystatechange = OnFrameDocUnload;
//			document.onreadystatechange = OnFrameDocUnload;
//		}

//		function OnTimerTick()
//		{
//			var frame = document.getElementById("_frame");
//			if (frame == null)
//				return;
//			if (frame.readyState == "loading")
//				return;
//			var frameDocument = frame.document;
//			if (frameDocument == null)
//				return;
//			//document.title = "hey!";
//		}

//		function OnFrameDocLoad()
//		{
//			alert("OnFrameDocLoad");
//		}

//		function OnFrameDocUnload()
//		{
//			alert("OnFrameDocUnload");
//		}
//	</script>
</asp:Content>

<asp:Content ContentPlaceHolderID="x_BodyHeader" runat="server">
	<div id="header">
		<div class="container">
			<div class="span-24 last">
				<a href="/">
					<img src="themes/greenshades/images/logo.png" style="width:385px; height:78px;" alt="Home" />
				</a>
				<div id="nav">
					<ul class="sf-menu sf-shadow">
						<li><a href="/">Home</a> </li>
						<li><a href="#">Quick Links</a>
							<ul>
								<li><a href="Benefits.aspx">Features and Benefits</a></li>
								<li><a href="Enroll.aspx">Enroll</a></li>
								<li><a href="ContactUs.aspx">Contact Us</a></li>
								<li><a href="Letter.aspx">Letter from the Founder</a></li>
								<li><a href="Tour.aspx">Take a Quick Tour!</a></li>
								<li><a href="Payroll.aspx?cmd=/sample/createSBASample.jsp">Take a Test Drive!</a></li>
								<%--<li><a href="FAQ.aspx">FAQ</a></li>--%>
								<%--<li><a href="Conversion.aspx">Conversion</a></li>--%>
							</ul>
						</li>
						<li><a href="Payroll.aspx?cmd=/login/wholesaleClientLogout.jsp">Log Out</a></li>
					</ul>
				</div>
			</div>
		</div>
	</div>
</asp:Content>

<asp:Content ContentPlaceHolderID="x_BodyContent" runat="server">
	<div style="background: transparent;">
		<%-- For future reference, the magic command path is: /in/todo/?Login.x=0&Login.y=0&hss=1 --%>
		<iframe id="_frame" class="frameReset frameTint" src="https://www.managepayroll.com/in/todo/default.jsp" frameborder="0" scrolling="yes" runat="server">Must not be empty</iframe>
	</div>
</asp:Content>
