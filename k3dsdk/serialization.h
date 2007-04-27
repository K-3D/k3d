#ifndef K3DSDK_SERIALIZATION_H
#define K3DSDK_SERIALIZATION_H

#include <string>

namespace k3d
{

class idocument;
class idocument_read_format;
class idocument_write_format;
class imaterial;
class ipersistent;
class ipersistent::load_context;
class ipersistent::save_context;
class mesh_selection;
namespace dev { class mesh; }
namespace legacy { class mesh; }
namespace xml { class element; }

/// Modifies an XML document as-needed so that both legacy and recent documents can be loaded with the same code
void upgrade_document(xml::element& XML);

/// Serializes a document DAG to an XML element
void save_dag(idocument& Document, xml::element& XML, const ipersistent::save_context& Context);
/// Deserializes document DAG information from an XML element
void load_dag(idocument& Document, xml::element& XML, const ipersistent::load_context& Context);

/// Serializes a mesh to an XML document
void save_mesh(const dev::mesh& Mesh, xml::element& Container, const ipersistent::save_context& Context);
/// Deserializes a mesh from an XML document
void load_mesh(dev::mesh& Mesh, xml::element& Container, const ipersistent::load_context& Context);

/// Imports data into a document from a file
bool import_file(idocument& Document, idocument_read_format& FormatFilter, const filesystem::path& FilePath);
/// Exports document data to a file
bool export_file(idocument& Document, idocument_write_format& FormatFilter, const filesystem::path& FilePath);

/// Serializes a document node to an XML document
void save_node(k3d::ipersistent& Node, xml::element& XML, const ipersistent::save_context& Context);

/// Serializes a mesh_selection to an XML document
void save_mesh_selection(const mesh_selection& Selection, xml::element& XML, const ipersistent::save_context& Context);
/// Deserializes a mesh_selection from an XML document
void load_mesh_selection(mesh_selection& Selection, xml::element& XML, const ipersistent::load_context& Context);

} // namespace k3d

#endif // !K3DSDK_SERIALIZATION_H

