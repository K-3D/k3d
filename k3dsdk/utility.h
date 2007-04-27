#ifndef K3DSDK_UTILITY_H
#define K3DSDK_UTILITY_H

namespace k3d
{

template<typename container_t>
struct inserter_t
{
	explicit inserter_t(container_t& Container) : container(Container) {}
	
	void operator()(typename container_t::value_type Value)
	{
		container.insert(container.end(), Value);
	}
	
	container_t& container;
};

template<typename container_t>
inserter_t<container_t> inserter(container_t& Container)
{
	return inserter_t<container_t>(Container);
}

/// Convenience function that returns a collection containing a single item
template<typename container_t, typename value_t>
const container_t make_collection(const value_t& Value)
{
	container_t result;
	result.insert(result.end(), Value);
	
	return result;
}

/// copy_if() - as described in "Effective STL" ...
template<typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator copy_if(InputIterator Begin, InputIterator End, OutputIterator DestBegin, Predicate P)
{
	while(Begin != End)
	{
		if(P(*Begin))
			*DestBegin++ = *Begin;
		++Begin;
	}
	
	return DestBegin;
}

/// Use with std::for_each to destroy collections of objects stored by pointer
struct delete_object
{
	template<typename T>
	void operator()(const T* ptr) const
	{
		delete ptr;
	}
};

} // namespace k3d

#endif // !K3DSDK_UTILITY_H
