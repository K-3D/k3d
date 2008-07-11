#include <k3dsdk/bitmap.h>
#include <k3dsdk/color.h>
#include <k3dsdk/data.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/signal_system.h>
#include <k3dsdk/types.h>
#include <k3dsdk/utility.h>

#include <iostream>

namespace k3d
{

namespace data
{

/////////////////////////////////////////////////////////////////////////////
// pointer_demand_storage

/// Read-only storage policy that stores a value by pointer, created on-demand
template<typename pointer_t, typename signal_policy_t>
class pointer_demand_storage :
	public signal_policy_t
{
	// This policy only works for data stored by-pointer
	BOOST_STATIC_ASSERT((boost::is_pointer<pointer_t>::value));

public:
	typedef typename boost::remove_pointer<pointer_t>::type non_pointer_t;
	typedef pointer_demand_storage<pointer_t, signal_policy_t> this_t;
	typedef std::vector<ihint*> pending_hints_t;

	/// Set the slot that will be called to bring the underlying data up-to-date
	void set_update_slot(const sigc::slot<void, const pending_hints_t&, non_pointer_t&>& Slot)
	{
		m_update_slot = Slot;
		update();
	}

/*
	/// Returns a slot that will invoke the reset() method
	sigc::slot<void, ihint*> make_reset_slot()
	{
		return sigc::bind<0>(sigc::mem_fun(*this, &this_t::reset), static_cast<pointer_t>(0));
	}
*/

	/// Returns a slot that will invoke the update() method
	sigc::slot<void, ihint*> make_slot()
	{
		return sigc::mem_fun(*this, &this_t::update);
	}

/*
	/// Store an object as the new value, taking control of its lifetime
	void reset(pointer_t NewValue = 0, ihint* const Hint = 0)
	{
		// Ensure that our value doesn't go out-of-scope while it's being modified
		if(m_executing)
			return;

		m_value.reset(NewValue);
		signal_policy_t::set_value(Hint);
	}
*/

	/// Schedule an update for the value the next time it's read
	void update(ihint* const Hint = 0)
	{
		m_pending_hints.push_back(Hint ? Hint->clone() : static_cast<ihint*>(0));
		signal_policy_t::set_value(Hint);
	}

	/// Accesses the underlying value, creating it if it doesn't already exist
	pointer_t internal_value()
	{
		if(!m_value.get())
			m_value.reset(new non_pointer_t());

		if(!m_pending_hints.empty())
		{
			// Create a temporary copy of pending hints in-case we are updated while executing ...
			const pending_hints_t pending_hints(m_pending_hints);
			m_update_slot(pending_hints, *m_value);
			
			std::for_each(m_pending_hints.begin(), m_pending_hints.end(), delete_object());
			m_pending_hints.clear();
		}

		return m_value.get();
	}

protected:
	template<typename init_t>
	pointer_demand_storage(const init_t& Init) :
		signal_policy_t(Init)
	{
	}

	~pointer_demand_storage()
	{
		std::for_each(m_pending_hints.begin(), m_pending_hints.end(), delete_object());
	}

private:
	/// Storage for this policy's value
	std::auto_ptr<non_pointer_t> m_value;
	/// Stores a slot that will be called to bring this policy's value up-to-date
	sigc::slot<void, const pending_hints_t&, non_pointer_t&> m_update_slot;
	/// Stores a collection of pending hints to be updated
	pending_hints_t m_pending_hints;
};

} // namespace data

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
	ihint* clone()
	{
		return new bitmap_dimensions_changed();
	}

	void print(std::ostream& Stream)
	{
		Stream << "bitmap_dimensions_changed";
	}

	static bitmap_dimensions_changed* instance()
	{
		static bitmap_dimensions_changed hint;
		return &hint;
	}
};

//////////////////////////////////////////////////////////////////////////////
// bitmap_pixels_changed

class bitmap_pixels_changed :
	public ihint
{
public:
	ihint* clone()
	{
		return new bitmap_pixels_changed();
	}

	void print(std::ostream& Stream)
	{
		Stream << "bitmap_pixels_changed";
	}

	static bitmap_pixels_changed* instance()
	{
		static bitmap_pixels_changed hint;
		return &hint;
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

