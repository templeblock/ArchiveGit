using System.Collections;
using System.Collections.Generic;

namespace FactoriesLibrary
{
	// A dictionary of classes that implement CollectionFactoryBase, for finding
	// which factory object to use for a particular CXML request
	internal class CollectionFactories
	{
		FactoryBaseCollection _factories = new FactoryBaseCollection();

		public int Count { get { return _factories.Count; } }

		public CollectionFactories()
		{
		}

		public void Add(CollectionFactoryBase factory)
		{
			_factories.Add(factory);
		}

		public void AddFromFolder(string folderPath)
		{
			IEnumerable<CollectionFactoryBase> factories = FactoryClassFinder.Find(folderPath);
			foreach (CollectionFactoryBase factory in factories)
				_factories.Add(factory);
		}

		//Note, if a given factory is not already loaded, this method does not attempt to find an
		// assembly containing new factories.
		public CollectionFactoryBase Get(string factoryName)
		{
			return _factories.TryGet(factoryName);
		}

		public IEnumerable<CollectionFactoryBase> EnumerateFactories()
		{
			return _factories;
		}

		// A dictionary of CollectionFactoryBase objects, indexed by their factoryName
		private class FactoryBaseCollection : SynchronizedKeyedCollection<string, CollectionFactoryBase>
		{
			public CollectionFactoryBase TryGet(string factoryName)
			{
				if (factoryName == null)
					return null;

				CollectionFactoryBase output;
				if (base.Dictionary != null)
					return base.Dictionary.TryGetValue(factoryName.ToLowerInvariant(), out output) ? output : null;

				return base.Items.Find(item => (0 == string.Compare(factoryName, item.FactoryName, true)));
			}

			protected override string GetKeyForItem(CollectionFactoryBase item)
			{
				return item.FactoryName.ToLowerInvariant();
			}
		}
	}
}
