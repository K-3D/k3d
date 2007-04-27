#include <k3dsdk/data.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/vectors.h>

#include <iostream>

#define print_size(type) \
	std::cout << "sizeof(" << #type << "): " << sizeof(type) << std::endl;

int main(int argc, char* argv[])
{
	print_size(sigc::connection);
		
	print_size(k3d_data(bool, no_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(bool, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(bool, immutable_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(bool, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	
	print_size(k3d_data(long, no_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(long, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(long, immutable_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(long, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	
	print_size(k3d_data(double, no_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(double, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(double, immutable_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	
	print_size(k3d_data(std::string, no_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(std::string, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(std::string, immutable_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(std::string, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	
	print_size(k3d_data(k3d::point3, no_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(k3d::point3, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(k3d::point3, immutable_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));
	print_size(k3d_data(k3d::point3, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization));

	k3d_data(long, no_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) a(init_value(0));
	assert(a.internal_value() == a.value() && a.internal_value() == 0);

	k3d_data(long, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) b(init_value(1));
	assert(b.internal_value() == b.value() && b.internal_value() == 1);

	k3d_data(long, immutable_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) c(init_value(2) + init_name("c"));
	assert(c.name() == "c" && c.internal_value() == c.value() && c.internal_value() == 2);

	k3d_data(long, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) d(init_value(3) + init_name("d"));
	assert(d.name() == "d" && d.internal_value() == d.value() && d.internal_value() == 3);

	k3d_data(long, immutable_name, no_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) e(init_value(4) + init_name("e"));
	e.set_value(5);
	assert(e.name() == "e" && e.internal_value() == e.value() && e.internal_value() == 5);

	k3d_data(long, immutable_name, no_signal, no_undo, local_storage, with_constraint, no_property, no_serialization) f(init_value(6) + init_name("f") + init_constraint(constraint::minimum(7L)));
	assert(f.name() == "f" && f.internal_value() == f.value() && f.internal_value() == 6);
	f.set_value(0);
	assert(f.internal_value() == 7);

	k3d_data(long, immutable_name, no_signal, no_undo, local_storage, with_constraint, no_property, no_serialization) g(init_value(8) + init_name("g") + init_constraint(constraint::maximum(9L)));
	assert(g.name() == "g" && g.internal_value() == g.value() && g.internal_value() == 8);
	g.set_value(10);
	assert(g.internal_value() == 9);

	return 0;
}

