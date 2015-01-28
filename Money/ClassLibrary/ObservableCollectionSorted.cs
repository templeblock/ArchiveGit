using System;
using System.Collections.ObjectModel;

namespace ClassLibrary
{
	public class ObservableCollectionSorted<TT> : ObservableCollection<TT> where TT : IComparable
	{
		protected override void InsertItem(int index, TT item)
		{
			// the passed insert index will be ignored
			for (int i = 0; i < base.Count; i++)
			{
				if (Math.Sign(this[i].CompareTo(item)) == 1)
				{
					base.InsertItem(i, item);
					return;
				}
			}

			base.InsertItem(base.Count, item);
		}
	}
}
