#ifndef K3DSDK_NAMESPACE_DOCUMENTATION_H
#define K3DSDK_NAMESPACE_DOCUMENTATION_H

/** \file
	\brief This do-nothing file is used to provide a central point for documenting the K-3D library namespaces for use with Doxygen
	\author Timothy M. Shead <tshead@k-3d.com>
*/

/// Namespace reserved for the boost library, see http://www.boost.org
namespace boost
{
	/// Namespace reserved for the boost filesystem library, see http://www.boost.org/libs/filesystem
	namespace filesystem {}
	/// Namespace reserved for the spirit parser framework, see http://spirit.sourceforge.net
	namespace spirit {}
}

/// Namespace reserved for the gdkmm C++ wrappers for GTK, see http://gtkmm.sourceforge.net
namespace Gdk {}

/// Namespace reserved for the glibmm C++ wrappers for glib, see http://gtkmm.sourceforge.net
namespace Glib {}

/// Namespace reserved for the gtkmm C++ wrappers for GTK, see http://gtkmm.sourceforge.net
namespace Gtk {}

/// Namespace reserved for K-3D plugin modules
namespace module {}

/// Namespace reserved for all public symbols in the K-3D SDK
namespace k3d
{

/// Contains commonly-used plugin object class identifiers, so specific plugin objects can be instantiated
namespace classes {}

/// Contains constraint objects used with k3d::data and related objects
namespace constraint {}

/// Contains private implementation details for the k3d API
namespace detail {}

/// Contains a boost::spirit grammar and related objects for parsing numeric expressions
namespace expression {}

/// Contains functions and objects to simplify working with the OpenGL API, sese http://www.opengl.org
namespace gl
{
	/// Contains private implementation details of the k3d::gl API
	namespace detail {}
}

/// Contains private implementation details for the k3d API (deprecated, use k3d::detail for new code instead)
namespace implementation_private {}

/// Contains objects for use with libsigc++ signals
namespace signal {}

/// Contains objects for storing and converting real-world units-of-measure
namespace measurement
{
	/// Contains policy objects for converting between real-world units-of-measure
	namespace conversion {}
} // namespace measurement

/// Contains functions and classes used to make object serialization & persistence easier
namespace persistence {}

/// Contains functions and classes used to export easily-modifiable "properties" from objects
namespace property {}

/// Contains functions and classes used to generate RenderMan compatible descriptions of a document
namespace ri
{
	/// Contains private implementation details of the k3d::ri API
	namespace detail {}
}

/// Contains functions and classes for working with RenderMan Shading Language data
namespace sl {}

/// Contains functions and classes used to abstract away differences in operating systems / platforms
namespace system {}

/// Contains functions and classes for working with XML data
namespace xml
{
	/// Contains private implementation details of the k3d::xml API
	namespace detail {}
}

} // namespace k3d

/// Namespace reserved for the libsigc++ signal library, see http://libsigc.sourceforge.net
namespace sigc {}

/// Namespace reserved for the libsigc++ signal library (compatibility layer), see http://libsigc.sourceforge.net
namespace SigC {}

/// Namespace reserved for the C++ Standard Library
namespace std {}

/// Namespace reserved for the ImageMagick C++ binding, see http://studio.imagemagick.org/Magick++
namespace Magick {}

#endif // !K3DSDK_NAMESPACE_DOCUMENTATION_H

