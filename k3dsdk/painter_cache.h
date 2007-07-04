#ifndef PAINTER_CACHE_H_
#define PAINTER_CACHE_H_

#include "idocument.h"
#include "log.h"

#include <map>
#include <set>

namespace k3d
{

/// Keep track of cached data for the VBO mesh painters
template<class key_t, class data_t> class painter_cache
{
	/// Collection of data associated with different meshes
	typedef std::map<const key_t, data_t*> data_collection_t;
	typedef std::map<const k3d::idocument*, painter_cache<key_t, data_t> > instances_t;
	/// Mesh <-> painter bindings
	typedef std::set<const k3d::gl::imesh_painter*> painter_set_t;
	typedef std::map<const key_t, painter_set_t> painters_t;

	public:

		/// Get the VBO cache associated with Document
		static painter_cache& instance(const k3d::idocument& Document)
		{
			typename instances_t::iterator instance = m_instances.find(&Document);
			if (instance == m_instances.end())
				instance = (m_instances.insert(std::make_pair(&Document, painter_cache()))).first;
			return instance->second;
		}
		
		/// Debug output in destructor
		~painter_cache()
		{
			if (!m_data.empty())
				k3d::log() << warning << "cache for <" << typeid(key_t).name() << ", " << typeid(data_t).name() << "> still has " << m_data.size() << " entries." << std::endl;
		}

		/// Get the data associated with Key, or NULL if there is none.
		data_t* get_data(const key_t& Key)
		{
			typename data_collection_t::iterator data = m_data.find(Key);
			if (data != m_data.end())
				return data->second;
			return 0;
		}

		/// Create new data for the given key if it doesn't already exist
		data_t* create_data(const key_t& Key)
		{
			typename data_collection_t::iterator data = m_data.find(Key);
			if (data == m_data.end())
			{
				data = m_data.insert(std::make_pair(Key, new data_t())).first;
				k3d::log() << debug << "cache for <" << typeid(&Key).name() << ", " << typeid(data->second).name() << "> now has " << m_data.size() << " entries. Last key: " << Key << std::endl;
			}
			return data->second;
		}

		/// Free the data if any for the given key
		void remove_data(const key_t& Key)
		{
			typename data_collection_t::iterator data = m_data.find(Key);
			if (data != m_data.end())
			{
				delete data->second;
				m_data.erase(data);
			}
		}
		
		/// Change the key of a data item, keeping the data
		void switch_key(const key_t& OldKey, const key_t& NewKey)
		{
			typename data_collection_t::iterator data = m_data.find(OldKey);
			if (data != m_data.end())
			{
				data_t* const value = data->second;
				m_data.erase(data);
				m_data.insert(std::make_pair(NewKey, value));
			}
		}

		/// Register a painter as being associated with Key
		void register_painter(const key_t Key, const k3d::gl::imesh_painter* Painter)
		{
			typename painters_t::iterator painters = m_painters.find(Key);
			if (painters == m_painters.end())
				painters = m_painters.insert(std::make_pair(Key, painter_set_t())).first;
			painters->second.insert(Painter);
		}
		
		/// Remove a painter. Frees GFX card memory when a key has no painters associated.
		void remove_painter(const k3d::gl::imesh_painter* Painter)
		{
			for (typename painters_t::iterator painters = m_painters.begin(); painters != m_painters.end(); ++painters)
			{
				painters->second.erase(Painter);
				if (painters->second.empty())
					remove_data(painters->first);
			}
		}

	private:

		/// The managed data
		data_collection_t m_data;
		/// The painters associated with the keys
		painters_t m_painters;

		painter_cache() {}
		static instances_t m_instances;
};

template<class T1, class T2> std::map<const k3d::idocument*, painter_cache<T1, T2> > painter_cache<T1, T2>::m_instances;

class mesh;

/// Split an operation between a cheap scheduled part and an expensive execution part
class scheduler
{
public:
	scheduler() : m_scheduled(true) {} 
	virtual ~scheduler(){}
	
	/// Schedule the operation to be performed
	void schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_scheduled = true;
		on_schedule(Mesh, Hint);
	}
	
	/// Execute the operation only on the first call after schedule
	void execute(const k3d::mesh& Mesh)
	{
		if (m_scheduled)
		{
			on_execute(Mesh);
		}
		m_scheduled = false;
	}
protected:
	/// Implements the scheduling phase of the operation
	virtual void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint) {}
	
	/// Implements the execution phase of the operation
	virtual void on_execute(const k3d::mesh& Mesh) = 0;
	
	/// Provides implementing classes access to the mesh data
	bool m_scheduled;
};

} // namespace k3d

#endif /*PAINTER_CACHE_H_*/
