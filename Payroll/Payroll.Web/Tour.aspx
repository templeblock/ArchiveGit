<%@ Page Title="Quick Tour | DiscountPayroll.net" Language="C#" MasterPageFile="SiteSidebar1.Master" AutoEventWireup="true" CodeBehind="Tour.aspx.cs" ClientIDMode="Static" Inherits="Payroll.Web.Tour" %>

<asp:Content ContentPlaceHolderID="x_HeadContent" runat="server">
</asp:Content>

<asp:Content ContentPlaceHolderID="x_BodyContent" runat="server">
	<div class="span-19 last">
		<h1>See how our payroll service works</h1>
		<object
				classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" 
				codebase="https://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" 
				style="width:100%; height:400px;">
			<param name="movie" value="/swf/acclientflashdemo.swf" />
			<param name="quality" value="high" />
			<param name="wmode" value="opaque" />
		<!--[if !IE]>-->
			<object
					type="application/x-shockwave-flash" 
					data="/swf/acclientflashdemo.swf" 
					style="width:100%; height:400px;">
				<param name="movie" value="/swf/acclientflashdemo.swf" />
				<param name="quality" value="high" />
				<param name="wmode" value="opaque" />
		<!--<![endif]-->
				<div>
					<p><a href="http://www.macromedia.com/go/getflashplayer"><img src="http://www.macromedia.com/images/shared/download_buttons/get_flash_player.gif" alt="Get macromedia Flash Player" style="border: none;" /></a></p>
				</div>
		<!--[if !IE]>-->
			</object>
		<!--<![endif]-->
		</object>
	</div>
</asp:Content>
