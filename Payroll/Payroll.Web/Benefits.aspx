<%@ Page Title="Features and Benefits | DiscountPayroll.net" Language="C#" MasterPageFile="SiteSidebar2.Master" AutoEventWireup="true" CodeBehind="Benefits.aspx.cs" ClientIDMode="Static" Inherits="Payroll.Web.Benefits" %>

<asp:Content ContentPlaceHolderID="x_HeadContent" runat="server">
	<style type="text/css">
		.t-container
		{
			margin: 1px;
			border: 1px solid gray;
		}
		.t-head
		{
			margin-top: -1px;
			border-top: 1px solid gray;
			background: #fcf0ba url(images/ui-bg_glass_55_fcf0ba_1x400.png) 50% 50% repeat-x;
			color: #363636; 
		}
		.t-left
		{
			margin-top: -1px;
			border-top: 1px solid gray;
			margin-right: -1px;
			/*border-right: 1px solid gray;*/
			padding-left: 5px;
			padding-right: 5px;
			vertical-align: middle;
		}
		.t-right
		{
			margin-top: -1px;
			border-top: 1px solid gray;
			text-align: center;
			vertical-align: middle;
		}
		.t-right.pricing
		{
			text-align: right;
			color:  DarkRed;
		}
		.t-pricing
		{
			margin-right: 15px;
		}
		.t-noteleft
		{
			text-align: right;
		}
		.t-noteright
		{
			font-family: Arial, Helvetica, Tahoma, sans-serif;
			font-size:10px;
			line-height: normal;
		}
		.t-free
		{
			margin-top:1px;
			margin-right:15px;
		}

	</style>
</asp:Content>

<asp:Content ContentPlaceHolderID="x_BodyContent" runat="server">
	<div>
		<div class="scale9Grid span-14 last">
			<h1>Features and Benefits</h1>
			<div class="t-container span-13 last">
				<div class="t-head span-13 last">&nbsp;Low, Upfront Pricing for our Plus Package</div>
				<div class="t-left span-8">Pay a low base monthly fee...</div>
				<div class="t-right pricing span-5 last"><span class="t-pricing">$20.00</span></div>
				<div class="t-left span-8">... then and simply add in...</div>
				<div class="t-right pricing span-5 last"><span class="t-pricing">$1.00 per active employee</span></div>
				<div class="t-left span-8">Direct deposit for ANY employee or independent contractor</div>
				<div class="t-right pricing span-5 last"><img class="t-free" src="images/free.png" alt="Free!"/></div>
				<div class="t-left span-8">Unlimited payroll runs</div>
				<div class="t-right pricing span-5 last"><img class="t-free" src="images/free.png" alt="Free!"/></div>
				<div class="t-left span-8">Easy download to popular accounting and personal finance programs</div>
				<div class="t-right pricing span-5 last"><img class="t-free" src="images/free.png" alt="Free!"/></div>
				<div class="t-left span-8">Federal electronic tax payments and filings</div>
				<div class="t-right pricing span-5 last"><img class="t-free" src="images/free.png" alt="Free!"/></div>
				<div class="t-left span-8">State tax payments and filings for 1<sup>st</sup> state - electronically in most cases</div>
				<div class="t-right pricing span-5 last"><img class="t-free" src="images/free.png" alt="Free!"/></div>
				<div class="t-left span-8">Option: State tax filings for additional states</div>
				<div class="t-right pricing span-5 last"><span class="t-pricing">$10 per extra state</span></div>
				<div class="t-left span-8">Option: 1099 e-file service at year end</div>
				<div class="t-right pricing span-5 last"><span class="t-pricing">$25 for up to 50 forms</span></div>

				<div class="t-head span-13 last">&nbsp;Easy to Setup and Use</div>
				<div class="t-left span-11">Streamlined setup for new users</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">No software to install</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Accessible from any computer</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Payday email reminders</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Free email support</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>

				<div class="t-head span-13 last">&nbsp;Easy Paydays</div>
				<div class="t-left span-11">Instant paycheck calculations</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Federal, state, and local tax calculations</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Voluntary deduction calculations (retirement plans, insurance premiums, etc.)</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Employer contribution calculations (retirement plans)</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">21 different types of pay, including: Annual salaries, Hourly wages, Overtime pay, Double overtime pay, Sick pay, Vacation pay, Holiday pay, Bonus, Commission, Allowance, Reimbursement, Cash tips, Paycheck tips, Clergy housing (cash), Clergy housing (in-kind), Nontaxable per diem, Group-term life insurance, S-Corp owners health insurance, Company HSA contribution, Personal use of company car, Bereavement</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Vacation and sick day tracking</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Professional-looking paycheck and paystub printing from your own computer</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Enable employees to view and print paystubs online</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>

				<div class="t-head span-13 last">&nbsp;Federal Payroll Taxes</div>
				<div class="t-left span-11">Federal tax deposit & filing email reminders</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Electronic, one-click federal tax payments*</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Electronic federal tax filings of all 941, 940, 944, and W-2 forms*</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Print signature-ready federal forms if you don't file electronic forms</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Guaranteed accurate calculations for all federal, state and local taxes****</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>

				<div class="t-head span-13 last">&nbsp;State Payroll Taxes (for all states in which you pay taxes)</div>
				<div class="t-left span-11">State tax deposit & filing email reminders</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Electronic, one-click state tax payments (where available)*</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Electronic state form filings (where available)*</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Signature-ready state forms or worksheets to help you complete the appropriate form</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Signature-ready state new hire and step-by-step employer registration forms</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>

				<div class="t-head span-13 last">&nbsp;Insightful Payroll Reports</div>
				<div class="t-left span-11">9 employer payroll reports: Tax Liability, Tax and Wage Summary, Total Pay, Tax Payments, Workers' Compensation, Vaction and Sick Leave, Billing Summary, Total Cost, Retirement Plans</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">5 employee detail reports: Payroll Summary, Payroll Details, Deductions, Last Paycheck, Employee Details</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">2 contractor detail reports: Contractor Payments & Contractor Details</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Drill-down access to report details</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">One-click Microsoft<sup>&reg;</sup> Excel export</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>

				<div class="t-head span-13 last">&nbsp;Easy Accounting and Bookkeeping Software Integration</div>
				<div class="t-left span-11">One-click export to QuickBooks, QuickBooks Online Edition, QuickBooks for Mac and Quicken</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
				<div class="t-left span-11">Easy export to most major accounting applications, including: Quickbooks, Quicken, Microsoft Money, Peachtree, ATX Client Write-Up, TaxWise Client Write-Up, CCH ProSystem fx Write-Up</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>

				<div class="t-head span-13 last">&nbsp;Flexible Account Access</div>
				<div class="t-left span-11">Free access to your payroll data for your accountant</div>
				<div class="t-right span-2 last"><img src="/images/checkmark_green.gif" alt="" /></div>
			</div>
			<div class="t-noteleft span-1">*</div>
			<div class="t-noteright span-12 last">The E-File &amp; Pay feature is available for federal and most state forms and taxes. For states where E-File is not available, You will be provided with signature ready forms or worksheets.<br/><br/></div>
			<div class="t-noteleft span-1">***</div>
			<div class="t-noteright span-12 last">Subscription, Internet access, Federal Employer Identification Number (FEIN), and U.S. billing address required. Check stock sold separately. Plus sales tax where applicable. Our payroll service integrates with QuickBooks 2007 &amp; higher, QuickBooks Mac 2006 &amp; higher, QuickBooks Online, Quicken 2005 &amp; higher and Peachtree Pro, Complete, Premium, and Quantum 2008 and 2009. Works with Microsoft Excel version 2002 or later. It does not integrate with QuickBooks Simple Start and Quicken Starter Edition. E-File &amp; E-Pay feature is available for federal and most state forms and taxes. For states where E-File is not available, you will be provided with signature ready forms or worksheets. 24/7 access is subject to occasional downtime. Terms, conditions, features, pricing, service and support are subject to change without notice.<br/><br/></div>
			<div class="t-noteleft span-1">****</div>
			<div class="t-noteright span-12 last">Your payroll service Accurate Calculation Guarantee. Payroll specialists work diligently to ensure that all federal and state taxes are calculated using the latest tax rates. In the unlikely event our calculations are incorrect; Intuit will pay any resulting penalties incurred. Customer must provide Intuit with a tax statement from the IRS, state or local agency showing a tax penalty incurred as a result of a calculation error during the time that you were an active subscriber. Client must present a copy of this letter to Intuit within 3 months of receiving the letter from the IRS. You are responsible for paying any additional tax liability you may owe and providing any other information Intuit reasonably requests. Terms, conditions, pricing, features, functionality, service and support are subject to change at any time without notice.<br/><br/></div>
		</div>
	</div>
</asp:Content>
