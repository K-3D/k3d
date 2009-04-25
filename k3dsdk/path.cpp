// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

// This code leans heavily on boost::filesystem::path, most changes are to support ustring UTF-8 storage

#include "path.h"
#include "k3d-platform-config.h"
#include "result.h"
#include "system.h"

#include <glibmm/convert.h>

#ifdef K3D_API_WIN32

	#include "win32.h"

#else // K3D_API_WIN32

	#include <sys/stat.h>
	#include <sys/types.h>
	#include <dirent.h>
	#include <fcntl.h>
	#include <unistd.h>

#endif // !K3D_API_WIN32

#include <boost/scoped_array.hpp>

namespace k3d
{

namespace filesystem
{

namespace detail
{

// Define some platform-specific odds-and-ends
#ifdef K3D_API_WIN32

const char NATIVE_SEARCHPATH_SEPARATOR = ';';
const char NATIVE_PATH_SEPARATOR = '\\';

#else // K3D_API_WIN32

const char NATIVE_SEARCHPATH_SEPARATOR = ':';
const char NATIVE_PATH_SEPARATOR = '/';

#endif // !K3D_API_WIN32

const char GENERIC_PATH_SEPARATOR = '/';

// end_pos is past-the-end position
// return 0 if str itself is leaf (or empty)
ustring::size_type leaf_pos( const ustring& str, ustring::size_type end_pos )
{
	if ( end_pos && str[end_pos-1] == '/' ) return end_pos-1;

	ustring::size_type pos( str.find_last_of( '/', end_pos-1 ) );
	if ( pos == ustring::npos ) pos = str.find_last_of( ':', end_pos-2 );

	return ( pos == ustring::npos // path itself must be a leaf (or empty)
	         || (pos == 1 && str[0] == '/') // or share
	       ) ? 0 // so leaf is entire string
	       : pos + 1; // or starts after delimiter
}

bool_t is_absolute_root(const ustring& s, ustring::size_type len)
{
	return
	    len && s[len-1] == '/'
	    &&
	    (
	        len == 1 // "/"
	        || ( len > 1
	             && ( s[len-2] == ':' // drive or device
	                  || ( s[0] == '/'   // share
	                       && s[1] == '/'
	                       && s.find( '/', 2 ) == len-1
	                     )
	                )
	           )
	    );
}

} // namespace detail

//////////////////////////////////////////////////////////////////////////////////
// path::iterator::implementation

class path::iterator::implementation
{
public:
	implementation() :
		storage(0),
		index(0)
	{
	}

	implementation(const ustring& Storage) :
		storage(&Storage),
		index(0)
	{
		if(Storage.size() > 2 && Storage[0] == '/' && Storage[1] == '/') // "//foo"
		{
			begin.push_back(0);
			end.push_back(std::min(Storage.size(), Storage.find('/', 2)));

			if(end.back() < Storage.size()) // "//foo/"
			{
				begin.push_back(end.back());
				end.push_back(end.back() + 1);

				if(end.back() < Storage.size()) // "//foo/bar"
				{
					begin.push_back(end.back());
					end.push_back(std::min(Storage.size(), Storage.find('/', end.back())));
				}
			}
		}
		else if(Storage.size() > 1 && Storage[1] == ':') // "c:"
		{
			begin.push_back(0);
			end.push_back(2);

			if(Storage.size() > 2)
			{
				if(Storage[2] == '/') // "c:/foo/bar"
				{
					begin.push_back(2);
					end.push_back(3);

					if(Storage.size() > 3)
					{
						begin.push_back(3);
						end.push_back(std::min(Storage.size(), Storage.find('/', 3)));
					}
				}
				else // "c:foo"
				{
					begin.push_back(2);
					end.push_back(std::min(Storage.size(), Storage.find('/', 2)));
				}
			}
		}
		else if(Storage.size() && Storage[0] == '/') // "/foo/bar"
		{
			begin.push_back(0);
			end.push_back(1);

			if(Storage.size() > 1)
			{
				begin.push_back(1);
				end.push_back(std::min(Storage.size(), Storage.find('/', 1)));
			}
		}
		else // "foo/bar"
		{
			begin.push_back(0);
			end.push_back(std::min(Storage.size(), Storage.find('/')));
		}

		while(end.size() && end.back() < Storage.size())
		{
			begin.push_back(end.back() + 1);
			end.push_back(std::min(Storage.size(), Storage.find('/', end.back() + 1)));
		}
	}

	ustring dereference() const
	{
		return storage->substr(begin[index], end[index] - begin[index]);
	}

	bool_t equal(const implementation& rhs) const
	{
		return storage == rhs.storage && index == rhs.index;
	}

	void increment()
	{
		++index;
		if(index >= begin.size())
		{
			storage = 0;
			begin.clear();
			end.clear();
			index = 0;
		}
	}

	const ustring* storage;
	std::vector<ustring::size_type> begin;
	std::vector<ustring::size_type> end;
	size_t index;
};

//////////////////////////////////////////////////////////////////////////////////
// path::iterator

path::iterator::iterator() :
	m_implementation(new implementation())
{
}

path::iterator::iterator(const ustring& Storage) :
	m_implementation(new implementation(Storage))
{
}

path::iterator::~iterator()
{
	delete m_implementation;
}

ustring path::iterator::operator*()
{
	return m_implementation->dereference();
}

bool_t path::iterator::operator==(const iterator& rhs)
{
	return m_implementation->equal(*rhs.m_implementation);
}

bool_t path::iterator::operator!=(const iterator& rhs)
{
	return !m_implementation->equal(*rhs.m_implementation);
}

path::iterator& path::iterator::operator++()
{
	m_implementation->increment();
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// path

path::path()
{
}

path::path(const ustring& GenericPath) :
		storage(GenericPath)
{
}

path generic_path(const ustring& GenericPath)
{
	return path(GenericPath);
}

path generic_path(const string_t& GenericPath)
{
	return path(ustring::from_utf8(GenericPath));
}

path native_path(const ustring& NativePath)
{
	ustring generic_path(NativePath);
	for(ustring::size_type i = generic_path.find('\\'); i != ustring::npos; i = generic_path.find('\\'))
		generic_path.replace(i, 1, 1, detail::GENERIC_PATH_SEPARATOR);

	return path(generic_path);
}

path& path::operator/=(const path& rhs)
{
	if(storage.empty())
	{
		storage = rhs.storage;
	}
	else
	{
		if((storage[storage.size()-1] == '/') || (rhs.storage.size() && rhs.storage[0] == '/'))
		{
			storage += rhs.storage;
		}
		else
		{
			storage += detail::GENERIC_PATH_SEPARATOR;
			storage += rhs.storage;
		}
	}

	return *this;
}

path path::operator/(const path& rhs) const
{
	path result(*this);
	result /= rhs;
	return result;
}

path path::operator+(const ustring& rhs) const
{
	return path(storage + rhs);
}

path path::operator+(const string_t& rhs) const
{
	return path(storage + ustring::from_utf8(rhs));
}

ustring path::generic_utf8_string() const
{
	return storage;
}

ustring path::native_utf8_string() const
{
	ustring native_string(storage);
	for(ustring::size_type i = native_string.find(detail::GENERIC_PATH_SEPARATOR); i != ustring::npos; i = native_string.find(detail::GENERIC_PATH_SEPARATOR, i+1))
		native_string.replace(i, 1, 1, detail::NATIVE_PATH_SEPARATOR);

	return native_string;
}

string_t path::native_console_string() const
{
	return native_utf8_string().raw();
}

string_t path::native_filesystem_string() const
{
#ifdef K3D_API_WIN32

	return Glib::locale_from_utf8(native_utf8_string().raw());

#else // K3D_API_WIN32

	return Glib::filename_from_utf8(native_utf8_string().raw());

#endif // !K3D_API_WIN32
}

path path::root_path() const
{
	return generic_path(root_name()) / generic_path(root_directory());
}

ustring path::root_name() const
{
	// Win32 cases ...
	ustring::size_type pos = storage.find(':');
	if(pos != ustring::npos)
		return storage.substr(0, pos + 1);

	if(storage.size() > 2 && storage[0] == '/' && storage[1] == '/')
	{
		pos = storage.find('/', 2);
		return storage.substr(0, pos);
	}

	// Posix case ...
	return ustring();
}

ustring path::root_directory() const
{
	if(storage.size() > 2 && storage[1] == ':' && storage[2] == '/')
		return ustring::from_utf8("/");

	if(storage.size() > 2 && storage[0] == '/' && storage[1] == '/')
		return ustring::from_utf8(storage.find('/', 2) != ustring::npos ? "/" : "");

	if(storage.size() && storage[0] == '/')
		return ustring::from_utf8("/");

	return ustring::from_utf8("");
}

ustring path::leaf() const
{
	return storage.substr(detail::leaf_pos(storage, storage.size()));
}

path path::branch_path() const
{
	ustring::size_type end_pos = detail::leaf_pos(storage, storage.size());

	// skip a '/' unless it is a root directory
	if(end_pos && storage[end_pos - 1] == '/' && !detail::is_absolute_root(storage, end_pos))
		--end_pos;

	return path(storage.substr(0, end_pos));
}

bool_t path::empty() const
{
	return storage.empty();
}

bool_t path::is_complete() const
{
	// Win32 cases ...
	if(storage.size() > 2 && storage[1] == ':' && storage[2] == '/') // "c:/"
		return true;
	if(storage.size() > 2 && storage[0] == '/' && storage[1] == '/') // "//share"
		return true;
	if(storage.size() > 2 && storage[storage.size()-1] == ':') // "prn:"
		return true;

	// Posix cases ...
	if(storage.size() && storage[0] == '/') // "/foo"
		return true;

	return false;
}

path::iterator path::begin() const
{
	return iterator(storage);
}

path::iterator path::end() const
{
	return iterator();
}

bool_t path::operator==(const path& that) const
{
	return !(*this < that) && !(that < *this);
}

bool_t path::operator!=(const path& that) const
{
	return !(*this == that);
}

bool_t path::operator<(const path& that) const
{
	return storage < that.storage;
}

bool_t path::operator<=(const path& that) const
{
	return !(that < *this);
}

bool_t path::operator>(const path& that) const
{
	return that < *this;
}

bool_t path::operator>=(const path& that) const
{
	return !(*this < that);
}

//////////////////////////////////////////////////////////////////////////////////
// exists

bool_t exists(const path& Path)
{
#ifdef K3D_API_WIN32

	return ::GetFileAttributesA(Path.native_filesystem_string().c_str()) != 0xFFFFFFFF;

#else // K3D_API_WIN32

	struct stat path_stat;
	return ::stat(Path.native_filesystem_string().c_str(), &path_stat) == 0;

#endif // !K3D_API_WIN32
}

const ustring extension(const path& Path);
bool_t create_directory(const path& Path);
bool_t create_directories(const path& Path);

/////////////////////////////////////////////////////////////////////////////////////////////
// make_relative_path

const filesystem::path make_relative_path(const filesystem::path& AbsolutePath, const filesystem::path& ReferencePath)
{
	// The AbsolutePath must actually *be* an absolute path!
	return_val_if_fail(AbsolutePath.is_complete(), filesystem::path());

	// As a special-case, if the AbsolutePath and ReferencePath don't share the same root name, return the AbsolutePath (which is the best we can do)
	if(AbsolutePath.root_name() != ReferencePath.root_name())
		return AbsolutePath;

	filesystem::path relative_path;

	const filesystem::path root_path = ReferencePath;
	const filesystem::path absolute_path(AbsolutePath);

	filesystem::path::iterator a = root_path.begin();
	filesystem::path::iterator b = absolute_path.begin();
	while(a != root_path.end() && b != absolute_path.end() && *a == *b)
	{
		++a;
		++b;
	}

	for(; a != root_path.end(); ++a)
		relative_path /= filesystem::generic_path("..");
	for(; b != absolute_path.end(); ++b)
		relative_path /= filesystem::generic_path(*b);

	return relative_path;
}

//////////////////////////////////////////////////////////////////////////////////
// extension

const ustring extension(const path& Path)
{
	ustring leaf = Path.leaf();

	ustring::size_type n = leaf.rfind('.');
	if (n != ustring::npos)
		return leaf.substr(n);
	else
		return ustring();
}

//////////////////////////////////////////////////////////////////////////////////
// replace_extension

const path replace_extension(const path& Path, const string_t& NewExtension)
{
	ustring temp = Path.generic_utf8_string(); 

	return generic_path(temp.substr(0, temp.rfind('.')) + ustring::from_utf8(NewExtension));
}

//////////////////////////////////////////////////////////////////////////////////
// create_directory

bool_t create_directory(const path& Path)
{
	return_val_if_fail(!Path.empty(), false);

	if(exists(Path))
	{
		if(is_directory(Path))
			return true;

		k3d::log() << error << "Path [" << Path.native_console_string() << "] exists and is not a directory" << std::endl;
		return false;
	}

#ifdef K3D_API_WIN32
	if ( ::CreateDirectoryA( Path.native_filesystem_string().c_str(), 0 ) )
		return true;
#else // K3D_API_WIN32
	if ( ::mkdir( Path.native_filesystem_string().c_str(), S_IRWXU|S_IRWXG|S_IRWXO ) == 0 )
		return true;
#endif // !K3D_API_WIN32

	k3d::log() << error << "Error creating directory [" << Path.native_console_string() << "]" << std::endl;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////
// create_directories

bool_t create_directories(const path& Path)
{
	return_val_if_fail(!Path.empty(), false);

	if(exists(Path))
	{
		if(is_directory(Path))
			return true;

		k3d::log() << error << "Path [" << Path.native_console_string() << "] exists and is not a directory" << std::endl;
		return false;
	}

	// First create branch, by calling ourself recursively
	if(!create_directories(Path.branch_path()))
		return false;

	// Now that parent's path exists, create the directory
	return create_directory(Path);
}

//////////////////////////////////////////////////////////////////////////////////
// is_directory

bool_t is_directory(const path& Path)
{
#ifdef K3D_API_WIN32

	const DWORD attributes = ::GetFileAttributesA(Path.native_filesystem_string().c_str());
	return attributes & FILE_ATTRIBUTE_DIRECTORY;

#else // K3D_API_WIN32

	struct stat path_stat;
	if(::stat(Path.native_filesystem_string().c_str(), &path_stat) != 0)
		return false;

	return S_ISDIR(path_stat.st_mode);

#endif // !K3D_API_WIN32
}

//////////////////////////////////////////////////////////////////////////////////
// remove

bool_t remove(const path& Path)
{
#ifdef K3D_API_WIN32

	if(is_directory(Path))
		return ::RemoveDirectory(Path.native_filesystem_string().c_str());

	return ::DeleteFile(Path.native_filesystem_string().c_str());

#else // K3D_API_WIN32

	if(is_directory(Path))
		return 0 == ::rmdir(Path.native_filesystem_string().c_str());

	return 0 == ::unlink(Path.native_filesystem_string().c_str());

#endif // !K3D_API_WIN32
}

//////////////////////////////////////////////////////////////////////////////////
// rename

bool_t rename(const path& Source, const path& Target)
{
#ifdef K3D_API_WIN32

	return ::MoveFile(Source.native_filesystem_string().c_str(), Target.native_filesystem_string().c_str());

#else // K3D_API_WIN32

	return 0 == ::rename(Source.native_filesystem_string().c_str(), Target.native_filesystem_string().c_str());

#endif // !K3D_API_WIN32
}

//////////////////////////////////////////////////////////////////////////////////
// copy_file

bool_t copy_file(const path& Source, const path& Target)
{
#ifdef K3D_API_WIN32

	return ::CopyFile(Source.native_filesystem_string().c_str(), Target.native_filesystem_string().c_str(), false);

#else // K3D_API_WIN32

	struct stat source_stat;
	if(0 != ::stat(Source.native_filesystem_string().c_str(), &source_stat))
		return false;

	const int source_file = ::open(Source.native_filesystem_string().c_str(), O_RDONLY);
	if(source_file < 1)
		return false;

	const int target_file = ::open(Target.native_filesystem_string().c_str(), O_WRONLY | O_CREAT | O_EXCL, source_stat.st_mode);
	if(target_file < 1)
	{
		::close(source_file);
		return false;
	}

	bool_t result = true;

	const std::size_t buffer_size = 32768;
	boost::scoped_array<char> buffer(new char[buffer_size]);

	for(ssize_t bytes_read = ::read(source_file, buffer.get(), buffer_size); bytes_read > 0; bytes_read = ::read(source_file, buffer.get(), buffer_size))
	{
		if(::write(target_file, buffer.get(), bytes_read) < 0)
		{
			result = false;
			break;
		}
	}

	if(::close(source_file) < 0 )
		result = false;

	if(::close(target_file) < 0 )
		result = false;

	return result;

#endif // !K3D_API_WIN32
}

//////////////////////////////////////////////////////////////////////////////////
// up_to_date

bool_t up_to_date(const path& Source, const path& Target)
{
	// Get the last modification time of the source ...
	time_t source_modified = 0;
	return_val_if_fail(system::file_modification_time(Source, source_modified), false);

	// Get the last modification time of the target (if it exists) ...
	time_t target_modified = 0;
	system::file_modification_time(Target, target_modified);

	return source_modified <= target_modified;
}

//////////////////////////////////////////////////////////////////////////////////
// split_native_paths

const path_list split_native_paths(const ustring& Paths)
{
	path_list results;

	ustring::size_type begin = 0;
	const ustring::size_type end = Paths.size();
	for(ustring::size_type separator = Paths.find(detail::NATIVE_SEARCHPATH_SEPARATOR, begin); separator != ustring::npos; separator = Paths.find(detail::NATIVE_SEARCHPATH_SEPARATOR, begin))
	{
		results.push_back(native_path(Paths.substr(begin, separator - begin)));
		begin = separator + 1;
	}
	if(begin < end)
		results.push_back(native_path(Paths.substr(begin, end - begin)));

	return results;
}

//////////////////////////////////////////////////////////////////////////////////
// directory_iterator::implementation

#ifdef K3D_API_WIN32

class directory_iterator::implementation
{
public:
	implementation(const path& Path) :
			handle(INVALID_HANDLE_VALUE),
			branch_path(Path)
	{
		path temp = Path + "\\*";

		handle = ::FindFirstFile(temp.native_filesystem_string().c_str(), &data);
		skip_dots();

		if(handle != INVALID_HANDLE_VALUE)
			full_path = branch_path / generic_path(ustring::from_utf8(data.cFileName));
	}

	implementation() :
			handle(INVALID_HANDLE_VALUE)
	{
	}

	~implementation()
	{
		if(handle != INVALID_HANDLE_VALUE)
			::FindClose(handle);
	}

	directory_iterator::reference dereference() const
	{
		return full_path;
	}

	void increment()
	{
		internal_increment();
		skip_dots();

		if(handle != INVALID_HANDLE_VALUE)
			full_path = branch_path / generic_path(ustring::from_utf8(data.cFileName));
	}

	bool_t equal(const implementation& rhs) const
	{
		return handle == rhs.handle;
	}

private:
	void internal_increment()
	{
		if(0 == ::FindNextFile(handle, &data))
		{
			::FindClose(handle);
			handle = INVALID_HANDLE_VALUE;
		}
	}

	void skip_dots()
	{
		while(handle != INVALID_HANDLE_VALUE && data.cFileName[0]=='.' && (data.cFileName[1]=='\0' || (data.cFileName[1]=='.' && data.cFileName[2]=='\0')))
			internal_increment();
	}

	HANDLE handle;
	WIN32_FIND_DATA data;
	const path branch_path;
	path full_path;
};

#else // K3D_API_WIN32

class directory_iterator::implementation
{
public:
	implementation(const path& Path) :
		handle(0),
		current(0),
		branch_path(Path)
	{
		handle = ::opendir(Path.native_filesystem_string().c_str());

		internal_increment();
		skip_dots();

		if(current)
			full_path = branch_path / generic_path(current->d_name);
	}

	implementation() :
		handle(0),
		current(0)
	{
	}

	~implementation()
	{
		if(handle)
			::closedir(handle);
	}

	directory_iterator::reference dereference() const
	{
		return full_path;
	}

	void increment()
	{
		internal_increment();
		skip_dots();

		if(current)
			full_path = branch_path / generic_path(current->d_name);
	}

	bool_t equal(const implementation& rhs)
	{
		return current == rhs.current;
	}

private:
	void internal_increment()
	{
		if(handle)
		{
			current = ::readdir(handle);
			if(!current)
			{
				::closedir(handle);
				handle = 0;
			}
		}
	}

	void skip_dots()
	{
		while(current && current->d_name[0]=='.' && (current->d_name[1]=='\0' || (current->d_name[1]=='.' && current->d_name[2]=='\0')))
			internal_increment();
	}

	DIR* handle;
	dirent* current;
	const path branch_path;
	path full_path;
};

#endif // !K3D_API_WIN32

//////////////////////////////////////////////////////////////////////////////////
// directory_iterator

directory_iterator::directory_iterator(const path& Path) :
		m_implementation(new implementation(Path))
{
}

directory_iterator::directory_iterator() :
		m_implementation(new implementation())
{
}

directory_iterator::~directory_iterator()
{
	delete m_implementation;
}

directory_iterator::reference directory_iterator::dereference() const
{
	return m_implementation->dereference();
}

void directory_iterator::increment()
{
	m_implementation->increment();
}

bool_t directory_iterator::equal(const directory_iterator& rhs) const
{
	return m_implementation->equal(*rhs.m_implementation);
}

} // namespace filesystem

} // namespace k3d

