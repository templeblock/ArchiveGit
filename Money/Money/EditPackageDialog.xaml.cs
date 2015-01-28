using System;
using System.Windows;
using ClassLibrary;

namespace Money
{
	public partial class EditPackageDialog : SpendingPackageDialogBase
	{
		private bool m_bCreateNew;
		private Package m_RootPackageOrig;

		public EditPackageDialog(Package package, bool bCreateNew)
			: base()
		{
			InitializeComponent();
			InitializeDialogPanel(true/*bModal*/, x_Name);
			base.Closed += OnDialogClosed;

			m_Title = x_Title;
			m_ParentSearch = null;
			m_DataGrid = x_DataGrid;
			m_ListControl = x_ListControl;
			m_Prev = x_Prev;
			m_Next = x_Next;

			m_RootPackageOrig = package;
			m_RootPackage = m_RootPackageOrig.DeepClone();
			m_bCreateNew = bCreateNew;
			if (m_bCreateNew)
			{
				ProfileDate startProfileDate = ProfileCode.StartOfPlan.ToProfileDate();// or App.Model.ProfileHolder.Profile.StartProfileDate, or DateTime.Now.ToJulian()
				ProfileDate endProfileDate = ProfileCode.EndOfPlan.ToProfileDate();// or App.Model.ProfileHolder.Profile.EndProfileDate;
				m_RootPackage.SetDates(startProfileDate, endProfileDate);
				m_RootPackage.MustHaveATransaction();
			}

			m_TitleFormat = (m_bCreateNew ? "Add" : "Edit") + " the '{0}' spending source";
		}

		private void OnDialogClosed(object sender, EventArgs e)
		{
			base.Closed -= OnDialogClosed;
			if (DialogResult == MessageBoxResult.Cancel)
				return;

			Document document = App.Model.DocumentHolder.Document;
			if (m_bCreateNew)
				document.Packages.AddAndRun(m_RootPackage, document);
			else
				document.Packages.UpdateAndRun(m_RootPackageOrig, m_RootPackage, document);
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
