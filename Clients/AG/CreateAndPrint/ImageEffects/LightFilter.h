#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	/*private interface*/ class ElevationMap
	{
		int getHeightAt(int, int);
	};


	class LightFilter : WholeImageFilter
	{
	private:
		void InitBlock()
		{
			normalEvaluator = new NormalEvaluator(this);
		}
	public:
		virtual property Function2D *BumpFunction
		{
			Function2D *get()
			{
				return bumpFunction;
			}

			void set(Function2D *value)
			{
				bumpFunction = value;
			}

		}
		virtual property float BumpHeight
		{
			float get()
			{
				return bumpHeight;
			}

			void set(float value)
			{
				bumpHeight = value;
			}

		}
		virtual property float BumpSoftness
		{
			float get()
			{
				return bumpSoftness;
			}

			void set(float value)
			{
				bumpSoftness = value;
			}

		}
		virtual property float ViewDistance
		{
			float get()
			{
				return viewDistance;
			}

			void set(float value)
			{
				viewDistance = value;
			}

		}
		virtual property int ColorSource
		{
			int get()
			{
				return colorSource;
			}

			void set(int value)
			{
				colorSource = value;
			}

		}
		virtual property int BumpSource
		{
			int get()
			{
				return bumpSource;
			}

			void set(int value)
			{
				bumpSource = value;
			}

		}
		virtual property System::Collections::ArrayList *Lights
		{
			System::Collections::ArrayList *get()
			{
				return lights;
			}

		}

		static const int COLORS_FROM_IMAGE = 0;
		static const int COLORS_CONSTANT = 1;

		static const int BUMPS_FROM_IMAGE = 0;
		static const int BUMPS_FROM_IMAGE_ALPHA = 1;
		static const int BUMPS_FROM_MAP = 2;
		static const int BUMPS_FROM_BEVEL = 3;

	private:
		float bumpHeight;
		float bumpSoftness;
		float viewDistance = 10000.0f;
	private:
		Material *material;
	private:
		System::Collections::ArrayList *lights;
		int colorSource = COLORS_FROM_IMAGE;
		int bumpSource = BUMPS_FROM_IMAGE;
		Function2D *bumpFunction;
		System::Drawing::Image *environmentMap;
		Array<int> *envPixels;
		int envWidth = 1, envHeight = 1;
		Vector3D *l;
		Vector3D *v;
		Vector3D *n;
		ARGB *shadedColor;
		ARGB *diffuse_color;
		ARGB *specular_color;
		Vector3D *tmpv, *tmpv2;
	public:
		NormalEvaluator *normalEvaluator;

		LightFilter()
		{
			InitBlock();
			lights = System::Collections::ArrayList::Synchronized(new System::Collections::ArrayList(10));
			//		addLight(new AmbientLight());
			addLight(new DistantLight(this));
			bumpHeight = 1.0f;
			bumpSoftness = 5.0f;
			material = new Material();
			l = new Vector3D();
			v = new Vector3D();
			n = new Vector3D();
			shadedColor = new ARGB();
			diffuse_color = new ARGB();
			specular_color = new ARGB();
			tmpv = new Vector3D();
			tmpv2 = new Vector3D();
		}

		virtual void setEnvironmentMap(System::Drawing::Image *environmentMap)
		{
			environmentMap = environmentMap;
			if (environmentMap != NULL)
			{
				PixelGrabber *pg = new PixelGrabber(environmentMap, 0, 0, -1, -1, NULL, 0, -1);
				try
				{
					pg->CapturePixels();
				}
				catch (System::Threading::ThreadInterruptedException *e)
				{
					throw new System::SystemException("interrupted waiting for pixels!");
				}
				if ((pg->status() & ImageObserver::ABORT) != 0)
				{
					throw new System::SystemException("image fetch aborted");
				}
				envPixels = (Array<int>*)(pg->Pixels);
				envWidth = pg->Width;
				envHeight = pg->Height;
			}
			else
			{
				envWidth = envHeight = 1;
				envPixels = NULL;
			}
		}

		virtual System::Drawing::Image *getEnvironmentMap()
		{
			return environmentMap;
		}

		virtual void addLight(Light *light)
		{
			lights->Add(light);
		}

		virtual void removeLight(Light *light)
		{
			lights->Remove(light);
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			int index = 0;
			Array<int> *outPixels = new Array<int>(width * height);
			float width45 = abs(6.0f * bumpHeight);
			bool invertBumps = bumpHeight < 0;
			float Nz = 1530.0f / width45;
			Vector3D *position = new Vector3D(0.0f, 0.0f, 0.0f);
			Vector3D *viewpoint = new Vector3D((float)(width) / 2.0f, (float)(height) / 2.0f, viewDistance);
			Vector3D *normal = new Vector3D(0.0f, 0.0f, Nz);
			ARGB *envColor = new ARGB();
			ARGB *diffuseColor = new ARGB(material->diffuseColor);
			ARGB *specularColor = new ARGB(material->specularColor);
			Function2D *bump = bumpFunction;
			if (bumpSource == BUMPS_FROM_IMAGE || bumpSource == BUMPS_FROM_IMAGE_ALPHA || bumpSource == BUMPS_FROM_MAP || bump == NULL)
			{
				if (bumpSoftness != 0)
				{
					int bumpWidth = width;
					int bumpHeight = height;
					Array<int> *bumpPixels = inPixels;
					if (bumpSource == BUMPS_FROM_MAP && dynamic_cast<ImageFunction2D*>(bumpFunction) != NULL)
					{
						ImageFunction2D *if2d = (ImageFunction2D*)(bumpFunction);
						bumpWidth = if2d->Width;
						bumpHeight = if2d->Height;
						bumpPixels = if2d->Pixels;
					}
					Kernel *kernel = GaussianFilter::makeKernel(bumpSoftness);
					Array<int> *tmpPixels = new Array<int>(bumpWidth * bumpHeight);
					Array<int> *softPixels = new Array<int>(bumpWidth * bumpHeight);
					GaussianFilter::convolveAndTranspose(kernel, bumpPixels, tmpPixels, bumpWidth, bumpHeight, true, ConvolveFilter::CLAMP_EDGES);
					GaussianFilter::convolveAndTranspose(kernel, tmpPixels, softPixels, bumpHeight, bumpWidth, true, ConvolveFilter::CLAMP_EDGES);
					bump = new ImageFunction2D(softPixels, bumpWidth, bumpHeight, ImageFunction2D::CLAMP, bumpSource == BUMPS_FROM_IMAGE_ALPHA);
				}
				else
					bump = new ImageFunction2D(inPixels, width, height, ImageFunction2D::CLAMP, bumpSource == BUMPS_FROM_IMAGE_ALPHA);
			}

			float reflectivity = material->reflectivity;
			float areflectivity = (1 - reflectivity);
			Vector3D *v1 = new Vector3D();
			Vector3D *v2 = new Vector3D();
			Vector3D *n = new Vector3D();
			Array<Light*> *lightsArray = new Array<Light*>(lights->Count);
			lights->CopyTo(lightsArray);
			for (int i = 0; i < lightsArray->Length; i++)
				lightsArray[i]->prepare(width, height);

			// Loop through each source pixel
			for (int y = 0; y < height; y++)
			{
				float ny = y;
				position->y = y;
				for (int x = 0; x < width; x++)
				{
					float nx = x;

					// Calculate the normal at this point
					if (bumpSource != BUMPS_FROM_BEVEL)
					{
						// Complicated and slower method
						// Calculate four normals using the gradients in +/- X/Y directions
						int count = 0;
						normal->x = normal->y = normal->z = 0;
						float m0 = width45 * bump->evaluate(nx, ny);
						float m1 = x > 0?width45 * bump->evaluate(nx - 1.0f, ny) - m0:- 2;
						float m2 = y > 0?width45 * bump->evaluate(nx, ny - 1.0f) - m0:- 2;
						float m3 = x < width - 1?width45 * bump->evaluate(nx + 1.0f, ny) - m0:- 2;
						float m4 = y < height - 1?width45 * bump->evaluate(nx, ny + 1.0f) - m0:- 2;

						if (m1 != - 2 && m4 != - 2)
						{
							v1->x = - 1.0f;
							v1->y = 0.0f;
							v1->z = m1;
							v2->x = 0.0f;
							v2->y = 1.0f;
							v2->z = m4;
							v1->crossProduct(v2, n);
							n->normalize();
							if (n->z < 0.0)
								n->z = - n->z;
							normal->add(n);
							count++;
						}

						if (m1 != - 2 && m2 != - 2)
						{
							v1->x = - 1.0f;
							v1->y = 0.0f;
							v1->z = m1;
							v2->x = 0.0f;
							v2->y = - 1.0f;
							v2->z = m2;
							v1->crossProduct(v2, n);
							n->normalize();
							if (n->z < 0.0)
								n->z = - n->z;
							normal->add(n);
							count++;
						}

						if (m2 != - 2 && m3 != - 2)
						{
							v1->x = 0.0f;
							v1->y = - 1.0f;
							v1->z = m2;
							v2->x = 1.0f;
							v2->y = 0.0f;
							v2->z = m3;
							v1->crossProduct(v2, n);
							n->normalize();
							if (n->z < 0.0)
								n->z = - n->z;
							normal->add(n);
							count++;
						}

						if (m3 != - 2 && m4 != - 2)
						{
							v1->x = 1.0f;
							v1->y = 0.0f;
							v1->z = m3;
							v2->x = 0.0f;
							v2->y = 1.0f;
							v2->z = m4;
							v1->crossProduct(v2, n);
							n->normalize();
							if (n->z < 0.0)
								n->z = - n->z;
							normal->add(n);
							count++;
						}

						// Average the four normals
						normal->x /= count;
						normal->y /= count;
						normal->z /= count;
					}
					else
					{
						if (normalEvaluator != NULL)
							normalEvaluator->getNormalAt(x, y, width, height, normal);
					}
					if (invertBumps)
					{
						normal->x = - normal->x;
						normal->y = - normal->y;
					}
					position->x = x;

					if (normal->z >= 0)
					{
						// Get the material colour at this point
						if (colorSource == COLORS_FROM_IMAGE)
							diffuseColor->setColor(inPixels[index]);
						else
							diffuseColor->setColor(material->diffuseColor);
						if (reflectivity != 0 && environmentMap != NULL)
						{
							//FIXME-too much normalizing going on here
							tmpv2->set_Renamed(viewpoint);
							tmpv2->z = 100.0f; //FIXME
							tmpv2->subtract(position);
							tmpv2->normalize();
							tmpv->set_Renamed(normal);
							tmpv->normalize();
							tmpv->reflect(tmpv2);
							tmpv->normalize();
							envColor->setColor(getEnvironmentMap(tmpv, inPixels, width, height));
							diffuseColor->r = reflectivity * envColor->r + areflectivity * diffuseColor->r;
							diffuseColor->g = reflectivity * envColor->g + areflectivity * diffuseColor->g;
							diffuseColor->b = reflectivity * envColor->b + areflectivity * diffuseColor->b;
						}
						// Shade the pixel
						ARGB *c = phongShade(position, viewpoint, normal, diffuseColor, specularColor, material, lightsArray);
						int alpha = inPixels[index] & ((int)(0xff000000));
						int rgb = c->argbValue() & 0x00ffffff;
						outPixels[index++] = alpha | rgb;
					}
					else
						outPixels[index++] = 0;
				}
			}
			return outPixels;
		}

	public:
		virtual ARGB *phongShade(Vector3D *position, Vector3D *viewpoint, Vector3D *normal, ARGB *diffuseColor, ARGB *specularColor, Material *material, Array<Light*> *lightsArray)
		{
			shadedColor->setColor(diffuseColor);
			shadedColor->multiply(material->ambientIntensity);

			for (int i = 0; i < lightsArray->Length; i++)
			{
				Light *light = lightsArray[i];
				//FIXME-normalize outside loop
				n->set_Renamed(normal);
				n->normalize();
				l->set_Renamed(light->position);
				if (light->type != DISTANT)
					l->subtract(position);
				l->normalize();
				float nDotL = n->innerProduct(l);
				if (nDotL >= 0.0)
				{
					float dDotL = 0;

					v->set_Renamed(viewpoint);
					v->subtract(position);
					v->normalize();

					// Spotlight
					if (light->type == SPOT)
					{
						dDotL = light->direction->innerProduct(l);
						if (dDotL < light->cosConeAngle)
							continue;
					}

					n->multiply(2.0f * nDotL);
					n->subtract(l);
					float rDotV = n->innerProduct(v);

					float rv;
					if (rDotV < 0.0)
						rv = 0.0f;
					else
					{
						rv = (float)(pow(rDotV, material->highlight));
					}

					// Spotlight
					if (light->type == SPOT)
					{
						dDotL = light->cosConeAngle / dDotL;
						float e = dDotL;
						e *= e;
						e *= e;
						e *= e;
						e = (float)(pow(dDotL, light->focus * 10)) * (1 - e);
						rv *= e;
						nDotL *= e;
					}

					diffuse_color->setColor(diffuseColor);
					diffuse_color->multiply(material->diffuseReflectivity);
					diffuse_color->multiply(light->realColor);
					diffuse_color->multiply(nDotL);
					specular_color->setColor(specularColor);
					specular_color->multiply(material->specularReflectivity);
					specular_color->multiply(light->realColor);
					specular_color->multiply(rv);
					diffuse_color->add(specular_color);
					diffuse_color->clamp();
					shadedColor->add(diffuse_color);
				}
			}
			shadedColor->clamp();
			return shadedColor;
		}

	private:
		Array<int> *rgb = new Array<int>(4);

		int getEnvironmentMap(Vector3D *normal, Array<int> *inPixels, int width, int height)
		{
			if (environmentMap != NULL)
			{
				float angle = (float)(acos(- normal->y));

				float x, y;
				y = angle / PI;

				if (y == 0.0f || y == 1.0f)
					x = 0.0f;
				else
				{
					float f = normal->x / (float)(sin(angle));

					if (f > 1.0f)
						f = 1.0f;
					else if (f < - 1.0f)
						f = - 1.0f;

					x = (float)(acos(f)) / PI;
				}
				// A bit of emprirical scaling....
				//			x = (x-0.5f)*1.2f+0.5f;
				//			y = (y-0.5f)*1.2f+0.5f;
				x = clamp(x * envWidth, 0, envWidth - 1);
				y = clamp(y * envHeight, 0, envHeight - 1);
				int ix = (int)(x);
				int iy = (int)(y);

				float xWeight = x - ix;
				float yWeight = y - iy;
				int i = envWidth * iy + ix;
				int dx = ix == envWidth - 1?0:1;
				int dy = iy == envHeight - 1?0:envWidth;
				rgb[0] = envPixels[i];
				rgb[1] = envPixels[i + dx];
				rgb[2] = envPixels[i + dy];
				rgb[3] = envPixels[i + dx + dy];
				return bilinearInterpolate(xWeight, yWeight, rgb);
			}
			return 0;
		}

	public:
	
		ref class NormalEvaluator
		{
		public:
			NormalEvaluator(LightFilter *enclosingInstance)
			{
				InitBlock(enclosingInstance);
			}
		private:
			void InitBlock(LightFilter *enclosingInstance)
			{
				enclosingInstance = enclosingInstance;
				shape = RECTANGLE;
				bevel = LINEAR;
			}
			LightFilter *enclosingInstance;
		public:
			virtual property int Margin
			{
				int get()
				{
					return margin;
				}

				void set(int value)
				{
					margin = value;
				}

			}
			virtual property int CornerRadius
			{
				int get()
				{
					return cornerRadius;
				}

				void set(int value)
				{
					cornerRadius = value;
				}

			}
			virtual property int Shape
			{
				int get()
				{
					return shape;
				}

				void set(int value)
				{
					shape = value;
				}

			}
			virtual property int Bevel
			{
				int get()
				{
					return bevel;
				}

				void set(int value)
				{
					bevel = value;
				}

			}
			property LightFilter *Enclosing_Instance
			{
				LightFilter *get()
				{
					return enclosingInstance;
				}

			}
			static const int RECTANGLE = 0;
			static const int ROUNDRECT = 1;
			static const int ELLIPSE = 2;

			static const int LINEAR = 0;
			static const int SIN = 1;
			static const int CIRCLE_UP = 2;
			static const int CIRCLE_DOWN = 3;
			static const int SMOOTH = 4;
			static const int PULSE = 5;
			static const int SMOOTH_PULSE = 6;
			static const int THING = 7;

		private:
			int margin = 10;
			int shape;
			int bevel;
			int cornerRadius = 15;

		public:
			virtual void getNormalAt(int x, int y, int width, int height, Vector3D *normal)
			{
				float distance = 0;
				normal->x = normal->y = 0;
				normal->z = 0.707f;
				switch (shape)
				{

					case RECTANGLE:
						if (x < margin)
						{
							if (x < y && x < height - y)
								normal->x = - 1;
						}
						else if (width - x <= margin)
						{
							if (width - x - 1 < y && width - x <= height - y)
								normal->x = 1;
						}
						if (normal->x == 0)
						{
							if (y < margin)
							{
								normal->y = - 1;
							}
							else if (height - y <= margin)
								normal->y = 1;
						}
						distance = min(min(x, y), min(width - x - 1, height - y - 1));
						break;

					case ELLIPSE:
						float a = width / 2;
						float b = height / 2;
						float a2 = a * a;
						float b2 = b * b;
						float dx = x - a;
						float dy = y - b;
						float x2 = dx * dx;
						float y2 = dy * dy;
						distance = (b2 - (b2 * x2) / a2) - y2;
						float radius = (float)(sqrt(x2 + y2));
						distance = 0.5f * distance / ((a + b) / 2); //FIXME
						if (radius != 0)
						{
							normal->x = dx / radius;
							normal->y = dy / radius;
						}
						break;

					case ROUNDRECT:
						distance = min(min(x, y), min(width - x - 1, height - y - 1));
						float c = min(cornerRadius, min(width / 2, height / 2));
						if ((x < c || width - x <= c) && (y < c || height - y <= c))
						{
							if (width - x <= c)
								x = (int)(x - (width - c - c - 1));
							if (height - y <= c)
								y = (int)(y - (height - c - c - 1));
							dx = x - c;
							dy = y - c;
							x2 = dx * dx;
							y2 = dy * dy;
							radius = (float)(sqrt(x2 + y2));
							distance = c - radius;
							normal->x = dx / radius;
							normal->y = dy / radius;
						}
						else if (x < margin)
						{
							normal->x = - 1;
						}
						else if (width - x <= margin)
						{
							normal->x = 1;
						}
						else if (y < margin)
						{
							normal->y = - 1;
						}
						else if (height - y <= margin)
							normal->y = 1;
						break;
					}
				distance /= margin;
				if (distance < 0)
				{
					normal->z = - 1;
					normal->normalize();
					return ;
				}

				float dx2 = 1.0f / margin;
				float z1 = bevelFunction(distance);
				float z2 = bevelFunction(distance + dx2);
				float dz = z2 - z1;
				normal->z = dx2;
				normal->x *= dz;
				normal->y *= dz;
				normal->normalize();
			}

		private:
			float bevelFunction(float x)
			{
				x = clamp(x, 0.0f, 1.0f);
				switch (bevel)
				{

					case LINEAR:
						return clamp(x, 0.0f, 1.0f);

					case SIN:
						return (float)(sin(x * PI / 2));

					case CIRCLE_UP:
						return circleUp(x);

					case CIRCLE_DOWN:
						return circleDown(x);

					case SMOOTH:
						return smoothStep(0.1f, 0.9f, x);

					case PULSE:
						return pulse(0.0f, 1.0f, x);

					case SMOOTH_PULSE:
						return smoothPulse(0.0f, 0.1f, 0.5f, 1.0f, x);

					case THING:
						return (float)(x < 0.2?sin(x / 0.2 * PI / 2):0.5 + 0.5 * sin(1 + x / 0.6 * PI / 2));
					}
				return x;
			}
		};

	public:
		virtual CString *ToString()
		{
			return new CString("Stylize/Light Effects...");
		}

	public:
	
		ref class ARGB
		{
		public:
			float a;
			float r;
			float g;
			float b;

			ARGB() : this(0, 0, 0, 0)
			{
			}

			ARGB(int a, int r, int g, int b)
			{
				a = (float)(a) / 255.0f;
				r = (float)(r) / 255.0f;
				g = (float)(g) / 255.0f;
				b = (float)(b) / 255.0f;
			}

			ARGB(float a, float r, float g, float b)
			{
				a = a;
				r = r;
				g = g;
				b = b;
			}

			ARGB(ARGB *v)
			{
				a = v->a;
				r = v->r;
				g = v->g;
				b = v->b;
			}

			ARGB(int v) : this(v >> 24 & 255, v >> 16 & 255, v >> 8 & 255, v & 255)
			{
			}

			ARGB(COLORREF& c) : this(c.ToArgb())
			{
			}

			virtual void setColor(int a, int r, int g, int b)
			{
				a = (float)(a) / 255.0f;
				r = (float)(r) / 255.0f;
				g = (float)(g) / 255.0f;
				b = (float)(b) / 255.0f;
			}

			virtual void setColor(float a, float r, float g, float b)
			{
				a = a;
				r = r;
				g = g;
				b = b;
			}

			virtual void setColor(ARGB *v)
			{
				a = v->a;
				r = v->r;
				g = v->g;
				b = v->b;
			}

			virtual void setColor(int v)
			{
				setColor(v >> 24 & 255, v >> 16 & 255, v >> 8 & 255, v & 255);
			}

			virtual int argbValue()
			{
				a = 1.0f;
				int ia = (int)(255.0 * a);
				int ir = (int)(255.0 * r);
				int ig = (int)(255.0 * g);
				int ib = (int)(255.0 * b);
				return ia << 24 | ir << 16 | ig << 8 | ib;
			}

			virtual float length()
			{
				return (float)(sqrt(r * r + g * g + b * b));
			}

			virtual void normalize()
			{
				float l = length();
				if (l != 0.0)
					multiply(1.0f / l);
			}

			virtual void clamp()
			{
				if (a < 0.0f)
					a = 0.0f;
				else if (a > 1.0f)
					a = 1.0f;
				if (r < 0.0f)
					r = 0.0f;
				else if (r > 1.0f)
					r = 1.0f;
				if (g < 0.0)
					g = 0.0f;
				else if (g > 1.0)
					g = 1.0f;
				if (b < 0.0f)
					b = 0.0f;
				else if (b > 1.0f)
					b = 1.0f;
			}

			virtual void multiply(float f)
			{
				r *= f;
				g *= f;
				b *= f;
			}

			virtual void add(ARGB *v)
			{
				r += v->r;
				g += v->g;
				b += v->b;
			}

			virtual void subtract(ARGB *v)
			{
				r -= v->r;
				g -= v->g;
				b -= v->b;
			}

			virtual void multiply(ARGB *v)
			{
				r *= v->r;
				g *= v->g;
				b *= v->b;
			}

			virtual CString *ToString()
			{
				return a + " " + r + " " + g + " " + b + ")";
			}
		};

	public:
	
		ref class Material
		{
		public:
			virtual property int DiffuseColor
			{
				int get()
				{
					return diffuseColor;
				}

				void set(int value)
				{
					diffuseColor = value;
				}

			}
		private:
			int diffuseColor;
			int specularColor;
			float ambientIntensity;
			float diffuseReflectivity;
			float specularReflectivity;
			float highlight;
			float reflectivity;

		public:
			Material()
			{
				ambientIntensity = 0.5f;
				diffuseReflectivity = 1.0f;
				specularReflectivity = 1.0f;
				highlight = 3.0f;
				reflectivity = 0.0f;
				diffuseColor = ((int)(0xff888888));
				specularColor = ((int)(0xffffffff));
			}
		};

	public:
		static const int AMBIENT = 0;
		static const int DISTANT = 1;
		static const int POINT = 2;
		static const int SPOT = 3;

	public:
		ref class Light
		{
		private:
			void InitBlock()
			{
				type = ImageEffects::LightFilter::AMBIENT;
				coneAngle = PI / 6;
			}
		public:
			virtual property float Azimuth
			{
				float get()
				{
					return azimuth;
				}

				void set(float value)
				{
					azimuth = value;
				}

			}
			virtual property float Elevation
			{
				float get()
				{
					return elevation;
				}

				void set(float value)
				{
					elevation = value;
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
			virtual property float Intensity
			{
				float get()
				{
					return intensity;
				}

				void set(float value)
				{
					intensity = value;
				}

			}
			virtual property float ConeAngle
			{
				float get()
				{
					return coneAngle;
				}

				void set(float value)
				{
					coneAngle = value;
				}

			}
			virtual property float Focus
			{
				float get()
				{
					return focus;
				}

				void set(float value)
				{
					focus = value;
				}

			}
			virtual property int Color
			{
				int get()
				{
					return color;
				}

				void set(int value)
				{
					color = value;
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

		private:
			int type;
			Vector3D *position;
			Vector3D *direction;
			ARGB *realColor;
			int color = ((int)(0xffffffff));
			float intensity;
			float azimuth;
			float elevation;
			float focus = 0.5f;
			float centreX = 0.5f, centreY = 0.5f;
			float coneAngle;
			float cosConeAngle;
			float distance = 100.0f;

		public:
			Light() : this(135 * ImageMath.PI / 180.0f, 0.5235987755982988f, 1.0f)
			{
			}

			Light(float azimuth, float elevation, float intensity)
			{
				azimuth = azimuth;
				elevation = elevation;
				intensity = intensity;
			}

			virtual void prepare(int width, int height)
			{
				float lx = (float)(cos(azimuth) * cos(elevation));
				float ly = (float)(sin(azimuth) * cos(elevation));
				float lz = (float)(sin(elevation));
				direction = new Vector3D(lx, ly, lz);
				direction->normalize();
				if (type != ImageEffects::LightFilter::DISTANT)
				{
					lx *= distance;
					ly *= distance;
					lz *= distance;
					lx += width * centreX;
					ly += height * (1 - centreY);
				}
				position = new Vector3D(lx, ly, lz);
				realColor = new ARGB(color);
				realColor->multiply(intensity);
				cosConeAngle = (float)(cos(coneAngle));
			}

			virtual System::Object *Clone()
			{
				try
				{
					Light *copy = (Light*)(__super::MemberwiseClone());
					return copy;
				}
				catch (...)
				{
					return NULL;
				}
			}

			virtual CString *ToString()
			{
				return new CString("Light";
			}
		};

	public:
		ref class AmbientLight : Light
		{
		public:
			AmbientLight(LightFilter *enclosingInstance)
			{
				InitBlock(enclosingInstance);
			}
		private:
			void InitBlock(LightFilter *enclosingInstance)
			{
				enclosingInstance = enclosingInstance;
			}
			LightFilter *enclosingInstance;
		public:
			property LightFilter *Enclosing_Instance
			{
				LightFilter *get()
				{
					return enclosingInstance;
				}

			}
			virtual CString *ToString()
			{
				return new CString("Ambient Light");
			}
		};

	public:
		ref class PointLight : Light
		{
		private:
			void InitBlock(LightFilter *enclosingInstance)
			{
				enclosingInstance = enclosingInstance;
			}
			LightFilter *enclosingInstance;
		public:
			property LightFilter *Enclosing_Instance
			{
				LightFilter *get()
				{
					return enclosingInstance;
				}

			}
			PointLight(LightFilter *enclosingInstance)
			{
				InitBlock(enclosingInstance);
				type = ImageEffects::LightFilter::POINT;
			}

			virtual CString *ToString()
			{
				return new CString("Point Light");
			}
		};

	public:
		ref class DistantLight : Light
		{
		private:
			void InitBlock(LightFilter *enclosingInstance)
			{
				enclosingInstance = enclosingInstance;
			}
			LightFilter *enclosingInstance;
		public:
			property LightFilter *Enclosing_Instance
			{
				LightFilter *get()
				{
					return enclosingInstance;
				}

			}
			DistantLight(LightFilter *enclosingInstance)
			{
				InitBlock(enclosingInstance);
				type = ImageEffects::LightFilter::DISTANT;
			}

			virtual CString *ToString()
			{
				return new CString("Distant Light");
			}
		};

	public:
		ref class SpotLight : Light
		{
		private:
			void InitBlock(LightFilter *enclosingInstance)
			{
				enclosingInstance = enclosingInstance;
			}
			LightFilter *enclosingInstance;
		public:
			property LightFilter *Enclosing_Instance
			{
				LightFilter *get()
				{
					return enclosingInstance;
				}

			}
			SpotLight(LightFilter *enclosingInstance)
			{
				InitBlock(enclosingInstance);
				type = ImageEffects::LightFilter::SPOT;
			}

			virtual CString *ToString()
			{
				return new CString("Spotlight");
			}
		};
	};
}
