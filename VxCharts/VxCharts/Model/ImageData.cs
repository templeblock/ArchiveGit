using System;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using ClassLibrary;

namespace VxCharts
{
	public class ImageDataCollection : ObservableCollection<ImageData>, IDisposable
	{
		public void Dispose()
		{
			foreach (ImageData item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	[DataContract]
	public class ImageData : ViewModelBase
	{
		[DataMember]
		public string ID { get; set; }
		public string bitmapData { get; set; }
	}
}
