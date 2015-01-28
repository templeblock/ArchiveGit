using System.Collections;
using System.Collections.Generic;

namespace PivotServerTools.Internal
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

		public void Add(CollectionFactoryBase collectionFactory)
		{
			_factories.Add(collectionFactory);
		}

		public void AddFromFolder(string folderPath)
		{
			IEnumerable<CollectionFactoryBase> factories = FactoryClassFinder.Find(folderPath);
			foreach (var f in factories)
				_factories.Add(f);
		}

		//Note, if a given factory is not already loaded, this method does not attempt to find an
		// assembly containing new factories.
		public CollectionFactoryBase Get(string factoryKey)
		{
			return _factories.TryGet(factoryKey);
		}

		public IEnumerable<CollectionFactoryBase> EnumerateFactories()
		{
			return _factories;
		}

		// A dictionary of CollectionFactoryBase objects, indexed by their factoryKey
		private class FactoryBaseCollection : SynchronizedKeyedCollection<string, CollectionFactoryBase>
		{
			public CollectionFactoryBase TryGet(string factoryKey)
			{
				if (factoryKey == null)
					return null;

				CollectionFactoryBase output;
				if (base.Dictionary != null)
					return base.Dictionary.TryGetValue(factoryKey.ToLowerInvariant(), out output) ? output : null;

				return base.Items.Find(item => (0 == string.Compare(factoryKey, item.Name, true)));
			}

			protected override string GetKeyForItem(CollectionFactoryBase item)
			{
				return item.Name.ToLowerInvariant();
			}
		}
	}
}
