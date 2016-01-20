#pragma once

namespace ImageEffects
{
	class ImageUtils abstract
	{

	public:
		static const int SELECTED = ((int)(0xff000000));
		static const int UNSELECTED = 0x00000000;

		static BITMAPINFOHEADER *backgroundImage = NULL;

		static BITMAPINFOHEADER *createImage(System::Drawing::Image *producer)
		{
			PixelGrabber *pg = new PixelGrabber(producer, 0, 0, -1, -1, NULL, 0, 0);
			try
			{
				pg->CapturePixels();
			}
			catch (System::Threading::ThreadInterruptedException *e)
			{
				throw new System::SystemException("Image fetch interrupted");
			}
			if ((pg->status() & ImageObserver::ABORT) != 0)
				throw new System::SystemException("Image fetch aborted");
			if ((pg->status() & ImageObserver::ERROR) != 0)
				throw new System::SystemException("Image fetch error");
			BITMAPINFOHEADER *p = new BITMAPINFOHEADER(pg->Width, pg->Height, (System::Drawing::Imaging::PixelFormat)(System::Drawing::Imaging::PixelFormat::Format32bppArgb));
			p->setRGB(0, 0, pg->Width, pg->Height, (Array<int>*)(pg->Pixels), 0, pg->Width);
			return p;
		}

		// Returns a *copy* of a subimage of image. This avoids the performance problems associated with BufferedImage.getSubimage.
		static BITMAPINFOHEADER *getSubimage(BITMAPINFOHEADER *image, int x, int y, int w, int h)
		{
			BITMAPINFOHEADER *newImage = new BITMAPINFOHEADER(w, h, (System::Drawing::Imaging::PixelFormat)(System::Drawing::Imaging::PixelFormat::Format32bppArgb));
			System::Drawing::Graphics *g = System::Drawing::Graphics::FromImage(newImage);
			System::Drawing::Drawing2D::Matrix *temp_Matrix;
			temp_Matrix = new System::Drawing::Drawing2D::Matrix();
			temp_Matrix->Translate((float)(- x), (float)(- y));
			g->Transform = temp_Matrix;
			g->DrawImage(image, 0, 0);
			delete g;
			return newImage;
		}

		static BITMAPINFOHEADER *cloneImage(BITMAPINFOHEADER *image)
		{
			BITMAPINFOHEADER *newImage = new BITMAPINFOHEADER(image->Width, image->Height, (System::Drawing::Imaging::PixelFormat)(System::Drawing::Imaging::PixelFormat::Format32bppArgb));
			System::Drawing::Graphics *g = System::Drawing::Graphics::FromImage(newImage);
			g->Transform = NULL;
			g->DrawImage(image, 0, 0);
			delete g;
			return newImage;
		}

		static void paintCheckedBackground(System::Windows::Forms::Control *c, System::Drawing::Graphics *g, int x, int y, int width, int height)
		{
			if (backgroundImage == NULL)
			{
				backgroundImage = new BITMAPINFOHEADER(64, 64, (System::Drawing::Imaging::PixelFormat)(System::Drawing::Imaging::PixelFormat::Format32bppArgb));
				System::Drawing::Graphics *bg = System::Drawing::Graphics::FromImage(backgroundImage);
				for (int by = 0; by < 64; by += 8)
				{
					for (int bx = 0; bx < 64; bx += 8)
					{
						bg->SetColor(((bx * by) & 8) != 0 ? COLORREF::LightGray : COLORREF::White);
						bg->FillRectangle(bg->GetPaint(), bx, by, 8, 8);
					}
				}
				delete bg;
			}

			if (backgroundImage != NULL)
			{
				System::Drawing::Graphics *saveClip = g;
				CRect r = CRect::Truncate(g->ClipBounds);
				if (r.IsEmpty)
				{
					System::Drawing::Size temp_Size;
					temp_Size = c->Size;
					r = CRect(0, 0, temp_Size.Width(), temp_Size.Height());
				}
				r = CRect::Intersect(r, CRect(x, y, width, height));
				g->SetClip(r);
				int w = backgroundImage->Width;
				int h = backgroundImage->Height;
				if (w != - 1 && h != - 1)
				{
					int x1 = (r.X / w) * w;
					int y1 = (r.Y / h) * h;
					int x2 = ((r.X + r.Width() + w - 1) / w) * w;
					int y2 = ((r.Y + r.Height() + h - 1) / h) * h;
					for (y = y1; y < y2; y += h)
						for (x = x1; x < x2; x += w)
						{
							g->DrawImage(backgroundImage, x, y);
						}
				}
				g->SetClip(saveClip);
			}
		}

		static CRect getSelectedBounds(BITMAPINFOHEADER *p)
		{
			int width = p->Width;
			int height = p->Height;
			int maxX = 0, maxY = 0, minX = width, minY = height;
			bool anySelected = false;
			int y1;

			for (y1 = height - 1; y1 >= 0; y1--)
			{
				for (int x = 0; x < minX; x++)
				{
					if ((p->GetPixel(x, y1)->ToArgb() & ((int)(0xff000000))) != 0)
					{
						minX = x;
						maxY = y1;
						anySelected = true;
						break;
					}
				}
				for (int x = width - 1; x >= maxX; x--)
				{
					if ((p->GetPixel(x, y1)->ToArgb() & ((int)(0xff000000))) != 0)
					{
						maxX = x;
						maxY = y1;
						anySelected = true;
						break;
					}
				}
				if (anySelected)
					break;
			}
			for (int y = 0; y < y1; y++)
			{
				for (int x = 0; x < minX; x++)
				{
					if ((p->GetPixel(x, y)->ToArgb() & ((int)(0xff000000))) != 0)
					{
						minX = x;
						if (y < minY)
							minY = y;
						anySelected = true;
						break;
					}
				}
				for (int x = width - 1; x >= maxX; x--)
				{
					if ((p->GetPixel(x, y)->ToArgb() & ((int)(0xff000000))) != 0)
					{
						maxX = x;
						if (y < minY)
							minY = y;
						anySelected = true;
						break;
					}
				}
			}
			if (anySelected)
				return CRect(minX, minY, maxX - minX + 1, maxY - minY + 1);
			return CRect::Empty;
		}

		/// Compose src onto dst using the alpha of sel to interpolate between the two.
		/// I can't think of a way to do this using AlphaComposite.
		static void composeThroughMask(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst, BITMAPINFOHEADER *sel)
		{
			int x = CRect::Truncate(RasterBoundsSupport(src))->X;
			int y = CRect::Truncate(RasterBoundsSupport(src))->Y;
			int w = src->biWidth;
			int h = src->biHeight;

			Array<int> *srcRGB = NULL;
			Array<int> *selRGB = NULL;
			Array<int> *dstRGB = NULL;

			for (int i = 0; i < h; i++)
			{
				srcRGB = src->getPixels(x, y, w, 1, srcRGB);
				selRGB = sel->getPixels(x, y, w, 1, selRGB);
				dstRGB = dst->getPixels(x, y, w, 1, dstRGB);

				int k = x;
				for (int j = 0; j < w; j++)
				{
					int sr = srcRGB[k];
					int dir = dstRGB[k];
					int sg = srcRGB[k + 1];
					int dig = dstRGB[k + 1];
					int sb = srcRGB[k + 2];
					int dib = dstRGB[k + 2];
					int sa = srcRGB[k + 3];
					int dia = dstRGB[k + 3];

					float a = selRGB[k + 3] / 255f;
					float ac = 1 - a;

					dstRGB[k] = (int)(a * sr + ac * dir);
					dstRGB[k + 1] = (int)(a * sg + ac * dig);
					dstRGB[k + 2] = (int)(a * sb + ac * dib);
					dstRGB[k + 3] = (int)(a * sa + ac * dia);
					k += 4;
				}

				dst->setPixels(x, y, w, 1, dstRGB);
				y++;
			}
		}
	};
}
