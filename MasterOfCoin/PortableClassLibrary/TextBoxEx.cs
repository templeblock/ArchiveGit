using System;
using System.Windows.Input;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace ClassLibrary
{
	// This TextBox derived class selects all text when it receives focus
	public class TextBoxFocusSelect : TextBox, IDisposable
	{
		// The EnterKeyCommand dependency property
		public static readonly DependencyProperty EnterKeyCommandProperty = DependencyProperty.Register("EnterKeyCommand", typeof(ICommand), typeof(TextBoxFocusSelect), null);
		public ICommand EnterKeyCommand
		{
			get { return (ICommand)GetValue(EnterKeyCommandProperty); }
			set { SetValue(EnterKeyCommandProperty, value); }
		}

		// The HasFocus dependency property
		public static readonly DependencyProperty HasFocusProperty = DependencyProperty.Register("HasFocus", typeof(bool), typeof(TextBoxFocusSelect), null);
		public bool HasFocus
		{
			get { return (bool)GetValue(HasFocusProperty); }
			set { SetValue(HasFocusProperty, value); }
		}

		// The HasFocusOrText dependency property
		public static readonly DependencyProperty HasFocusOrTextProperty = DependencyProperty.Register("HasFocusOrText", typeof(bool), typeof(TextBoxFocusSelect), null);
		public bool HasFocusOrText
		{
			get { return (bool)GetValue(HasFocusOrTextProperty); }
			set { SetValue(HasFocusOrTextProperty, value); }
		}

		// The HasText dependency property
		public static readonly DependencyProperty HasTextProperty = DependencyProperty.Register("HasText", typeof(bool), typeof(TextBoxFocusSelect), null);
		public bool HasText
		{
			get { return (bool)GetValue(HasTextProperty); }
			set { SetValue(HasTextProperty, value); }
		}

		public TextBoxFocusSelect()
		{
			GotFocus += OnGotFocus;
			LostFocus += OnLostFocus;
			TextChanged += OnTextChanged;
			KeyDown += OnKeyDown;

			// Make this a numeric TextBox
			InputScope = new InputScope();
			InputScope.Names.Add(new InputScopeName { NameValue = InputScopeNameValue.NumberFullWidth });
		}

		public virtual void Dispose()
		{
			GotFocus -= OnGotFocus;
			LostFocus -= OnLostFocus;
			TextChanged -= OnTextChanged;
			KeyDown -= OnKeyDown;
		}

		private void OnGotFocus(object sender, RoutedEventArgs e)
		{
			HasFocus = true;
			HasFocusOrText = HasFocus || HasText;
			SelectAll();
		}

		private void OnLostFocus(object sender, RoutedEventArgs e)
		{
			HasFocus = false;
			HasFocusOrText = HasFocus || HasText;
		}

		void OnTextChanged(object sender, TextChangedEventArgs e)
		{
			HasText = !Text.IsEmpty();
			HasFocusOrText = HasFocus || HasText;
		}

		private void OnKeyDown(object sender, KeyRoutedEventArgs e)
		{
			if (e.Key == VirtualKey.Enter && EnterKeyCommand != null)
				EnterKeyCommand.Execute(this);
		}
	}
}
