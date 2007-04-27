// K3D
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
		\brief Implements a framework for doing real-world unit-of-measure conversion
		\author Tim Shead (tshead@k-3d.com)
*/

#include "expression_grammar.h"
#include "measurement.h"
#include "result.h"

#include <stdexcept>

namespace k3d
{

namespace measurement
{

using namespace boost::spirit;

bool parse(std::istream& Stream, double&Value, std::string& Units)
{
	std::string buffer;
	std::copy(std::istreambuf_iterator<char>(Stream), std::istreambuf_iterator<char>(), std::back_inserter(buffer));

	return parse(buffer, Value, Units);
}

bool parse(const std::string Buffer, double& Value, std::string& Units)
{
	std::stack<double> stack;
	std::string units;

	if(!parse(Buffer.c_str(), k3d::expression::expression_grammar(stack) >>!(alpha_p >> *graph_p)[assign(units)], space_p).full)
		return false;

	assert(!stack.empty());
	Value = stack.top();
	Units = units;

	return true;
}

bool parse(const std::string Buffer, double&Value, const std::type_info* const Units)
{
	// Parse the input expression into value and unit-of-measure (it can do mathematical expressions, too, for fun)
	double value;
	std::string units;
	if(!parse(Buffer, value, units))
		return false;

	// If the caller didn't specify the type of units, they're scalar (no unit of measurement) ...
	if(!Units)
	{
		// In this case, if the user entered units, it's an error ...
		if(!units.empty())
			return false;

		// Just return the value ...
		Value = value;
		return true;
	}

	// As a special-case, if the user didn't enter units, and the unit type is "angle", we assume the input is in degrees, not radians
	if(units.empty() && *Units == typeid(angle))
		units = "deg";

	// No units specified, so we can assume that input is already in the internal (SI) units
	if(units.empty())
	{
		Value = value;
		return true;
	}

	// Attempt converting the value to its internal (SI) units ...
	try
	{
		if(*Units == typeid(angle))
			Value = convert(quantity<angle>(value, angle_units(units)), angle_units("rad"));
		else if(*Units == typeid(area))
			Value = convert(quantity<area>(value, area_units(units)), area_units("m^2"));
		else if(*Units == typeid(distance))
			Value = convert(quantity<distance>(value, distance_units(units)), distance_units("m"));
		else if(*Units == typeid(force))
			Value = convert(quantity<force>(value, force_units(units)), force_units("N"));
		else if(*Units == typeid(mass))
			Value = convert(quantity<mass>(value, mass_units(units)), mass_units("Kg"));
		else if(*Units == typeid(pressure))
			Value = convert(quantity<pressure>(value, pressure_units(units)), pressure_units("Pa"));
		else if(*Units == typeid(k3d::measurement::time))
			Value = convert(quantity<k3d::measurement::time>(value, time_units(units)), time_units("s"));
		else if(*Units == typeid(volume))
			Value = convert(quantity<volume>(value, volume_units(units)), volume_units("m^3"));

		return true;
	}
	catch(...)
	{
	}

	return false;
}

const angle_units_t& angle_units()
{
	static angle_units_t units;
	if(units.empty())
	{
		units.insert(std::make_pair("d", angle("Degree", "degrees", 0.01745329252)));
		units.insert(std::make_pair("deg", angle("Degree", "degrees", 0.01745329252)));
		units.insert(std::make_pair("degree", angle("Degree", "degrees", 0.01745329252)));
		units.insert(std::make_pair("arcmin", angle("Minute of arc", "Minutes of arc", 0.00290888209)));
		units.insert(std::make_pair("arcs", angle("Second of arc", "Seconds of arc", 0.00000484814)));
		units.insert(std::make_pair("g", angle("Grade", "Grades", 0.01570796327)));
		units.insert(std::make_pair("grad", angle("Grade", "Grades", 0.01570796327)));
		units.insert(std::make_pair("r", angle("Radian", "radians", 1.0)));
		units.insert(std::make_pair("rad", angle("Radian", "radians", 1.0)));
		units.insert(std::make_pair("radian", angle("Radian", "radians", 1.0)));
	}

	return units;
}

const area_units_t& area_units()
{
	static area_units_t units;
	if(units.empty())
	{
		units.insert(std::make_pair("m^2", area("Square Meter", "Square Meters", 1.0)));
		units.insert(std::make_pair("a", area("Are", "Ares", 100.0)));
		units.insert(std::make_pair("acre", area("Acre", "Acres", 4046.87260987)));
		units.insert(std::make_pair("b", area("Barn", "Barns", 1e-28)));
	}

	return units;
}

const distance_units_t& distance_units()
{
	static distance_units_t units;
	if(units.empty())
	{
		units.insert(std::make_pair("m", distance("Meter", "Meters", 1.0)));
		units.insert(std::make_pair("au", distance("Astronomical Unit", "Astronomical Units", 1.495979e11)));
		units.insert(std::make_pair("A", distance("Angstrom", "Angstroms", 0.0000000001)));
		units.insert(std::make_pair("angstrom", distance("Angstrom", "Angstroms", 0.0000000001)));
		units.insert(std::make_pair("chain", distance("Chain", "Chains", 20.1168402337)));
		units.insert(std::make_pair("fath", distance("Fathom", "Fathoms", 1.82880365761)));
		units.insert(std::make_pair("fathom", distance("Fathom", "Fathoms", 1.82880365761)));
		units.insert(std::make_pair("fermi", distance("Fermi", "Fermis", 1e-15)));
		units.insert(std::make_pair("ftintl", distance("International Foot", "International Feet", 0.3048)));
		units.insert(std::make_pair("ft", distance("Survey Foot", "Survey Feet", 0.304800609601)));
		units.insert(std::make_pair("foot", distance("Survey Foot", "Survey Feet", 0.304800609601)));
		units.insert(std::make_pair("ftUS", distance("Survey Foot", "Survey Feet", 0.304800609601)));
		units.insert(std::make_pair("in", distance("Inch", "Inches", 0.0254)));
		units.insert(std::make_pair("inch", distance("Inch", "Inches", 0.0254)));
		units.insert(std::make_pair("ly", distance("Light Year", "Light Years", 9.46052840488e15)));
		units.insert(std::make_pair("lyr", distance("Light Year", "Light Years", 9.46052840488e15)));
		units.insert(std::make_pair("lightyear", distance("Light Year", "Light Years", 9.46052840488e15)));
		units.insert(std::make_pair("Em", distance("Exameter", "Exameters", 1e18)));
		units.insert(std::make_pair("Pm", distance("Petemeter", "Petemeters", 1e15)));
		units.insert(std::make_pair("Tm", distance("Terameter", "Terameters", 1e12)));
		units.insert(std::make_pair("Gm", distance("Gigameter", "Gigameters", 1e9)));
		units.insert(std::make_pair("Mm", distance("Megameter", "Megameters", 1e6)));
		units.insert(std::make_pair("Km", distance("Kilometer", "Kilometers", 1e3)));
		units.insert(std::make_pair("km", distance("Kilometer", "Kilometers", 1e3)));
		units.insert(std::make_pair("Hm", distance("Hectometer", "Hectometers", 1e2)));
		units.insert(std::make_pair("hm", distance("Hectometer", "Hectometers", 1e2)));
		units.insert(std::make_pair("Dm", distance("Dekameter", "Dekameters", 1e1)));
		units.insert(std::make_pair("meter", distance("Meter", "Meters", 1.0)));
		units.insert(std::make_pair("dm", distance("Decimeter", "Decimeters", 1e-1)));
		units.insert(std::make_pair("cm", distance("Centimeter", "Centimeters", 1e-2)));
		units.insert(std::make_pair("mm", distance("Millimeter", "Millimeters", 1e-3)));
		units.insert(std::make_pair("um", distance("Micrometer", "Micrometers", 1e-6)));
		units.insert(std::make_pair("nm", distance("Nanometer", "Nanometers", 1e-9)));
		units.insert(std::make_pair("pm", distance("Picometer", "Picometers", 1e-12)));
		units.insert(std::make_pair("fm", distance("Femtometer", "Femtometers", 1e-15)));
		units.insert(std::make_pair("am", distance("Attometer", "Attometers", 1e-18)));
		units.insert(std::make_pair("miIntl", distance("International Mile", "International Miles", 1609.344)));
		units.insert(std::make_pair("mil", distance("Mil", "Mils", 0.0000254)));
		units.insert(std::make_pair("mi", distance("US Statute Mile", "US Statute Miles", 1609.34721869)));
		units.insert(std::make_pair("mile", distance("US Statute Mile", "US Statute Miles", 1609.34721869)));
		units.insert(std::make_pair("mileUS", distance("US Statute Mile", "US Statute Miles", 1609.34721869)));
		units.insert(std::make_pair("nmi", distance("Nautical Mile", "Nautical Miles", 1852.0)));
		units.insert(std::make_pair("pc", distance("Parsec", "Parsecs", 3.08567818585e16)));
		units.insert(std::make_pair("parsec", distance("Parsec", "Parsecs", 3.08567818585e16)));
		units.insert(std::make_pair("rd", distance("Rod", "Rods", 5.02921005842)));
		units.insert(std::make_pair("rod", distance("Rod", "Rods", 5.02921005842)));
		units.insert(std::make_pair("yd", distance("Yard", "Yards", 0.9144)));
		units.insert(std::make_pair("yard", distance("Yard", "Yards", 0.9144)));
	}

	return units;
}

const force_units_t& force_units()
{
	static force_units_t units;
	if(units.empty())
	{
		units.insert(std::make_pair("N", force("Newton", "Newtons", 1.0)));
		units.insert(std::make_pair("dyn", force("Dyne", "Dynes", 0.00001)));
		units.insert(std::make_pair("dyne", force("Dyne", "Dynes", 0.00001)));
		units.insert(std::make_pair("gf", force("Gram-Force", "Grams-Force", 0.00980665)));
		units.insert(std::make_pair("kip", force("Kilopound-Force", "Kilopounds-Force", 4448.22161526)));
		units.insert(std::make_pair("lbf", force("Point-Force", "Points-Force", 4.44822161526)));
		units.insert(std::make_pair("Newton", force("Newton", "Newtons", 1.0)));
		units.insert(std::make_pair("pdl", force("Poundal", "Poundals", 0.138254954376)));
	}

	return units;
}

const mass_units_t& mass_units()
{
	static mass_units_t units;
	if(units.empty())
	{
		units.insert(std::make_pair("Kg", mass("Kilogram", "Kilograms", 1.0)));
		units.insert(std::make_pair("ct", mass("Carat", "Carats", 0.0002)));
		units.insert(std::make_pair("g", mass("Gram", "Grams", 0.001)));
		units.insert(std::make_pair("gram", mass("Gram", "Grams", 0.001)));
		units.insert(std::make_pair("grain", mass("Grain", "Grains", 0.00006479891)));
		units.insert(std::make_pair("lb", mass("Avoirdupois Pound", "Avoirdupois Pounds", 0.45359237)));
		units.insert(std::make_pair("lbt", mass("Troy Pound", "Troy Pounds", 0.3732417)));
		units.insert(std::make_pair("oz", mass("Ounce", "Ounces", 0.028349523125)));
		units.insert(std::make_pair("ounce", mass("Ounce", "Ounces", 0.028349523125)));
		units.insert(std::make_pair("ozt", mass("Troy Ounce", "Troy Ounces", 0.031103475)));
		units.insert(std::make_pair("slug", mass("Slug", "Slugs", 14.5939029372)));
		units.insert(std::make_pair("t", mass("Metric Ton", "Metric Tons", 1000.0)));
		units.insert(std::make_pair("ton", mass("Short Ton", "Short Tons", 907.18474)));
		units.insert(std::make_pair("tonUK", mass("Long Ton", "Long Tons", 1016.0469088)));
		units.insert(std::make_pair("u", mass("Unified Atomic Mass", "Unified Atomic Masses", 1.66057e-27)));
	}

	return units;
}

const pressure_units_t& pressure_units()
{
	static pressure_units_t units;
	if(units.empty())
	{
		units.insert(std::make_pair("Pa", pressure("Pascal", "Pascal", 1.0)));
		units.insert(std::make_pair("atm", pressure("Atmosphere", "Atmospheres", 101325.0)));
		units.insert(std::make_pair("bar", pressure("Bar", "Bars", 100000.0)));
		units.insert(std::make_pair("inHG", pressure("Inch of Mercury", "Inches of Mercury", 3386.38815789)));
		units.insert(std::make_pair("inH2O", pressure("Inch of Water", "Inches of Water", 248.84)));
		units.insert(std::make_pair("mmHG", pressure("Millimeter of Mercury", "Millimeters of Mercury", 133.322368421)));
		units.insert(std::make_pair("Pascal", pressure("Pascal", "Pascals", 1.0)));
		units.insert(std::make_pair("psi", pressure("Pound per Square Inch", "Pounds per Square Inch", 6894.75729317)));
		units.insert(std::make_pair("torr", pressure("Torr", "Torrs", 133.322368421)));
	}

	return units;
}

const time_units_t& time_units()
{
	static time_units_t units;
	if(units.empty())
	{
		units.insert(std::make_pair("s", time("Second", "Seconds", 1.0)));
		units.insert(std::make_pair("d", time("Day", "Days", 86400.0)));
		units.insert(std::make_pair("day", time("Day", "Days", 86400.0)));
		units.insert(std::make_pair("h", time("Hour", "Hours", 3600.0)));
		units.insert(std::make_pair("hour", time("Hour", "Hours", 3600.0)));
		units.insert(std::make_pair("ms", time("Millisecond", "Milliseconds", 0.001)));
		units.insert(std::make_pair("msec", time("Millisecond", "Milliseconds", 0.001)));
		units.insert(std::make_pair("min", time("Minute", "Minutes", 60.0)));
		units.insert(std::make_pair("minute", time("Minute", "Minutes", 60.0)));
		units.insert(std::make_pair("second", time("Second", "Seconds", 1.0)));
		units.insert(std::make_pair("yr", time("Year", "Years", 31556925.9747)));
		units.insert(std::make_pair("year", time("Year", "Years", 31556925.9747)));
	}

	return units;
}

const volume_units_t& volume_units()
{
	static volume_units_t units;
	if(units.empty())
	{
		units.insert(std::make_pair("m^3", volume("Cubic Meter", "Cubic Meters", 1.0)));
		units.insert(std::make_pair("bbl", volume("Barrel (Oil)", "Barrels (Oil)", 0.158987294928)));
		units.insert(std::make_pair("bu", volume("Bushel", "Bushels", 0.03523907)));
		units.insert(std::make_pair("cu", volume("US Cup", "US Cups", 2.365882365e-4)));
		units.insert(std::make_pair("fbm", volume("Board Foot", "Board Feet", 0.002359737216)));
		units.insert(std::make_pair("gal", volume("US Gallon", "US Gallons", 0.003785411784)));
		units.insert(std::make_pair("galUS", volume("US Gallon", "US Gallons", 0.003785411784)));
		units.insert(std::make_pair("galC", volume("Canadian Gallon", "Canadian Gallons", 0.00454609)));
		units.insert(std::make_pair("galUK", volume("UK Gallon", "UK Gallons", 0.004546092)));
		units.insert(std::make_pair("l", volume("Liter", "Liters", 0.001)));
		units.insert(std::make_pair("ozfl", volume("US Fluid Ounce", "US Fluid Ounce", 2.95735295625e-5)));
		units.insert(std::make_pair("ozUK", volume("UK Fluid Ounce", "UK Fluid Ounce", 0.000028413075)));
		units.insert(std::make_pair("pk", volume("Peck", "Pecks", 0.0088097675)));
		units.insert(std::make_pair("pt", volume("Pint", "Pints", 0.000473176473)));
		units.insert(std::make_pair("pint", volume("Pint", "Pints", 0.000473176473)));
		units.insert(std::make_pair("qt", volume("Quart", "Quarts", 0.000946352946)));
		units.insert(std::make_pair("quart", volume("Quart", "Quarts", 0.000946352946)));
		units.insert(std::make_pair("st", volume("Stere", "Steres", 1.0)));
		units.insert(std::make_pair("stere", volume("Stere", "Steres", 1.0)));
		units.insert(std::make_pair("tbsp", volume("Tablespoon", "Tablespoons", 1.47867647813e-5)));
		units.insert(std::make_pair("tsp", volume("Teaspoon", "Teaspoons", 4.92892159375e-6)));
	}

	return units;
}

const angle& angle_units(const std::string Symbol)
{
	angle_units_t::const_iterator unit = angle_units().find(Symbol);

	if(unit == angle_units().end())
		throw std::domain_error("unknown unit of measure");

	return unit->second;
}

const area& area_units(const std::string Symbol)
{
	area_units_t::const_iterator unit = area_units().find(Symbol);

	if(unit == area_units().end())
		throw std::domain_error("unknown unit of measure");

	return unit->second;
}

const distance& distance_units(const std::string Symbol)
{
	distance_units_t::const_iterator unit = distance_units().find(Symbol);

	if(unit == distance_units().end())
		throw std::domain_error("unknown unit of measure");

	return unit->second;
}

const force& force_units(const std::string Symbol)
{
	force_units_t::const_iterator unit = force_units().find(Symbol);

	if(unit == force_units().end())
		throw std::domain_error("unknown unit of measure");

	return unit->second;
}

const mass& mass_units(const std::string Symbol)
{
	mass_units_t::const_iterator unit = mass_units().find(Symbol);

	if(unit == mass_units().end())
		throw std::domain_error("unknown unit of measure");

	return unit->second;
}

const pressure& pressure_units(const std::string Symbol)
{
	pressure_units_t::const_iterator unit = pressure_units().find(Symbol);

	if(unit == pressure_units().end())
		throw std::domain_error("unknown unit of measure");

	return unit->second;
}

const time& time_units(const std::string Symbol)
{
	time_units_t::const_iterator unit = time_units().find(Symbol);

	if(unit == time_units().end())
		throw std::domain_error("unknown unit of measure");

	return unit->second;
}

const volume& volume_units(const std::string Symbol)
{
	volume_units_t::const_iterator unit = volume_units().find(Symbol);

	if(unit == volume_units().end())
		throw std::domain_error("unknown unit of measure");

	return unit->second;
}

} // namespace measurement

} // namespace k3d

