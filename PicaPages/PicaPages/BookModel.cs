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
using ClassLibrary;
using System.Collections.Generic;
using System.ComponentModel;

namespace PicaPages
{
	public enum BookChangeCause
	{
		PageAdded,
		PageDeleted,
		PageMoved
	}

	public class BookChangedEventArgs : EventArgs
	{
		// Methods
		public BookChangedEventArgs(BookChangeCause type, IList<PageModel> affectedPages)
		{
			this.ChangeType = type;
			this.AffectedPages = affectedPages;
		}

		// Properties
		public IList<PageModel> AffectedPages { get; private set; }

		public BookChangeCause ChangeType { get; private set; }
	}

	public class BookModel
	{
		// Fields
		private ProjectModel _project;

		// Events
		public event EventHandler<BookChangedEventArgs> BookChanged;

		public event EventHandler<BookChangedEventArgs> BookChanging;

		public event EventHandler<BookChangedEventArgs> BookPageDeleting;

		// Methods
		private PageModel BuildPageModel(ContentLayout layout)
		{
			return new PageModel { Book = this, Layout = layout };
		}

		private PageModel CreateNewModel()
		{
			ContentLayout item = new ContentLayout {
				ProjectID = this.Project.ProjectID,
				ContentLayoutID = Guid.NewGuid(),
				SectionID = this.Project.Project.Sections.First<Section>(s => (s.TemplateTypeID == 1)).SectionID,
				ContentPlacements = new List<ContentPlacement>(),
				BackgroundID = this.Project.Backgrounds.First<Background>(b => (b.BackgroundTypeID == 2)).BackgroundID,
				TemplateID = this.Project.Templates.First<ContentLayoutTemplate>(t => (t.TemplateTypeID == 1)).TemplateID
			};
			this.Project.Project.ContentLayouts.Add(item);
			this.Project.Project.Sections.First<Section>(s => (s.TemplateTypeID == 1)).ContentLayouts.Add(item);
			return this.BuildPageModel(item);
		}

		public void DeletePage()
		{
			if (!this.CurrentPage.IsCover && (this.FirstPage != this.LastPage))
			{
				BookChangedEventArgs args = new BookChangedEventArgs(BookChangeCause.PageDeleted, new PageModel[] { this.CurrentPage }.ToList<PageModel>());
				this.FireEvent(this.BookPageDeleting, args);
				this.FireEvent(this.BookChanging, args);
				DeleteCommand.Dispatch(this.Project, this.CurrentPage.Layout.ContentLayoutID);
				PageModel nextPage = this.CurrentPage.NextPage;
				this.Project.Project.ContentLayouts.Remove(this.CurrentPage.Layout);
				this.Project.Project.Sections.First<Section>(s => (s.SectionTypeID == 1)).ContentLayouts.Remove(this.CurrentPage.Layout);
				this.Pages.Remove(this.CurrentPage);
				this.CurrentPage = nextPage;
				this.FireEvent(this.BookChanged, args);
			}
		}

		public void DeletePageConfirmation()
		{
			if (this.CurrentPage.IsCover)
			{
				MessageBox.Show("You cannot delete the Front or Back Cover.", "", MessageBoxButtons.OK, MessageBoxIcon.Information);
			}
			else if (this.Pages.Count <= 20)
			{
				MessageBox.Show("After you've deleted this page, you'll have fewer than 20 pages in your book. If you continue with deleting this page, a blank page will be added to the end of the book.", "", MessageBoxButtons.OKCancel, MessageBoxIcon.Question, new Action<MessageBoxResult>(this.DeletePageConfirmationAnswer));
			}
			else
			{
				MessageBox.Show("You are about to delete a page. Are you sure you want to delete? ", "", MessageBoxButtons.YesNo, MessageBoxIcon.Question, new Action<MessageBoxResult>(this.DeletePageConfirmationAnswer));
			}
		}

		private void DeletePageConfirmationAnswer(MessageBoxResult result)
		{
			if ((result == MessageBoxResult.Yes) || (result == MessageBoxResult.OK))
			{
				this.DeletePage();
			}
		}

		private void FireEvent(EventHandler<BookChangedEventArgs> handler, BookChangedEventArgs args)
		{
			if (handler != null)
			{
				handler(this, args);
			}
		}

		public int? GetLeftFacingPageNumber(int pageNumber)
		{
			if (this.IsLeftFacing(pageNumber))
			{
				return new int?(pageNumber);
			}
			if (this.IsCover(pageNumber))
			{
				return null;
			}
			if (pageNumber == 1)
			{
				return null;
			}
			return new int?(pageNumber - 1);
		}

		public PageModel GetPage(int? pageNumber)
		{
			if (pageNumber.HasValue)
			{
				switch (pageNumber.Value)
				{
					case -3:
						return this.BackCover;

					case -2:
						return this.FrontCover;

					case 0:
						return null;
				}
				if (this.Pages.Count > (pageNumber.Value - 1))
				{
					return this.Pages[pageNumber.Value - 1];
				}
			}
			return null;
		}

		public int GetPageNumber(PageModel page)
		{
			if (page == null)
			{
				throw new ArgumentNullException();
			}
			if (page == this.FrontCover)
			{
				return -2;
			}
			if (page == this.BackCover)
			{
				return -3;
			}
			if (!this.Pages.Contains(page))
			{
				throw new ArgumentOutOfRangeException("This page is no longer valid");
			}
			return (this.Pages.IndexOf(page) + 1);
		}

		public int? GetRightFacingPageNumber(int pageNumber)
		{
			if (this.IsRightFacing(pageNumber))
			{
				return new int?(pageNumber);
			}
			if (this.IsCover(pageNumber))
			{
				return null;
			}
			if (pageNumber == this.LastPageNumber)
			{
				return null;
			}
			return new int?(pageNumber + 1);
		}

		private void InitializePages()
		{
			Section section = this.Project.Project.Sections.SingleOrDefault<Section>(s => s.SectionTypeID == 2);
			Section section2 = this.Project.Project.Sections.SingleOrDefault<Section>(s => s.SectionTypeID == 3);
			Section section3 = this.Project.Project.Sections.SingleOrDefault<Section>(s => s.SectionTypeID == 1);
			this.Pages = new List<PageModel>();
			if (section != null)
			{
				this.FrontCover = this.BuildPageModel(section.ContentLayouts.First<ContentLayout>());
			}
			if (section2 != null)
			{
				this.BackCover = this.BuildPageModel(section2.ContentLayouts.First<ContentLayout>());
			}
			if (section3 != null)
			{
				foreach (ContentLayout layout in (from ContentLayout in section3.ContentLayouts
					orderby ContentLayout.SortOrder
					select ContentLayout).ToList<ContentLayout>())
				{
					this.Pages.Add(this.BuildPageModel(layout));
				}
			}
		}

		public void InsertPageAfter()
		{
			if ((this.CurrentPage != this.BackCover) && !this.IsBookFull())
			{
				BookChangedEventArgs args = new BookChangedEventArgs(BookChangeCause.PageAdded, new List<PageModel>(0));
				this.FireEvent(this.BookChanging, args);
				PageModel item = this.CreateNewModel();
				InsertCommand.Dispatch(this.Project, BookChangeType.InsertAfter, this.CurrentPage.Layout.ContentLayoutID, item.Layout);
				if (this.CurrentPage == this.FrontCover)
				{
					this.Pages.Insert(0, item);
				}
				else if (this.CurrentPage == this.LastPage)
				{
					this.Pages.Add(item);
				}
				else
				{
					this.Pages.Insert(this.Pages.IndexOf(this.CurrentPage) + 1, item);
				}
				BookChangedEventArgs args2 = new BookChangedEventArgs(BookChangeCause.PageAdded, new PageModel[] { item }.ToList<PageModel>());
				this.FireEvent(this.BookChanged, args2);
				this.CurrentPage = item;
			}
		}

		public void InsertPageBefore()
		{
			if ((this.CurrentPage != this.FrontCover) && !this.IsBookFull())
			{
				BookChangedEventArgs args = new BookChangedEventArgs(BookChangeCause.PageAdded, new List<PageModel>(0));
				this.FireEvent(this.BookChanging, args);
				PageModel item = this.CreateNewModel();
				InsertCommand.Dispatch(this.Project, BookChangeType.InsertBefore, this.CurrentPage.Layout.ContentLayoutID, item.Layout);
				if (this.CurrentPage == this.BackCover)
				{
					this.Pages.Add(item);
				}
				else
				{
					this.Pages.Insert(this.Pages.IndexOf(this.CurrentPage), item);
				}
				BookChangedEventArgs args2 = new BookChangedEventArgs(BookChangeCause.PageAdded, new PageModel[] { item }.ToList<PageModel>());
				this.FireEvent(this.BookChanged, args2);
				this.CurrentPage = item;
			}
		}

		private bool IsBookFull()
		{
			if (this.Pages.Count >= 120)
			{
				MessageBox.Show("We're sorry, but at present a book can only accommodate 120 pages. If you have more pictures or text to place in your book, you'll need to delete some pages to make room.", "", MessageBoxButtons.OK, MessageBoxIcon.Information);
				return true;
			}
			return false;
		}

		public bool IsCover(int pageNumber)
		{
			return (pageNumber < 0);
		}

		public bool IsLeftFacing(int pageNumber)
		{
			switch (pageNumber)
			{
				case -5:
				case -2:
					return false;

				case -4:
				case -3:
					return true;
			}
			return ((pageNumber % 2) == 0);
		}

		public bool IsRightFacing(int pageNumber)
		{
			return !this.IsLeftFacing(pageNumber);
		}

		public void MovePageDown()
		{
			if ((this.CurrentPage != this.LastPage) && !this.CurrentPage.IsCover)
			{
				BookChangedEventArgs args = new BookChangedEventArgs(BookChangeCause.PageMoved, this.Pages.Skip<PageModel>(this.Pages.IndexOf(this.CurrentPage)).Take<PageModel>(2).ToList<PageModel>());
				this.FireEvent(this.BookChanging, args);
				MoveCommand.Dispatch(this.Project, BookChangeType.MoveDown, this.CurrentPage.Layout.ContentLayoutID);
				this.Pages.Reverse(this.Pages.IndexOf(this.CurrentPage), 2);
				this.FireEvent(this.BookChanged, args);
			}
		}

		public void MovePageUp()
		{
			if ((this.CurrentPage != this.FirstPage) && !this.CurrentPage.IsCover)
			{
				BookChangedEventArgs args = new BookChangedEventArgs(BookChangeCause.PageMoved, this.Pages.Skip<PageModel>((this.Pages.IndexOf(this.CurrentPage) - 1)).Take<PageModel>(2).ToList<PageModel>());
				this.FireEvent(this.BookChanging, args);
				MoveCommand.Dispatch(this.Project, BookChangeType.MoveUp, this.CurrentPage.Layout.ContentLayoutID);
				this.Pages.Reverse(this.Pages.IndexOf(this.CurrentPage) - 1, 2);
				this.FireEvent(this.BookChanged, args);
			}
		}

		public int NextPairedPage(int currentPageNumber)
		{
			if (currentPageNumber == -2)
			{
				return 1;
			}
			if (currentPageNumber == -3)
			{
				return -3;
			}
			if (currentPageNumber == this.LastPageNumber)
			{
				return -3;
			}
			if (this.IsLeftFacing(currentPageNumber) && (this.LastPageNumber == (currentPageNumber + 1)))
			{
				return -3;
			}
			if (this.IsLeftFacing(currentPageNumber))
			{
				return (currentPageNumber + 2);
			}
			return (currentPageNumber + 1);
		}

		public void OnAddedPagesDirectly(IEnumerable<PageModel> addedPages)
		{
			BookChangedEventArgs args = new BookChangedEventArgs(BookChangeCause.PageAdded, addedPages.ToList<PageModel>());
			this.FireEvent(this.BookChanged, args);
		}

		public void OnRemovedPagesDirectly(IEnumerable<PageModel> removedPages)
		{
			BookChangedEventArgs args = new BookChangedEventArgs(BookChangeCause.PageDeleted, removedPages.ToList<PageModel>());
			this.FireEvent(this.BookChanged, args);
		}

		public void OnRemovingPagesDirectly(IEnumerable<PageModel> removedPages)
		{
			BookChangedEventArgs args = new BookChangedEventArgs(BookChangeCause.PageDeleted, removedPages.ToList<PageModel>());
			this.FireEvent(this.BookPageDeleting, args);
			this.FireEvent(this.BookChanging, args);
		}

		public int PreviousPairedPage(int currentPageNumber)
		{
			if (currentPageNumber == -2)
			{
				return -2;
			}
			if (currentPageNumber == -3)
			{
				return this.LastPageNumber;
			}
			if (currentPageNumber == 1)
			{
				return -2;
			}
			if (this.IsLeftFacing(currentPageNumber))
			{
				return (currentPageNumber - 1);
			}
			return (currentPageNumber - 2);
		}

		// Properties
		public PageModel BackCover { get; private set; }

		public CoverType CoverType
		{
			get
			{
				return this.OutputSize.CoverTypes.First<CoverType>();
			}
		}

		public PageModel CurrentPage
		{
			get
			{
				return this.Project.CurrentPage;
			}
			set
			{
				this.Project.CurrentPage = value;
			}
		}

		public PageModel FirstPage
		{
			get
			{
				return this.Pages.First<PageModel>();
			}
		}

		public PageModel FrontCover { get; private set; }

		public PageModel LastPage
		{
			get
			{
				return this.Pages.Last<PageModel>();
			}
		}

		public int LastPageNumber
		{
			get
			{
				return this.LastPage.PageNumber;
			}
		}

		public OutputSize OutputSize
		{
			get
			{
				return this.Project.OutputSize;
			}
		}

		public List<PageModel> Pages { get; private set; }

		public ProjectModel Project
		{
			get
			{
				return this._project;
			}
			set
			{
				this._project = value;
				this.InitializePages();
			}
		}
	}
}
