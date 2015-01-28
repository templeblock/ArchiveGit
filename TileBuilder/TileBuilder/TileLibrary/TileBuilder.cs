using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;

namespace TileLibrary
{
	internal class TileBuilder
	{
		private static readonly string _collectionMasterFileFormat = "png";

		private bool _missingTilesOnly;
		private bool _buildCollectionTiles;
		private string _outputFolder;
		private string _outputPath;
		private Size? _commonSize;
		private int _tileSize;
		private string _fileFormat;

		public TileBuilder(bool missingTilesOnly, bool buildCollectionTiles, string outputFolder, Size? commonSize)
		{
			_missingTilesOnly = missingTilesOnly;
			_buildCollectionTiles = buildCollectionTiles;
			_outputFolder = outputFolder;
			_commonSize = commonSize;
		}

		public int UpdateTiles(int imageID, string sourcePath, bool sourceIsFile)
		{
			if (sourceIsFile && !File.Exists(sourcePath))
				return -1;

			ImageProviderBase imageProvider = ImageProvider.Create(sourcePath, sourceIsFile, _commonSize);
			if (imageProvider == null)
				return -1;

			int minLevel = 0;
			int maxLevel = 0;
			if (_buildCollectionTiles)
			{
				minLevel = 0;
				maxLevel = AllTileDefaults.CollectionTileMaxLevel;
				_tileSize = AllTileDefaults.CollectionTileSize;
				_fileFormat = _collectionMasterFileFormat;
				_outputPath = _outputFolder + "_master\\";
			}
			else
			{
				int maxDimension = Math.Max(imageProvider.Size.Width, imageProvider.Size.Height);
				maxLevel = 1;
				while ((maxDimension /= 2) > 1)
					maxLevel++;

				minLevel = AllTileDefaults.CollectionTileMaxLevel + 1;
				_tileSize = AllTileDefaults.DziTileSize;
				_fileFormat = AllTileDefaults.DziTileFormat;
				_outputPath = _outputFolder + string.Format("\\dzi\\{0}_files\\", imageID);
			}

			int count = 0;
			for (int level = minLevel; level <= maxLevel; level++)
				count += (UpdateLevel(imageID, level, maxLevel, imageProvider) ? 1 : 0);

			return count;
		}

		private bool UpdateLevel(int imageID, int level, int maxLevel, ImageProviderBase imageProvider)
		{
			ImageFormat fileFormat = (_fileFormat == "png" ? ImageFormat.Png : ImageFormat.Jpeg);
			int subTileCount = MortonHelpers.LevelToSubTileCount(level, maxLevel);

			try
			{
				if (_buildCollectionTiles)
				{
					int tileX, tileY, subTileX, subTileY;
					MortonHelpers.MortonBreakdown(imageID, level, maxLevel, out tileX, out tileY, out subTileX, out subTileY);
					int subTileSize = _tileSize / subTileCount;

					string tilePath = GetTilePath(_fileFormat, level, tileX, tileY);
					if (_missingTilesOnly && File.Exists(tilePath))
						return false;

					ImageProvider.FolderPrep(tilePath);
					using (FileStream fileStream = new FileStream(tilePath, FileMode.OpenOrCreate, FileAccess.ReadWrite))
					{
						using (CollectionTile tile = new CollectionTile(_tileSize, subTileSize))
						{
							if (fileStream.Length > 0)
							{
								tile.Load(fileStream);
								fileStream.Position = 0;
							}

							tile.Draw(imageProvider, subTileX * subTileSize, subTileY * subTileSize);
							tile.Save(fileStream, fileFormat);

							string publishedTilePath = GetPublishPathFromMasterPath(tilePath);
							if (File.Exists(publishedTilePath))
								File.Delete(publishedTilePath);
						}
					}

					return true;
				}
				else
				{
					int dimension = subTileCount * _tileSize;
					int numTilesX = (imageProvider.Size.Width - 1) / dimension;
					int numTilesY = (imageProvider.Size.Height - 1) / dimension;

					bool ok = false;
					for (int tileX = 0; tileX <= numTilesX; tileX++)
					{
						for (int tileY = 0; tileY <= numTilesY; tileY++)
						{
							string tilePath = GetTilePath(_fileFormat, level, tileX, tileY);
							if (_missingTilesOnly && File.Exists(tilePath))
								continue;

							ImageProvider.FolderPrep(tilePath);
							using (FileStream fileStream = new FileStream(tilePath, FileMode.Create, FileAccess.Write))
							{
								using (DziTile tile = new DziTile(_tileSize))
								{
									tile.Draw(imageProvider, level, tileX, tileY);
									tile.Save(fileStream, fileFormat);
								}
							}

							ok |= true;
						}
					}

					return ok;
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}

			return false;
		}

		private string GetTilePath(string extension, int level, int tileX, int tileY)
		{
			return _outputPath + string.Format("{0}\\{1}_{2}.{3}", level, tileX, tileY, extension);
		}

		public int PublishCollectionTiles()
		{
			string[] tilePaths = GetFilesInFolder(_outputPath + "*.*", true);
			int count = 0;
			foreach (string tilePath in tilePaths)
			{
				try
				{
					string publishedTilePath = GetPublishPathFromMasterPath(tilePath);
					if (File.Exists(publishedTilePath))
						continue;

					ImageProvider.FolderPrep(publishedTilePath);
					using (Image masterImage = Image.FromFile(tilePath))
					{
						masterImage.Save(publishedTilePath, ImageFormat.Jpeg);
						count++;
					}
				}
				catch (Exception ex)
				{
					ex.GetType();
				}
			}

			return count;
		}

		private static string GetPublishPathFromMasterPath(string filePath)
		{
			return filePath.Replace("_master", "_files").Replace(".png", ".jpg");
		}

		// return a list of all the files matching the source wildcards in a folder
		private static string[] GetFilesInFolder(string fileSearchPattern, bool recurse)
		{
			string folder = Path.GetDirectoryName(fileSearchPattern);
			string searchPattern = Path.GetFileName(fileSearchPattern);
			if (folder == string.Empty)
				folder = "."; //use current directory
			SearchOption searchOption = (recurse ? SearchOption.AllDirectories : SearchOption.TopDirectoryOnly);
			return Directory.GetFiles(folder, searchPattern, searchOption);
		}
	}
}
