using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;

namespace VxCharts
{
	public class PointEngagementTopicLanguageInfoCollection : ObservableCollection<PointEngagementTopicLanguageInfo>, IDisposable
	{
		public void Dispose()
		{
			foreach (PointEngagementTopicLanguageInfo item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class PointEngagementTopicLanguageInfo : ViewModelBase
	{
		[DataMember]
		public string Topic { get; set; }
		[DataMember]
		public int Count { get; set; }
		[DataMember]
		public string Language { get; set; }
	}
}
