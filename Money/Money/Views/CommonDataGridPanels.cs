using System;
using System.Collections;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;

namespace Money
{
	public abstract class CommonDataGridPanels : ViewPanel
	{
		private DataGrid _DataGrid;

		internal abstract IEnumerable GetItemsSource();
		internal abstract TriSelector GetContentSelector();
		internal abstract void EditItem(object item);
		internal abstract void DeleteItem(object item);

		public CommonDataGridPanels()
		{
			base.Loaded += OnLoaded;
			base.VisibilityChange += OnVisibilityChange;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			_DataGrid = this.ChildOfType<DataGrid>();

			// Start listening to the document holder
			App.Model.DocumentHolder.DocumentCreated += OnDocumentCreated;
			OnDocumentCreated(null, null);
		}

		private void OnVisibilityChange(bool visible)
		{
			if (base.IsVisible)
				AttachBinding();
			else
				DetachBinding();
		}

		private void OnDocumentCreated(object sender, PropertyChangedEventArgs e)
		{
			if (base.IsVisible)
				AttachBinding();
		}

		private void DetachBinding()
		{
			if (_DataGrid != null)
				_DataGrid.ItemsSource = null;
		}

		private void AttachBinding()
		{
			if (_DataGrid != null)
				_DataGrid.ItemsSource = GetItemsSource();
		}

		internal void OnAddClick(object sender, RoutedEventArgs e)
		{
			TriSelector selector = GetContentSelector();
			if (selector.Value == "B")
				selector.Value = "A";
			else
				selector.Value = "B";
		}

		internal void OnEditClick(object sender, RoutedEventArgs e)
		{
			if (_DataGrid == null /*|| _DataGrid.IsReadOnly*/) //j
				return;

			DataGridRow item = (sender as DependencyObject).ParentOfType<DataGridRow>();
			if (item != null)
				_DataGrid.SelectedIndex = item.GetIndex();

			EditItem(_DataGrid.SelectedItem);
		}

		internal void OnDeleteClick(object sender, RoutedEventArgs e)
		{
			DeleteSelectedItem(sender);
		}

		private void DeleteSelectedItem(object sender)
		{
			if (_DataGrid == null /*|| _DataGrid.IsReadOnly*/) //j
				return;

			DataGridRow item = (sender as DependencyObject).ParentOfType<DataGridRow>();
			if (item != null)
				_DataGrid.SelectedIndex = item.GetIndex();

			DeleteItem(_DataGrid.SelectedItem);
		}
	}

	public class IncomeDataGridPanel : CommonDataGridPanels
	{
		internal override IEnumerable GetItemsSource()
		{
			return App.Model.DocumentHolder.Document.Incomes;
		}

		internal override TriSelector GetContentSelector()
		{
			IncomePage page = this.ParentOfType<IncomePage>();
			return page.x_IncomeSelector;
		}

		internal override void EditItem(object item)
		{
			Income income = (item as Income);
			if (income != null)
				new IncomeDialog(income, false/*bCreateNew*/);
		}

		internal override void DeleteItem(object item)
		{
			Income income = (item as Income);
			if (income == null)
				return;

			MessageBoxEx.ShowOKCancel("Delete Income Source", string.Format("Are you sure you want to delete the income source?\n\n{0}", income.Name), MessageBoxIcon.Question, delegate(object sender, EventArgs e)
			{
				MessageBoxDialog dialog = sender as MessageBoxDialog;
				if (dialog.DialogResult == MessageBoxResult.Cancel)
					return;

				Document document = App.Model.DocumentHolder.Document;
				document.Incomes.RemoveAndRun(income, document);
			});
		}
	}

	public class SpendingDataGridPanel : CommonDataGridPanels
	{
		internal override IEnumerable GetItemsSource()
		{
			return App.Model.DocumentHolder.Document.Packages;
		}

		internal override TriSelector GetContentSelector()
		{
			SpendingPage page = this.ParentOfType<SpendingPage>();
			return page.x_PlanEventsSelector;
		}

		internal override void EditItem(object item)
		{
			Package package = (item as Package);
			if (package != null)
				new EditPackageDialog(package, false/*bCreateNew*/);
		}

		internal override void DeleteItem(object item)
		{
			Package package = (item as Package);
			if (package == null)
				return;

			MessageBoxEx.ShowOKCancel("Delete Spending Package", string.Format("Are you sure you want to delete the spending package?\n\n{0}", package.Name), MessageBoxIcon.Question, delegate(object sender, EventArgs e)
			{
				MessageBoxDialog dialog = sender as MessageBoxDialog;
				if (dialog.DialogResult == MessageBoxResult.Cancel)
					return;

				Document document = App.Model.DocumentHolder.Document;
				document.Packages.RemoveAndRun(package, document);
			});
		}
	}
}
