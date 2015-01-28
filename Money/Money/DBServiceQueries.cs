using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using ClassLibrary;

namespace Money
{
	internal class AddPackageSqlQuery : DBServiceBase<Transaction>
	{
		private Package _package;

		internal AddPackageSqlQuery(Package package)
		{
			_package = package;
		}

		internal override string SqlQuery()
		{
			StringBuilder sql = new StringBuilder();

			// InsertPackage
			sql.Append(string.Format(@"
				INSERT INTO _FM_Packages
				(Id, Name, ParentId, Icon, Options, Sort)
				VALUES ({0}, '{1}', {2}, '{3}', '{4}', {5});",
					_package.Id, _package.Name, _package.ParentId, _package.Icon, _package.Options, _package.Sort));

			if (_package.Transactions.Count <= 0)
				return sql.ToString();

			// InsertTransactions
			sql.Append(string.Format(@"
				INSERT INTO _FM_Transactions
				(Id, PackageId, Name, Frequency, Amount, Type, SourceAccount, TargetAccount, Duration, StartLate)
				VALUES "));
			string prefix = string.Empty;
			foreach (Transaction transaction in _package.Transactions)
			{
				sql.Append(string.Format("{0}({1}, {2}, '{3}', '{4}', {5}, '{6}', '{7}', '{8}', {9}, {10})",
					prefix, transaction.Id, transaction.PackageId, transaction.Name, transaction.Frequency, transaction.Amount,
					transaction.Type, transaction.SourceAccount, transaction.TargetAccount,
					transaction.Duration, transaction.StartLate));
				prefix = ", ";
			}
			sql.Append(";");

			return sql.ToString();
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			return null;
		}
	}

	internal class UpdatePackageSqlQuery : DBServiceBase<Transaction>
	{
		private Package _package;

		internal UpdatePackageSqlQuery(Package package)
		{
			_package = package;
		}

		internal override string SqlQuery()
		{
			StringBuilder sql = new StringBuilder();

			// UpdatePackage
			sql.Append(string.Format(@"
				UPDATE _FM_Packages
				SET Name='{1}', ParentId={2}, Icon='{3}', Options='{4}', Sort={5}
				WHERE (Id = {0});",
					_package.Id, _package.Name, _package.ParentId, _package.Icon, _package.Options, _package.Sort));

			// DeleteTransactions
			sql.Append(string.Format(@"
				DELETE FROM _FM_Transactions
				WHERE (PackageId = {0});",
					_package.Id));

			if (_package.Transactions.Count <= 0)
				return sql.ToString();

			// InsertTransactions(
			sql.Append(string.Format(@"
				INSERT INTO _FM_Transactions
				(Id, PackageId, Name, Frequency, Amount, Type, SourceAccount, TargetAccount, Duration, StartLate)
				VALUES "));
			string prefix = string.Empty;
			foreach (Transaction transaction in _package.Transactions)
			{
				sql.Append(string.Format("{0}({1}, {2}, '{3}', '{4}', {5}, '{6}', '{7}', '{8}', {9}, {10})",
					prefix, transaction.Id, transaction.PackageId, transaction.Name, transaction.Frequency, transaction.Amount,
					transaction.Type, transaction.SourceAccount, transaction.TargetAccount,
					transaction.Duration, transaction.StartLate));
				prefix = ", ";
			}
			sql.Append(";");

			return sql.ToString();

#if NOTUSED //j can't update transactions in place because we would need to track deletes and order
			// UpdateTransactions
			foreach (Transaction transaction in package.Transactions)
			{
				if (transaction.Id != 0)
				{
					// UpdateTransaction
					sql.Append(string.Format(@"
						UPDATE _FM_Transactions
						SET PackageId={1}, Name='{2}', Frequency='{3}', Amount={4},
							Type='{5}', SourceAccount='{6}', TargetAccount='{7}',
							Duration={8}, StartLate={9}
						WHERE (Id = {0});",
							transaction.Id, transaction.PackageId, transaction.Name, transaction.Frequency, transaction.Amount,
							transaction.Type, transaction.SourceAccount, transaction.TargetAccount,
							transaction.Duration, transaction.StartLate));
				}
				else
				{
					// InsertTransaction
					transaction.PackageId = package.Id;
					sql.Append(string.Format(@"
						INSERT INTO _FM_Transactions
						(Id, PackageId, Name, Frequency, Amount, Type, SourceAccount, TargetAccount, Duration, StartLate)
						VALUES ({0}, {1}, '{2}', '{3}', {4}, '{5}', '{6}', '{7}', {8}, {9});",
							transaction.Id, transaction.PackageId, transaction.Name, transaction.Frequency, transaction.Amount,
							transaction.Type, transaction.SourceAccount, transaction.TargetAccount,
							transaction.Duration, transaction.StartLate));
				}
			}
#endif
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			return null;
		}
	}

	internal class DeletePackageSqlQuery : DBServiceBase<Transaction>
	{
		private Package _package;

		internal DeletePackageSqlQuery(Package package)
		{
			_package = package;
		}

		internal override string SqlQuery()
		{
			StringBuilder sql = new StringBuilder();

			// DeletePackage
			sql.Append(string.Format(@"
				DELETE FROM _FM_Packages
				WHERE (Id = {0});",
					_package.Id));

			// DeleteTransactions
			sql.Append(string.Format(@"
				DELETE FROM _FM_Transactions
				WHERE (PackageId = {0});",
					_package.Id));

			// UpdatePackageParentId
			sql.Append(string.Format(@"
				UPDATE _FM_Packages
				SET ParentId={0}
				WHERE (ParentId = {1});",
					_package.ParentId, _package.Id));

			return sql.ToString();
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			return null;
		}
	}

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
				PackageId = item.AttributeValue("PackageId").ToInt64(),
				Name = item.AttributeValue("Name"),
				Frequency = item.AttributeValue("Frequency").ToEnum<Frequency>(),
				Amount = item.AttributeValue("Amount").ToInt64(),
				Type = item.AttributeValue("Type").ToEnum<TransactionType>(),
				TargetAccount = item.AttributeValue("TargetAccount").ToEnum<AccountType>(),
				SourceAccount = item.AttributeValue("SourceAccount").ToEnum<AccountType>(),
				Duration = item.AttributeValue("Duration").ToInt(),
				StartLate = item.AttributeValue("StartLate").ToInt(),
			};

			return query;
		}

		private string PackageIdList(Package package)
		{
			StringBuilder sql = new StringBuilder();
			PackageIdList(sql, package, true/*root*/);
			return sql.ToString();
		}

		private void PackageIdList(StringBuilder sql, Package package, bool root)
		{
			if (!package.TransactionsLoaded)
				sql.Append(string.Format("{0}(PackageId = {1})", (root ? "" : " OR "), package.Id));

			foreach (Package childPackage in package.Packages)
				PackageIdList(sql, childPackage, false/*root*/);
		}
	}

	internal class PackagesSqlQuery : DBServiceBase<Package>
	{
		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT Id, Name, ParentId, Icon, Options, Sort
				FROM _FM_Packages
				ORDER BY Sort;");
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			IEnumerable<Package> query = from item in element.Descendants()
			select new Package
			{
				Id = item.AttributeValue("Id").ToInt64(),
				ParentId = item.AttributeValue("ParentId").ToInt64(),
				Name = item.AttributeValue("Name"),
				Icon = item.AttributeValue("Icon"),
				Options = item.AttributeValue("Options"),
				Sort = item.AttributeValue("Sort").ToDouble(),
			};

			return query;
		}
	}

	internal class IncomeSqlQuery : DBServiceBase<Income>
	{
		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT Id, Name, Description, Frequency, Amount, Type, SourceAccount, TargetAccount, StartDate, EndDate, TaxExempt
				FROM _FM_Income
				ORDER BY Sort;");
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			IEnumerable<Income> query = from item in element.Descendants()
			select new Income
			{
				Id = item.AttributeValue("Id").ToInt64(),
				Name = item.AttributeValue("Name"),
				Description = item.AttributeValue("Description"),
				Frequency = item.AttributeValue("Frequency").ToEnum<Frequency>(),
				Amount = item.AttributeValue("Amount").ToInt64(),
				Type = item.AttributeValue("Type").ToEnum<IncomeType>(),
				TargetAccount = item.AttributeValue("TargetAccount").ToEnum<AccountType>(),
				SourceAccount = item.AttributeValue("SourceAccount").ToEnum<AccountType>(),
				StartDate = item.AttributeValue("StartDate").ToUInt(),
				EndDate = item.AttributeValue("EndDate").ToUInt(),
				TaxExempt = item.AttributeValue("TaxExempt").ToEnum<TaxExemptFlags>(),
			};

			return query;
		}
	}

	internal class LocationSqlQuery : DBServiceBase<Location>
	{
		private string _postalCode;

		internal LocationSqlQuery(string postalCode)
		{
			_postalCode = postalCode;
		}

		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT _FM_zips.City, _FM_states.StateAbbreviation, _FM_zips.Latitude, _FM_zips.Longitude 
				FROM _FM_zips 
				INNER JOIN _FM_states ON _FM_zips.StateCode = _FM_states.StateCode
				WHERE (_FM_zips.ZipCode = '{0}');",
					_postalCode);
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			IEnumerable<Location> query = from item in element.Descendants()
			select new Location
			{
				City = item.AttributeValue("City"),
				StateOrSubCountryCode = item.AttributeValue("StateAbbreviation"),
				Latitude = item.AttributeValue("Latitude"),
				Longitude = item.AttributeValue("Longitude"),
				CountryCode = App.Model.CountryCode,
				CountryName = App.Model.CountryName,
			};

			return query;
		}
	}

	internal class LifeExpectancySqlQuery : DBServiceBase<string>
	{
		private string _gender;
		private double _age;

		internal LifeExpectancySqlQuery(string gender, double age)
		{
			_gender = gender;
			_age = age;
		}

		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT ex 
				FROM _FM_US_life 
				WHERE (Gender = '{0}') AND (Age = {1});",
					_gender, _age);
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			IEnumerable<string> query = from item in element.Descendants()
			select item.AttributeValue("ex");

			return query;
		}
	}

	internal class CountryHolder
	{
		public string CountryCode { get; set; }
		public string CountryName { get; set; }
	}

	internal class CountryCodeSqlQuery : DBServiceBase<CountryHolder>
	{
		private int _ipNum;

		internal CountryCodeSqlQuery(int ipNum)
		{
			_ipNum = ipNum;
		}

		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT CountryCode, CountryName
				FROM _FM_ip2country
				WHERE ({0} BETWEEN IpStartNum AND IpEndNum);",
					_ipNum);
		}

		internal override IEnumerable LinqQuery(XElement element)
		{
			IEnumerable<CountryHolder> query = from item in element.Descendants()
			select new CountryHolder
			{
				CountryCode = item.AttributeValue("CountryCode"),
				CountryName = item.AttributeValue("CountryName"),
			};

			return query;
		}
	}
}
