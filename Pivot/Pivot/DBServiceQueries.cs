using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using ClassLibrary;

namespace Pivot
{
#if NOTUSED
	internal class TransactionSqlQuery : DBServiceBase<Transaction>
	{
		private Package _package;

		internal TransactionSqlQuery(Package package)
		{
			_package = package;
		}

		internal override string SqlQuery()
		{
			string idList = PackageIdList(_package);
			if (idList.IsEmpty())
				return null;
			return string.Format(@"
				SELECT Id, PackageId, Name, Frequency, Amount, Type, SourceAccount, TargetAccount, Duration, StartLate
				FROM _FM_Transactions
				WHERE {0};",
					idList);
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			IEnumerable<Transaction> query = from item in element.Descendants()
			select new Transaction
			{
				Id = item.AttributeValue("Id").ToInt64(),
			};

			return query;
		}
	}
#endif
}
