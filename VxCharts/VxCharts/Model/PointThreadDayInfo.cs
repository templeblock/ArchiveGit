using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;
namespace VxCharts
{
	public class PointThreadDayInfoCollection : ObservableCollection<PointThreadDayInfo>, IDisposable
	{
		public void Dispose()
		{
			foreach (PointThreadDayInfo item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class PointThreadDayInfo : ViewModelBase
	{
		[DataMember]
		public int Count { get; set; }
		[DataMember]
		public string Date { get; set; }
	}
}
