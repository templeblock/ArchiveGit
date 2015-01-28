using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;

namespace VxCharts
{
	public class NodesStatCollection : ObservableCollection<NodesStat>, IDisposable
	{
		public void Dispose()
		{
			foreach (NodesStat item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class NodesStat : ViewModelBase
	{
		[DataMember]
		public string StartTopicId { get; set; }
		[DataMember]
		public string StartTopicName { get; set; }
		[DataMember]
		public string EndTopicId { get; set; }
		[DataMember]
		public string EndTopicName { get; set; }
		[DataMember]
		public int ConversationCount { get; set; }
		[DataMember]
		public double X1 { get; set; }
		[DataMember]
		public double Y1 { get; set; }
		[DataMember]
		public double X2 { get; set; }
		[DataMember]
		public double Y2 { get; set; }
	}
}
