using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;
using System.IO;
using System.Xml;
using System.Text;

namespace PicaPages
{
	public class WebResources
	{
		public static List<Account> Accounts { get { return m_Accounts; } }
		private static List<Account> m_Accounts;

		public static List<User> Users { get { return m_Users; } }
		private static List<User> m_Users;

		public static List<Layout> Layouts { get { return m_Layouts; } }
		private static List<Layout> m_Layouts;

		public static List<Cover> Covers { get { return m_Covers; } }
		private static List<Cover> m_Covers;

		public static List<Background> Backgrounds { get { return m_Backgrounds; } }
		private static List<Background> m_Backgrounds;

		public static List<Corner> Corners { get { return m_Corners; } }
		private static List<Corner> m_Corners;

		public static List<Photo> Photos { get { return m_Photos; } }
		private static List<Photo> m_Photos;

		/////////////////////////////////////////////////////////////////////////////
		public static void DownloadAccounts()
		{
			DBServiceHelper dbServiceHelper = new DBServiceHelper();
			dbServiceHelper.GetDataSet("SELECT * FROM _FM_accounts;", DownloadAccountsCompleted);
		}

		/////////////////////////////////////////////////////////////////////////////
		public static void DownloadUsers()
		{
			DBServiceHelper dbServiceHelper = new DBServiceHelper();
			dbServiceHelper.GetDataSet("SELECT * FROM _FM_users;", DownloadUsersCompleted);
		}

		/////////////////////////////////////////////////////////////////////////////
		public static void DownloadLayouts()
		{
			DBServiceHelper dbServiceHelper = new DBServiceHelper();
			dbServiceHelper.GetDataSet("SELECT * FROM _PP_Layouts;", DownloadLayoutsCompleted);
		}

		/////////////////////////////////////////////////////////////////////////////
		public static void DownloadCovers()
		{
			DBServiceHelper dbServiceHelper = new DBServiceHelper();
			dbServiceHelper.GetDataSet("SELECT * FROM _PP_Covers;", DownloadCoversCompleted);
		}

		/////////////////////////////////////////////////////////////////////////////
		public static void DownloadBackgrounds()
		{
			DBServiceHelper dbServiceHelper = new DBServiceHelper();
			dbServiceHelper.GetDataSet("SELECT * FROM _PP_Backgrounds;", DownloadBackgroundsCompleted);
		}

		/////////////////////////////////////////////////////////////////////////////
		public static void DownloadCorners()
		{
			DBServiceHelper dbServiceHelper = new DBServiceHelper();
			dbServiceHelper.GetDataSet("SELECT * FROM _PP_Corners;", DownloadCornersCompleted);
		}

		/////////////////////////////////////////////////////////////////////////////
		public static void DownloadPhotos()
		{
			DBServiceHelper dbServiceHelper = new DBServiceHelper();
			dbServiceHelper.GetDataSet("SELECT * FROM _PP_Photos;", DownloadPhotosCompleted);
		}

		/////////////////////////////////////////////////////////////////////////////
		public static void DownloadLayoutPayload(int PayloadId, EventHandler requestCompletedHandler)
		{
			DBServiceHelper dbServiceHelper = new DBServiceHelper();
			string query = string.Format("SELECT * FROM _FM_packagePayloads WHERE PayloadId = {0}", PayloadId);
			dbServiceHelper.GetDataSet(query, requestCompletedHandler);
		}

		/////////////////////////////////////////////////////////////////////////////
		private static void DownloadAccountsCompleted(object sender, EventArgs args)
		{
			if (sender is string)
			{
				string error = sender as string;
				MessageBoxEx.Show("Error", error, null);
				return;
			}

			if (!(sender is XDocument))
				return;

			XDocument xmlDoc = sender as XDocument; // xmlDoc.Root.Name should be "Root" or null
			if (xmlDoc.Root == null) // An valid empty result
				return;
			var query = from item in xmlDoc.Root.Descendants()
			select new Account
			{
				Name = item.Attribute("Name").Value,
				AccountType = item.Attribute("AccountType").Value.ToEnum<EAccountType>(),
			};

			m_Accounts = query.ToList<Account>();
			//string output = "";
			//foreach (Account account in m_Accounts)
			//    output += account.Name + " " + account.AccountType + " " + account.AllowNegative + "\r\n";
			//MessageBox.Show("Alert", output, null);
		}

		/////////////////////////////////////////////////////////////////////////////
		private static void DownloadUsersCompleted(object sender, EventArgs args)
		{
			if (sender is string)
			{
				string error = sender as string;
				MessageBoxEx.Show("Error", error, null);
				return;
			}

			if (!(sender is XDocument))
				return;

			XDocument xmlDoc = sender as XDocument; // xmlDoc.Root.Name should be "Root" or null
			if (xmlDoc.Root == null) // An valid empty result
				return;
			var query = from item in xmlDoc.Root.Descendants()
			where item.Attribute("EmailVerified").Value == "0"
			orderby item.Attribute("CustomerID").Value ascending
			select new User
			{
				CustomerID = item.Attribute("CustomerID").Value.ToInt(),
				EmailAddress = item.Attribute("EmailAddress").Value,
				EmailVerified = item.Attribute("EmailVerified").Value.ToBool(),
				Password = item.Attribute("Password").Value,
			};

			m_Users = query.ToList<User>();
			//string output = "";
			//foreach (User user in m_Users)
			//    output += user.CustomerID + " " + user.EmailAddress + " " + user.EmailVerified + " " + user.Password + "\r\n";
			//MessageBox.Show("Alert", output, null);
		}

		/////////////////////////////////////////////////////////////////////////////
		private static void DownloadLayoutsCompleted(object sender, EventArgs args)
		{
			if (sender is string)
			{
				string error = sender as string;
				MessageBoxEx.Show("Error", error, null);
				return;
			}

			if (!(sender is XDocument))
				return;

			XDocument xmlDoc = sender as XDocument; // xmlDoc.Root.Name should be "Root" or null
			if (xmlDoc.Root == null) // An valid empty result
				return;
			var query = from item in xmlDoc.Root.Descendants()
			select new Layout
			{
				CategoryPath = item.Attribute("CategoryPath").Value,
				Name = item.Attribute("Name").Value,
				Icon = item.Attribute("Icon").Value,
				PayloadId = item.Attribute("PayloadId").Value.ToInt(),
			};

			m_Layouts = query.ToList<Layout>();
			//string output = "";
			//foreach (Layout layout in m_Layouts)
			//    output += layout.CategoryPath + " " + layout.Name + " " + layout.Icon + " " + layout.PayloadId + "\r\n";
			//MessageBox.Show("Alert", output, null);
		}

		/////////////////////////////////////////////////////////////////////////////
		private static void DownloadCoversCompleted(object sender, EventArgs args)
		{
			if (sender is string)
			{
				string error = sender as string;
				MessageBoxEx.Show("Error", error, null);
				return;
			}

			if (!(sender is XDocument))
				return;

			XDocument xmlDoc = sender as XDocument; // xmlDoc.Root.Name should be "Root" or null
			if (xmlDoc.Root == null) // An valid empty result
				return;
			var query = from item in xmlDoc.Root.Descendants()
			select new Cover
			{
				CategoryPath = item.Attribute("CategoryPath").Value,
				Name = item.Attribute("Name").Value,
				Icon = item.Attribute("Icon").Value,
				PayloadId = item.Attribute("PayloadId").Value.ToInt(),
			};

			m_Covers = query.ToList<Cover>();
			//string output = "";
			//foreach (Cover cover in m_Covers)
			//    output += cover.CategoryPath + " " + cover.Name + " " + cover.Icon + " " + cover.PayloadId + "\r\n";
			//MessageBox.Show("Alert", output, null);
		}

		/////////////////////////////////////////////////////////////////////////////
		private static void DownloadBackgroundsCompleted(object sender, EventArgs args)
		{
			if (sender is string)
			{
				string error = sender as string;
				MessageBoxEx.Show("Error", error, null);
				return;
			}

			if (!(sender is XDocument))
				return;

			XDocument xmlDoc = sender as XDocument; // xmlDoc.Root.Name should be "Root" or null
			if (xmlDoc.Root == null) // An valid empty result
				return;
			var query = from item in xmlDoc.Root.Descendants()
			select new Background
			{
				CategoryPath = item.Attribute("CategoryPath").Value,
				Name = item.Attribute("Name").Value,
				Icon = item.Attribute("Icon").Value,
				PayloadId = item.Attribute("PayloadId").Value.ToInt(),
			};

			m_Backgrounds = query.ToList<Background>();
			//string output = "";
			//foreach (Background background in m_Backgrounds)
			//    output += background.CategoryPath + " " + background.Name + " " + background.Icon + " " + background.PayloadId + "\r\n";
			//MessageBox.Show("Alert", output, null);
		}

		/////////////////////////////////////////////////////////////////////////////
		private static void DownloadCornersCompleted(object sender, EventArgs args)
		{
			if (sender is string)
			{
				string error = sender as string;
				MessageBoxEx.Show("Error", error, null);
				return;
			}

			if (!(sender is XDocument))
				return;

			XDocument xmlDoc = sender as XDocument; // xmlDoc.Root.Name should be "Root" or null
			if (xmlDoc.Root == null) // An valid empty result
				return;
			var query = from item in xmlDoc.Root.Descendants()
			select new Corner
			{
				CategoryPath = item.Attribute("CategoryPath").Value,
				Name = item.Attribute("Name").Value,
				Icon = item.Attribute("Icon").Value,
				PayloadId = item.Attribute("PayloadId").Value.ToInt(),
			};

			m_Corners = query.ToList<Corner>();
			//string output = "";
			//foreach (Corner corner in m_Corners)
			//    output += corner.CategoryPath + " " + corner.Name + " " + corner.Icon + " " + corner.PayloadId + "\r\n";
			//MessageBox.Show("Alert", output, null);
		}

		/////////////////////////////////////////////////////////////////////////////
		private static void DownloadPhotosCompleted(object sender, EventArgs args)
		{
			if (sender is string)
			{
				string error = sender as string;
				MessageBoxEx.Show("Error", error, null);
				return;
			}

			if (!(sender is XDocument))
				return;

			XDocument xmlDoc = sender as XDocument; // xmlDoc.Root.Name should be "Root" or null
			if (xmlDoc.Root == null) // An valid empty result
				return;
			var query = from item in xmlDoc.Root.Descendants()
			select new Photo
			{
				CategoryPath = item.Attribute("CategoryPath").Value,
				Name = item.Attribute("Name").Value,
				Icon = item.Attribute("Icon").Value,
				PayloadId = item.Attribute("PayloadId").Value.ToInt(),
			};

			m_Photos = query.ToList<Photo>();
			//string output = "";
			//foreach (Photo photo in m_Photos)
			//    output += photo.CategoryPath + " " + photo.Name + " " + photo.Icon + " " + photo.PayloadId + "\r\n";
			//MessageBox.Show("Alert", output, null);
		}
	}
}
