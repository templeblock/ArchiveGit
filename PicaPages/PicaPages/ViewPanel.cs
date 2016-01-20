using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;

namespace PicaPages
{
	public interface IDocumentListener
	{
		void InitDocumentListener(DocumentHolder documentHolder);
		void DocumentClose(Document document, EventArgs e);
		void DocumentChange(Document document, EventArgs e);
	}

	// Disabling class renaming
	//[System.Reflection.ObfuscationAttribute(Feature = "renaming", ApplyToMembers = false)]
	public abstract class ViewPanel : Grid, IDocumentListener
	{
		/////////////////////////////////////////////////////////////////////////////
		public ViewPanel()
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		public virtual void InitDocumentListener(DocumentHolder documentHolder)
		{
			// Set the view event listeners
			documentHolder.CloseListeners += DocumentClose;
			documentHolder.ChangeListeners += DocumentChange;
		}

		/////////////////////////////////////////////////////////////////////////////
		// Method used to catch the document's CloseEvent
		public abstract void DocumentClose(Document document, EventArgs e);

		/////////////////////////////////////////////////////////////////////////////
		// Method used to catch the document's ChangeEvent
		public abstract void DocumentChange(Document document, EventArgs e);
	}
}
