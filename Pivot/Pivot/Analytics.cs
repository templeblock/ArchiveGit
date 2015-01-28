using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using Microsoft.WebAnalytics;

namespace Pivot
{
	public class Analytics
	{
		public enum Category
		{
			AppEvents,
		}

		public enum ActionValue
		{
			SignIn,
			SignOut,
			Navigate,
		}

		[Import("Log")]
		public Action<AnalyticsEvent> Log { get; set; }

		//private DataCollector _collector = new DataCollector();
		//private Dictionary<string, string> _dictionary = new Dictionary<string, string>();
		//private bool _init = false;

		public Analytics()
		{
			CompositionInitializer.SatisfyImports(this);
		}

		public void TrackEvent(ActionValue actionValue, string name, int value = 0)
		{
			//if (!_init)
			//{
			//    _collector.Initialize(_dictionary);
			//    _init = true;
			//}

			var analyticsEvent = new AnalyticsEvent()
			{
				Category = Category.AppEvents.ToString(),
				ActionValue = actionValue.ToString(),
				Name = name,
				Value = value,
				ObjectType = GetType().Name
			};

			this.Log(analyticsEvent);
			//_collector.Log(analyticsEvent);

			// AppName
			//ClipContext Clip
			// ContentId
			//Uri DocumentUri
			//Collection<PropertyValue> EventArgs
			//HitType HitType
			//InstallState InstallState
			//bool IsFullScreen
			// Language
			//Uri MediaSource
			//MediaStreamType? MediaStreamType
			//TimeSpan MediaTimeCode
			// NavigationState
			// ObjectName
			// ObjectType
			// Referrer
			//Size Resolution
			// SilverlightVersion
			// StateName
			//TimeSpan TimeCode
			//DateTimeOffset Timestamp
			//TrackInfo Track
			// UserAgent
			// Version
			//double ZoomFactor
		}
	}
}
