﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Web.Services;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Xml;
using System.Xml.Linq;

namespace TrumpITReporting.Web
{
	public partial class Index : Page
	{
		[WebMethod] // a PageMethod called from Silverlight's PageMethodInvoker
		public static void ModelShutdown(object args)
		{
			System.Diagnostics.Debug.WriteLine("*** Signing Off: " + args);
		}

		[WebMethod] // a PageMethod called from Silverlight's PageMethodInvoker
		public static string ILincGetSessionFromUserID(string userId)
		{
			return ILinc.ILincGetSessionFromUserID(userId);
		}

		[WebMethod] // a PageMethod called from Silverlight's PageMethodInvoker
		public static string ILincGetSessionFromActivityID(string activityID)
		{
			return ILinc.ILincGetSessionFromActivityID(activityID);
		}

		// Don't forget to edit the Sitemap.xml as well
		private CommandItem[] m_MainCommands = {
			new CommandItem() { Name="Home", Command="/Home/Welcome", Content="Content/Welcome.html" },
			new CommandItem() { Name="Community", Command="/Community/Around_the_Globe", Content=null },
		};

		private const string m_Home = "/Home";
		private CommandItem[] m_HomeCommands = {
			new CommandItem() { Name="Welcome", Command="/Home/Welcome", Content="Content/Welcome.html" },
		};

		private const string m_Community = "/Community";
		private CommandItem[] m_CommunityCommands = {
			new CommandItem() { Name="Community Around the Globe", Command="/Community/Around_the_Globe", Content=null },
		};

		protected void Page_Load(object sender, EventArgs e)
		{
			if (base.IsPostBack)
				return;

			string query = this.Request.Url.Query;
			IEnumerable commandArray = GetCommandArray(query);
			x_Links.DataSource = commandArray;
			x_Links.DataBind();

			CommandItem command = GetCommand(query, commandArray);
			MergeContent(command);
			SetTitle(command);
		}

		private CommandItem GetCommand(string query, IEnumerable commandArray)
		{
			if (query.IsEmpty())
				query = m_MainCommands[0].Command;

			CommandItem commandFound = null;
			foreach (CommandItem command in commandArray)
			{
				if (query.ContainsIgnoreCase(command.Command))
					commandFound = command;
			}

			return commandFound;
		}

		private void MergeContent(CommandItem command)
		{
			if (command == null || command.Content == null)
				return;

			MergeContentFile(command.Content);
		}

		private void MergeContentFile(string filename)
		{
			if (filename == null)
				return;
			string contentFileName = Server.MapPath(filename);
			if (contentFileName == null || !File.Exists(contentFileName))
				return;

			StringBuilder styleText = new StringBuilder();
			StringBuilder contentText = new StringBuilder();
			XmlReaderSettings settings = new XmlReaderSettings();
			settings.IgnoreWhitespace = false;
			settings.IgnoreProcessingInstructions = true;
			settings.IgnoreComments = true;
			try
			{
				using (XmlReader xmlReader = XmlReader.Create(contentFileName, settings))
				{
					// Be sure the HTML/XML file uses &#160; instead of &nbsp;
					XDocument xml = XDocument.Load(xmlReader);
					string xmlns = "http://www.w3.org/1999/xhtml";

					IEnumerable<XElement> styleElements = xml.Descendants(XName.Get("style", xmlns));
					foreach (XElement element in styleElements)
						styleText.Append(element.ToString());

					IEnumerable<XElement> bodyElements = xml.Descendants(XName.Get("body", xmlns));
					XElement body = (bodyElements.Count() > 0 ? bodyElements.First() : xml.Root);
					IEnumerable<XElement> contentElements = body.Elements();
					foreach (XElement element in contentElements)
						contentText.Append(element.ToString());
				}
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}

			x_HeadContent.Text += styleText.ToString();
			x_BodyContent.Text = contentText.ToString();
		}

		private void SetTitle(CommandItem command)
		{
			if (command == null)
				return;

			HtmlMeta metaKeywords = null;
			HtmlMeta metaDescription = null;
			foreach (Control control in base.Header.Controls)
			{
				if (!(control is HtmlMeta))
					continue;
				HtmlMeta meta = control as HtmlMeta;
				string metaName = meta.Name.ToLower();
				if (string.Compare(metaName, "keywords") == 0)
					metaKeywords = meta;
				else
				if (string.Compare(metaName, "description") == 0)
					metaDescription = meta;
			}

			if (metaKeywords == null)
			{
				metaKeywords = new HtmlMeta();
				metaKeywords.Name = "keywords";
				base.Header.Controls.Add(metaKeywords);
			}

			if (metaDescription == null)
			{
				metaDescription = new HtmlMeta();
				metaDescription.Name = "description";
				base.Header.Controls.Add(metaDescription);
			}

			metaDescription.Content = command.Name + ". " + metaDescription.Content;
			metaKeywords.Content = command.Name + ", " + metaKeywords.Content;
			base.Title += " " + command.Name;
		}

		private IEnumerable GetCommandArray(string query)
		{
			foreach (CommandItem command in m_MainCommands)
				yield return command;

			if (query.IsEmpty())
				query = m_MainCommands[0].Command;

			CommandItem[] items = null;
			if (query.ContainsIgnoreCase(m_Home))			items = m_HomeCommands; else
			if (query.ContainsIgnoreCase(m_Community))		items = m_CommunityCommands;

			if (items != null)
				foreach (CommandItem command in items)
					yield return command;
		}
	}

	public class CommandItem
	{
		public string Name { get; set; }
		public string Command { get; set; }
		public string Content { get; set; }
	}

	internal static class StringExt
	{
		// Extension for string
		public static bool ContainsIgnoreCase(this string source, string target)
		{
			if (source.IsEmpty())
				return false;
			return (source.ToLower().Contains(target.ToLower()));
		}

		// Extension for string
		public static bool IsEmpty(this string source)
		{
			return string.IsNullOrEmpty(source);
		}
	}
}
