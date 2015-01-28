using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;

namespace VxCharts
{
	public class PointLanguageSentimentInfoCollection : ObservableCollection<PointLanguageSentimentInfo>, IDisposable
	{
		public void Dispose()
		{
			foreach (PointLanguageSentimentInfo item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class PointLanguageSentimentInfo : ViewModelBase
	{
		[DataMember]
		public string SentimentType { get; set; } // Contains  Positive,Negative,Neutral,Mixed, or (Total – may need filter)
		[DataMember]
		public int SentimentCount { get; set; }
		[DataMember]
		public string Language { get; set; }
		[DataMember]
		public string Date { get; set; }
	}
}
