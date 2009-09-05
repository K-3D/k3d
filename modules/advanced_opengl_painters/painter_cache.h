#ifndef MODULES_REFERENCE_OPENGL_PAINTERS_PAINTER_CACHE_H
#define MODULES_REFERENCE_OPENGL_PAINTERS_PAINTER_CACHE_H

#include <k3dsdk/data.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/log.h>
#include <k3dsdk/pointer_demand_storage.h>
#include <k3dsdk/signal_system.h>

#include <map>
#include <set>

namespace k3d { class mesh; }

namespace module
{

namespace opengl
{

namespace painters
{

/// Stores cached data that can be shared between mesh painters
/**
 * key_t must be a type that allows compare operations
 * data_t must have a constructor that takes a k3d::iproperty::changed_signal_t& argument
 */
template<class key_t, class data_t> class painter_cache
{
	/// Collection of cached data associated with different mesh components
	typedef std::map<key_t, data_t*> data_collection_t;
public:

	/// Get an instance of the global cache
	static painter_cache& instance()
	{
		static painter_cache* g_painter_cache = 0;
		if(!g_painter_cache)
			g_painter_cache = new painter_cache();
		return *g_painter_cache;
	}

	~painter_cache()
	{
		for (typename data_collection_t::iterator data = m_data.begin(); data != m_data.end(); ++data)
			delete data->second;
	}

	/// Get the up-to-date data associated with Key
	/**
	 * @param ChangedSignal: This signal is connected to by the cached data, so it is notified when to update
	 */
	data_t& get_cached_data(const key_t& Key, k3d::iproperty::changed_signal_t& ChangedSignal)
	{
		// Maximum size for the data cache
		static const k3d::uint_t MAX_CACHE_SIZE = 100;
		m_stack.use(Key);
		typename data_collection_t::iterator data = m_data.find(Key);
		if (data == m_data.end())
		{
			// First check if the max cache size would be exceeded, and if so, remove the least recently used item
			if(m_data.size() == MAX_CACHE_SIZE)
				remove_data(m_stack.pop());

			data = m_data.insert(std::make_pair(Key, new data_t(Key, ChangedSignal))).first;
		}
		return *(data->second);
	}

private:
	/// The managed data
	data_collection_t m_data;

	/// Free the data if any for the given key
	void remove_data(key_t Key)
	{
		typename data_collection_t::iterator data = m_data.find(Key);
		if (data != m_data.end())
		{
			delete data->second;
			m_data.erase(data);
		}
	}

	/// A custom stack, where items are unique
	class lru_stack
	{
	public:
		lru_stack() : m_first_item(0), m_last_item(0) {}

		~lru_stack()
		{
			while(m_first_item)
				pop();
		}

		void use(const key_t& Key)
		{
			stackitem* item;
			if(!m_stackitems.count(Key))
			{
				item = new stackitem(Key);
				m_stackitems[Key] = item;
				if(!m_first_item)
					m_first_item = item;
				if(!m_last_item)
					m_last_item = item;
			}
			else
			{
				item = m_stackitems[Key];
			}
			// Move the item to the top of the stack
			if(item != m_first_item)
			{
				if(item->previous)
					item->previous->next = item->next;
				if(item->next)
					item->next->previous = item->previous;
				if(item == m_last_item)
					m_last_item = item->previous;
				item->previous = 0;
				item->next = m_first_item;
				m_first_item = item;
			}
		}

		const key_t pop()
		{
			if(!m_first_item)
				throw std::runtime_error("The painter cache LRU stack was empty!");
			key_t key = m_first_item->key;
			stackitem* item = m_first_item;
			m_first_item = item->next;
			m_first_item->previous = 0;
			m_stackitems.erase(key);
			delete item;
			return key;
		}

	private:
		struct stackitem
		{
			stackitem(const key_t& Key) : previous(0), next(0), key(Key) {}
			stackitem* previous;
			stackitem* next;
			const key_t key;
		};
		std::map<key_t, stackitem*> m_stackitems;
		stackitem* m_first_item;
		stackitem* m_last_item;
	};

	lru_stack m_stack;

	/// Private constructor for singleton pattern
	painter_cache() {}
};

/// Convenience function for easy access to cache data
template<class data_t, class key_t> data_t& get_cached_data(const key_t& Key, k3d::iproperty::changed_signal_t& ChangedSignal)
{
	return painter_cache<key_t, data_t>::instance().get_cached_data(Key, ChangedSignal);
}

/// Boilerplate implementation for hint processing
/**
 * Data that is to be stored in a painter cache can derive from this class.
 */
template<class key_t, class data_t>
class cached_polyhedron_data
{
public:
	cached_polyhedron_data(const key_t Key, k3d::iproperty::changed_signal_t& ChangedSignal) :
		m_key(Key),
		m_data(init_owner(*this) + init_name("data")),
		m_input_mesh(0)
	{
		m_data.set_update_slot(sigc::mem_fun(*this, &cached_polyhedron_data<key_t, data_t>::execute));
		m_changed_connection = ChangedSignal.connect(k3d::hint::converter<
						k3d::hint::convert<k3d::hint::mesh_geometry_changed, k3d::hint::unchanged,
						k3d::hint::convert<k3d::hint::mesh_topology_changed, k3d::hint::unchanged,
						k3d::hint::convert<k3d::hint::selection_changed, k3d::hint::unchanged,
						k3d::hint::convert<k3d::hint::any, k3d::hint::none> > > > >(m_data.make_slot()));
	}

	~cached_polyhedron_data()
	{
		m_changed_connection.disconnect();
	}

	/// Get the up-to-date value of the cache, using InputMesh as input data
	const data_t& value(const k3d::mesh& InputMesh)
	{
		m_input_mesh = &InputMesh;
		return *m_data.internal_value();
	}
protected:

	void execute(const std::vector<k3d::ihint*>& Hints, data_t& Data)
	{
		return_if_fail(m_input_mesh);
		k3d::bool_t topology_changed = false;
		k3d::bool_t selection_changed = false;
		std::vector<k3d::mesh::indices_t> changed_points;

		for(int i = 0; i != Hints.size(); ++i)
		{
			if(k3d::hint::mesh_geometry_changed* hint = dynamic_cast<k3d::hint::mesh_geometry_changed*>(Hints[i]))
			{
				changed_points.push_back(hint->changed_points);
			}
			else if(k3d::hint::selection_changed* hint = dynamic_cast<k3d::hint::selection_changed*>(Hints[i]))
			{
				selection_changed = true;
			}
			else if(k3d::hint::mesh_topology_changed* hint = dynamic_cast<k3d::hint::mesh_topology_changed*>(Hints[i]))
			{
				topology_changed = true;
			}
			else
			{
				selection_changed = true;
				topology_changed = true;
			}
		}
		if(topology_changed)
			on_topology_changed(Data, *m_input_mesh);
		if(selection_changed)
			on_selection_changed(Data, *m_input_mesh);
		for(k3d::uint_t i = 0; i != changed_points.size(); ++i)
			on_geometry_changed(Data, *m_input_mesh, changed_points[i]);
	}

	const key_t m_key;
private:
	virtual void on_topology_changed(data_t& Output, const k3d::mesh& InputMesh) = 0;
	virtual void on_selection_changed(data_t& Output, const k3d::mesh& InputMesh) = 0;
	virtual void on_geometry_changed(data_t& Output, const k3d::mesh& InputMesh, const k3d::mesh::indices_t& ChangedPoints) = 0;

	k3d_data(data_t*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, no_property, no_serialization) m_data;
	const k3d::mesh* m_input_mesh;
	sigc::connection m_changed_connection;
};


} // namespace opengl

} // namespace painters

} // namespace module

#endif // !MODULES_REFERENCE_OPENGL_PAINTERS_PAINTER_CACHE_H
