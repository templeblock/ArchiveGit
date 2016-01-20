var g_strTopic = '';
var g_strPrevTopic = '';
var g_strNextTopic = '';
var g_ahost = 'http://www.americangreetings.com';
var g_ahostname = 'AmericanGreetings.com';
var g_strBackTopic = '';
var g_strThisTopic = '';

function GoToTopic(strTopic)
{
	self.location.href = 'cphelptopics.htm?topic=' + strTopic + '&back=' + g_strThisTopic;
}

function GoToContents()
{
	GoToTopic('0');
}

function GoBack()
{
	GoToTopic(g_strBackTopic);
//j	self.history.back();
}

function GoForward()
{
	self.history.forward();
}

function PrintTopic()
{
	self.focus();
	self.print();
}

function GoToNextTopic()
{
	if (g_strNextTopic == '')
	{
		var iTopic = parseInt(g_strTopic);
		iTopic = (iTopic == NaN ? 0 : iTopic + 1);
		g_strNextTopic = String(iTopic);
	}

	GoToTopic(g_strNextTopic);
}

function GoToPrevTopic()
{
	if (g_strPrevTopic == '')
	{
		var iTopic = parseInt(g_strTopic);
		iTopic = (iTopic == NaN ? 0 : (iTopic > 0 ? iTopic - 1 : 0));
		g_strPrevTopic = String(iTopic);
	}

	GoToTopic(g_strPrevTopic);
}

function SetTopic(strTopic, strBackTopic)
{
	g_strBackTopic = strBackTopic;
	g_strThisTopic = strTopic;
}

function SetPrevNextTopic(strPrevTopic, strNextTopic)
{
	g_strPrevTopic = strPrevTopic;
	g_strNextTopic = strNextTopic;
}
	
function GetContext()
{
	var strContext = window.location.search;
	if (!strContext || !strContext.length)
	{
		strContext = parent.location.search;
		if (!strContext || !strContext.length)
			return '';
	}

	return strContext.substr(1);
}

function GetOption(strContext, strNameTarget)
{
	if (!strContext || !strContext.length)
		return '';

	strContext += '&';
	var iStart = strContext.indexOf('?') + 1;
	while (iStart >= 0)
	{
		var iEnd = strContext.indexOf('&', iStart);
		if (iEnd <= iStart)
			break;
		var iPair = strContext.indexOf('=', iStart);
		if (iPair > iStart && iPair < iEnd)
		{
			var strName = strContext.substr(iStart, iPair-iStart);
			strName.toLowerCase();
			if (strName == strNameTarget)
			{
				iPair++;
				var strValue = strContext.substr(iPair, iEnd-iPair);
				return unescape(strValue);
			}
		}

		iStart = iEnd + 1;
	}

	return '';
}

function Img(imgfile)
{
	return './cp/help/' + imgfile;
}

function WriteTopic()
{
	var topic = g_strThisTopic;

	var strHtml = '';
	if (topic == '' || topic == '0')
	{
		SetPrevNextTopic('11', '1');
		strHtml = '\
		<h1>Table of Contents</h1> \
		<a href="javascript:GoToTopic(\'1\')">What is HP Design Studio by AmericanGreetings.com?</a><br /> \
		<a href="javascript:GoToTopic(\'2\')">What do I need to do in order to use HP Design Studio?</a><br /> \
		<a href="javascript:GoToTopic(\'3\')">Getting started - How to use HP Design Studio.</a><br /> \
		<a href="javascript:GoToTopic(\'4\')">Frequently Asked Questions</a><br /> \
		<a href="javascript:GoToTopic(\'5\')">Using the Creative Workspace</a><br /> \
		<a href="javascript:GoToTopic(\'7\')">Browser Support</a><br /> \
		<a href="javascript:GoToTopic(\'8\')">Contact us</a><br /> \
		<a href="javascript:GoToTopic(\'9\')">4 by 8 Photo Card Instructions</a><br /> \
		<a href="javascript:GoToTopic(\'10\')">Modifying Projects to be Bordered or Borderless</a><br /> \
		<a href="javascript:GoToTopic(\'11\')">Do I have an HP borderless-capable printer?</a><br /> \
		';
	}
	else if (topic == '1')
	{
		SetPrevNextTopic('0', '2');
		strHtml = '\
		<h1>What is HP Design Studio by AmericanGreetings.com?</h1> \
		<p> \
			HP Design Studio was developed for people who like to express themselves with \
			one-of-a-kind cards, notes, envelopes, invitations, and more.  Our printable \
			projects are easy to design and personalize, and fun to send.<br /> \
			<br /> \
			You can:</p> \
		<ul> \
			<li>Add a photo to your projects</li> \
			<li>Double-check your spelling</li> \
			<li>Save your completed project so you can use it again in the future</li> \
		</ul> \
		<p> \
			HP Design Studio helps you do all that and more!<br /> \
			<br /> \
			Whether you are celebrating a special occasion, or just keeping in touch, \
			HP Design Studio has lots of ways to help you say what\'s on your mind and in \
			your heart.  We\'ll give you art and words.  You add the magic. \
		</p> \
		';
	}
	else if (topic == '2')
	{
		SetPrevNextTopic('1', '3');
		strHtml = '\
		<h1>What do I need to do in order to use HP Design Studio?</h1> \
		<ul> \
			<li>HP Design Studio software, art and templates</li> \
			<li>A personal printer</li> \
			<li>Paper</li> \
			<li>Your imagination</li> \
		</ul> \
		It\'s quick, easy and fun! \
		';
	}
	else if (topic == '3')
	{
		SetPrevNextTopic('2', '4');
		strHtml = '\
		<h1>Getting started - How to use HP Design Studio.</h1> \
			Starting at the home page of HP Design Studio, you can browse through all of the available categories \
			of projects to find one that meets your needs. \
		<ol> \
			<li>The left side of the page will allow you to navigate through the project design <b>categories</b>. \
			Click on the category names to open and explore them.</li> \
			<li>The right side of the page will show you the <b>thumbnail images</b> of various projects in that category.</li> \
			<li>To see a larger <b>preview</b> of the project, click on the thumbnail image.</li> \
			<li>If you would like to view more projects you can click the <b>Back</b> button to return to the thumbnails \
			page, or you can just select a new category.</li> \
			<li>Continue to browse until you find the right project for your needs.  Click on the <b>Personalize and Print This Project</b> link to open up \
			the <b>HP Design Studio Workspace</b>.  In the workspace, you can personalize your \
			project by changing the text, adding new text messages, adding a favorite \
			photo, and so much more.  Have fun!</li> \
			<li>When you have completed your personalizations, click the <b>Print</b> button.  Your project will be sent to your printer.</li> \
		</ol> \
		<p><b>Tips:<br /> \
			</b> \
			<br /> \
			The first time you print a note card or greeting card, \
			be sure to run a <b>paper feed print test</b> in the Print window.  \
			You’ll only need to do this the first time you print a card and HP Design Studio will remember your settings. \
			<br /> \
			<br /> \
			All text on the HP Design Studio greeting cards and projects can be edited to \
			fit your specific need.  If you see a Floral design in Thank you that you think \
			would make a great Anniversary greeting, just change the text message.  It is easy, fast and \
			fun!<br /> \
			<br /> \
		</p> \
		';
	}
	else if (topic == '4')
	{
		SetPrevNextTopic('3', '51');
		strHtml = '\
		<h1>Frequently Asked Questions</h1> \
		<a href="javascript:GoToTopic(\'51\')">Is HP Design Studio safe to install on my computer?</a><br /> \
		<a href="javascript:GoToTopic(\'58\')">How do I change greeting card sizes?</a><br /> \
		<a href="javascript:GoToTopic(\'59\')">How do I Add-A-Photo to my project?</a><br /> \
		<a href="javascript:GoToTopic(\'60\')">What kind of paper is best to use with HP Design Studio?</a><br /> \
		<a href="javascript:GoToTopic(\'61\')">How does a half-fold greeting print?</a><br /> \
		<a href="javascript:GoToTopic(\'62\')">How does a quarter-fold greeting print?</a><br /> \
		<a href="javascript:GoToTopic(\'63\')">How many copies of my project can I print?</a><br /> \
		<a href="javascript:GoToTopic(\'64\')">How do I print Envelopes?</a><br /> \
		<a href="javascript:GoToTopic(\'65\')">My note card prints off the page.  What do I do?</a><br /> \
		<a href="javascript:GoToTopic(\'66\')">How can I achieve optimal photo quality when printing?</a><br /> \
		<a href="javascript:GoToTopic(\'28\')">When I print 4X8 Photo Cards, nothing prints or only half of the design prints.  What do I do?</a><br /> \
		';
	}
	else if (topic == '5')
	{
		SetPrevNextTopic('66', '40');
		strHtml = '\
		<h1>Using the Creative Workspace</h1> \
		<p> \
		After you have chosen the project you want to personalize, click on the <b>Personalize \
		and Print This Project</b> link and the HP Design Studio \
		Workspace window will pop up.  The Workspace is where you can \
		personalize and print your greeting card or project.  You can add, delete, or \
		change text and art elements.<br /> \
		<br /> \
		You can start by simply highlighting the \
		project\'s existing text message and begin typing.  Your keystrokes will change \
		the sentiment right on your screen.  You can also change the size, color, \
		alignment and type of font used on your project, and add shapes or photos.<br /> \
		<br /> \
		There are many other tools available in the workspace if you wish to create a \
		truly unique printed project.  Each button icon represents one of the tools in the workshop.  If you need a quick hint as to what each tool does, \
		place the cursor over the tool icon.  Hold the pointer over the tool icon \
		for a several seconds and the message will appear.<br /> \
		</p> \
		<a href="javascript:GoToTopic(\'40\')">Top Toolbar</a><br /> \
		<a href="javascript:GoToTopic(\'41\')">Left Toolbar</a><br /> \
		';
	}
	else if (topic == '7')
	{
		SetPrevNextTopic('41', '8');
		strHtml = '\
		<h1>Browser Support</h1> \
		<p>We strongly recommend you use HP Design Studio with either Internet \
		Explorer 5.5 (or higher) or Netscape 4.7x (or higher).  HP Design Studio is not compatible with Macintosh computers, \
		WebTV, Eye Opener, or Internet Appliances.</p> \
		<p>For Internet Explorer, go to <a target="ie" href="http://www.microsoft.com/ie">www.microsoft.com/ie</a>, then Downloads.</p> \
		<p>For Netscape, got to <a target="ns" href="http://www.netscape.com">www.netscape.com</a>, then browser central.</p> \
		';
	}
	else if (topic == '8')
	{
		SetPrevNextTopic('7', '9');
		strHtml = '\
		<h1>Contact us</h1> \
		<p>We are here to provide support for the site, and we\'re always happy to hear from our customers!<br /><br /> \
		<a target="email" href="' + g_ahost + '/customer/emailus_plugin.pd?source=hp">Email us  by clicking on this link.</a> \
		</p> \
		';
	}
	else if (topic == '9')
	{
		SetPrevNextTopic('8', '10');
		strHtml = '\
		<h1>4 by 8 Photo Card Instructions</h1> \
		<a href="javascript:GoToTopic(\'20\')">Print Setup Instructions</a><br /> \
		<a href="javascript:GoToTopic(\'21\')">Loading the Paper</a><br /> \
		<a href="javascript:GoToTopic(\'22\')">Printing</a><br /> \
		<a href="javascript:GoToTopic(\'23\')">About Borderless Cards</a><br /> \
		<a href="javascript:GoToTopic(\'24\')">Modifying Projects to be Bordered or Borderless</a><br /> \
		<a href="javascript:GoToTopic(\'25\')">Printing Borderless Cards</a><br /> \
		<a href="javascript:GoToTopic(\'26\')">Printing Envelopes</a><br /> \
		<a href="javascript:GoToTopic(\'27\')">Storing the Paper</a><br /> \
		<a href="javascript:GoToTopic(\'28\')">When I print 4X8 Photo Cards, nothing prints or only half of the design prints.  What do I do?</a><br /> \
		';
	}
	else if (topic == '10')
	{
		SetPrevNextTopic('9', '11');
		strHtml = '\
		<h1>Modifying Projects to be Bordered or Borderless</h1> \
		<p> \
		All templates can be made bordered or borderless by resizing the background image.<br /> \
		<ul> \
			<li>To make a card with a border out of a borderless template, select the background image and drag the corner handles in about ¼ inch from each edge.</li> \
			<li>To make a borderless card out of a template with a border, select the background image and drag the corner handles out to the edge.</li> \
		</ul> \
		When changing a template to bordered or borderless, you may need to adjust the photo you\'ve placed in the card so that it fits properly in the photo area. \
		</p> \
		';
	}
	else if (topic == '11')
	{
		SetPrevNextTopic('10', '20');
		strHtml = '\
		<h1>Do I have an HP borderless-capable printer?</h1> \
		<p>This following is a list of the HP printers that support borderless printing (as of January 2005):<br /><br /> \
		<ul> \
			<li>Deskjet 5600 Series</li> \
			<li>Deskjet 5700 Series</li> \
			<li>Deskjet 5800 Series</li> \
			<li>Deskjet 6500 Series</li> \
			<li>Deskjet 6800 Series</li> \
			<li>Deskjet 9600 Series</li> \
			<li>OfficeJet 6200 Series</li> \
			<li>OfficeJet 7300 Series</li> \
			<li>OfficeJet 7400 Series</li> \
			<li>Photosmart 2600 Series</li> \
			<li>Photosmart 2700 Series</li> \
			<li>Photosmart 7400 Series</li> \
			<li>Photosmart 7600 Series</li> \
			<li>Photosmart 7700 Series</li> \
			<li>Photosmart 7900 Series</li> \
			<li>Photosmart 8100 Series</li> \
			<li>Photosmart 8400 Series</li> \
			<li>Photosmart 8700 Series</li> \
			<li>PSC 1600 Series</li> \
			<li>PSC 2300 Series</li> \
			<li>PSC 2400 Series</li> \
			<li>PSC 2500 Series</li> \
			<li>PSC 2600 Series</li> \
			<li>PSC 2700 Series</li> \
		</ul> \
		Most HP printers released after January 2005 are borderless-capable. \
		<br/> \
			<a href="javascript:GoToTopic(\'24\')">If you do not have a borderless printer, click here to learn how to change your project to be bordered</a><br /> \
		</p> \
		';
	}

	else if (topic == '20')
	{
		SetPrevNextTopic('10', '21');
		strHtml = '\
		<h1>Print Setup Instructions</h1> \
		<p> \
			HP Design Studio is set up to print 4 by 8 Photo Cards correctly on HP printers.  You do not need to set up a custom page size or paper type; just click "Print" once you\'ve completed your card without any special setup.  For special considerations for borderless cards, see the "About Borderless Cards" section. \
		</p> \
		';
	}
	else if (topic == '21')
	{
		SetPrevNextTopic('20', '22');
		strHtml = '\
		<h1>Loading the Paper</h1> \
		<p> \
			<ul> \
				<li>Handle paper by the edges to avoid fingerprints.</li> \
				<li>Keep a minimum of 10 sheets in the input tray.</li> \
				<li>Use only flat paper. In very low humidity, the paper might curl, which can skew the image. If the paper curls, put it back in the plastic bag and gently bend the stack of paper back until it is flat and then load it.</li> \
				<li>Use paper under the following conditions: 15 to 25ºC (50 to 91ºF) and 20 to 80 percent relative humidity.</li> \
			</ul> \
			For HP Deskjet printers, HP all-in-ones, and HP Photosmart printers (except HP Photosmart 100, 200, and 300 series printers).<br /> \
			<br /> \
			TIP: For best results, load the paper into the standard input tray.<br /> \
			<ol> \
				<li>Insert the photo paper into the standard input tray with the perforated, tabbed edge toward you with the HP logos facing up.</li> \
				<li>Adjust the paper width and length guides in the standard input tray to fit closely to the edges of the paper.</li> \
			</ol> \
		</p> \
		';
	}
	else if (topic == '22')
	{
		SetPrevNextTopic('21', '23');
		strHtml = '\
		<h1>Printing</h1> \
		<p> \
		HP Design Studio is set up to automatically print correctly on HP printers and should not require special print settings.<br /> \
		<ol> \
			<li>Select Print in the software program.</li> \
			<li>Choose your printer from the menu.</li> \
			<li>To achieve the best possible photo quality, set your printer driver print quality selection to "Best".</li> \
			<li>Choose the Paper Feed Settings button to set the way paper feeds into the printer (right side, left side, or center).</li> \
			<li>When the print test is complete, insert the Photo Cards paper and print your project.</li> \
		</ol> \
		NOTE: The first time you print, the application will run a paper feed print test to make sure the software works correctly with your printer. Use plain paper and follow the on-screen instructions to print the test page.<br /> \
		</p> \
		';
	}
	else if (topic == '23')
	{
		SetPrevNextTopic('22', '24');
		strHtml = '\
		<h1>About Borderless Cards</h1> \
		<p> \
		HP Photo Cards come with a number of borderless designs.  These are indicated by the word “Borderless” in the name of the card.  You can print a borderless card only on newer printers that are capable of four-sided borderless printing.<br /> \
		<br /> \
		To determine if you have an HP borderless-capable printer, <a href="javascript:GoToTopic(\'11\')">click here to view a list of printers</a>. \
		</p> \
		';
	}
	else if (topic == '24')
	{
		SetPrevNextTopic('23', '25');
		strHtml = '\
		<h1>Modifying Projects to be Bordered or Borderless</h1> \
		<p> \
		All templates can be made bordered or borderless by resizing the background image.<br /> \
		<ul> \
			<li>To make a card with a border out of a borderless template, select the background image and drag the corner handles in about ¼ inch from each edge.</li> \
			<li>To make a borderless card out of a template with a border, select the background image and drag the corner handles out to the edge.</li> \
		</ul> \
		When changing a template to bordered or borderless, you may need to adjust the photo you\'ve placed in the card so that it fits properly in the photo area. \
		</p> \
		';
	}
	else if (topic == '25')
	{
		SetPrevNextTopic('24', '26');
		strHtml = '\
		<h1>Printing Borderless Cards</h1> \
		<p> \
		If you have a borderless-capable printer, use the following steps to print a borderless card: <br /> \
		<ul> \
			<li>Select Print.</li> \
			<li>Choose your printer from the menu and select Preferences.</li> \
			<li>Click to select the "Borderless Printing" check box (depending upon your printer, this may be found under the Print/Quality, Page Setup or possibly another tab).</li> \
			<li>In the printer driver properties, use the custom size option or custom borderless option for setting the paper size to 4x8.</li> \
			<li>Click OK to save your changes.</li> \
			<li>Click OK to print.</li> \
		</ul> \
		</p> \
		';
	}
	else if (topic == '26')
	{
		SetPrevNextTopic('25', '27');
		strHtml = '\
		<h1>Printing Envelopes</h1> \
		<p> \
		HP Design Studio includes an envelope wizard to set up and print envelopes. Follow the on-screen instructions to personalize the envelopes. Print the envelopes the same way you print a card; it should not require any special print setup.<br /> \
		<br /> \
		Printing borderless envelopes is possible if you successfully printed a borderless card.<br /> \
		Use the same printer settings to print borderless cards and envelopes.<br /> \
		<br /> \
		TIP: Tuck the flaps inside the envelopes to ensure a clean feed into the printer.<br /> \
		</p> \
		';
	}
	else if (topic == '27')
	{
		SetPrevNextTopic('26', '28');
		strHtml = '\
		<h1>Storing the Paper</h1> \
		<p> \
		Keep unused paper in the plastic bag and reseal the box. Store the box flat and in a cool place.<br /> \
		<br /> \
		TIP: For product usage tips including driver settings for non-HP printers, product support, and warranty information, visit <a target="hpmedia" href="http://www.hp.com/support/inkjet_media">http://www.hp.com/support/inkjet_media</a><br /> \
		</p> \
		';
	}
	else if (topic == '28')
	{
		SetPrevNextTopic('25', '0');
		strHtml = '\
		<h1>When I print 4X8 Photo Cards, nothing prints or only half of the design prints.  What do I do?</h1> \
		<p> \
		All printers feed custom sized paper differently.  In order for HP Design Studio to determine how to print on 4X8 Photo Cards using your printer, you need to set the Paper Feed Settings within the Print dialog box.  To do this:<br /> \
		<br /> \
		<ol> \
			<li>Click <b>Paper Feed Settings</b> located within the print dialog box.</li> \
			<li>Under <b>Feeds from:</b>, if the Right option is selected, then change to Left.  If the Left option is selected, then change to Right.  In other words, opposite of the way it was set when you entered the Paper Feed Settings dialog.</li> \
			<li>Click OK to save changes and reprint your card.</li> \
			<li>If the above steps do not work you will need to recalibrate your printer. Please refer to your printers manual for instructions.</li> \
		</ol> \
		</p> \
		';
	}

	else if (topic == '40')
	{
		SetPrevNextTopic('28', '41');
		strHtml = '\
		<h1>Using the Creative Workspace</h1> \
		<h2>Top Toolbar</h2> \
		<p> \
			<img src="' + Img('help_help.gif') + '" border="0" align="middle"></img> &nbsp;<b>Get Help</b> \
		</p> \
		<p> \
			Click to display Help<br /> \
			<br /> \
			<img src="' + Img('help_print.gif') + '" border="0" align="middle"></img> &nbsp;<b>Print  \
				your project</b> \
		</p> \
		<p> \
			Send your completed greeting or project to the printer.<br /> \
			<br /> \
			<img src="' + Img('help_open.gif') + '" border="0" align="middle"></img> &nbsp;<b>Open a saved project</b> \
		</p> \
		<p> \
			Open a saved project.<br /> \
			<br /> \
			<img src="' + Img('help_close.gif') + '" border="0" align="middle"></img> &nbsp;<b>Close your project</b> \
		</p> \
		<p> \
			A project can be closed from the creative workspace.<br /> \
			<br /> \
			<img src="' + Img('help_saveas.gif') + '" border="0" align="middle"></img> &nbsp;<b>Save your project to </b> \
		</p> \
		<p> \
			Click to save your completed project to your hard drive.<br /> \
			<br /> \
			<img src="' + Img('help_undo.gif') + '" border="0" align="middle"></img> &nbsp;<b>Undo the last change</b> \
		</p> \
		<p> \
			Click to undo your last edit.<br /> \
			<br /> \
			<img src="' + Img('help_redo.gif') + '" border="0" align="middle"></img> &nbsp;<b>Redo the last change</b> \
		</p> \
		<p> \
			Click to redo your last edit.<br /> \
			<br /> \
			<img src="' + Img('help_cut.gif') + '" border="0" align="middle"></img> &nbsp;<b>Cut the selected object to the clipboard</b> \
		</p> \
		<p> \
			Click to cut/remove a selected item from your project.<br /> \
			<br /> \
			<img src="' + Img('help_copy.gif') + '" border="0" align="middle"></img> &nbsp;<b>Copy the selected object to the clipboard</b> \
		</p> \
		<p> \
			Click to copy a selected text or image to the clipboard.<br /> \
			<br /> \
			<img src="' + Img('help_paste.gif') + '" border="0" align="middle"></img> &nbsp;<b>Paste from the clipboard</b> \
		</p> \
		<p> \
			Click to place a copy of your text or image to the clipboard.<br /> \
			<br /> \
			<img src="' + Img('help_delete.gif') + '" border="0" align="middle"></img> &nbsp;<b>Delete the selected object</b> \
		</p> \
		<p> \
			Click to remove a selected text or image from your project.<br /> \
			<br /> \
			<img src="' + Img('help_spellcheck.gif') + '" border="0" align="middle"></img> &nbsp;<b>Check Spelling</b> \
		</p> \
		<p> \
			Click to show or hide a grid that can be used for aligning text and images on  \
			your project.<br /> \
			<br /> \
			<img src="' + Img('help_extras.gif') + '" border="0" align="middle"></img> &nbsp;<b>Access the Extras</b> \
		</p> \
		<p> \
			Click to download fonts and spell check.<br /> \
		</p> \
		';
	}
	else if (topic == '41')
	{
		SetPrevNextTopic('40', '0');
		strHtml = '\
		<h1>Using the Creative Workspace</h1> \
		<h2>Left Toolbar</h2> \
		<p> \
			<br /> \
			<img src="' + Img('help_addtextbox.gif') + '" border="0" align="middle"></img> &nbsp;<b>Add a Text Box object</b> \
		</p> \
		<p> \
			Click to add a text box to your greeting or project.<br /> \
			<br /> \
			<img src="' + Img('help_addphoto.gif') + '" border="0" align="middle"></img> &nbsp;<b>Add-A-Photo  \
				object</b> \
		</p> \
		<p> \
			Click to add a shape to your greeting or project.<br /> \
			<br /> \
			<img src="' + Img('help_zoom.gif') + '" border="0" align="middle"></img> &nbsp;<b>Change the Zoom level</b> \
		</p> \
		<p> \
			Allows you to zoom in or out of your project.<br /> \
			<br /> \
			<img src="' + Img('help_flip.gif') + '" border="0" align="middle"></img> &nbsp;<b>Flip the selected object</b> \
		</p> \
		<p> \
			Flip your image or text from left to right or top to bottom on your project.<br /> \
			<br /> \
			<img src="' + Img('help_rotate.gif') + '" border="0" align="middle"></img> &nbsp;<b>Rotate the selected object</b> \
		</p> \
		<p> \
			Move selected text or image to various points on your project.<br /> \
			<br /> \
			<img src="' + Img('help_layers.gif') + '" border="0" align="middle"></img> &nbsp;<b>Change the Layering of the selected object</b> \
		</p> \
		<p> \
			Move selected text or image from front to back or back to front.<br /> \
			<br /> \
			<img src="' + Img('help_select.gif') + '" border="0" align="middle"></img> &nbsp;<b>Select an object</b> \
		</p> \
		<p> \
			Click to select the next or previous object within your project.<br /> \
			<br /> \
		</p> \
		';
	}
	else if (topic == '51')
	{
		SetPrevNextTopic('4', '58');
		strHtml = '\
		<h1>Is HP Design Studio safe to install on my computer?</h1> \
		Yes.  The HP Design Studio software is developed by ' + g_ahostname + ' and  \
		is guaranteed to be safe from any virus.<br /> \
		<br /> \
		';
	}
	else if (topic == '58')
	{
		SetPrevNextTopic('51', '59');
		strHtml = '\
		<h1>How do I change greeting card sizes?</h1> \
		<p>All greeting cards can be printed as ½ Fold or ¼ Fold.  This  \
			is done within the print dialog box.</p> \
		<ol> \
			<li>Complete your project and click the <b>Print</b> button.</li> \
			<li>The print dialog box will appear.  In the lower left corner, click on either <b>Half-fold</b> or <b>Quarter-fold</b> \
			to indicate what size greeting card you would like to print.</li> \
			<li>Click the <b>OK</b> button and your project will be sent to your printer for printing.</li> \
		</ol> \
		';
	}
	else if (topic == '59')
	{
		SetPrevNextTopic('58', '60');
		strHtml = '\
		<h1>How do I Add-A-Photo to my project?</h1> \
		You can add a photo to any HP Design Studio project.  Some projects are  \
		specifically designed as Add-A-Photo projects, with a space for your photo  \
		incorporated into the project layout.  But photos can be added to all projects.  You \
		just need to decide where to feature the photo.<br /> \
		<br /> \
		<b>Add-A-Photo projects:<br /> \
		</b> \
		<br /> \
		If you have chosen a project that incorporates the Add-A-Photo feature in its  \
		design, you can simply double-click on the words <b>"Double-Click to Add-A-Photo®"</b> to place an image \
		within the card or project.<br /> \
		<br /> \
		The Add-A-Photo dialog box will open.  Browse your hard drive for the photo  \
		you would like to attach to the project.  When you have found it, click the <b>Open</b> button.  The \
		picture will insert itself into he designed location.<br /> \
		<br /> \
		<b>All Projects:<br /> \
		</b> \
		<br /> \
		Click on the <b>Photo</b> button located in the left hand tool bar.  The Add-A-Photo  \
		dialog box will open and you can browse your hard drive to find a photo.  When you  \
		find the one you want, click the <b>Open</b> button.  The photo will be dropped in the center of the  \
		project.  Double-click on the inserted photo to move it anywhere on the \
		project, or to resize it.<br /> \
		<br /> \
		<b>Resizing and moving photos:</b><br /> \
		<br /> \
		To resize a photo, click the photo to select it, and then drag the handles in any direction to make the photo the size you would like.  \
		Dragging the photo by the corners will maintain the aspect ratio and will not distort the image.<br /> \
		<br /> \
		To move a photo, click and drag it from any point within the photo and move it to a different location.  Use the arrow keys on the keboard to "tweak" the location by a very small amount.<br /> \
		';
	}
	else if (topic == '60')
	{
		SetPrevNextTopic('59', '61');
		strHtml = '\
		<h1>What kind of paper is best to use with HP Design Studio?</h1> \
		<p> \
			HP paper products are specially designed to work with HP printers, HP inks and HP Design Studio, and are available at most office supply  \
			stores and many department stores.  HP greeting card and note card stock are comparable to store-bought  \
			card quality, and are pre-scored to make folding easier.  Other high quality,  \
			heavyweight paper may also be used for your HP Design Studio projects. \
			<br /> \
			<br /> \
			For a complete listing of HP papers and supplies, visit <a target="hp" href="http://www.hpshopping.com/">www.hpshopping.com</a>.</p> \
		';
	}
	else if (topic == '61')
	{
		SetPrevNextTopic('60', '62');
		strHtml = '\
		<h1>How does a half-fold greeting print?</h1> \
		<p> \
			Half-fold greetings print on both sides of the paper.  You will have to  \
			reinsert the paper to print on both sides.  After HP Design Studio has checked  \
			to see what type of printer you have, it may need to run a <b>paper feed print test</b> using a single  \
			sheet of plain paper.  Next, a series of dialog boxes will give you the exact instructions  \
			specific to your printer.  Simply follow the instructions, click the <b>Next</b>  \
			button when prompted, and you will have no difficulty printing your greetings.</p> \
		';
	}
	else if (topic == '62')
	{
		SetPrevNextTopic('61', '63');
		strHtml = '\
		<h1>How does a quarter-fold greeting print?</h1> \
		<p> \
			Quarter-fold greetings print with all of the art and text on one side of the  \
			paper.  After you have printed your card, just fold the paper in half twice, and your greeting will be finished!</p> \
		';
	}
	else if (topic == '63')
	{
		SetPrevNextTopic('62', '64');
		strHtml = '\
		<h1>How many copies of my project can I print?</h1> \
		<p> \
			You can print as many copies as you like.  This feature can be especially  \
			helpful when you are printing invitations, newsletters, labels, envelopes, or  \
			announcements.  Look for the <b>Number of copies</b> field in the <b>Print dialog box</b>.</p> \
		';
	}
	else if (topic == '64')
	{
		SetPrevNextTopic('63', '65');
		strHtml = '\
		<h1>How do I print Envelopes?</h1> \
		<p>All envelopes are categorized by size.  ½-Fold envelopes are the correct size for  \
			½ fold greeting cards, ¼ -Fold envelopes fit the ¼-fold cards and Stationery  \
			Envelopes are designed for the #10 business envelop.  Currently you cannot print  \
			a ½ size envelope with a ¼ envelope design.<br /> \
			To specify how your printer will print the envelopes</p> \
		<ol> \
			<li>Click <b>Paper Feed Settings</b> \
			located within the print dialog box.</li> \
			<li>Set the envelope position (left, center or right).</li> \
			<li>Set the position of the flap (face up or face down).</li> \
			<li>Set the leading edge (this is the edge that is facing the printer).</li> \
			<li>Click <b>OK</b> when ready to print</li> \
		</ol> \
		';
	}
	else if (topic == '65')
	{
		SetPrevNextTopic('64', '66');
		strHtml = '\
		<h1>My note card prints off the page.  What do I do?</h1> \
		<p> \
			When you click the <b>Print</b> button and the dialog box appears, click on the <b>Paper Feed \
			Settings</b> button.  Select the way your printer feeds paper into the printer; either from \
			the left side, centered or from the right side.</p> \
		';
	}
	else if (topic == '66')
	{
		SetPrevNextTopic('65', '0');
		strHtml = '\
		<h1>How can I achieve optimal photo quality when printing?</h1> \
		<p> \
			To achieve the best possible photo quality, set your printer driver quality selection to "Best".</p> \
		';			
	}

	if (strHtml != '')
		document.write(strHtml);
}
