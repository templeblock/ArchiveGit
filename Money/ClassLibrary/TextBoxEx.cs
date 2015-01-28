using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace ClassLibrary
{
	// This TextBox derived class selects all text when it receives focus
	public class TextBoxFocusSelect : TextBox, IDisposable
	{
		// The HasFocus dependency property
		public static readonly DependencyProperty HasFocusProperty =
			DependencyProperty.Register("HasFocus", typeof(bool), typeof(TextBoxFocusSelect),
			null);

		public bool HasFocus
		{
			get { return (bool)GetValue(HasFocusProperty); }
			set { SetValue(HasFocusProperty, value); }
		}

		// The HasFocusOrText dependency property
		public static readonly DependencyProperty HasFocusOrTextProperty =
			DependencyProperty.Register("HasFocusOrText", typeof(bool), typeof(TextBoxFocusSelect),
			null);

		public bool HasFocusOrText
		{
			get { return (bool)GetValue(HasFocusOrTextProperty); }
			set { SetValue(HasFocusOrTextProperty, value); }
		}

		// The HasText dependency property
		public static readonly DependencyProperty HasTextProperty =
			DependencyProperty.Register("HasText", typeof(bool), typeof(TextBoxFocusSelect),
			null);

		public bool HasText
		{
			get { return (bool)GetValue(HasTextProperty); }
			set { SetValue(HasTextProperty, value); }
		}


		public TextBoxFocusSelect()
		{
			base.GotFocus += OnGotFocus;
			base.LostFocus += OnLostFocus;
			base.TextChanged += OnTextChanged;
		}

		public virtual void Dispose()
		{
			base.GotFocus -= OnGotFocus;
			base.LostFocus -= OnLostFocus;
			base.TextChanged -= OnTextChanged;
		}

		private void OnGotFocus(object sender, RoutedEventArgs e)
		{
			HasFocus = true;
			HasFocusOrText = HasFocus || HasText;
			base.SelectAll();
		}

		private void OnLostFocus(object sender, RoutedEventArgs e)
		{
			HasFocus = false;
			HasFocusOrText = HasFocus || HasText;
		}

		void OnTextChanged(object sender, TextChangedEventArgs e)
		{
			HasText = !base.Text.IsEmpty();
			HasFocusOrText = HasFocus || HasText;
		}
	}

	// This TextBox derived class sets the focus to the next control when enter is pressed
	public class TextBoxEx : TextBoxFocusSelect
	{
		public TextBoxEx()
		{
			base.KeyDown += OnKeyDown;
		}

		public override void Dispose()
		{
			base.Dispose();
			base.KeyDown -= OnKeyDown;
		}

		private void OnKeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key != Key.Enter)
				return;
			Control sibling = this.NextControlSibling();
			if (sibling != null)
				sibling.FocusEx();
		}
	}
}
