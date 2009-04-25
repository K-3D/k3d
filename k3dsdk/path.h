#ifndef K3DSDK_PATH_H
#define K3DSDK_PATH_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\author Tim Shead (tshead@k-3d.com)
*/

#include "types.h"
#include "ustring.h"
#include <boost/iterator/iterator_facade.hpp>
#include <vector>

namespace k3d
{

namespace filesystem
{

/// Portable filesystem path that closely follows the boost::filesystem::path interface, with some important exceptions
/** \note Never throws an exception */
/** \note Stores paths in UTF-8 encoding.  All input and output is in UTF-8 encoding. */
class path
{
public:
	/// Constructs an empty path
	path();
	/// Constructs a path from a string containing the generic grammar
	friend path generic_path(const ustring& GenericPath);
	/// Constructs a path from a string containing the generic grammar (the string is assumed to contain UTF-8 data, no character set conversion is applied)
	friend path generic_path(const string_t& GenericPath);
	/// Constructs a path from a native file string, i.e. any valid Posix or Win32 path (doesn't matter which platform we were compiled on)
	friend path native_path(const ustring& NativePath);

	/// Path concatenation
	path& operator/=(const path& rhs);
	/// Path concatenation
	path operator/(const path& rhs) const;

	/// Appending text (useful for file extensions)
	path operator+(const ustring& rhs) const;
	/// Appending text (useful for file extensions)
	path operator+(const string_t& rhs) const;

	/// Returns the generic form of the path as a UTF-8 encoded string
	ustring generic_utf8_string() const;
	/// Returns the native form of the path as a UTF-8 encoded string
	ustring native_utf8_string() const;
	/// Returns the native form of the path as a string suitable for console output
	string_t native_console_string() const;
	/// Returns the native form of the path as a string suitable for use with the underlying OS filesystem
	string_t native_filesystem_string() const;

	// modification functions:
//	path& normalize();

	// decomposition functions:
	path root_path() const;
	ustring root_name() const;
	ustring root_directory() const;
//	path relative_path() const;
	ustring leaf() const;
	path branch_path() const;

	// query functions:
	bool_t empty() const;
	bool_t is_complete() const;
/*
	bool_t has_root_path() const;
	bool_t has_root_name() const;
	bool_t has_root_directory() const;
	bool_t has_relative_path() const;
	bool_t has_leaf() const;
	bool_t has_branch_path() const;
*/

	// iteration:
	class iterator
	{
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef ustring value_type;
		typedef ustring& reference;
		typedef ustring* pointer;
		typedef ptrdiff_t difference_type;

		iterator();
		iterator(const ustring& Storage);
		~iterator();

		value_type operator*();

		bool_t operator==(const iterator& RHS);
		bool_t operator!=(const iterator& RHS);
		iterator& operator++();

	private:
		class implementation;
		implementation* const m_implementation;
	};

	iterator begin() const;
	iterator end() const;

	// relational operators:
	bool_t operator==(const path& that) const;
	bool_t operator!=(const path& that) const;
	bool_t operator<(const path& that) const;
	bool_t operator<=(const path& that) const;
	bool_t operator>(const path& that) const;
	bool_t operator>=(const path& that) const;

private:
	path(const ustring& GenericPath);
	ustring storage;
};

/// Constructs a path from a string containing the generic grammar
path generic_path(const ustring& GenericPath);
/// Constructs a path from a string containing the generic grammar (the string is assumed to contain UTF-8 data, no character set conversion is applied)
path generic_path(const string_t& GenericPath);
/// Constructs a path from a native file string, i.e. any valid Posix or Win32 path (doesn't matter which platform we were compiled on)
path native_path(const ustring& NativePath);

/// Defines a collection of paths
typedef std::vector<path> path_list;

/// Returns true if the operating system reports that the given file exists
bool_t exists(const path& Path);
/// Converts the given absolute path into a relative path, relative to the given reference path
const path make_relative_path(const path& AbsolutePath, const path& ReferencePath);
/// If Path.leaf() contains a dot, returns the substring of Path.leaf() starting from the last dot.  Otherwise, returns empty string
const ustring extension(const path& Path);
/// If Path.leaf() contains a dot, replaces the substring of Path.leaf() starting from the last dot with the given text.  Otherwise, returns empty string.
const path replace_extension(const path& Path, const string_t& NewExtension);
/// Creates the given directory
bool_t create_directory(const path& Path);
/// Creates the given directory, including any missing parent directories
bool_t create_directories(const path& Path);
/// Returns true if the given path is a directory
bool_t is_directory(const path& Path);
/// Removes a file
bool_t remove(const path& Path);
/// Renames a file
bool_t rename(const path& Source, const path& Target);
/// Copies a file
bool_t copy_file(const path& Source, const path& Target);

/// Returns true iff the Target exists and is newer than the Source
bool_t up_to_date(const path& Source, const path& Target);

/// Splits a string containing delimited paths (such as the PATH environment variable) into a collection of individual paths
/** \note: Splits the string using the filesystem delimiter for the native system (the system we were compiled on). */
const path_list split_native_paths(const ustring& Paths);

/// Provides iteration over the contents of a directory
class directory_iterator :
	public boost::iterator_facade<directory_iterator, const path, boost::single_pass_traversal_tag>
{
public:
	/// Creates the "begin" iterator for the contents of the given directory
	explicit directory_iterator(const path& Path);
	/// Creates an "end" iterator
	directory_iterator();
	~directory_iterator();

private:
	class implementation;
	implementation* m_implementation;

	friend class boost::iterator_core_access;
	reference dereference() const;
	void increment();
	bool_t equal(const directory_iterator& rhs) const;
};

} // namespace filesystem

} // namespace k3d

#endif // !K3DSDK_PATH_H

