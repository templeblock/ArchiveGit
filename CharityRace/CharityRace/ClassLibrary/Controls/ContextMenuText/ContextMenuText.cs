using System;
using System.Linq;
using System.Security;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Printing;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

namespace ClassLibrary
{
	public static class ContextMenuTextService
	{
		// The List attached property
		public static readonly DependencyProperty ListProperty =
			DependencyProperty.RegisterAttached("List", typeof(string), typeof(ContextMenuTextService),
				new PropertyMetadata(null, OnListPropertyChanged));

		internal static string GetList(DependencyObject obj)
		{
			return (string)obj.GetValue(ListProperty);
		}

		internal static void SetList(DependencyObject obj, string list)
		{
			obj.SetValue(ListProperty, list);
		}

		private static void OnListPropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
		{
			if (sender == null)
				return;
			string list = (string)e.NewValue;
			ContextMenuText menu = (list != null ? new ContextMenuText(list) { Owner = sender } : null);
			ContextMenuService.SetContextMenu(sender, menu);
		}
	}

	public class ContextMenuText : ContextMenuBase
	{
		public DependencyObject Owner { get; set; }
		public string ItemList { get; set; }

		public ContextMenuText(string list = "")
		{
//			base.DefaultStyleKey = base.GetType();

			ItemList = list;
			if (ItemList == null || ItemList.IsEmpty() || ItemList.ToLower() == "all")
				ItemList = "Cut, Copy, Paste, Delete, Select All, Print";

			string[] items = ItemList.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
			foreach (string itemText in items)
			{
				MenuItem item = new MenuItem() { Header = itemText.Trim(), IsEnabled = true };
				base.Items.Add(item);
				if (MatchMenuItem(item, "cut"))
				{
					item.Click += OnCutClick;
					item.Icon = new Image() { Source = ResourceFile.GetBitmap("ClassLibrary/Controls/ContextMenuText/Images/Cut.png", AssemblySource.This) };
				}
				else
				if (MatchMenuItem(item, "copy"))
				{
					item.Click += OnCopyClick;
					item.Icon = new Image() { Source = ResourceFile.GetBitmap("ClassLibrary/Controls/ContextMenuText/Images/Copy.png", AssemblySource.This) };
				}
				else
				if (MatchMenuItem(item, "paste"))
				{
					item.Click += OnPasteClick;
					item.Icon = new Image() { Source = ResourceFile.GetBitmap("ClassLibrary/Controls/ContextMenuText/Images/Paste.png", AssemblySource.This) };
				}
				else
				if (MatchMenuItem(item, "delete"))
				{
					item.Click += OnDeleteClick;
					item.Icon = new Image() { Source = ResourceFile.GetBitmap("ClassLibrary/Controls/ContextMenuText/Images/Delete.png", AssemblySource.This) };
				}
				else
				if (MatchMenuItem(item, "select"))
				{
					item.Click += OnSelectAllClick;
					item.Icon = new Image() { Source = ResourceFile.GetBitmap("ClassLibrary/Controls/ContextMenuText/Images/SelectAll.png", AssemblySource.This) };
				}
				else
				if (MatchMenuItem(item, "print"))
				{
					item.Click += OnPrintClick;
					item.Icon = new Image() { Source = ResourceFile.GetBitmap("ClassLibrary/Controls/ContextMenuText/Images/Print.png", AssemblySource.This) };
				}

			}
		}

		protected override void OnOpened(RoutedEventArgs e)
		{
			base.OnOpened(e);
			TextBox textBox = GetOwnerAs<TextBox>();
			RichTextBox richTextBox = GetOwnerAs<RichTextBox>();
			if (textBox == null && richTextBox == null)
				return;

			bool isAllSelected = (textBox != null ? textBox.Text.Length == textBox.SelectedText.Length : false);
			bool isReadOnly = (textBox != null ? textBox.IsReadOnly : false);
			string selectedText = (textBox != null ? textBox.SelectedText : string.Empty);
			if (richTextBox != null)
			{
				Rect startContent = richTextBox.ContentEnd.GetCharacterRect(LogicalDirection.Backward);
				Rect startSelection = richTextBox.Selection.End.GetCharacterRect(LogicalDirection.Backward);
				Rect endContent = richTextBox.ContentEnd.GetCharacterRect(LogicalDirection.Forward);
				Rect endSelection = richTextBox.Selection.End.GetCharacterRect(LogicalDirection.Forward);
				isAllSelected = (startContent == startSelection && endContent == endSelection);
				isReadOnly = richTextBox.IsReadOnly;
				selectedText = richTextBox.Selection.Text;
			}

			MenuItem item = FindMenuItem("cut");
			if (item != null)
				SetEnabled(item, (selectedText.Length > 0) && !isReadOnly);
			item = FindMenuItem("copy");
			if (item != null)
				SetEnabled(item, (selectedText.Length > 0));
			item = FindMenuItem("paste");
			if (item != null)
				SetEnabled(item, Clipboard.ContainsText() && !isReadOnly);
			item = FindMenuItem("delete");
			if (item != null)
				SetEnabled(item, (selectedText.Length > 0) && !isReadOnly);
			item = FindMenuItem("select");
			if (item != null)
				SetEnabled(item, !isAllSelected);
			item = FindMenuItem("print");
			if (item != null)
				SetEnabled(item, selectedText.Length > 0);
		}

		protected override void OnClosed(RoutedEventArgs e)
		{
			base.OnClosed(e);
			Control owner = GetOwnerAs<Control>();
			if (owner != null)
				owner.TakeFocus();
		}

		private void SetEnabled(MenuItem item, bool enabled)
		{
			item.IsEnabled = enabled;
			if (item.Icon is Image)
				(item.Icon as Image).Opacity = (enabled ? 1.0 : 0.5);
			VisualStateManager.GoToState(item, (enabled ? "Normal" : "Disabled"), true);
		}

		private TT GetOwnerAs<TT>() where TT : DependencyObject
		{
			TT owner = Owner as TT;
			if (owner == null)
				owner = (Owner != null ? Owner.ChildOfType<TT>() : null);
			return owner;
		}

		private MenuItem FindMenuItem(string name)
		{
			return base.Items
					.OfType<MenuItem>()
					.Where(item => MatchMenuItem(item, name))
					.FirstOrDefault() as MenuItem;
		}

		private bool MatchMenuItem(MenuItem item, string name)
		{
			return (item.Header != null && item.Header.ToString().StartsWithIgnoreCase(name));
		}

		private void OnCutClick(object sender, RoutedEventArgs e)
		{
			MenuItem item = FindMenuItem("cut");
			if (item == null || !item.IsEnabled)
				return;

			try
			{
				TextBox textBox = GetOwnerAs<TextBox>();
				if (textBox != null)
				{
					Clipboard.SetText(textBox.SelectedText);
					textBox.SelectedText = string.Empty;
				}

				RichTextBox richTextBox = GetOwnerAs<RichTextBox>();
				if (richTextBox != null)
				{
					Clipboard.SetText(richTextBox.Selection.Text);
					richTextBox.Selection.Text = string.Empty;
				}
			}
			catch (SecurityException)
			{
				ShowClipboardError();
			}
		}

		private void OnCopyClick(object sender, RoutedEventArgs e)
		{
			MenuItem item = FindMenuItem("copy");
			if (item == null || !item.IsEnabled)
				return;

			try
			{
				TextBox textBox = GetOwnerAs<TextBox>();
				if (textBox != null)
					Clipboard.SetText(textBox.SelectedText);

				RichTextBox richTextBox = GetOwnerAs<RichTextBox>();
				if (richTextBox != null)
				{
					bool shiftKey = (Keyboard.Modifiers & ModifierKeys.Shift) != 0;
					if (shiftKey)
						Clipboard.SetText(richTextBox.Selection.Xaml);
					else
						Clipboard.SetText(richTextBox.Selection.Text);
				}
			}
			catch (SecurityException)
			{
				ShowClipboardError();
			}
		}

		private void OnPasteClick(object sender, RoutedEventArgs e)
		{
			MenuItem item = FindMenuItem("paste");
			if (item == null || !item.IsEnabled)
				return;

			try
			{
				TextBox textBox = GetOwnerAs<TextBox>();
				if (textBox != null)
					textBox.SelectedText = Clipboard.GetText();

				RichTextBox richTextBox = GetOwnerAs<RichTextBox>();
				if (richTextBox != null)
					richTextBox.Selection.Text = Clipboard.GetText();
			}
			catch (SecurityException)
			{
				ShowClipboardError();
			}
		}

		private void OnDeleteClick(object sender, RoutedEventArgs e)
		{
			MenuItem item = FindMenuItem("delete");
			if (item == null || !item.IsEnabled)
				return;

			TextBox textBox = GetOwnerAs<TextBox>();
			if (textBox != null)
				textBox.SelectedText = string.Empty;

			RichTextBox richTextBox = GetOwnerAs<RichTextBox>();
			if (richTextBox != null)
				richTextBox.Selection.Text = string.Empty;
		}

		private void OnSelectAllClick(object sender, RoutedEventArgs e)
		{
			MenuItem item = FindMenuItem("select");
			if (item == null || !item.IsEnabled)
				return;

			TextBox textBox = GetOwnerAs<TextBox>();
			if (textBox != null)
				textBox.SelectAll();

			RichTextBox richTextBox = GetOwnerAs<RichTextBox>();
			if (richTextBox != null)
				richTextBox.SelectAll();
		}

		private void OnPrintClick(object sender, RoutedEventArgs e)
		{
			MenuItem item = FindMenuItem("print");
			if (item == null || !item.IsEnabled)
				return;

			RichTextBlock richTextBlock = new RichTextBlock();

			TextBox textBox = GetOwnerAs<TextBox>();
			if (textBox != null)
			{
				string text = textBox.SelectedText;
				Paragraph paragraph = new Paragraph();
				paragraph.Inlines.Add(new Run() { Text = text });
				richTextBlock.Blocks.Add(paragraph);
			}

			RichTextBox richTextBox = GetOwnerAs<RichTextBox>();
			if (richTextBox != null)
				richTextBlock.Xaml = richTextBox.Selection.Xaml;

			PrintDocument document = new PrintDocument();
			document.PrintPage += delegate(object s, PrintPageEventArgs args)
			{
				args.PageVisual = richTextBlock;
			};

			// call the Print() with a proper name which will be visible in the Print Queue
			document.Print(UriHelper.UriSite().ToString());
		}

		private const string errorCaption = "Clipboard permission was denied";
		private static readonly string[] error = {
			"To allow clipboard access:",
			"",
			"● Right click this message and click the 'Silverlight' menu item",
			"● Click the 'Permissions' tab",
			"● Find the site listing '" + UriHelper.UriSite().ToString() + "'",
			"● Click the 'Clipboard' permission item to select it",
			"● Click the 'Allow' button, then click 'OK'",
		};

		private void ShowClipboardError()
		{
			StringBuilder builder = new StringBuilder();
			foreach (string line in error)
				builder.AppendFormat("{0}\n", line);
			MessageBox.Show(builder.ToString(), errorCaption, MessageBoxButton.OK);
		}
	}
}
