#include <k3dsdk/iunknown.h>
#include <k3dsdk/signal_system.h>
#include <k3dsdk/types.h>

#include <iostream>

namespace k3d
{

/// Abstract interface implemented by objects that can act as "hints" for change events
class ihint :
	public virtual iunknown
{
public:
	virtual void print(std::ostream& Stream) = 0;

protected:
	ihint() {}
	ihint(const ihint&) {}
	ihint& operator=(const ihint&) { return *this; }
	virtual ~ihint() {}
};

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

/// iostream-compatible manipulator object that serializes information about a hint object
class print
{
public:
	print(ihint* Hint);

	ihint* const hint;
};

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const print& RHS);

//////////////////////////////////////////////////////////////////////////////
// print

print::print(ihint* Hint) :
	hint(Hint)
{
}

std::ostream& operator<<(std::ostream& Stream, const print& RHS)
{
	if(RHS.hint)
		RHS.hint->print(Stream);
	else
		Stream << "(none)";
	
	return Stream;
}

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
	sigc::signal<void, k3d::ihint*> input_bitmap;
	sigc::signal<void, k3d::ihint*> border;
	sigc::signal<void, k3d::ihint*> color;
	sigc::signal<void, k3d::ihint*> output_bitmap;

	// We want to print to the console whenever a signal is emitted ...
	input_bitmap.connect(sigc::bind(sigc::ptr_fun(signal_changed), "input changed"));
	border.connect(sigc::bind(sigc::ptr_fun(signal_changed), "border changed"));
	color.connect(sigc::bind(sigc::ptr_fun(signal_changed), "color changed"));
	output_bitmap.connect(sigc::bind(sigc::ptr_fun(signal_changed), "output changed"));

	// Setup our "network" of mappings from input signals to output signals ...
	k3d::hint::connect(input_bitmap, k3d::hint::converter<
		k3d::hint::convert<k3d::hint::bitmap_dimensions_changed, k3d::hint::unchanged,
		k3d::hint::convert<k3d::hint::bitmap_pixels_changed, k3d::hint::unchanged,
		k3d::hint::convert<k3d::hint::any, k3d::hint::none> > > >(output_bitmap.make_slot()));

	k3d::hint::connect(border, k3d::hint::converter<
		k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_dimensions_changed> >(output_bitmap.make_slot()));

	k3d::hint::connect(color, k3d::hint::converter<
		k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_pixels_changed> >(output_bitmap.make_slot()));

	// Exercise the hint-mapping network ...
	std::cerr << "****************" << std::endl;
	input_bitmap.emit(&k3d::hint::bitmap_dimensions_changed());

	std::cerr << "****************" << std::endl;
	input_bitmap.emit(&k3d::hint::bitmap_pixels_changed());

	std::cerr << "****************" << std::endl;
	input_bitmap.emit(0);

	std::cerr << "****************" << std::endl;
	border.emit(0);

	std::cerr << "****************" << std::endl;
	color.emit(0);

	return 0;
}

