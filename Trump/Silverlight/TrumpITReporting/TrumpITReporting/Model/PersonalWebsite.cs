using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class PersonalWebsiteCollection : ObservableCollection<PersonalWebsite>, IDisposable
	{
		public void Dispose()
		{
			foreach (PersonalWebsite item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	public class PersonalWebsite : NotifyPropertyChanges
	{
		public EventCollection Events { get { return _Events; } set { SetProperty(ref _Events, value); } }
		private EventCollection _Events = new EventCollection();

		public override void Dispose()
		{
			base.Dispose();
		}

		public static PersonalWebsite Create(string name)
		{
			PersonalWebsite item = new PersonalWebsite();
			return item;
		}
	}
}
