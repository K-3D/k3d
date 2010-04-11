#include <k3dsdk/bitmap.h>
#include <k3dsdk/color.h>
#include <k3dsdk/data.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/pointer_demand_storage.h>
#include <k3dsdk/types.h>

#include <iostream>

void signal_changed(k3d::ihint* Hint, const k3d::string_t& Message)
{
	std::cerr << Message << ": " << k3d::hint::print(Hint) << std::endl;
}

void property_executed(const std::vector<k3d::ihint*>& Hints, k3d::bitmap& Bitmap)
{
	for(size_t i = 0; i != Hints.size(); ++i)
		std::cerr << "executing with: " << k3d::hint::print(Hints[i]) << std::endl;
}

int main(int argc, char* arv[])
{
	// Setup some example signals ...
	k3d_data(k3d::bitmap*, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) input_bitmap(init_value<k3d::bitmap*>(0));
	k3d_data(k3d::uint32_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) border(init_value<k3d::uint32_t>(0));
	k3d_data(k3d::color, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) color(init_value(k3d::color(0, 0, 0)));
	k3d_data(k3d::bitmap*, no_name, change_signal, no_undo, pointer_demand_storage, no_constraint, no_property, no_serialization) output_bitmap(init_value<k3d::bitmap*>(0));

	// We want to print to the console whenever a signal is emitted ...
	input_bitmap.changed_signal().connect(sigc::bind(sigc::ptr_fun(signal_changed), "input changed"));
	border.changed_signal().connect(sigc::bind(sigc::ptr_fun(signal_changed), "border changed"));
	color.changed_signal().connect(sigc::bind(sigc::ptr_fun(signal_changed), "color changed"));
	output_bitmap.changed_signal().connect(sigc::bind(sigc::ptr_fun(signal_changed), "output changed"));

	// We want to print whenever the output property is executed ...
	output_bitmap.set_update_slot(sigc::ptr_fun(property_executed));

	// Setup our "network" of mappings from input signals to output signals ...
	input_bitmap.changed_signal().connect(k3d::hint::converter<
		k3d::hint::convert<k3d::hint::bitmap_dimensions_changed, k3d::hint::unchanged,
		k3d::hint::convert<k3d::hint::bitmap_pixels_changed, k3d::hint::unchanged,
		k3d::hint::convert<k3d::hint::any, k3d::hint::none> > > >(output_bitmap.make_slot()));

	border.changed_signal().connect(k3d::hint::converter<
		k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_dimensions_changed> >(output_bitmap.make_slot()));

	color.changed_signal().connect(k3d::hint::converter<
		k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_pixels_changed> >(output_bitmap.make_slot()));

	// Exercise the hint-mapping network ...
	std::cerr << "****************" << std::endl;
	input_bitmap.changed_signal().emit(k3d::hint::bitmap_dimensions_changed::instance());

	std::cerr << "****************" << std::endl;
	input_bitmap.changed_signal().emit(k3d::hint::bitmap_pixels_changed::instance());

	std::cerr << "****************" << std::endl;
	input_bitmap.changed_signal().emit(0);

	std::cerr << "****************" << std::endl;
	border.changed_signal().emit(0);

	std::cerr << "****************" << std::endl;
	color.changed_signal().emit(0);

	std::cerr << "****************" << std::endl;
	output_bitmap.internal_value();

	std::cerr << "****************" << std::endl;
	output_bitmap.internal_value();

	return 0;
}

