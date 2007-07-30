// K-3D
// Copyright (c) 2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/string_cast.h>

namespace libk3dcore
{

/////////////////////////////////////////////////////////////////////////////
// show_matrix

class show_matrix :
	public k3d::persistent<k3d::node>,
	public k3d::itransform_source,
	public k3d::itransform_sink
{
	typedef k3d::persistent<k3d::node> base;

public:
	show_matrix(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input(init_owner(*this) + init_name("input_matrix") + init_label(_("Input matrix")) +init_description(_("Input matrix")) + init_value(k3d::identity3D())),
		m_line1(init_owner(*this) + init_name("line1") + init_label(_("Line1")) + init_description(_("First matrix line or row")) + init_value<std::string>("0 0 0 0")),
		m_line2(init_owner(*this) + init_name("line2") + init_label(_("Line2")) + init_description(_("Second matrix line or row")) + init_value<std::string>("0 0 0 0")),
		m_line3(init_owner(*this) + init_name("line3") + init_label(_("Line3")) + init_description(_("Third matrix line or row")) + init_value<std::string>("0 0 0 0")),
		m_line4(init_owner(*this) + init_name("line4") + init_label(_("Line4")) + init_description(_("Fourth matrix line or row")) + init_value<std::string>("0 0 0 0")),
		m_transpose(init_owner(*this) + init_name("transpose") + init_label(_("Transpose")) + init_description(_("Transpose shown matrix")) + init_value(false)),
		m_output(init_owner(*this) + init_name("output_matrix") + init_label(_("Output matrix")) + init_description(_("Read only")) + init_slot(sigc::mem_fun(*this, &show_matrix::output_value)))
	{
		m_input.changed_signal().connect(m_output.changed_signal().make_slot());
		m_input.changed_signal().connect(sigc::mem_fun(*this, &show_matrix::on_reset_matrix));
		m_transpose.changed_signal().connect(sigc::mem_fun(*this, &show_matrix::on_reset_matrix));
	}

	k3d::iproperty& transform_source_output()
	{
		return m_output;
	}

	k3d::iproperty& transform_sink_input()
	{
		return m_input;
	}

	k3d::matrix4 output_value()
	{
		return m_input.pipeline_value();
	}

	std::string matrix_line(const k3d::matrix4& Matrix, unsigned long Line, const bool Transpose)
	{
		if(Transpose)
			return k3d::string_cast(Matrix[0][Line]) + " " + k3d::string_cast(Matrix[1][Line]) + " " + k3d::string_cast(Matrix[2][Line]) + " " + k3d::string_cast(Matrix[3][Line]);
		else
			return k3d::string_cast(Matrix[Line][0]) + " " + k3d::string_cast(Matrix[Line][1]) + " " + k3d::string_cast(Matrix[Line][2]) + " " + k3d::string_cast(Matrix[Line][3]);
	}

	void on_reset_matrix(k3d::iunknown*)
	{
		const k3d::matrix4 matrix = m_input.pipeline_value();
		const bool transpose = m_transpose.pipeline_value();

		m_line1.set_value(matrix_line(matrix, 0, transpose));
		m_line2.set_value(matrix_line(matrix, 1, transpose));
		m_line3.set_value(matrix_line(matrix, 2, transpose));
		m_line4.set_value(matrix_line(matrix, 3, transpose));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<show_matrix,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::uuid(0x488da0da, 0xbd1f4c37, 0x9ee29974, 0x7f6f99c3),
				"ShowMatrix",
				_("Displays input matrix values"),
				"Utility",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input;
	k3d_data(std::string, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_line1;
	k3d_data(std::string, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_line2;
	k3d_data(std::string, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_line3;
	k3d_data(std::string, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_line4;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_transpose;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output;
};

/////////////////////////////////////////////////////////////////////////////
// show_matrix_factory

k3d::iplugin_factory& show_matrix_factory()
{
	return show_matrix::get_factory();
}

} // namespace libk3dcore

