#pragma once

namespace ImageEffects
{
	/// A warp grid.
	class WarpGrid
	{
	public:
		Array<float> *xGrid;
		Array<float> *yGrid;
		int rows, cols;

		WarpGrid(int rows, int cols, int w, int h)
		{
			rows = rows;
			cols = cols;
			xGrid = new Array<float>(rows * cols);
			yGrid = new Array<float>(rows * cols);
			int index = 0;
			for (int row = 0; row < rows; row++)
			{
				for (int col = 0; col < cols; col++)
				{
					xGrid->SetValue((float)(col) * w / (cols - 1), index);
					yGrid->SetValue((float)(row) * h / (rows - 1), index);
					index++;
				}
			}
		}

		virtual void lerpw(float t, WarpGrid *destination, WarpGrid *intermediate)
		{
			if (rows != destination->rows || cols != destination->cols)
				throw new System::ArgumentException("source and destination are different sizes");
			if (rows != intermediate->rows || cols != intermediate->cols)
				throw new System::ArgumentException("source and intermediate are different sizes");
			int index = 0;
			for (int row = 0; row < rows; row++)
			{
				for (int col = 0; col < cols; col++)
				{
					intermediate->xGrid->SetValue((float)(lerp(t, xGrid->GetAt(index), destination->xGrid->GetAt(index))), index);
					intermediate->yGrid->SetValue((float)(lerp(t, yGrid->GetAt(index), destination->yGrid->GetAt(index))), index);
					index++;
				}
			}
		}

		virtual void warp(Array<int> *inPixels, int cols, int rows, WarpGrid *sourceGrid, WarpGrid *destGrid, Array<int> *outPixels)
		{
			try
			{
				int x, y;
				int u, v;
				Array<int> *intermediate;
				WarpGrid *splines;

				if (sourceGrid->rows != destGrid->rows || sourceGrid->cols != destGrid->cols)
					throw new System::ArgumentException("source and destination grids are different sizes");

				int size = max(cols, rows);
				Array<float> *xrow = new Array<float>(size);
				Array<float> *yrow = new Array<float>(size);
				Array<float> *scale = new Array<float>(size + 1);
				Array<float> *interpolated = new Array<float>(size + 1);

				int gridCols = sourceGrid->cols;
				int gridRows = sourceGrid->rows;

				splines = new WarpGrid(rows, gridCols, 1, 1);

				for (u = 0; u < gridCols; u++)
				{
					int i = u;

					for (v = 0; v < gridRows; v++)
					{
						xrow->SetValue(sourceGrid->xGrid->GetAt(i), v);
						yrow->SetValue(sourceGrid->yGrid->GetAt(i), v);
						i += gridCols;
					}

					interpolate(yrow, xrow, 0, interpolated, 0, rows);

					i = u;
					for (y = 0; y < rows; y++)
					{
						splines->xGrid->SetValue(interpolated->GetAt(y), i);
						i += gridCols;
					}
				}

				for (u = 0; u < gridCols; u++)
				{
					int i = u;

					for (v = 0; v < gridRows; v++)
					{
						xrow->SetValue(destGrid->xGrid->GetAt(i), v);
						yrow->SetValue(destGrid->yGrid->GetAt(i), v);
						i += gridCols;
					}

					interpolate(yrow, xrow, 0, interpolated, 0, rows);

					i = u;
					for (y = 0; y < rows; y++)
					{
						splines->yGrid->SetValue(interpolated->GetAt(y), i);
						i += gridCols;
					}
				}

				// first pass: warp x using splines 
				intermediate = new Array<int>(rows * cols);

				int offset = 0;
				for (y = 0; y < rows; y++)
				{
					// fit spline to x-intercepts; resample over all cols 
					interpolate(splines->xGrid, splines->yGrid, offset, scale, 0, cols);
					scale->SetValue((float)cols, cols);
					resample(inPixels, intermediate, cols, y * cols, 1, scale);
					offset += gridCols;
				}

				// create table of y-intercepts for intermediate mesh's hor splines 

				splines = new WarpGrid(gridRows, cols, 1, 1);

				offset = 0;
				int offset2 = 0;
				for (v = 0; v < gridRows; v++)
				{
					interpolate(sourceGrid->xGrid, sourceGrid->yGrid, offset, splines->xGrid, offset2, cols);
					offset += gridCols;
					offset2 += cols;
				}

				offset = 0;
				offset2 = 0;
				for (v = 0; v < gridRows; v++)
				{
					interpolate(destGrid->xGrid, destGrid->yGrid, offset, splines->yGrid, offset2, cols);
					offset += gridCols;
					offset2 += cols;
				}

				// second pass: warp y 

				for (x = 0; x < cols; x++)
				{
					int i = x;

					for (v = 0; v < gridRows; v++)
					{
						xrow->SetValue(splines->xGrid->GetAt(i), v);
						yrow->SetValue(splines->yGrid->GetAt(i), v);
						i += cols;
					}

					interpolate(xrow, yrow, 0, scale, 0, rows);
					scale->SetValue((float)rows, rows);
					resample(intermediate, outPixels, rows, x, cols, scale);
				}
			}
			catch (...)
			{
			}
		}

	public:
		virtual void interpolate(Array<float> *xKnots, Array<float> *yKnots, int offset, Array<float> *splineY, int splineOffset, int splineLength)
		{
			int index = offset;
			float leftX, rightX;
			float leftY, rightY;

			leftX = xKnots->GetAt(index);
			leftY = yKnots->GetAt(index);
			rightX = xKnots->GetAt(index + 1);
			rightY = yKnots->GetAt(index + 1);

			for (int i = 0; i < splineLength; i++)
			{
				if (i > xKnots->GetAt(index))
				{
					leftX = xKnots->GetAt(index);
					leftY = yKnots->GetAt(index);
					index++;
					rightX = xKnots->GetAt(index);
					rightY = yKnots->GetAt(index);
				}
				float f = (i - leftX) / (rightX - leftX);
				splineY->SetValue(leftY + f * (rightY - leftY), splineOffset + i);
			}
		}
	};
}
