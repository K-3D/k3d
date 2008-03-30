// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include "mesh.h"
#include <boost/any.hpp>

/*

Excerpted from http://mail.python.org/pipermail/c++-sig/2003-July/004906.html:

Most of the subtle problems with dynamic linking C++ have to do with
symbols C++ generates implicitly:

    RTTI and EH information
    virtual tables
    static data members of class template instantiations
    static variables in function template instantiations

Typically, in the Global model this information (known as "weak
symbols" on Linux) is generated in every translation unit where it is
used, and resolved away at link time.  Of course, that leaves us with
a single copy per link unit (executable or shared library), and which
copy is used needs to be resolved at load time.**

The solutions generally come down to controlling where those symbols
are generated and how they are shared across boundaries.

Whether the first two things are a problem for your application
depends in part on the ABI (application binary interface) of your
compiler.  The ABI encompasses issues such as function calling
convention, class layout, the format of EH tables, the format of RTTI
information, and of course, the procedures the compiler uses to
manipulate that information.

Your problem with boost::any was an interaction with the compiler's
implementation of dynamic_cast.  Recent GCCs determine dynamic type
equivalence by comparing the *addresses* (not the contents) of their
RTTI information.  Because your two extension modules are not sharing
any symbols other than those in the Boost.Python library, each one
gets its own copy of the RTTI for the class in the any object, and the
template instantiation of the any_cast on each side of the library
boundary uses a different copy.  The solution is to get that RTTI
information generated in a place that both libraries can see.  The
best way to achieve that is to link both extension modules to a common
shared library using the global model, and put the RTTI information
there.  For the case in question, you might achieve that by explicitly
instantiating the any_cast template there.

The problem we had earlier with catching exceptions is an interaction
with the compiler's implementation of EH and the behavior of glibc
under dynamic linking.  Recent glibc implementations have an
"inconvenient" (charitably speaking) behavior with weak symbols.  The
result is that if the symbol appears in both extension modules *and*
in the Boost.Python shared library, the first extension module loaded
will share a copy with the Boost.Python library and each module loaded
thereafter will get its own copy.  Fortunately, I knew that classes
with non-inline virtual functions are different, and usually a
*strong* symbol gets generated in the translation unit containing the
class' first non-inline virtual function.  By adding a virtual
destructor in the Boost.Python library we ensured that both extension
modules got the same copy of the exception's EH info.

>>The Windows crash, FWIW, is a poor-QOI issue "bug" in the Dinkumware
>>standard library that ships with vc6.  vc7 solves it:

This problem is that the vc6 associative containers use a special
static data member representing a "NULL" node, used as a sentinel to
tell the container implementation where the "edge" is, so it would
stop searching there.  Because you instantiated the map<> template in
two separate extension modules, each one got it's own copy of the
"NULL" node, and a map passed across the boundary would contain the
wrong sentinel value for the code on the other side.

-- 
Dave Abrahams
Boost Consulting
www.boost-consulting.com

*/

template k3d::mesh* boost::any_cast<k3d::mesh*>(boost::any&);
template k3d::mesh* boost::any_cast<k3d::mesh*>(const boost::any&);

