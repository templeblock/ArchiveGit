using System;
using System.Diagnostics;
using System.Net;
using System.Windows;
using System.Windows.Controls;

// The HtmlTextBlock class can be used in one of two ways.
//
// 1.)	In code, you can set HTML text into any RichTextBox or Panel contol by calling one
//		of the two extension methods; either richTextBox.SetHtml() or panel.SetHtml()
//		Example:
//			RichTextBox richTextBox = new RichTextBox();
//			richTextBox.SetHtml("<span style="color:'Red'>Hello World!</span>");
//		Or:
//			StackPanel panel = new StackPanel();
//			panel.SetHtml("<span style="color:'Red'>Hello World!</span>");
//
//		The only advantage to using a Panel as the root control is that you can include active hypelinks in the HTML
//
// 2.) From XAML, create an <c:HtmlTextBlock> control, and set either the Html property, or the HtmlUri property
//		If using the inline Html property, you must translate any HTML angle brackets <> into square brackets [], in order to pass as XAML syntax
//		If using the HtmlUri property, you do not need to translate angle brackets, since it points to a resource that contains the HTML

namespace ClassLibrary
{
	internal static class RichTextBoxExtension
	{
		// SetHtml() extension method for any RichTextBox
		internal static void SetHtml(this RichTextBox richTextBox, string htmlText)
		{
			HtmlParser htmlParser = new HtmlParser();
			htmlParser.SetHtml(richTextBox, htmlText, null);
		}
	}

	internal static class PanelExtension
	{
		// SetHtml() extension method for any Panel
		internal static void SetHtml(this Panel panel, string htmlText)
		{
			HtmlParser htmlParser = new HtmlParser();
			htmlParser.SetHtml(panel, htmlText, null);
		}
	}

	public class HtmlTextBlock : ContentControl
	{
		public event RoutedEventHandler HyperlinkClick;

		public string HtmlUri { get { return m_HtmlUri; } set { SetHtmlUri(value); } }
		private string m_HtmlUri;

		public string Html { get { return m_HtmlText; } set { SetHtml(value, true/*replaceBrackets*/); } }
		private string m_HtmlText;

		private Uri m_DocumentUri;

		public HtmlTextBlock()
		{
			base.DefaultStyleKey = base.GetType();
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			base.Content = new StackPanel();
			AttachHtml();
		}

		private void SetHtmlUri(string htmlUri)
		{
			m_HtmlUri = htmlUri;

			if (Helper.IsInDesignTool)
				return;

			Uri navigateUri = null;
			bool absoluteUri = Uri.TryCreate(m_HtmlUri, UriKind.Absolute, out navigateUri);
			if (absoluteUri)
				DownloadHtml(navigateUri);
			else
			{
				string htmlText = ResourceFile.GetFileText(m_HtmlUri, AssemblySource.App);
				if (htmlText == null)
					DownloadHtml(UriHelper.UriAppRelative(m_HtmlUri));
				else
					SetHtml(htmlText, false/*replaceBrackets*/);
			}
		}

		private void DownloadHtml(Uri uri)
		{
			if (uri == null)
				return;

			m_DocumentUri = uri;
			WebClient webClient = new WebClient();
			webClient.DownloadStringCompleted += DownloadHtmlComplete;
			webClient.DownloadStringAsync(uri);
		}

		private void DownloadHtmlComplete(object sender, DownloadStringCompletedEventArgs e)
		{
			if (e.Error != null)
			{
				m_DocumentUri = null;
				MessageBoxEx.ShowError("Error downloading file", "Cannot download file '" + m_HtmlUri + "'\n\n" + e.Error.Details(), null);
				return;
			}

			SetHtml(e.Result, false/*replaceBrackets*/);
		}

		private void SetHtml(string htmlText, bool replaceBrackets)
		{
			m_HtmlText = htmlText;
			if (m_HtmlText == null)
				m_HtmlText = string.Empty;
			if (replaceBrackets)
				m_HtmlText = m_HtmlText.Replace("]", ">").Replace("[", "<");
			if (base.Content != null)
				AttachHtml();
		}

		private void AttachHtml()
		{
			if (m_HtmlText == null)
				return;

			HtmlParser htmlParser = new HtmlParser();
			htmlParser.DocumentUri = m_DocumentUri;
			if (HyperlinkClick != null)
				htmlParser.HyperlinkClick += HyperlinkClick;
			htmlParser.SetHtml(base.Content as StackPanel, m_HtmlText, this);
		}

		protected override void OnContentChanged(object oldContent, object newContent)
		{
			Debug.Assert(base.Content == null || base.Content.GetType() == typeof(StackPanel),
			"HtmlTextBlock controls do not use the Content property.  Use the HtmlUri or Html properties instead.");
		}
	}
}
