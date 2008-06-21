#include <k3dsdk/iunknown.h>
#include <k3dsdk/signal_system.h>

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

class bitmap_dimensions_changed :
	public ihint
{
public:
	void print(std::ostream& Stream)
	{
		Stream << "bitmap_dimensions_changed";
	}
};

class bitmap_pixels_changed :
	public ihint
{
public:
	void print(std::ostream& Stream)
	{
		Stream << "bitmap_pixels_changed";
	}
};

class any
{
public:
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

template<typename SlotT>
class unfiltered_slot
{
public:
	unfiltered_slot(const SlotT& Slot) :
		slot(Slot)
	{
	}

	void operator()(ihint* const Hint)
	{
		slot(Hint);
	}

private:
	SlotT slot;
};

template<typename SlotT>
unfiltered_slot<SlotT> make_filter_slot(const SlotT& Slot)
{
	return unfiltered_slot<SlotT>(Slot);
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

	input_bitmap_signal.connect(k3d::hint::make_filter_slot(output_bitmap_signal.make_slot()));
	border_signal.connect(k3d::hint::make_filter_slot(output_bitmap_signal.make_slot()));
	color_signal.connect(k3d::hint::make_filter_slot(output_bitmap_signal.make_slot()));

	std::cerr << "****************" << std::endl;
	border_signal.emit(0);

	std::cerr << "****************" << std::endl;
	color_signal.emit(0);

	std::cerr << "****************" << std::endl;
	input_bitmap_signal.emit(&k3d::hint::bitmap_dimensions_changed());

	std::cerr << "****************" << std::endl;
	input_bitmap_signal.emit(&k3d::hint::bitmap_pixels_changed());

	std::cerr << "****************" << std::endl;
	input_bitmap_signal.emit(0);

	return 0;
}

