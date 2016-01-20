using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using ClassLibrary;

namespace PicaPages
{
	public partial class AddPackageDialog : UserControl, IDialogPanel
	{
		// Implement IDialogPanel
		private DialogPanelImpl m_DialogPanelImpl = new DialogPanelImpl();
		public void DialogPanelInit(FrameworkElement panel, bool bModal, EventHandler closeHandler) { m_DialogPanelImpl.DialogPanelInit(panel, bModal, closeHandler); }
		public void DialogPanelShow(bool bShow) { m_DialogPanelImpl.DialogPanelShow(bShow); }

		private MessageBoxResult m_DialogResult;
		public MessageBoxResult Result { get { return m_DialogResult; } }

		List<Transaction> m_SavedTransactions;
		public List<Transaction> Transactions { get { return m_SavedTransactions; } }

		public AddPackageDialog(string name, List<Transaction> transactions, EventHandler closeHandler)
		{
			InitializeComponent();

			string title = string.Format("Select the start and end dates for '{0}'", name);

			x_Title.Text = title;
			m_SavedTransactions = transactions;
			DialogPanelInit(this, false/*bModal*/, closeHandler);
		}

		private void OkButton_Click(object sender, EventArgs args)
		{
			m_DialogResult = MessageBoxResult.OK;
			DialogPanelShow(false);
		}

		private void CancelButton_Click(object sender, EventArgs args)
		{
			m_DialogResult = MessageBoxResult.Cancel;
			DialogPanelShow(false);
		}
	}
}
