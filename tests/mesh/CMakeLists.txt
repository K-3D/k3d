K3D_TEST(mesh.validation
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.validation.py
	LABELS mesh)

K3D_TEST(mesh.serialization
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.serialization.py
	LABELS mesh)

K3D_TEST(mesh.diff
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.diff.py
	LABELS mesh)

K3D_TEST(mesh.solid
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.solid.py
	LABELS mesh)

K3D_TEST(mesh.solid.failure
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.solid.failure.py
	LABELS mesh)







K3D_TEST(mesh.selection.null 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.selection.null.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh modifier selection)

K3D_TEST(mesh.selection.none 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.selection.none.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh modifier selection)

K3D_TEST(mesh.selection.all 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.selection.all.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh modifier selection)

K3D_TEST(mesh.selection.points 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.selection.points.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh modifier selection)

K3D_TEST(mesh.selection.GrowSelection 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.selection.GrowSelection.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE K3D_BUILD_SELECTION_MODULE
	LABELS mesh modifier selection)

K3D_TEST(mesh.selection.SelectBoundaryEdges 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.selection.SelectBoundaryEdges.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE K3D_BUILD_SELECTION_MODULE
	LABELS mesh modifier selection)

K3D_TEST(mesh.selection.SelectDegenerateFaces 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.selection.SelectDegenerateFaces.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE K3D_BUILD_SELECTION_MODULE
	LABELS mesh modifier selection)




K3D_TEST(mesh.metrics.GTSMeshArea.PolyCube 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.metrics.GTSMeshArea.PolyCube.py
	REQUIRES K3D_BUILD_GTS_MODULE K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh metric)

K3D_TEST(mesh.metrics.GTSMeshArea.self_intersection 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.metrics.GTSMeshArea.self_intersection.py
	REQUIRES K3D_BUILD_GTS_MODULE K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh metric)

K3D_TEST(mesh.metrics.GTSMeshVolume.PolyCube 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.metrics.GTSMeshVolume.PolyCube.py
	REQUIRES K3D_BUILD_GTS_MODULE K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh metric)






K3D_TEST(mesh.source.3DSMeshReader
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.3DSMeshReader.py
	REQUIRES K3D_BUILD_3DS_IO_MODULE
	LABELS mesh source reader 3DSMeshReader)

K3D_TEST(mesh.source.COLLADAMeshReader
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.COLLADAMeshReader.py
	REQUIRES K3D_BUILD_COLLADA_IO_MODULE
	LABELS mesh source reader COLLADAMeshReader)

K3D_TEST(mesh.source.GTSMeshReader.cube
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.GTSMeshReader.cube.py
	REQUIRES K3D_BUILD_GTS_IO_MODULE
	LABELS mesh source reader GTSMeshReader)

K3D_TEST(mesh.source.GTSMeshReader.gz.cube
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.GTSMeshReader.gz.cube.py
	REQUIRES K3D_BUILD_GTS_IO_MODULE
	LABELS mesh source reader GTSMeshReader)

K3D_TEST(mesh.source.MD2MeshReader
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MD2MeshReader.py
	REQUIRES K3D_BUILD_MD2_IO_MODULE
	LABELS mesh source reader MD2MeshReader)

K3D_TEST(mesh.source.OBJMeshReader.nurbs
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.OBJMeshReader.nurbs.py
	REQUIRES K3D_BUILD_OBJ_IO_MODULE
	LABELS mesh source reader OBJMeshReader)

K3D_TEST(mesh.source.OBJMeshReader.polyhedron
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.OBJMeshReader.polyhedron.py
	REQUIRES K3D_BUILD_OBJ_IO_MODULE
	LABELS mesh source reader OBJMeshReader)

K3D_TEST(mesh.source.PLYMeshReader
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PLYMeshReader.py
	REQUIRES K3D_BUILD_PLY_IO_MODULE
	LABELS mesh source reader PLYMeshReader)

K3D_TEST(mesh.source.STLMeshReader
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.STLMeshReader.py
	REQUIRES K3D_BUILD_STL_IO_MODULE
	LABELS mesh source reader STLMeshReader)

K3D_TEST(mesh.source.SVGMeshReader
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.SVGMeshReader.py
	REQUIRES K3D_BUILD_SVG_IO_MODULE
	LABELS mesh source reader SVGMeshReader)






K3D_TEST(mesh.source.MeshSourceScript.bezier_triangle_patches
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.bezier_triangle_patches.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.bicubic.patches
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.bicubic.patches.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.bilinear.patches
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.bilinear.patches.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.blobby
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.blobby.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.cones
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.cones.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.cubic.curves
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.cubic.curves.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.cylinders
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.cylinders.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.disks
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.disks.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.hyperboloids
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.hyperboloids.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.linear.curves
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.linear.curves.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.nurbs.curves
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.nurbs.curves.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.nurbs.patches
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.nurbs.patches.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.paraboloids
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.paraboloids.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.particles
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.particles.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.polyhedra
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.polyhedra.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.simple_polyhedron
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.simple_polyhedron.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.spheres
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.spheres.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.teapots
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.teapots.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)

K3D_TEST(mesh.source.MeshSourceScript.tori
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.MeshSourceScript.tori.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS mesh source MeshSourceScript)













K3D_TEST(mesh.source.QuadricCone
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.QuadricCone.py
	REQUIRES K3D_BUILD_QUADRICS_MODULE
	LABELS mesh source quadric QuadricCone)

K3D_TEST(mesh.source.QuadricCylinder
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.QuadricCylinder.py
	REQUIRES K3D_BUILD_QUADRICS_MODULE
	LABELS mesh source quadric QuadricCylinder)

K3D_TEST(mesh.source.QuadricDisk
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.QuadricDisk.py
	REQUIRES K3D_BUILD_QUADRICS_MODULE
	LABELS mesh source quadric QuadricDisk)

K3D_TEST(mesh.source.QuadricLogo
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.QuadricLogo.py
	REQUIRES K3D_BUILD_QUADRICS_MODULE
	LABELS mesh source quadric QuadricLogo)

K3D_TEST(mesh.source.QuadricHyperboloid
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.QuadricHyperboloid.py
	REQUIRES K3D_BUILD_QUADRICS_MODULE
	LABELS mesh source quadric QuadricHyperboloid)

K3D_TEST(mesh.source.QuadricParaboloid
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.QuadricParaboloid.py
	REQUIRES K3D_BUILD_QUADRICS_MODULE
	LABELS mesh source quadric QuadricParaboloid)

K3D_TEST(mesh.source.QuadricSphere
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.QuadricSphere.py
	REQUIRES K3D_BUILD_QUADRICS_MODULE
	LABELS mesh source quadric QuadricSphere)

K3D_TEST(mesh.source.QuadricTorus
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.QuadricTorus.py
	REQUIRES K3D_BUILD_QUADRICS_MODULE
	LABELS mesh source quadric QuadricTorus)






K3D_TEST(mesh.source.Teapot
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.Teapot.py
	REQUIRES K3D_BUILD_QUADRICS_MODULE
	LABELS mesh source teapot Teapot)








K3D_TEST(mesh.source.BlobbyEllipsoid
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.BlobbyEllipsoid.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh source BlobbyEllipsoid)

K3D_TEST(mesh.source.BlobbySegment
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.BlobbySegment.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh source BlobbySegment) 

K3D_TEST(mesh.source.LinearKnot
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/source.LinearKnot.py
	REQUIRES K3D_BUILD_LINEAR_CURVE_MODULE
	LABELS mesh source LinearKnot)

K3D_TEST(mesh.source.LSystemParser
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.LSystemParser.py
	REQUIRES K3D_BUILD_LSYSTEM_MODULE
	LABELS mesh source LSystemParser)

K3D_TEST(mesh.source.LinearLissajousCurve
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/source.LinearLissajousCurve.py
	REQUIRES K3D_BUILD_LINEAR_CURVE_MODULE
	LABELS mesh source LinearLissajousCurve)

K3D_TEST(mesh.source.Newell.teacup
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.Newell.teacup.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source Newell)

K3D_TEST(mesh.source.Newell.teapot
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.Newell.teapot.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source Newell)

K3D_TEST(mesh.source.Newell.teaspoon
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.Newell.teaspoon.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source Newell)

K3D_TEST(mesh.source.NurbsCircle
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsCircle.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsCircle)

K3D_TEST(mesh.source.NurbsCone
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsCone.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsCone)

K3D_TEST(mesh.source.NurbsCurve
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsCurve.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsCurve)

K3D_TEST(mesh.source.NurbsCylinder
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsCylinder.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsCylinder)

K3D_TEST(mesh.source.NurbsDisk
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsDisk.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsDisk)

K3D_TEST(mesh.source.NurbsGrid
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsGrid.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsGrid)

K3D_TEST(mesh.source.NurbsHyperboloid
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsHyperboloid.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsHyperboloid)

K3D_TEST(mesh.source.NurbsParaboloid
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsParaboloid.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsParaboloid)

K3D_TEST(mesh.source.NurbsPolygon
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsPolygon.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsPolygon)

K3D_TEST(mesh.source.NurbsSphere
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsSphere.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsSphere)

K3D_TEST(mesh.source.NurbsTorus
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.NurbsTorus.py
	REQUIRES K3D_BUILD_NURBS_SOURCES_MODULE
	LABELS mesh source NurbsTorus)

K3D_TEST(mesh.source.PolyCone
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyCone.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolyCone)

K3D_TEST(mesh.source.PolyCube
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyCube.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolyCube)

K3D_TEST(mesh.source.PolyCushion
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyCushion.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolyCushion)

K3D_TEST(mesh.source.PolyCylinder
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyCylinder.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolyCylinder)

K3D_TEST(mesh.source.PolyCylinder.caps
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyCylinder.caps.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolyCylinder)

K3D_TEST(mesh.source.PolyDisk
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyDisk.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolyDisk)

K3D_TEST(mesh.source.PolyGrid
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyGrid.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolyGrid)

K3D_TEST(mesh.source.PolyIcosahedron
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyIcosahedron.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source POlyIcosahedron)

K3D_TEST(mesh.source.PolySphere.1
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolySphere.1.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolySphere)

K3D_TEST(mesh.source.PolySphere.2
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolySphere.2.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolySphere)

K3D_TEST(mesh.source.PolySphere.3
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolySphere.3.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolySphere)

K3D_TEST(mesh.source.PolyText
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyText.py
	REQUIRES K3D_BUILD_FREETYPE2_MODULE
	LABELS mesh source PolyText)

K3D_TEST(mesh.source.PolyTorus
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyTorus.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source PolyTorus)

K3D_TEST(mesh.source.UniformPolyhedron
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/source.UniformPolyhedron.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh source polyhedron UniformPolyhedron)

K3D_TEST(mesh.source.SurfacePlot
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.SurfacePlot.py
	REQUIRES K3D_BUILD_PLOT_MODULE
	LABELS mesh source SurfacePlot)

K3D_TEST(mesh.source.reader.bogus_input
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.reader.bogus_input.py
	LABELS mesh source)










K3D_TEST(mesh.source.CUDAPolyGrid
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.CUDAPolyGrid.py
	REQUIRES K3D_BUILD_CUDA_MODULE
	LABELS mesh source)

K3D_TEST(mesh.source.CUDAPolyGridAndTransformPoints
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.CUDAPolyGridAndTransformPoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh source)

K3D_TEST(mesh.source.CUDAPolyGrid.benchmark
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.CUDAPolyGrid.benchmark.py
	REQUIRES K3D_BUILD_CUDA_MODULE
	LABELS mesh source)

K3D_TEST(mesh.source.PolyGrid.benchmark
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.PolyGrid.benchmark.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh source)

K3D_TEST(mesh.source.BenchmarkComparison
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.BenchmarkComparison.py
	REQUIRES K3D_BUILD_POLYHEDRON_SOURCES_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh source)

K3D_TEST(mesh.source.CUDAPolyGridAndTransformPoints.benchmark
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.CUDAPolyGridAndTransformPoints.benchmark.py
	REQUIRES K3D_BUILD_CUDA_MODULE
	LABELS mesh source)

K3D_TEST(mesh.source.CUDA.Breakdown
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.source.CUDA.Breakdown.py
	REQUIRES K3D_BUILD_CUDA_MODULE
	LABELS mesh source)






K3D_TEST(mesh.modifier.BendPoints
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BendPoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE K3D_BUILD_POLYHEDRON_SOURCES_MODULE
	LABELS mesh modifier)

#K3D_TEST(mesh.modifier.BevelFaces 
#	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BevelFaces.py
#	REQUIRES K3D_BUILD_MESH_MODULE K3D_BUILD_POLYHEDRON_SOURCES_MODULE
#	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BevelPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BevelPoints.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BevelPoints.boundary 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BevelPoints.boundary.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BlobbyAdd 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BlobbyAdd.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BlobbyDivide 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BlobbyDivide.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BlobbyMaximum 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BlobbyMaximum.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BlobbyMinimum 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BlobbyMinimum.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BlobbyMultiply 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BlobbyMultiply.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BlobbyNegate 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BlobbyNegate.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BlobbySubtract 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BlobbySubtract.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BridgeEdges 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BridgeEdges.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier BridgeEdges)

K3D_TEST(mesh.modifier.BridgeFaces 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BridgeFaces.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier BridgeFaces)

K3D_TEST(mesh.modifier.BulgePoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BulgePoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CalculateNormals 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CalculateNormals.py
	REQUIRES K3D_BUILD_MESH_ATTRIBUTES_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CalculateNormals.2 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CalculateNormals.2.py
	REQUIRES K3D_BUILD_MESH_ATTRIBUTES_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CapHoles 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CapHoles.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CatmullClark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CatmullClark.py
	REQUIRES K3D_BUILD_SUBDIVISION_SURFACE_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CatmullClark.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CatmullClark.benchmark.py
	REQUIRES K3D_BUILD_SUBDIVISION_SURFACE_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CatmullClark.complex 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CatmullClark.complex.py
	REQUIRES K3D_BUILD_SUBDIVISION_SURFACE_MODULE
	LABELS mesh modifier)
	
K3D_TEST(mesh.modifier.CatmullClark.cube 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CatmullClark.cube.py
	REQUIRES K3D_BUILD_SUBDIVISION_SURFACE_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CenterPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CenterPoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CARVEBoolean.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CARVEBoolean.benchmark.py
	REQUIRES K3D_BUILD_CARVE_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CARVEBoolean.cubes 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CARVEBoolean.cubes.py
	REQUIRES K3D_BUILD_CARVE_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CARVEBoolean.cylinders 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CARVEBoolean.cylinders.py
	REQUIRES K3D_BUILD_CARVE_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CARVEBoolean.ply 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CARVEBoolean.ply.py
	REQUIRES K3D_BUILD_CARVE_MODULE K3D_BUILD_PLY_IO_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CGALBoolean.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CGALBoolean.benchmark.py
	REQUIRES K3D_BUILD_CGAL_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CGALBoolean.cubes 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CGALBoolean.cubes.py
	REQUIRES K3D_BUILD_CGAL_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CGALBoolean.cylinders 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CGALBoolean.cylinders.py
	REQUIRES K3D_BUILD_CGAL_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CollapseEdges 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CollapseEdges.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CollapseFaces 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CollapseFaces.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CollapsePoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CollapsePoints.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.ConnectVertices 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.ConnectVertices.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CylindricalWavePoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CylindricalWavePoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.DeformationExpression 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.DeformationExpression.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier DeformationExpression)

K3D_TEST(mesh.modifier.DeleteComponents.point 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.DeleteComponents.point.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier DeleteComponents)

K3D_TEST(mesh.modifier.DeleteComponents.edge 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.DeleteComponents.edge.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier DeleteComponents)

K3D_TEST(mesh.modifier.DeleteComponents.face 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.DeleteComponents.face.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier DeleteComponents)

K3D_TEST(mesh.modifier.DeleteComponents.complex 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.DeleteComponents.complex.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier DeleteComponents)

K3D_TEST(mesh.modifier.DissolveFaces 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.DissolveFaces.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier polyhedron PolyGrid DissolveFaces)

K3D_TEST(mesh.modifier.EdgesToBlobby 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.EdgesToBlobby.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.EulerKillEdgeAndVertex 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.EulerKillEdgeAndVertex.py
	REQUIRES K3D_BUILD_EULER_OPERATIONS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.EulerKillEdgeMakeLoop 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.EulerKillEdgeMakeLoop.py
	REQUIRES K3D_BUILD_EULER_OPERATIONS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.ExtrudeFaces 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.ExtrudeFaces.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.LeastSquaresPlot 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.LeastSquaresPlot.py
	REQUIRES K3D_BUILD_PLOT_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.LinearPointNoise 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.LinearPointNoise.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.LinearWavePoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.LinearWavePoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.PointsToParticles 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.PointsToParticles.py
	REQUIRES K3D_BUILD_PARTICLE_MODULE
	LABELS mesh modifier particles PointsToParticles)

K3D_TEST(mesh.modifier.PolyToBilinear 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.PolyToBilinear.py
	REQUIRES K3D_BUILD_BILINEAR_PATCH_MODULE
	LABELS mesh modifier PolyToBilinear)

K3D_TEST(mesh.modifier.MergeCollinearEdges 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.MergeCollinearEdges.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.MergeCoplanarFaces 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.MergeCoplanarFaces.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.MergeMesh 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.MergeMesh.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.MergeMesh.generic 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.MergeMesh.generic.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.MeshArray1D 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.MeshArray1D.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.MeshArray2D 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.MeshArray2D.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.MeshArray3D 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.MeshArray3D.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.MorphPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.MorphPoints.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.MoveFirstEdge 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.MoveFirstEdge.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsAddTrimCurve 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsAddTrimCurve.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsCloseCurve1 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsCloseCurve1.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsCloseCurve2 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsCloseCurve2.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsConnectCurves 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsConnectCurves.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsCreateCap 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsCreateCap.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsCurveInsertKnot 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsCurveInsertKnot.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsCurveTraversal 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsCurveTraversal.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsDegreeElevateCurve 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsDegreeElevateCurve.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsEditKnotVector1 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsEditKnotVector1.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsExtractCurvePoint 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsExtractCurvePoint.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsExtractPatchCurve 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsExtractPatchCurve.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsExtractTrimCurves 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsExtractTrimCurves.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsExtrudeCurve 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsExtrudeCurve.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsExtrudePatch 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsExtrudePatch.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsFlipCurve 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsFlipCurve.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsMergeConnectedCurves 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsMergeConnectedCurves.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsMergeCurveKnotVectors 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsMergeCurveKnotVectors.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsPatchDegreeElevation 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsPatchDegreeElevation.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsPatchInsertKnot 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsPatchInsertKnot.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsPolygonizeCurve 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsPolygonizeCurve.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsPolygonizePatch 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsPolygonizePatch.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsRevolveCurve 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsRevolveCurve.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsRuledSurface 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsRuledSurface.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsSetWeight1 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsSetWeight1.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsSetWeight2 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsSetWeight2.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsSkinnedSurface 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsSkinnedSurface.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsSplitCurve 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsSplitCurve.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsSplitPatch 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsSplitPatch.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsSplitPatch2 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsSplitPatch2.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.NurbsSweepSurface 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.NurbsSweepSurface.py
	REQUIRES K3D_BUILD_NURBS_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.PGPRemesh 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.PGPRemesh.py
	REQUIRES K3D_BUILD_PGP_REMESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.PGPRemesh.high 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.PGPRemesh.high.py
	REQUIRES K3D_BUILD_PGP_REMESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.PGPRemesh.ply.bunny 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.PGPRemesh.ply.bunny.py
	REQUIRES K3D_BUILD_PGP_REMESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.PGPRemesh.ply.torus 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.PGPRemesh.ply.torus.py
	REQUIRES K3D_BUILD_PGP_REMESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.PGPRemesh.triang 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.PGPRemesh.triang.py
	REQUIRES K3D_BUILD_PGP_REMESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.PGPRemesh.bun_zipper_res4 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.PGPRemesh.bun_zipper_res4.py
	REQUIRES K3D_BUILD_PGP_REMESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.PointsToBlobby 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.PointsToBlobby.py
	REQUIRES K3D_BUILD_BLOBBY_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BicubicToPoly 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.BicubicToPoly.py
	REQUIRES K3D_BUILD_BICUBIC_PATCH_MODULE
	LABELS mesh modifier BicubicToPoly)

K3D_TEST(mesh.modifier.QSlim 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.QSlim.py
	REQUIRES K3D_BUILD_QSLIM_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.RotatePoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.RotatePoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SDSCorner 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SDSCorner.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SDSCrease 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SDSCrease.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SDSHole 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SDSHole.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.ScalePoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.ScalePoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SelectConnectedComponents 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SelectConnectedComponents.py
	REQUIRES K3D_BUILD_SELECTION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SelectCube 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SelectCube.py
	REQUIRES K3D_BUILD_SELECTION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SelectedFacesToPolyhedron 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SelectedFacesToPolyhedron.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SelectNSided 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SelectNSided.py
	REQUIRES K3D_BUILD_SELECTION_MODULE
	LABELS mesh modifier polyhedron UniformPolyhedron SelectNSided)

K3D_TEST(mesh.modifier.SetColor 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SetColor.py
	REQUIRES K3D_BUILD_MESH_ATTRIBUTES_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.ShearPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.ShearPoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SmoothPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SmoothPoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SphereizePoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SphereizePoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SubdivideEdges 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SubdivideEdges.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SubdivideEdges.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SubdivideEdges.benchmark.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SubdivideFaces.Center 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SubdivideFaces.Center.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SubdivideFaces.Midpoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SubdivideFaces.Midpoints.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.SubdivideFaces.CenterMidpoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.SubdivideFaces.CenterMidpoints.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TaperPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TaperPoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TransformPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TransformPoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TranslatePoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TranslatePoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TriangulateFaces.PolyCube 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TriangulateFaces.PolyCube.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TriangulateFaces.PolyGrid 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TriangulateFaces.PolyGrid.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TriangulateFaces.PolyGrid.selection 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TriangulateFaces.PolyGrid.selection.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TriangulateFaces.vertex 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TriangulateFaces.vertex.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TriangulateFaces.self_intersection 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TriangulateFaces.self_intersection.py
	REQUIRES K3D_BUILD_POLYHEDRON_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TwistPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TwistPoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.WeldPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.WeldPoints.py
	REQUIRES K3D_BUILD_MESH_MODULE
	LABELS mesh modifier)












K3D_TEST(mesh.modifier.CUDATransformPoints 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CUDATransformPoints.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CUDATransformPoints.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CUDATransformPoints.benchmark.py
	REQUIRES K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CUDATransformPointsAsynchronous 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CUDATransformPointsAsynchronous.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CUDATransformPointsAsynchronous.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CUDATransformPointsAsynchronous.benchmark.py
	REQUIRES K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CUDASubdivideEdges 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CUDASubdivideEdges.py
	REQUIRES K3D_BUILD_MESH_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CUDASubdivideEdgesSimple 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CUDASubdivideEdgesSimple.py
	REQUIRES K3D_BUILD_MESH_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CUDASubdivideEdges.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CUDASubdivideEdges.benchmark.py
	REQUIRES K3D_BUILD_MESH_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.TransformPoints.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.TransformPoints.benchmark.py
	REQUIRES K3D_BUILD_DEFORMATION_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.BenchmarkComparison 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.BenchmarkComparison.py
	REQUIRES K3D_BUILD_MESH_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CUDA.Breakdown 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CUDA.Breakdown.py
	REQUIRES K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier)

K3D_TEST(mesh.modifier.CUDAConversion
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.modifier.CUDAConversion.py
	REQUIRES K3D_BUILD_MESH_MODULE K3D_BUILD_CUDA_MODULE
	LABELS mesh modifier cuda)





K3D_TEST(mesh.sink.K3DMeshWriter
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.sink.K3DMeshWriter.py
	REQUIRES K3D_BUILD_K3D_IO_MODULE
	LABELS mesh sink K3DMeshWriter)

K3D_TEST(mesh.sink.OBJMeshWriter
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.sink.OBJMeshWriter.py
	REQUIRES K3D_BUILD_OBJ_IO_MODULE
	LABELS mesh sink OBJMeshWriter)

K3D_TEST(mesh.sink.PLYMeshWriter
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.sink.PLYMeshWriter.py
	REQUIRES K3D_BUILD_PLY_IO_MODULE
	LABELS mesh sink PLYMeshWriter)

K3D_TEST(mesh.sink.STLMeshWriter
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/mesh.sink.STLMeshWriter.py
	REQUIRES K3D_BUILD_STL_IO_MODULE
	LABELS mesh sink STLMeshWriter)

