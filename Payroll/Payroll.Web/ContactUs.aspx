<%@ Page Title="Contact Us | DiscountPayroll.net" Language="C#" MasterPageFile="SiteSidebar2.Master" AutoEventWireup="true" CodeBehind="ContactUs.aspx.cs" ClientIDMode="Static" Inherits="Payroll.Web.ContactUs" %>

<asp:Content ContentPlaceHolderID="x_HeadContent" runat="server">
	<script type="text/javascript">
		// This script is for the tabbed panels - See http://docs.jquery.com/UI/Tabs for documentation
		$(function()
		{
			$('#tabs1').tabs();
			$('#tabs2').tabs();
			$('#tabs3').tabs();
			$('#tabs4').tabs();
		});
	</script>
</asp:Content>

<asp:Content ContentPlaceHolderID="x_BodyContent" runat="server">
	<div class="scale9Grid">
		<h1>Contact Us</h1>
		<h3>Thank you for your interest in us.</h3>
		<p>Our contact info is as follows:</p>
		<div id="tabs1">
			<ul>
				<li><a href="#tabs1-1">Sales</a></li>
			</ul>
			<div id="tabs1-1">
				<p>
					If you would like to enroll in our payroll service now, please fill out our 
					<a href="Enroll.aspx">get started form</a>.&nbsp; If you have 
					questions about the service, please contact us at 
					<a href="mailto:Sales@DiscountPayroll.net">Sales@DiscountPayroll.net</a>.
				</p>
			</div>
		</div>
		<br/>
		<div id="tabs2">
			<ul>
				<li><a href="#tabs2-1">Support</a></li>
			</ul>
			<div id="tabs2-1">
				<p>
					If you are a current customer and have questions about the service, please contact us at 
					<a href="mailto:Support@DiscountPayroll.net">Support@DiscountPayroll.net</a>.
				</p>
			</div>
		</div>
		<br/>
		<div id="tabs3">
			<ul>
				<li><a href="#tabs3-1">Billing</a></li>
			</ul>
			<div id="tabs3-1">
				<p>
					If you are a current customer and have questions about billing, please contact us at 
					<a href="mailto:Billing@DiscountPayroll.net">Billing@DiscountPayroll.net</a>.
				</p>
			</div>
		</div>
		<br/>
		<div id="tabs4">
			<ul>
				<li><a href="#tabs4-1">DiscountPayroll.net</a></li>
			</ul>
			<div id="tabs4-1">
				<p>
					Andover, MA 01810<br/>
					Phone: 505-695-2228
				</p>
			</div>
		</div>
	</div>
</asp:Content>
