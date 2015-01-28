using System;

namespace StockVisualization
{
	public delegate void StockDataEventHandler<T>(object sender, EventWithDataArgs<T> e);
}
