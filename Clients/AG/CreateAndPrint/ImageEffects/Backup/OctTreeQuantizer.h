#pragma once
#include "Quantizer.h"

namespace ImageEffects
{
	/// An image Quantizer based on the Octree algorithm. This is a very basic implementation
	/// at present and could be much improved by picking the nodes to reduce more carefully 
	/// (i.e. not completely at random) when I get the time.
	class OctTreeQuantizer : Quantizer
	{
		/// The greatest depth the tree is allowed to reach
	private:
		static const int MAX_LEVEL = 5;

	/// An Octtree node.
	private:
		class OctTreeNode
		{
		public:
			OctTreeNode(OctTreeQuantizer *enclosingInstance)
			{
				InitBlock(enclosingInstance);
			}
		private:
			void InitBlock(OctTreeQuantizer *enclosingInstance)
			{
				enclosingInstance = enclosingInstance;
			}
			OctTreeQuantizer *enclosingInstance;
#ifdef NOTUSED
		public:
			property OctTreeQuantizer *Enclosing_Instance
			{
				OctTreeQuantizer *get()
				{
					return enclosingInstance;
				}
			}
#endif NOTUSED
		private:
			int children;
			int level;
			OctTreeNode *parent;
			OctTreeNode leaf[8];
			bool isLeaf;
			int count;
			int totalRed;
			int totalGreen;
			int totalBlue;
			int index;
		};

	private:
		int nodes;
		OctTreeNode *root;
		int reduceColors;
		int maximumColors;
		int colors;
		Array<System::Collections::ArrayList*> *colorList;

	public:
		OctTreeQuantizer()
		{
			setup(256);
			colorList = new Array<System::Collections::ArrayList*>(MAX_LEVEL + 1);
			for (int i = 0; i < MAX_LEVEL + 1; i++)
				colorList[i] = System::Collections::ArrayList::Synchronized(new System::Collections::ArrayList(10));
			root = new OctTreeNode(this);
		}

		/// Initialize the quantizer. This should be called before adding any pixels.
		/// <param name="numColors">the number of colors we're quantizing to.
		/// </param>
		virtual void setup(int numColors)
		{
			maximumColors = numColors;
			reduceColors = max(512, numColors * 2);
		}

		/// Add pixels to the quantizer.
		/// <param name="pixels">the Array of ARGB pixels
		/// </param>
		/// <param name="offset">the offset into the Array
		/// </param>
		/// <param name="count">the count of pixels
		/// </param>
		virtual void addPixels(Array<int> *pixels, int offset, int count)
		{
			for (int i = 0; i < count; i++)
			{
				insertColor(pixels[i + offset]);
				if (colors > reduceColors)
					reduceTree(reduceColors);
			}
		}

		virtual int getIndexForColor(int rgb)
		{
			int red = (rgb >> 16) & 0xff;
			int green = (rgb >> 8) & 0xff;
			int blue = rgb & 0xff;

			OctTreeNode *node = root;

			for (int level = 0; level <= MAX_LEVEL; level++)
			{
				OctTreeNode *child;
				int bit = 0x80 >> level;

				int index = 0;
				if ((red & bit) != 0)
					index += 4;
				if ((green & bit) != 0)
					index += 2;
				if ((blue & bit) != 0)
					index += 1;

				child = node->leaf[index];

				if (child == NULL)
					return node->index;
				else if (child->isLeaf)
					return child->index;
				else
					node = child;
			}
			return 0;
		}

	private:
		void insertColor(int rgb)
		{
			int red = (rgb >> 16) & 0xff;
			int green = (rgb >> 8) & 0xff;
			int blue = rgb & 0xff;

			OctTreeNode *node = root;

			for (int level = 0; level <= MAX_LEVEL; level++)
			{
				OctTreeNode *child;
				int bit = 0x80 >> level;

				int index = 0;
				if ((red & bit) != 0)
					index += 4;
				if ((green & bit) != 0)
					index += 2;
				if ((blue & bit) != 0)
					index += 1;

				child = node->leaf[index];

				if (child == NULL)
				{
					node->children++;

					child = new OctTreeNode(this);
					child->parent = node;
					node->leaf[index] = child;
					node->isLeaf = false;
					nodes++;
					colorList[level]->Add(child);

					if (level == MAX_LEVEL)
					{
						child->isLeaf = true;
						child->count = 1;
						child->totalRed = red;
						child->totalGreen = green;
						child->totalBlue = blue;
						child->level = level;
						colors++;
						return ;
					}

					node = child;
				}
				else if (child->isLeaf)
				{
					child->count++;
					child->totalRed += red;
					child->totalGreen += green;
					child->totalBlue += blue;
					return ;
				}
				else
					node = child;
			}
		}

		void reduceTree(int numColors)
		{
			for (int level = MAX_LEVEL - 1; level >= 0; level--)
			{
				System::Collections::ArrayList *v = colorList[level];
				if (v != NULL && v->Count > 0)
				{
					for (int j = 0; j < v->Count; j++)
					{
						OctTreeNode *node = (OctTreeNode*)(v[j]);
						if (node->children > 0)
						{
							for (int i = 0; i < 8; i++)
							{
								OctTreeNode *child = node->leaf[i];
								if (child != NULL)
								{
									//if (!child->isLeaf) // not a leaf!
									node->count += child->count;
									node->totalRed += child->totalRed;
									node->totalGreen += child->totalGreen;
									node->totalBlue += child->totalBlue;
									node->leaf[i] = NULL;
									node->children--;
									colors--;
									nodes--;
									colorList[level + 1]->Remove(child);
								}
							}
							node->isLeaf = true;
							colors++;
							if (colors <= numColors)
								return ;
						}
					}
				}
			}
		}

	public:
		virtual Array<int> *buildColorTable()
		{
			Array<int> *table = new Array<int>(colors);
			buildColorTable(root, table, 0);
			return table;
		}

		/// A quick way to use the quantizer. Just create a table the right size and pass in the pixels.
		virtual void buildColorTable(Array<int> *inPixels, Array<int> *table)
		{
			int count = inPixels->Length;
			maximumColors = table->Length;
			for (int i = 0; i < count; i++)
			{
				insertColor(inPixels[i]);
				if (colors > reduceColors)
					reduceTree(reduceColors);
			}
			if (colors > maximumColors)
				reduceTree(maximumColors);
			buildColorTable(root, table, 0);
		}

	private:
		int buildColorTable(OctTreeNode *node, Array<int> *table, int index)
		{
			if (colors > maximumColors)
				reduceTree(maximumColors);

			if (node->isLeaf)
			{
				int count = node->count;
				table[index] = ((int)(0xff000000)) | ((node->totalRed / count) << 16) | ((node->totalGreen / count) << 8) | node->totalBlue / count;
				node->index = index++;
			}
			else
			{
				for (int i = 0; i < 8; i++)
				{
					if (node->leaf[i] != NULL)
					{
						node->index = index;
						index = buildColorTable(node->leaf[i], table, index);
					}
				}
			}
			return index;
		}
	};
}