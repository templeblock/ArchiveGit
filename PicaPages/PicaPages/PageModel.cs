using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace PicaPages
{
	public class PageModel
	{
		// Fields
		private BookModel _book;

		// Events
		public event EventHandler<EventArgs> ViewChanged;

		public event EventHandler<EventArgs> ViewDeleted;

		// Methods
		private void Book_BookPageDeleting(object sender, BookChangedEventArgs e)
		{
			if (e.AffectedPages.Contains(this))
			{
				this.Book.BookPageDeleting -= new EventHandler<BookChangedEventArgs>(this.Book_BookPageDeleting);
				if (this.ViewDeleted != null)
				{
					this.ViewDeleted(this, EventArgs.Empty);
				}
			}
		}

		public bool IsAfter(PageModel other)
		{
			if (this == this.Book.FrontCover)
			{
				return false;
			}
			return ((this == this.Book.BackCover) || (this.PageNumber > other.PageNumber));
		}

		public bool IsBefore(PageModel other)
		{
			if (this == this.Book.FrontCover)
			{
				return true;
			}
			if (this == this.Book.BackCover)
			{
				return false;
			}
			return (this.PageNumber < other.PageNumber);
		}

		public void OnViewChanged()
		{
			if (this.ViewChanged != null)
			{
				this.ViewChanged(this, EventArgs.Empty);
			}
		}

		// Properties
		public BookModel Book
		{
			get
			{
				return this._book;
			}
			set
			{
				if (this.Book != null)
				{
					this.Book.BookPageDeleting -= new EventHandler<BookChangedEventArgs>(this.Book_BookPageDeleting);
				}
				this._book = value;
				if (this.Book != null)
				{
					this.Book.BookPageDeleting += new EventHandler<BookChangedEventArgs>(this.Book_BookPageDeleting);
				}
			}
		}

		public bool IsBackCover
		{
			get
			{
				return (this.Book.BackCover == this);
			}
		}

		public bool IsCover
		{
			get
			{
				if (!this.IsFrontCover)
				{
					return this.IsBackCover;
				}
				return true;
			}
		}

		public bool IsFrontCover
		{
			get
			{
				return (this.Book.FrontCover == this);
			}
		}

		public bool IsLeftFacing
		{
			get
			{
				return this.Book.IsLeftFacing(this.PageNumber);
			}
		}

		public virtual bool IsPadding
		{
			get
			{
				return false;
			}
		}

		public bool IsRightFacing
		{
			get
			{
				return this.Book.IsRightFacing(this.PageNumber);
			}
		}

		public ContentLayout Layout { get; set; }

		public PageModel LeftFacingPage
		{
			get
			{
				return this.Book.GetPage(this.LeftFacingPageNumber);
			}
		}

		public int? LeftFacingPageNumber
		{
			get
			{
				return this.Book.GetLeftFacingPageNumber(this.PageNumber);
			}
		}

		public PageModel NextPage
		{
			get
			{
				if (this.IsRightFacing)
				{
					return this.Book.GetPage(new int?(this.NextPairedPage));
				}
				if (this.RightFacingPage != null)
				{
					return this.RightFacingPage;
				}
				return this.Book.BackCover;
			}
		}

		public int NextPairedPage
		{
			get
			{
				return this.Book.NextPairedPage(this.PageNumber);
			}
		}

		public virtual int PageNumber
		{
			get
			{
				return this.Book.GetPageNumber(this);
			}
		}

		public PageModel PreviousPage
		{
			get
			{
				if (this.IsLeftFacing)
				{
					return this.Book.GetPage(new int?(this.PreviousPairedPage));
				}
				return this.LeftFacingPage;
			}
		}

		public int PreviousPairedPage
		{
			get
			{
				return this.Book.PreviousPairedPage(this.PageNumber);
			}
		}

		public ProjectModel Project
		{
			get
			{
				return this.Book.Project;
			}
		}

		public PageModel RightFacingPage
		{
			get
			{
				return this.Book.GetPage(this.RightFacingPageNumber);
			}
		}

		public int? RightFacingPageNumber
		{
			get
			{
				return this.Book.GetRightFacingPageNumber(this.PageNumber);
			}
		}
	}
}
