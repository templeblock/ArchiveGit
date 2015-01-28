using System;
using System.Windows;
using ClassLibrary;

namespace Money
{
	public partial class DatabasePackageDialog : SpendingPackageDialogBase
	{
		private bool m_bCreateNew;
		private Package m_RootPackageOrig;

		public DatabasePackageDialog(Package package)
			: base()
		{
			InitializeComponent();
			InitializeDialogPanel(true/*bModal*/, x_Name);
			base.Closed += OnDialogClosed;

			m_Title = x_Title;
			m_ParentSearch = x_ParentSearch;
			m_DataGrid = x_DataGrid;
			m_ListControl = x_ListControl;
			m_Prev = x_Prev;
			m_Next = x_Next;

			if (package == null)
			{
				m_bCreateNew = true;
				m_RootPackageOrig = null;
				m_RootPackage = Package.CreateDefault();
				m_RootPackage.MustHaveATransaction();
			}
			else
			{
				m_bCreateNew = false;
				m_RootPackageOrig = package;
				m_RootPackage = package.DeepClone();
			}

			m_TitleFormat = (m_bCreateNew ? "Add a new database package" : "Edit the '{0}' database package");
		}

		private void OnDialogClosed(object sender, EventArgs e)
		{
			base.Closed -= OnDialogClosed;
			if (DialogResult == MessageBoxResult.Cancel)
				return;

			bool hasTransactions = (m_RootPackage.Transactions.Count > 0);
			m_RootPackage.OptionsAddRemove(hasTransactions, "T");
			if (m_bCreateNew)
				SpendingPackagesPanel.Current.Add(m_RootPackage);
			else
				SpendingPackagesPanel.Current.Update(m_RootPackageOrig, m_RootPackage);
		}

		private void OnInsertClick(object sender, RoutedEventArgs e)
		{
			_OnInsertClick(sender, e);
		}

		private void OnDeleteClick(object sender, RoutedEventArgs e)
		{
			_OnDeleteClick(sender, e);
		}

		private void OnPrevClick(object sender, RoutedEventArgs e)
		{
			_OnPrevClick(sender, e);
		}

		private void OnNextClick(object sender, RoutedEventArgs e)
		{
			_OnNextClick(sender, e);
		}

		private void OnOkClick(object sender, RoutedEventArgs e)
		{
			_OnOkClick(sender, e);
		}

		private void OnCancelClick(object sender, RoutedEventArgs e)
		{
			_OnCancelClick(sender, e);
		}

		private void OnDropDownClosed(object sender, EventArgs e)
		{
			_OnDropDownClosed(sender, e);
		}
	}
}
