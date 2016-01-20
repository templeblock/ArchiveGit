#pragma once
//j#include "AgMatrix.h"

namespace ImageEffects
{
	class MotionBlurFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Angle
		{
			float get()
			{
				return angle;
			}

			void set(float value)
			{
				angle = value;
			}

		}
		virtual property float Distance
		{
			float get()
			{
				return distance;
			}

			void set(float value)
			{
				distance = value;
			}

		}
		virtual property float Rotation
		{
			float get()
			{
				return rotation;
			}

			void set(float value)
			{
				rotation = value;
			}

		}
		virtual property float Zoom
		{
			float get()
			{
				return zoom;
			}

			void set(float value)
			{
				zoom = value;
			}

		}
		virtual property bool WrapEdges
		{
			bool get()
			{
				return wrapEdges;
			}

			void set(bool value)
			{
				wrapEdges = value;
			}

		}
#endif NOTUSED

		static const int LINEAR = 0;
		static const int RADIAL = 1;
		static const int ZOOM = 2;

	private:
		float angle;
		float falloff;
		float distance;
		float zoom;
		float rotation;
		bool wrapEdges;

	public:
		MotionBlurFilter()
		{
			angle = 0.0f;
			falloff = 1.0f;
			distance = 1.0f;
			zoom = 0.0f;
			rotation = 0.0f;
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;

			if (dst == NULL)
			{
//j				COLORREF tempAux = CLR_NONE;
//j				dst = createCompatibleDestImage(src, tempAux);
			}

			Array<int> *inPixels = new Array<int>(width * height);
			Array<int> *outPixels = new Array<int>(width * height);
//j			getRGB(src, 0, 0, width, height, inPixels);

			float sinAngle = (float)sin(angle);
			float cosAngle = (float)cos(angle);
			int cx = width / 2;
			int cy = height / 2;
			int index = 0;

			float imageRadius = (float)sqrt((float)cx * cx + cy * cy);
			float translateX = (float)(distance * cos(angle));
			float translateY = (float)(distance * (- sin(angle)));
			float maxDistance = distance + abs(rotation * imageRadius) + zoom * imageRadius;
			int repetitions = (int)(maxDistance);
//j			CAgMatrix t;
			float pX;
			float pY;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int a = 0, r = 0, g = 0, b = 0;
					int count = 0;
					for (int i = 0; i < repetitions; i++)
					{
						int newX = x, newY = y;
						float f = (float)(i) / repetitions;

						pX = (float)x;
						pY = (float)y;
//j						t.Reset();
//j						t.Translate((cx + f * translateX), (cy + f * translateY));
						float s = 1 - zoom * f;
//j						t.Scale((s), (s));
//j						if (rotation != 0)
//j							t.Rotate((float)((- rotation) * f * (180 / PI)));
//j						t.Translate((- cx), (- cy));
//j						t.TransformPoints(p);
						newX = (int)pX;
						newY = (int)pY;

						if (newX < 0 || newX >= width)
						{
							if (wrapEdges)
								newX = mod(newX, width);
							else
								break;
						}
						if (newY < 0 || newY >= height)
						{
							if (wrapEdges)
								newY = mod(newY, height);
							else
								break;
						}

						count++;
						int rgb = inPixels->GetAt(newY * width + newX);
						a += ((rgb >> 24) & 0xff);
						r += ((rgb >> 16) & 0xff);
						g += ((rgb >> 8) & 0xff);
						b += (rgb & 0xff);
					}
					if (count == 0)
					{
						outPixels->SetValue(inPixels->GetAt(index), index);
					}
					else
					{
						a = PixelUtils::clamp((int)(a / count));
						r = PixelUtils::clamp((int)(r / count));
						g = PixelUtils::clamp((int)(g / count));
						b = PixelUtils::clamp((int)(b / count));
						outPixels->SetValue((a << 24) | (r << 16) | (g << 8) | b, index);
					}
					index++;
				}
			}

//j			setRGB(dst, 0, 0, width, height, outPixels);
			return dst;
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Motion Blur...");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}
