#ifndef MODULES_OPENGL_PAINTERS_PAINTER_CACHE_H
#define MODULES_OPENGL_PAINTERS_PAINTER_CACHE_H

#include <k3dsdk/hints.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/log.h>
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

/// Keep track of cached data for the VBO mesh painters
template<class data_t, class key_t=const k3d::mesh* const> class painter_cache
{
	/// Collection of data associated with different meshes
	typedef std::map<key_t, data_t*> data_collection_t;
	typedef std::map<const k3d::idocument*, painter_cache<data_t, key_t>* > instances_t;
	/// Mesh <-> painter bindings
	typedef std::set<k3d::inode*> painter_set_t;
	typedef std::map<key_t, painter_set_t> painters_t;

	public:

		/// Get the VBO cache associated with Document
		static painter_cache& instance(k3d::idocument& Document)
		{
			typename instances_t::iterator instance = m_instances.find(&Document);
			if (instance == m_instances.end())
			{
				instance = (m_instances.insert(std::make_pair(&Document, new painter_cache(Document)))).first;
				Document.close_signal().connect(sigc::bind(sigc::ptr_fun(&painter_cache<data_t>::remove_cache), &Document));
			}
			return *(instance->second);
		}
		
		~painter_cache()
		{
			for (typename data_collection_t::iterator data = m_data.begin(); data != m_data.end(); ++data)
				delete data->second;
			for (typename connections_t::iterator connection = m_deleted_connections.begin(); connection != m_deleted_connections.end(); ++connection)
			{
				connection->second.disconnect();
			}
			m_deleted_connections.clear();
		}
		
		/// Get the up-to-date data associated with Key
		data_t& get_data(key_t Key, k3d::inode* Painter)
		{
			typename data_collection_t::iterator data = m_data.find(Key);
			if (data == m_data.end())
			{
				schedule_data(Key, 0, Painter);
			}
			data = m_data.find(Key);
			return_val_if_fail(data != m_data.end(), *(new data_t(Key))); // This should never happen
			return_val_if_fail(data->second, *(new data_t(Key))); // Nor should this
			data->second->execute(*Key, Painter);
			return *(data->second);
		}

		/// Schedule a cache creation/update for the given key
		void schedule_data(key_t Key, k3d::ihint* Hint, k3d::inode* Painter)
		{
			if(dynamic_cast<k3d::hint::mesh_deleted*>(Hint))
			{
				remove_data(Key);
				return;
			}
			typename data_collection_t::iterator data_it = m_data.find(Key);
			if (data_it == m_data.end())
			{
				data_it = m_data.insert(std::make_pair(Key, new data_t(Key))).first;
				register_painter(Key, Painter);
			}
			data_t& data = *data_it->second;
			
			if(!Hint)
			{
				data.schedule(Painter);
			}
			else if(k3d::hint::mesh_geometry_changed* hint = dynamic_cast<k3d::hint::mesh_geometry_changed*>(Hint))
			{
				data.schedule(hint, Painter);
			}
			else if(k3d::hint::selection_changed* hint = dynamic_cast<k3d::hint::selection_changed*>(Hint))
			{
				data.schedule(hint, Painter);
			}
			else if(k3d::hint::mesh_topology_changed* hint = dynamic_cast<k3d::hint::mesh_topology_changed*>(Hint))
			{
				data.schedule(hint, Painter);
			}
			else
			{
				k3d::log() << warning << "Unknown hint " << Hint << " encountered" << std::endl;
				data.schedule(Painter);
			}
		}

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

	private:
		
		static void remove_cache(const k3d::idocument* Document)
		{
			typename instances_t::iterator instance = m_instances.find(Document);
			if (instance != m_instances.end())
			{
				delete instance->second;
			}
			m_instances.erase(Document);
		}
		
		/// Register a painter as being associated with Key
		void register_painter(key_t Key, k3d::inode* Painter)
		{
			typename painters_t::iterator painters = m_painters.find(Key);
			if (painters == m_painters.end())
				painters = m_painters.insert(std::make_pair(Key, painter_set_t())).first;
			painters->second.insert(Painter);
			typename connections_t::iterator connection = m_deleted_connections.find(Painter);
			if (connection == m_deleted_connections.end())
				m_deleted_connections[Painter] = Painter->deleted_signal().connect(sigc::bind(sigc::mem_fun(*this, &painter_cache<data_t>::remove_painter), Painter));
		}
		
		/// Remove a painter. Frees GFX card memory when a key has no painters associated.
		void remove_painter(k3d::inode* Painter)
		{
			for (typename painters_t::iterator painters = m_painters.begin(); painters != m_painters.end(); ++painters)
			{
				painters->second.erase(Painter);
				if (painters->second.empty())
					remove_data(painters->first);
			}
		}

		/// The managed data
		data_collection_t m_data;
		/// The painters associated with the keys
		painters_t m_painters;
		/// The document this cache is associated with
		const k3d::idocument& m_document;
		painter_cache(const k3d::idocument& Document) : m_document(Document) {}
		static instances_t m_instances;
		typedef std::map<const k3d::inode*, sigc::connection> connections_t;
		connections_t m_deleted_connections;
};

template<class T1, class T2> std::map<const k3d::idocument*, painter_cache<T1, T2>* > painter_cache<T1, T2>::m_instances;

/// Convenience function for easy access to cache data
template<class data_t> data_t& get_data(const k3d::mesh* const Key, k3d::inode* Painter)
{
	return painter_cache<data_t>::instance(Painter->document()).get_data(Key, Painter);
}

/// Convenience function to easily schedule cache storage
template<class data_t> void schedule_data(const k3d::mesh* const Key, k3d::ihint* Hint, k3d::inode* Painter)
{
	painter_cache<data_t>::instance(Painter->document()).schedule_data(Key, Hint, Painter);
}

/// Split an operation between a cheap scheduled part and an expensive execution part
class scheduler
{
public:
	scheduler() : m_scheduled(true) {} 
	virtual ~scheduler(){}
	
	/// Schedule the operation to be performed
	void schedule(k3d::inode* Painter)
	{
		m_scheduled = true;
		on_schedule(Painter);
	}
	void schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter)
	{
		m_scheduled = true;
		on_schedule(Hint, Painter);
	}
	void schedule(k3d::hint::selection_changed* Hint, k3d::inode* Painter)
	{
		m_scheduled = true;
		on_schedule(Hint, Painter);
	}
	void schedule(k3d::hint::mesh_topology_changed* Hint, k3d::inode* Painter)
	{
		m_scheduled = true;
		on_schedule(Hint, Painter);
	}
	
	/// Execute the operation only on the first call after schedule
	void execute(const k3d::mesh& Mesh, k3d::inode* Painter)
	{
		if (m_scheduled)
		{
			on_execute(Mesh, Painter);
		}
		m_scheduled = false;
	}
	
protected:
	/// Implements the scheduling phase of the operation
	virtual void on_schedule(k3d::inode* Painter) {}
	virtual void on_schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter) {}
	virtual void on_schedule(k3d::hint::selection_changed* Hint, k3d::inode* Painter) {}
	virtual void on_schedule(k3d::hint::mesh_topology_changed* Hint, k3d::inode* Painter) { on_schedule(Painter); }
	
	/// Implements the execution phase of the operation
	virtual void on_execute(const k3d::mesh& Mesh, k3d::inode* Painter) = 0;
	
	/// Provides implementing classes access to the mesh data
	bool m_scheduled;
};

} // namespace opengl

} // namespace painters

} // namespace module

#endif // !MODULES_OPENGL_PAINTERS_PAINTER_CACHE_H
