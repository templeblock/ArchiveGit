using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;

namespace VxCharts
{
	public class PointEngagementTopicLanguageSentimentInfoCollection : ObservableCollection<PointEngagementTopicLanguageSentimentInfo>, IDisposable
	{
		public void Dispose()
		{
			foreach (PointEngagementTopicLanguageSentimentInfo item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class PointEngagementTopicLanguageSentimentInfo : ViewModelBase
	{
		[DataMember]
		public string Topic { get; set; }
		[DataMember]
		public string SentimentType { get; set; } //positive/negative/neutral/mixed
		[DataMember]
		public int SentimentCount { get; set; }
		[DataMember]
		public string Language { get; set; }
	}
}
