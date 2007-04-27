#ifndef K3DSDK_MEASUREMENT_H
#define K3DSDK_MEASUREMENT_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\brief Provides a type-safe framework for doing real-world unit-of-measure conversion and I/O
		\author Tim Shead (tshead@k-3d.com)
*/

#include <cassert>
#include <iosfwd>
#include <map>
#include <string>

namespace k3d
{

namespace measurement
{

namespace conversion
{

/// Conversion policy for real world units that can be converted solely through multiplication
class multiplicative
{
public:
	const double to_si(const double RHS) const
	{
		return RHS * m_constant;
	}

	const double from_si(const double RHS) const
	{
		return RHS / m_constant;
	}

protected:
	explicit multiplicative(const double Constant) :
		m_constant(Constant)
	{
		assert(m_constant);
	}

private:
	double m_constant;
};

} // namespace conversion

/// Defines a real-world unit of measure, including singular written name, plural written name, and functions to_si() and from_si() for converting quantities to-and-from their corresponding SI standard units
template<typename conversion_policy>
class unit :
	public conversion_policy
{
public:
	typedef unit<conversion_policy> this_t;

	const std::string name() const
	{
		return m_name;
	}

	const std::string plural_name() const
	{
		return m_plural_name;
	}

	friend bool operator==(const this_t& LHS, const this_t& RHS)
	{
		return LHS.m_name == RHS.m_name;
	}

	friend bool operator<(const this_t& LHS, const this_t& RHS)
	{
		return LHS.m_name < RHS.m_name;
	}

protected:
/*
	explicit unit(const std::string Name, const std::string PluralName) :
		conversion_policy(),
		m_name(Name),
		m_plural_name(Name)
	{
		assert(m_name.size());
		assert(m_plural_name.size());
	}
*/

	explicit unit(const std::string Name, const std::string PluralName, const double MultiplicativeConstant) :
		conversion_policy(MultiplicativeConstant),
		m_name(Name),
		m_plural_name(PluralName)
	{
		assert(m_name.size());
		assert(m_plural_name.size());
	}

private:
	std::string m_name;
	std::string m_plural_name;
};

/// Defines a proxy object so we can do conversion-by-assignment
template<typename quantity_t>
class conversion_proxy
{
public:
	explicit conversion_proxy(const quantity_t Quantity) :
		m_quantity(Quantity)
	{
	}

	const double to_si() const
	{
		return m_quantity.to_si();
	}

private:
	const quantity_t m_quantity;
};

/// Convenience function for creating conversion_proxy objects
template<typename quantity_t>
conversion_proxy<quantity_t> convert(const quantity_t Quantity)
{
	return conversion_proxy<quantity_t>(Quantity);
}

/// Convenience function for converting a quantity to another unit-of-measure
template<typename quantity_t>
const quantity_t convert(const quantity_t Quantity, const typename quantity_t::unit_type Units)
{
	return quantity_t(Units.from_si(Quantity.to_si()), Units);
}

/// Defines a real-world quantity (a combination of value and unit)
template<typename unit_t>
class quantity
{
public:
	typedef unit_t unit_type;
	typedef quantity<unit_t> this_t;

	explicit quantity(const unit_t Units) :
		m_units(Units)
	{
	}

	explicit quantity(const double Value, const unit_t Units) :
		m_value(Value),
		m_units(Units)
	{
	}

	operator double() const
	{
		return m_value;
	}

	const double value() const
	{
		return m_value;
	}

	const unit_t& units() const
	{
		return m_units;
	}

	const double to_si() const
	{
		return m_units.to_si(m_value);
	}

	this_t& operator=(const conversion_proxy<this_t>& RHS)
	{
		m_value = m_units.from_si(RHS.to_si());
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const this_t& RHS)
	{
		Stream << RHS.m_value << " ";
		Stream << (1 == RHS.m_value ? RHS.m_units.name() : RHS.m_units.plural_name());
		return Stream;
	}

private:
	double m_value;
	unit_t m_units;
};

/// Defines a "scalar" quantity (no unit-of-measure)
typedef void scalar;

/// Defines an angular unit-of-measure
class angle :
	public unit<conversion::multiplicative>
{
public:
	explicit angle(const std::string Name, const std::string PluralName, const double Constant) :
		unit<conversion::multiplicative>(Name, PluralName, Constant)
	{
	}
};

/// Defines a unit-of-measure for area
class area :
	public unit<conversion::multiplicative>
{
public:
	explicit area(const std::string Name, const std::string PluralName, const double Constant) :
		unit<conversion::multiplicative>(Name, PluralName, Constant)
	{
	}
};

/// Defines a unit-of-measure for distance
class distance :
	public unit<conversion::multiplicative>
{
public:
	explicit distance(const std::string Name, const std::string PluralName, const double Constant) :
		unit<conversion::multiplicative>(Name, PluralName, Constant)
	{
	}
};

/// Defines a unit-of-measure for force
class force :
	public unit<conversion::multiplicative>
{
public:
	explicit force(const std::string Name, const std::string PluralName, const double Constant) :
		unit<conversion::multiplicative>(Name, PluralName, Constant)
	{
	}
};

/// Defines a unit-of-measure for mass
class mass :
	public unit<conversion::multiplicative>
{
public:
	explicit mass(const std::string Name, const std::string PluralName, const double Constant) :
		unit<conversion::multiplicative>(Name, PluralName, Constant)
	{
	}
};

/// Defines a unit-of-measure for pressure
class pressure :
	public unit<conversion::multiplicative>
{
public:
	explicit pressure(const std::string Name, const std::string PluralName, const double Constant) :
		unit<conversion::multiplicative>(Name, PluralName, Constant)
	{
	}
};

/// Defines a unit-of-measure for time
class time :
	public unit<conversion::multiplicative>
{
public:
	explicit time(const std::string Name, const std::string PluralName, const double Constant) :
		unit<conversion::multiplicative>(Name, PluralName, Constant)
	{
	}
};

/// Defines a unit-of-measure for volume
class volume :
	public unit<conversion::multiplicative>
{
public:
	explicit volume(const std::string Name, const std::string PluralName, const double Constant) :
		unit<conversion::multiplicative>(Name, PluralName, Constant)
	{
	}
};

/**

\brief Parses a mathematical expression with optional real-world units-of-measure
\param Stream Input stream containing the expression to be parsed
\param Value Iff the input expression is completely parsed, contains the expression value; otherwise unchanged
\param Units Iff the input expression is completely parsed, contains the (optional) unit-of-measure symbol; otherwise unchanged
\return true, iff the input expression is completely parsed, false otherwise

*/
bool parse(std::istream& Stream, double&Value, std::string& Units);

/**

\brief Parses a mathematical expression with optional real-world units-of-measure
\param Buffer Input buffer containing the expression to be parsed
\param Value Iff the input expression is completely parsed, contains the expression value; otherwise unchanged
\param Units Iff the input expression is completely parsed, contains the (optional) unit-of-measure symbol; otherwise unchanged
\return true, iff the input expression is completely parsed, false otherwise

*/
bool parse(const std::string Buffer, double&Value, std::string& Units);

/**

\brief Parses a mathematical expression with optional real-world units-of-measure, automatically converting it to the appropriate SI units
\param Buffer Input buffer containing the expression to be parsed
\param Value Iff the input expression is completely parsed, contains the expression value in SI units
\param Units Defines the type of measurement (e.g. angle, distance, time, volume, etc) to be formatted
\return true, iff the input expression is completely parsed, false otherwise

*/
bool parse(const std::string Buffer, double&Value, const std::type_info* const Units);

/// Defines a collection of units of angular measure, keyed by string (typically name or symbol)
typedef std::map<std::string, angle> angle_units_t;
/// Defines a collection of units of area, keyed by string (typically name or symbol)
typedef std::map<std::string, area> area_units_t;
/// Defines a collection of units of distance, keyed by string (typically name or symbol)
typedef std::map<std::string, distance> distance_units_t;
/// Defines a collection of units of force, keyed by string (typically name or symbol)
typedef std::map<std::string, force> force_units_t;
/// Defines a collection of units of mass, keyed by string (typically name or symbol)
typedef std::map<std::string, mass> mass_units_t;
/// Defines a collection of units of pressure, keyed by string (typically name or symbol)
typedef std::map<std::string, pressure> pressure_units_t;
/// Defines a collection of units of time, keyed by string (typically name or symbol)
typedef std::map<std::string, time> time_units_t;
/// Defines a collection of units of volume, keyed by string (typically name or symbol)
typedef std::map<std::string, volume> volume_units_t;

/// Returns a singleton collection of common units of angular measure, keyed by symbol
const angle_units_t& angle_units();
/// Returns a singleton collection of common units of area, keyed by symbol
const area_units_t& area_units();
/// Returns a singleton collection of common units of distance, keyed by symbol
const distance_units_t& distance_units();
/// Returns a singleton collection of common units of force, keyed by symbol
const force_units_t& force_units();
/// Returns a singleton collection of common units of mass, keyed by symbol
const mass_units_t& mass_units();
/// Returns a singleton collection of common units of pressure, keyed by symbol
const pressure_units_t& pressure_units();
/// Returns a singleton collection of common units of time, keyed by symbol
const time_units_t& time_units();
/// Returns a singleton collection of common units of volume, keyed by symbol
const volume_units_t& volume_units();

/// Returns a reference to a common unit of angular measure keyed by symbol.  Throws std::exception if the symbol is unknown.
const angle& angle_units(const std::string Symbol);
/// Returns a reference to a common unit of area keyed by symbol.  Throws std::exception if the symbol is unknown.
const area& area_units(const std::string Symbol);
/// Returns a reference to a common unit of distance keyed by symbol.  Throws std::exception if the symbol is unknown.
const distance& distance_units(const std::string Symbol);
/// Returns a reference to a common unit of force keyed by symbol.  Throws std::exception if the symbol is unknown.
const force& force_units(const std::string Symbol);
/// Returns a reference to a common unit of mass keyed by symbol.  Throws std::exception if the symbol is unknown.
const mass& mass_units(const std::string Symbol);
/// Returns a reference to a common unit of pressure keyed by symbol.  Throws std::exception if the symbol is unknown.
const pressure& pressure_units(const std::string Symbol);
/// Returns a reference to a common unit of time keyed by symbol.  Throws std::exception if the symbol is unknown.
const time& time_units(const std::string Symbol);
/// Returns a reference to a common unit of volume keyed by symbol.  Throws std::exception if the symbol is unknown.
const volume& volume_units(const std::string Symbol);

} // namespace measurement

} // namespace k3d

#endif // K3DSDK_MEASUREMENT_H


