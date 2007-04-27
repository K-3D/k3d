// ============================================================================
// gzstream, C++ iostream classes wrapping the zlib compression library.
// Copyright (C) 2001  Deepak Bandyopadhyay, Lutz Kettner
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// ============================================================================
//
// File          : gzstream.C
// Revision      : $Revision: 1.3 $
// Revision_date : $Date: 2007/02/17 01:35:57 $
// Author(s)     : Deepak Bandyopadhyay, Lutz Kettner
//
// Standard streambuf implementation following Nicolai Josuttis, "The
// Standard C++ Library".
// ============================================================================

#include "gzstream.h"
#include "path.h"

#include <zlib.h>
#include <string.h>

namespace k3d
{

namespace filesystem
{

///////////////////////////////////////////////////////////////////////////////////
// gzstreambuf

class gzstreambuf :
	public std::streambuf
{
private:
	static const int bufferSize = 47+256;    // size of data buff totals 512 bytes under g++ for igzstream at the end.
	gzFile           file;               // file handle for compressed file
	char             buffer[bufferSize]; // data buffer
	char             opened;             // open/close state of stream
	int              mode;               // I/O mode

	int flush_buffer();

public:
	gzstreambuf() :
		opened(0)
	{
	setp( buffer, buffer + (bufferSize-1));
	setg( buffer + 4,     // beginning of putback area
	      buffer + 4,     // read position
	      buffer + 4);    // end position
	// ASSERT: both input & output capabilities will not be used together
	}
	int is_open() { return opened; }
	gzstreambuf* open(const filesystem::path& name, int open_mode);
	gzstreambuf* close();
	~gzstreambuf() { close(); }

	virtual int     overflow( int c = EOF);
	virtual int     underflow();
	virtual int     sync();
};

gzstreambuf* gzstreambuf::open(const filesystem::path& name, int open_mode) {
    if ( is_open())
        return (gzstreambuf*)0;
    mode = open_mode;
    // no append nor read/write mode
    if ((mode & std::ios::ate) || (mode & std::ios::app)
        || ((mode & std::ios::in) && (mode & std::ios::out)))
        return (gzstreambuf*)0;
    char  fmode[10];
    char* fmodeptr = fmode;
    if ( mode & std::ios::in)
        *fmodeptr++ = 'r';
    else if ( mode & std::ios::out)
        *fmodeptr++ = 'w';
    *fmodeptr++ = 'b';
    *fmodeptr = '\0';
    file = gzopen( name.native_filesystem_string().c_str(), fmode);
    if (file == 0)
        return (gzstreambuf*)0;
    opened = 1;
    return this;
}

gzstreambuf * gzstreambuf::close() {
    if ( is_open()) {
        sync();
        opened = 0;
        if ( gzclose( file) == Z_OK)
            return this;
    }
    return (gzstreambuf*)0;
}

int gzstreambuf::underflow() { // used for input buffer only
    if ( gptr() && ( gptr() < egptr()))
        return * reinterpret_cast<unsigned char *>( gptr());

    if ( ! (mode & std::ios::in) || ! opened)
        return EOF;
    // Josuttis' implementation of inbuf
    int n_putback = gptr() - eback();
    if ( n_putback > 4)
        n_putback = 4;
    memcpy( buffer + (4 - n_putback), gptr() - n_putback, n_putback);

    int num = gzread( file, buffer+4, bufferSize-4);
    if (num <= 0) // ERROR or EOF
        return EOF;

    // reset buffer pointers
    setg( buffer + (4 - n_putback),   // beginning of putback area
          buffer + 4,                 // read position
          buffer + 4 + num);          // end of buffer

    // return next character
    return * reinterpret_cast<unsigned char *>( gptr());
}

int gzstreambuf::flush_buffer() {
    // Separate the writing of the buffer from overflow() and
    // sync() operation.
    int w = pptr() - pbase();
    if ( gzwrite( file, pbase(), w) != w)
        return EOF;
    pbump( -w);
    return w;
}

int gzstreambuf::overflow( int c) { // used for output buffer only
    if ( ! ( mode & std::ios::out) || ! opened)
        return EOF;
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    if ( flush_buffer() == EOF)
        return EOF;
    return c;
}

int gzstreambuf::sync() {
    // Changed to use flush_buffer() instead of overflow( EOF)
    // which caused improper behavior with std::endl and flush(),
    // bug reported by Vincent Ricard.
    if ( pptr() && pptr() > pbase()) {
        if ( flush_buffer() == EOF)
            return -1;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////
// gzstreambase

gzstreambase::gzstreambase() :
	buf(new gzstreambuf())
{
	init(buf);
}

gzstreambase::gzstreambase(const filesystem::path& name, int mode) :
	buf(new gzstreambuf())
{
	init(buf);
	open(name, mode);
}

gzstreambase::~gzstreambase()
{
	buf->close();
	delete buf;
}

void gzstreambase::open(const filesystem::path& name, int open_mode)
{
	if(!buf->open(name, open_mode))
		clear(rdstate() | std::ios::badbit);
}

void gzstreambase::close()
{
	if(buf->is_open())
	{
		if(!buf->close())
			clear(rdstate() | std::ios::badbit);
	}
}

std::streambuf* gzstreambase::rdbuf()
{
	return buf;
}

///////////////////////////////////////////////////////////////////
// igzstream

igzstream::igzstream() :
	gzstreambase(),
	std::istream(buf)
{
}

igzstream::igzstream(const filesystem::path& name) :
	gzstreambase(name, std::ios::in),
	std::istream(buf)
{
}

std::streambuf* igzstream::rdbuf()
{
	return gzstreambase::rdbuf();
}

void igzstream::open(const filesystem::path& name)
{
	gzstreambase::open(name, std::ios::in);
}

////////////////////////////////////////////////////////////////////
// ogzstream

ogzstream::ogzstream() :
	gzstreambase(),
	std::ostream(buf)
{
}

ogzstream::ogzstream(const filesystem::path& name) :
	gzstreambase(name, std::ios::out),
	std::ostream(buf)
{
}

std::streambuf* ogzstream::rdbuf()
{
	return gzstreambase::rdbuf();
}

void ogzstream::open(const filesystem::path& name)
{
	gzstreambase::open(name, std::ios::out);
}

} // namespace filesystem

} // namespace k3d

