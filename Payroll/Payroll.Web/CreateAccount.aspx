<%@ Page Title="Create Account | DiscountPayroll.net" Language="C#" MasterPageFile="SiteSidebar2.Master" AutoEventWireup="True" CodeBehind="CreateAccount.aspx.cs" ClientIDMode="Static" Inherits="Payroll.Web.CreateAccount" %>

<asp:Content ContentPlaceHolderID="x_HeadContent" runat="server">
<%--
	<link rel="stylesheet" type="text/css" href="validation/css/validationEngine.jquery.css" />
	<script type="text/javascript" src="validation/js/jquery.validationEngine-en.js"></script>
	<script type="text/javascript" src="validation/js/jquery.validationEngine.js"></script>
--%>
	<script type="text/javascript" src="js/input_validators.js"></script>
	<script type="text/javascript" src="js/Enroll.js"></script>
	<style type="text/css">
		.ui-progressbar-value
		{
			background: url(/jquery/ui/css/greenshades/images/ui-bg_highlight-hard_15_459e00_1x100.png);
		}
		label {
			color:#555;
			padding-right:0;
			text-align:right;
		}
		legend 
		{
			margin-bottom: 0px;
		}
		fieldset 
		{
			margin-top: 5px;
			margin-bottom: 0px;
			padding-top: 0px;
			padding-bottom: 0px;
		}
		select 
		{
			border-color: #bbb;
		}
		select:focus
		{
			border-color:#666;
		}
		.rounded-corners {
			-moz-border-radius: 10px;/*Firefox*/
			-webkit-border-radius: 10px;/*Chrome/Safari*/
			border-radius: 10px;/*CSS3*/
			/*behavior: url(/css/border-radius.htc);*//*IE*/
		}
	</style>
</asp:Content>

<asp:Content ContentPlaceHolderID="x_BodyContent" runat="server">
	<div class="scale9Grid">
		<form id="_CreateAccountForm" method="post" action="#">
			<h1>Your email address has been verified</h1>
			<p>Select a user id to login to the secure payroll site, and your new 
			payroll administrator account will be created.</p>
			<p>Remember: you won't pay anything until you run your first payroll.</p>
			<div>
				<fieldset class="rounded-corners">
					<legend>Login Information</legend>
					<div class="span-10 last">
						<label for="_UserId" class="span-5">User ID</label>
						<input id="_UserId" name="_UserId" class="validate[required]" type="text" />
					</div>
				</fieldset>
				<div>
					<input id="_BusinessType" name="_BusinessType" type="hidden" value="<%= RequestQuery("BusinessType") %>" />
					<input id="_BizName" name="_BizName" type="hidden" value="<%= RequestQuery("BizName") %>" />
					<input id="_Address" name="_Address" type="hidden" value="<%= RequestQuery("Address") %>" />
					<input id="_Address2" name="_Address2" type="hidden" value="<%= RequestQuery("Address2") %>" />
					<input id="_City" name="_City" type="hidden" value="<%= RequestQuery("City") %>" />
					<input id="_County" name="_County" type="hidden" value="<%= RequestQuery("County") %>" />
					<input id="_State" name="_State" type="hidden" value="<%= RequestQuery("State") %>" />
					<input id="_ZipCode" name="_ZipCode" type="hidden" value="<%= RequestQuery("ZipCode") %>" />
					<input id="_FirstName" name="_FirstName" type="hidden" value="<%= RequestQuery("FirstName") %>" />
					<input id="_LastName" name="_LastName" type="hidden" value="<%= RequestQuery("LastName") %>" />
					<input id="_Phone" name="_Phone" type="hidden" value="<%= RequestQuery("Phone") %>" />
					<input id="_EmailAddr" name="_EmailAddr" type="hidden" value="<%= RequestQuery("EmailAddr") %>" />
				</div>
				<div id="_Error" class="error" style="display: none;"></div>
				<div>
					<br/>
					<input id="_Submit" value="Create your account" type="submit" />
				</div>
			</div>
		</form>
		<div id="_Result" style="display: none;">
			<h1>Welcome to DiscountPayroll.net!</h1>
			<p>Your new payroll administrator account has been created, and you can 
			<a href='Payroll.aspx'>Access Your Payoll</a> here.
			</p>
			<p>Your account details are as follows:
			</p>
			<p>User Id: <b><span id="_ResultValue"></span></b><br/>
			Temporary password: <b>password1</b>
			</p>
			<p>Once logged in, you will be prompted to select your own personal password,
			and you will be prompted to run through a streamlined setup of your new online payroll account.
			</p>
			<p>If you wish to change your User Id at any point, you can do it from the Setup menu.
			</p>
			<p>If you have any questions, please email our support staff at <a href='mailto:support@discountpayroll.net'>support@discountpayroll.net</a>.
			</p>
		</div>
		<div id="_Dialog" title="Dialog Title"></div>
		<div id="_ProgressBarDialog">
			<div id="_ProgressBar"></div>
		</div>
	</div>
</asp:Content>
