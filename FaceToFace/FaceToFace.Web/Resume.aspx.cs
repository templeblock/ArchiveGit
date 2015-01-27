//#define OLD
using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Web.UI;
#if OLD
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;
#else
using HtmlAgilityPack;
#endif

namespace FaceToFace.Web
{
	public partial class Resume : Page
	{
		protected void Page_Load(object sender, EventArgs e)
		{
			MergeContentFile("Content/JimMcCurdy.html");
		}

		private void MergeContentFile(string filename)
		{
			if (filename == null)
				return;
			string contentFileName = Server.MapPath(filename);
			if (contentFileName == null || !File.Exists(contentFileName))
				return;

			string stylesText = string.Empty;
			string contentText = string.Empty;
#if OLD
			try
			{ // Extract the styles and body via XML
				StringBuilder styles = new StringBuilder();
				StringBuilder content = new StringBuilder();
				XmlReaderSettings settings = new XmlReaderSettings
				{
					IgnoreWhitespace = false, IgnoreProcessingInstructions = true, IgnoreComments = true
				};

				using (XmlReader xmlReader = XmlReader.Create(contentFileName, settings))
				{
					// Be sure the HTML/XML file uses &#160; instead of &nbsp;
					XDocument xml = XDocument.Load(xmlReader);
					const string xmlns = "http://www.w3.org/1999/xhtml";

					IEnumerable<XElement> styleElements = xml.Descendants(XName.Get("style", xmlns));
					foreach (XElement element in styleElements)
						styles.Append(element);

					IEnumerable<XElement> bodyElements = xml.Descendants(XName.Get("body", xmlns));
					XElement body = (bodyElements.Any() ? bodyElements.First() : xml.Root);
					if (body != null)
					{
						IEnumerable<XElement> contentElements = body.Elements();
						foreach (XElement element in contentElements)
							content.Append(element);
					}
				}

				stylesText += styles.ToString();
				contentText = content.ToString();
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}
#else
			try
			{ // Extract the styles and body via HtmlAgilityPack
				StringBuilder styles = new StringBuilder();
				HtmlWeb html = new HtmlWeb();
				HtmlDocument htmlDoc = html.Load(contentFileName);
				if (htmlDoc.DocumentNode != null)
				{
					var bodyNode = htmlDoc.DocumentNode.SelectSingleNode("//body");
					if (bodyNode != null)
					{
						contentText = bodyNode.InnerHtml;
					}

					var styleNodes = htmlDoc.DocumentNode.SelectNodes("//style");
					if (styleNodes != null)
					{
						foreach (HtmlNode style in styleNodes)
							styles.Append(style.OuterHtml);
					}
				}

				stylesText += styles.ToString();
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}
#endif
			x_HeadContent.Text += stylesText;
			x_BodyContent.Text = contentText;
		}
	}
}
