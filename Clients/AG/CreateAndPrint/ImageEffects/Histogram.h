#pragma once

namespace ImageEffects
{
	/// An image histogram.
	class Histogram
	{
#ifdef NOTUSED
	public:
		virtual property bool Gray
		{
			bool get()
			{
				return isGray_Renamed_Field;
			}
		}
		virtual property int NumSamples
		{
			int get()
			{
				return numSamples;
			}
		}
#endif NOTUSED

	public:
		static const int RED = 0;
		static const int GREEN = 1;
		static const int BLUE = 2;
		static const int GRAY = 3;

	public:
		int histogram[3][256];
		int numSamples;
		int minValue[4];
		int maxValue[4];
		int minFrequency[3];
		int maxFrequency[3];
		float mean[3];
		bool isGray_Renamed_Field;

	public:
		Histogram()
		{
			numSamples = 0;
			isGray_Renamed_Field = true;
		}

		Histogram(Array<int> *pixels, int w, int h, int offset, int stride)
		{
			numSamples = w * h;
			isGray_Renamed_Field = true;

			int index = 0;
			for (int y = 0; y < h; y++)
			{
				index = offset + y * stride;
				for (int x = 0; x < w; x++)
				{
					int rgb = pixels->GetAt(index++);
					int r = (rgb >> 16) & 0xff;
					int g = (rgb >> 8) & 0xff;
					int b = rgb & 0xff;
					histogram[RED][r]++;
					histogram[GREEN][g]++;
					histogram[BLUE][b]++;
				}
			}

			for (int i = 0; i < 256; i++)
			{
				if (histogram[RED][i] != histogram[GREEN][i] || histogram[GREEN][i] != histogram[BLUE][i])
				{
					isGray_Renamed_Field = false;
					break;
				}
			}

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 256; j++)
				{
					if (histogram[i][j] > 0)
					{
						minValue[i] = j;
						break;
					}
				}

				for (int j = 255; j >= 0; j--)
				{
					if (histogram[i][j] > 0)
					{
						maxValue[i] = j;
						break;
					}
				}

				minFrequency[i] = MAXLONG;
				maxFrequency[i] = 0;
				for (int j = 0; j < 256; j++)
				{
					minFrequency[i] = min(minFrequency[i], histogram[i][j]);
					maxFrequency[i] = max(maxFrequency[i], histogram[i][j]);
					mean[i] += (float)(j * histogram[i][j]);
				}
				mean[i] /= (float)(numSamples);
			}
			minValue[GRAY] = min(min(minValue[RED], minValue[GREEN]), minValue[BLUE]);
			maxValue[GRAY] = max(max(maxValue[RED], maxValue[GREEN]), maxValue[BLUE]);
		}

		virtual int getFrequency(int value_Renamed)
		{
			if (numSamples > 0 && isGray_Renamed_Field && value_Renamed >= 0 && value_Renamed <= 255)
				return histogram[0][value_Renamed];
			return - 1;
		}

		virtual int getFrequency(int channel, int value_Renamed)
		{
			if (numSamples < 1 || channel < 0 || channel > 2 || value_Renamed < 0 || value_Renamed > 255)
				return - 1;
			return histogram[channel][value_Renamed];
		}

		virtual int getMinFrequency()
		{
			if (numSamples > 0 && isGray_Renamed_Field)
				return minFrequency[0];
			return - 1;
		}

		virtual int getMinFrequency(int channel)
		{
			if (numSamples < 1 || channel < 0 || channel > 2)
				return - 1;
			return minFrequency[channel];
		}


		virtual int getMaxFrequency()
		{
			if (numSamples > 0 && isGray_Renamed_Field)
				return maxFrequency[0];
			return - 1;
		}

		virtual int getMaxFrequency(int channel)
		{
			if (numSamples < 1 || channel < 0 || channel > 2)
				return - 1;
			return maxFrequency[channel];
		}


		virtual int getMinValue()
		{
			if (numSamples > 0 && isGray_Renamed_Field)
				return minValue[0];
			return - 1;
		}

		virtual int getMinValue(int channel)
		{
			return minValue[channel];
		}

		virtual int getMaxValue()
		{
			if (numSamples > 0 && isGray_Renamed_Field)
				return maxValue[0];
			return - 1;
		}

		virtual int getMaxValue(int channel)
		{
			return maxValue[channel];
		}

		virtual float getMeanValue()
		{
			if (numSamples > 0 && isGray_Renamed_Field)
				return mean[0];
			return - 1.0F;
		}

		virtual float getMeanValue(int channel)
		{
			if (numSamples > 0 && RED <= channel && channel <= BLUE)
				return mean[channel];
			return - 1.0F;
		}
	};
}