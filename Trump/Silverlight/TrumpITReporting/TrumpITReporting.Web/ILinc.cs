using System.IO;
using System.Text.RegularExpressions;
using System.Xml.Serialization;
using TrumpITReporting.Web.iLincAPI;

namespace TrumpITReporting.Web
{
	public class ILinc
	{
		private const string _APIVersion = "10.0";
		private const string _ClientID = "59289";
		private const string _AuthorizedUserName = "trumpWS";
		private const string _AuthorizedPassword = "ampu874$";
		private static AuthorizationType _Auth = new AuthorizationType()
		{
			APIVersion = _APIVersion,
			AuthorizedPassword = _AuthorizedPassword,
			AuthorizedUserName = _AuthorizedUserName,
			ClientID = _ClientID,
		};

		public static string ILincGetSessionFromUserID(string userId)
		{
			//the iLinc web service
			//j Could also get user pictures, first name, last name, email address, groups, divisions
			// as well as all activities and attendees, schedule, etc.
			TrumpITReporting.Web.iLincAPI.iLincAPI px = new TrumpITReporting.Web.iLincAPI.iLincAPI();
			GetSessionDataType gtype = new GetSessionDataType();
			//ActivityID ID string of an existing activity in the iLinc system (call to GetActivityID)
			//UserID ID string of an existing user in the iLinc system (call to GetUserID)
			//		Either ActivityID or UserID is required. Both are not.
			//StartDate Date/time string in format: mm/dd/yyyy (i.e. 03/23/2006)
			//EndDate Date/time string in format: mm/dd/yyyy (i.e. 03/23/2006)
			//TimeZone TimeZone string from the list of acceptable TimeZone values. Only needed if StartDate
			//		or EndDate present. If a TimeZone is not specified, the site’s time zone will be used.

			gtype.UserID = userId;
			//gtype.ActivityID = null;
			//gtype.StartDate = "01/01/2009"; // optional
			//gtype.EndDate = "12/31/2009"; // optional
			//gtype.TimeZone = "US/Eastern"; // If a TimeZone is not specified, the site’s time zone will be used
			GetSessionDataResponseType response = px.GetSessionData(_Auth, gtype);
			XmlSerializer serializer = new XmlSerializer(typeof(GetSessionDataResponseType));
			StringWriter xml = new StringWriter();
			serializer.Serialize(xml, response);
			// Remove all xmlns:* instances to simplify LINQ and xpath expressions
			string xmlResult = Regex.Replace(xml.ToString(), @"(xmlns:?[^=]*=[""][^""]*[""])", "", RegexOptions.IgnoreCase | RegexOptions.Multiline);
			return xmlResult;
		}

		public static string ILincGetSessionFromActivityID(string activityID)
		{
			TrumpITReporting.Web.iLincAPI.iLincAPI px = new TrumpITReporting.Web.iLincAPI.iLincAPI();
			GetSessionDataType gtype = new GetSessionDataType();
			gtype.ActivityID = activityID;
			GetSessionDataResponseType response = px.GetSessionData(_Auth, gtype);
			XmlSerializer serializer = new XmlSerializer(typeof(GetSessionDataResponseType));
			StringWriter xml = new StringWriter();
			serializer.Serialize(xml, response);
			// Remove all xmlns:* instances to simplify LINQ and xpath expressions
			string xmlResult = Regex.Replace(xml.ToString(), @"(xmlns:?[^=]*=[""][^""]*[""])", "", RegexOptions.IgnoreCase | RegexOptions.Multiline);
			return xmlResult;
		}
	}
}
