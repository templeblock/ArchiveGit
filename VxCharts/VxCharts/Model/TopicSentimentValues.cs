using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;

namespace VxCharts
{
	public class TopicSentimentValuesCollection : ObservableCollection<TopicSentimentValues>, IDisposable
	{
		public void Dispose()
		{
			foreach (TopicSentimentValues item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class TopicSentimentValues : ViewModelBase
	{
	}
}
