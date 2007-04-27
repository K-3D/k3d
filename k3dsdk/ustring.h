#ifndef K3DSDK_USTRING_H
#define K3DSDK_USTRING_H

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

#include <glibmm/ustring.h>

namespace k3d
{

/// UTF-8 string class that is based on Glib::ustring, with some omissions
/** \note: Does not provide implicit conversions to/from std::string */
/** \note: Does not insertion and extraction operators */
class ustring
{
public:
	typedef Glib::ustring::size_type size_type;
	typedef Glib::ustring::difference_type difference_type;
	typedef gunichar value_type;
	typedef gunichar& reference;
	typedef const gunichar& const_reference;
	typedef Glib::ustring::iterator iterator;
	typedef Glib::ustring::const_iterator const_iterator;
	typedef Glib::ustring::reverse_iterator reverse_iterator;
	typedef Glib::ustring::const_reverse_iterator const_reverse_iterator;

	static const size_type npos;

	/*! Default constructor, which creates an empty string.
	*/
	ustring();

	~ustring();

	/*! Construct a ustring as a copy of another ustring.
	* @param other A source string.
	*/
	ustring(const ustring& other);

	/*! Construct a ustring as a copy of a substring.
	* @param src %Source ustring.
	* @param i Index of first character to copy from.
	* @param n Number of characters to copy (defaults to copying the remainder).
	*/
	ustring(const ustring& src, size_type i, size_type n=npos);

	/*! Construct a ustring as multiple characters.
	* @param n Number of characters.
	* @param uc UCS-4 code point to use.
	*/  
	ustring(size_type n, gunichar uc);

	/// Constructs a ustring from a string that already contains UTF-8 characters (i.e. no conversion is performed)
	static ustring from_utf8(const std::string& src);
	/// Constructs a ustring from a string that already contains UTF-8 characters (i.e. no conversion is performed)
	static ustring from_utf8(const char* src);

	/*! Assign the value of another string to this string.
	* @param other A source string.
	*/ 
	ustring& operator=(const ustring& other);

	/*! Swap contents with another string.
	* @param other String to swap with.
	*/
	void swap(ustring& other);

	//! @name Assign new contents.
	//! @{

	ustring& assign(const ustring& src);
	ustring& assign(const ustring& src, size_type i, size_type n);
	ustring& assign(size_type n, gunichar uc);

	//! @}
	//! @name Append to the string.
	//! @{

	ustring operator+(const ustring& src) const;
	ustring& operator+=(const ustring& src);
	ustring& operator+=(gunichar uc);
	void push_back(gunichar uc);

	ustring& append(const ustring& src);
	ustring& append(const ustring& src, size_type i, size_type n);
	ustring& append(size_type n, gunichar uc);

	//! @}
	//! @name Insert into the string.
	//! @{

	ustring& insert(size_type i, const ustring& src);
	ustring& insert(size_type i, const ustring& src, size_type i2, size_type n);
	ustring& insert(size_type i, size_type n, gunichar uc);
	iterator insert(iterator p, gunichar uc);
	void     insert(iterator p, size_type n, gunichar uc);

	//! @}
	//! @name Replace sub-strings.
	//! @{

	ustring& replace(size_type i, size_type n, const ustring& src);
	ustring& replace(size_type i, size_type n, const ustring& src, size_type i2, size_type n2);
	ustring& replace(size_type i, size_type n, size_type n2, gunichar uc);
	ustring& replace(iterator pbegin, iterator pend, const ustring& src);
	ustring& replace(iterator pbegin, iterator pend, size_type n, gunichar uc);

	//! @}
	//! @name Erase sub-strings.
	//! @{

	void clear();
	ustring& erase(size_type i, size_type n=npos);
	ustring& erase();
	iterator erase(iterator p);
	iterator erase(iterator pbegin, iterator pend);

	//! @}
	//! @name Compare and collate.
	//! @{

	int compare(const ustring& rhs) const;
	int compare(size_type i, size_type n, const ustring& rhs) const;
	int compare(size_type i, size_type n, const ustring& rhs, size_type i2, size_type n2) const;

	/*! Create a unique sorting key for the UTF-8 string.  If you need to
	* compare UTF-8 strings regularly, e.g. for sorted containers such as
	* <tt>std::set<></tt>, you should consider creating a collate key first
	* and compare this key instead of the actual string.
	*
	* The ustring::compare() methods as well as the relational operators
	* <tt>==&nbsp;!=&nbsp;<&nbsp;>&nbsp;<=&nbsp;>=</tt> are quite costly
	* because they have to deal with %Unicode and the collation rules defined by
	* the current locale.  Converting both operands to UCS-4 is just the first
	* of several costly steps involved when comparing ustrings.  So be careful.
	*/
	std::string collate_key() const;

	/*! Create a unique key for the UTF-8 string that can be used for caseless
	* sorting.  <tt>ustr.casefold_collate_key()</tt> results in the same string
	* as <tt>ustr.casefold().collate_key()</tt>, but the former is likely more
	* efficient.
	*/
	std::string casefold_collate_key() const;

	//! @}
	//! @name Extract characters and sub-strings.
	//! @{

	/*! No reference return; use replace() to write characters. */
	value_type operator[](size_type i) const;

	/*! No reference return; use replace() to write characters. @throw std::out_of_range */
	value_type at(size_type i) const;

	inline ustring substr(size_type i=0, size_type n=npos) const;

	//! @}
	//! @name Access a sequence of characters.
	//! @{

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end()   const;
	reverse_iterator rbegin();
	reverse_iterator rend();
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend()   const;

	//! @}
	//! @name Find sub-strings.
	//! @{

	size_type find(const ustring& str, size_type i=0) const;
	size_type find(gunichar uc, size_type i=0) const;

	size_type rfind(const ustring& str, size_type i=npos) const;
	size_type rfind(gunichar uc, size_type i=npos) const;

	//! @}
	//! @name Match against a set of characters.
	//! @{

	size_type find_first_of(const ustring& match, size_type i=0) const;
	size_type find_first_of(gunichar uc, size_type i=0) const;

	size_type find_last_of(const ustring& match, size_type i=npos) const;
	size_type find_last_of(gunichar uc, size_type i=npos) const;

	size_type find_first_not_of(const ustring& match, size_type i=0) const;
	size_type find_first_not_of(gunichar uc, size_type i=0) const;

	size_type find_last_not_of(const ustring& match, size_type i=npos) const;
	size_type find_last_not_of(gunichar uc, size_type i=npos) const;
	
	//! @}
	//! @name Retrieve the string's size.
	//! @{

	/** Returns true if the string is empty. Equivalent to *this == "".
	* @result Whether the string is empty.
	*/
	bool empty()  const;

	/** Returns the number of characters in the string, not including any null-termination.
	* @result The number of UTF-8 characters.
	*
	* @see bytes(), empty() 
	*/
	size_type size()   const;

	//We have length() as well as size(), because std::string has both.

	/** This is the same as size().
	*/
	size_type length() const;

	/** Returns the number of bytes in the string, not including any null-termination.
	* @result The number of bytes.
	*
	* @see size(), empty()
	*/
	size_type bytes()  const;

	//! @}
	//! @name Change the string's size.
	//! @{

	void resize(size_type n, gunichar uc);

	//! @}
	//! @name Control the allocated memory.
	//! @{

	size_type capacity() const;
	size_type max_size() const;
	void reserve(size_type n=0);

	//! @}
	//! @name Get a per-byte representation of the string.
	//! @{

	const std::string& raw() const;

	//! @}
	//! @name UTF-8 utilities.
	//! @{

	/*! Check whether the string is valid UTF-8. */
	bool validate() const;

	/*! Check whether the string is valid UTF-8. */
	bool validate(iterator& first_invalid);

	/*! Check whether the string is valid UTF-8. */
	bool validate(const_iterator& first_invalid) const;

	/*! Check whether the string is plain 7-bit ASCII. @par
	* Unlike any other ustring method, is_ascii() is safe to use on invalid
	* UTF-8 strings.  If the string isn't valid UTF-8, it cannot be valid
	* ASCII either, therefore is_ascii() will just return @c false then.
	* @return Whether the string contains only ASCII characters.
	*/
	bool is_ascii() const;

	/*! "Normalize" the %Unicode character representation of the string. */
//	ustring normalize(Glib::ustring::NormalizeMode mode = Glib::ustring::NORMALIZE_DEFAULT_COMPOSE) const;

	//! @}
	//! @name Character case conversion.
	//! @{

	/*! Returns a new UTF-8 string with all characters characters converted to
	* their lowercase equivalent, while honoring the current locale.  The
	* resulting string may change in the number of bytes as well as in the
	* number of characters.  For instance, the German sharp&nbsp;s
	* <tt>&quot;&szlig;&quot;</tt> will be replaced by two characters
	* <tt>"SS"</tt> because there is no capital <tt>&quot;&szlig;&quot;</tt>.
	*/
	ustring uppercase() const;

	/*! Returns a new UTF-8 string with all characters characters converted to
	* their lowercase equivalent, while honoring the current locale.  The
	* resulting string may change in the number of bytes as well as in the
	* number of characters.
	*/
	ustring lowercase() const;

	/*! Returns a caseless representation of the UTF-8 string.  The resulting
	* string doesn't correspond to any particular case, therefore the result
	* is only useful to compare strings and should never be displayed to the
	* user.
	*/
	ustring casefold() const;

//! @}

	friend bool operator<(const ustring& lhs, const ustring& rhs);
	friend bool operator==(const ustring& lhs, const ustring& rhs);
	friend bool operator!=(const ustring& lhs, const ustring& rhs);

private:
	ustring(const Glib::ustring& str);
	Glib::ustring storage;
};

inline ustring::ustring() {}
inline ustring::~ustring() {}
inline ustring::ustring(const ustring& other) : storage(other.storage) {}
inline ustring::ustring(const ustring& src, size_type i, size_type n) : storage(src.storage, i, n) {}
inline ustring::ustring(size_type n, gunichar uc) : storage(n, uc) {}
inline ustring::ustring(const Glib::ustring& str) : storage(str) {}
inline ustring ustring::from_utf8(const std::string& src) { return ustring(Glib::ustring(src)); }
inline ustring ustring::from_utf8(const char* src) { return ustring(Glib::ustring(src)); }
inline ustring::ustring& ustring::operator=(const ustring& other) { storage = other.storage; return *this; }
inline void ustring::swap(ustring& other) { storage.swap(other.storage); }
inline ustring& ustring::assign(const ustring& src) { storage.assign(src.storage); return *this; }
inline ustring& ustring::assign(const ustring& src, size_type i, size_type n) { storage.assign(src.storage, i, n); return *this; }
inline ustring& ustring::assign(size_type n, gunichar uc) { storage.assign(n, uc); return *this; }
inline ustring ustring::operator+(const ustring& src) const { ustring temp(storage); temp += src; return temp; }
inline ustring& ustring::operator+=(const ustring& src) { storage += src.storage; return *this; }
inline ustring& ustring::operator+=(gunichar uc) { storage += uc; return *this; }
inline void ustring::push_back(gunichar uc) { storage.push_back(uc); }
inline ustring& ustring::append(const ustring& src) { storage.append(src.storage); return *this; }
inline ustring& ustring::append(const ustring& src, size_type i, size_type n) { storage.append(src.storage, i, n); return *this; }
inline ustring& ustring::append(size_type n, gunichar uc) { storage.append(n, uc); return *this; }
inline ustring& ustring::insert(size_type i, const ustring& src) { storage.insert(i, src.storage); return *this; }
inline ustring& ustring::insert(size_type i, const ustring& src, size_type i2, size_type n) { storage.insert(i, src.storage, i2, n); return *this; }
inline ustring& ustring::insert(size_type i, size_type n, gunichar uc) { storage.insert(i, n, uc); return *this; }
inline ustring::iterator ustring::insert(ustring::iterator p, gunichar uc) { return storage.insert(p, uc); }
inline void ustring::insert(ustring::iterator p, size_type n, gunichar uc) { storage.insert(p, n, uc); }
inline ustring& ustring::replace(size_type i, size_type n, const ustring& src) { storage.replace(i, n, src.storage); return *this; }
inline ustring& ustring::replace(size_type i, size_type n, const ustring& src, size_type i2, size_type n2) { storage.replace(i, n, src.storage, i2, n2); return *this; }
inline ustring& ustring::replace(size_type i, size_type n, size_type n2, gunichar uc) { storage.replace(i, n, n2, uc); return *this; }
inline ustring& ustring::replace(iterator pbegin, iterator pend, const ustring& src) { storage.replace(pbegin, pend, src.storage); return *this; }
inline ustring& ustring::replace(iterator pbegin, iterator pend, size_type n, gunichar uc) { storage.replace(pbegin, pend, n, uc); return *this; }
inline void ustring::clear() { storage.clear(); }
inline ustring& ustring::erase(size_type i, size_type n) { storage.erase(i, n); return *this; }
inline ustring& ustring::erase() { storage.erase(); return *this; }
inline ustring::iterator ustring::erase(ustring::iterator p) { return storage.erase(p); }
inline ustring::iterator ustring::erase(ustring::iterator pbegin, iterator pend) { return storage.erase(pbegin, pend); }
inline int ustring::compare(const ustring& rhs) const { return storage.compare(rhs.storage); }
inline int ustring::compare(size_type i, size_type n, const ustring& rhs) const { return storage.compare(i, n, rhs.storage); }
inline int ustring::compare(size_type i, size_type n, const ustring& rhs, size_type i2, size_type n2) const { return storage.compare(i, n, rhs.storage, i2, n2); }
inline std::string ustring::collate_key() const { return storage.collate_key(); }
inline std::string ustring::casefold_collate_key() const { return storage.casefold_collate_key(); }
inline ustring::value_type ustring::operator[](size_type i) const { return storage[i]; }
inline ustring::value_type ustring::at(size_type i) const { return storage.at(i); }
inline ustring ustring::substr(size_type i, size_type n) const { return storage.substr(i, n); }
inline ustring::iterator ustring::begin() { return storage.begin(); }
inline ustring::iterator ustring::end() { return storage.end(); }
inline ustring::const_iterator ustring::begin() const { return storage.begin(); }
inline ustring::const_iterator ustring::end() const { return storage.end(); }
inline ustring::reverse_iterator ustring::rbegin() { return storage.rbegin(); }
inline ustring::reverse_iterator ustring::rend() { return storage.rend(); }
inline ustring::const_reverse_iterator ustring::rbegin() const { return storage.rbegin(); }
inline ustring::const_reverse_iterator ustring::rend() const { return storage.rend(); }
inline ustring::size_type ustring::find(const ustring& str, size_type i) const { return storage.find(str.storage, i); }
inline ustring::size_type ustring::find(gunichar uc, size_type i) const { return storage.find(uc, i); }
inline ustring::size_type ustring::rfind(const ustring& str, size_type i) const { return storage.rfind(str.storage, i); }
inline ustring::size_type ustring::rfind(gunichar uc, size_type i) const { return storage.rfind(uc, i); }
inline ustring::size_type ustring::find_first_of(const ustring& match, size_type i) const { return storage.find_first_of(match.storage, i); }
inline ustring::size_type ustring::find_first_of(gunichar uc, size_type i) const { return storage.find_first_of(uc, i); }
inline ustring::size_type ustring::find_last_of(const ustring& match, size_type i) const { return storage.find_last_of(match.storage, i); }
inline ustring::size_type ustring::find_last_of(gunichar uc, size_type i) const { return storage.find_last_of(uc, i); }
inline ustring::size_type ustring::find_first_not_of(const ustring& match, size_type i) const { return storage.find_first_not_of(match.storage, i); }
inline ustring::size_type ustring::find_first_not_of(gunichar uc, size_type i) const { return storage.find_first_not_of(uc, i); }
inline ustring::size_type ustring::find_last_not_of(const ustring& match, size_type i) const { return storage.find_last_not_of(match.storage, i); }
inline ustring::size_type ustring::find_last_not_of(gunichar uc, size_type i) const { return storage.find_last_not_of(uc, i); }
inline bool ustring::empty() const { return storage.empty(); }
inline ustring::size_type ustring::size() const { return storage.size(); }
inline ustring::size_type ustring::length() const { return storage.length(); }
inline ustring::size_type ustring::bytes() const { return storage.bytes(); }
inline void ustring::resize(size_type n, gunichar uc) { storage.resize(n, uc); }
inline ustring::size_type ustring::capacity() const { return storage.capacity(); }
inline ustring::size_type ustring::max_size() const { return storage.max_size(); }
inline void ustring::reserve(size_type n) { storage.reserve(n); }
inline const std::string& ustring::raw() const { return storage.raw(); }
inline bool ustring::validate() const { return storage.validate(); }
inline bool ustring::validate(iterator& first_invalid) { return storage.validate(first_invalid); }
inline bool ustring::validate(const_iterator& first_invalid) const { return storage.validate(first_invalid); }
inline bool ustring::is_ascii() const { return storage.is_ascii(); }
//inline ustring ustring::normalize(NormalizeMode mode) const { return storage.normalize(mode); }
inline ustring ustring::uppercase() const { return storage.uppercase(); }
inline ustring ustring::lowercase() const { return storage.lowercase(); }
inline ustring ustring::casefold() const { return storage.casefold(); }

inline bool operator<(const ustring& lhs, const ustring& rhs) { return lhs.storage < rhs.storage; }
inline bool operator==(const ustring& lhs, const ustring& rhs) { return lhs.storage == rhs.storage; }
inline bool operator!=(const ustring& lhs, const ustring& rhs) { return lhs.storage != rhs.storage; }

} // namespace k3d

#endif // !K3DSDK_USTRING_H

