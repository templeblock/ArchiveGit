using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using ClassLibrary;

namespace Money
{
	public abstract class SpendingPackageDialogBase : DialogPanel
	{
		private Package m_CurrentPackage;
		private int m_CurrentPackageIndex = 0;
		private PackageCollection m_EditablePackages = new PackageCollection();

		protected Package m_RootPackage;
		protected string m_TitleFormat;

		protected TextBlock m_Title;
		protected AutoCompleteBox m_ParentSearch;
		protected Button m_Prev;
		protected Button m_Next;
		protected Button m_OK;
		protected Button m_Cancel;

		protected DataGrid m_DataGrid;
		protected ItemsControl m_ListControl;

		public SpendingPackageDialogBase()
		{
			base.Loaded += OnLoaded;
		}

		public override void Dispose()
		{
			base.MouseLeftButtonDown -= OnMouseButtonDown;
			base.DataContext = null;

			m_EditablePackages.Clear();
			m_EditablePackages = null;

			if (m_ParentSearch != null)
				m_ParentSearch.ItemsSource = null;

			if (m_DataGrid != null)
			{
				m_DataGrid.CurrentCellChanged -= OnCurrentCellChanged;
				m_DataGrid.KeyDown -= OnItemsKeyDown;
				m_DataGrid.ItemsSource = null;
			}

			if (m_ListControl != null)
			{
				m_ListControl.KeyDown -= OnItemsKeyDown;
				m_ListControl.ItemsSource = null;
			}
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			LoadEditablePackagesList(m_RootPackage);
			SetCurrentPackage();

			base.MouseLeftButtonDown += OnMouseButtonDown;
		}

		protected void OnMouseButtonDown(object sender, MouseButtonEventArgs e)
		{
			bool doubleClick = MouseButtonHelper.IsDoubleClick(sender, e);
			if (doubleClick)
				InsertNewTransaction(sender);
		}

		private void LoadEditablePackagesList(Package package)
		{
			bool bOkToAdd = (package.Transactions.Count != 0 || package == m_RootPackage);
			if (bOkToAdd)
				m_EditablePackages.Add(package);

			foreach (Package childPackage in package.Packages)
				LoadEditablePackagesList(childPackage);
		}

		private void SetCurrentPackage()
		{
			m_CurrentPackage = m_EditablePackages[m_CurrentPackageIndex];
			base.DataContext = m_CurrentPackage;

			if (m_Title != null)
				m_Title.Text = string.Format(m_TitleFormat, m_CurrentPackage.Name);
			if (m_Prev != null)
			{
				m_Prev.Visibility = (m_EditablePackages.Count > 1 ? Visibility.Visible : Visibility.Collapsed);
				m_Prev.IsEnabled = !(m_CurrentPackageIndex <= 0);
			}
			if (m_Next != null)
			{
				m_Next.Visibility = (m_EditablePackages.Count > 1 ? Visibility.Visible : Visibility.Collapsed);
				m_Next.IsEnabled = !(m_CurrentPackageIndex >= m_EditablePackages.Count - 1);
			}
			if (m_ParentSearch != null)
			{
				m_ParentSearch.IsEnabled = (m_CurrentPackageIndex == 0);
				m_ParentSearch.ItemsSource = SpendingPackagesPanel.Current.Tree.FilterPackages(null);
				m_ParentSearch.ItemFilter = delegate(string search, object item)
				{
					Package package = item as Package;
					return (package != null && package.Name.ContainsIgnoreCase(search));
				};
			}

			if (m_DataGrid != null)
			{
				m_DataGrid.CurrentCellChanged += OnCurrentCellChanged;
				m_DataGrid.KeyDown += OnItemsKeyDown;
				m_DataGrid.ItemsSource = m_CurrentPackage.Transactions;
				//m_DataGrid.LoadingRow += OnLoadingRow;
			}

			if (m_ListControl != null)
			{
				m_ListControl.KeyDown += OnItemsKeyDown;
				m_ListControl.ItemsSource = m_CurrentPackage.Transactions;
			}
		}

		//void OnLoadingRow(object sender, DataGridRowEventArgs e)
		//{
		//    e.Row.MouseEnter += OnRowMouseEnter;
		//}

		//void OnRowMouseEnter(object sender, MouseEventArgs e)
		//{
		//    DataGridRow row = sender as DataGridRow;
		//    m_DataGrid.SelectedIndex = row.GetIndex();
		//}

		private void DeleteSelectedTransaction(object sender)
		{
			if (m_DataGrid != null && m_DataGrid.IsVisible() && !m_DataGrid.IsReadOnly)
				DataGrid_DeleteSelectedTransaction(sender);
			if (m_ListControl != null && m_ListControl.IsVisible())
				ListControl_DeleteSelectedTransaction(sender);
		}

		private void DataGrid_DeleteSelectedTransaction(object sender)
		{
			DataGridRow item = (sender as DependencyObject).ParentOfType<DataGridRow>();
			if (item != null)
				m_DataGrid.SelectedIndex = item.GetIndex();
			Transaction transaction = (m_DataGrid.SelectedItem as Transaction);
			try
			{
				if (transaction != null)
					m_CurrentPackage.Transactions.Remove(transaction);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
			}
		}

		private void ListControl_DeleteSelectedTransaction(object sender)
		{
			ContentPresenter item = (sender as DependencyObject).ParentOfType<ContentPresenter>();
			if (item == null)
				return;
			Transaction transaction = (item.Content as Transaction);
			try
			{
				if (transaction != null)
					m_CurrentPackage.Transactions.Remove(transaction);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
			}
		}

		private void InsertNewTransaction(object sender)
		{
			if (m_DataGrid != null && m_DataGrid.IsVisible() && !m_DataGrid.IsReadOnly)
				DataGrid_InsertNewTransaction(sender);
			if (m_ListControl != null && m_ListControl.IsVisible())
				ListControl_InsertNewTransaction(sender);
		}

		private void DataGrid_InsertNewTransaction(object sender)
		{
			DataGridRow item = (sender as DependencyObject).ParentOfType<DataGridRow>();
			if (item != null)
				m_DataGrid.SelectedIndex = item.GetIndex();
			Transaction transaction = (m_DataGrid.SelectedItem as Transaction);
			int index = m_CurrentPackage.Transactions.IndexOf(transaction);
			if (index >= 0)
				index++;
			else
				index = m_CurrentPackage.Transactions.Count;

			m_CurrentPackage.Transactions.Insert(index, Transaction.CreateDefault(m_CurrentPackage.Id));
			m_DataGrid.SelectedIndex = index;
		}

		private void ListControl_InsertNewTransaction(object sender)
		{
			ContentPresenter item = (sender as DependencyObject).ParentOfType<ContentPresenter>();
			Transaction transaction = (item != null ? item.Content as Transaction : null);
			int index = m_CurrentPackage.Transactions.IndexOf(transaction);
			if (index >= 0)
				index++;
			else
				index = m_CurrentPackage.Transactions.Count;

			m_CurrentPackage.Transactions.Insert(index, Transaction.CreateDefault(m_CurrentPackage.Id));
			//j	m_ListControl.SelectedIndex = index;
		}

		private void OnItemsKeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Delete)
				DeleteSelectedTransaction(sender);
			else
			if (e.Key == Key.Insert)
				InsertNewTransaction(sender);
		}

		private void OnCurrentCellChanged(object sender, EventArgs e)
		{
			if (m_DataGrid != null && m_DataGrid.IsVisible() && !m_DataGrid.IsReadOnly)
				m_DataGrid.BeginEdit();
		}

		protected void _OnInsertClick(object sender, RoutedEventArgs e)
		{
			InsertNewTransaction(sender);
		}

		protected void _OnDeleteClick(object sender, RoutedEventArgs e)
		{
			DeleteSelectedTransaction(sender);
		}

		protected void _OnPrevClick(object sender, RoutedEventArgs e)
		{
			if (m_CurrentPackageIndex <= 0)
				return;
			m_CurrentPackageIndex--;
			SetCurrentPackage();
		}

		protected void _OnNextClick(object sender, RoutedEventArgs e)
		{
			if (m_CurrentPackageIndex >= m_EditablePackages.Count - 1)
				return;
			m_CurrentPackageIndex++;
			SetCurrentPackage();
		}

		protected void _OnOkClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.OK;
		}

		protected void _OnCancelClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Cancel;
		}

		protected void _OnDropDownClosed(object sender, EventArgs e)
		{
			if (sender is Control)
				(sender as Control).FocusEx();
		}
	}
}
