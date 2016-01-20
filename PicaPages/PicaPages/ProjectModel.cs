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
using System.Collections.Generic;

namespace PicaPages
{
	public class ProjectModel : FrameworkTemplate
	{
		// Fields
		public static readonly DependencyProperty CurrentPageProperty = DependencyProperty.Register("CurrentPage", typeof(PageModel), typeof(ProjectModel), new PropertyMetadata(new PropertyChangedCallback(ProjectModel.CurrentPagePropertyChanged)));
		public static readonly DependencyProperty ProjectIDProperty = DependencyProperty.Register("ProjectID", typeof(Guid), typeof(ProjectModel), new PropertyMetadata(new PropertyChangedCallback(ProjectModel.ProjectIDChanged)));
		public static readonly DependencyProperty ThemeIDProperty = DependencyProperty.Register("ThemeID", typeof(int), typeof(ProjectModel), new PropertyMetadata(new PropertyChangedCallback(ProjectModel.ThemeIDChanged)));

		// Events
		public event EventHandler<EventArgs> BackgroundWorkingSetChanged;

		public event EventHandler<EventArgs> CurrentPageChanged;

		public event EventHandler<EventArgs> CurrentPageChanging;

		public event EventHandler<UploadFileFailedEventArgs> ErrorUploadingFile;

		public event EventHandler<FileUploadedEventArgs> FileUploadBeginning;

		public event EventHandler<FileUploadedEventArgs> ImageUploaded;

		public event EventHandler<EventArgs> TemplateWorkingSetChanged;

		public event EventHandler<TextEntryUploadedEventArgs> TextEntryUploaded;

		public event EventHandler<FileUploadedEventArgs> TextFileUploaded;

		public event EventHandler<EventArgs> UploadCancelledRequest;

		// Methods
		public ProjectModel()
		{
			this.Placements = new PlacementMediator();
			this.Uploader = new Uploader();
			this.Uploader.Project = this;
			this.TextEditor = new TextPlacementEditor();
			this.TextEditor.Project = this;
			this.ImageEditor = new ImagePlacementEditor();
			this.ImageEditor.Project = this;
		}

		public void BackgroundWorkingSetHasChanged()
		{
			if (this.BackgroundWorkingSetChanged != null)
			{
				this.BackgroundWorkingSetChanged(this, EventArgs.Empty);
			}
		}

		public void CancelCurrentUpload()
		{
			if (this.UploadCancelledRequest != null)
			{
				this.UploadCancelledRequest(this, EventArgs.Empty);
			}
		}

		public void CloseEditors()
		{
			this.TextEditor.Detach();
			this.ImageEditor.Detach();
		}

		private static void CurrentPagePropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
		{
			if (args.NewValue != args.OldValue)
			{
				((ProjectModel) sender).OnCurrentPageChanged(EventArgs.Empty);
			}
		}

		public void FileUploadFailed(string fileName, string exceptionMessage, int activeUploadedFileCount, int totalFileCount)
		{
			if (this.ErrorUploadingFile != null)
			{
				this.ErrorUploadingFile(this, new UploadFileFailedEventArgs(fileName, exceptionMessage, activeUploadedFileCount, totalFileCount));
			}
		}

		public void FileUploadsAreBeginning(int activeUploadCount, int totalUploadCount)
		{
			if (this.FileUploadBeginning != null)
			{
				this.FileUploadBeginning(this, new FileUploadedEventArgs(null, activeUploadCount, totalUploadCount));
			}
		}

		public void ImageElementUploaded(ContentElement element, int activeUploadCount, int totalUploadCount)
		{
			if (this.ImageUploaded != null)
			{
				this.ImageUploaded(this, new FileUploadedEventArgs(element, activeUploadCount, totalUploadCount));
			}
		}

		private void OnCurrentPageChanged(EventArgs args)
		{
			if (this.CurrentPageChanged != null)
			{
				this.CurrentPageChanged(this, args);
			}
		}

		private void OnCurrentPageChanging(EventArgs args)
		{
			if (this.CurrentPageChanging != null)
			{
				this.CurrentPageChanging(this, args);
			}
		}

		private static void ProjectIDChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
		{
		}

		public void TemplateWorkingSetHasChanged()
		{
			if (this.TemplateWorkingSetChanged != null)
			{
				this.TemplateWorkingSetChanged(this, EventArgs.Empty);
			}
		}

		public void TextEntryElementUploaded(ContentElement element)
		{
			if (this.TextEntryUploaded != null)
			{
				this.TextEntryUploaded(this, new TextEntryUploadedEventArgs(element));
			}
		}

		public void TextFileElementUploaded(ContentElement element, int activeUploadCount, int totalUploadCount)
		{
			if (this.TextFileUploaded != null)
			{
				this.TextFileUploaded(this, new FileUploadedEventArgs(element, activeUploadCount, totalUploadCount));
			}
		}

		private static void ThemeIDChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
		{
		}

		// Properties
		public IList<cs_User> AssociatedProjectUsers { get; set; }

		public IList<Background> Backgrounds { get; set; }

		public BookModel Book { get; set; }

		public PageModel CurrentPage
		{
			get
			{
				return (PageModel) this.GetValue(CurrentPageProperty);
			}
			set
			{
				this.OnCurrentPageChanging(EventArgs.Empty);
				base.SetValue(CurrentPageProperty, value);
			}
		}

		public ImagePlacementEditor ImageEditor { get; set; }

		public IDictionary<string, UriMapping> ImageTokenMappings { get; set; }

		public OutputSize OutputSize { get; set; }

		public PlacementMediator Placements { get; set; }

		public Project Project { get; set; }

		public Guid ProjectID
		{
			get
			{
				return (Guid) this.GetValue(ProjectIDProperty);
			}
			set
			{
				base.SetValue(ProjectIDProperty, value);
			}
		}

		public IList<Background> ProjectWorkingBackgrounds { get; set; }

		public IList<ContentLayoutTemplate> ProjectWorkingTemplates { get; set; }

		public IList<ContentLayoutTemplate> Templates { get; set; }

		public TextPlacementEditor TextEditor { get; set; }

		public int ThemeID
		{
			get
			{
				return (int) this.GetValue(ThemeIDProperty);
			}
			set
			{
				base.SetValue(ThemeIDProperty, value);
			}
		}

		public Uploader Uploader { get; private set; }
	}
}
