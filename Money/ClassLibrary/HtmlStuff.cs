using System;
using System.IO.IsolatedStorage;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Browser;

namespace ClassLibrary
{
#if NOTUSED
	internal class ContextMenuInterceptor
	{
		public ContextMenuInterceptor()
		{
			HtmlPage.Document.AttachEvent("oncontextmenu", OnContextMenu);
		}

		private void OnContextMenu(object sender, HtmlEventArgs e)
		{
			// Cancels the event if it is cancelable
			e.PreventDefault();
			Helper.Alert("Right Clicked Blocked at " + e.OffsetX + "," + e.OffsetY);
		}
	}
#endif

	internal class HtmlStuff
	{
		private bool _editMode;

		/// <summary>
		/// Root HTML container
		/// </summary>
		private HtmlElement _linkContainer;

		/// <summary>
		/// Default color key
		/// </summary>
		private const string DefaultColor = "red";

		/// <summary>
		/// Set of stylesheet colors
		/// </summary>
		private readonly string[] Colors = { "red", "blue", "gray" };

		/// <summary>
		/// ctor
		/// </summary>
		public HtmlStuff()
		{
			HtmlPageSetup();
			UpdateDisplay();
		}

		/// <summary>
		/// Redraw the UI
		/// </summary>
		private static void UpdateDisplay()
		{
		}

		/// <summary>
		/// Initial setup, loads the stylesheet and makes the
		/// Silverlight plugin 0x0 in size
		/// </summary>
		public void HtmlPageSetup()
		{
			// title
			HtmlPage.Document.SetProperty("title", "my start page");

			// coloring
			const string color = DefaultColor;
			HtmlPage.Document.Body.CssClass = color;

			// stylesheet
			HtmlElement css = HtmlPage.Document.CreateElement("link");
			css.SetAttribute("rel", "Stylesheet");
			css.SetAttribute("type", "text/css");
			css.SetAttribute("href", "http://media.jeff.wilcox.name/apps/home/LinkPage.css");
			((HtmlElement)HtmlPage.Document.GetElementsByTagName("head")[0]).AppendChild(css);

			// hide the Silverlight control
			HtmlPage.Plugin.SetStyleAttribute("width", "0");
			HtmlPage.Plugin.SetStyleAttribute("height", "0");
			HtmlPage.Plugin.Parent.SetStyleAttribute("width", "0");
			HtmlPage.Plugin.Parent.SetStyleAttribute("height", "0");

			// Container
			_linkContainer = HtmlPage.Document.CreateElement("div");
			_linkContainer.Id = "cloud";
			HtmlPage.Document.Body.AppendChild(_linkContainer);

			CreateHtmlForm();
		}

		/// <summary>
		/// The import/export link was clicked
		/// </summary>
		private void ImportClick(object sender, HtmlEventArgs e)
		{
			ImportClick();
		}

		/// <summary>
		/// The import/export link was clicked
		/// </summary>
		public void ImportClick()
		{
			HtmlElement pop = CreatePopup(450, 400);
			pop.SetStyleAttribute("textAlign", "center");
			pop.SetStyleAttribute("padding", "8px");
			pop.SetProperty("innerHTML", "<strong>Import/Export</strong><br /><br />Copy and store the link data to transfer to another machine or backup the values.  You can also import and change the existing underlying data.<br /><br />Data format is:<br />URL<br />Title<br />Clicks<br />Sort order<br /><br />");

			HtmlElement txt = HtmlPage.Document.CreateElement("textarea");
			txt.SetAttribute("rows", "8");
			txt.SetAttribute("wrap", "off");
			txt.SetStyleAttribute("width", "85%");

			string s = String.Empty;
			s = s.Replace(Environment.NewLine, "\n"); // temp
			s = s.Replace("\n", Environment.NewLine);
			txt.SetProperty("value", s);
			pop.AppendChild(txt);

			HtmlElement br = HtmlPage.Document.CreateElement("br");
			pop.AppendChild(br);

			HtmlElement save = HtmlPage.Document.CreateElement("input");
			save.SetAttribute("type", "button");
			save.SetAttribute("value", "Save");
			save.AttachEvent("onclick", delegate(object sender3, HtmlEventArgs e3)
			{
				// update
				string newValue = txt.GetProperty("value").ToString();
				newValue = newValue.Replace(Environment.NewLine, "\n");

				HtmlElement pr = pop.Parent;
				pr.RemoveChild(pop);

				UpdateDisplay();
			});
			pop.AppendChild(save);

			HtmlElement closeButton = HtmlPage.Document.CreateElement("input");
			closeButton.SetAttribute("type", "button");
			closeButton.SetAttribute("value", "Close");
			closeButton.AttachEvent("onclick", delegate(object sender2, HtmlEventArgs e2)
			{
				HtmlElement pr = pop.Parent;
				pr.RemoveChild(pop);
			});
			pop.AppendChild(closeButton);
		}

		/// <summary>
		/// Create a div for a 'popup'
		/// </summary>
		public HtmlElement CreatePopup(int width, int height)
		{
			HtmlElement popup = HtmlPage.Document.CreateElement("div");
			popup.SetStyleAttribute("position", "fixed");
			popup.SetStyleAttribute("width", width.ToString() + "px");
			popup.SetStyleAttribute("height", height.ToString() + "px");
			popup.SetStyleAttribute("top", "100px");
			popup.SetStyleAttribute("left", "100px");
			popup.SetStyleAttribute("right", "auto");
			popup.SetStyleAttribute("border", "4px solid #999");
			popup.SetStyleAttribute("backgroundColor", "#eee");

			HtmlPage.Document.Body.AppendChild(popup);
			return popup;
		}

		/// <summary>
		/// Creates the add form for the bottom of the page
		/// </summary>
		public void CreateHtmlForm()
		{
			HtmlElement e1 = HtmlPage.Document.CreateElement("div");
			e1.SetStyleAttribute("position", "fixed");
			e1.SetStyleAttribute("bottom", "0px");
			e1.SetStyleAttribute("top", "auto");
			e1.SetStyleAttribute("left", "0px");
			e1.SetStyleAttribute("right", "0px");
			e1.SetStyleAttribute("height", "auto");
			e1.SetStyleAttribute("padding", "4px");
			e1.SetStyleAttribute("backgroundColor", "#eee");
			e1.SetStyleAttribute("borderTop", "1px solid #999");
			HtmlPage.Document.Body.AppendChild(e1);

			// url label
			HtmlElement lblUrl = HtmlPage.Document.CreateElement("span");
			lblUrl.SetProperty("innerHTML", "URL:");
			lblUrl.SetStyleAttribute("paddingRight", "6px");
			e1.AppendChild(lblUrl);

			// url text
			HtmlElement _txtUrl;
			_txtUrl = HtmlPage.Document.CreateElement("input");
			_txtUrl.SetAttribute("type", "text");
			_txtUrl.SetAttribute("size", "32");
			_txtUrl.SetStyleAttribute("paddingRight", "6px");
			e1.AppendChild(_txtUrl);

			// title label
			HtmlElement lblTitle = HtmlPage.Document.CreateElement("span");
			lblTitle.SetProperty("innerHTML", "Title:");
			lblTitle.SetStyleAttribute("paddingRight", "6px");
			lblTitle.SetStyleAttribute("paddingLeft", "6px");
			e1.AppendChild(lblTitle);

			// title text
			HtmlElement _txtTitle;
			_txtTitle = HtmlPage.Document.CreateElement("input");
			_txtTitle.SetAttribute("type", "text");
			_txtTitle.SetAttribute("size", "24");
			_txtTitle.SetStyleAttribute("paddingRight", "6px");
			e1.AppendChild(_txtTitle);

			// button
			HtmlElement _btnAdd;
			_btnAdd = HtmlPage.Document.CreateElement("input");
			_btnAdd.SetAttribute("type", "button");
			_btnAdd.SetStyleAttribute("paddingRight", "6px");
			_btnAdd.SetAttribute("value", "Add");
			_btnAdd.AttachEvent("onclick", delegate(object sender, HtmlEventArgs e)
			{
				//HtmlElement btn = sender as HtmlElement;
				//string title = _txtTitle.GetProperty("value").ToString();
				//string url = _txtUrl.GetProperty("value").ToString();
				//if (!url.Contains(":"))
				//	url = "http://" + url;

				_txtTitle.SetProperty("value", "");
				_txtUrl.SetProperty("value", "");

				// Add and store the new link
				UpdateDisplay();
			}
			);
			e1.AppendChild(_btnAdd);

			// color selection
			const string currentColor = DefaultColor;
			HtmlElement sel = HtmlPage.Document.CreateElement("select");
			sel.SetAttribute("size", "1");
			e1.AppendChild(sel);
			sel.SetStyleAttribute("marginLeft", "8px");
			foreach (string color in Colors)
			{
				HtmlElement opt = HtmlPage.Document.CreateElement("option");
				opt.SetProperty("innerHTML", color);
				if (color == currentColor)
				{
					opt.SetAttribute("selected", "selected");
				}
				sel.AppendChild(opt);
			}
			sel.AttachEvent("onchange", delegate(object sender, HtmlEventArgs e)
			{
				int i = (int)((double)sel.GetProperty("selectedIndex"));
				HtmlPage.Document.Body.CssClass = Colors[i];
			});

			//HtmlElement o1 = HtmlPage.Document.CreateElement("option");

			// menu
			HtmlElement menu = HtmlPage.Document.CreateElement("span");
			menu.SetStyleAttribute("position", "absolute");
			menu.SetStyleAttribute("right", "6px");

			// edit or view mode
			HtmlElement _toggleEditLink;
			_toggleEditLink = HtmlPage.Document.CreateElement("a");
			_toggleEditLink.SetProperty("innerHTML", "Edit Links");
			_toggleEditLink.SetAttribute("href", "#");
			_toggleEditLink.AttachEvent("onclick", ToggleEditMode);
			_toggleEditLink.SetStyleAttribute("paddingRight", "6px");
			menu.AppendChild(_toggleEditLink);

			// import
			HtmlElement import = HtmlPage.Document.CreateElement("a");
			import.SetProperty("innerHTML", "Import/Export");
			import.SetAttribute("href", "#");
			import.AttachEvent("onclick", ImportClick);
			import.SetStyleAttribute("paddingRight", "6px");
			menu.AppendChild(import);

			// clear all
			HtmlElement clearAll = HtmlPage.Document.CreateElement("a");
			clearAll.SetProperty("innerHTML", "Clear All");
			clearAll.SetAttribute("href", "#");
			clearAll.AttachEvent("onclick", delegate(object sender, HtmlEventArgs e)
			{
			}
			);
			menu.AppendChild(clearAll);

			e1.AppendChild(menu);

		}

		/// <summary>
		/// Edit/View toggle link
		/// </summary>
		private void ToggleEditMode(object sender, HtmlEventArgs e)
		{
			HtmlElement elem = sender as HtmlElement;
			if (elem != null)
			{
				_editMode = !_editMode;
				elem.SetProperty("innerHTML", _editMode ? "View" : "Edit");
				//DisplayLinks();
				UpdateDisplay();
				e.StopPropagation();
			}
		}
	}
}
