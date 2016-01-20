#pragma once

namespace ImageEffects
{
	class MotionBlurOp
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
		virtual property float CentreX
		{
			float get()
			{
				return centreX;
			}

			void set(float value)
			{
				centreX = value;
			}

		}
		virtual property float CentreY
		{
			float get()
			{
				return centreY;
			}

			void set(float value)
			{
				centreY = value;
			}

		}
		virtual property CPointF Centre
		{
			CPointF get()
			{
				return CPointF(centreX, centreY);
			}

			void set(CPointF value)
			{
				centreX = (float)(value::X);
				centreY = (float)(value::Y);
			}
		}
#endif NOTUSED

	private:
		float centreX;
		float centreY;
		float distance;
		float angle;
		float rotation;
		float zoom;

	public:
		MotionBlurOp()
		{
			centreX = 0.5f;
			centreY = 0.5f;
		}

		MotionBlurOp(float newdistance, float newangle, float newrotation, float newzoom)
		{
			distance = newdistance;
			angle = newangle;
			rotation = newrotation;
			zoom = newzoom;
		}

	private:
		int log2(int n)
		{
			int m = 1;
			int log2n = 0;

			while (m < n)
			{
				m *= 2;
				log2n++;
			}
			return log2n;
		}

	public:
		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			if (dst == NULL)
			{
//j				COLORREF tempAux = CLR_NONE;
//j				dst = createCompatibleDestImage(src, tempAux);
			}

			BITMAPINFOHEADER *tsrc = src;
			float cx = (float)(src->biWidth) * centreX;
			float cy = (float)(src->biHeight) * centreY;
			float imageRadius = (float)(sqrt(cx * cx + cy * cy));
			float translateX = (float)(distance * cos(angle));
			float translateY = (float)(distance * (- sin(angle)));
			float scale = zoom;
			float rotate = rotation;
			float maxDistance = distance + abs(rotation * imageRadius) + zoom * imageRadius;
			int steps = log2((int)(maxDistance));

			translateX /= maxDistance;
			translateY /= maxDistance;
			scale /= maxDistance;
			rotate /= maxDistance;

			if (steps == 0)
			{
//j				System::Drawing::Graphics *g = System::Drawing::Graphics::FromImage(dst);
//j				g->Transform = NULL;
//j				g->DrawImage(src, 0, 0);
//j				delete g;
				return dst;
			}

			COLORREF tempAux2 = CLR_NONE;
//j			BITMAPINFOHEADER *tmp = createCompatibleDestImage(src, tempAux2);
			for (int i = 0; i < steps; i++)
			{
/*				System::Drawing::Graphics *g = System::Drawing::Graphics::FromImage(tmp);
				g->Transform = System::Object.Equals(NULL, NULL)?new System::Drawing::Drawing2D::Matrix(1, 0, 0, 1, 0, 0):NULL;
				g->DrawImage(tsrc, 0, 0);
				g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
				g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, (System::Object) System::Drawing::Drawing2D.InterpolationMode.Bilinear);
				g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 0.5f));

				g->TranslateTransform((cx + translateX), (cy + translateY));
				g->ScaleTransform((1.0001 + scale), (1.0001 + scale), System::Drawing::Drawing2D::MatrixOrder::Append); // The.0001 works round a bug on Windows where drawImage throws an ArrayIndexOutofBoundException
				if (rotation != 0)
					g->RotateTransform((float)(RadiansToDegrees(rotate)), System::Drawing::Drawing2D::MatrixOrder::Append);
				g->TranslateTransform((- cx), (- cy));

				g->Transform = System::Object.Equals(NULL, NULL)?new System::Drawing::Drawing2D::Matrix(1, 0, 0, 1, 0, 0):NULL;
				g->DrawImage(dst, 0, 0);
				delete g;
				BITMAPINFOHEADER *ti = dst;
				dst = tmp;
				tmp = ti;
*/				tsrc = dst;

				translateX *= 2;
				translateY *= 2;
				scale *= 2;
				rotate *= 2;
			}
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