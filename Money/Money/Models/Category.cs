using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using ClassLibrary;
using JulianDay = System.UInt32;
using Millicents = System.Int64;

namespace Money
{
	public class Category : ViewModelBase
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Name;

		public Millicents Balance { get { return _Balance; } set { SetProperty(ref _Balance, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Millicents _Balance;

		private BalanceList _Balances = new BalanceList();

		public Category()
		{
			_Name = "";
			_Balance = 0;
		}

		public override void Dispose()
		{
			base.Dispose();
			_Balances.Dispose();
//j			m_Balance = new Balance();
		}

		internal Millicents GetBalance(JulianDay date, bool bStartOfTheDay)
		{
			return _Balances.GetAmount(date, bStartOfTheDay);
		}

		internal void SetBalance(JulianDay date, Millicents balance)
		{
			_Balances.SetAmount(date, balance);
		}

		internal void TruncateBalance(JulianDay date)
		{
			_Balances.Truncate(date);
		}

		internal bool IsUnused()
		{
			return (_Balances.Count <= 0);
		}
	}

	public class CategoryCollection : ObservableCollection<Category>, IDisposable
	{
		public void Dispose()
		{
			foreach (Category category in this)
				category.Dispose();
			Clear();
		}

		internal void IncreaseCategoryBalance(string name, JulianDay date, Millicents amount)
		{
			Category category = Find(name, true/*bCreate*/);
			if (category == null)
				return;

			Millicents balance = category.GetBalance(date, false/*bStartOfTheDay*/);
			category.SetBalance(date, balance + amount);
		}

		internal Category Find(string name, bool bCreate)
		{
			// Loop thru all the categories
			foreach (Category category in this)
			{
				// If we found a name match, return it
				if (category.Name == name)
					return category;
			}

			// We didn't find a category by that name, and we won't create a new one, so get out
			if (!bCreate)
				return null;

			// Create a category, by request
			Category newCategory = new Category();
			newCategory.Name = name;
			if (newCategory != null && !Contains(newCategory))
				Add(newCategory);

			return newCategory;
		}

		internal void TruncateBalance(JulianDay startDate)
		{
			foreach (Category category in this)
				category.TruncateBalance(startDate);
		}

		internal void UpdateBalances(JulianDay date, BackgroundWorker worker, DoWorkEventArgs e)
		{
			// Delete any category items that are no longer in use
			for (int i = 0; i < Count; i++)
			{
				if (worker != null && worker.CancellationPending)
				{
					e.Cancel = true;
					return;
				}

				Category category = this[i];
				if (category.IsUnused())
				{
					Remove(category);
					i--;
				}
			}

			foreach (Category category in this)
			{ // Delete any new items
				if (worker != null && worker.CancellationPending)
				{
					e.Cancel = true;
					return;
				}

				category.Balance = category.GetBalance(date, false/*bStartOfTheDay*/);
			}
		}
	}
}
