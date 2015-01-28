<%@ Page Title="Enroll | DiscountPayroll.net" Language="C#" MasterPageFile="SiteSidebar2.Master" AutoEventWireup="true" CodeBehind="Enroll.aspx.cs" ClientIDMode="Static" Inherits="Payroll.Web.Enroll" %>

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
		<form id="_EnrollForm" method="post" action="#">
			<h1>Enroll in our online payroll service</h1>
			<p>Submit the form below so we can open your new payroll account.</p>
			<p>Remember: you won't pay anything until you run your first payroll.</p>
			<div>
				<fieldset class="rounded-corners">
					<legend>Business Information</legend>
					<div class="span-10 last">
						<label for="_BusinessType" class="span-5">Business Type</label>
						<select id="_BusinessType" name="_BusinessType" class="validate[required]">
							<option value="">Please Select</option>
							<option value="SB">Small business</option>
							<option value="HH">Household employer</option>
						</select>
					</div>
					<div class="span-10 last">
						<label for="_BizName" class="span-5">Business Name</label>
						<input id="_BizName" name="_BizName" class="validate[required,length[3,50]]" type="text" maxlength="50" />
					</div>
					<div class="span-10 last" style="margin-bottom: 0px;">
						<label for="_Address" class="span-5">Business Address</label>
						<input id="_Address" name="_Address" class="validate[required,length[6,50]]" type="text" maxlength="50" />
					</div>
					<div class="span-10 last" style="margin-top: -13px;">
						<label for="_Address2" class="span-5">(continued)</label>
						<input id="_Address2" name="_Address2" class="validate[length[0,50]]" type="text" maxlength="50" />
					</div>
					<div class="span-10 last">
						<label for="_City" class="span-5">City</label>
						<input id="_City" name="_City" class="validate[required,length[2,45]]" type="text" maxlength="45" />
					</div>
					<div class="span-10 last">
						<label for="_County" class="span-5">County</label>
						<input id="_County" name="_County" class="validate[length[0,50]]" type="text" maxlength="50" />
					</div>
					<div class="span-10 last">
						<label for="_State" class="span-5">State</label>
						<select id="_State" name="_State" class="validate[required]">
							<option value="">Please Select</option>
							<option value="AL">Alabama </option>
							<option value="AK">Alaska </option>
							<option value="AZ">Arizona </option>
							<option value="AR">Arkansas </option>
							<option value="CA">California </option>
							<option value="CO">Colorado </option>
							<option value="CT">Connecticut </option>
							<option value="DE">Delaware </option>
							<option value="DC">District of Columbia </option>
							<option value="FL">Florida </option>
							<option value="GA">Georgia </option>
							<option value="GU">Guam </option>
							<option value="HI">Hawaii </option>
							<option value="ID">Idaho </option>
							<option value="IL">Illinois </option>
							<option value="IN">Indiana </option>
							<option value="IA">Iowa </option>
							<option value="KS">Kansas </option>
							<option value="KY">Kentucky </option>
							<option value="LA">Louisiana </option>
							<option value="ME">Maine </option>
							<option value="MD">Maryland </option>
							<option value="MA">Massachusetts </option>
							<option value="MI">Michigan </option>
							<option value="MN">Minnesota </option>
							<option value="MS">Mississippi </option>
							<option value="MO">Missouri </option>
							<option value="MT">Montana </option>
							<option value="NE">Nebraska </option>
							<option value="NV">Nevada </option>
							<option value="NH">New Hampshire </option>
							<option value="NJ">New Jersey </option>
							<option value="NM">New Mexico </option>
							<option value="NY">New York </option>
							<option value="NC">North Carolina </option>
							<option value="ND">North Dakota </option>
							<option value="OH">Ohio </option>
							<option value="OK">Oklahoma </option>
							<option value="OR">Oregon </option>
							<option value="PR">Puerto Rico </option>
							<option value="PA">Pennsylvania </option>
							<option value="RI">Rhode Island </option>
							<option value="SC">South Carolina </option>
							<option value="SD">South Dakota </option>
							<option value="TN">Tennessee </option>
							<option value="TX">Texas </option>
							<option value="UT">Utah </option>
							<option value="VT">Vermont </option>
							<option value="VA">Virginia </option>
							<option value="WA">Washington </option>
							<option value="WV">West Virginia </option>
							<option value="WI">Wisconsin </option>
							<option value="WY">Wyoming </option>
						</select>
					</div>
					<div class="span-10 last">
						<label for="_ZipCode" class="span-5">Zip</label>
						<input id="_ZipCode" name="_ZipCode" class="validate[required,onlyNumber]" maxlength="5" type="text" />
					</div>
				</fieldset>
				<fieldset class="rounded-corners">
					<legend>Primary Contact Information</legend>
					<div class="span-10 last">
						<label for="_FirstName" class="span-5">First Name</label>
						<input id="_FirstName" name="_FirstName" class="validate[required,length[2,50],onlyLetter]" maxlength="50" type="text" />
					</div>
					<div class="span-10 last">
						<label for="_LastName" class="span-5">Last Name</label>
						<input id="_LastName" name="_LastName" class="validate[required,length[2,50],onlyLetter]" maxlength="50" type="text" />
					</div>
					<div class="span-10 last">
						<label for="_Phone" class="span-5">Phone</label>
						<input id="_Phone" name="_Phone" class="validate[required,telephone]" maxlength="15" type="text" />
					</div>
					<div class="span-10 last">
						<label for="_EmailAddr" class="span-5">Email Address</label>
						<input id="_EmailAddr" name="_EmailAddr" class="validate[required,length[0,80],email]" maxlength="80" type="text" />
					</div>
					<div class="span-10 last">
						<label for="_EmailAddr2" class="span-5">Re-enter Email Address</label>
						<input id="_EmailAddr2" name="_EmailAddr2" class="validate[required,length[0,80],email]" maxlength="80" type="text" />
					</div>
				</fieldset>
				<div id="_Error" class="error" style="display: none;"></div>
				<div>
					<br/>
					<input id="_Submit" value="Enroll at DiscountPayroll.net" type="submit" />
				</div>
			</div>
		</form>
		<div id="_Result" style="display: none;">
			<h1>Verify your email address</h1>
			<p>A verification email message has been sent to your inbox. 
			To protect your privacy, we require that you verify your email address before your new 
			payroll account can be created.</p>
			<ol>
				<li>Go to your inbox for <b><span id="_ResultValue"></span></b>.</li>
				<li>Open the verification message from <b>'DiscountPayroll.net Sales'</b> with the subject <b>'Email Verification'</b>.</li>
				<li>Click the link in the body of the email or follow the instructions.</li>
			</ol>
			<p>If you do not receive the verification email message, please check the following:</p>
			<ul>
				<li><b>Check the email address</b><br/>
				Make sure that you have typed in the correct email address. If you have made a mistake, please <a href="Enroll.aspx">re-submit your enrollment form</a> and specify your correct email address.
				</li>
				<li><b>Check your junk mail folder</b><br/>
				Make sure that the verification email did not get filtered into the junk mail folder of your email application.
				</li>
				<li><b>Check if blocked by mail server</b><br/>
				In some cases emails may be accidentally blocked by anti-spam technology used by your email provider. Contact them and ask them if they are blocking email from 'DiscountPayroll.net'.
				</li>
			</ul>
		</div>
		<div id="_Dialog" title="Dialog Title"></div>
		<div id="_ProgressBarDialog">
			<div id="_ProgressBar"></div>
		</div>
	</div>
</asp:Content>
