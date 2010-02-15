#include <k3dsdk/xml.h>
using namespace k3d::xml;

#include <cassert>

int main()
{
	// Test element initialization ...
	element e1("e1", "value2");
	assert(e1.name == "e1");
	assert(e1.text == "value2");
	assert(e1.children.empty());
	assert(e1.attributes.empty());

	// Test attribute initialization ...
	attribute a1("a1", "value1");
	assert(a1.name == "a1");
	assert(a1.value == "value1");

	// Test appending elements ...
	assert(!find_element(e1, "e2"));
	assert(e1.children.empty());
	e1.append(element("e2", "value3"));
	assert(e1.children.size() == 1);
	assert(find_element(e1, "e2"));
	assert(find_element(e1, "e2")->name == "e2");
	assert(find_element(e1, "e2")->text == "value3");

	// Test appending attributes ...
	assert(!find_attribute(e1, "a2"));
	assert(e1.attributes.empty());
	e1.append(attribute("a2", "value4"));
	assert(e1.attributes.size() == 1);
	assert(find_attribute(e1, "a2"));
	assert(find_attribute(e1, "a2")->name == "a2");
	assert(find_attribute(e1, "a2")->value == "value4");

	// Test safe element access ...
	e1.safe_element("e3");
	assert(e1.children.size() == 2);
	assert(find_element(e1, "e3"));
	assert(find_element(e1, "e3")->name == "e3");
	assert(find_element(e1, "e3")->text.empty());
	assert(e1.safe_element("e3").name == "e3");
	assert(e1.children.size() == 2);

	assert(e1.safe_element(element("e4"), element("e4", "value7")).text == "value7");
	assert(e1.children.size() == 3);
	assert(e1.safe_element("e4").text == "value7");
	assert(e1.children.size() == 3);

	// Test setting attributes ...
	set_attribute(e1, attribute("a3", "value5"));
	assert(e1.attributes.size() == 2);
	assert(find_attribute(e1, "a3"));
	assert(find_attribute(e1, "a3")->name == "a3");
	assert(find_attribute(e1, "a3")->value == "value5");

	set_attribute(e1, attribute("a3", "value6"));
	assert(e1.attributes.size() == 2);
	assert(find_attribute(e1, "a3"));
	assert(find_attribute(e1, "a3")->name == "a3");
	assert(find_attribute(e1, "a3")->value == "value6");
	
	return 0;
}
