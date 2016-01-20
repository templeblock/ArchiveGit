#pragma once
#include "WholeImageFilter.h"
#include "WarpGrid.h"

namespace ImageEffects
{
	/// A filter for warping images using the gridwarp algorithm.
	/// You need to supply two warp grids, one for the source image and
	/// one for the destination image. The image will be warped so that
	/// a point in the source grid moves to its counterpart in the destination
	/// grid.
	class WarpFilter : WholeImageFilter
	{
		/// Get the source warp grid.
		/// <returns> the source grid
		/// </returns>
		/// Set the source warp grid.
		/// <param name="sourceGrid">the source grid
		/// </param>
#ifdef NOTUSED
	public:
		virtual property WarpGrid *SourceGrid
		{
			WarpGrid *get()
			{
				return sourceGrid;
			}

			void set(WarpGrid *value)
			{
				sourceGrid = value;
			}

		}

		/// Get the destination warp grid.
		/// <returns> the destination grid
		/// </returns>
		/// Set the destination warp grid.
		/// <param name="destGrid">the destination grid
		/// </param>
		virtual property WarpGrid *DestGrid
		{
			WarpGrid *get()
			{
				return destGrid;
			}

			void set(WarpGrid *value)
			{
				destGrid = value;
			}

		}
		virtual property int Frames
		{
			int get()
			{
				return frames;
			}

			void set(int value)
			{
				frames = value;
			}

		}
#endif NOTUSED

	private:
		WarpGrid *sourceGrid;
		WarpGrid *destGrid;
		int frames;

		/// Create a WarpFilter.
	public:
		WarpFilter()
		{
			frames = 1;
		}

		/// Create a WarpFilter with two warp grids.
		/// <param name="sourceGrid">the source grid
		/// </param>
		/// <param name="destGrid">the destination grid
		/// </param>
		WarpFilter(WarpGrid *newsourceGrid, WarpGrid *newdestGrid)
		{
			sourceGrid = newsourceGrid;
			destGrid = newdestGrid;
		}

	public:
		virtual void transformSpace(CRect& r)
		{
			r.right = r.left + (r.Width() * frames);
		}

		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			Array<int> *outPixels = new Array<int>(width * height);

			if (frames <= 1)
			{
				sourceGrid->warp(inPixels, width, height, sourceGrid, destGrid, outPixels);
				//			setRGB.setRGB(0, 0, width, height, outPixels);
			}
			else
			{
				WarpGrid *newGrid = new WarpGrid(sourceGrid->rows, sourceGrid->cols, width, height);
				for (int i = 0; i < frames; i++)
				{
					float t = (float)(i) / (frames - 1);
					sourceGrid->lerp(t, destGrid, newGrid);
					sourceGrid->warp(inPixels, width, height, sourceGrid, newGrid, outPixels);
					//setRGB(i*width, 0, width, height, outPixels);
				}
			}
			return outPixels;
		}

	public:
		virtual Array<int> *getPixels(System::Drawing::Image *image, int width, int height)
		{
			Array<int> *pixels = new Array<int>(width * height);

			pixels = new Array<int>(width * height);
			PixelGrabber *pg = new PixelGrabber(image, 0, 0, width, height, pixels, 0, width);
			try
			{
				pg->CapturePixels();
			}
			catch (System::Threading::ThreadInterruptedException *e)
			{
				return NULL;
			}

			if ((pg->status() & ImageObserver::ABORT) != 0)
			{
				return NULL;
			}
			return pixels;
		}

		virtual void morph(Array<int> *srcPixels, Array<int> *destPixels, Array<int> *outPixels, WarpGrid *srcGrid, WarpGrid *destGrid, int width, int height, float t)
		{
			WarpGrid *newGrid = new WarpGrid(srcGrid->rows, srcGrid->cols, width, height);
			srcGrid->lerp(t, destGrid, newGrid);
			srcGrid->warp(srcPixels, width, height, srcGrid, newGrid, outPixels);
			Array<int> *destPixels2 = new Array<int>(width * height);
			destGrid->warp(destPixels, width, height, destGrid, newGrid, destPixels2);
			crossDissolve(outPixels, destPixels2, width, height, t);
		}

		virtual void crossDissolve(Array<int> *pixels1, Array<int> *pixels2, int width, int height, float t)
		{
			int index = 0;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					pixels1->SetValue(mixColors(t, pixels1[index], pixels2[index]), index);
					index++;
				}
			}
		}

		virtual CString *ToString()
		{
			return new CString("Distort/Mesh Warp...");
		}
	};
}
