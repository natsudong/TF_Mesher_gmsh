#ifndef MESH_IO_H_
#define MESH_IO_H_
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <map>
#include <iomanip>
#include "gmsh.h"
#include "../utils.h"

namespace mesh_io {
	int readVTK(const std::string& filename, 
		std::vector < std::vector < double > >& points, 
		std::vector < std::vector < size_t > >& elements
	);

	int saveVTK(const std::string& filename, 
		const std::vector < std::vector < double > >& points, 
		const std::vector < std::vector < size_t > >& elements
	);

	int readOBJ(const std::string& filename,
		std::vector < std::vector < double > >& points,
		std::vector < std::vector < size_t > >& elements
	);

	int saveOBJ(const std::string& filename,
		const std::vector < std::vector < double > >& points,
		const std::vector < std::vector < size_t > >& tris
	);

	int saveTriBDF(const std::string& filename);

	int saveTriBDF_Face(const std::string& filename);

	int saveQuadBDF(const std::string& filename);

	int saveQuadBDF_Face(const std::string& filename);

	int saveTetBDF(const std::string& filename);

	int saveHexBDF(const std::string& filename);

	int saveBDFByGmsh(const std::string& filename, 
		const std::string& meshType
	);

	int saveBDFByGmsh_Face(const std::string& filename,
		const std::string& meshType
	);

	int save3dpmTriMesh(const std::string& filename);

	int gmshReader(const std::string& filename);

	int gmshWriter(const std::string& filename,
		const std::string& meshType
	);

	int gmshWriter_Face(const std::string& filename,
		const std::string& meshType
	);

	int gmshWriter(const std::string& filename);

	int convertBDFToVTKByGmsh(const std::string& filename);

	int convertVTKToOBJ(const std::string& filename);

	int convertVTKToOBJ(const std::string& filename,
		std::vector < std::vector < double > >& points,
		std::vector < std::vector < size_t > >& cells
	);
}

#endif