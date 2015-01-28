using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;

namespace VxCharts
{
	public class PointTopicSentimentInfoCollection : ObservableCollection<PointTopicSentimentInfo>, IDisposable
	{
		public void Dispose()
		{
			foreach (PointTopicSentimentInfo item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class PointTopicSentimentInfo : ViewModelBase
	{
		[DataMember]
		public int TotalCount { get; set; } //total Scored Items count 
		[DataMember]
		public double SentimentScore { get; set; } //% of the sentimented values (determined the y position)
		[DataMember]
		public string TopicName { get; set; }
		[DataMember]
		public string Language { get; set; }
	}
}
