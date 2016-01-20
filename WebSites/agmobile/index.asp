<%@ Language=JavaScript %>
<% Response.Buffer = true; %>
<% 
	var Agent = String(Request.servervariables('HTTP_USER_AGENT'));
	var bWireless = (Agent.indexOf('Mozilla') < 0);
	var bWML = false; //j

	function output(Text)
	{
		Response.write(Text + '\n');
	}
	
	function outputReturnCategories()
	{
		output('	<a accesskey="8" href="index.asp?cid=categories" style="font-weight:normal">Return to Categories</a><br/>');
	}

	function outputReturnHome()
	{
		output('	<a accesskey="9" href="index.asp" style="font-weight:normal">Return Home</a><br/>');
	}

	function outputBug()
	{
		output('	<img src="bug.gif" style="width:24px; height:24px; float:left" alt=""/>');
		output('	<img src="bug.gif" style="width:24px; height:24px; float:right" alt=""/>');
	}

	function outputNav(pos, flag)
	{
		if (!bWireless)
			return;

//		output('<table><tr><td align="center">');
		if (flag != 'bottom')
		{
			var dir = pos + (flag == 'top' ? 'u' : 'd');
			output('<a id="page' + dir + '" href="#page' + (pos + 1) + 'd" class="nav" style="font-weight:normal">Down</a>&nbsp;');
		}
		if (flag != 'top')
		{
			var dir = pos + (flag == 'bottom' ? 'd' : 'u');
			output('<a id="page' + dir + '" href="#page' + (pos - 1) + 'u" class="nav" style="font-weight:normal">Up</a>');
		}
		output('<br/>');
//		output('</td></tr></table>');
	}

	function SendGreeting(pid, FromName, FromAddress, ToName, ToAddress, Type)
	{
		if (Type == 'sms')
			return SendSmsViaHTTP(pid, FromName, FromAddress, ToName, ToAddress);
		else
		if (Type == 'mms')
			return SendMmsViaHTTP(pid, FromName, FromAddress, ToName, ToAddress);
		else
		if (Type == 'email')
			return SendSMTP(pid, FromName, FromAddress, ToName, ToAddress);

		return 'Bad send type';
	}

	function SendSmsViaHTTP(pid, FromName, FromAddress, ToName, ToAddress)
	{
		if (ToAddress == '')
			return 'The recipient\'s phone number was empty';

		if (FromAddress == '')
			FromAddress = '8885551212';
		if (FromName == '')
			FromName = FromAddress;
		if (ToName == '')
			ToName = ToAddress;

		var xml = Server.CreateObject('Microsoft.XMLHTTP'); // ('MSXML2.ServerXMLHTTP'); // 
		if (!xml)
			return 'No HTTP service was available';

		var lf = '%20%20%20';
		var ques = '%3f';
		var amp = '%26';
		var BodyText = 'Hi, I sent you a greeting!' + lf + 'You can pick it up at http://66.155.47.228/agmobile/preview.asp' + ques + 'x=1' + amp + 'pid=' + pid;
		var URL = 'http://204.188.185.226/ag/?recipient=' + ToAddress + '&sender=' + FromAddress + '&message=' + BodyText;

		xml.Open('GET'/*strMethod*/, URL, false/*bAsync, strUser, strPassword*/);
		xml.Send();
		var Error = xml.responseText;
		xml = null;

		if (Error.indexOf('E_OK') >= 0)
			return null;

		if (Error.indexOf('E_INVALID_RECIPIENT') >= 0)
			return 'The recipient\'s phone number was invalid';

		if (Error.indexOf('E_MESSAGE_TOO_LONG') >= 0)
			return 'The message was too long';

		if (Error.indexOf('E_MESSAGE_EMPTY') >= 0)
			return 'The message was empty';

		if (Error.indexOf('E_INVALID_SENDER') >= 0)
			return 'The sender\'s phone number was invalid';

		return Error;
	}

	function SendMmsViaHTTP(pid, FromName, FromAddress, ToName, ToAddress)
	{
		if (ToAddress == '')
			return 'The recipient\'s phone number was empty';

		if (FromAddress == '')
			FromAddress = '8885551212';
		if (FromName == '')
			FromName = FromAddress;
		if (ToName == '')
			ToName = ToAddress;

		var xml = Server.CreateObject('Microsoft.XMLHTTP'); // ('MSXML2.ServerXMLHTTP'); // 
		if (!xml)
			return 'No HTTP service was available';

		var SmilFile = '\\agmobile\\' + pid + '\\index.smil';
		var Subject = 'Hi, I sent you a greeting!';
		var MmscAddress = '63.108.142.183'; // '63.108.142.207' /*this works*/; // '63.108.142.183' /*(mmsc4.msw.wirelessfuture.com)*/; //'66.140.51.101' /*adsl-66-140-51-101.dsl.rcsntx.swbell.net*/;
		var MmscPort = '80850'; // '50850' '50750' '5080'/*it works*/
		var URL = 'http://66.155.47.228/cgi-bin/SendMMS.exe?smil=' + SmilFile + '&addr=' + MmscAddress + '&port=' + MmscPort + '&subj=' + Subject + '&to=' + ToAddress + '&from=' + FromAddress;
		//j URL += '&v=1'; //j for verbose output
		xml.Open('GET'/*strMethod*/, URL, false/*bAsync, strUser, strPassword*/);
		xml.Send();
		var Error = xml.responseText;
		xml = null;

		if (Error.indexOf('OK') >= 0)
			return null;

		return Error;
	}

	function SendSMTP(pid, FromName, FromAddress, ToName, ToAddress)
	{
		if (ToAddress == '')
			return 'The recipient\'s email address was empty';

		if (ToAddress.indexOf('@') < 0)
			return 'The recipient\'s email address was invalid';

		if (FromAddress == '')
			FromAddress = 'Anonymuos';
		if (FromName == '')
			FromName = FromAddress;
		if (ToName == '')
			ToName = ToAddress;

		var mail = Server.CreateObject('SMTPsvg.Mailer');
		if (!mail)
			return 'No message service was available';

		mail.QMessage = true;
		mail.RemoteHost = 'ns3.vianetworks.net'; // '66.155.47.228'; // '207.127.198.7'; // 'mail.attbi.com'; // 
		mail.FromName = FromName;
		mail.FromAddress = FromAddress;
		if (ToAddress.indexOf('@') < 0)
			ToAddress += '@mobile.att.net';
		var bSuccess = mail.AddRecipient(ToName, ToAddress);
		if (bSuccess)
		{
			mail.Subject = 'Hi, I sent you a greeting!';
			mail.BodyText = 'You can pick it up at http://66.155.47.228/agmobile/preview.asp?pid=' + pid + '&x=1';
			mail.ContentType = 'text/plain'; // 'text/html'; 
			//mail.CharSet = 1; // us-ascii=?
			//mail.WordWrap = true;
			//mail.WordWrapLen = 70;
			//? mail.ContentBase = baseURL;
			//? mail.BodyFormat = 0;
			//? mail.MailFormat = 0;
			bSuccess = mail.SendMail();
		}
		
		var Error = (bSuccess ? null : mail.Response);
		mail = null;
		return Error;
	}

//j	Response.Expires = 0; // Be sure this pages always refreshes
	var cid = (!Request.QueryString('cid').Count ? 'home' : String(Request.QueryString('cid')));
	var Title = '';
		 if (cid == 'home')			Title = 'AG Home';
	else if (cid == 'pickup')		Title = 'AG Pickup';
	else if (cid == 'sendform')		Title = 'AG Sender';
	else if (cid == 'smsform')		Title = 'AG Recipient';
	else if (cid == 'mmsform')		Title = 'AG Recipient';
	else if (cid == 'emailform')	Title = 'AG Recipient';
	else if (cid == 'sendconfirm')	Title = 'AG Send Complete';
	else if (cid == 'categories')	Title = 'AG Categories';
	else if (cid == 'holidays')		Title = 'AG Holidays';
	else if (cid == 'justbecause')	Title = 'AG Just Because';
	else if (cid == 'friendship')	Title = 'AG Friendship';
	else if (cid == 'love')			Title = 'AG Love &amp; Dating';
	else if (cid == 'birthday')		Title = 'AG Birthday';
	else if (cid == 'baby')			Title = 'AG Baby';
	else if (cid == 'justforkids')	Title = 'AG Just For Kids';
	else if (cid == 'teens')		Title = 'AG Teens';
	else if (cid == 'pets')			Title = 'AG Pets';
	else if (cid == 'sports')		Title = 'AG Sports';
	else if (cid == 'thankyou')		Title = 'AG Thank You';
	else if (cid == 'events')		Title = 'AG Events &amp; Occasions';
	else if (cid == 'care')			Title = 'AG Care &amp; Concern';
	else if (cid == 'special')		Title = 'AG Special Collections';
	else if (cid == 'family')		Title = 'AG Family';
	else if (cid == 'inspire')		Title = 'AG Inspire &amp; Encourage';
	else if (cid == 'religious')	Title = 'AG Religious';
	else if (cid == 'international')Title = 'AG International';
	else if (cid == 'travel')		Title = 'AG Travel';
	else if (cid == 'workplace')	Title = 'AG Workplace';
	else if (cid == 'celebdate')	Title = 'AG Celebrate The Date';

	if (bWML)
	{
		Response.ContentType = 'text/vnd.wap.wml';
		output('<?xml version="1.0"?>');
		output('<!DOCTYPE wml PUBLIC "-//WAPFORUM//DTD WML 1.3//EN" "http://www.wapforum.org/DTD/wml_1_1.dtd">');
		output('<wml>');
		output('<template>');
		output('	<do type="prev" label="Back">');
		output('		<prev/>');
		output('	</do>');
		output('	<do type="options" label="Home to ag.com">');
		output('		<go href="index.xml"/>');
		output('	</do>');
		output('</template>');
		output('<card id="index" title="' + Title + '">');
	}
	else
	{
		Response.ContentType = 'text/html';
		output('<?xml version="1.0" encoding="UTF-8"?>');
		output('<!DOCTYPE html PUBLIC "-//WAPFORUM//DTD XHTML Mobile 1.0//EN" "http://www.wapforum.org/DTD/xhtml-mobile10.dtd">');
		output('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">');
		output('<head>');
//		output('	<link rel="stylesheet" href="style.css" type="text/css"/>');
		output('	<style>');
		output('		body, img, form, p, a, table, tr, td {padding:0px; margin:0px}');
		output('		img.bug {width:24px; height:24px; float:left}');
		output('		p {color:black; font-size:medium; font-family:sans-serif}');
		output('		a {color:navy; font-size:medium; font-family:sans-serif; font-weight:bold}');
		output('		a.nav {color:#000000; text-decoration:none}');
		output('		br {font-size:small; font-weight:normal}');
//		output('		table {background-color:#ffffff; width:100%}');
//		output('		table, tr, td {border:0px}');
		output('		span.hilite {color:#400000; font-size:large; font-weight:bold}');
		output('		span.hilite2 {color:#400000; font-size:medium; font-weight:bold}');
		output('	</style>');
		output('	<title>' + Title + '</title>');
		output('</head>');
		output('<body style="background-color:#ff8080">');
	}

	// Build the body of the greeting
	if (cid == 'home')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		output('	<br/>');
		output('	<span class="hilite">Welcome to<br/>AG Mobile!</span.hilight><br/><br/>');
		output('	<a accesskey="1" href="index.asp?cid=categories">Send a Greeting</a><br/>');
		output('	<a accesskey="2" href="index.asp?cid=pickup">Pickup a Greeting</a><br/>');
	//	output('	<a accesskey="3" href="index.asp?cid=test">Test</a><br/>');
	//	output(Agent);
	//	output(String(Request.servervariables('HTTP_ACCEPT')));
		output('	<br/>');
		output('	<br/>');
		output('</p>');
	}
	else
	if (cid == 'test')
	{
		output('<span style="font-size:small; font-weight:normal">This is a font test small normal</span><br/>');
		output('<span style="font-size:small; font-weight:bold">This is a font test small bold</span><br/>');
		output('<span style="font-size:medium; font-weight:normal">This is a font test medium normal</span><br/>');
		output('<span style="font-size:medium; font-weight:bold">This is a font test medium bold</span><br/>');
		output('<span style="font-size:large; font-weight:normal">This is a font test large normal</span><br/>');
		output('<span style="font-size:large; font-weight:bold">This is a font test large bold</span><br/>');
	}
	else
	if (cid == 'pickup')
	{
		output('<form action="preview.asp" method="get">');
		output('<input type="hidden" name="x" value="1"/>');
		output('<p align="center">');
		outputBug();
		output('	<b>');
		output('	Enter the<br/><span class="hilite2">Greeting Number</span><br/>from the pickup message you received<br/>');
		output('	<input type="text" name="pid" style="background-color:#ffffff; -wap-input-format:NNNNNNN" format="NNNNNNN" size="4" maxlength="7" /><br/>');
		output('	<br/>');
		output('	<input accesskey="1" type="submit" value="View your greeting"/><br/>');
		output('	</b>');
		output('	<br/>');
		outputReturnHome();
		output('</p>');
		output('</form>');
	}
	else
	if (cid == 'sendform')
	{
		var pid = (!Request.QueryString('pid').Count ? 'NotFound' : String(Request.QueryString('pid')));
		output('<form action="index.asp" method="get">');
		output('<input type="hidden" name="pid" value="' + pid + '"/>');
		output('<p align="center">');
		outputBug();
		output('	<b>');
		output('	Enter <span class="hilite2">Sender</span><br/><span class="hilite2">Phone number</span><br/>');
		output('	<input type="text" name="from" style="background-color:#ffffff; -wap-input-required:true; -wap-input-format:NNNNNNNNNN" format="NNNNNNNNNN" size="6" maxlength="10"/><br/>');
		output('	Select a send method<br/>');
		output('	<select name="cid">');
		output('	<option value="smsform">SMS Text Pickup</option>');
		output('	<option value="mmsform">MMS Multimedia</option>');
		output('	<option value="emailform">Email Text Pickup</option>');
		output('	</select><br/>');
		output('	<br/>');
		output('	<input accesskey="1" type="submit" value="Continue to Send"/><br/>');
		output('	</b>');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
		output('</form>');
	}
	else
	if (cid == 'smsform' || cid == 'mmsform')
	{
		var pid = (!Request.QueryString('pid').Count ? 'NotFound' : String(Request.QueryString('pid')));
		var FromAddress = (!Request.QueryString('from').Count ? '' : String(Request.QueryString('from')));
		var type = (cid == 'smsform' ? 'sms' : 'mms');
		output('<form action="index.asp" method="get">');
		output('<input type="hidden" name="pid" value="' + pid + '"/>');
		output('<input type="hidden" name="from" value="' + FromAddress + '"/>');
		output('<input type="hidden" name="cid" value="sendconfirm"/>');
		output('<input type="hidden" name="type" value="' + type + '"/>');
		output('<p align="center">');
		outputBug();
		output('	<b>');
		output('	Enter <span class="hilite2">Recipient</span><br/><span class="hilite2">Phone number</span><br/>');
		output('	<input type="text" name="recipient" style="background-color:#ffffff; -wap-input-required:true; -wap-input-format:NNNNNNNNNN" format="NNNNNNNNNN" size="6" maxlength="10"/><br/>');
		output('	<br/>');
		output('	<input accesskey="1" type="submit" value="Send this greeting"/><br/>');
		output('	</b>');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('	<br/>');
		output('</p>');
		output('</form>');
	}
	else
	if (cid == 'emailform')
	{
		var pid = (!Request.QueryString('pid').Count ? 'NotFound' : String(Request.QueryString('pid')));
		var FromAddress = (!Request.QueryString('from').Count ? '' : String(Request.QueryString('from')));
		output('<form action="index.asp" method="get">');
		output('<input type="hidden" name="pid" value="' + pid + '"/>');
		output('<input type="hidden" name="from" value="' + FromAddress + '"/>');
		output('<input type="hidden" name="cid" value="sendconfirm"/>');
		output('<input type="hidden" name="type" value="email"/>');
		output('<p align="center">');
		outputBug();
		output('	<b>');
		output('	Enter <span class="hilite2">Recipient</span><br/><span class="hilite2">Email address</span><br/>');
		output('	<input type="text" name="recipient" style="background-color:#ffffff; -wap-input-required:true; -wap-input-format:*x" format="*x" size="10" maxlength="50"/><br/>');
		output('	<input accesskey="1" type="submit" value="Send this greeting"/><br/>');
		output('	</b>');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('	<br/>');
		output('</p>');
		output('</form>');
	}
	else
	if (cid == 'sendconfirm')
	{
		var pid = (!Request.QueryString('pid').Count ? 'NotFound' : String(Request.QueryString('pid')));
		var FromAddress = (!Request.QueryString('from').Count ? '' : String(Request.QueryString('from')));
		var ToAddress = (!Request.QueryString('recipient').Count ? '' : String(Request.QueryString('recipient')));
		var Type = (!Request.QueryString('type').Count ? '' : String(Request.QueryString('type')));
		var Error = SendGreeting(pid, ''/*FromName*/, FromAddress, ''/*ToName*/, ToAddress, Type);
		var Message = (!Error ? 'was sent' : 'could not be sent');
		if (ToAddress != '')
			Message += ' to';

		output('<p align="center">');
		outputBug();
		output('	<b>');
		if (!Error)
			output('	Congratulations!<br/>');
		output('	Greeting<br/><span class="hilite">' + pid + '</span><br/>' + Message + '<br/><span class="hilite">' + ToAddress + '</span><br/>');
		output('	</b>');
		if (Error)
			output(Error + '<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'categories')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="index.asp?cid=holidays">Holidays</a><br/>');
		output('	<a href="index.asp?cid=justbecause">Just Because</a><br/>');
		output('	<a href="index.asp?cid=friendship">Friendship</a><br/>');
		output('	<a href="index.asp?cid=love">Love &amp; Dating</a><br/>');
		output('	<a href="index.asp?cid=birthday">Birthday</a><br/>');
		output('	<a href="index.asp?cid=baby">Baby</a><br/>');
		output('	<a href="index.asp?cid=justforkids">Just For Kids</a><br/>');
		outputNav(2, '');
		output('	<a href="index.asp?cid=teens">Teens</a><br/>');
		output('	<a href="index.asp?cid=pets">Pets</a><br/>');
		output('	<a href="index.asp?cid=sports">Sports</a><br/>');
		output('	<a href="index.asp?cid=thankyou">Thank You</a><br/>');
		output('	<a href="index.asp?cid=events">Events &amp; Occasions</a><br/>');
		output('	<a href="index.asp?cid=care">Care &amp; Concern</a><br/>');
		output('	<a href="index.asp?cid=special">Special Collections</a><br/>');
		outputNav(3, '');
		output('	<a href="index.asp?cid=family">Family</a><br/>');
		output('	<a href="index.asp?cid=inspire">Inspire &amp; Encourage</a><br/>');
		output('	<a href="index.asp?cid=religious">Religious</a><br/>');
		output('	<a href="index.asp?cid=international">International</a><br/>');
		output('	<a href="index.asp?cid=travel">Travel</a><br/>');
		output('	<a href="index.asp?cid=workplace">Workplace</a><br/>');
		output('	<a href="index.asp?cid=celebdate">Celebrate The Date</a><br/>');
		outputNav(4, 'bottom');
		output('	<br/>');
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'holidays')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=3020586">Celebrate Fall</a><br/>');
		output('	<a href="preview.asp?pid=2037225">Sweetest Day 10/19</a><br/>');
		output('	<a href="preview.asp?pid=2037203">Mother-In-Law`s Day 10/27</a><br/>');
		output('	<a href="preview.asp?pid=2037448">Halloween 10/31</a><br/>');
		output('	<a href="preview.asp?pid=3024206">Ramadan 11/6</a><br/>');
		output('	<a href="preview.asp?pid=2037494">Veterans Day 11/11</a><br/>');
		output('	<a href="preview.asp?pid=2020061">Remembrance Day 11/11</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=2038483">Thanksgiving 11/28</a><br/>');
		output('	<a href="preview.asp?pid=3002101">Celebrate Winter</a><br/>');
		output('	<a href="preview.asp?pid=2028690">Hanukkah 11/30 - 12/7</a><br/>');
		output('	<a href="preview.asp?pid=2021802">Season`s Greetings</a><br/>');
		output('	<a href="preview.asp?pid=3024202">Hari Raya 12/6</a><br/>');
		output('	<a href="preview.asp?pid=3022969">Christmas 12/25</a><br/>');
		output('	<a href="preview.asp?pid=3001024">Kwanzaa 12/26 - 1/1</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=2019822">Boxing Day 12/26</a><br/>');
		output('	<a href="preview.asp?pid=2011766">New Year`s Day 1/1</a><br/>');
		output('	<a href="preview.asp?pid=3002151">Martin Luther King Day 1/20</a><br/>');
		output('	<a href="preview.asp?pid=3002195">Tu B`Shvat 1/18</a><br/>');
		output('	<a href="preview.asp?pid=2011398">Black History Month 02</a><br/>');
		output('	<a href="preview.asp?pid=2011403">Chinese New Year 2/1</a><br/>');
		output('	<a href="preview.asp?pid=2028946">Groundhog Day 2/2</a><br/>');
		output('	<a href="preview.asp?pid=2031011">Valentine`s Day 2/14</a><br/>');
		outputNav(4, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'justbecause')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=2035186">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=2035185">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=2035080">Say "Hi"</a><br/>');
		output('	<a href="preview.asp?pid=2035187">Wassup?</a><br/>');
		output('	<a href="preview.asp?pid=2021314">Funny</a><br/>');
		output('	<a href="preview.asp?pid=2009588">Thinking Of You</a><br/>');
		output('	<a href="preview.asp?pid=2009608">Miss You</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=3025193">Keep In Touch</a><br/>');
		output('	<a href="preview.asp?pid=2021302">Sorry</a><br/>');
		output('	<a href="preview.asp?pid=2034111">Get Together</a><br/>');
		output('	<a href="preview.asp?pid=2021326">Just For Kids</a><br/>');
		output('	<a href="preview.asp?pid=2016473">Smiles</a><br/>');
		output('	<a href="preview.asp?pid=3021680">Hugs</a><br/>');
		output('	<a href="preview.asp?pid=2011779">Internet Talk</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=3002590">Special Collections</a><br/>');
		output('	<a href="preview.asp?pid=2037422">Bubblegum</a><br/>');
		output('	<a href="preview.asp?pid=2015779">Fun with Clay</a><br/>');
		output('	<a href="preview.asp?pid=2009609">Huggable Bears</a><br/>');
		output('	<a href="preview.asp?pid=2036209">Spanish</a><br/>');
		outputNav(4, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'friendship')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=2037524">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=2009590">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=3022575">Funny</a><br/>');
		output('	<a href="preview.asp?pid=3022565">Best Friends</a><br/>');
		output('	<a href="preview.asp?pid=2022865">Between Women</a><br/>');
		output('	<a href="preview.asp?pid=2022878">Miss You</a><br/>');
		output('	<a href="preview.asp?pid=2038399">Thanks Friend</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=2022871">Inspire &amp; Encourage</a><br/>');
		output('	<a href="preview.asp?pid=2038313">Sorry</a><br/>');
		output('	<a href="preview.asp?pid=3021669">Get Together</a><br/>');
		output('	<a href="preview.asp?pid=3024055">Angels</a><br/>');
		output('	<a href="preview.asp?pid=2010711">Hugs</a><br/>');
		output('	<a href="preview.asp?pid=2034115">Smiles</a><br/>');
		output('	<a href="preview.asp?pid=2036204">Spanish</a><br/>');
		outputNav(3, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'love')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=3022255">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=2035184">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=2009756">Loving You</a><br/>');
		output('	<a href="preview.asp?pid=2021390">Intimate Moments</a><br/>');
		output('	<a href="preview.asp?pid=2035253">Miss You</a><br/>');
		output('	<a href="preview.asp?pid=3002291">More Than Friends</a><br/>');
		output('	<a href="preview.asp?pid=2034102">Funny</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=3021676">Cute</a><br/>');
		output('	<a href="preview.asp?pid=2009771">Sorry</a><br/>');
		output('	<a href="preview.asp?pid=2021379">Get Together</a><br/>');
		output('	<a href="preview.asp?pid=3023466">Hugs</a><br/>');
		output('	<a href="preview.asp?pid=2037514">Kisses</a><br/>');
		output('	<a href="preview.asp?pid=2021401">Dating Woes</a><br/>');
		output('	<a href="preview.asp?pid=3003298">Poetry</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=2021365">Internet Love</a><br/>');
		output('	<a href="preview.asp?pid=2036136">Marry Me</a><br/>');
		output('	<a href="preview.asp?pid=3020949">Religious</a><br/>');
		output('	<a href="preview.asp?pid=2012202">Bubblegum</a><br/>');
		output('	<a href="preview.asp?pid=2010715">Huggable Bears</a><br/>');
		output('	<a href="preview.asp?pid=2036281">Spanish</a><br/>');
		outputNav(4, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'birthday')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=2034098">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=2035179">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=2012183">Anyone</a><br/>');
		output('	<a href="preview.asp?pid=2009297">Funny</a><br/>');
		output('	<a href="preview.asp?pid=2009438">Belated</a><br/>');
		output('	<a href="preview.asp?pid=2009325">Just For Her</a><br/>');
		output('	<a href="preview.asp?pid=2009298">Just For Him</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=2028937">Friends</a><br/>');
		output('	<a href="preview.asp?pid=2009377">Family</a><br/>');
		output('	<a href="preview.asp?pid=2010714">Just For Kids</a><br/>');
		output('	<a href="preview.asp?pid=2035246">Just For Teens</a><br/>');
		output('	<a href="preview.asp?pid=2026288">For a Teacher</a><br/>');
		output('	<a href="preview.asp?pid=2009457">Co-workers</a><br/>');
		output('	<a href="preview.asp?pid=2009307">Love</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=2037282">Religious</a><br/>');
		output('	<a href="preview.asp?pid=2021328">Over The Hill</a><br/>');
		output('	<a href="preview.asp?pid=2027532">Milestone</a><br/>');
		output('	<a href="preview.asp?pid=2035211">Pets</a><br/>');
		output('	<a href="preview.asp?pid=3002650">Holiday Birthdays</a><br/>');
		output('	<a href="preview.asp?pid=2026267">Invitations</a><br/>');
		output('	<a href="preview.asp?pid=2015773">Special Collections</a><br/>');
		outputNav(4, '');
		output('	<a href="preview.asp?pid=2012187">Bubblegum</a><br/>');
		output('	<a href="preview.asp?pid=2010714">Huggable Bears</a><br/>');
		output('	<a href="preview.asp?pid=2036462">Spanish</a><br/>');
		outputNav(5, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'baby')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=3022270">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=2021775">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=2019776">Announcements</a><br/>');
		output('	<a href="preview.asp?pid=2035559">Baby`s Firsts</a><br/>');
		output('	<a href="preview.asp?pid=2035560">Baby Shower</a><br/>');
		output('	<a href="preview.asp?pid=2035561">Baptism &amp; Christening</a><br/>');
		output('	<a href="preview.asp?pid=2035586">Inspire &amp; Encourage</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=2035565">Family Relations</a><br/>');
		output('	<a href="preview.asp?pid=2035577">Invitations</a><br/>');
		output('	<a href="preview.asp?pid=2021325">New Baby</a><br/>');
		output('	<a href="preview.asp?pid=2036197">New Boy</a><br/>');
		output('	<a href="preview.asp?pid=2021776">New Girl</a><br/>');
		output('	<a href="preview.asp?pid=2035567">New Grandchild</a><br/>');
		output('	<a href="preview.asp?pid=2021820">Multiple Births</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=2035583">Parents-to-be</a><br/>');
		output('	<a href="preview.asp?pid=2035573">Religious</a><br/>');
		output('	<a href="preview.asp?pid=2035589">Thank You</a><br/>');
		output('	<a href="preview.asp?pid=3024968">Spanish</a><br/>');
		outputNav(4, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'justforkids')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=3020150">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=2011526">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=2010666">Birthday</a><br/>');
		output('	<a href="preview.asp?pid=3001691">Congratulations</a><br/>');
		output('	<a href="preview.asp?pid=2009496">Encouragement</a><br/>');
		output('	<a href="preview.asp?pid=2026248">Games &amp; Jokes</a><br/>');
		output('	<a href="preview.asp?pid=2026261">Get Well</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=3020938">Hugs</a><br/>');
		output('	<a href="preview.asp?pid=3003320">Invitations</a><br/>');
		output('	<a href="preview.asp?pid=3025666">Just Because</a><br/>');
		output('	<a href="preview.asp?pid=2026192">For a Babysitter</a><br/>');
		output('	<a href="preview.asp?pid=3020164">For a Teacher</a><br/>');
		output('	<a href="preview.asp?pid=2011527">Miss You</a><br/>');
		output('	<a href="preview.asp?pid=2035225">Reminders</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=3023269">Smiles</a><br/>');
		output('	<a href="preview.asp?pid=2036946">Sports</a><br/>');
		output('	<a href="preview.asp?pid=2026188">Sorry</a><br/>');
		output('	<a href="preview.asp?pid=2036619">School Talk</a><br/>');
		output('	<a href="preview.asp?pid=2035262">Thanks</a><br/>');
		output('	<a href="preview.asp?pid=3002590">Barney™</a><br/>');
		output('	<a href="preview.asp?pid=3025667">Care Bears</a><br/>');
		outputNav(4, '');
		output('	<a href="preview.asp?pid=3021076">My Pet Monster</a><br/>');
		output('	<a href="preview.asp?pid=3021071">Popples</a><br/>');
		output('	<a href="preview.asp?pid=3026039">Strawberry Shortcake</a><br/>');
		outputNav(5, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'teens')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=2036015">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=2022872">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=2021362">Just `Cuz</a><br/>');
		output('	<a href="preview.asp?pid=2021344">B-day</a><br/>');
		output('	<a href="preview.asp?pid=2037411">Collections</a><br/>');
		output('	<a href="preview.asp?pid=3020636">Life Events</a><br/>');
		output('	<a href="preview.asp?pid=2034103">Love</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=2035220">Pets</a><br/>');
		output('	<a href="preview.asp?pid=2035139">Sports</a><br/>');
		output('	<a href="preview.asp?pid=2036775">Students</a><br/>');
		output('	<a href="preview.asp?pid=2035082">Travel</a><br/>');
		output('	<a href="preview.asp?pid=2020050">Holidays</a><br/>');
		outputNav(3, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'pets')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=2035236">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=2038429">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=2035203">Announcements</a><br/>');
		output('	<a href="preview.asp?pid=2009437">Belated Birthday</a><br/>');
		output('	<a href="preview.asp?pid=2009370">Birthday</a><br/>');
		output('	<a href="preview.asp?pid=2035215">Congratulations</a><br/>');
		output('	<a href="preview.asp?pid=2012438">Funny</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=2010694">Get Well</a><br/>');
		output('	<a href="preview.asp?pid=2037458">Holidays</a><br/>');
		output('	<a href="preview.asp?pid=2037447">Invitations</a><br/>');
		output('	<a href="preview.asp?pid=2021317">Just Because</a><br/>');
		output('	<a href="preview.asp?pid=2035250">Love</a><br/>');
		output('	<a href="preview.asp?pid=2035256">Miss You</a><br/>');
		output('	<a href="preview.asp?pid=2012444">Pet Sitting</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=2035228">Reminders</a><br/>');
		output('	<a href="preview.asp?pid=2011560">Sympathy</a><br/>');
		output('	<a href="preview.asp?pid=2011532">Thank You</a><br/>');
		output('	<a href="preview.asp?pid=2035260">Veterinarian</a><br/>');
		output('	<a href="preview.asp?pid=2037416">Animal Crackers</a><br/>');
		output('	<a href="preview.asp?pid=2035508">Bubblegum</a><br/>');
		output('	<a href="preview.asp?pid=3024307">Garfield</a><br/>');
		outputNav(4, '');
		output('	<a href="preview.asp?pid=2021394">intuitions</a><br/>');
		output('	<a href="preview.asp?pid=2038428">Pets by Gary Patterson</a><br/>');
		outputNav(5, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'sports')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">Baseball</a><br/>');
		output('	<a href="preview.asp?pid=       ">Basketball</a><br/>');
		output('	<a href="preview.asp?pid=       ">Bicycling</a><br/>');
		output('	<a href="preview.asp?pid=       ">Bowling</a><br/>');
		output('	<a href="preview.asp?pid=       ">Congratulations</a><br/>');
		output('	<a href="preview.asp?pid=       ">Encouragement</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Equestrian</a><br/>');
		output('	<a href="preview.asp?pid=       ">Exercising</a><br/>');
		output('	<a href="preview.asp?pid=       ">Fishing</a><br/>');
		output('	<a href="preview.asp?pid=       ">Football</a><br/>');
		output('	<a href="preview.asp?pid=       ">Golf</a><br/>');
		output('	<a href="preview.asp?pid=       ">Gymnastics</a><br/>');
		output('	<a href="preview.asp?pid=       ">Hiking/Camping</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=       ">Martial Arts</a><br/>');
		output('	<a href="preview.asp?pid=       ">Motor Sports</a><br/>');
		output('	<a href="preview.asp?pid=       ">Other Sports</a><br/>');
		output('	<a href="preview.asp?pid=       ">Rollerblading</a><br/>');
		output('	<a href="preview.asp?pid=       ">Running</a><br/>');
		output('	<a href="preview.asp?pid=       ">Sailing</a><br/>');
		output('	<a href="preview.asp?pid=       ">Skateboarding</a><br/>');
		outputNav(4, '');
		output('	<a href="preview.asp?pid=       ">Soccer</a><br/>');
		output('	<a href="preview.asp?pid=       ">Softball</a><br/>');
		output('	<a href="preview.asp?pid=       ">Surfing/Windsurfing</a><br/>');
		output('	<a href="preview.asp?pid=       ">Swimming/Diving</a><br/>');
		output('	<a href="preview.asp?pid=       ">Tennis</a><br/>');
		output('	<a href="preview.asp?pid=       ">Track and Field</a><br/>');
		output('	<a href="preview.asp?pid=       ">Volleyball</a><br/>');
		outputNav(6, '');
		output('	<a href="preview.asp?pid=       ">Water Sports</a><br/>');
		output('	<a href="preview.asp?pid=       ">Everyday Champions</a><br/>');
		output('	<a href="preview.asp?pid=       ">Animated Stationery</a><br/>');
		output('	<a href="preview.asp?pid=       ">Sportin` Pep Talk</a><br/>');
		output('	<a href="preview.asp?pid=       ">Sports Fun by Gary Patterson</a><br/>');
		outputNav(7, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'thankyou')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=       ">Anytime Thanks</a><br/>');
		output('	<a href="preview.asp?pid=       ">Baby</a><br/>');
		output('	<a href="preview.asp?pid=       ">Babysitter</a><br/>');
		output('	<a href="preview.asp?pid=       ">Bridal Shower</a><br/>');
		output('	<a href="preview.asp?pid=       ">Engagement</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Family</a><br/>');
		output('	<a href="preview.asp?pid=       ">Thanks for the Greeting</a><br/>');
		output('	<a href="preview.asp?pid=       ">Friendship</a><br/>');
		output('	<a href="preview.asp?pid=       ">Gift</a><br/>');
		output('	<a href="preview.asp?pid=       ">Help</a><br/>');
		output('	<a href="preview.asp?pid=       ">Holidays</a><br/>');
		output('	<a href="preview.asp?pid=       ">Hospitality</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=       ">Pet Sitting</a><br/>');
		output('	<a href="preview.asp?pid=       ">Religious</a><br/>');
		output('	<a href="preview.asp?pid=       ">Support</a><br/>');
		output('	<a href="preview.asp?pid=       ">Teacher</a><br/>');
		output('	<a href="preview.asp?pid=       ">Veterinarian</a><br/>');
		output('	<a href="preview.asp?pid=       ">Wedding</a><br/>');
		output('	<a href="preview.asp?pid=       ">Workplace</a><br/>');
		outputNav(4, '');
		output('	<a href="preview.asp?pid=       ">Genuine Garfield</a><br/>');
		output('	<a href="preview.asp?pid=       ">Spanish</a><br/>');
		outputNav(5, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'events')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=       ">Anniversary</a><br/>');
		output('	<a href="preview.asp?pid=       ">Announcements</a><br/>');
		output('	<a href="preview.asp?pid=       ">Bridal Shower</a><br/>');
		output('	<a href="preview.asp?pid=       ">Congratulations</a><br/>');
		output('	<a href="preview.asp?pid=       ">Engagement</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Good Bye</a><br/>');
		output('	<a href="preview.asp?pid=       ">Good Luck</a><br/>');
		output('	<a href="preview.asp?pid=       ">Graduation</a><br/>');
		output('	<a href="preview.asp?pid=       ">Invitations</a><br/>');
		output('	<a href="preview.asp?pid=       ">Marry Me</a><br/>');
		output('	<a href="preview.asp?pid=       ">Retirement</a><br/>');
		output('	<a href="preview.asp?pid=       ">Wedding</a><br/>');
		outputNav(3, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'care')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=       ">Get Well</a><br/>');
		output('	<a href="preview.asp?pid=       ">Support</a><br/>');
		output('	<a href="preview.asp?pid=       ">Thanks for Support</a><br/>');
		output('	<a href="preview.asp?pid=       ">Sympathy</a><br/>');
		output('	<a href="preview.asp?pid=       ">Hugs</a><br/>');
		output('	<a href="preview.asp?pid=       ">Smiles</a><br/>');
		outputNav(2, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'special')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=       ">African American</a><br/>');
		output('	<a href="preview.asp?pid=       ">Animated Stationery</a><br/>');
		output('	<a href="preview.asp?pid=       ">Astrology</a><br/>');
		output('	<a href="preview.asp?pid=       ">Barney™</a><br/>');
		output('	<a href="preview.asp?pid=       ">Bubblegum</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Buddy Bugs</a><br/>');
		output('	<a href="preview.asp?pid=       ">Care Bears</a><br/>');
		output('	<a href="preview.asp?pid=       ">Chicken Soup for the Soul</a><br/>');
		output('	<a href="preview.asp?pid=       ">Comic Greetings</a><br/>');
		output('	<a href="preview.asp?pid=       ">CrunchyWorld</a><br/>');
		output('	<a href="preview.asp?pid=       ">Dance, Man, Dance!</a><br/>');
		output('	<a href="preview.asp?pid=       ">EcologyFund</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=       ">Flavia</a><br/>');
		output('	<a href="preview.asp?pid=       ">Fun with Clay</a><br/>');
		output('	<a href="preview.asp?pid=       ">Game Greetings</a><br/>');
		output('	<a href="preview.asp?pid=       ">Gary Patterson</a><br/>');
		output('	<a href="preview.asp?pid=       ">Genuine Garfield</a><br/>');
		output('	<a href="preview.asp?pid=       ">Gift Cards</a><br/>');
		output('	<a href="preview.asp?pid=       ">Huggable Bears</a><br/>');
		outputNav(4, '');
		output('	<a href="preview.asp?pid=       ">InstaGreetings</a><br/>');
		output('	<a href="preview.asp?pid=       ">Internet Talk</a><br/>');
		output('	<a href="preview.asp?pid=       ">Laura &amp; Company</a><br/>');
		output('	<a href="preview.asp?pid=       ">Life`s A Gas!</a><br/>');
		output('	<a href="preview.asp?pid=       ">Military</a><br/>');
		output('	<a href="preview.asp?pid=       ">Movies</a><br/>');
		output('	<a href="preview.asp?pid=       ">Nature and Wildlife</a><br/>');
		outputNav(5, '');
		output('	<a href="preview.asp?pid=       ">My Pet Monster</a><br/>');
		output('	<a href="preview.asp?pid=       ">Oh, Joy!</a><br/>');
		output('	<a href="preview.asp?pid=       ">Photography</a><br/>');
		output('	<a href="preview.asp?pid=       ">Popples</a><br/>');
		output('	<a href="preview.asp?pid=       ">Redneck Humor</a><br/>');
		output('	<a href="preview.asp?pid=       ">School Talk</a><br/>');
		output('	<a href="preview.asp?pid=       ">Shtick Happens</a><br/>');
		outputNav(6, '');
		output('	<a href="preview.asp?pid=       ">Speed Bump</a><br/>');
		output('	<a href="preview.asp?pid=       ">Strawberry Shortcake</a><br/>');
		output('	<a href="preview.asp?pid=       ">The New Yorker</a><br/>');
		output('	<a href="preview.asp?pid=       ">3 Scoops</a><br/>');
		output('	<a href="preview.asp?pid=       ">Twisted Whiskers</a><br/>');
		output('	<a href="preview.asp?pid=       ">U.S. Cities</a><br/>');
		output('	<a href="preview.asp?pid=       ">Write Your Own</a><br/>');
		outputNav(7, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'family')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=       ">Baby</a><br/>');
		output('	<a href="preview.asp?pid=       ">Birthday</a><br/>');
		output('	<a href="preview.asp?pid=       ">Coupons</a><br/>');
		output('	<a href="preview.asp?pid=       ">Reminders</a><br/>');
		output('	<a href="preview.asp?pid=       ">Get Together</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Just Because</a><br/>');
		output('	<a href="preview.asp?pid=       ">Milestone Events</a><br/>');
		output('	<a href="preview.asp?pid=       ">Thanks Family</a><br/>');
		output('	<a href="preview.asp?pid=       ">Spanish</a><br/>');
		outputNav(3, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'inspire')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=       ">Just Because</a><br/>');
		output('	<a href="preview.asp?pid=       ">Friendship</a><br/>');
		output('	<a href="preview.asp?pid=       ">Light-Hearted</a><br/>');
		output('	<a href="preview.asp?pid=       ">Angels</a><br/>');
		output('	<a href="preview.asp?pid=       ">Students</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Diet</a><br/>');
		output('	<a href="preview.asp?pid=       ">In Remembrance</a><br/>');
		output('	<a href="preview.asp?pid=       ">Chicken Soup for the Soul</a><br/>');
		output('	<a href="preview.asp?pid=       ">Flavia</a><br/>');
		outputNav(3, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'religious')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=       ">Christian</a><br/>');
		output('	<a href="preview.asp?pid=       ">Jewish</a><br/>');
		output('	<a href="preview.asp?pid=       ">Buddhist</a><br/>');
		output('	<a href="preview.asp?pid=       ">Hindu</a><br/>');
		output('	<a href="preview.asp?pid=       ">Islam</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Sikh</a><br/>');
		output('	<a href="preview.asp?pid=       ">Ramadan</a><br/>');
		output('	<a href="preview.asp?pid=       ">Flavia</a><br/>');
		output('	<a href="preview.asp?pid=       ">National Prayer Day 5/2</a><br/>');
		output('	<a href="preview.asp?pid=       ">Religious Week</a><br/>');
		outputNav(3, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'international')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">U.S. Cities</a><br/>');
		output('	<a href="preview.asp?pid=       ">Australia</a><br/>');
		output('	<a href="preview.asp?pid=       ">Deutschland</a><br/>');
		output('	<a href="preview.asp?pid=       ">France</a><br/>');
		output('	<a href="preview.asp?pid=       ">French Canada</a><br/>');
		output('	<a href="preview.asp?pid=       ">Japanese</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Nederlands</a><br/>');
		output('	<a href="preview.asp?pid=       ">Spanish</a><br/>');
		output('	<a href="preview.asp?pid=       ">United Kingdom</a><br/>');
		outputNav(3, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'travel')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">Activities</a><br/>');
		output('	<a href="preview.asp?pid=       ">Bon Voyage</a><br/>');
		output('	<a href="preview.asp?pid=       ">Funny</a><br/>');
		output('	<a href="preview.asp?pid=       ">Have Fun</a><br/>');
		output('	<a href="preview.asp?pid=       ">Invitations</a><br/>');
		output('	<a href="preview.asp?pid=       ">Notices</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Students</a><br/>');
		output('	<a href="preview.asp?pid=       ">Summer Vacation</a><br/>');
		output('	<a href="preview.asp?pid=       ">To Kids</a><br/>');
		output('	<a href="preview.asp?pid=       ">U.S. Cities</a><br/>');
		output('	<a href="preview.asp?pid=       ">Virtual Vacation</a><br/>');
		output('	<a href="preview.asp?pid=       ">Welcome Home</a><br/>');
		output('	<a href="preview.asp?pid=       ">CrunchyWorld</a><br/>');
		outputNav(3, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'workplace')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">Featured Pick</a><br/>');
		output('	<a href="preview.asp?pid=       ">QuickGreets</a><br/>');
		output('	<a href="preview.asp?pid=       ">Acknowledgments/Reminders</a><br/>');
		output('	<a href="preview.asp?pid=       ">Announcements</a><br/>');
		output('	<a href="preview.asp?pid=       ">Between Women</a><br/>');
		output('	<a href="preview.asp?pid=       ">Birthday</a><br/>');
		output('	<a href="preview.asp?pid=       ">Congratulations</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">Comic Greetings</a><br/>');
		output('	<a href="preview.asp?pid=       ">Encouragement</a><br/>');
		output('	<a href="preview.asp?pid=       ">Funny</a><br/>');
		output('	<a href="preview.asp?pid=       ">Game Greetings</a><br/>');
		output('	<a href="preview.asp?pid=       ">Good Bye</a><br/>');
		output('	<a href="preview.asp?pid=       ">Good Luck</a><br/>');
		output('	<a href="preview.asp?pid=       ">I Love My Career</a><br/>');
		outputNav(3, '');
		output('	<a href="preview.asp?pid=       ">Invitations</a><br/>');
		output('	<a href="preview.asp?pid=       ">Life Events</a><br/>');
		output('	<a href="preview.asp?pid=       ">Miss You</a><br/>');
		output('	<a href="preview.asp?pid=       ">New Job</a><br/>');
		output('	<a href="preview.asp?pid=       ">Promotion</a><br/>');
		output('	<a href="preview.asp?pid=       ">Retirement</a><br/>');
		output('	<a href="preview.asp?pid=       ">Shtick Happens</a><br/>');
		outputNav(4, '');
		output('	<a href="preview.asp?pid=       ">Thank You</a><br/>');
		output('	<a href="preview.asp?pid=       ">The New Yorker</a><br/>');
		output('	<a href="preview.asp?pid=       ">Travel</a><br/>');
		output('	<a href="preview.asp?pid=       ">Write Your Own</a><br/>');
		output('	<a href="preview.asp?pid=       ">Holidays</a><br/>');
		outputNav(5, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}
	else
	if (cid == 'celebdate')
	{
		output('<p align="center" mode="wrap">');
		outputBug();
		outputNav(1, 'top');
		output('	<a href="preview.asp?pid=       ">October 2002</a><br/>');
		output('	<a href="preview.asp?pid=       ">November 2002</a><br/>');
		output('	<a href="preview.asp?pid=       ">December 2002</a><br/>');
		output('	<a href="preview.asp?pid=       ">January 2003</a><br/>');
		output('	<a href="preview.asp?pid=       ">February 2003</a><br/>');
		output('	<a href="preview.asp?pid=       ">March 2003</a><br/>');
		output('	<a href="preview.asp?pid=       ">April 2003</a><br/>');
		outputNav(2, '');
		output('	<a href="preview.asp?pid=       ">May 2003</a><br/>');
		output('	<a href="preview.asp?pid=       ">June 2003</a><br/>');
		output('	<a href="preview.asp?pid=       ">July 2003</a><br/>');
		output('	<a href="preview.asp?pid=       ">August 2003</a><br/>');
		output('	<a href="preview.asp?pid=       ">September 2003</a><br/>');
		outputNav(3, 'bottom');
		output('	<br/>');
		outputReturnCategories();
		outputReturnHome();
		output('</p>');
	}

	if (bWML)
	{
		output('</card>');
		output('</wml>');
	}
	else
	{
		output('</body>');
		output('</html>');
	}
%>
