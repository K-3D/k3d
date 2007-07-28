// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "base64.h"
#include "data.h"
#include "fstream.h"
#include "path.h"
#include "share.h"
#include "system.h"

namespace k3d
{

std::ostream& operator<<(std::ostream& Stream, const ipath_property::reference_t& RHS)
{
	switch(RHS)
	{
		case ipath_property::ABSOLUTE_REFERENCE:
			Stream << "absolute";
			break;
		case ipath_property::RELATIVE_REFERENCE:
			Stream << "relative";
			break;
		case ipath_property::INLINE_REFERENCE:
			Stream << "inline";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, ipath_property::reference_t& RHS)
{
	std::string temp;
	Stream >> temp;

	if(temp == "absolute")
		RHS = ipath_property::ABSOLUTE_REFERENCE;
	else if(temp == "relative")
		RHS = ipath_property::RELATIVE_REFERENCE;
	else if(temp == "inline")
		RHS = ipath_property::INLINE_REFERENCE;
	else
	{
		log() << error << "unknown enumeration" << std::endl;
		RHS = ipath_property::RELATIVE_REFERENCE;
	}

	return Stream;
}

namespace data
{

iproperty* property_lookup(iproperty* const Source, ipipeline& DAG)
{
	iproperty* result = Source;

    iproperty* slow = Source ? DAG.dependency(*Source) : 0;
    iproperty* fast = slow ? DAG.dependency(*slow) : 0;
	while(slow)
	{
		if(slow == fast)
            return Source;

		result = slow;

        slow = DAG.dependency(*slow);
        fast = fast ? DAG.dependency(*fast) : 0;
        fast = fast ? DAG.dependency(*fast) : 0;
	}

	return result;
}

void save_external_resource(xml::element& Element, const ipersistent::save_context& Context, const std::string& Name, const ipath_property::reference_t Reference, const filesystem::path& Value)
{
	xml::element& xml_storage = Element.append(
	xml::element("property",
	xml::attribute("name", Name)));

	// Construct absolute and relative paths for later reference
	const filesystem::path absolute_path = filesystem::path(Value.is_complete() ? Value : Context.root_path / Value);

	// Under certain circumstances, we override the user's choice of reference ...

	// As a special-case, if the value doesn't have the same root name as the reference
	// (i.e. different drive letters on Win32), force an absolute path ...
	ipath_property::reference_t reference = Reference;
	if(reference == ipath_property::RELATIVE_REFERENCE)
	{
		if(Value.is_complete() && Value.root_name() != Context.root_path.root_name())
			reference = ipath_property::ABSOLUTE_REFERENCE;
	}

	// As a special-case, an absolute path that points into the share directory
	// becomes a relative path ...
	if(reference == ipath_property::ABSOLUTE_REFERENCE)
	{
		if(0 == absolute_path.generic_utf8_string().find(share_path().generic_utf8_string()))
			reference = ipath_property::RELATIVE_REFERENCE;
	}

	// Actually store the data ...
	switch(reference)
	{
		case ipath_property::ABSOLUTE_REFERENCE:
		{
			xml_storage.append(xml::attribute("reference", string_cast(ipath_property::ABSOLUTE_REFERENCE)));
			xml_storage.append(xml::attribute("absolute_path", absolute_path.generic_utf8_string().raw()));
			break;
		}
		case ipath_property::RELATIVE_REFERENCE:
		{
			// As a special-case, a path that points into the share directory
			// will be stored relative to share, instead of relative to the document
			if(0 == absolute_path.generic_utf8_string().find(share_path().generic_utf8_string()))
			{
				const filesystem::path relative_path = filesystem::make_relative_path(absolute_path, share_path());
				xml_storage.append(xml::attribute("reference", string_cast(ipath_property::RELATIVE_REFERENCE)));
				xml_storage.append(xml::attribute("relative_path", relative_path.generic_utf8_string().raw()));
				xml_storage.append(xml::element("root", "$K3D_SHARE_PATH"));
			}
			else
			{
				const filesystem::path relative_path = filesystem::make_relative_path(absolute_path, Context.root_path);
				xml_storage.append(xml::attribute("reference", string_cast(ipath_property::RELATIVE_REFERENCE)));
				xml_storage.append(xml::attribute("relative_path", relative_path.generic_utf8_string().raw()));
			}
			break;
		}
		case ipath_property::INLINE_REFERENCE:
		{
			xml_storage.append(xml::attribute("reference", string_cast(ipath_property::INLINE_REFERENCE)));
			xml_storage.append(xml::attribute("filename", absolute_path.leaf().raw()));
			xml_storage.append(xml::attribute("encoding", "base64"));

			filesystem::ifstream file(absolute_path);
			std::stringstream buffer;
			base64::encode(file, buffer);
			xml_storage.text = "\n" + buffer.str();

			break;
		}
	}
}

void load_external_resource(xml::element& Element, const ipersistent::load_context& Context, ipath_property::reference_t& Reference, filesystem::path& Value)
{
	Reference = xml::attribute_value<ipath_property::reference_t>(Element, "reference", ipath_property::RELATIVE_REFERENCE);
	switch(Reference)
	{
		case ipath_property::ABSOLUTE_REFERENCE:
		{
			try
			{
				/** \todo Change this to use generic_path after version 0.8 */
				Value = filesystem::native_path(ustring::from_utf8(xml::attribute_text(Element, "absolute_path")));
				log() << info << "Resolved absolute path " << Value.native_console_string() << std::endl;
			}
			catch(std::exception& e)
			{
				log() << error << e.what() << std::endl;
			}
			break;
		}
		case ipath_property::RELATIVE_REFERENCE:
		{
			try
			{
				filesystem::path root_path = Context.root_path;
				if(xml::element* const xml_root = xml::find_element(Element, "root"))
				{
					if(xml_root->text == "$K3D_SHARE_PATH")
						root_path = share_path();
					else
						root_path = filesystem::generic_path(xml_root->text);
				}

				Value = root_path / filesystem::generic_path(xml::attribute_text(Element, "relative_path"));
				log() << info << "Resolved relative path as " << Value.native_console_string() << std::endl;
			}
			catch(std::exception& e)
			{
				log() << error << e.what() << std::endl;
			}
			break;
		}
		case ipath_property::INLINE_REFERENCE:
		{
			try
			{
				Value = system::get_temp_directory() / filesystem::generic_path(xml::attribute_text(Element, "filename"));
				filesystem::ofstream file(Value);
				std::stringstream buffer(Element.text);
				base64::decode(buffer, file);

				log() << info << "Extracted inline document to " << Value.native_console_string() << std::endl;
			}
			catch(std::exception& e)
			{
				log() << error << e.what() << std::endl;
			}
			break;
		}
	}
}

} // namespace data

} // namespace k3d

