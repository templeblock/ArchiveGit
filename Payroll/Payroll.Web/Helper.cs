using System;
using System.Net.Mail;

namespace Payroll.Web
{
	public class Helper
	{
		public static void SendMail(MailAddress from, MailAddress to, string subject, string body)
		{
			try
			{
				MailMessage mail = new MailMessage();
				mail.From = from;
				mail.To.Add(to);
				mail.Subject = subject;
				mail.Body = body;
				mail.IsBodyHtml = (body.IndexOf("html") >= 0);
				SmtpClient smtp = new SmtpClient();
				smtp.Send(mail);
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}
	}
}