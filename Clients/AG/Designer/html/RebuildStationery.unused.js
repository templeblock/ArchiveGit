			function CSettings()
			{
				this.bg_color				= '';
				this.bg_pad_left			= '';
				this.bg_pad_top				= '';
				this.bg_pad_right			= '';
				this.bg_pad_bottom			= '';

				this.bg_url_enabled			= false;
				this.bg_url					= '';
				this.bg_pos					= '';
				this.bg_repeat_width		= '';
				this.bg_repeat_height		= '';
			//j	this.bg_fixed				= '';

				this.fr_enabled				= false;
				this.fr_color				= '';
				this.fr_pad_left			= '';
				this.fr_pad_top				= '';
				this.fr_pad_right			= '';
				this.fr_pad_bottom			= '';
				this.fr_width				= '';			
				this.fr_height				= '';

				this.fr_url_enabled			= false;
				this.fr_url					= '';
				this.fr_pos					= '';
				this.fr_repeat_width		= '';
				this.fr_repeat_height		= '';
			//j	this.fr_fixed				= '';

				this.gr1_enabled			= false;
				this.gr1_url				= '';
				this.gr1_pos				= '';
				this.gr1_width				= '';
				this.gr1_height				= '';
				this.gr1_zindex				= '';

				this.gr2_enabled			= false;
				this.gr2_url				= '';
				this.gr2_pos				= '';
				this.gr2_width				= '';
				this.gr2_height				= '';
				this.gr2_zindex				= '';

				this.gr3_enabled			= false;
				this.gr3_url				= '';
				this.gr3_pos				= '';
				this.gr3_width				= '';
				this.gr3_height				= '';
				this.gr3_zindex				= '';

				this.gr4_enabled			= false;	
				this.gr4_url				= '';
				this.gr4_pos				= '';
				this.gr4_width				= '';
				this.gr4_height				= '';
				this.gr4_zindex				= '';

				this.gr5_enabled			= false;		
				this.gr5_url				= '';
				this.gr5_pos				= '';
				this.gr5_width				= '';
				this.gr5_height				= '';
				this.gr5_zindex				= '';

				this.gr6_enabled			= false;	
				this.gr6_url				= '';
				this.gr6_pos				= '';
				this.gr6_width				= '';
				this.gr6_height				= '';
				this.gr6_zindex				= '';

				this.text_head_enabled		= false;
				this.text_head				= '';
				this.text_head_font			= '';
				this.text_head_fontsize		= '';
				this.text_head_fontweight	= '';
				this.text_head_fontstyle	= '';
				this.text_head_decoration	= '';
				this.text_head_align		= '';
				this.text_head_color		= '';

				this.text_foot_enabled		= false;
				this.text_foot				= '';
				this.text_foot_font			= '';
				this.text_foot_fontsize		= '';
				this.text_foot_fontweight	= '';
				this.text_foot_fontstyle	= '';	
				this.text_foot_decoration	= '';
				this.text_foot_align		= '';
				this.text_foot_color		= '';

				this.text_body_enabled		= false;
				this.text_body				= '';
				this.text_body_font			= '';
				this.text_body_fontsize		= '';
				this.text_body_fontweight	= '';
				this.text_body_fontstyle	= '';
				this.text_body_decoration	= '';
				this.text_body_align		= '';
				this.text_body_color		= '';

				this.borders_enabled		= false;
				this.bg_bdr_size			= '';
				this.bg_bdr_color			= '';
				this.tb_bdr_size			= '';
				this.tb_bdr_color			= '';
				this.fr_bdr_size			= '';
				this.fr_bdr_color			= '';
				this.gr1_bdr_size			= '';
				this.gr1_bdr_color			= '';
				this.gr2_bdr_size			= '';
				this.gr2_bdr_color			= '';
				this.gr3_bdr_size			= '';
				this.gr3_bdr_color			= '';
				this.gr4_bdr_size			= '';
				this.gr4_bdr_color			= '';
				this.gr5_bdr_size			= '';
				this.gr5_bdr_color			= '';
				this.gr6_bdr_size			= '';
				this.gr6_bdr_color			= '';		

				this.options_enabled		= false;
				this.gr_transparent			= '';
				this.gr_autoplay			= '';
				this.gr_loop				= '';
				this.bg_float				= '';
				this.units					= '';
			}

			function alert(strMessage)
			{
				WScript.Echo(strMessage);
			}
			
			var PaddingUnits = 'px';
			var DefaultSettingsFileName = 'settings.txt';
			var SenderMsgBlockName = '[textmessage]';
			var ForReading = 1, ForWriting = 2, ForAppending = 8;
			var form = null;

			function Initialize(bReset)
			{
				delete form;
				form = new CSettings();

			//	if (bReset)
			//		form.reset();
			//	else
			//	{
			//		// Ensure that all elements (except the input fields) are unselectable
			//		for (i=0; i<document.all.length; i++)
			//			if (document.all(i).type != 'text')
			//				document.all(i).unselectable = 'on';
			//	}

				SetColor('bg_color',        '#ffffff', '');
				SetColor('bg_bdr_color',    '#000000', '');
				SetColor('tb_bdr_color',    '#000000', '');
				SetColor('fr_color',        '#ccffff', '');
				SetColor('fr_bdr_color',    '#000000', '');
				SetColor('gr1_bdr_color',   '#000000', '');
				SetColor('gr2_bdr_color',   '#000000', '');
				SetColor('gr3_bdr_color',   '#000000', '');
				SetColor('gr4_bdr_color',   '#000000', '');
				SetColor('gr5_bdr_color',   '#000000', '');
				SetColor('gr6_bdr_color',   '#000000', '');
				SetColor('text_head_color', '#000000', '');
				SetColor('text_foot_color', '#000000', '');
				SetColor('text_body_color', '#000000', '');

				// Since this is not a refresh, initalize all form elements to their default values
				text_head_Default(0,0);
				text_foot_Default(0,0);
				text_body_Default(0,0);
				bg_Default(0,0);
				fr_Default(0,0);
				gr1_Default(0,0);
				gr2_Default(0,0);
				gr3_Default(0,0);
				gr4_Default(0,0);
				gr5_Default(0,0);
				gr6_Default(0,0);
				export_Default();

				form.bg_pad_left   = 15;
				form.bg_pad_top    = 0;
				form.bg_pad_right  = 15;
				form.bg_pad_bottom = 0;

				form.fr_pad_left   = 15;
				form.fr_pad_top    = 15;
				form.fr_pad_right  = 15;
				form.fr_pad_bottom = 15;

				form.fr_width  = 400;
				form.fr_height = 200;

				form.gr1_width = '';
				form.gr1_height = '';

				form.gr2_width = '';
				form.gr2_height = '';

				form.gr3_width = '';
				form.gr3_height = '';

				form.gr4_width = '';
				form.gr4_height = '';

				form.gr5_width = '';
				form.gr5_height = '';

				form.gr6_width = '';
				form.gr6_height = '';
			}
			function PutCheck(element)
			{
				return element.toString() + '=' + GetCheck(element);
			}
			function PutValue(element)
			{
				return element.toString() + '=' + GetValue(element);
			}
			function PutBlock(element)
			{
				return element.toString() + '=' + eval('escape(form.' + element + ')');
			}
			function PutConstant(element, value)
			{
				return element.toString() + '=' + value;
			}
			function GetCheck(element)
			{
				return eval('form.' + element);
			}
			function GetValue(element)
			{
				return eval('form.' + element);
			}
			function SetCheck(element, value)
			{
				eval('form.' + element + ' = ' + value);
			}
			function SetValue(element, value)
			{
				eval('form.' + element + ' = value');
			}
			function SetBlock(element, value)
			{
				SetValue(element, unescape(value));
			}
			function SetColor(element, color, defcolor)
			{
				SetValue(element, color);
			}
			function text_head_Default(bClear, bPreview)
			{
				var str = 'Text header block';
				SetValue('text_head', str);
			}
			function text_foot_Default(bClear, bPreview)
			{
				var str = 'Text footer block';
				SetValue('text_foot', str);
			}
			function text_body_Default(bClear, bPreview)
			{
				var str = 'The sender\'s message text goes here.  ';
				str += str;
				str += str;
				str += str;
				SetValue('text_body', str);
			}
			function bg_Default(bClear, bPreview)
			{
				var str = 'bg.gif';
				SetValue('bg_url', str);
			}
			function fr_Default(bClear, bPreview)
			{
				var str = 'fr.gif';
				SetValue('fr_url', str);
			}
			function gr1_Default(bClear, bPreview)
			{
				var str = 'gr1.gif';
				SetValue('gr1_url', str);
			}
			function gr2_Default(bClear, bPreview)
			{
				var str = 'gr2.gif';
				SetValue('gr2_url', str);
			}
			function gr3_Default(bClear, bPreview)
			{
				var str = 'gr3.gif';
				SetValue('gr3_url', str);
			}
			function gr4_Default(bClear, bPreview)
			{
				var str = 'gr4.gif';
				SetValue('gr4_url', str);
			}
			function gr5_Default(bClear, bPreview)
			{
				var str = 'gr5.gif';
				SetValue('gr5_url', str);
			}
			function gr6_Default(bClear, bPreview)
			{
				var str = 'gr6.gif';
				SetValue('gr6_url', str);
			}
			function export_Default()
			{
				var str = ''; // Someday get this out of a cookie
				SetValue('export_path', str);
				return true;
			}
			function Open(SettingsPath)
			{
				if (!SettingsPath)
					return false;

				var fso = OpenFSO();
				if (!fso)
					return false;

				if (!fso.FileExists(SettingsPath))
				{
					alert('File ' + SettingsPath + ' does not exist');
					return false;
				}

				Initialize(true/*bReset*/);

				var tso = fso.OpenTextFile(SettingsPath, ForReading, false/*bCreate*/);
				if (!tso)
					return false;

				while (!tso.AtEndOfStream)
				{
					var line = tso.ReadLine();
					if (line == null || line == '')
						continue;
					var namevalue = line.split('=');
					if (namevalue == null)
						continue;
					var element = namevalue[0];
					var value = namevalue[1];
					switch (element)
					{
						case 'bg_color':			{ SetColor(element, value, '#ffffff'); break; }
						case 'bg_pad_left':			{ SetValue(element, value); break; }
						case 'bg_pad_top':			{ SetValue(element, value); break; }
						case 'bg_pad_right':		{ SetValue(element, value); break; }
						case 'bg_pad_bottom':		{ SetValue(element, value); break; }
						case 'bg_url_enabled':		{ SetCheck(element, value); break; }

						case 'bg_url':				{ SetValue(element, value); break; }
						case 'bg_pos':				{ SetValue(element, value); break; }
						case 'bg_repeat_width':		{ SetCheck(element, value); break; }
						case 'bg_repeat_height':	{ SetCheck(element, value); break; }
					//j	case 'bg_fixed':			{ SetCheck(element, value); break; }

						case 'fr_enabled':			{ SetCheck(element, value); break; }
						case 'fr_color':			{ SetColor(element, value, '#ccffff'); break; }
						case 'fr_pad_left':			{ SetValue(element, value); break; }
						case 'fr_pad_top':			{ SetValue(element, value); break; }
						case 'fr_pad_right':		{ SetValue(element, value); break; }
						case 'fr_pad_bottom':		{ SetValue(element, value); break; }
						case 'fr_width':			{ SetValue(element, value); break; }
						case 'fr_height':			{ SetValue(element, value); break; }
						case 'fr_url_enabled':		{ SetCheck(element, value); break; }

						case 'fr_url':				{ SetValue(element, value); break; }
						case 'fr_pos':				{ SetValue(element, value); break; }
						case 'fr_repeat_width':		{ SetCheck(element, value); break; }
						case 'fr_repeat_height':	{ SetCheck(element, value); break; }
					//j	case 'fr_fixed':			{ SetCheck(element, value); break; }

						case 'gr1_enabled':			{ SetCheck(element, value); break; }
						case 'gr1_url':				{ SetValue(element, value); break; }
						case 'gr1_pos':				{ SetValue(element, value); break; }
						case 'gr1_width':			{ SetValue(element, value); break; }
						case 'gr1_height':			{ SetValue(element, value); break; }
						case 'gr1_zindex':			{ SetValue(element, value); break; }

						case 'gr2_enabled':			{ SetCheck(element, value); break; }
						case 'gr2_url':				{ SetValue(element, value); break; }
						case 'gr2_pos':				{ SetValue(element, value); break; }
						case 'gr2_width':			{ SetValue(element, value); break; }
						case 'gr2_height':			{ SetValue(element, value); break; }
						case 'gr2_zindex':			{ SetValue(element, value); break; }

						case 'gr3_enabled':			{ SetCheck(element, value); break; }
						case 'gr3_url':				{ SetValue(element, value); break; }
						case 'gr3_pos':				{ SetValue(element, value); break; }
						case 'gr3_width':			{ SetValue(element, value); break; }
						case 'gr3_height':			{ SetValue(element, value); break; }
						case 'gr3_zindex':			{ SetValue(element, value); break; }

						case 'gr4_enabled':			{ SetCheck(element, value); break; }
						case 'gr4_url':				{ SetValue(element, value); break; }
						case 'gr4_pos':				{ SetValue(element, value); break; }
						case 'gr4_width':			{ SetValue(element, value); break; }
						case 'gr4_height':			{ SetValue(element, value); break; }
						case 'gr4_zindex':			{ SetValue(element, value); break; }

						case 'gr5_enabled':			{ SetCheck(element, value); break; }
						case 'gr5_url':				{ SetValue(element, value); break; }
						case 'gr5_pos':				{ SetValue(element, value); break; }
						case 'gr5_width':			{ SetValue(element, value); break; }
						case 'gr5_height':			{ SetValue(element, value); break; }
						case 'gr5_zindex':			{ SetValue(element, value); break; }

						case 'gr6_enabled':			{ SetCheck(element, value); break; }
						case 'gr6_url':				{ SetValue(element, value); break; }
						case 'gr6_pos':				{ SetValue(element, value); break; }
						case 'gr6_width':			{ SetValue(element, value); break; }
						case 'gr6_height':			{ SetValue(element, value); break; }
						case 'gr6_zindex':			{ SetValue(element, value); break; }

						case 'text_head_enabled':	{ SetCheck(element, value); break; }
						case 'text_head':			{ SetBlock(element, value); break; }
						case 'text_head_font':		{ SetValue(element, value); break; }
						case 'text_head_fontsize':	{ SetValue(element, value); break; }
						case 'text_head_fontweight':{ SetCheck(element, value); break; }
						case 'text_head_fontstyle':	{ SetCheck(element, value); break; }
						case 'text_head_decoration':{ SetCheck(element, value); break; }
						case 'text_head_align':		{ SetValue(element, value); break; }
						case 'text_head_color':		{ SetColor(element, value, '#000000'); break; }

						case 'text_foot_enabled':	{ SetCheck(element, value); break; }
						case 'text_foot':			{ SetBlock(element, value); break; }
						case 'text_foot_font':		{ SetValue(element, value); break; }
						case 'text_foot_fontsize':	{ SetValue(element, value); break; }
						case 'text_foot_fontweight':{ SetCheck(element, value); break; }
						case 'text_foot_fontstyle':	{ SetCheck(element, value); break; }
						case 'text_foot_decoration':{ SetCheck(element, value); break; }
						case 'text_foot_align':		{ SetValue(element, value); break; }
						case 'text_foot_color':		{ SetColor(element, value, '#000000'); break; }

						case 'text_body_enabled':	{ SetCheck(element, value); break; }
						case 'text_body':			{ SetBlock(element, value); break; }
						case 'text_body_font':		{ SetValue(element, value); break; }
						case 'text_body_fontsize':	{ SetValue(element, value); break; }
						case 'text_body_fontweight':{ SetCheck(element, value); break; }
						case 'text_body_fontstyle':	{ SetCheck(element, value); break; }
						case 'text_body_decoration':{ SetCheck(element, value); break; }
						case 'text_body_align':		{ SetValue(element, value); break; }
						case 'text_body_color':		{ SetColor(element, value, '#000000'); break; }

						case 'borders_enabled':		{ SetCheck(element, value); break; }
						case 'bg_bdr_size':			{ SetValue(element, value); break; }
						case 'bg_bdr_color':		{ SetColor(element, value, '#000000'); break; }
						case 'tb_bdr_size':			{ SetValue(element, value); break; }
						case 'tb_bdr_color':		{ SetColor(element, value, '#000000'); break; }
						case 'fr_bdr_size':			{ SetValue(element, value); break; }
						case 'fr_bdr_color':		{ SetColor(element, value, '#000000'); break; }
						case 'gr1_bdr_size':		{ SetValue(element, value); break; }
						case 'gr1_bdr_color':		{ SetColor(element, value, '#000000'); break; }
						case 'gr2_bdr_size':		{ SetValue(element, value); break; }
						case 'gr2_bdr_color':		{ SetColor(element, value, '#000000'); break; }
						case 'gr3_bdr_size':		{ SetValue(element, value); break; }
						case 'gr3_bdr_color':		{ SetColor(element, value, '#000000'); break; }
						case 'gr4_bdr_size':		{ SetValue(element, value); break; }
						case 'gr4_bdr_color':		{ SetColor(element, value, '#000000'); break; }
						case 'gr5_bdr_size':		{ SetValue(element, value); break; }
						case 'gr5_bdr_color':		{ SetColor(element, value, '#000000'); break; }
						case 'gr6_bdr_size':		{ SetValue(element, value); break; }
						case 'gr6_bdr_color':		{ SetColor(element, value, '#000000'); break; }

						case 'options_enabled':		{ SetCheck(element, value); break; }
						case 'gr_transparent':		{ SetCheck(element, value); break; }
						case 'gr_autoplay':			{ SetCheck(element, value); break; }
						case 'gr_loop':				{ SetCheck(element, value); break; }
						case 'bg_float':			{ SetCheck(element, value); break; }
						case 'units':				{ SetValue(element, value); break; }
					}
				}

			// START PATCHES
				// If necessary, change the text body block (sender message block) to the default
				if (GetValue('text_body') == SenderMsgBlockName)
					text_body_Default(0,0);
				
				// Force bg_float to be true
				SetCheck('bg_float', 'true');

				// Force the text size to be 11pt
				SetValue('text_body_fontsize', '11')
				SetValue('units', 'pt');
			// END PATCHES

				if (GetValue('bg_url')  == '')	{ SetCheck('bg_url_enabled', false); bg_Default(0,0); }
				if (GetValue('fr_url')  == '')	{ SetCheck('fr_url_enabled', false); fr_Default(0,0); }
				if (GetValue('gr1_url') == '')	{ SetCheck('gr1_enabled', false);	 gr1_Default(0,0); }
				if (GetValue('gr2_url') == '')	{ SetCheck('gr2_enabled', false);	 gr2_Default(0,0); }
				if (GetValue('gr3_url') == '')	{ SetCheck('gr3_enabled', false);	 gr3_Default(0,0); }
				if (GetValue('gr4_url') == '')	{ SetCheck('gr4_enabled', false);	 gr4_Default(0,0); }
				if (GetValue('gr5_url') == '')	{ SetCheck('gr5_enabled', false);	 gr5_Default(0,0); }
				if (GetValue('gr6_url') == '')	{ SetCheck('gr6_enabled', false);	 gr6_Default(0,0); }
				
				var SettingsFolder = fso.GetParentFolderName(SettingsPath);
				if (GetCheck('bg_url_enabled'))	SetValue('bg_url',  VerifyFile(GetValue('bg_url'),  SettingsFolder, false/*bStripPath*/));
				if (GetCheck('fr_url_enabled'))	SetValue('fr_url',  VerifyFile(GetValue('fr_url'),  SettingsFolder, false/*bStripPath*/));
				if (GetCheck('gr1_enabled'))	SetValue('gr1_url', VerifyFile(GetValue('gr1_url'), SettingsFolder, false/*bStripPath*/));
				if (GetCheck('gr2_enabled'))	SetValue('gr2_url', VerifyFile(GetValue('gr2_url'), SettingsFolder, false/*bStripPath*/));
				if (GetCheck('gr3_enabled'))	SetValue('gr3_url', VerifyFile(GetValue('gr3_url'), SettingsFolder, false/*bStripPath*/));
				if (GetCheck('gr4_enabled'))	SetValue('gr4_url', VerifyFile(GetValue('gr4_url'), SettingsFolder, false/*bStripPath*/));
				if (GetCheck('gr5_enabled'))	SetValue('gr5_url', VerifyFile(GetValue('gr5_url'), SettingsFolder, false/*bStripPath*/));
				if (GetCheck('gr6_enabled'))	SetValue('gr6_url', VerifyFile(GetValue('gr6_url'), SettingsFolder, false/*bStripPath*/));

				tso.Close();
				return true;
			}
			function Export(SettingsPath)
			{
				var fso = OpenFSO();
				if (!fso)
					return;

				// The SettingsPath is a full specification of the settings file
				var SettingsFile = fso.GetFileName(SettingsPath);
				var SettingsFolder = fso.GetParentFolderName(SettingsPath);

				try
				{
					var f = fso.CreateFolder(SettingsFolder);
				//j	alert(f.Path); // Debug
				}
				catch(error)
				{
					if (error.description.indexOf('already exists') < 0)
					{
						var FsoError = 'Could not create folder "' + SettingsFolder + '"';
						alert(error.description + '\n\n' + FsoError);
						return;
					}
				}

				var HtmlPath = SettingsFolder + '\\index.html';
				try
				{
					var tso = fso.OpenTextFile(HtmlPath, ForWriting, true/*bCreate*/);
					tso.WriteLine('<!doctype html public "-//W3C//DTD HTML 4.0 Transitional//EN">');
					tso.WriteLine('<html><head><title></title></head>');
					tso.WriteLine(GetBody(SettingsFolder));
					tso.WriteLine('</html>');
					tso.Close();

					// Save the settings last
					var NewSettingsPath = SettingsFolder + '\\' + SettingsFile;
					SaveSettings(fso, NewSettingsPath);
				//j	alert('Settings saved to ' + NewSettingsPath); // Debug
				}
				catch(error)
				{
					var FsoError = 'Could not export HTML to "' + HtmlPath + '"';
					alert(error.description + '\n\n' + FsoError);
				}
			}
			function SaveSettings(fso, SettingsPath)
			{
				try
				{
					var tso = fso.OpenTextFile(SettingsPath, ForWriting, true/*bCreate*/);
					if (!tso)
						return;

					tso.WriteLine(PutValue('bg_color'));
					tso.WriteLine(PutValue('bg_pad_left'));
					tso.WriteLine(PutValue('bg_pad_top'));
					tso.WriteLine(PutValue('bg_pad_right'));
					tso.WriteLine(PutValue('bg_pad_bottom'));
					tso.WriteLine(PutCheck('bg_url_enabled'));
					if (GetCheck('bg_url_enabled'))
					{
						tso.WriteLine(PutValue('bg_url'));
						tso.WriteLine(PutValue('bg_pos'));
						tso.WriteLine(PutCheck('bg_repeat_width'));
						tso.WriteLine(PutCheck('bg_repeat_height'));
					//j	tso.WriteLine(PutCheck('bg_fixed'));
					}

					tso.WriteLine(PutCheck('fr_enabled'));
					if (GetCheck('fr_enabled'))
					{
						tso.WriteLine(PutValue('fr_color'));
						tso.WriteLine(PutValue('fr_pad_left'));
						tso.WriteLine(PutValue('fr_pad_top'));
						tso.WriteLine(PutValue('fr_pad_right'));
						tso.WriteLine(PutValue('fr_pad_bottom'));
						tso.WriteLine(PutValue('fr_width'));
						tso.WriteLine(PutValue('fr_height'));
						tso.WriteLine(PutCheck('fr_url_enabled'));
						if (GetCheck('fr_url_enabled'))
						{
							tso.WriteLine(PutValue('fr_url'));
							tso.WriteLine(PutValue('fr_pos'));
							tso.WriteLine(PutCheck('fr_repeat_width'));
							tso.WriteLine(PutCheck('fr_repeat_height'));
						//j	tso.WriteLine(PutCheck('fr_fixed'));
						}
					}

					tso.WriteLine(PutCheck('gr1_enabled'));
					if (GetCheck('gr1_enabled'))
					{
						tso.WriteLine(PutValue('gr1_url'));
						tso.WriteLine(PutValue('gr1_pos'));
						tso.WriteLine(PutValue('gr1_width'));
						tso.WriteLine(PutValue('gr1_height'));
						tso.WriteLine(PutValue('gr1_zindex'));
					}
					
					tso.WriteLine(PutCheck('gr2_enabled'));
					if (GetCheck('gr2_enabled'))
					{
						tso.WriteLine(PutValue('gr2_url'));
						tso.WriteLine(PutValue('gr2_pos'));
						tso.WriteLine(PutValue('gr2_width'));
						tso.WriteLine(PutValue('gr2_height'));
						tso.WriteLine(PutValue('gr2_zindex'));
					}

					tso.WriteLine(PutCheck('gr3_enabled'));
					if (GetCheck('gr3_enabled'))
					{
						tso.WriteLine(PutValue('gr3_url'));
						tso.WriteLine(PutValue('gr3_pos'));
						tso.WriteLine(PutValue('gr3_width'));
						tso.WriteLine(PutValue('gr3_height'));
						tso.WriteLine(PutValue('gr3_zindex'));
					}

					tso.WriteLine(PutCheck('gr4_enabled'));
					if (GetCheck('gr4_enabled'))
					{
						tso.WriteLine(PutValue('gr4_url'));
						tso.WriteLine(PutValue('gr4_pos'));
						tso.WriteLine(PutValue('gr4_width'));
						tso.WriteLine(PutValue('gr4_height'));
						tso.WriteLine(PutValue('gr4_zindex'));
					}

					tso.WriteLine(PutCheck('gr5_enabled'));
					if (GetCheck('gr5_enabled'))
					{
						tso.WriteLine(PutValue('gr5_url'));
						tso.WriteLine(PutValue('gr5_pos'));
						tso.WriteLine(PutValue('gr5_width'));
						tso.WriteLine(PutValue('gr5_height'));
						tso.WriteLine(PutValue('gr5_zindex'));
					}

					tso.WriteLine(PutCheck('gr6_enabled'));
					if (GetCheck('gr6_enabled'))
					{
						tso.WriteLine(PutValue('gr6_url'));
						tso.WriteLine(PutValue('gr6_pos'));
						tso.WriteLine(PutValue('gr6_width'));
						tso.WriteLine(PutValue('gr6_height'));
						tso.WriteLine(PutValue('gr6_zindex'));
					}

					tso.WriteLine(PutCheck('text_head_enabled'));
					if (GetCheck('text_head_enabled'))
					{
						tso.WriteLine(PutBlock('text_head'));
						tso.WriteLine(PutValue('text_head_font'));
						tso.WriteLine(PutValue('text_head_fontsize'));
						tso.WriteLine(PutCheck('text_head_fontweight'));
						tso.WriteLine(PutCheck('text_head_fontstyle'));
						tso.WriteLine(PutCheck('text_head_decoration'));
						tso.WriteLine(PutValue('text_head_align'));
						tso.WriteLine(PutValue('text_head_color'));
					}

					tso.WriteLine(PutCheck('text_foot_enabled'));
					if (GetCheck('text_foot_enabled'))
					{
						tso.WriteLine(PutBlock('text_foot'));
						tso.WriteLine(PutValue('text_foot_font'));
						tso.WriteLine(PutValue('text_foot_fontsize'));
						tso.WriteLine(PutCheck('text_foot_fontweight'));
						tso.WriteLine(PutCheck('text_foot_fontstyle'));
						tso.WriteLine(PutCheck('text_foot_decoration'));
						tso.WriteLine(PutValue('text_foot_align'));
						tso.WriteLine(PutValue('text_foot_color'));
					}

					tso.WriteLine(PutCheck('text_body_enabled'));
					if (GetCheck('text_body_enabled'))
					{
						tso.WriteLine(PutConstant('text_body', SenderMsgBlockName));
					//j	tso.WriteLine(PutBlock('text_body'));
						tso.WriteLine(PutValue('text_body_font'));
						tso.WriteLine(PutValue('text_body_fontsize'));
						tso.WriteLine(PutCheck('text_body_fontweight'));
						tso.WriteLine(PutCheck('text_body_fontstyle'));
						tso.WriteLine(PutCheck('text_body_decoration'));
						tso.WriteLine(PutValue('text_body_align'));
						tso.WriteLine(PutValue('text_body_color'));
					}

					tso.WriteLine(PutCheck('borders_enabled'));
					if (GetCheck('borders_enabled'))
					{
						tso.WriteLine(PutValue('bg_bdr_size'));
						tso.WriteLine(PutValue('bg_bdr_color'));
						tso.WriteLine(PutValue('tb_bdr_size'));
						tso.WriteLine(PutValue('tb_bdr_color'));
						tso.WriteLine(PutValue('fr_bdr_size'));
						tso.WriteLine(PutValue('fr_bdr_color'));
						tso.WriteLine(PutValue('gr1_bdr_size'));
						tso.WriteLine(PutValue('gr1_bdr_color'));
						tso.WriteLine(PutValue('gr2_bdr_size'));
						tso.WriteLine(PutValue('gr2_bdr_color'));
						tso.WriteLine(PutValue('gr3_bdr_size'));
						tso.WriteLine(PutValue('gr3_bdr_color'));
						tso.WriteLine(PutValue('gr4_bdr_size'));
						tso.WriteLine(PutValue('gr4_bdr_color'));
						tso.WriteLine(PutValue('gr5_bdr_size'));
						tso.WriteLine(PutValue('gr5_bdr_color'));
						tso.WriteLine(PutValue('gr6_bdr_size'));
						tso.WriteLine(PutValue('gr6_bdr_color'));
					}

					tso.WriteLine(PutCheck('options_enabled'));
					tso.WriteLine(PutCheck('gr_transparent'));
					tso.WriteLine(PutCheck('gr_autoplay'));
					tso.WriteLine(PutCheck('gr_loop'));
					tso.WriteLine(PutCheck('bg_float'));
					tso.WriteLine(PutValue('units'));

					tso.Close();
				}
				catch(error)
				{
					var FsoError = 'Could not save settings to "' + SettingsPath + '"';
					alert(error.description + '\n\n' + FsoError);
				}
			}
			function Break(text)
			{
				var replacetext = '<br>';
				text = escape(text)
				var index = -1;
				while (text.indexOf('%0D%0A') > -1)
					text = text.replace('%0D%0A', replacetext);
				while (text.indexOf('%0A') > -1)
					text = text.replace('%0A', replacetext);
				while (text.indexOf('%0D') > -1)
					text = text.replace('%0D', replacetext);

				return unescape(text)
			}
			var buffer = '';
			function Buffer(text)
			{
				buffer += text;
			}
			function CopyFile(filepath, folder)
			{
				if (!folder)
					return null;
					
				var fso = OpenFSO();
				if (!fso)
					return null;

			//	fso.GetParentFolderName(filepath)	Path, no Filename, no trailing slash
			//	fso.GetDriveName(filepath)			\\Gromit\wallaby (empty for http path)
			//	fso.GetBaseName(filepath)			Filename, no extension
			//	fso.GetAbsolutePathName(filepath)	Full path (weird for http path)
			//	fso.GetExtensionName(filepath)		Extension
			///	fso.GetFile(filepath)				Full path (File not found for http path)
			//	fso.GetFileName(filepath)			Filename with extension
				try
				{
					var filename = fso.GetFileName(filepath);
					var separator = (folder.indexOf('/') >= 0 ? '/' : '\\');
					var newfilepath = folder + separator + filename;
					var fp1 = filepath.toUpperCase();
					var fp2 = newfilepath.toUpperCase();
//j					if (fp1 != fp2)
//j						fso.CopyFile(filepath, newfilepath, false/*bOverwrite*/);
					return newfilepath;
				}
				catch(error)
				{
					var bIsHttp = (filepath.indexOf('http:') >= 0);
					var FsoError = error.description + '\n\n' + filepath;
					if (bIsHttp)
						FsoError += '\n\nImages referenced from the Web cannot be exported.  However, the exported page will continue to reference this image from the Web.\n\nIf you wish, change the reference to a local hard disk or file server and save again.'
					alert(FsoError);
					return null;
				}

				return null;
			}
			function VerifyFile(filepath, SettingsFolder, bStripPath)
			{
			//j	var bIsHttp = (filepath.indexOf('http:') >= 0);
			//j	if (bIsHttp)
			//j		return filepath;

				var fso = OpenFSO();
				if (!fso)
					return filepath;

				var filename = fso.GetFileName(filepath);
				var filepath2 = SettingsFolder + '\\' + filename;
				if (fso.FileExists(filepath2))
					return (bStripPath ? filename : filepath2);

				if (fso.FileExists(filepath))
					return filepath;

				alert('File verification failed for\n\n"' + filepath2 + '"');
				return filepath2;
			}
			function BufferBackgroundStyle(type, defcolor, SettingsFolder)
			{
				var color         = GetValue(type + '_color');
				var url_enabled   = GetCheck(type + '_url_enabled');         
				var repeat_width  = GetCheck(type + '_repeat_width');         
				var repeat_height = GetCheck(type + '_repeat_height');         
				var pos           = GetValue(type + '_pos');         
				var fixed         = GetCheck(type + '_fixed');         
				var url           = GetValue(type + '_url');         
				if (SettingsFolder && url_enabled && url != '')
				{
					var newfilepath = CopyFile(url, SettingsFolder);
					if (newfilepath)
					{
						SetValue(type + '_url', newfilepath);
						url = VerifyFile(newfilepath, SettingsFolder, true/*bStripPath*/);
					}
				}

				if (color == '') color = defcolor;

				Buffer('background-color:' + color + '; ');
				if (url_enabled && url != '')
				{
					Buffer('background-image:url(' + url + '); ');
					var repeat = 'no-repeat';
					if (repeat_width)
						repeat = (repeat_height ? 'both' : 'repeat-x');
					else
					if (repeat_height)
						repeat = 'repeat-y';
					Buffer('background-repeat:' + repeat + '; ');
					Buffer('background-position:' + pos + '; ');
					Buffer('background-attachment:' + (fixed ? 'fixed' : 'scroll') + '; ');
				}
			}
			function BufferTextStyle(type, bUnderline, bAlign)
			{
				var text_font        = GetValue(type + '_font');         
				var text_fontsize    = GetValue(type + '_fontsize');
				var text_fontweight  = GetCheck(type + '_fontweight');
				var text_fontstyle   = GetCheck(type + '_fontstyle');
				var text_decoration  = GetCheck(type + '_decoration');
				var text_align       = GetValue(type + '_align');
				var text_color       = GetValue(type + '_color');
				if (text_color == '') text_color = '#000000';

				Buffer('font-family:'     + text_font + '; ');
				Buffer('font-size:'       + text_fontsize + GetValue('units') + '; ');
				Buffer('font-weight:'     + (text_fontweight ? 'bold' : 'normal') + '; ');
				Buffer('font-style:'      + (text_fontstyle ? 'italic' : 'normal') + '; ');
				Buffer('color:'           + text_color + '; ');
				if (bAlign)
					Buffer('text-align:'      + text_align + '; ');
				if (bUnderline)
					Buffer('text-decoration:' + (text_decoration ? 'underline' : 'none') + '; ');
			}
			function BufferText(type, bSenderMsgBlock, SettingsFolder)
			{
				if (!GetCheck(type + '_enabled'))
					return;

				Buffer(bSenderMsgBlock ? '<tr><td style="width:100%; height:100%;">' : '<tr><td>');
				Buffer('<div contenteditable="true" style="position:relative; ');
				BufferTextStyle(type, true, true);
				if (bSenderMsgBlock)
				{
					Buffer('width:100%; height:100%; ');
					var bBorders = GetCheck('borders_enabled');
					if (bBorders && (GetValue('tb_bdr_size') != ''))
					{
						var tb_bdr_color = GetValue('tb_bdr_color');
						if (tb_bdr_color == '') tb_bdr_color = '#000000';
						Buffer('border:' + GetValue('tb_bdr_size') + ' solid ' + tb_bdr_color + '; ');
					}
				}
				Buffer('">');
				var strText = (bSenderMsgBlock && SettingsFolder ? SenderMsgBlockName : Break(GetValue(type)));
				Buffer(strText);
				Buffer('</div>');
				Buffer('</td></tr>');
			}
			function BufferGraphic(type, bIfNormal, SettingsFolder)
			{
				if (!GetCheck(type + '_enabled'))
					return;

				var gr_pos = GetValue(type + '_pos');
				var bNormal = (gr_pos.indexOf('normal') >= 0);
				if (bNormal != bIfNormal)
					return;

				var gr_zindex       = GetValue(type + '_zindex');
				var gr_width        = GetValue(type + '_width');
				var gr_height       = GetValue(type + '_height');
				var gr_url          = GetValue(type + '_url');
				var gr_bdr_size     = GetValue(type + '_bdr_size');
				var gr_bdr_color    = GetValue(type + '_bdr_color');
				if (gr_bdr_color == '') gr_bdr_color = '#000000';

				if (SettingsFolder && gr_url != '')
				{
					var newfilepath = CopyFile(gr_url, SettingsFolder);
					if (newfilepath)
					{
						SetValue(type + '_url', newfilepath);
						gr_url = VerifyFile(newfilepath, SettingsFolder, true/*bStripPath*/);
					}
				}

				var urltest = gr_url.toLowerCase();
				var bGIF = (urltest.indexOf('.gif') >= 0);
				var bJPG = (urltest.indexOf('.jpg') >= 0);
				var bFlash = (urltest.indexOf('.swf') >= 0);
				if (!bGIF && !bJPG && !bFlash)
					return;

				var bTile = (gr_pos.indexOf('tile') >= 0);
				var bLeft = (gr_pos.indexOf('left') >= 0);
				var bCenter = (gr_pos.indexOf('center') >= 0);
				var bRight = (gr_pos.indexOf('right') >= 0);
				var bTop = (gr_pos.indexOf('top') >= 0);
				var bMiddle = (gr_pos.indexOf('middle') >= 0);
				var bBottom = (gr_pos.indexOf('bottom') >= 0);
				var bHorz = (bTop || bMiddle || bBottom);
				var bVert = (bLeft || bCenter || bRight);
				var bBorders = GetCheck('borders_enabled');

				var bg_color = GetValue('bg_color');
				if (bg_color == '') bg_color = '#ffffff';

				var fr_color = GetValue('fr_color');
				if (fr_color == '') fr_color = '#ccffff';

				if (bNormal)
					Buffer('<tr><td>');
				Buffer('<div contenteditable="false" unselectable="on" ');

				if (bNormal)
				{
					Buffer('style="position:relative; " ');
					if (bLeft)
						Buffer('align="left">');
					else
					if (bRight)
						Buffer('align="right">');
					else // default
						Buffer('align="center">');
				}
				else
				{
					Buffer('style="position:absolute; z-index:' + gr_zindex + '; ');
					if (bCenter)
					{
						var half = gr_width / 2;
						if (!half)
						{
							var number = type.replace('gr', '');
							alert('Be sure to enter the width of Graphic ' + number + ' in order to place it "top center" or "bottom center".');
						}
						Buffer('left:50%; margin-left:-' + half + 'px; ');
					}
					if (bLeft)
						Buffer('left:0px; ');
					if (bRight)
						Buffer('right:-1px; ');
					if (bTop)
						Buffer('top:0px; ');
					if (bBottom)
						Buffer('bottom:-1px; ');

					if (bTile)
					{
						if (bHorz)
						{
							Buffer('width:100%; ');
						//j	if (bFlash && gr_width == '')
							if (bFlash)
								gr_width = '100%';
						}
						if (bVert)
						{
							Buffer('height:100%; ');
						//j	if (bFlash && gr_height == '')
							if (bFlash)
								gr_height = '100%';
						}
						if (!bFlash)
						{
							Buffer('background-image:url(' + gr_url + '); ');
							Buffer('background-repeat:' + (bVert && bHorz ? 'both' : (bVert ? 'repeat-y' : 'repeat-x')) + '; ');
							Buffer('background-position:left top; ');
							Buffer('background-attachment:scroll; ');
						}
					}
					
					if (bBorders && gr_bdr_size != '')
						Buffer('border:' + gr_bdr_size + ' solid ' + gr_bdr_color + '; ');

					Buffer('">');
				}

				Buffer((bFlash ? '<object' : '<img') + ' unselectable="on" id="' + type + '" style="');
				if (bNormal)
				{
					if (bBorders && gr_bdr_size != '')
						Buffer('border:' + gr_bdr_size + ' solid ' + gr_bdr_color + '; ');
				}
			//j	if (bFlash && gr_width == '' && gr_height == '')
			//j		gr_width = '100%';
				if (gr_width != '')
					Buffer('width:' + gr_width + '; ');
				if (gr_height != '')
					Buffer('height:' + gr_height + '; ');
				Buffer('" ');

				if (!bFlash)
				{
					Buffer('src="' + gr_url + '" alt="">');
				}
				else
				{
					var strPrefix = ((!SettingsFolder && gr_url.indexOf('://') < 0) ? 'file://' : '');
					var url = strPrefix + gr_url;
					
					Buffer('codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0" ');
					Buffer('classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000">');
					Buffer('<param name="Movie" value="' + url + '">');
				//j	Buffer('<param name="Src" value="' + url + '">');
					Buffer('<param name="Quality" value="High">');
					Buffer('<param name="BGColor" value="' + (GetCheck('fr_enabled') ? fr_color : bg_color) + '">');
					Buffer('<param name="Play" value="' + (GetCheck('gr_autoplay') ? '-1' : '0') + '">');
					Buffer('<param name="Loop" value="' + (GetCheck('gr_loop') ? '-1' : '0') + '">');
					Buffer('<param name="Menu" value="-1">');
					Buffer('<param name="WMode" value="' + (GetCheck('gr_transparent') ? 'Transparent' : 'Window') + '">');
					Buffer('<param name="Scale" value="' + (bTile ? 'ExactFit' : 'ShowAll') + '">');
				//j	Buffer('<param name="SAlign" value="">');
				//j	Buffer('<param name="Menu" value="-1">');
				//j	Buffer('<param name="Base" value="">');
				//j	Buffer('<param name="AllowScriptAccess" value="always">');
				//j	Buffer('<param name="DeviceFont" value="0">');
				//j	Buffer('<param name="EmbedMovie" value="-1">');
				//j	Buffer('<param name="SWRemote" value="">');
				//j	Buffer('<param name="_cx" value="14552">');
				//j	Buffer('<param name="_cy" value="10583">');
				//j	Buffer('<param name="FlashVars" value="">');
					Buffer('<embed ');
					Buffer('src="' + url + '" ');
					Buffer('quality="high" ');
					Buffer('swLiveConnect="false" ');
					Buffer('type="application/x-shockwave-flash" ');
					Buffer('pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash" ');
					Buffer('>');
					Buffer('</embed>');
					Buffer('</object>');
				}

				Buffer('</div>');
				if (bNormal)
					Buffer('</td></tr>');
			}
			function BufferPadding(type)
			{
				var l = GetValue(type + '_pad_left');
				var r = GetValue(type + '_pad_right');
				var t = GetValue(type + '_pad_top');
				var b = GetValue(type + '_pad_bottom');
				if (l == '') l = '0';
				if (r == '') r = '0';
				if (t == '') t = '0';
				if (b == '') b = '0';
				Buffer('padding-left:' + l + PaddingUnits + '; ');
				Buffer('padding-right:' + r + PaddingUnits + '; ');
				Buffer('padding-top:' + t + PaddingUnits + '; ');
				Buffer('padding-bottom:' + b + PaddingUnits + '; ');
			}
			function GetBody(SettingsFolder)
			{
				// Issue: absolute vs. relative
				//		absolute (adv - can get all the way to the edges of the document on a cut/paste)
				//		relative (adv - no fixed images during replys)
				// Issue: body bg img vs. div bg img (bg_float)
				//		body bg img (adv - get fixed scroll bg feature)
				//		div bg img (adv - can cut and paste the bg, and reply has no bg to get in way)
				// Issue: to use contenteditable or not
				//		use contenteditable (adv - avoid typing in arbitrary areas; dis - could be unsupported on some platforms)
				//		don't use contenteditable (adv - can move images around)
				// Issue: to use divs or tables
				//		div's (adv - can use contenteditable, simpler)
				//		table's (adv - none)
				// Other things I have learned:
				//		Don't stylize the body if you want to be able to cut and paste the entire document
				//		Don't use style sheets because they won't copy or paste

				var bBorders = GetCheck('borders_enabled');
				var bFloat = GetCheck('bg_float');
				
				buffer = '';

				// Start the body tag
				Buffer('<body style="margin:0px; ');
				if (!bFloat)
					BufferBackgroundStyle('bg', '#ffffff', SettingsFolder);
				Buffer('">');

				// Start the outer container div tag that contains any background image
				Buffer('<div contenteditable="false" unselectable="on" style="position:relative; width:100%; height:100%; ');
			//j	BufferTextStyle('text_body', false, false); // Should be unnecessary, but what the heck
				if (bFloat)
					BufferBackgroundStyle('bg', '#ffffff', SettingsFolder);
				if (bBorders && (GetValue('bg_bdr_size') != ''))
				{
					var bg_bdr_color = GetValue('bg_bdr_color');
					if (bg_bdr_color == '') bg_bdr_color = '#000000';
					Buffer('border:' + GetValue('bg_bdr_size') + ' solid ' + bg_bdr_color + '; ');
				}
				Buffer('">');

				// Output any graphics that float, wrapped in div tags
				BufferGraphic('gr1', false, SettingsFolder);
				BufferGraphic('gr2', false, SettingsFolder);
				BufferGraphic('gr3', false, SettingsFolder);
				BufferGraphic('gr4', false, SettingsFolder);
				BufferGraphic('gr5', false, SettingsFolder);
				BufferGraphic('gr6', false, SettingsFolder);

				// Start the inner padding div tag
				Buffer('<div contenteditable="false" unselectable="on" style="position:relative; width:100%; height:100%; ');  
			//j	BufferTextStyle('text_body', false, false); // Should be unnecessary, but what the heck
				BufferPadding('bg');
				Buffer('" align="center">');

				if (GetCheck('fr_enabled'))
				{
					// Start the inner frame div tag
					Buffer('<div contenteditable="false" unselectable="on" style="position:relative; ');
					BufferTextStyle('text_body', false, true);
					BufferBackgroundStyle('fr', '#ccffff', SettingsFolder);
					BufferPadding('fr');
					if (bBorders && (GetValue('fr_bdr_size') != ''))
					{
						var fr_bdr_color = GetValue('fr_bdr_color');
						if (fr_bdr_color == '') fr_bdr_color = '#000000';
						Buffer('border:' + GetValue('fr_bdr_size') + ' solid ' + fr_bdr_color + '; ');
					}
					var fr_width = GetValue('fr_width');
					if (fr_width != '')
						Buffer('width:' + fr_width + '; ');
					var fr_height = GetValue('fr_height');
					if (fr_height != '')
						Buffer('height:' + fr_height + '; ');
					Buffer('">');
				}

				// Output anything that flows normally; must be wrapped in a table
				Buffer('<table unselectable="on" cellspacing="0" cellpadding="0" style="width:100%; height:100%; ');
			//j	BufferTextStyle('text_body', false, false); // Should be unnecessary, but what the heck
				Buffer('"><tbody unselectable="on">');
				BufferGraphic('gr1', true, SettingsFolder);
				BufferText('text_head', false, SettingsFolder);
				BufferGraphic('gr2', true, SettingsFolder);
				BufferText('text_foot', false, SettingsFolder);
				BufferGraphic('gr3', true, SettingsFolder);
				BufferText('text_body', true, SettingsFolder);
				BufferGraphic('gr4', true, SettingsFolder);
				BufferGraphic('gr5', true, SettingsFolder);
				BufferGraphic('gr6', true, SettingsFolder);
				// Add padding if the text body block (sender message block) is not displayed
				if (!GetCheck('text_body_enabled'))
					Buffer('<tr><td style="width:100%; height:100%"></td></tr>');
				Buffer('</tbody></table>');

				// End the inner frame div tag
				if (GetCheck('fr_enabled'))
					Buffer('</div>');

				// End the inner padding div tag
				Buffer('</div>');

				// End the outer container div tag
				Buffer('</div>');

				Buffer('</body>');
				return buffer;
			}

			//	fso.GetParentFolderName(filepath)	Path, no Filename, no trailing slash
			//	fso.GetDriveName(filepath)			\\Gromit\wallaby (empty for http path)
			//	fso.GetBaseName(filepath)			Filename, no extension
			//	fso.GetAbsolutePathName(filepath)	Full path (weird for http path)
			//	fso.GetExtensionName(filepath)		Extension
			//	fso.GetFile(filepath)				Full path (File not found for http path)
			//	fso.GetFileName(filepath)			Filename with extension

			function OpenFSO()
			{
				try
				{
					var fso = new ActiveXObject('Scripting.FileSystemObject');
					if (!fso)
						throw '';
					return fso;
				}
				catch (error)
				{
					var FsoError = 'Your security settings are preventing access to the file utilities.  To correct this problem, use the Internet Explorer Tools menu to access the InternetOptions/Security/LocalIntranet/DefaultLevel button.  Then set the Security Level slider to "Low".';
					alert(error.description + '\n\n' + FsoError);
					return null;
				}
			}

			function OpenSaveDialog(bSave, Filter, DefaultExt, InitDir, DefFileName)
			{
				// Filter is of the form: 'Text Settings Files (*.txt)|*.txt|All Files (*.*)|*.*';
				var dialog = null;
				try
				{
					dialog = new ActiveXObject('MSComDlg.CommonDialog');
					if (!dialog)
						throw '';
				}
				catch (error)
				{
					alert('The common dialogs do not exist');
					return null;
				}

				// some common dialog constants...
				var OFN_HIDEREADONLY = 0x4;
				var OFN_CREATEPROMPT = 0x2000;
				var OFN_EXPLORER = 0x80000;
				var OFN_LONGNAMES = 0x200000;
				
				// setup parameters...
				dialog.Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_CREATEPROMPT | OFN_HIDEREADONLY;
				dialog.MaxFileSize = 260;		// allocate space for file name
				dialog.InitDir = InitDir;		// set opening directory
				dialog.DefaultExt = DefaultExt;	// default extension
				dialog.Filter = Filter;			// show file types
				dialog.FilterIndex = 1;			// show all files
				dialog.FileName = DefFileName;	// set default filename

				if (bSave)
					dialog.ShowSave();
				else
					dialog.ShowOpen();

				// retrieve the result...
  				var filepath = dialog.FileName;
				if (!filepath)
					filepath = '';
				return filepath;
			}

			function MakeFolder(fso, Folder)
			{
				try
				{
					if (!fso.FolderExists(Folder))
						var f = fso.CreateFolder(Folder);
					return true;
				}
				catch (error)
				{
					var FsoError = 'Could not create Folder "' + Folder + '"';
					alert(error.description + '\n\n' + FsoError);
					return false;
				}
			}

			function CopyFile2(fso, Source, Destination)
			{
				try
				{
					if (fso.FileExists(Destination))
						return true;
					fso.CopyFile(Source, Destination, false/*bOverwrite*/);
					return true;
				}
				catch (error)
				{
					var FsoError = 'Could not copy "' + Source + '" to "' + Destination + '"';
					alert(error.description + '\n\n' + FsoError);
					return false;
				}

				return true;
			}

			function Trim(str)
			{
				// Match spaces at beginning and end of text and replace with null strings
				return str.replace(/^\s+/,'').replace(/\s+$/,'');
			}

			function FilterFileName(FileName)
			{
				Trim(FileName);
				// Remove any illegal UNC filename characters
				var regexp = /[?*|":<>\\\/\t]/gi;
				return FileName.replace(regexp, '-');
			}

			function FileCountLines(FileName)
			{
				var fso = OpenFSO();
				if (!fso)
					return 0;

				var iCount = 0;
				var tso = fso.OpenTextFile(FileName, ForReading);
				if (!tso)
					return 0;

				while (!tso.AtEndOfStream)
				{
					var Line = tso.ReadLine();
					iCount++;
				}

				return iCount;
			}

			function ProcessCSV(FileName)
			{
				var fso = OpenFSO();
				if (!fso)
				{
					alert('File system object could not be created');
					return;
				}

				if (FileName)
				{
					if (!fso.FileExists(FileName))
					{
						alert('File ' + FileName + ' does not exist');
						FileName = null;
					}
				}

				if (!FileName)
				{
					FileName = OpenSaveDialog(false/*bSave*/,
						'Comma Separated Values Files (*.csv)|*.csv|All Files (*.*)|*.*'/*Filter*/,
						'txt'/*DefaultExt*/,
						'c:\\'/*InitDir*/,
						''/*DefFileName*/);
					if (!FileName)
						return;
				}

				var MaxLines = FileCountLines(FileName);
				var MaxFields = 1;

				var tso = fso.OpenTextFile(FileName, ForReading);
				if (!tso)
					return;

				// skip first line which contains format definition
				tso.SkipLine(); 

				// Compute the output folder
				var index = FileName.lastIndexOf('\\');
				var strFolder = FileName.substr(0, index+1);

				// read the file line by line
				var Progress = new ProgressDialog(strFolder);
				Progress.SetHeading('Offline Content Builder');

				iFixCount = 0;

				var iCount = 0;
				while (!tso.AtEndOfStream)
				{  
					var Line = tso.ReadLine();
					var Fields = Line.split(',');
					if (Fields.length != MaxFields)
					{
						alert('This record has', Fields.length, 'fields.  It should have ' + MaxFields + '.\n\n' + Line);
						continue;
					}

					var PATH = Fields[0];

					var TITLE = PATH;
					var index = TITLE.lastIndexOf('\\settings.txt');
					TITLE = TITLE.substr(0, index);
					index = TITLE.lastIndexOf('\\');
					TITLE = TITLE.substr(index+1);

					Progress.SetMessage(TITLE);
					Progress.SetProgress(tso.Line, MaxLines);

					var SettingsPath = PATH;

					// Make a copy of the settings file for debug purposes
					var SettingsCopyPath = SettingsPath + ".orig.txt";
					if (fso.FileExists(SettingsPath) && !fso.FileExists(SettingsCopyPath))
						CopyFile2(fso, SettingsPath, SettingsCopyPath);

					if (Open(SettingsPath))
					{
						Export(SettingsPath)
					}

				//j	if (++iCount > 0)
				//j		break;
				}

				tso.Close();
				Progress.Close();
				Progress = null;
				alert('Finished: ' + iCount + ' HTML files written');
			}

			function ProgressDialog(strFolder)
			{
				this.READYSTATE_COMPLETE = 4;  // from enum of tagREADYSTATE...

				// instantiate internetexplorer 
				this.oIE = WScript.CreateObject('InternetExplorer.Application', 'oIE_');
				this.oIE.Left = 40;
				this.oIE.Top = 80;
				this.oIE.Width = 400;
				this.oIE.Height = 205;
				this.oIE.MenuBar = false;
				this.oIE.ToolBar = false;
				this.oIE.StatusBar = false;
				this.oIE.Navigate2(strFolder + 'ProgressDialog.htm');

				try
				{
					// wait for html page to load...
					while (this.oIE.ReadyState != this.READYSTATE_COMPLETE)
						WScript.Sleep(100);

					// when ready, get the document object...
					while (!this.oIE.document || !this.oIE.document.body)
						;

					this.oDoc = this.oIE.document;
					this.oIE.Visible = true;
				}
				catch (error)
				{
					alert('Progress dialog error: ' + error.description);
				}

				this.SetHeading = function(strHeading)
				{
					try
					{
						this.oDoc.all.item('Heading').innerHTML = strHeading;
						return true;
					}
					catch (error)
					{
					//j	alert(error.description);
						return false;
					}
				}
				this.SetMessage = function(strMessage)
				{
					try
					{
						this.oDoc.all.item('Message').innerHTML = strMessage;
						return true;
					}
					catch (error)
					{
					//j	alert(error.description);
						return false;
					}
				}

				this.SetProgress = function(min, max)
				{
					try
					{
						if (min > max) min = max;
						var iPct = parseInt(((100 * min) + max/2) / max);
						if (!this.ProgressBarWidth)
							this.ProgressBarWidth = 250;
						this.oDoc.all.item('ProgressBar').style.width = (min * this.ProgressBarWidth) / max;  // set the progress bar width
						this.oDoc.all.item('PercentComplete').innerHTML = iPct + '% Complete';
						return true;
					}
					catch (error)
					{
					//j	alert(error.description);
						return false;
					}
				}
				this.Close = function()
				{
					try
					{
						this.oIE.Visible = false;  // close internet explorer...
						this.oDoc = null;
						this.oIE = null;
						return true;
					}
					catch (error)
					{
						alert(error.description);
						return false;
					}
				}
			}

			ProcessCSV(null);

			WScript.Quit();
