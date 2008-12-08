#python

# At one point on OSX, I was accidentally linking our Python code against the system
# Python (2.3), while boost.python was linked against a later version of Python (2.5)
# provided by MacPorts.  This didn't cause link errors, and the 'k3d' module could be
# loaded without error at runtime, but the module itself was empty.
#
# This test checks the contents of the 'k3d' module to ensure that it isn't empty.
# You will have to modify this script whenever new symbols are added-to or removed-from
# the 'k3d' module.

import k3d

expected_contents = ['__doc__', '__name__', 'almost_equal', 'angle_axis', 'angle_order', 'attribute_arrays', 'batch_mode', 'bicubic_patch', 'bilinear_patch', 'bitmap', 'blobby', 'bounding_box3', 'check_node_environment', 'close_document', 'color', 'command_nodes', 'cone', 'const_attribute_arrays', 'const_bitmap', 'const_named_arrays', 'const_named_attribute_arrays', 'const_nurbs_curve_groups', 'const_nurbs_patches', 'const_polyhedra', 'const_typed_array_bool_t', 'const_typed_array_color', 'const_typed_array_double_t', 'const_typed_array_imaterial', 'const_typed_array_inode', 'const_typed_array_int16_t', 'const_typed_array_int32_t', 'const_typed_array_int64_t', 'const_typed_array_int8_t', 'const_typed_array_matrix4', 'const_typed_array_mesh_polyhedra_t_polyhedron_type', 'const_typed_array_normal3', 'const_typed_array_point2', 'const_typed_array_point3', 'const_typed_array_point4', 'const_typed_array_string_t', 'const_typed_array_texture3', 'const_typed_array_uint16_t', 'const_typed_array_uint32_t', 'const_typed_array_uint64_t', 'const_typed_array_uint8_t', 'const_typed_array_vector2', 'const_typed_array_vector3', 'const_uint_t_array', 'create_plugin', 'cubic_curve', 'cylinder', 'disk', 'documents', 'dynamic_cast', 'euler', 'euler_angles', 'execute_script', 'exit', 'generic_path', 'get_command_node', 'hyperboloid', 'icommand_node', 'identity3', 'idocument', 'imaterial', 'imesh_storage', 'imetadata', 'inode', 'inode_selection', 'intersect_lines', 'iplugin_factory', 'iproperty', 'iproperty_collection', 'is_solid', 'is_triangles', 'is_uninitialized', 'isnappable', 'iunknown', 'iuser_interface', 'length', 'linear_curve', 'log', 'log_critical', 'log_debug', 'log_error', 'log_info', 'log_warning', 'matrix4', 'mesh', 'mesh_selection', 'mime', 'named_arrays', 'named_attribute_arrays', 'native_path', 'new_document', 'node', 'normal3', 'nurbs_curve', 'nurbs_patch', 'nurbs_curve_groups', 'nurbs_patches', 'open_document', 'paraboloid', 'parallel', 'path', 'plugin', 'plugins', 'point_group', 'point2', 'point3', 'point4', 'polyhedra', 'polyhedron', 'polyhedron_type', 'print_diff', 'resource', 'ri', 'rotate3', 'scale3', 'share_path', 'sphere', 'teapot', 'texture3', 'torus', 'to_vector3', 'translate3', 'typed_array_bool_t', 'typed_array_color', 'typed_array_double_t', 'typed_array_imaterial', 'typed_array_inode', 'typed_array_int16_t', 'typed_array_int32_t', 'typed_array_int64_t', 'typed_array_int8_t', 'typed_array_matrix4', 'typed_array_mesh_polyheda_t_polyhedron_type', 'typed_array_normal3', 'typed_array_point2', 'typed_array_point3', 'typed_array_point4', 'typed_array_string_t', 'typed_array_texture3', 'typed_array_uint16_t', 'typed_array_uint32_t', 'typed_array_uint64_t', 'typed_array_uint8_t', 'typed_array_vector2', 'typed_array_vector3', 'ui', 'uint_t_array', 'uuid', 'validate', 'vector2', 'vector3']

actual_contents = dir(k3d)

error = False
for expected in expected_contents:
	if expected not in actual_contents:
		error = True
		print "Missing symbol", expected

for actual in actual_contents:
	if actual not in expected_contents:
		error = True
		print "Unexpected symbol", actual

if error:
	raise Exception("Python module 'k3d' contents changed unexpectedly")

