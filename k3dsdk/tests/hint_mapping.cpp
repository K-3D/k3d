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

namespace detail
{

//////////////////////////////////////////////////////////////////////////////
// connector

template<typename SlotT>
class connector
{
public:
	connector(sigc::signal<void, ihint*>& Source, const SlotT& Target) :
		m_source(Source),
		m_target(Target)
	{
	}

	~connector()
	{
	}

	template<typename ConverterT>
	void operator=(const ConverterT& Converter)
	{
		m_source.connect(Converter);
	}

private:
	sigc::signal<void, ihint*>& m_source;
	const SlotT& m_target;
};

class first_converter
{
public:
	void operator()(ihint* const Hint)
	{
		std::cerr << "unhandled hint: " << print(Hint) << std::endl;
	}
};

template<typename SourceT, typename TargetT, typename NextT>
class converter :
	private NextT
{
public:
	typedef converter<SourceT, TargetT, NextT> ThisT;

	template<typename NextSourceT, typename NextTargetT>
	converter<NextSourceT, NextTargetT, ThisT> convert()
	{
		return converter<NextSourceT, NextTargetT, ThisT>();
	}

	void operator()(ihint* const Hint)
	{
		if(hint_traits<SourceT>::match(Hint))
		{
			std::cerr << "input hint: " << print(Hint) << " output hint: " << print(hint_traits<TargetT>::convert(Hint)) << std::endl;
		}
		else
		{
			NextT::operator()(Hint);
		}
	}
};

class converter_helper
{
public:
	template<typename SourceT, typename TargetT>
	converter<SourceT, TargetT, first_converter> convert()
	{
		return converter<SourceT, TargetT, first_converter>();
	}
};

} // namespace detail

//////////////////////////////////////////////////////////////////////////////
// connection

template<typename SlotT>
detail::connector<SlotT> connection(sigc::signal<void, ihint*>& Source, const SlotT& Target)
{
	return detail::connector<SlotT>(Source, Target);
}

detail::converter_helper converter()
{
	return detail::converter_helper();
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
	k3d::hint::connection(input_bitmap, output_bitmap.make_slot()) = k3d::hint::converter()
		.convert<k3d::hint::bitmap_dimensions_changed, k3d::hint::unchanged>()
		.convert<k3d::hint::bitmap_pixels_changed, k3d::hint::unchanged>()
		.convert<k3d::hint::any, k3d::hint::none>();

	k3d::hint::connection(border, output_bitmap.make_slot()) = k3d::hint::converter()
		.convert<k3d::hint::any, k3d::hint::bitmap_dimensions_changed>();

	k3d::hint::connection(color, output_bitmap.make_slot()) = k3d::hint::converter()
		.convert<k3d::hint::any, k3d::hint::bitmap_pixels_changed>();

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

