using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Xml;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

namespace ClassLibrary
{
	internal class HtmlParser
	{
		public event RoutedEventHandler HyperlinkClick;

		private struct DocumentState
		{
			internal string Face;
			internal double Size;
			internal Brush Brush;
			internal bool Bold;
			internal bool Italic;
			internal bool Underline;
			internal char BulletType;
			internal int ListItemNumber;
			internal int Indent;
			internal string Href;
		};

		private static readonly Brush _HyperlinkMouseOverForeground = null;//Colors.Red.ToBrush();
		private static readonly TextDecorationCollection _HyperlinkMouseOverTextDecorations = TextDecorations.Underline;

		private Stack<DocumentState> _DocumentStates;
		private bool _IgnoreText;
		private string _InternalStyles;
		private CssSelectors _GlobalSelectors;
		private int _PriorLineBreaks;
		private FrameworkElement _RootElement;
		private RichTextBox _CurrentRichTextBox;
		private Paragraph _CurrentParagraph;

		public Uri DocumentUri { get { return _DocumentUri; } set { _DocumentUri = value; } }
		private Uri _DocumentUri;

		internal void SetHtml(FrameworkElement rootElement, string htmlText, Control optionalFontTemplate)
		{
			try
			{
				// This is the main entry point, initialize everything
				_RootElement = rootElement;
				if (!(_RootElement is Panel || _RootElement is RichTextBox))
					return;
				if (_RootElement is Panel)
				{
					_CurrentRichTextBox = new RichTextBlock();
					(_RootElement as Panel).Children.Clear();
					(_RootElement as Panel).Children.Add(_CurrentRichTextBox);
				}
				else
				if (_RootElement is RichTextBox)
				{
					_CurrentRichTextBox = _RootElement as RichTextBox;
				}
				_CurrentParagraph = new Paragraph();
				_CurrentRichTextBox.Blocks.Add(_CurrentParagraph);

				// Setup the initial document state
				DocumentState documentState = new DocumentState();
				if (optionalFontTemplate == null)
					optionalFontTemplate = _RootElement as Control;
				if (optionalFontTemplate != null)
				{
					documentState.Brush = optionalFontTemplate.Foreground;
					documentState.Face = optionalFontTemplate.FontFamily.Source;
					documentState.Size = optionalFontTemplate.FontSize;
					documentState.Italic = (optionalFontTemplate.FontStyle == FontStyles.Italic);
					documentState.Bold = (optionalFontTemplate.FontWeight != FontWeights.Normal);
					documentState.Underline = false;
				}
				else
				{
					Run defaultRun = new Run();
					documentState.Brush = defaultRun.Foreground;
					documentState.Face = defaultRun.FontFamily.Source;
					documentState.Size = defaultRun.FontSize;
					documentState.Italic = (defaultRun.FontStyle == FontStyles.Italic);
					documentState.Bold = (defaultRun.FontWeight != FontWeights.Normal);
					documentState.Underline = (defaultRun.TextDecorations == TextDecorations.Underline);
				}

				_DocumentStates = new Stack<DocumentState>();
				_DocumentStates.Push(documentState);

				_PriorLineBreaks = 2; // As if we are following a paragraph

				bool hasMarkup = (htmlText != null && htmlText.IndexOf("<") >= 0 && htmlText.IndexOf(">") > 0);
				if (!hasMarkup) // translate ampersands & and the like
					htmlText = HttpUtility.HtmlEncode(htmlText);
				bool hasXmlDirective = (hasMarkup && htmlText.IndexOf("<?xml") >= 0);
				if (!hasXmlDirective) // add an outer <span> to ensure valid XML regardless of the text markup
					htmlText = string.Concat("<span>", htmlText, "</span>");
				bool hasInvalidEntity = (htmlText.IndexOf("&nbsp;") >= 0);
				if (hasInvalidEntity)
					htmlText = htmlText.Replace("&nbsp;", "&#160;");
				hasInvalidEntity = (htmlText.IndexOf("&copy;") >= 0);
				if (hasInvalidEntity)
					htmlText = htmlText.Replace("&copy;", "&#169;");

				// Read the XML DOM input
				StringReader stringReader = new StringReader(htmlText);
				XmlReaderSettings settings = new XmlReaderSettings();
				settings.DtdProcessing = DtdProcessing.Ignore;
				settings.IgnoreWhitespace = false;
				settings.IgnoreProcessingInstructions = true;
				settings.IgnoreComments = true;
				//settings.CheckCharacters = false;
				//settings.ConformanceLevel = ConformanceLevel.Auto;
				XmlReader xmlReader = XmlReader.Create(stringReader, settings);
				while (xmlReader.Read())
				{
					string nameLower = xmlReader.Name.ToLower();
					if (xmlReader.NodeType == XmlNodeType.Element)
					{
						// Process the element start
						bool bEmpty = xmlReader.IsEmptyElement;
						switch (nameLower)
						{
							// Process the following elements:
							// "style"
							// "a" "img" "font"
							// "b" "strong"
							// "h1" "h2" "h3" "h4" "h5" "h6"
							// "i" "em" "cite"
							// "u" "br" "pre" "code" "tt"
							// "p" "form"
							// "ol" "ul" "blockquote" "dir" "menu"
							// "li" "div" "center" "span"
							case "head":
							case "object":
							case "meta":
							case "title":
							case "script":
							case "noscript":
							{
								_IgnoreText = true;
								break;
							}

							case "style":
							{
								_InternalStyles = string.Empty;
								break;
							}

							case "link":
							{
								string rel = null;
								string href = null;
								if (xmlReader.MoveToFirstAttribute())
								do
								{
									if (xmlReader.Name.ToLower() == "rel")
										rel = xmlReader.Value;
									else
									if (xmlReader.Name.ToLower() == "href")
										href = xmlReader.Value;
								}
								while (xmlReader.MoveToNextAttribute());

								if (rel == "stylesheet" && href != null)
								{
									if (_DocumentUri != null)
										href = _DocumentUri.Site().Append(href).ToString();
									//j	CssParser.DownloadStyles(UriHelper.MakeAbsolute(_DocumentUri.Site(), href), ref _GlobalStyles);
								}
								break;
							}

							case "a":
							case "area":
							{
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								string href = null;
								if (xmlReader.MoveToFirstAttribute())
								do
								{
									if (xmlReader.Name.ToLower() == "href")
										href = xmlReader.Value;
								}
								while (xmlReader.MoveToNextAttribute());

								if (href != null)
								{
									if (_DocumentUri != null && !href.StartsWith("#"))
										href = _DocumentUri.Site().Append(href).ToString();
									state.Href = href;
								}

								_DocumentStates.Push(state);
								break;
							}

							case "base":
							{
								string href = null;
								if (xmlReader.MoveToFirstAttribute())
								do
								{
									if (xmlReader.Name.ToLower() == "href")
										href = xmlReader.Value;
								}
								while (xmlReader.MoveToNextAttribute());

								if (href != null)
									_DocumentUri = new Uri(href, UriKind.Absolute);
								break;
							}

							case "img":
							{
								string source = null;
								DocumentState state = _DocumentStates.Peek();
								if (xmlReader.MoveToFirstAttribute())
								do
								{
									if (xmlReader.Name.ToLower() == "src")
										source = xmlReader.Value;
								}
								while (xmlReader.MoveToNextAttribute());

								if (source != null)
								{
									if (_DocumentUri != null)
										source = _DocumentUri.Site().Append(source).ToString();
									AddImage(source);
								}
								break;
							}

							case "font":
							{
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								if (xmlReader.MoveToFirstAttribute())
								do
								{
									if (xmlReader.Name.ToLower() == "face")
										state.Face = xmlReader.Value;
									else
									if (xmlReader.Name.ToLower() == "size")
										state.Size = CssParser.ParseFontSize(xmlReader.Value, state.Size);
									else
									if (xmlReader.Name.ToLower() == "color")
										state.Brush = xmlReader.Value.ToColor().ToBrush();
								}
								while (xmlReader.MoveToNextAttribute());
								_DocumentStates.Push(state);
								break;
							}

							case "big":
							case "small":
							{
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								double percent = (nameLower == "big" ? 1.2 : .8);
								state.Size *= percent;
								_DocumentStates.Push(state);
								break;
							}
							
							case "b":
							case "strong":
							{
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								state.Bold = true;
								_DocumentStates.Push(state);
								break;
							}

							case "h1":
							case "h2":
							case "h3":
							case "h4":
							case "h5":
							case "h6":
							{
								MoveToBeginLine(true);
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								// Special h? font size handling
								if (!ProcessCommonStyles(ref state, xmlReader, nameLower))
									state.Size = CssParser.ParseFontSize(nameLower, state.Size);
								state.Bold = true;

								_DocumentStates.Push(state);
								break;
							}

							case "i":
							case "em":
							case "cite":
							case "address":
							case "dfn": // Definition term
							case "var": // Variable
							{
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								state.Italic = true;
								_DocumentStates.Push(state);
								break;
							}

							case "u":
							case "ins":
							{
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								state.Underline = true;
								_DocumentStates.Push(state);
								break;
							}

							case "s":
							case "strike":
							case "del":
							{
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								//state.StrikeThrough = true;
								_DocumentStates.Push(state);
								break;
							}

							case "br":
							{
								AddLineBreak();
								break;
							}

							case "pre":
							case "code":
							case "samp": // Sample computer code
							case "kbd":
							case "tt":
							{
								if (nameLower == "pre")
									MoveToBeginLine(true);
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								state.Face = "Courier New";
								_DocumentStates.Push(state);
								break;
							}

							case "ol":
							case "ul":
							case "dir": // Same as "ul"
							case "menu": // Same as "ul"
							{
								DocumentState state = _DocumentStates.Peek();
								bool newParagraph = (bEmpty || state.BulletType == '\0');
								MoveToBeginLine(newParagraph);
								if (bEmpty)
									break;

								ProcessCommonStyles(ref state, xmlReader, nameLower);
								state.BulletType = (nameLower == "ol" ? 'o' : 'u');
								state.ListItemNumber = 0;
								state.Indent += 8;
								_DocumentStates.Push(state);
								break;
							}

							case "li":
							{
								MoveToBeginLine(false);

								// Bump the list item number
								DocumentState state = _DocumentStates.Pop();
								state.ListItemNumber++;
								_DocumentStates.Push(state);

								//DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								_DocumentStates.Push(state);

								AddIndent();
								AddListItem();
								break;
							}

							case "blockquote":
							{
								MoveToBeginLine(true);
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								state.Indent += 8;
								_DocumentStates.Push(state);
								break;
							}

							case "div":
							case "p":
							case "body":
							case "form":
							case "center":
							case "textarea":
							{
								MoveToBeginLine(true);
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								_DocumentStates.Push(state);
								break;
							}

							case "table":
							case "caption":
							case "tr":
							case "td":
							{
								if (nameLower != "td")
									MoveToBeginLine(false);
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								_DocumentStates.Push(state);
								break;
							}

							case "sup":
							case "sub":
							{
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								_DocumentStates.Push(state);
								break;
							}

							case "dl":
							case "dt":
							case "dd":
							{
								bool newParagraph = (nameLower == "dl");
								MoveToBeginLine(newParagraph);
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								if (nameLower == "dd")
									state.Indent += 8;
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								_DocumentStates.Push(state);
								break;
							}

							case "span":
							case "label":
							case "q":
							case "abbr":
							case "acronym":
							{
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								_DocumentStates.Push(state);
								break;
							}

							case "legend":
							{
								MoveToBeginLine(false);
								if (bEmpty)
									break;

								DocumentState state = _DocumentStates.Peek();
								ProcessCommonStyles(ref state, xmlReader, nameLower);
								_DocumentStates.Push(state);
								break;
							}
						}
					}
					else
					if (xmlReader.NodeType == XmlNodeType.EndElement)
					{
						// Process the element end
						switch (nameLower)
						{
							case "head":
							case "object":
							case "meta":
							case "title":
							case "script":
							case "noscript":
							{
								_IgnoreText = false;
								break;
							}

							case "style":
							{
								_GlobalSelectors = CssSelectors.Create(_InternalStyles);
								_InternalStyles = null;
								break;
							}

							case "link":
							{
								_IgnoreText = false;
								break;
							}

							case "a":
							case "area":
							{
								_DocumentStates.Pop();
								break;
							}

							case "base":
							{
								break;
							}

							case "img":
							{
								break;
							}

							case "font":
							{
								_DocumentStates.Pop();
								break;
							}

							case "big":
							case "small":
							{
								_DocumentStates.Pop();
								break;
							}

							case "b":
							case "strong":
							{
								_DocumentStates.Pop();
								break;
							}

							case "h1":
							case "h2":
							case "h3":
							case "h4":
							case "h5":
							case "h6":
							{
								MoveToBeginLine(true);
								_DocumentStates.Pop();
								break;
							}

							case "i":
							case "em":
							case "cite":
							case "address":
							case "dfn": // Definition term
							case "var": // Variable
							{
								_DocumentStates.Pop();
								break;
							}

							case "u":
							case "ins":
							{
								_DocumentStates.Pop();
								break;
							}

							case "s":
							case "strike":
							case "del":
							{
								_DocumentStates.Pop();
								break;
							}

							case "br":
							{
								break;
							}

							case "pre":
							case "code":
							case "samp": // Sample computer code
							case "kbd":
							case "tt":
							{
								if (nameLower == "pre")
									MoveToBeginLine(true);
								_DocumentStates.Pop();
								break;
							}

							case "ol":
							case "ul":
							case "dir":
							case "menu":
							{
								MoveToBeginLine(true);
								_DocumentStates.Pop();
								break;
							}

							case "li":
							{
								MoveToBeginLine(false);
								_DocumentStates.Pop();
								break;
							}

							case "blockquote":
							{
								MoveToBeginLine(true);
								_DocumentStates.Pop();
								break;
							}

							case "div":
							case "p":
							case "body":
							case "form":
							case "center":
							case "textarea":
							{
								MoveToBeginLine(false);
								_DocumentStates.Pop();
								break;
							}

							case "table":
							case "caption":
							case "tr":
							case "td":
							{
								if (nameLower != "td")
									MoveToBeginLine(false);
								_DocumentStates.Pop();
								break;
							}

							case "sup":
							case "sub":
							{
								_DocumentStates.Pop();
								break;
							}

							case "dl":
							case "dt":
							case "dd":
							{
								bool newParagraph = (nameLower == "dl");
								MoveToBeginLine(newParagraph);
								_DocumentStates.Pop();
								break;
							}

							case "span":
							case "label":
							case "q":
							case "abbr":
							case "acronym":
							{
								_DocumentStates.Pop();
								break;
							}

							case "legend":
							{
								MoveToBeginLine(false);
								_DocumentStates.Pop();
								break;
							}
						}
					}
					else
					if (xmlReader.NodeType == XmlNodeType.Text)
					{
						// Process the element text
						string text = "";
						try { text = xmlReader.Value; }
						catch (Exception ex)
						{
							text = ex.Message;
						}

						if (_InternalStyles != null)
							_InternalStyles += text;
						else
						if (!_IgnoreText)
						{
							// Remove redundant whitespace ala HTML
							StringBuilder builder = new StringBuilder(text.Length);
							char cLast = (_PriorLineBreaks > 0 ? ' ' : '\0');
							foreach (char ch in text)
							{
								char c = ch;
								if (c == '\t' || c == '\n') c = ' ';
								bool bSkip = (cLast == ' ' && c == ' ');
								cLast = c;
								if (!bSkip)
									builder.Append(c);
							}

							// Output the text
							string textRun = builder.ToString();
							AddText(textRun);
						}
					}
					else
					if (xmlReader.NodeType == XmlNodeType.Whitespace)
					{
						// Process the element whitespace
						if (_InternalStyles != null)
							_InternalStyles += " ";
						else
						if (!_IgnoreText)
						{
							// Remove redundant whitespace ala HTML
							bool bSkip = (_PriorLineBreaks > 0);
							if (!bSkip)
								AddText(" ");
						}
					}
				}

				FlushAll();
			}
			catch (Exception ex)
			{
				//ex.DebugOutput();
				ex.Alert();

				// Invalid XHTML; Clear any existing collection of Inlines
				if (_RootElement is RichTextBox)
				{
					(_RootElement as RichTextBox).Blocks.Clear();
					Paragraph paragraph = new Paragraph();
					paragraph.Inlines.Add(new Run() { Text = htmlText });
					(_RootElement as RichTextBox).Blocks.Add(paragraph);
				}
			}
			finally
			{
				_DocumentStates.Clear();
				_DocumentStates = null;

				if (_GlobalSelectors != null)
				{
					_GlobalSelectors.Dispose();
					_GlobalSelectors = null;
				}

				_RootElement = null;
				_CurrentRichTextBox = null;
				_CurrentParagraph = null;
			}
		}

		private bool ProcessCommonStyles(ref DocumentState state, XmlReader xmlReader, string elementName)
		{
			bool hasStyles = ProcessSelectorStyles(ref state, elementName);
			if (xmlReader.MoveToFirstAttribute())
			do
			{
				if (xmlReader.Name.ToLower() == "class")
				{
					hasStyles |= ProcessSelectorStyles(ref state, elementName + "." + xmlReader.Value);
					hasStyles |= ProcessSelectorStyles(ref state, "." + xmlReader.Value);
				}
				else
				if (xmlReader.Name.ToLower() == "id")
				{
					hasStyles |= ProcessSelectorStyles(ref state, elementName + "#" + xmlReader.Value);
					hasStyles |= ProcessSelectorStyles(ref state, "#" + xmlReader.Value);
				}
				else
				if (xmlReader.Name.ToLower() == "style")
					hasStyles |= ProcessInlineStyles(ref state, xmlReader.Value);
			}
			while (xmlReader.MoveToNextAttribute());

			return hasStyles;
		}

		private bool ProcessSelectorStyles(ref DocumentState state, string selector)
		{
			if (_GlobalSelectors == null)
				return false;

			CssStyles styles = _GlobalSelectors.GetStyles(selector);
			bool hasStyles = ProcessStyles(ref state, styles);
			styles.Dispose();
			return hasStyles;
		}

		private bool ProcessInlineStyles(ref DocumentState state, string style)
		{
			CssStyles styles = CssStyles.Create(style);
			bool hasStyles = ProcessStyles(ref state, styles);
			styles.Dispose();
			return hasStyles;
		}

		private bool ProcessStyles(ref DocumentState state, CssStyles styles)
		{
			if (styles == null || styles.Count <= 0)
				return false;

			string value;
			value = styles.Get("font-family");
			if (value != string.Empty)
				state.Face = value;
			value = styles.Get("font-weight");
			if (value != string.Empty)
				state.Bold = !value.EqualsIgnoreCase("normal");
			value = styles.Get("font-style");
			if (value != string.Empty)
				state.Italic = value.EqualsIgnoreCase("italic");
			value = styles.Get("text-decoration");
			if (value != string.Empty)
				state.Underline = value.EqualsIgnoreCase("underline");
			value = styles.Get("font-size");
			if (value != string.Empty)
				state.Size = CssParser.ParseFontSize(value, state.Size);
			value = styles.Get("color");
			if (value != string.Empty)
				state.Brush = value.ToColor().ToBrush();
			return true;
		}

		private void MoveToBeginLine(bool newParagraph)
		{
			if (_PriorLineBreaks == 0)
				AddLineBreak();

			if (newParagraph && _PriorLineBreaks < 2)
				AddLineBreak();
		}

		private void AddLineBreak()
		{
			_PriorLineBreaks++;

			// Create the LineBreak and buffer it
			LineBreak lineBreak = new LineBreak();
			DocumentState state = _DocumentStates.Peek();
			DecorateRun(lineBreak, state);
			_CurrentParagraph.Inlines.Add(lineBreak);
		}

		private void AddImage(string source)
		{
			BitmapImage bitmap = null;
			Uri navigateUri = null;
			bool absoluteUri = Uri.TryCreate(source, UriKind.Absolute, out navigateUri);
			if (absoluteUri)
				bitmap = new BitmapImage(navigateUri);
			else
				bitmap = ResourceFile.GetBitmap(source, AssemblySource.App);

			if (bitmap == null)
				return;

			_PriorLineBreaks = 0;

			// Create the Image and output it
			Image image = new Image();
			image.Stretch = Stretch.None;
			image.VerticalAlignment = VerticalAlignment.Bottom;
			if (bitmap.PixelWidth != 0) image.Width = bitmap.PixelWidth;
			if (bitmap.PixelHeight != 0) image.Height = bitmap.PixelHeight;
			image.Source = bitmap;

			InlineUIContainer uiContainer = new InlineUIContainer() { Child = image };
			_CurrentParagraph.Inlines.Add(uiContainer);
		}

		private void AddIndent()
		{
			DocumentState state = _DocumentStates.Peek();
			if (state.Indent <= 0)
				return;

			state = _DocumentStates.Pop();
			int indent = state.Indent;
			state.Indent = 0;
			_DocumentStates.Push(state);

			if (state.BulletType == 'o') indent -= 8;
			if (state.BulletType == 'u') indent -= 4;
			if (indent <= 0)
				return;

			Run indentRun = new Run();
			DecorateRun(indentRun, state);
			indentRun.FontFamily = new FontFamily("Arial");
			indentRun.Text = new string(' ', indent);
			AddTextRun(indentRun);
		}

		private void AddListItem()
		{
			_PriorLineBreaks = 0;

			// Add a bullet
			DocumentState state = _DocumentStates.Peek();
			if (state.BulletType == '\0')
				return;

			string bulletText = null;
			if (state.BulletType == 'o')
			{
				Run numRun = new Run();
				DecorateRun(numRun, state);
				numRun.FontFamily = new FontFamily("Courier New");
				numRun.FontWeight = FontWeights.Bold;
				numRun.Text = state.ListItemNumber.ToString().PadLeft(2);
				AddTextRun(numRun);
				bulletText = ".  ";
			}
			else
			if (state.BulletType == 'u')
				bulletText = "•  ";

			if (bulletText == null)
				return;

			Run bulletRun = new Run();
			DecorateRun(bulletRun, state);
			bulletRun.Text = bulletText;
			AddTextRun(bulletRun);
		}

		private void AddText(string text)
		{
			_PriorLineBreaks = 0;

			if (text == null || text.Length <= 0)
				return;

			// Create the Run
			Run run = new Run();
			run.Text = text;
			DocumentState state = _DocumentStates.Peek();
			DecorateRun(run, state);

			if (state.Href != null)
				AddHyperLink(run, state.Href);
			else
				AddTextRun(run);
		}

		private void AddTextRun(Run run)
		{
			_CurrentParagraph.Inlines.Add(run);
		}

		private void AddHyperLink(Run run, string link)
		{
			// Create the Hyperlink and output it
			Hyperlink hyperlink = new Hyperlink();
			hyperlink.TargetName = "_blank";
			hyperlink.FontFamily = run.FontFamily;
			hyperlink.FontSize = run.FontSize;
			hyperlink.FontStyle = run.FontStyle;
			hyperlink.FontWeight = run.FontWeight;
			hyperlink.Foreground = run.Foreground;
			hyperlink.TextDecorations = run.TextDecorations;
			hyperlink.MouseOverForeground = (_HyperlinkMouseOverForeground != null ? _HyperlinkMouseOverForeground : run.Foreground);
			hyperlink.MouseOverTextDecorations = _HyperlinkMouseOverTextDecorations;
			hyperlink.Inlines.Add(run.Text);

			// Absolute links will be launched automatically by the Hyperlink control
			// Relative Uri's will be passed to the HyperlinkClick event in the CommandParameter property
			Uri navigateUri = null;
			bool absoluteUri = Uri.TryCreate(link, UriKind.Absolute, out navigateUri);
			if (absoluteUri)
				hyperlink.NavigateUri = navigateUri;
			else
			{
				hyperlink.CommandParameter = link;
				if (HyperlinkClick != null)
					hyperlink.Click += HyperlinkClick;
			}

			_CurrentParagraph.Inlines.Add(hyperlink);
		}

		private void DecorateRun(Inline inline, DocumentState state)
		{
			// Style the Run appropriately
			inline.FontFamily = new FontFamily(state.Face != null ? state.Face : "Arial");
			inline.FontSize = state.Size;
			inline.FontStyle = (state.Italic ? FontStyles.Italic : FontStyles.Normal);
			inline.FontWeight = (state.Bold ? FontWeights.Bold : FontWeights.Normal);
			inline.Foreground = state.Brush;
			inline.TextDecorations = (state.Underline ? TextDecorations.Underline : null);
		}

		private void FlushAll()
		{
		}
	}
}
