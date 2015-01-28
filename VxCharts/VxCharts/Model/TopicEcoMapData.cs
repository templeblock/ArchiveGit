using System.Collections.Generic;
using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;

namespace VxCharts
{
	public class TopicEcoMapDataCollection : ObservableCollection<TopicEcoMapData>, IDisposable
	{
		public void Dispose()
		{
			foreach (TopicEcoMapData item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class TopicEcoMapData : ViewModelBase
	{
		[DataMember]
		public List<TopicStats> Nodes { get; set; }
		[DataMember]
		public List<NodesStat> Edges { get; set; }
		[DataMember]
		public int TotalPostCount { get; set; }
		[DataMember]
		public string Language { get; set; }
		[DataMember]
		public string DateRange { get; set; }
		[DataMember]
		public string Domain { get; set; }
		[DataMember]
		public string ProjectId { get; set; }
		[DataMember]
		public int TotalThreadCount { get; set; }
		[DataMember]
		public double TotalSentimentCount { get; set; }
		[DataMember]
		public List<ImageData> NodeImages { get; set; }

	}
}
