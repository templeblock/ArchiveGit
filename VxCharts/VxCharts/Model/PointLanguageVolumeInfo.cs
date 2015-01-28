using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;

namespace VxCharts
{
	public class PointLanguageVolumeInfoCollection : ObservableCollection<PointLanguageVolumeInfo>, IDisposable
	{
		public void Dispose()
		{
			foreach (PointLanguageVolumeInfo item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class PointLanguageVolumeInfo : ViewModelBase
	{
		[DataMember]
		public string VolumeType { get; set; } //Site/Post/Author
		[DataMember]
		public int VolumeCount { get; set; }
		[DataMember]
		public string Language { get; set; }
		[DataMember]
		public string Date { get; set; }
	}
}
