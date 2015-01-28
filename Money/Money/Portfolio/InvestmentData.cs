using System;

namespace StockVisualization
{
	public class InvestmentData
	{
		public string Symbol { get; set; }
		public DateTime TimeStamp { get; set; }
		public double Price { get; set; }
		public double Change { get; set; }
	}
}
