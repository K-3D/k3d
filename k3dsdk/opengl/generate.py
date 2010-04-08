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

def return_type(function):
	return k3d_types[function["return"]]

def full_return_type(function):
	result = k3d_types[function["return"]]
	if result == "const GLubyte *":
		return "const api::GLubyte *"
	if result != "void":
		return "api::" + result
	return result

def parameter_type(parameter):
	result = ""
	if parameter["cardinality"] == "array" and parameter["direction"] == "in":
		result += "const "
	result += k3d_types[parameter["type"]]
	if parameter["cardinality"] == "array":
		result += "*"
	return result

def full_parameter_type(parameter):
	result = ""
	if parameter["cardinality"] == "array" and parameter["direction"] == "in":
		result += "const "
	if k3d_types[parameter["type"]] != "void":
		result += "api::"
	result += k3d_types[parameter["type"]]
	if parameter["cardinality"] == "array":
		result += "*"
	return result

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
			type = return_value.group(1)
			function["return"] = type
			continue

		parameter = re.search("param\s+([^\s]+)\s+([^\s]+)\s+(in|out)\s+(value|array)", line)
		if parameter:
			name = parameter.group(1)
			type = parameter.group(2)
			direction = parameter.group(3)
			cardinality = parameter.group(4)

			function["parameters"].append({"name":name,"type":type,"direction":direction,"cardinality":cardinality})
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

	template<typename FunctorT>
	void load(const FunctorT& Functor)
	{
""")

for function in sorted(functions, lambda lhs, rhs: cmp(lhs["name"], rhs["name"])):
	stream.write("\t\tFunctor(\"gl" + function["name"] + "\", gl" + function["name"] + ");\n")

stream.write("""\t}\n\n""")

for function in sorted(functions, lambda lhs, rhs: cmp(lhs["name"], rhs["name"])):

	stream.write("\t")
	stream.write(return_type(function))
	stream.write(" (*gl" + function["name"] + ")")
	stream.write("(")

	for p in range(len(function["parameters"])):

		if p:
			stream.write(", ")

		parameter = function["parameters"][p]

		stream.write(parameter_type(parameter))
		stream.write(" ")
		stream.write(parameter["name"])

	stream.write(");\n")

stream.write("""};

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

#include <k3dsdk/log.h>
#include <k3dsdk/opengl/api.h>

namespace k3d
{

namespace gl
{

""")

for function in sorted(functions, lambda lhs, rhs: cmp(lhs["name"], rhs["name"])):

	stream.write("static ")
	stream.write(full_return_type(function))
	stream.write(" fallback" + function["name"])
	stream.write("(")

	for p in range(len(function["parameters"])):

		if p:
			stream.write(", ")

		parameter = function["parameters"][p]

		stream.write(full_parameter_type(parameter))
		stream.write(" ")
		stream.write(parameter["name"])

	stream.write(")\n")
	stream.write("{\n")
	stream.write("\tlog() << warning << \"gl" + function["name"] + " not available.\" << std::endl;\n")
	stream.write("}\n\n")

stream.write("""api::api() :""")

index = False
for function in sorted(functions, lambda lhs, rhs: cmp(lhs["name"], rhs["name"])):

	if index:
		stream.write(",\n\t")
	else:
		stream.write("\n\t")
		index = True

	stream.write("gl" + function["name"] + "(fallback" + function["name"] + ")")

stream.write("""
{
}

} // namespace gl

} // namespace k3d
""")

