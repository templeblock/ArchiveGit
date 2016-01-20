#pragma once

namespace ImageEffects
{
	class ShadowFilter
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
		/// Get the radius of the kernel.
		/// <returns> the radius
		/// </returns>
		/// Set the radius of the kernel, and hence the amount of blur. The bigger the radius, the longer this filter will take.
		/// <param name="radius">the radius of the blur in pixels.
		/// </param>
		virtual property float Radius
		{
			float get()
			{
				return radius;
			}

			void set(float value)
			{
				radius = value;
			}

		}
		virtual property float Opacity
		{
			float get()
			{
				return opacity;
			}

			void set(float value)
			{
				opacity = value;
			}

		}
		virtual property int ShadowColor
		{
			int get()
			{
				return shadowColor;
			}

			void set(int value)
			{
				shadowColor = value;
			}

		}
		virtual property bool AddMargins
		{
			bool get()
			{
				return addMargins;
			}

			void set(bool value)
			{
				addMargins = value;
			}

		}
		virtual property bool ShadowOnly
		{
			bool get()
			{
				return shadowOnly;
			}

			void set(bool value)
			{
				shadowOnly = value;
			}

		}
#endif NOTUSED

	private:
		float radius;
		float angle;
		float distance;
		float opacity;
		bool addMargins;
		bool shadowOnly;
		int shadowColor;

	public:
		ShadowFilter()
		{
			radius = 5;
			angle = PI * 6 / 4;
			distance = 5;
			opacity = 0.5f;
			shadowColor = (int)(0xff000000);
		}

		ShadowFilter(float newradius, float xOffset, float yOffset, float newopacity)
		{
			radius = newradius;
			angle = (float)(atan2(yOffset, xOffset));
			distance = (float)(sqrt(xOffset * xOffset + yOffset * yOffset));
			opacity = newopacity;
		}

	public:
		virtual void transformSpace(CRect& r)
		{
			if (addMargins)
			{
				float xOffset = distance * (float)(cos(angle));
				float yOffset = (- distance) * (float)(sin(angle));
				r.right += (int)(abs(xOffset) + 2 * radius);
				r.bottom += (int)(abs(yOffset) + 2 * radius);
			}
		}

	public:
		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;

			if (dst == NULL)
			{
				if (addMargins)
				{
//j					COLORREF dstCM;
//j					dstCM = src.getColorModel();
//j					dst = new BufferedImage(dstCM, src->biWidth, src->biHeight);
				}
				else
				{
//j					COLORREF tempAux = CLR_NONE;
//j					dst = createCompatibleDestImage(src, tempAux);
				}
			}

/*			float shadowR = ((shadowColor >> 16) & 0xff) / 255f;
			float shadowG = ((shadowColor >> 8) & 0xff) / 255f;
			float shadowB = (shadowColor & 0xff) / 255f;

			// Make a black mask from the image's alpha channel 
			Array<Array<float>*> *extractAlpha = new Array<Array<float>*>{new Array<float>{0, 0, 0, shadowR}, new Array<float>{0, 0, 0, shadowG}, new Array<float>{0, 0, 0, shadowB}, new Array<float>{0, 0, 0, opacity}};
			BITMAPINFOHEADER *shadow = new BITMAPINFOHEADER(width, height, (System::Drawing::Imaging::PixelFormat)(System::Drawing::Imaging::PixelFormat::Format32bppArgb));
			new BandCombineOp(extractAlpha, NULL)->filter(src, shadow);
			shadow = new GaussianFilter(radius)->filter(shadow, NULL);

			float xOffset = distance * (float)(cos(angle));
			float yOffset = (- distance) * (float)(sin(angle));

			System::Drawing::Graphics *g = System::Drawing::Graphics::FromImage(dst);
//j			g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, opacity));
			if (addMargins)
			{
				float radius2 = radius / 2;
				float topShadow = max(0, radius - yOffset);
				float leftShadow = max(0, radius - xOffset);
				g->TranslateTransform((topShadow), (leftShadow));
			}
			System::Drawing::Drawing2D::Matrix *temp_Matrix;
			temp_Matrix = new System::Drawing::Drawing2D::Matrix();
			temp_Matrix->Translate((float)(xOffset), (float)(yOffset));
			g->Transform = temp_Matrix;
			g->DrawImage(shadow, 0, 0);
			if (!shadowOnly)
			{
//j				g.setComposite(AlphaComposite.SrcOver);
				g->Transform = NULL;
				g->DrawImage(src, 0, 0);
			}
			delete g;
*/
			return dst;
		}

		virtual CString *ToString()
		{
			return new CString("Stylize/Drop Shadow...");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}
