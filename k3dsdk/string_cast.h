#ifndef K3DSDK_STRING_CAST_H
#define K3DSDK_STRING_CAST_H

#include "path.h"

#include <boost/format.hpp>

#include <limits>
#include <sstream>
#include <string>

namespace k3d
{

///////////////////////////////////////////////////////////////////////////
// string_cast

/// Converts any serializable type to a string
template<typename type>
const string_t string_cast(const type& RHS)
{
	std::ostringstream buffer;
	if(std::numeric_limits<type>::is_specialized)
	        buffer.precision(std::numeric_limits<type>::digits10 + 1);

	buffer << RHS;
	return buffer.str();
}

/// Specialization of string_cast for type bool_t
template<>
inline const string_t string_cast<bool_t>(const bool_t& RHS)
{
	return RHS ? "true" : "false";
}

/// Specialization of string_cast for strings
template<>
inline const string_t string_cast<string_t>(const string_t& RHS)
{
	return RHS;
}

/// Specialization of string_cast() for filesystem::path
/** \todo Is this really the correct behavior?  */
template<>
inline const string_t string_cast<filesystem::path>(const filesystem::path& RHS)
{
	return RHS.native_utf8_string().raw();
}

/// Specialization of string_cast() for boost::format
template<>
inline const string_t string_cast<boost::format>(const boost::format& RHS)
{
	return RHS.str();
}

/////////////////////////////////////////////////////////////////////////////
// from_string

/// Converts a string into any serializeable type
template<typename type>
type from_string(const string_t& Value, const type& Default)
{
	type result = Default;
	std::istringstream stream(Value.c_str());
	stream >> result;

	return result;
}

/// Specialization of from_string for type bool_t
template<>
inline bool_t from_string(const string_t& Value, const bool_t& Default)
{
	bool_t result = Default;

	if(Value == "true")
		result = true;
	else if(Value == "false")
		result = false;

	return result;
}

/// Specialization of from_string for strings
template<>
inline string_t from_string(const string_t& Value, const string_t&)
{
	return Value;
}

/// Specialization of from_string for filesystem::path
template<>
inline filesystem::path from_string(const string_t& Value, const filesystem::path&)
{
	return filesystem::native_path(ustring::from_utf8(Value));
}

} // namespace k3d

#endif // !K3DSDK_STRING_CAST_H

