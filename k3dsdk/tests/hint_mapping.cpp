#include <k3dsdk/bitmap.h>
#include <k3dsdk/color.h>
#include <k3dsdk/data.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/signal_system.h>
#include <k3dsdk/types.h>

#include <iostream>

namespace k3d
{

namespace hint
{

//////////////////////////////////////////////////////////////////////////////
// slot_t

typedef sigc::slot<void, ihint*> slot_t;

//////////////////////////////////////////////////////////////////////////////
// bitmap_dimensions_changed

class bitmap_dimensions_changed :
	public ihint
{
public:
	void print(std::ostream& Stream)
	{
		Stream << "bitmap_dimensions_changed";
	}
};

//////////////////////////////////////////////////////////////////////////////
// bitmap_pixels_changed

class bitmap_pixels_changed :
	public ihint
{
public:
	void print(std::ostream& Stream)
	{
		Stream << "bitmap_pixels_changed";
	}
};

//////////////////////////////////////////////////////////////////////////////
// any

class any
{
public:
};

//////////////////////////////////////////////////////////////////////////////
// none

class none
{
public:
};

//////////////////////////////////////////////////////////////////////////////
// unchanged

class unchanged
{
public:
};

//////////////////////////////////////////////////////////////////////////////
// hint_traits

template<typename HintT>
class hint_traits
{
public:
};

template<>
class hint_traits<bitmap_dimensions_changed>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return dynamic_cast<bitmap_dimensions_changed*>(Hint);
	}

	static ihint* convert(ihint*)
	{
		static bitmap_dimensions_changed hint;
		return &hint;
	}
};

template<>
class hint_traits<bitmap_pixels_changed>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return dynamic_cast<bitmap_pixels_changed*>(Hint);
	}

	static ihint* convert(ihint*)
	{
		static bitmap_pixels_changed hint;
		return &hint;
	}
};

template<>
class hint_traits<any>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return true;
	}
};

template<>
class hint_traits<none>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return false;
	}

	static ihint* convert(ihint*)
	{
		return 0;
	}
};

template<>
class hint_traits<unchanged>
{
public:
	static ihint* convert(ihint* Hint)
	{
		return Hint;
	}
};

//////////////////////////////////////////////////////////////////////////////
// last_conversion

class last_conversion
{
};

//////////////////////////////////////////////////////////////////////////////
// convert

template<typename SourceT, typename TargetT, typename NextT = last_conversion>
struct convert
{
	typedef SourceT Source;
	typedef TargetT Target;
	typedef NextT Next;
};

namespace detail
{

template<typename ListT>
void execute(ihint* const Hint, const slot_t& Slot)
{
	if(hint_traits<typename ListT::Source>::match(Hint))
	{
		Slot(hint_traits<typename ListT::Target>::convert(Hint));
	}
	else
	{
		execute<typename ListT::Next>(Hint, Slot);
	}
}

template<>
void execute<last_conversion>(ihint* const Hint, const slot_t& Slot)
{
	std::cerr << "unhandled hint: " << print(Hint) << std::endl;
}

template<typename ListT>
class converter
{
public:
	converter(const slot_t& Slot) :
		slot(Slot)
	{
	}

	void operator()(ihint* const Hint)
	{
		execute<ListT>(Hint, slot);
	}

private:
	slot_t slot;
};

} // namespace detail

//////////////////////////////////////////////////////////////////////////////
// connect

template<typename SourceT, typename ConverterT>
sigc::connection connect(SourceT& Source, const ConverterT& Converter)
{
	return Source.connect(Converter);
}

//////////////////////////////////////////////////////////////////////////////
// converter

template<typename ListT>
detail::converter<ListT> converter(const sigc::slot<void, ihint*>& Slot)
{
	return detail::converter<ListT>(Slot);
}

} // namespace hint

} // namespace k3d

void signal_changed(k3d::ihint* Hint, const k3d::string_t& Message)
{
	std::cerr << Message << ": " << k3d::hint::print(Hint) << std::endl;
}

int main(int argc, char* arv[])
{
	// Setup some example signals ...
	k3d_data(k3d::bitmap*, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) input_bitmap(init_value<k3d::bitmap*>(0));
	k3d_data(k3d::uint32_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) border(init_value<k3d::uint32_t>(0));
	k3d_data(k3d::color, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) color(init_value(k3d::color(0, 0, 0)));
	k3d_data(k3d::bitmap*, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) output_bitmap(init_value<k3d::bitmap*>(0));

	// We want to print to the console whenever a signal is emitted ...
	input_bitmap.changed_signal().connect(sigc::bind(sigc::ptr_fun(signal_changed), "input changed"));
	border.changed_signal().connect(sigc::bind(sigc::ptr_fun(signal_changed), "border changed"));
	color.changed_signal().connect(sigc::bind(sigc::ptr_fun(signal_changed), "color changed"));
	output_bitmap.changed_signal().connect(sigc::bind(sigc::ptr_fun(signal_changed), "output changed"));

	// Setup our "network" of mappings from input signals to output signals ...
	k3d::hint::connect(input_bitmap.changed_signal(), k3d::hint::converter<
		k3d::hint::convert<k3d::hint::bitmap_dimensions_changed, k3d::hint::unchanged,
		k3d::hint::convert<k3d::hint::bitmap_pixels_changed, k3d::hint::unchanged,
		k3d::hint::convert<k3d::hint::any, k3d::hint::none> > > >(output_bitmap.changed_signal().make_slot()));

	k3d::hint::connect(border.changed_signal(), k3d::hint::converter<
		k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_dimensions_changed> >(output_bitmap.changed_signal().make_slot()));

	k3d::hint::connect(color.changed_signal(), k3d::hint::converter<
		k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_pixels_changed> >(output_bitmap.changed_signal().make_slot()));

	// Exercise the hint-mapping network ...
	std::cerr << "****************" << std::endl;
	input_bitmap.changed_signal().emit(&k3d::hint::bitmap_dimensions_changed());

	std::cerr << "****************" << std::endl;
	input_bitmap.changed_signal().emit(&k3d::hint::bitmap_pixels_changed());

	std::cerr << "****************" << std::endl;
	input_bitmap.changed_signal().emit(0);

	std::cerr << "****************" << std::endl;
	border.changed_signal().emit(0);

	std::cerr << "****************" << std::endl;
	color.changed_signal().emit(0);

	return 0;
}

