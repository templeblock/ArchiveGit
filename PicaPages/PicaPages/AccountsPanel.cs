using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

using tMillicents = System.Int64;
using ClassLibrary;

namespace PicaPages
{
	public class AccountsPanel : ViewPanel
	{
//j		private TextBlock m_AccountsTextBlock;

		/////////////////////////////////////////////////////////////////////////////
		public AccountsPanel()
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
//j			m_AccountsTextBlock = FindName("x_AccountsTextBlock") as TextBlock;
		}

		/////////////////////////////////////////////////////////////////////////////
		// Method used to catch the document's CloseEvent
		public override void DocumentClose(Document document, EventArgs e)
		{
//j			m_AccountsTextBlock.Text = "";
		}

		/////////////////////////////////////////////////////////////////////////////
		// Method used to catch the document's ChangeEvent
		public override void DocumentChange(Document document, EventArgs e)
		{
			Draw(document);
		}

		/////////////////////////////////////////////////////////////////////////////
		private void Draw(Document document)
		{
			if (document == null)
				return;

			List<Account> accounts = document.AccountList;
		}
	}
}
