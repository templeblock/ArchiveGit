using System.Collections.Generic;
using System.Linq;
using System;

namespace VxCharts
{
	public class SampleData
	{
		//
		public IEnumerable<PointLanguageSentimentInfo> PointLanguageSentimentInfoNeutral { get { return _PointLanguageSentimentInfo.Where(item => item.SentimentType == "Neutral"); } }
		public IEnumerable<PointLanguageSentimentInfo> PointLanguageSentimentInfoMixed { get { return _PointLanguageSentimentInfo.Where(item => item.SentimentType == "Mixed"); } }
		public IEnumerable<PointLanguageSentimentInfo> PointLanguageSentimentInfoNegative { get { return _PointLanguageSentimentInfo.Where(item => item.SentimentType == "Negative"); } }
		public IEnumerable<PointLanguageSentimentInfo> PointLanguageSentimentInfoPositive { get { return _PointLanguageSentimentInfo.Where(item => item.SentimentType == "Positive"); } }

		private IEnumerable<PointLanguageSentimentInfo> _PointLanguageSentimentInfo = BuildPointLanguageSentimentInfo();

		public static IEnumerable<PointLanguageSentimentInfo> BuildPointLanguageSentimentInfo()
		{
			IEnumerable<PointLanguageSentimentInfo> collection = new PointLanguageSentimentInfoCollection()
			{
				new PointLanguageSentimentInfo() { Language = "Arabic", SentimentType="Positive", SentimentCount = 30, },
				new PointLanguageSentimentInfo() { Language = "Arabic", SentimentType="Negative", SentimentCount = 50, },
				new PointLanguageSentimentInfo() { Language = "Arabic", SentimentType="Neutral", SentimentCount = 100, },
				new PointLanguageSentimentInfo() { Language = "Arabic", SentimentType="Mixed", SentimentCount = 20, },

				new PointLanguageSentimentInfo() { Language = "Farsi", SentimentType="Positive", SentimentCount = 33, },
				new PointLanguageSentimentInfo() { Language = "Farsi", SentimentType="Negative", SentimentCount = 55, },
				new PointLanguageSentimentInfo() { Language = "Farsi", SentimentType="Neutral", SentimentCount = 110, },
				new PointLanguageSentimentInfo() { Language = "Farsi", SentimentType="Mixed", SentimentCount = 22, },
	
				new PointLanguageSentimentInfo() { Language = "Pashto", SentimentType="Positive", SentimentCount = 18, },
				new PointLanguageSentimentInfo() { Language = "Pashto", SentimentType="Negative", SentimentCount = 30, },
				new PointLanguageSentimentInfo() { Language = "Pashto", SentimentType="Neutral", SentimentCount = 60, },
				new PointLanguageSentimentInfo() { Language = "Pashto", SentimentType="Mixed", SentimentCount = 12, },
				
				new PointLanguageSentimentInfo() { Language = "Urdu", SentimentType="Positive", SentimentCount = 21, },
				new PointLanguageSentimentInfo() { Language = "Urdu", SentimentType="Negative", SentimentCount = 35, },
				new PointLanguageSentimentInfo() { Language = "Urdu", SentimentType="Neutral", SentimentCount = 70, },
				new PointLanguageSentimentInfo() { Language = "Urdu", SentimentType="Mixed", SentimentCount = 14, },
				
				new PointLanguageSentimentInfo() { Language = "Russian", SentimentType="Positive", SentimentCount = 27, },
				new PointLanguageSentimentInfo() { Language = "Russian", SentimentType="Negative", SentimentCount = 45, },
				new PointLanguageSentimentInfo() { Language = "Russian", SentimentType="Neutral", SentimentCount = 90, },
				new PointLanguageSentimentInfo() { Language = "Russian", SentimentType="Mixed", SentimentCount = 18, },
			};
			return collection;
		}

		//
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoPostsArabic { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Posts" && item.Language == "Arabic"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoAuthorsArabic { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Author" && item.Language == "Arabic"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoSitesArabic { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Site" && item.Language == "Arabic"); } }

		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoPostsFarsi { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Posts" && item.Language == "Farsi"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoAuthorsFarsi { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Author" && item.Language == "Farsi"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoSitesFarsi { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Site" && item.Language == "Farsi"); } }

		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoPostsPashto { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Posts" && item.Language == "Pashto"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoAuthorsPashto { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Author" && item.Language == "Pashto"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoSitesPashto { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Site" && item.Language == "Pashto"); } }

		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoPostsUrdu { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Posts" && item.Language == "Urdu"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoAuthorsUrdu { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Author" && item.Language == "Urdu"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoSitesUrdu { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Site" && item.Language == "Urdu"); } }

		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoPostsRussian { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Posts" && item.Language == "Russian"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoAuthorsRussian { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Author" && item.Language == "Russian"); } }
		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfoSitesRussian { get { return PointLanguageVolumeInfo.Where(item => item.VolumeType == "Site" && item.Language == "Russian"); } }

		public IEnumerable<PointLanguageVolumeInfo> PointLanguageVolumeInfo { get { return _PointLanguageVolumeInfo; } }

		private IEnumerable<PointLanguageVolumeInfo> _PointLanguageVolumeInfo = BuildPointLanguageVolumeInfo();

		public static IEnumerable<PointLanguageVolumeInfo> BuildPointLanguageVolumeInfo()
		{
			IEnumerable<PointLanguageVolumeInfo> collection = new PointLanguageVolumeInfoCollection()
			{
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/1", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/2", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/3", VolumeCount = 5 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/4", VolumeCount = 7 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/5", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/6", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/7", VolumeCount = 3 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/8", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/9", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/10", VolumeCount = 2 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/11", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/12", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/13", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/14", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/15", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/16", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/17", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/18", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/19", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/20", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/21", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/22", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/23", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/24", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/25", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/26", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/27", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/28", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/29", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "9/30", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Arabic",  Date = "10/1", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/1", VolumeCount = 16 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/2", VolumeCount = 12 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/3", VolumeCount = 26 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/4", VolumeCount = 14 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/5", VolumeCount = 4 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/6", VolumeCount = 5 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/7", VolumeCount = 11 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/8", VolumeCount = 15 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/9", VolumeCount = 19 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/10", VolumeCount = 6 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/11", VolumeCount = 3 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/12", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/13", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/14", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/15", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/16", VolumeCount = 4 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/17", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/18", VolumeCount = 8 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/19", VolumeCount = 10 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/20", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/21", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/22", VolumeCount = 5 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/23", VolumeCount = 7 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/24", VolumeCount = 8 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/25", VolumeCount = 3 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/26", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/27", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/28", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/29", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "9/30", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Author", Language = "Urdu",  Date = "10/1", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/1", VolumeCount = 2 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/2", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/3", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/4", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/5", VolumeCount = 2 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/6", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/7", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/8", VolumeCount = 3 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/9", VolumeCount = 11 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/10", VolumeCount = 8 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/11", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/12", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/13", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/14", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/15", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/16", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/17", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/18", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/19", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/20", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/21", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/22", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/23", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/24", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/25", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/26", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/27", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/28", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/29", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "9/30", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Arabic",  Date = "10/1", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/1", VolumeCount = 5 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/2", VolumeCount = 4 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/3", VolumeCount = 17 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/4", VolumeCount = 2 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/5", VolumeCount = 2 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/6", VolumeCount = 3 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/7", VolumeCount = 12 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/8", VolumeCount = 7 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/9", VolumeCount = 15 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/10", VolumeCount = 7 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/11", VolumeCount = 2 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/12", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/13", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/14", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/15", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/16", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/17", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/18", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/19", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/20", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/21", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/22", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/23", VolumeCount = 5 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/24", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/25", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/26", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/27", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/28", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/29", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "9/30", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Posts", Language = "Urdu",  Date = "10/1", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/1", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/2", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/3", VolumeCount = 10 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/4", VolumeCount = 2 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/5", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/6", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/7", VolumeCount = 12 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/8", VolumeCount = 28 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/9", VolumeCount = 23 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/10", VolumeCount = 59 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/11", VolumeCount = 6 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/12", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/13", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/14", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/15", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/16", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/17", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/18", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/19", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/20", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/21", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/22", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/23", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/24", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/25", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/26", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/27", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/28", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/29", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "9/30", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Arabic",  Date = "10/1", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/1", VolumeCount = 9 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/2", VolumeCount = 2 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/3", VolumeCount = 35 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/4", VolumeCount = 7 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/5", VolumeCount = 8 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/6", VolumeCount = 4 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/7", VolumeCount = 2 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/8", VolumeCount = 12 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/9", VolumeCount = 14 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/10", VolumeCount = 5 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/11", VolumeCount = 4 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/12", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/13", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/14", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/15", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/16", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/17", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/18", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/19", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/20", VolumeCount = 4 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/21", VolumeCount = 1 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/22", VolumeCount = 10 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/23", VolumeCount = 21 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/24", VolumeCount = 32 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/25", VolumeCount = 13 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/26", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/27", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/28", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/29", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "9/30", VolumeCount = 0 },
				new PointLanguageVolumeInfo() { VolumeType = "Site", Language = "Urdu",  Date = "10/1", VolumeCount = 0 },
			};
			return collection;
		}

		//
		public IEnumerable<PointTopicSentimentInfo> PointTopicSentimentInfo1 { get { return _PointTopicSentimentInfo.Where(item => item.TopicName == "A/A Military Operations"); } }
		public IEnumerable<PointTopicSentimentInfo> PointTopicSentimentInfo2 { get { return _PointTopicSentimentInfo.Where(item => item.TopicName == "A/A Non-Military Operations"); } }
		public IEnumerable<PointTopicSentimentInfo> PointTopicSentimentInfo3 { get { return _PointTopicSentimentInfo.Where(item => item.TopicName == "Ideology"); } }
		public IEnumerable<PointTopicSentimentInfo> PointTopicSentimentInfo4 { get { return _PointTopicSentimentInfo.Where(item => item.TopicName == "Political Beliefs and Activities"); } }
		public IEnumerable<PointTopicSentimentInfo> PointTopicSentimentInfo5 { get { return _PointTopicSentimentInfo.Where(item => item.TopicName == "Terrorist Operations"); } }

		public IEnumerable<PointTopicSentimentInfo> PointTopicSentimentInfo { get { return _PointTopicSentimentInfo; } }

		private IEnumerable<PointTopicSentimentInfo> _PointTopicSentimentInfo = BuildPointTopicSentimentInfo();

		public static IEnumerable<PointTopicSentimentInfo> BuildPointTopicSentimentInfo()
		{
			IEnumerable<PointTopicSentimentInfo> collection = new PointTopicSentimentInfoCollection()
			{
				new PointTopicSentimentInfo() { TopicName = "A/A Military Operations", Language = "Urdu", TotalCount = 100,SentimentScore = 81.25, },
				new PointTopicSentimentInfo() { TopicName = "A/A Non-Military Operations", Language = "Urdu", TotalCount = 28,SentimentScore = 50, },
				new PointTopicSentimentInfo() { TopicName = "Ideology", Language = "Urdu", TotalCount = 210,SentimentScore = 56.5, },
				new PointTopicSentimentInfo() { TopicName = "Political Beliefs and Activities", Language = "Urdu", TotalCount = 47,SentimentScore = 37.14, },
				new PointTopicSentimentInfo() { TopicName = "Terrorist Operations", Language = "Urdu", TotalCount = 151,SentimentScore = 24.71, },
			};
			return collection;
		}
	}
}

#if false
		public IEnumerable<ImageData> ImageData { get { return _ImageData; } }
		private IEnumerable<ImageData> _ImageData = BuildImageData();

		public IEnumerable<NodesStat> NodesStat { get { return _NodesStat; } }
		private IEnumerable<NodesStat> _NodesStat = BuildNodesStat();

		public IEnumerable<PointEngagementTopicLanguageInfo> PointEngagementTopicLanguageInfo { get { return _PointEngagementTopicLanguageInfo; } }
		private IEnumerable<PointEngagementTopicLanguageInfo> _PointEngagementTopicLanguageInfo = BuildPointEngagementTopicLanguageInfo();

		public IEnumerable<PointEngagementTopicLanguageSentimentInfo> PointEngagementTopicLanguageSentimentInfo { get { return _PointEngagementTopicLanguageSentimentInfo; } }
		private IEnumerable<PointEngagementTopicLanguageSentimentInfo> _PointEngagementTopicLanguageSentimentInfo = BuildPointEngagementTopicLanguageSentimentInfo();

		public IEnumerable<PointThreadDayInfo> PointThreadDayInfo { get { return _PointThreadDayInfo; } }
		private IEnumerable<PointThreadDayInfo> _PointThreadDayInfo = BuildPointThreadDayInfo();

		public IEnumerable<TopicEcoMapData> TopicEcoMapData { get { return _TopicEcoMapData; } }
		private IEnumerable<TopicEcoMapData> _TopicEcoMapData = BuildTopicEcoMapData();

		public IEnumerable<TopicSentimentValues> TopicSentimentValues { get { return _TopicSentimentValues; } }
		private IEnumerable<TopicSentimentValues> _TopicSentimentValues = BuildTopicSentimentValues();

		public IEnumerable<TopicStats> TopicStats { get { return _TopicStats; } }
		private IEnumerable<TopicStats> _TopicStats = BuildTopicStats();

		public static IEnumerable<ImageData> BuildImageData()
		{
			IEnumerable<ImageData> collection = new ImageDataCollection()
			{
				new ImageData() {  },
				new ImageData() {  },
			};
			return collection;
		}

		public static IEnumerable<NodesStat> BuildNodesStat()
		{
			IEnumerable<NodesStat> collection = new NodesStatCollection()
			{
				new NodesStat() {  },
				new NodesStat() {  },
				new NodesStat() {  },
				new NodesStat() {  },
			};
			return collection;
		}

		public static IEnumerable<PointEngagementTopicLanguageInfo> BuildPointEngagementTopicLanguageInfo()
		{
			IEnumerable<PointEngagementTopicLanguageInfo> collection = new PointEngagementTopicLanguageInfoCollection()
			{
				new PointEngagementTopicLanguageInfo() {  },
				new PointEngagementTopicLanguageInfo() {  },
			};
			return collection;
		}

		public static IEnumerable<PointEngagementTopicLanguageSentimentInfo> BuildPointEngagementTopicLanguageSentimentInfo()
		{
			IEnumerable<PointEngagementTopicLanguageSentimentInfo> collection = new PointEngagementTopicLanguageSentimentInfoCollection()
			{
				new PointEngagementTopicLanguageSentimentInfo() {  },
				new PointEngagementTopicLanguageSentimentInfo() {  },
			};
			return collection;
		}

		public static IEnumerable<PointThreadDayInfo> BuildPointThreadDayInfo()
		{
			IEnumerable<PointThreadDayInfo> collection = new PointThreadDayInfoCollection()
			{
				new PointThreadDayInfo() {  },
				new PointThreadDayInfo() {  },
			};
			return collection;
		}

		public static IEnumerable<TopicEcoMapData> BuildTopicEcoMapData()
		{
			IEnumerable<TopicEcoMapData> collection = new TopicEcoMapDataCollection()
			{
				new TopicEcoMapData() {  },
				new TopicEcoMapData() {  },
			};
			return collection;
		}

		public static IEnumerable<TopicSentimentValues> BuildTopicSentimentValues()
		{
			IEnumerable<TopicSentimentValues> collection = new TopicSentimentValuesCollection()
			{
				new TopicSentimentValues() {  },
				new TopicSentimentValues() {  },
			};
			return collection;
		}

		public static IEnumerable<TopicStats> BuildTopicStats()
		{
			IEnumerable<TopicStats> collection = new TopicStatsCollection()
			{
				new TopicStats() {  },
				new TopicStats() {  },
			};
			return collection;
		}
#endif
