using System.Collections.ObjectModel;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using ClassLibrary;

namespace Money
{
	public partial class OpenSaveDialog : DialogPanel
	{
		public class IsolatedFileInfo
		{
			public string Name { get; set; }
		}

		public string OpenFilePath { get { return (x_FilePathList.SelectedItem == null ? "" : (x_FilePathList.SelectedItem as IsolatedFileInfo).Name); } }
		public string OpenFilePathXml { get { return AddXmlExtension(OpenFilePath); } }
		public string SaveFilePath { get { return x_FilePath.Text; } }
		public string SaveFilePathXml { get { return AddXmlExtension(SaveFilePath); } }

        private ObservableCollection<IsolatedFileInfo> m_FileInfoList = new ObservableCollection<IsolatedFileInfo>();
		private RoutedEventHandler m_DefaultEvent;
		private bool m_bOverwriteWarning;
		private string m_DefaultFilePath;
		private string m_FileExtension;

		public OpenSaveDialog(string defaultFilePath, string fileType, bool bOpen)
		{
			InitializeComponent();
			InitializeDialogPanel(true/*bModal*/, (bOpen ? x_FilePathList as Control : x_FilePath));
			base.Loaded += OnLoaded;

			m_DefaultFilePath = defaultFilePath;
			m_FileExtension = (fileType[0] == '.' ? "" : ".") + fileType;
			m_bOverwriteWarning = false;

			x_OpenTitle.SetVisible(bOpen);
			x_Open.SetVisible(bOpen);
			x_SaveTitle.SetVisible(!bOpen);
			x_Save.SetVisible(!bOpen);
			x_SavePanel.SetVisible(!bOpen);

			// Set Enter and Escape keys to the default controls
			if (bOpen)
				m_DefaultEvent = OnOpenClick;
			else
				m_DefaultEvent = OnSaveClick;
			base.EnterKey += m_DefaultEvent;
		}

		public override void Dispose()
		{
			base.EnterKey -= m_DefaultEvent;
			m_FileInfoList.Clear();
			m_FileInfoList = null;
			x_FilePathList.ItemsSource = null;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			base.MouseLeftButtonUp += OnDoubleClick;

			string[] fileNames = IsolatedFileHelper.GetFileNames("*" + m_FileExtension);
			if (fileNames == null)
				return;
			string defaultItem = RemoveXmlExtension(m_DefaultFilePath);
			int defaultIndex = -1;
			int index = 0;
			foreach (string fileName in fileNames)
			{
				string item = RemoveXmlExtension(fileName);
				m_FileInfoList.Add(new IsolatedFileInfo() { Name = item });
				if (item == defaultItem)
					defaultIndex = index;
				index++;
			}

			x_FilePathList.ItemsSource = m_FileInfoList;
			x_FilePathList.UpdateLayout();
			if (defaultIndex >= 0)
				x_FilePathList.SelectedIndex = defaultIndex;
			else
				x_FilePath.Text = defaultItem;
		}

		private void OnDoubleClick(object sender, MouseButtonEventArgs e)
		{
			bool doubleClick = MouseButtonHelper.IsDoubleClick(sender, e);
			if (doubleClick)
				m_DefaultEvent(sender, e);
		}

		private void OnSelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			if (x_FilePathList.SelectedItem != null)
				x_FilePath.Text = (x_FilePathList.SelectedItem as IsolatedFileInfo).Name;
		}

		private void OnFilePathChanged(object sender, TextChangedEventArgs e)
		{
			m_bOverwriteWarning = false;
			x_ErrorStatus.Text = null;
		}

		private void OnOpenClick(object sender, RoutedEventArgs e)
		{
			string filePath = OpenFilePathXml;
			bool bExists = IsolatedFileHelper.Exists(filePath);
			if (!bExists)
				return;

			DialogResult = MessageBoxResult.OK;
		}

		private void OnSaveClick(object sender, RoutedEventArgs e)
		{
			string filePath = SaveFilePathXml;
			if (filePath.IsEmpty())
				return;
			bool bExists = IsolatedFileHelper.Exists(filePath);
			if (bExists && !m_bOverwriteWarning)
			{
				x_ErrorStatus.Text = string.Format("Click Save again to overwrite the file", RemoveXmlExtension(filePath));
				m_bOverwriteWarning = true;
				return;
			}

			DialogResult = MessageBoxResult.OK;
		}

		private void OnCancelClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Cancel;
		}

		private string AddXmlExtension(string filePath)
		{
			string extension = Path.GetExtension(filePath);
			if (!m_FileExtension.EqualsIgnoreCase(extension))
				return filePath + m_FileExtension;
			return filePath;
		}

		private string RemoveXmlExtension(string filePath)
		{
			string extension = Path.GetExtension(filePath);
			if (m_FileExtension.EqualsIgnoreCase(extension))
				return Path.ChangeExtension(filePath, null);
			return filePath;
		}
	}
}
