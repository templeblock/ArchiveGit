using System;

namespace StockVisualization
{
	public class EventWithDataArgs<T> : EventArgs
	{
		public T Data { get; internal set; }
		public EventWithDataArgs(T data)
		{
			this.Data = data;
		}
	}
}
