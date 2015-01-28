using System.ServiceModel.DomainServices.Client;
using System.Text.RegularExpressions;
using System.Windows;
using ClassLibrary;

namespace Pivot
{
	public partial class FeedbackDialog : DialogPanel
	{
		public FeedbackDialog()
			: base()
		{
			InitializeComponent();
			InitializeDialogPanel(true/*bModal*/, x_Subject);
			base.Loaded += OnLoaded;
		}

		public override void Dispose()
		{
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
		}

		private void OnOkClick(object sender, RoutedEventArgs e)
		{
			if (x_Subject.Text.IsEmpty())
			{
				x_ErrorStatus.Text = "You need to include a subject.";
				return;
			}

			if (x_Message.Text.IsEmpty())
			{
				x_ErrorStatus.Text = "You need to include a message.";
				return;
			}

			UserServices userServices = App.Model.UserServices;
			bool isSignedIn = userServices.User.IsAuthenticated;
			string fromEmailAddress = (isSignedIn && x_FromEmailAddress.Text.IsEmpty() ? userServices.User.MemberEmail : x_FromEmailAddress.Text);
			if (fromEmailAddress.IsEmpty())
			{
				x_ErrorStatus.Text = "You need to include an email address.";
				return;
			}

			if (!Validate.EmailAddress(fromEmailAddress))
			{
				x_ErrorStatus.Text = "Your email address is invalid.";
				return;
			}

			x_OK.IsEnabled = false;
			string toEmailAddress = ResxResources.EmailAddress;
			string fromDisplayName = (isSignedIn ? userServices.User.Name : "Anonymous");
			App.Model.UserServices.EmailBodyEmail(toEmailAddress,
				fromDisplayName, fromEmailAddress, x_Subject.Text, x_Message.Text, OnSendEmailCompleted, null/*userState*/);
		}

		private void OnSendEmailCompleted(InvokeOperation operation)
		{
			x_OK.IsEnabled = true;
			string errorStatus = operation.CheckErrorStatus();
			if (errorStatus != null)
			{
				x_ErrorStatus.Text = errorStatus;
				return;
			}

			x_ErrorStatus.Text = null;
			DialogResult = MessageBoxResult.OK;
		}

		private void OnCancelClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Cancel;
		}
	}

	internal static class Validate
	{
		internal static bool EmailAddress(string emailAddress)
		{
			string emailAddressPattern = @"^([a-zA-Z0-9_\-\.]+)@((\[[0-9]{1,3}" +
				@"\.[0-9]{1,3}\.[0-9]{1,3}\.)|(([a-zA-Z0-9\-]+\" + 
				@".)+))([a-zA-Z]{2,4}|[0-9]{1,3})$";
			Regex regex = new Regex(emailAddressPattern, RegexOptions.None/*Compiled*/);
			return regex.IsMatch(emailAddress);
		}
	}
}
