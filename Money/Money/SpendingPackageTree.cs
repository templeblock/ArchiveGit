using System.Collections.Generic;
using ClassLibrary;
using Id = System.Int64;

namespace Money
{
	public class SpendingPackageTree
	{
		public PackageCollection Packages { get { return m_RootPackage.Packages; } }
		private Package m_RootPackage = new Package();
		private PackageCollection m_Packages = new PackageCollection();

		internal void Init(PackageCollection packages)
		{
			m_Packages.Clear();

			// Initialize the root package
			m_RootPackage = new Package();
			m_Packages.Add(m_RootPackage);

			// Create a flattened list of packages
			for (int i = 0; i < packages.Count; i++)
			{
				Package package = new Package(packages[i]);
				m_RootPackage.Packages.Add(package);
			}

			// Convert the flattened list of packages into an actual tree
			for (int i = 0; i < m_RootPackage.Packages.Count; i++)
			{
				Package package = m_RootPackage.Packages[i];
				Package parentPackage = FindPackageById(package.ParentId);
				if (parentPackage == null)
					continue;
				parentPackage.Packages.Add(package);
				m_RootPackage.Packages.Remove(package);
				i--;
			}
		}

		internal void AttachTransactions(TransactionCollection transactions)
		{
			foreach (Transaction transaction in transactions)
			{
				Package package = FindPackageById(transaction.PackageId);
				if (package == null)
					continue;
				package.Transactions.Add(transaction);
				package.TransactionsLoaded = true;
			}
		}

		internal void Add(Package package)
		{
			if (package == null)
				return;
			Package parentPackage = FindPackageById(package.ParentId);
			if (parentPackage == null)
				parentPackage = m_RootPackage;
			parentPackage.Packages.Add(package);
		}

		internal void Update(Package packageOld, Package package)
		{
			if (packageOld == null || package == null)
				return;
			Package parentPackageOld = FindPackageById(packageOld.ParentId);
			if (parentPackageOld == null)
				parentPackageOld = m_RootPackage;
			parentPackageOld.Packages.Remove(packageOld);

			Package parentPackage = FindPackageById(package.ParentId);
			if (parentPackage == null)
				parentPackage = m_RootPackage;
			parentPackage.Packages.Add(package);
		}

		internal void Delete(Package package)
		{
			if (package == null)
				return;
			Package parentPackage = FindPackageById(package.ParentId);
			if (parentPackage == null)
				parentPackage = m_RootPackage;
			parentPackage.Packages.Remove(package);

			foreach (Package childPackage in package.Packages)
			{ // Reparent the children
				parentPackage.Packages.Add(childPackage);
				childPackage.ParentId = parentPackage.Id;
			}

			package.Packages.Clear();
		}

		internal string FindImageUriById(Id id)
		{
			Package package = FindPackageById(id);
			string iconName = (package == null ? "unknown" : package.Icon);
			string imageName = iconName + ".png";
			string imageUri = UriHelper.UriImages(imageName).ToString();
			return imageUri;
		}

		internal Package FindPackageById(Id id)
		{
			return FindPackageById(m_RootPackage.Packages, id);
		}

		private Package FindPackageById(IEnumerable<Package> packageList, Id id)
		{
			if (id == 0)
				return null;
			foreach (Package package in packageList)
			{
				if (package.Id == id)
					return package;
				Package childPackage = FindPackageById(package.Packages, id);
				if (childPackage != null)
					return childPackage;
			}

			return null;
		}

#if NOTUSED
		internal Package FindPackageByName(IEnumerable<Package> packageList, string name)
		{
			if (name.IsEmpty())
				return null;
			foreach (Package package in packageList)
			{
				if (string.Compare(package.Name, name, StringComparison.CurrentCultureIgnoreCase) == 0)
					return package;
				Package childPackage = FindPackageByName(package.Packages, name);
				if (childPackage != null)
					return childPackage;
			}

			return null;
		}
#endif
		internal IEnumerable<Package> FilterPackages(string filterText)
		{
			return FilterPackages(m_RootPackage.Packages, (filterText == null ? null : filterText.ToLower()));
		}

		private IEnumerable<Package> FilterPackages(IEnumerable<Package> packageList, string filterText)
		{
			foreach (Package package in packageList)
			{
				// null filterText includes everything
				if (filterText == null || package.Name.ToLower().Contains(filterText))
					yield return package;
				foreach (Package childPackage in FilterPackages(package.Packages, filterText))
				{
					yield return childPackage;
				}
			}
		}
	}
}
