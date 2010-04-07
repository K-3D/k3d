import fileinput
import re
import sys

# Parse the type-map so we can convert from OpenGL to K-3D types ...
k3d_types = {}

for line in fileinput.input("gl.tm"):
	if line[0:1] == "#":
		continue

	type = re.search("([^,]+),[*],[*],\s*([^,]+)", line)
	k3d_types[type.group(1)] = type.group(2)

k3d_types["void"] = "void"

# Parse OpenGL function signatures
functions = []
function = None

for line in fileinput.input("gl.spec"):
	if line[0:1] == "#":
		continue

	if line[0:3] == "@@@":
		continue

	if line.strip() == "":
		continue

	if line.count(":"):
		continue

	if line[0:1] == "\t":
		return_value = re.search("return\s+(.*)", line)
		if return_value:
			function["return"] = return_value.group(1)
			continue

		parameter = re.search("param\s+([^\s]+)\s+([^\s]+)\s+(in|out)\s+(value|array)", line)
		if parameter:
			function["parameters"].append({"name":parameter.group(1),"type":parameter.group(2),"direction":parameter.group(3),"cardinality":parameter.group(4)})
			continue

	else:
		if function:
			functions.append(function)

		function = { "name" : re.search("[^(]+", line).group(0), "parameters" : [] }

if function:
	functions.append(function)





# Generate the K-3D OpenGL API header ...
stream = open("api.h", "w")

stream.write("""#ifndef K3DSDK_OPENGL_API_H
#define K3DSDK_OPENGL_API_H

/////////////////////////////////////////////////////////////////////////////////
//
// Generated file, do not edit by hand!
//
/////////////////////////////////////////////////////////////////////////////////

#include <k3dsdk/types.h>

namespace k3d
{

namespace gl
{

class api
{
public:
	typedef void GLvoid;
	typedef char GLchar;
	typedef int8_t GLbyte;
	typedef int16_t GLshort;
	typedef int32_t GLint;
	typedef int32_t GLsizei;
	typedef float_t GLfloat;
	typedef float_t GLclampf;
	typedef double_t GLdouble;
	typedef double_t GLclampd;
	typedef uint8_t GLubyte;
	typedef uint8_t GLboolean;
	typedef uint16_t GLushort;
	typedef uint32_t GLuint;
	typedef uint32_t GLenum;
	typedef uint32_t GLbitfield;

	typedef ptrdiff_t GLintptr;
	typedef ptrdiff_t GLsizeiptr;
	typedef ptrdiff_t GLintptrARB;
	typedef ptrdiff_t GLsizeiptrARB;
	typedef char GLcharARB;
	typedef unsigned int GLhandleARB;
	typedef unsigned short GLhalfARB;
	typedef unsigned short GLhalfNV;

	typedef int64_t GLint64EXT;
	typedef uint64_t GLuint64EXT;

	typedef int64_t GLint64;
	typedef uint64_t GLuint64;
	typedef struct __GLsync *GLsync;


	api();

""")

for function in sorted(functions, lambda lhs, rhs: cmp(lhs["name"], rhs["name"])):

	stream.write("\t")
	stream.write(k3d_types[function["return"]])
	stream.write(" (*gl" + function["name"] + ")")
	stream.write("(")

	for p in range(len(function["parameters"])):

		if p:
			stream.write(", ")

		parameter = function["parameters"][p]

		if parameter["cardinality"] == "array" and parameter["direction"] == "in":
			stream.write("const ")
		stream.write(k3d_types[parameter["type"]])
		if parameter["cardinality"] == "array":
			stream.write("*")
		stream.write(" ")
		stream.write(parameter["name"])

	stream.write(");\n")

stream.write("""
};

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_OPENGL_API_H
""")






# Generate the K-3D OpenGL API implementation ...
stream = open("api.cpp", "w")

stream.write("""/////////////////////////////////////////////////////////////////////////////////
//
// Generated file, do not edit by hand!
//
/////////////////////////////////////////////////////////////////////////////////

#include <k3dsdk/opengl/api.h>

namespace k3d
{

namespace gl
{

api::api() :""")

index = False
for function in sorted(functions, lambda lhs, rhs: cmp(lhs["name"], rhs["name"])):

	if index:
		stream.write(",\n\t")
	else:
		stream.write("\n\t")
		index = True

	stream.write("gl" + function["name"] + "(0)")

stream.write("""
{
}

} // namespace gl

} // namespace k3d
""")

