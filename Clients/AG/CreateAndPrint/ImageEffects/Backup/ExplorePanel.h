#pragma once

namespace ImageEffects
{
	class ExplorePanel : System::Windows::Forms::Panel
	{
	private:
		ref class AnonymousClassRunnable : IThreadRunnable
		{
		public:
			AnonymousClassRunnable(bool keepShape, bool keepColors, int id, ExplorePanel *enclosingInstance)
			{
				InitBlock(keepShape, keepColors, id, enclosingInstance);
			}
		private:
			void InitBlock(bool keepShape, bool keepColors, int id, ExplorePanel *enclosingInstance)
			{
				keepShape = keepShape;
				keepColors = keepColors;
				id = id;
				enclosingInstance = enclosingInstance;
			}
			bool keepShape;
			bool keepColors;
			int id;
			ExplorePanel *enclosingInstance;
		public:
			property ExplorePanel *Enclosing_Instance
			{
				ExplorePanel *get()
				{
					return enclosingInstance;
				}

			}
			virtual void Run()
			{
				MutatableFilter *sourceFilter = (MutatableFilter*)(Enclosing_Instance->samples[0]->filter);
				for (int i = 0; !Enclosing_Instance->stopThread && i < Enclosing_Instance->samples->Length; i++)
				{
					if (i != 0)
						sourceFilter->mutate(Enclosing_Instance->mutationLevel, Enclosing_Instance->samples[i]->filter, keepShape, keepColors);
					if (i != 0 || id != 0)
						Enclosing_Instance->samples[i]->refilter();
				}
			}
		};
	public:
		virtual property BITMAPINFOHEADER *Selected
		{
			BITMAPINFOHEADER *get()
			{
				return samples[0]->Filter;
			}

		}
		virtual property bool KeepShape
		{
			bool get()
			{
				return keepShape;
			}

			void set(bool value)
			{
				keepShape = value;
			}

		}
		virtual property bool KeepColors
		{
			bool get()
			{
				return keepColors;
			}

			void set(bool value)
			{
				keepColors = value;
			}

		}
		virtual property BITMAPINFOHEADER *Filter
		{
			void set(BITMAPINFOHEADER *value)
			{
				for (int i = 0; i < samples->Length; i++)
				{
					samples[i]->filter = (BITMAPINFOHEADER*)(value->Clone());
					int w = i == 0?144:64;
					samples[i]->Image = new BITMAPINFOHEADER(w, w, (System::Drawing::Imaging::PixelFormat)(System::Drawing::Imaging::PixelFormat::Format32bppArgb));
				}
				samples[0]->refilter();
				choose(0, false, false);
			}

		}
	private:
		Array<MutateSample*> *samples;
		bool keepColors, keepShape;
		float mutationLevel = 1.0f;

	public:
		ExplorePanel()
		{
			System::Windows::Forms::Border3DStyle b = System::Windows::Forms::Border3DStyle::Flat;
			setLayout(new ExploreLayout());
			samples = new Array<MutateSample*>(13);
			for (int i = 0; i < samples->Length; i++)
			{
				samples[i] = new MutateSample(this, i);
				System::Windows::Forms::ControlPaint::DrawBorder3D(samples[i]->CreateGraphics(), 0, 0, samples[i]->Width, samples[i]->Height, b);
				Controls->Add(samples[i]);
			}
		}

		virtual void setMutationLevel(float mutationLevel)
		{
			mutationLevel = mutationLevel;
		}

		virtual float setMutationLevel()
		{
			return mutationLevel;
		}

	private:
		bool stopThread;
		SupportClass::ThreadClass *filterThread;

	public:
		virtual void choose(int id, bool keepColors, bool keepShape)
		{
			if (id != 0)
			{
				BITMAPINFOHEADER *filter = samples[0]->filter;
				samples[0]->filter = samples[id]->filter;
				samples[id]->filter = filter;
			}
			for (int i = id == 0?1:0; i < samples->Length; i++)
			{
				samples[i]->filtering = true;
				samples[i]->Refresh();
			}

			stopThread = true;
			try
			{
				if (filterThread != NULL)
					filterThread->Join();
			}
			catch (System::Threading::ThreadInterruptedException *e)
			{
			}
			stopThread = false;
			filterThread = new SupportClass::ThreadClass(new System::Threading::ThreadStart(new AnonymousClassRunnable(keepShape, keepColors, id, this)->Run));
			filterThread->Start();
			firePropertyChange("filter", NULL, Selected);
		}

	private:
	
		ref class MutateSample : System::Windows::Forms::Control
		{
		private:
			static System::Int32 state3;
			static void mouseDown(System::Object *event_sender, System::Windows::Forms::MouseEventArgs *e)
			{
				state3 = ((int)(e->Button) | (int)(System::Windows::Forms::Control::ModifierKeys));
			}
			void InitBlock(ExplorePanel *enclosingInstance)
			{
				enclosingInstance = enclosingInstance;
			}
			ExplorePanel *enclosingInstance;
		public:
			virtual property BITMAPINFOHEADER *Filter
			{
				BITMAPINFOHEADER *get()
				{
					return filter;
				}

				void set(BITMAPINFOHEADER *value)
				{
					filter = value;
				}

			}
			virtual property BITMAPINFOHEADER *Image
			{
				void set(BITMAPINFOHEADER *value)
				{
					image = value;
					filtering = false;
					Refresh();
				}

			}
			property ExplorePanel *Enclosing_Instance
			{
				ExplorePanel *get()
				{
					return enclosingInstance;
				}

			}

			BITMAPINFOHEADER *filter;
		private:
			BITMAPINFOHEADER *image;
			int id;
		public:
			bool filtering;

			MutateSample(ExplorePanel *enclosingInstance, int id)
			{
				InitBlock(enclosingInstance);
				id = id;
				MouseDown += new System::Windows::Forms::MouseEventHandler(this, &ImageEffects::ExplorePanel::MutateSample::mouseDown);
				Click += new System::EventHandler(this, &MutateSample::mouseClicked);
				MouseEnter += new System::EventHandler(this, &MutateSample::mouseEntered);
				MouseLeave += new System::EventHandler(this, &MutateSample::mouseExited);
				MouseDown += new System::Windows::Forms::MouseEventHandler(this, &MutateSample::mousePressed);
				MouseUp += new System::Windows::Forms::MouseEventHandler(this, &MutateSample::mouseReleased);
				BackColor = COLORREF::White;
			}

			System::Drawing::Size getPreferredSize()
			{
				int w = id == 0?144:64;
				return System::Drawing::Size(w, w);
			}

			virtual void refilter()
			{
				if (image != NULL)
				{
					image = filter->filter(image, image);
					Image = image;
				}
			}

			virtual void mousePressed(System::Object *event_sender, System::Windows::Forms::MouseEventArgs *e)
			{
				Enclosing_Instance->choose(id, Enclosing_Instance->keepColors, Enclosing_Instance->keepShape);
			}

			virtual void mouseReleased(System::Object *event_sender, System::Windows::Forms::MouseEventArgs *e)
			{
			}

			virtual void mouseClicked(System::Object *event_sender, System::EventArgs *e)
			{
			}

			virtual void mouseEntered(System::Object *event_sender, System::EventArgs *e)
			{
			}

			virtual void mouseExited(System::Object *event_sender, System::EventArgs *e)
			{
			}

		protected:
			virtual void OnPaint(System::Windows::Forms::PaintEventArgs *g_EventArg)
			{
				System::Drawing::Graphics *g = NULL;
				if (g_EventArg != NULL)
					g = g_EventArg->Graphics;
				System::Drawing::Size size = Size;
				if (filtering)
				{
				}
				else if (image != NULL)
				{
					int x = (size.Width() - image->Width) / 2;
					int y = (size.Height() - image->Height) / 2;
					g->DrawImage(image, x, y);
				}
			}
		};
	};

	private ref class ExploreLayout : LayoutManager
	{

	private:
		static const int CELL_SIZE = 80;
		static const Array<int> *positions = new Array<int>{1, 1, 0, 0, 1, 0, 2, 0, 3, 0, 3, 1, 3, 2, 3, 3, 2, 3, 1, 3, 0, 3, 0, 2, 0, 1};

	public:
		virtual void addLayoutComponent(CString *name, System::Windows::Forms::Control *comp)
		{
		}

		virtual void removeLayoutComponent(System::Windows::Forms::Control *comp)
		{
		}

		virtual System::Drawing::Size preferredLayoutSize(System::Windows::Forms::Control *target)
		{
			return System::Drawing::Size(4 * CELL_SIZE + 2 * 20, 4 * CELL_SIZE + 2 * 20);
		}

		virtual System::Drawing::Size minimumLayoutSize(System::Windows::Forms::Control *target)
		{
			return System::Drawing::Size(0, 0);
		}

		virtual void layoutContainer(System::Windows::Forms::Control *target)
		{
//			lock (target.getTreeLock())
			System::Threading::Monitor::Enter(target.getTreeLock());
			try
			{
				Array<System::Int32> *insets = SupportClass::GetInsets(target);
				System::Drawing::Size size = target->Size;
				//			int w = size.width - (insets.left + insets.right);
				//			int h = size.height - (insets.top + insets.bottom);
				int count = (int)(target->Controls->Count);

				for (int i = 0; i < count; i++)
				{
					System::Windows::Forms::Control *m = target->Controls[i];
					if (m->Visible)
					{
						System::Drawing::Size d = m->Size;
						int c = i == 0?2 * CELL_SIZE:CELL_SIZE;
						int x = 20 + CELL_SIZE * positions[2 * i] + (c - d.Width()) / 2;
						int y = 20 + CELL_SIZE * positions[2 * i + 1] + (c - d.Height()) / 2;
						int w = i == 0?144:64;
						m->Bounds = CRect(x, y, w, w);
					}
				}
			}
			finally
			{
				System::Threading::Monitor::Exit(target.getTreeLock());
			}
		}
	};
}
