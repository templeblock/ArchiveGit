using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class EventCollection : ObservableCollection<Event>, IDisposable
	{
		public void Dispose()
		{
			foreach (Event item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	public class Event : NotifyPropertyChanges
	{
		public override void Dispose()
		{
			base.Dispose();
		}

		public static Event Create(string name)
		{
			Event item = new Event();
			return item;
		}
	}
}
