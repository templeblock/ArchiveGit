// Find: public[ \t]+{.*}[ \t\n]+.*\{[ \t\n]+get[ \t\n]+\{[ \t\n]+{.*}[ \t\n]+\}[ \t\n]+set[ \t\n]+\{[ \t\n]+{.*}[ \t\n]+\}[ \t\n]+\}
// Replace: public \1 { get { \2 } set { \3 } }
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class TemplateCollection : ObservableCollection<Template>, IDisposable
	{
		public void Dispose()
		{
			foreach (Template item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	internal class TemplateSqlQuery : DBServiceBase<Template>
	{
		private string _accountID;

		internal TemplateSqlQuery(string accountID)
		{
			_accountID = accountID;
		}

		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT *
				FROM Template
				WHERE (AccountID = '{0}')", _accountID);
		}

		internal override IEnumerable LinqQuery(XDocument xmlDoc)
		{
			IEnumerable<Template> query = from item in xmlDoc.Root.Descendants()
			//select Extract(item);
			select new Template
			{
				Modified = item.AttributeValue("Modified").ToBool(),
			};

			return query;
		}
	}

	public class Template : NotifyPropertyChanges
	{
		public bool Modified { get { return _Modified; } set { SetProperty(ref _Modified, value); } }
		private bool _Modified;

		public override void Dispose()
		{
			base.Dispose();
		}
	}
}
