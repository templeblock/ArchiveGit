using System;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using ClassLibrary;
using Id = System.Int64;

namespace Money
{
	public class SpendingPackagesPanel : ViewPanel
	{
		// Static controls
		internal static SpendingPackagesPanel Current;

		internal SpendingPackageTree Tree { get { return m_PackageTree; } }
		private static SpendingPackageTree m_PackageTree;

		private bool m_bForUseInDocument;

		public SpendingPackagesPanel()
		{
			base.Loaded += OnLoaded;

			Current = this;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			if (Helper.IsInDesignTool)
				return;

			DownloadPackages();
		}

		private void DownloadPackages()
		{
			if (m_PackageTree != null)
				return;
			m_PackageTree = new SpendingPackageTree();

			PackagesSqlQuery query = new PackagesSqlQuery();
			query.Result += PackagesQueryResult;
			query.Execute();
		}

		private void PackagesQueryResult(Collection<Package> collection, string error)
		{
			if (error != null)
			{
				MessageBoxEx.ShowError("Web access", error, null);
				return;
			}

			if (collection == null || collection.Count <= 0)
				return;

			PackageCollection packagesList = collection.ToObservableCollection<PackageCollection,Package>();;
			m_PackageTree.Init(packagesList);
			TreeView treeView = this.ChildOfType<TreeView>();
			treeView.ItemsSource = m_PackageTree.Packages;
		}

		private void CreatePackageDialog(Package package)
		{
			if (m_bForUseInDocument)
				new EditPackageDialog(package, true/*bCreateNew*/);
			else
				new DatabasePackageDialog(package);
		}

		internal void CreatePackageDialog(Id id, bool bForUseInDocument)
		{
			m_bForUseInDocument = bForUseInDocument;
			Package package = m_PackageTree.FindPackageById(id);
			if (package == null)
				return;

			if (AllTransactionsLoaded(package))
			{
				CreatePackageDialog(package);
				return;
			}

			TransactionSqlQuery query = new TransactionSqlQuery(package);
			query.Result += delegate(Collection<Transaction> collection, string error)
			{
				if (error != null)
				{
					MessageBoxEx.ShowError("Web access", error, null);
					return;
				}

				TransactionsQueryResult(collection);
				CreatePackageDialog(package);
			};
			query.Execute();
		}

		private void TransactionsQueryResult(Collection<Transaction> collection)
		{
			if (collection == null || collection.Count <= 0)
				return;

			TransactionCollection transactionsList = collection.ToObservableCollection<TransactionCollection,Transaction>();;
			m_PackageTree.AttachTransactions(transactionsList);
		}

		internal void OnSearchTextChanged(object sender, TextChangedEventArgs e)
		{
			TextBox filterTextBox = sender as TextBox;
			string filterText = filterTextBox.Text;
			bool isFilterTextEmpty = filterText.IsEmpty();

			TreeView treeView = this.ChildOfType<TreeView>();
			treeView.ItemsSource = (isFilterTextEmpty ? m_PackageTree.Packages : m_PackageTree.FilterPackages(filterText));
		}

		internal void OnSearchResetClick(object sender, RoutedEventArgs e)
		{
			TextBox searchTextBox = this.ChildOfType<TextBox>();
			searchTextBox.Text = "";
		}

		internal void OnPackagesTreeKeyDown(object sender, KeyEventArgs e)
		{
			bool controlKey = (Keyboard.Modifiers & ModifierKeys.Control) != 0;
			if (e.Key == Key.Delete && controlKey)
			{
				TreeView treeView = (sender as TreeView);
				if (treeView == null)
					return;
				Package package = treeView.SelectedItem as Package;
				Delete(package);
			}
			else
			if (e.Key == Key.Enter)
			{
				TreeView treeView = (sender as TreeView);
				if (treeView == null)
					return;
				Package package = treeView.SelectedItem as Package;
				if (package == null)
					return;
				if (package.HasOption('T'))
					CreatePackageDialog(package.Id, true/*bForUseInDocument*/);
			}
		}

		internal void OnPackagesTreeClick(object sender, MouseButtonEventArgs e)
		{
			Package package = (sender as FrameworkElement).Tag as Package;
			if (package == null)
				return;
			bool controlKey = (Keyboard.Modifiers & ModifierKeys.Control) != 0;
			bool doubleClick = MouseButtonHelper.IsDoubleClick(sender, e);
			if (controlKey)
				CreatePackageDialog(package.Id, false/*bForUseInDocument*/);
			else
			if (doubleClick && package.HasOption('T'))
				CreatePackageDialog(package.Id, true/*bForUseInDocument*/);
		}

		internal void OnAddNewSpendingClick(object sender, RoutedEventArgs e)
		{
			TreeView treeView = this.ChildOfType<TreeView>();
			if (treeView == null)
				return;
			Package package = treeView.SelectedItem as Package;
			if (package != null && package.HasOption('T'))
				CreatePackageDialog(package.Id, true/*bForUseInDocument*/);
		}

		internal void Add(Package package)
		{
			m_PackageTree.Add(package);
			package.TransactionsLoaded = true;

			AddPackageSqlQuery query = new AddPackageSqlQuery(package);
			//query.Result += ;
			query.Execute();
		}

		internal void Update(Package packageOld, Package package)
		{
			m_PackageTree.Update(packageOld, package);
			UpdateDatabaseRecursive(package);
		}

		private bool UpdateDatabaseRecursive(Package package)
		{
			UpdateDatabase(package);
			foreach (Package childPackage in package.Packages)
				UpdateDatabase(childPackage);

			return true;
		}

		internal void UpdateDatabase(Package package)
		{
			UpdatePackageSqlQuery query = new UpdatePackageSqlQuery(package);
			//query.Result += ;
			query.Execute();
		}

		private void Delete(Package package)
		{
			MessageBoxEx.ShowOKCancel("Delete Package", string.Format("Are you sure you want to delete the spending package?\n\n{0}", package.Name), MessageBoxIcon.Question, delegate(object sender, EventArgs e)
			{
				MessageBoxDialog dialog = sender as MessageBoxDialog;
				if (dialog.DialogResult == MessageBoxResult.Cancel)
					return;

				m_PackageTree.Delete(package);

				DeletePackageSqlQuery query = new DeletePackageSqlQuery(package);
				//query.Result += ;
				query.Execute();
			});
		}

		private bool AllTransactionsLoaded(Package package)
		{
			if (!package.TransactionsLoaded)
				return false;

			foreach (Package childPackage in package.Packages)
			{
				if (!AllTransactionsLoaded(childPackage))
					return false;
			}

			return true;
		}
	}

	public class PackageIdToPackage : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			return SpendingPackagesPanel.Current.Tree.FindPackageById((Id)value);
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			return ((Package)value).Id;
		}
	}

	public class PackageIdToImageUri : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			return SpendingPackagesPanel.Current.Tree.FindImageUriById((Id)value);
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

#if false
	internal static class TreeViewExtensions
	{
		internal static TreeViewItem ContainerFromItem(this TreeView treeView, object item)
		{
			TreeViewItem containerThatMightContainItem = (TreeViewItem)treeView.ItemContainerGenerator.ContainerFromItem(item);
			if (containerThatMightContainItem != null)
				return containerThatMightContainItem;
			else
				return ContainerFromItem(treeView.ItemContainerGenerator, treeView.Items, item);
		}

		private static TreeViewItem ContainerFromItem(ItemContainerGenerator parentItemContainerGenerator, ItemCollection itemCollection, object item)
		{
			foreach (object curChildItem in itemCollection)
			{
				TreeViewItem parentContainer = (TreeViewItem)parentItemContainerGenerator.ContainerFromItem(curChildItem);
				TreeViewItem containerThatMightContainItem = (TreeViewItem)parentContainer.ItemContainerGenerator.ContainerFromItem(item);
				if (containerThatMightContainItem != null)
					return containerThatMightContainItem;

				TreeViewItem recursionResult = ContainerFromItem(parentContainer.ItemContainerGenerator, parentContainer.Items, item);
				if (recursionResult != null)
					return recursionResult;
			}

			return null;
		}

		internal static object ItemFromContainer(this TreeView treeView, TreeViewItem container)
		{
			TreeViewItem itemThatMightBelongToContainer = (TreeViewItem)treeView.ItemContainerGenerator.ItemFromContainer(container);
			if (itemThatMightBelongToContainer != null)
				return itemThatMightBelongToContainer;
			else
				return ItemFromContainer(treeView.ItemContainerGenerator, treeView.Items, container);
		}

		private static object ItemFromContainer(ItemContainerGenerator parentItemContainerGenerator, ItemCollection itemCollection, TreeViewItem container)
		{
			foreach (object curChildItem in itemCollection)
			{
				TreeViewItem parentContainer = (TreeViewItem)parentItemContainerGenerator.ContainerFromItem(curChildItem);
				TreeViewItem itemThatMightBelongToContainer = (TreeViewItem)parentContainer.ItemContainerGenerator.ItemFromContainer(container);
				if (itemThatMightBelongToContainer != null)
					return itemThatMightBelongToContainer;

				TreeViewItem recursionResult = ItemFromContainer(parentContainer.ItemContainerGenerator, parentContainer.Items, container) as TreeViewItem;
				if (recursionResult != null)
					return recursionResult;
			}

			return null;
		}
	}

	public class CustomTreeView : TreeView
	{
		protected override void PrepareContainerForItemOverride(DependencyObject element, object item)
		{
			base.PrepareContainerForItemOverride(element, item);
			TreeViewItem treeViewItem = element as TreeViewItem;
			Package package = item as Package;
			if (treeViewItem != null && package != null)
				App.MainPage.DragDrop.AddSource(treeViewItem, package.Id);
		}

		protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
		{
			base.OnMouseLeftButtonDown(e);
		}

		protected override void OnMouseLeftButtonUp(MouseButtonEventArgs e)
		{
			base.OnMouseLeftButtonUp(e);
		}
	}
#endif
}
