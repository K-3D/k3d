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

//////////////////////////////////////////////////////////////////////////////
// filtered_slot

template<typename InputT, typename OutputT, typename SlotT>
class filtered_slot
{
public:
	filtered_slot(const SlotT& Slot) :
		slot(Slot)
	{
	}

	void operator()(ihint* const Hint)
	{
		if(hint_traits<InputT>::match(Hint))
			slot(hint_traits<OutputT>::convert(Hint));
	}

private:
	SlotT slot;
};

template<typename InputT, typename OutputT, typename SlotT>
filtered_slot<InputT, OutputT, SlotT> make_filter_slot(const SlotT& Slot)
{
	return filtered_slot<InputT, OutputT, SlotT>(Slot);
}

} // namespace hint

} // namespace k3d

void input_bitmap_changed(k3d::ihint* Hint)
{
	std::cerr << "input bitmap changed: " << k3d::hint::print(Hint) << std::endl;
}

void border_changed(k3d::ihint* Hint)
{
	std::cerr << "border changed: " << k3d::hint::print(Hint) << std::endl;
}

void color_changed(k3d::ihint* Hint)
{
	std::cerr << "color changed: " << k3d::hint::print(Hint) << std::endl;
}

void output_bitmap_changed(k3d::ihint* Hint)
{
	std::cerr << "output bitmap changed: " << k3d::hint::print(Hint) << std::endl;
}

int main(int argc, char* arv[])
{
	sigc::signal<void, k3d::ihint*> input_bitmap_signal;
	sigc::signal<void, k3d::ihint*> border_signal;
	sigc::signal<void, k3d::ihint*> color_signal;
	sigc::signal<void, k3d::ihint*> output_bitmap_signal;

	input_bitmap_signal.connect(sigc::ptr_fun(input_bitmap_changed));
	border_signal.connect(sigc::ptr_fun(border_changed));
	color_signal.connect(sigc::ptr_fun(color_changed));
	output_bitmap_signal.connect(sigc::ptr_fun(output_bitmap_changed));

	input_bitmap_signal.connect(k3d::hint::make_filter_slot<k3d::hint::bitmap_dimensions_changed, k3d::hint::unchanged>(output_bitmap_signal.make_slot()));
	input_bitmap_signal.connect(k3d::hint::make_filter_slot<k3d::hint::bitmap_pixels_changed, k3d::hint::unchanged>(output_bitmap_signal.make_slot()));
	input_bitmap_signal.connect(k3d::hint::make_filter_slot<k3d::hint::any, k3d::hint::none>(output_bitmap_signal.make_slot()));
	border_signal.connect(k3d::hint::make_filter_slot<k3d::hint::any, k3d::hint::bitmap_dimensions_changed>(output_bitmap_signal.make_slot()));
	color_signal.connect(k3d::hint::make_filter_slot<k3d::hint::any, k3d::hint::bitmap_pixels_changed>(output_bitmap_signal.make_slot()));

	std::cerr << "****************" << std::endl;
	input_bitmap_signal.emit(&k3d::hint::bitmap_dimensions_changed());

	std::cerr << "****************" << std::endl;
	input_bitmap_signal.emit(&k3d::hint::bitmap_pixels_changed());

	std::cerr << "****************" << std::endl;
	input_bitmap_signal.emit(0);

	std::cerr << "****************" << std::endl;
	border_signal.emit(0);

	std::cerr << "****************" << std::endl;
	color_signal.emit(0);

	return 0;
}

