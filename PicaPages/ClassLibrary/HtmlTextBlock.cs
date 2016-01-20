using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;
using System.Xml;

namespace ClassLibrary
{
	public static class HtmlTextBlock
	{
		private struct FontProperties
		{
			public string face;
			public double size;
			public Brush brush;
			public bool bold;
			public bool italic;
			public bool underline;
			public bool listitem;
			public int listtype;
			public int listitems;
			public int indent;
			public string link;
		};
		
		// State variables
		private static Stack<FontProperties> g_sFonts;
		private static int g_iPrevLineBreaks;

		public static void SetHtml(this TextBlock textBlock, string text)
		{
			// Constants
			try
			{
				FontProperties item = new FontProperties();
				item.face = textBlock.FontFamily.Source;
				item.size = textBlock.FontSize;
				item.brush = textBlock.Foreground;
				item.bold = false;
				item.italic = false;
				item.underline = false;
				item.listitem = false;
				item.listtype = 0;
				item.listitems = 0;
				item.indent = 0;
				item.link = null;
				g_sFonts = new Stack<FontProperties>();
				g_sFonts.Push(item);
				g_iPrevLineBreaks = 2;

				if (textBlock.Inlines == null)
					textBlock.Text = ""; // TextBlock Bug: work around the null inlines

				// Clear the collection of Inlines
				textBlock.Inlines.Clear();

				// Wrap the input in a <DIV> (so even plain text becomes valid XML)
				StringReader stringReader = new StringReader(string.Concat("<DIV>", text, "</DIV>"));

				// Read the input
				XmlReader xmlReader = XmlReader.Create(stringReader);

				// Read the entire XML DOM...
				while (xmlReader.Read())
				{
					string nameLower = xmlReader.Name.ToLower();
					if (xmlReader.NodeType == XmlNodeType.Element)
					{
						bool bEmpty = xmlReader.IsEmptyElement;

						// Handle the begin element
						switch (nameLower)
						{
							case "a":
							{
								if (bEmpty)
									break;

								FontProperties font = g_sFonts.Peek();
								if (xmlReader.MoveToFirstAttribute())
								do
								{
									if (xmlReader.Name.ToLower() == "href")
									{
										font.link = xmlReader.Value;
										font.underline = true;
										font.brush = Colors.Blue.ToBrush();
									}
								}
								while (xmlReader.MoveToNextAttribute());
								g_sFonts.Push(font);
								break;
							}

							case "font":
							{
								if (bEmpty)
									break;

								FontProperties font = g_sFonts.Peek();
								if (xmlReader.MoveToFirstAttribute())
								do
								{
									if (xmlReader.Name.ToLower() == "face")
										font.face = xmlReader.Value;
									if (xmlReader.Name.ToLower() == "size")
										font.size = Convert.ToDouble(xmlReader.Value);
									if (xmlReader.Name.ToLower() == "color")
										font.brush = xmlReader.Value.ToColor().ToBrush();
								}
								while (xmlReader.MoveToNextAttribute());
								g_sFonts.Push(font);
								break;
							}
							
							case "b":
							case "strong":
							{
								if (bEmpty)
									break;

								FontProperties font = g_sFonts.Peek();
								font.bold = true;
								g_sFonts.Push(font);
								break;
							}

							case "h1":
							case "h2":
							case "h3":
							case "h4":
							case "h5":
							case "h6":
							{
								textBlock.BreakAfterText();
								textBlock.StartNewParagraph();

								if (bEmpty)
									break;

								FontProperties font = g_sFonts.Peek();
								if (nameLower == "h1") font.size *= (24.0/12);	// xx-large	24pt	32pix
								if (nameLower == "h2") font.size *= (18.0/12);	// x-large	18pt	24pix
								if (nameLower == "h3") font.size *= (13.5/12);	// large	13.5pt	18pix
								if (nameLower == "h4") font.size *= (12.0/12);	// medium	12pt	16pix
								if (nameLower == "h5") font.size *= (10.5/12);	// sSmall	10.5pt	14pix
								if (nameLower == "h6") font.size *= ( 7.5/12);	// x-small	7.5pt	10pix
																				// xx-small	7.5pt	10pix
								font.bold = true;
								g_sFonts.Push(font);
								break;
							}

							case "i":
							case "em":
							case "cite":
							{
								if (bEmpty)
									break;

								FontProperties font = g_sFonts.Peek();
								font.italic = true;
								g_sFonts.Push(font);
								break;
							}

							case "u":
							{
								if (bEmpty)
									break;

								FontProperties font = g_sFonts.Peek();
								font.underline = true;
								g_sFonts.Push(font);
								break;
							}

							case "br":
							{
								textBlock.InlinesAdd(new LineBreak());
								break;
							}

							case "pre":
							{
								textBlock.BreakAfterText();
								textBlock.StartNewParagraph();

								if (bEmpty)
									break;

								FontProperties font = g_sFonts.Peek();
								font.face = "Courier New";
								g_sFonts.Push(font);
								break;
							}

							case "p":
							case "form":
							{
								textBlock.BreakAfterText();
								textBlock.StartNewParagraph();
								break;
							}

							case "ol":
							case "ul":
							case "dir":
							case "menu":
							{
								textBlock.BreakAfterText();

								FontProperties font = g_sFonts.Peek();
								if (bEmpty || font.listtype == 0)
									textBlock.StartNewParagraph();

								if (bEmpty)
									break;

								font.listtype = (nameLower == "ol" ? 1 : 2);
								font.listitems = 0;
								font.indent += 3;
								g_sFonts.Push(font);
								break;
							}

							case "li":
							{
								textBlock.BreakAfterText();
								if (bEmpty)
									break;

								FontProperties font = g_sFonts.Pop(); // Pop not Peek here
								font.listitems++;
								g_sFonts.Push(font);
								font.listitem = true;
								g_sFonts.Push(font);
								break;
							}

							case "div":
							case "center":
							{
								textBlock.BreakAfterText();
								break;
							}

							case "span":
							{
								break;
							}
						}
					}
					else
					if (xmlReader.NodeType == XmlNodeType.EndElement)
					{
						// Handle the end element
						switch (nameLower)
						{
							case "a":
							{
								g_sFonts.Pop();
								break;
							}

							case "font":
							{
								g_sFonts.Pop();
								break;
							}

							case "b":
							case "strong":
							{
								g_sFonts.Pop();
								break;
							}

							case "h1":
							case "h2":
							case "h3":
							case "h4":
							case "h5":
							case "h6":
							{
								textBlock.BreakAfterText();
								textBlock.StartNewParagraph();
								g_sFonts.Pop();
								break;
							}

							case "i":
							case "em":
							case "cite":
							{
								g_sFonts.Pop();
								break;
							}

							case "u":
							{
								g_sFonts.Pop();
								break;
							}

							case "br":
							{
								break;
							}

							case "pre":
							{
								textBlock.BreakAfterText();
								textBlock.StartNewParagraph();
								g_sFonts.Pop();
								break;
							}

							case "p":
							case "form":
							{
								textBlock.BreakAfterText();
								textBlock.StartNewParagraph();
								break;
							}

							case "ol":
							case "ul":
							case "dir":
							case "menu":
							{
								textBlock.BreakAfterText();

								FontProperties font = g_sFonts.Pop();
								if (font.listtype == 0)
									textBlock.StartNewParagraph();

								break;
							}

							case "li":
							{
								textBlock.BreakAfterText();
								g_sFonts.Pop();
								break;
							}

							case "div":
							case "center":
							{
								textBlock.BreakAfterText();
								break;
							}

							case "span":
							{
								break;
							}
						}
					}
					else
					if (xmlReader.NodeType == XmlNodeType.Text)
					{
						// Create a Run for the visible text
						// Collapse contiguous whitespace per HTML behavior
						StringBuilder builder = new StringBuilder(xmlReader.Value.Length);
						char cLast = (g_iPrevLineBreaks > 0 ? ' ' : '\0');
						foreach (char ch in xmlReader.Value)
						{
							char c = ch;
							if (c == '\t' || c == '\n') c = ' ';
							bool bSkip = (cLast == ' ' && c == ' ');
							cLast = c;
							if (!bSkip)
								builder.Append(c);
						}

						// If any text to display...
						string builderString = builder.ToString();
						if (builderString.Length > 0)
						{
							// Create a Run to display it
							Run run = new Run();
							run.Text = builderString;
							textBlock.InlinesAdd(run);
						}
					}
					else
					if (xmlReader.NodeType == XmlNodeType.Whitespace)
					{
					}
				}
			}
			catch (Exception e)
			{
				System.Diagnostics.Debug.WriteLine(e.Message);
				e.Assert();

				// Invalid XHTML
				textBlock.Inlines.Clear();
				textBlock.Text = text;
			}
		}

		private static void InlinesAdd(this TextBlock textBlock, Inline run)
		{
			// Style the Run appropriately
			FontProperties font = g_sFonts.Peek();
			run.Foreground = font.brush;
			run.FontSize = font.size;
			run.FontFamily = new FontFamily(font.face);
			run.FontStyle = (font.italic ? FontStyles.Italic : FontStyles.Normal);
			run.FontWeight = (font.bold ? FontWeights.ExtraBold : FontWeights.Normal);
			run.TextDecorations = (font.underline ? TextDecorations.Underline : null);
			if (font.link != null) // runs don't really support links
				(run as Run).Text += string.Concat(" <", font.link, ">");

			if (font.indent > 0 && run is Run)
			{
				Run indentRun = new Run();
				indentRun.Foreground = font.brush;
				indentRun.FontSize = font.size;
				indentRun.FontFamily = new FontFamily("Courier New");
				indentRun.Text = new string(' ', font.indent);
				textBlock.Inlines.Add(indentRun);
			}

			if (font.listitem && run is Run)
			{
				Run bulletRun = new Run();
				bulletRun.Foreground = font.brush;
				bulletRun.FontSize = font.size;
				bulletRun.FontFamily = new FontFamily(font.listtype == 1 ? "Courier New" : "Webdings");
				bulletRun.Text = (font.listtype == 1 ? font.listitems.ToString().PadLeft(2) + "." : "=");
				textBlock.Inlines.Add(bulletRun);
				(run as Run).Text = ' ' + (run as Run).Text;
			}

			textBlock.Inlines.Add(run);

			if (run is LineBreak)
				g_iPrevLineBreaks++;
			else
				g_iPrevLineBreaks = 0;
		}

		private static void BreakAfterText(this TextBlock textBlock)
		{
			if (g_iPrevLineBreaks == 0)
				textBlock.InlinesAdd(new LineBreak());
		}

		private static void StartNewParagraph(this TextBlock textBlock)
		{
			if (g_iPrevLineBreaks < 2)
				textBlock.InlinesAdd(new LineBreak());
		}
	}
}
