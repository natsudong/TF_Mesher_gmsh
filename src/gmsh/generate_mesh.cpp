#include "generate_mesh.h"

namespace generate_mesh {
	int generateMesh(const std::string& meshType) 
	{
		if (meshType == "Tri") {
			gmsh::model::mesh::generate(2);
		}
		else if (meshType == "StructuredQuad") {
			gmsh::option::setNumber("Mesh.Algorithm", 11);
			gmsh::model::mesh::generate(2);
		}
		else if (meshType == "UnstructuredQuad") {
			gmsh::option::setNumber("Mesh.Algorithm", 8);
			gmsh::option::setNumber("Mesh.RecombineAll", 1);
			gmsh::option::setNumber("Mesh.SubdivisionAlgorithm", 1);
			gmsh::model::mesh::generate(2);
		}
		else if (meshType == "Tet") {
			gmsh::model::mesh::generate(3);
		}
		else if (meshType == "Hex") {
			gmsh::option::setNumber("Mesh.SubdivisionAlgorithm", 2);
			gmsh::model::mesh::generate(3);
		}
		else {
			std::cout << "Unsupport Type" << std::endl;
			return 0;
		}
		return 1;
	}
}