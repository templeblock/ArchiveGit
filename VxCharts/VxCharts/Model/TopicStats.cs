using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;
using System.Collections.Generic;


namespace VxCharts
{
	public class TopicStatsCollection : ObservableCollection<TopicStats>, IDisposable
	{
		public void Dispose()
		{
			foreach (TopicStats item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class TopicStats : ViewModelBase
	{
		[DataMember]
		public string TopicName { get; set; }
		[DataMember]
		public string TopicId { get; set; }
		[DataMember]
		public bool isTopic { get; set; }
		[DataMember]
		public double PositiveCount { get; set; }
		[DataMember]
		public double NegativeCount { get; set; }
		[DataMember]
		public double NeutralCount { get; set; }
		[DataMember]
		public double MixedCount { get; set; }
		[DataMember]
		public double SentimentPercentage { get; set; }
		[DataMember]
		public double PostsVolume { get; set; }
		[DataMember]
		public double ConversationPercentage { get; set; }
		[DataMember]
		public double ScoredVolumePercentage { get; set; }
		[DataMember]
		public string PercentTotalText { get; set; }
		[DataMember]
		public string Sentiment { get; set; }
		[DataMember]
		public string SentimentScoreText { get; set; }
		[DataMember]
		public double DisplaySize { get; set; }
		[DataMember]
		public double ScoredVolume { get; set; }
		[DataMember]
		public List<NodesStat> NodesStats { get; set; }
		[DataMember]
		public string Icon { get; set; }
		[DataMember]
		public ObservableCollection<TopicSentimentValues> SentimentScoreObserv { get; set; }
		[DataMember]
		public double X { get; set; }
		[DataMember]
		public double Y { get; set; }
	}
}
