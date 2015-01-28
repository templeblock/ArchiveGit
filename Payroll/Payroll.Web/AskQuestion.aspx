<%@ Page Title="" Language="C#" MasterPageFile="SiteSidebar2.master" AutoEventWireup="true" CodeBehind="AskQuestion.aspx.cs" Inherits="Payroll.Web.AskQuestion" %>

<asp:Content ContentPlaceHolderID="x_BodyContent" runat="server">
	<table border="0" cellspacing="0" cellpadding="3" width="100%">
		<tr class="popupHeader">
			<td class="popupTitle" colspan="2">
				Email Us
			</td>
			<td class="popupClose" align="right">
				<a href="javascript:window.close()">close window</a>
			</td>
		</tr>
		<tr>
			<td colspan="3" class="infoTH2">
				Get an Answer
			</td>
		</tr>
		<tr>
			<td colspan="3" style="padding-left: 10px">
				<table cellspacing="0" cellpadding="0" border="0" width="100%">
					<tr>
						<td colspan="3">
						</td>
					</tr>
					<tr>
						<td colspan="3">
							Email us any time.&nbsp;<span class="error">Due to heavy volume, our current response time is 2-3 business days. If you have an urgent issue we recommend contacting us on chat or by phone.</span>
						</td>
					</tr>
					<tr>
						<td colspan="3">
							<form name="emailus_form" id="emailus_form" method="POST" onsubmit="return pc_form_emailus_form_submit(this); ">
								<input type="hidden" name="_fwiz" value="1" />
								<table valign="top" border="0" cellspacing="2" cellpadding="2">
									<tr>
										<td>
											<table border="0" cellspacing="2" cellpadding="2" width="100%">
												<tr>
													<td>
														&nbsp;
													</td>
													<td align="right">
														How do you want us to contact you?
													</td>
													<td>
														&nbsp;
													</td>
												</tr>
												<tr>
													<td colspan="2" align="right" valign="top">
														<input type="radio" id="contactMethod" name="contactMethod" value="1" checked="checked" />
													</td>
													<td>
														Call or Email
													</td>
												</tr>
												<tr>
													<td colspan="2" align="right" valign="top">
														<input type="radio" id="contactMethod" name="contactMethod" value="2" />
													</td>
													<td>
														Email
													</td>
												</tr>
												<tr>
													<td colspan="2" align="right" valign="top">
														<input type="radio" id="contactMethod" name="contactMethod" value="3" />
													</td>
													<td>
														Call
													</td>
												</tr>
												<tr>
													<td>
														&nbsp;
													</td>
													<td align="right">
														The email address we have for you:
													</td>
													<td colspan="2" align="left">
														jmccurdy@facetofacesoftware.com
													</td>
												</tr>
												<tr>
													<td>
														&nbsp;
													</td>
													<td align="right">
														The phone number we have for you:
													</td>
													<td colspan="2" align="left">
														978-973-1734
													</td>
												</tr>
												<tr>
													<td>
														&nbsp;
													</td>
													<td align="right">
														Alternate phone number:
													</td>
													<td colspan="2" align="left">
														<input type="text" name="altPhone" id="altPhone" value="" size="15" />&nbsp;<span class="note">(optional)</span>
													</td>
												</tr>
												<tr>
													<td>
														Subject:
													</td>
													<td colspan="2" align="left">
														<input type="text" name="customerArea" id="customerArea" value="" size="75" />
													</td>
												</tr>
												<tr>
													<td>
														&nbsp;
													</td>
													<td colspan="2" align="left">
														<textarea name="ticketDetails" id="ticketDetails" cols="58" rows="10"></textarea>
													</td>
												</tr>
											</table>
											<table border="0" cellspacing="2" cellpadding="2" width="100%">
												<tr>
													<td valign="middle" align="left" width="420px">
														<a href="javascript:window.close();" id="Cancel" onclick="">
															<img src='/images/standard/buttons/cancel.gif' alt='Cancel' class='' id='Cancel' border="0" /></a>
													</td>
													<td align="left">
														<input type="image" name='Send' alt='Send' class='' src='/images/standard/setup/buttons/send_default.gif' onclick="" />
													</td>
												</tr>
											</table>
										</td>
									</tr>
								</table>
								<input type="hidden" name="hco" value="1238910" />
								<input type="hidden" name="hss" value="1" />
							</form>
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
</asp:Content>
