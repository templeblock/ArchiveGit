<%@ Page Title="Welcome | DiscountPayroll.net" Language="C#" MasterPageFile="SiteSidebar1.Master" AutoEventWireup="true" CodeBehind="Default.aspx.cs" ClientIDMode="Static" Inherits="Payroll.Web.Default" %>

<asp:Content ContentPlaceHolderID="x_HeadContent" runat="server">
	<meta name="description" content="" />
	<%-- keywords should only include misspellings and synoymns not used in the document--%>
	<meta name="keywords" content="" />
</asp:Content>

<asp:Content ContentPlaceHolderID="x_BodyContent" runat="server">
		<div class="span-14">
			<div class="scale9Grid">
				<h1>Welcome to DiscountPayroll.net!</h1>
				<div>
					<div style="width:520px; height:401px; background-image:url(/images/main_test_calendar.jpg)">
						<h3 style="position:absolute; margin:20px 0px 0px 25px;">Worry-Free Payroll &amp; Taxes</h3>
						<div style="position:absolute; margin:55px 55px 0px 275px; ">
							<p>We are the online payroll alternative, where you manage-it-yourself and
							save a bundle of money.</p>
							<p>Don't be fooled by introductory prices, hidden fees, or prices that don't 
							include direct deposit, state tax handling, or contractor pay.</p>
							<p>Our self-service payroll makes it easy!</p>
							<p style="text-align:center;">
								<a title="Start Now" href="Enroll.aspx">Start Now</a><br/>
								<a title="Learn More" href="Benefits.aspx">Learn More</a>
							</p>
						</div>
					</div>
				</div>

				<div>
					<p />
					<p>Over 1 million businesses use this same comprehensive payroll application.</p>
					<p>Create paychecks in minutes anytime and anywhere you have an internet connection.</p>
					<p>No desktop software required.</p>
					<object
							classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" 
							codebase="https://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" 
							width="520" height="416">
						<param name="movie" value="/swf/mini_generic_v11.swf"/>
						<param name="quality" value="high" />
						<param name="wmode" value="opaque" />
					<!--[if !IE]>-->
						<object
								type="application/x-shockwave-flash" 
								data="/swf/mini_generic_v11.swf" 
								width="520" height="416">
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
				<p>Our low $21.00 monthly fee covers your first employee and 1 state:</p>
				<ul>
					<li>Unlimited number of paychecks each month</li>
					<li>Direct deposit for employees and independent contractors</li>
					<li>Electronic tax payments</li>
					<li>Quarterly & year-end tax filings</li>
					<li>W-2s</li>
					<li>Guaranteed accurate calculations for all federal, state and local taxes</li>
					<li>Employee website for employees to view paystubs online</li>
					<li>Easy download to QuickBooks, Peachtree, and other accounting programs</li>
					<li>See all of the <a href="Benefits.aspx">Features and Benefits</a></li>
				</ul>
				<p>Additional services available:</p>
				<ul>
					<li>Additional employees - $1.00 per month</li>
					<li>Additional states - $10.00 per additional state per month</li>
					<li>1099 e-file service - $25.00 for up to 50 forms</li>
				</ul>
				<%--<div id="accordion">
					<h3>
						<a href="#">Section 1</a></h3>
					<div>
						<ul>
							<li><a href="#">Menu item 1</a></li>
							<li><a href="#">Menu item 2</a></li>
							<li><a href="#">Menu item 3</a></li>
						</ul>
					</div>
					<h3>
						<a href="#">Section 2</a></h3>
					<div>
						<p>
							Sed non urna. Donec et ante. Phasellus eu ligula. Vestibulum sit amet purus. Vivamus hendrerit, dolor at aliquet laoreet, mauris turpis porttitor velit, faucibus interdum tellus libero ac justo. Vivamus non quam. In suscipit faucibus urna.
						</p>
					</div>
					<h3>
						<a href="#">Section 3</a></h3>
					<div>
						<p>
							Nam enim risus, molestie et, porta ac, aliquam ac, risus. Quisque lobortis. Phasellus pellentesque purus in massa. Aenean in pede. Phasellus ac libero ac tellus pellentesque semper. Sed ac felis. Sed commodo, magna quis lacinia ornare, quam ante aliquam nisi, eu iaculis leo purus venenatis dui.
						</p>
						<ul>
							<li>List item one</li>
							<li>List item two</li>
							<li>List item three</li>
						</ul>
					</div>
					<h3>
						<a href="#">Section 4</a></h3>
					<div>
						<p>
							Cras dictum. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Aenean lacinia mauris vel est.
						</p>
						<p>
							Suspendisse eu nisl. Nullam ut libero. Integer dignissim consequat lectus. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos.
						</p>
					</div>
				</div>
				<br />--%>
				<%--<h5>
					This is a H5 heading</h5>
				<p>
					Cras et dapibus magna. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Praesent dolor neque, gravida sed tempus a, vehicula nec metus. Cras accumsan ultricies sapien id tincidunt. Etiam vitae ante quis lectus pulvinar</p>
				<table width="100%" border="0" cellspacing="0" cellpadding="0" summary="This is the table summary">
					<caption>
						This is the table caption
					</caption>
					<tr>
						<th scope="col">
							Heading1
						</th>
						<th scope="col">
							Heading 2
						</th>
						<th scope="col">
							Heading 3
						</th>
						<th scope="col">
							Heading 4
						</th>
						<th scope="col">
							Heading 5
						</th>
					</tr>
					<tr class="even">
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
					</tr>
					<tr class="odd">
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
					</tr>
					<tr class="even">
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
						<td>
							Table Cell
						</td>
					</tr>
				</table>--%>
			</div>
		</div>
		<div class="span-5 last">
			<div class="sidebarbox">
				<h3>A better way to do payroll</h3>
				<p>DiscountPayroll.net has everything you need to do payroll.  We guide you through the 
				entire payroll process, from setup to paychecks to W-2s. And we leave you in control.
				</p>
			</div>
			<div class="sidebarbox">
				<h3>Instant calculations and forms</h3>
				<p>You save time with instant paycheck calculations and completed tax forms. Our 
				automated system makes this all possible. You can even export payroll data to accounting 
				software such as QuickBooks.
				</p>
			</div>
			<div class="sidebarbox">
				<h3>See how easy payroll can be</h3>
				<p>You can access DiscountPayroll.net anytime, anywhere there’s an Internet connection, 
				with a PC or a Mac. There’s no software to buy, install, or learn. Best of all, you won't 
				pay a dime until your run your first payroll. You can even pay your employees today.<br/>
				Start now and <a href="Enroll.aspx">Enroll</a>.
				</p>
			</div>
		</div>
</asp:Content>
