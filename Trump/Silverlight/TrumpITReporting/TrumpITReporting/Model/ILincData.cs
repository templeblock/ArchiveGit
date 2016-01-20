using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Xml;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class ILincDataCollection : ObservableCollection<ILincData>, IDisposable
	{
		public void Dispose()
		{
			foreach (ILincData item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
			base.Clear();
		}

		internal void GetSessionFromUserID(string userID)
		{
			PageMethodInvoker pageMethod = new PageMethodInvoker();
			pageMethod.Completed += OnGetSessionCompleted;
			pageMethod.Invoke("ILincGetSessionFromUserID", userID, null);
		}

		internal void GetSessionFromActivityID(string activityID)
		{
			PageMethodInvoker pageMethod = new PageMethodInvoker();
			pageMethod.Completed += OnGetSessionCompleted;
			pageMethod.Invoke("ILincGetSessionFromActivityID", activityID, null);
		}

		private void OnGetSessionCompleted(object methodName, bool success, object result, object userContext)
		{
			string xmlResult = result as string;
			if (xmlResult == null)
				return;

			StringReader stringReader = new StringReader(xmlResult);
			XDocument xmlDoc = XDocument.Load(XmlReader.Create(stringReader));
			IEnumerable<ILincData> query = from item in xmlDoc.Root.Descendants("Session")
			select new ILincData
			{
				ActivityID = item.Element("ActivityID").Value.ToInt(),
				UserID = item.Element("UserID").Value.ToInt(),
				JoinTime = item.Element("JoinTime").Value.ToDateTime(),
				LeaveTime = item.Element("LeaveTime").Value.ToDateTime(),
				Duration = item.Element("Duration").Value.ToInt(),
			};

			base.Clear();
			foreach (ILincData item in query)
				base.Add(item);
		}
	}

	public class ILincData : NotifyPropertyChanges
	{
		public int UserID { get { return m_UserID; } set { SetProperty(ref m_UserID, value); } }
		private int m_UserID;

		public int ActivityID { get { return m_ActivityID; } set { SetProperty(ref m_ActivityID, value); } }
		private int m_ActivityID;

		public DateTime JoinTime { get { return m_JoinTime; } set { SetProperty(ref m_JoinTime, value); } }
		private DateTime m_JoinTime;

		public DateTime LeaveTime { get { return m_LeaveTime; } set { SetProperty(ref m_LeaveTime, value); } }
		private DateTime m_LeaveTime;

		public int Duration { get { return m_Duration; } set { SetProperty(ref m_Duration, value); } }
		private int m_Duration;
		
		public override void Dispose()
		{
			base.Dispose();
		}
	}
}
