using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using ClassLibrary;
using Money.LanguageService;

namespace Money
{
	internal class Translator
	{
		// For app id's: http://www.bing.com/developers/appids.aspx
		private const string TRANSLATOR_APP_NAME = "YinYangMe";
		private const string TRANSLATOR_APP_ID = "DFE45240AB302EF81D98FC25FF97F2697BC70967";
		private const string SOURCE_LANGUAGE = "en";
		private const string TARGET_LANGUAGE = "fr"; //j CultureInfo.CurrentCulture.TwoLetterISOLanguageName;
		private readonly LanguageServiceClient m_LanguageServiceClient = new LanguageServiceClient();

		public Translator()
		{
		}

		public void Init(DependencyObject element)
		{
			m_LanguageServiceClient.TranslateCompleted += TranslateCompleted;
			if (SOURCE_LANGUAGE != TARGET_LANGUAGE)
				TranslateElements(element);  //SetLoadedEvents(element);
		}

#if false
		private void SetLoadedEvents(DependencyObject element)
		{
			if (element == null)
				return;

			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(element, i);
				if (child is FrameworkElement)
					(child as FrameworkElement).Loaded += OnLoaded;
				if (child is DependencyObject)
					SetLoadedEvents(child);
			}
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			FrameworkElement element = (sender as FrameworkElement);
			element.Loaded -= OnLoaded;
			TranslateElements(element);
		}
#endif

		private void TranslateElements(DependencyObject element)
		{
			if (element == null)
				return;

			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(element, i);
				if (child is UIElement)
					TranslateElement(child as UIElement);
				if (child is DependencyObject)
					TranslateElements(child);
			}
		}

		private void TranslateElement(UIElement element)
		{
			// contentcontrol elements (button, etc)
			if (element is ContentControl)
			{
				ContentControl contentControl = element as ContentControl;
				if (contentControl != null && contentControl.Content is string)
					Translate(contentControl.Content.ToString(), contentControl);
			}
			else
			if (element is TextBlock)
			{
				TextBlock textBlock = element as TextBlock;
				if (textBlock != null)
					Translate(textBlock.Text, textBlock);
			}
			else
			if (element is TextBox)
			{
				TextBox textBox = element as TextBox;
				if (textBox != null)
					Translate(textBox.Text, textBox);
			}
#if false
			else
			if (element is TabControl)
				return;
			else
			if (element is TabItem)
				return;
			else
			if (element is ComboBox)
				return;
			else
			if (element is ComboBoxItem)
				return;
			else
			if (element is ListBox)
				return;
			else
			if (element is ListBoxItem)
				return;
			else
			if (element is TreeView)
				return;
			else
			if (element is TreeViewItem)
				return;
#endif
		}

		private void Translate(string text, object userState)
		{
			if (text.IsEmpty())
				return;

			m_LanguageServiceClient.TranslateAsync(TRANSLATOR_APP_ID, text, SOURCE_LANGUAGE, TARGET_LANGUAGE, userState);
		}

		private static void TranslateCompleted(object sender, TranslateCompletedEventArgs e)
		{
			string errorStatus = (e.Error != null ? e.Error.Message : null);
			if (errorStatus != null)
			{
				//MessageBoxEx.ShowOK("Translate Error", errorStatus, null);
				return;
			}

			var element = e.UserState;
			if (element is ContentControl)
			{
				ContentControl contentControl = e.UserState as ContentControl;
				if (contentControl != null)
					contentControl.Content = e.Result;
			}
			else
			if (element is TextBlock)
			{
				TextBlock textBlock = e.UserState as TextBlock;
				if (textBlock != null)
					textBlock.Text = e.Result;
			}
			else
			if (element is TextBox)
			{
				TextBox textBox = e.UserState as TextBox;
				if (textBox != null)
					textBox.Text = e.Result;
			}
		}
	}
}
