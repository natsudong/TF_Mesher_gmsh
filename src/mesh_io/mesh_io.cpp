#include "mesh_io.h"

namespace mesh_io {
	int readVTK(const std::string& filename,
		std::vector < std::vector < double > >& points,
		std::vector < std::vector < size_t > >& elements
	)
	{
		points.clear();
		elements.clear();
		std::ifstream input_file(filename);
		if (!input_file.is_open()) {
			std::cout << "Unable to open file";
			return 0;
		}
		std::string line;
		char str[50];
		size_t pointsNum;
		size_t elementsNum;
		size_t elementsLists;
		int elementType;
		double elementNodes[8];
		std::vector < size_t > element;
		while (std::getline(input_file, line)) {
			std::istringstream iss(line);
			if (line.find("POINTS") != std::string::npos) {
				iss >> str >> pointsNum >> str;
				points.resize(pointsNum);
				break;
			}
		}
		double x, y, z;
		for (size_t i = 0; i < pointsNum; ++i) {
			std::getline(input_file, line);
			std::istringstream iss(line);
			iss >> x >> y >> z;
			points[i].emplace_back(x);
			points[i].emplace_back(y);
			points[i].emplace_back(z);
		}
		while (std::getline(input_file, line)) {
			std::istringstream iss(line);
			if (line.find("CELLS") != std::string::npos) {
				iss >> str >> elementsNum >> elementsLists;
				elements.resize(elementsNum);
				break;
			}
		}
		for (size_t i = 0; i < elementsNum; ++i) {
			std::getline(input_file, line);
			std::istringstream iss(line);
			iss >> elementType;
			switch (elementType) {
			case 1:
				iss >> elementNodes[0];
				elements[i].emplace_back(elementNodes[0]);
				break;
			case 2:
				iss >> elementNodes[0] >> elementNodes[1];
				for (int j = 0; j < 2; ++j) {
					elements[i].emplace_back(elementNodes[j]);
				}
				break;
			case 3:
				iss >> elementNodes[0] >> elementNodes[1]
					>> elementNodes[2];
				for (int j = 0; j < 3; ++j) {
					elements[i].emplace_back(elementNodes[j]);
				}
				break;
			case 4:
				iss >> elementNodes[0] >> elementNodes[1]
					>> elementNodes[2] >> elementNodes[3];
				for (int j = 0; j < 4; ++j) {
					elements[i].emplace_back(elementNodes[j]);
				}
				break;
			case 5:
				iss >> elementNodes[0] >> elementNodes[1]
					>> elementNodes[2] >> elementNodes[3]
					>> elementNodes[4];
				for (int j = 0; j < 5; ++j) {
					elements[i].emplace_back(elementNodes[j]);
				}
				break;
			case 6:
				iss >> elementNodes[0] >> elementNodes[1]
					>> elementNodes[2] >> elementNodes[3]
					>> elementNodes[4] >> elementNodes[5];
				for (int j = 0; j < 6; ++j) {
					elements[i].emplace_back(elementNodes[j]);
				}
				break;
			case 7:
				iss >> elementNodes[0] >> elementNodes[1]
					>> elementNodes[2] >> elementNodes[3]
					>> elementNodes[4] >> elementNodes[5]
					>> elementNodes[6];
				for (int j = 0; j < 7; ++j) {
					elements[i].emplace_back(elementNodes[j]);
				}
				break;
			case 8:
				iss >> elementNodes[0] >> elementNodes[1]
					>> elementNodes[2] >> elementNodes[3]
					>> elementNodes[4] >> elementNodes[5]
					>> elementNodes[6] >> elementNodes[7];
				for (int j = 0; j < 8; ++j) {
					elements[i].emplace_back(elementNodes[j]);
				}
				break;
			default:
				std::cout << "Error : Unsupport Type\n";
				return 0;
				break;
			}
		}
		input_file.close();
		std::cout << "read mesh success\n";
		return 1;
	}

	int saveVTK(const std::string& filename,
		const std::vector < std::vector < double > >& points,
		const std::vector < std::vector < size_t > >& elements
	)
	{

		std::ofstream os;
		os.open(filename);
		os << "# vtk DataFile Version 3.0\nUnstructured mesh\nASCII\nDATASET UNSTRUCTURED_GRID\n";

		os << "POINTS " << points.size() << " double\n";
		for (size_t i = 0; i < points.size(); ++i) {
			os << points[i][0] << " " << points[i][1] << " " << points[i][2] << "\n";
		}
		os << "CELLS " << elements.size() << " ";
		size_t elementListsNum = 0;
		for (size_t i = 0; i < elements.size(); ++i) {
			elementListsNum += (elements[i].size() + 1);
		}
		os << elementListsNum << "\n";
		std::vector < int > vtkTypes;
		for (size_t i = 0; i < elements.size(); ++i) {
			switch (elements[i].size()) {
			case 1:
				os << "1 " << elements[i][0] << "\n";
				vtkTypes.emplace_back(1);
				break;
			case 2:
				os << "2 " << elements[i][0] << " " << elements[i][1] << "\n";
				vtkTypes.emplace_back(3);
				break;
			case 3:
				os << "3 " << elements[i][0] << " " << elements[i][1] << " "
					<< elements[i][2] << "\n";
				vtkTypes.emplace_back(5);
				break;
			case 4:
				os << "4 " << elements[i][0] << " " << elements[i][1] << " "
					<< elements[i][2] << " " << elements[i][3] << "\n";
				vtkTypes.emplace_back(10);
				break;
			case 5:
				os << "5 " << elements[i][0] << " " << elements[i][1] << " "
					<< elements[i][2] << " " << elements[i][3] << " "
					<< elements[i][4] << "\n";
				vtkTypes.emplace_back(14);
				break;
			case 6:
				os << "6 " << elements[i][0] << " " << elements[i][1] << " "
					<< elements[i][2] << " " << elements[i][3] << " "
					<< elements[i][4] << " " << elements[i][5] << "\n";
				vtkTypes.emplace_back(13);
				break;
			case 8:
				os << "8 " << elements[i][0] << " " << elements[i][1] << " "
					<< elements[i][2] << " " << elements[i][3] << " "
					<< elements[i][4] << " " << elements[i][5] << " "
					<< elements[i][6] << " " << elements[i][7] << "\n";
				vtkTypes.emplace_back(12);
				break;
			default:
				std::cout << "Error : Unsupport Type\n";
				return 0;
				break;
			}
		}
		os << "CELL_TYPES " << elements.size() << "\n";
		for (size_t i = 0; i < elements.size(); ++i)
		{
			os << vtkTypes[i] << "\n";
		}
		os.close();
		std::cout << "save VTK success\n";
		return 1;
	}

	int readOBJ(const std::string& filename, std::vector < std::vector < double > >& points, std::vector < std::vector < size_t > >& elements)
	{
		points.clear();
		elements.clear();
		std::ifstream is(filename);
		if (!is) {
			std::cout << "ERROR : FILE IS NOT EXIST!" << "\n";
			abort();
		}
		std::vector<double>  vs;
		std::vector<size_t>  fs;
		std::string line, pair[3];
		double  node[3];
		int  tri;
		while (!is.eof()) {
			std::getline(is, line);
			if (line.empty() || 13 == line[0])
				continue;
			std::istringstream instream(line);
			std::string word;
			instream >> word;
			if ("v" == word || "V" == word) {
				instream >> node[0] >> node[1] >> node[2];
				for (size_t j = 0; j < 3; ++j) {
					vs.emplace_back(node[j]);
				}
			}
			else if ('f' == word[0] || 'F' == word[0]) {
				instream >> pair[0] >> pair[1] >> pair[2];
				for (size_t j = 0; j < 3; ++j) {
					tri = strtoul(pair[j].c_str(), NULL, 10) - 1;
					fs.emplace_back(tri);
				}
			}
		}
		is.close();
		for (size_t i = 0; i < vs.size(); i += 3) {
			std::vector < double > cv(3);
			cv[0] = vs[i];
			cv[1] = vs[i + 1];
			cv[2] = vs[i + 2];
			points.emplace_back(cv);
		}
		for (size_t i = 0; i < fs.size(); i += 3) {
			std::vector < size_t > cf(3);
			cf[0] = fs[i];
			cf[1] = fs[i + 1];
			cf[2] = fs[i + 2];
			elements.emplace_back(cf);
		}
		return 1;
	}

	int saveOBJ(const std::string& filename,
		const std::vector < std::vector < double > >& points,
		const std::vector < std::vector < size_t > >& tris
	)
	{
		std::ofstream os(filename);
		for (size_t i = 0; i < points.size(); ++i) {
			os << "v " << points[i][0] << " " << points[i][1] << " " << points[i][2] << "\n";
		}
		for (size_t i = 0; i < tris.size(); ++i) {
			if (tris[i].size() == 3)
			{
				os << "f " << tris[i][0] + 1 << " " << tris[i][1] + 1 << " " << tris[i][2] + 1 << "\n";
			}
			if (tris[i].size() == 4)
			{
				os << "f " << tris[i][0] + 1 << " " << tris[i][1] + 1 << " " << tris[i][2] + 1 << " " << tris[i][3] + 1 << "\n";
			}
		}
		os.close();
		std::cout << "save OBJ success\n";
		return 1;
	}

	int saveTriBDF(const std::string& filename)
	{
		std::vector < std::pair < int, int > > curveDimTags;
		std::vector < std::pair < int, int > > surfaceDimTags;
		std::vector < std::pair < int, int > > volumeDimTags;
		std::multimap<int, int > surfaceVolumeMap;
		gmsh::model::getEntities(curveDimTags, 1);
		gmsh::model::getEntities(surfaceDimTags, 2);
		gmsh::model::getEntities(volumeDimTags, 3);
		std::vector < size_t > nodeTags;
		std::vector < double > coord;
		std::vector < double > paramCoord;
		gmsh::model::mesh::getNodes(nodeTags, coord, paramCoord);
		std::vector < int > elemTypes;
		std::vector < std::vector < size_t > > elemTags;
		std::vector < std::vector < size_t > > elemNodeTags;
		std::ofstream os;
		os.open(filename);
		for (size_t i = 0; i < nodeTags.size(); ++i) {
			os << std::left << std::setw(8) << "GRID*"
				<< std::left << std::setw(16) << nodeTags[i]
				<< std::left << std::setw(16) << "0"
				<< std::left << std::setw(16) << coord[i * 3]
				<< std::left << std::setw(16) << coord[i * 3 + 1] << std::endl
				<< std::left << std::setw(8) << "*"
				<< std::left << std::setw(16) << coord[i * 3 + 2] << std::endl;
		}

		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PSOLID"
				<< std::left << std::setw(8) << volumeDimTags[i].second
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0" << std::endl;
		}
		std::vector < std::pair < int, int > > singleVolume(1);
		std::vector < std::pair < int, int > > singleSurface(1);
		int maxSurTag = gmsh::model::occ::getMaxTag(2);
		size_t maxElemTag;
		gmsh::model::mesh::getMaxElementTag(maxElemTag);
		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			singleVolume[0] = volumeDimTags[i];
			std::vector < std::pair < int, int > > outSurfaceDimTags;
			gmsh::model::getBoundary(singleVolume, outSurfaceDimTags, false);
			for (auto& surDimTag : outSurfaceDimTags)
			{
				surfaceVolumeMap.emplace(std::abs(surDimTag.second), singleVolume[0].second);
			}
		}

		for (size_t i = 0; i < surfaceDimTags.size(); ++i) {
			auto surfaceVolumeMapIte = surfaceVolumeMap.lower_bound(surfaceDimTags[i].second);
			auto surfaceVolumeMapIte_end = surfaceVolumeMap.upper_bound(surfaceDimTags[i].second);
			if (surfaceVolumeMapIte == surfaceVolumeMapIte_end)
			{
				os << std::left << std::setw(8) << "PSHELL"
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << -1
					<< std::left << std::setw(8) << "0" << std::endl;
			}
			for (; surfaceVolumeMapIte != surfaceVolumeMapIte_end; surfaceVolumeMapIte++)
			{
				os << std::left << std::setw(8) << "PSHELL"
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << surfaceVolumeMapIte->second
					<< std::left << std::setw(8) << "0" << std::endl;
			}
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, surfaceDimTags[i].first, std::abs(surfaceDimTags[i].second));
			for (size_t k = 0; k < elemTags[0].size(); ++k) {
				os << std::left << std::setw(8) << "CTRIA3"
					<< std::left << std::setw(8) << elemTags[0][k]
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][k * 3]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 3 + 1]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 3 + 2] << std::endl;
			}
		}

		for (size_t i = 0; i < curveDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PBAR"
				<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00" << std::endl;
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, curveDimTags[i].first, std::abs(curveDimTags[i].second));
			for (size_t j = 0; j < elemTags[0].size(); ++j) {
				os << std::left << std::setw(8) << "CBAR"
					<< std::left << std::setw(8) << elemTags[0][j]
					<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2 + 1]
					<< std::left << "1.0000001.0000001.000000" << std::endl;
			}
		}

		os << "ENDDATA" << std::endl;
		os.close();
		std::cout << "Done writing " << "'" << filename << "'" << std::endl;
		return 1;
	}

	int saveTriBDF_Face(const std::string& filename
	)
	{
		std::vector < std::pair < int, int > > curveDimTags;
		std::vector < std::pair < int, int > > volumeDimTags;
		gmsh::model::getEntities(curveDimTags, 1);
		gmsh::model::getEntities(volumeDimTags, 3);
		std::vector < size_t > nodeTags;
		std::vector < double > coord;
		std::vector < double > paramCoord;
		gmsh::model::mesh::getNodes(nodeTags, coord, paramCoord);
		std::vector < int > elemTypes;
		std::vector < std::vector < size_t > > elemTags;
		std::vector < std::vector < size_t > > elemNodeTags;
		std::ofstream os;
		os.open(filename);
		for (size_t i = 0; i < nodeTags.size(); ++i) {
			os << std::left << std::setw(8) << "GRID*"
				<< std::left << std::setw(16) << nodeTags[i]
				<< std::left << std::setw(16) << "0"
				<< std::left << std::setw(16) << coord[i * 3]
				<< std::left << std::setw(16) << coord[i * 3 + 1] << std::endl
				<< std::left << std::setw(8) << "*"
				<< std::left << std::setw(16) << coord[i * 3 + 2] << std::endl;
		}

		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PSOLID"
				<< std::left << std::setw(8) << volumeDimTags[i].second
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0" << std::endl;
		}
		std::vector < std::pair < int, int > > singleVolume(1);
		std::vector < std::pair < int, int > > singleSurface(1);
		int maxSurTag = gmsh::model::occ::getMaxTag(2);
		size_t maxElemTag;
		gmsh::model::mesh::getMaxElementTag(maxElemTag);

		gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, 2);
		for (size_t k = 0; k < elemTags[0].size(); ++k) {
			os << std::left << std::setw(8) << "CTRIA3"
				<< std::left << std::setw(8) << elemTags[0][k]
				<< std::left << std::setw(8) << 1
				<< std::left << std::setw(8) << elemNodeTags[0][k * 3]
				<< std::left << std::setw(8) << elemNodeTags[0][k * 3 + 1]
				<< std::left << std::setw(8) << elemNodeTags[0][k * 3 + 2] << std::endl;
		}
		for (size_t i = 0; i < curveDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PBAR"
				<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00" << std::endl;
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, curveDimTags[i].first, std::abs(curveDimTags[i].second));
			for (size_t j = 0; j < elemTags[0].size(); ++j) {
				os << std::left << std::setw(8) << "CBAR"
					<< std::left << std::setw(8) << elemTags[0][j]
					<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2 + 1]
					<< std::left << "1.0000001.0000001.000000" << std::endl;
			}
		}
		int maxCurveTag = gmsh::model::occ::getMaxTag(1);
		os << "ENDDATA" << std::endl;
		os.close();
		std::cout << "Done writing " << "'" << filename << "'" << std::endl;
		return 1;
	}

	int saveQuadBDF(const std::string& filename
	)
	{
		std::vector < std::pair < int, int > > curveDimTags;
		std::vector < std::pair < int, int > > surfaceDimTags;
		std::vector < std::pair < int, int > > volumeDimTags;
		std::multimap<int, int > surfaceVolumeMap;
		gmsh::model::getEntities(curveDimTags, 1);
		gmsh::model::getEntities(surfaceDimTags, 2);
		gmsh::model::getEntities(volumeDimTags, 3);
		std::vector < size_t > nodeTags;
		std::vector < double > coord;
		std::vector < double > paramCoord;
		gmsh::model::mesh::getNodes(nodeTags, coord, paramCoord);
		std::vector < int > elemTypes;
		std::vector < std::vector < size_t > > elemTags;
		std::vector < std::vector < size_t > > elemNodeTags;
		std::ofstream os;
		os.open(filename);
		for (size_t i = 0; i < nodeTags.size(); ++i) {
			os << std::left << std::setw(8) << "GRID*"
				<< std::left << std::setw(16) << nodeTags[i]
				<< std::left << std::setw(16) << "0"
				<< std::left << std::setw(16) << coord[i * 3]
				<< std::left << std::setw(16) << coord[i * 3 + 1] << std::endl
				<< std::left << std::setw(8) << "*"
				<< std::left << std::setw(16) << coord[i * 3 + 2] << std::endl;
		}
		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PSOLID"
				<< std::left << std::setw(8) << volumeDimTags[i].second
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0" << std::endl;
		}
		std::vector < std::pair < int, int > > singleVolume(1);
		std::vector < std::pair < int, int > > singleSurface(1);
		int maxSurTag = gmsh::model::occ::getMaxTag(2);
		size_t maxElemTag;
		gmsh::model::mesh::getMaxElementTag(maxElemTag);

		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			singleVolume[0] = volumeDimTags[i];
			std::vector < std::pair < int, int > > outSurfaceDimTags;
			gmsh::model::getBoundary(singleVolume, outSurfaceDimTags, false);
			for (auto& surDimTag : outSurfaceDimTags)
			{
				surfaceVolumeMap.emplace(std::abs(surDimTag.second), singleVolume[0].second);
			}
		}

		for (size_t i = 0; i < surfaceDimTags.size(); ++i) {
			auto surfaceVolumeMapIte = surfaceVolumeMap.lower_bound(surfaceDimTags[i].second);
			auto surfaceVolumeMapIte_end = surfaceVolumeMap.upper_bound(surfaceDimTags[i].second);
			if (surfaceVolumeMapIte == surfaceVolumeMapIte_end)
			{
				os << std::left << std::setw(8) << "PSHELL"
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << -1
					<< std::left << std::setw(8) << "0" << std::endl;
			}
			for (; surfaceVolumeMapIte != surfaceVolumeMapIte_end; surfaceVolumeMapIte++)
			{
				os << std::left << std::setw(8) << "PSHELL"
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << surfaceVolumeMapIte->second
					<< std::left << std::setw(8) << "0" << std::endl;
			}
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, surfaceDimTags[i].first, std::abs(surfaceDimTags[i].second));
			for (size_t k = 0; k < elemTags[0].size(); ++k) {
				os << std::left << std::setw(8) << "CQUAD4"
					<< std::left << std::setw(8) << elemTags[0][k]
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][k * 4]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 4 + 1]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 4 + 2]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 4 + 3] << std::endl;
			}
		}

		for (size_t i = 0; i < curveDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PBAR"
				<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00" << std::endl;
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, curveDimTags[i].first, std::abs(curveDimTags[i].second));
			for (size_t j = 0; j < elemTags[0].size(); ++j) {
				os << std::left << std::setw(8) << "CBAR"
					<< std::left << std::setw(8) << elemTags[0][j]
					<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2 + 1]
					<< std::left << "1.0000001.0000001.000000" << std::endl;
			}
		}
		os << "ENDDATA" << std::endl;
		os.close();
		std::cout << "Done writing " << "'" << filename << "'" << std::endl;
		return 1;
	}

	int saveQuadBDF_Face(const std::string& filename
	)
	{
		std::vector < std::pair < int, int > > curveDimTags;
		std::vector < std::pair < int, int > > surfaceDimTags;
		std::vector < std::pair < int, int > > volumeDimTags;
		gmsh::model::getEntities(curveDimTags, 1);
		gmsh::model::getEntities(surfaceDimTags, 2);
		gmsh::model::getEntities(volumeDimTags, 3);
		std::vector < size_t > nodeTags;
		std::vector < double > coord;
		std::vector < double > paramCoord;
		gmsh::model::mesh::getNodes(nodeTags, coord, paramCoord);
		std::vector < int > elemTypes;
		std::vector < std::vector < size_t > > elemTags;
		std::vector < std::vector < size_t > > elemNodeTags;
		std::ofstream os;
		os.open(filename);
		for (size_t i = 0; i < nodeTags.size(); ++i) {
			os << std::left << std::setw(8) << "GRID*"
				<< std::left << std::setw(16) << nodeTags[i]
				<< std::left << std::setw(16) << "0"
				<< std::left << std::setw(16) << coord[i * 3]
				<< std::left << std::setw(16) << coord[i * 3 + 1] << std::endl
				<< std::left << std::setw(8) << "*"
				<< std::left << std::setw(16) << coord[i * 3 + 2] << std::endl;
		}

		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PSOLID"
				<< std::left << std::setw(8) << volumeDimTags[i].second
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0" << std::endl;
		}
		std::vector < std::pair < int, int > > singleVolume(1);
		std::vector < std::pair < int, int > > singleSurface(1);
		int maxSurTag = gmsh::model::occ::getMaxTag(2);
		size_t maxElemTag;
		gmsh::model::mesh::getMaxElementTag(maxElemTag);
		gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, 2);
		for (size_t k = 0; k < elemTags[0].size(); ++k) {
			os << std::left << std::setw(8) << "CQUAD4"
				<< std::left << std::setw(8) << elemTags[0][k]
				<< std::left << std::setw(8) << 1
				<< std::left << std::setw(8) << elemNodeTags[0][k * 4]
				<< std::left << std::setw(8) << elemNodeTags[0][k * 4 + 1]
				<< std::left << std::setw(8) << elemNodeTags[0][k * 4 + 2]
				<< std::left << std::setw(8) << elemNodeTags[0][k * 4 + 3] << std::endl;
		}
		for (size_t i = 0; i < curveDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PBAR"
				<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00" << std::endl;
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, curveDimTags[i].first, std::abs(curveDimTags[i].second));
			for (size_t j = 0; j < elemTags[0].size(); ++j) {
				os << std::left << std::setw(8) << "CBAR"
					<< std::left << std::setw(8) << elemTags[0][j]
					<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2 + 1]
					<< std::left << "1.0000001.0000001.000000" << std::endl;
			}
		}
		int maxCurveTag = gmsh::model::occ::getMaxTag(1);
		os << "ENDDATA" << std::endl;
		os.close();
		std::cout << "Done writing " << "'" << filename << "'" << std::endl;
		return 1;
	}

	int saveTetBDF(const std::string& filename
	)
	{
		std::vector < std::pair < int, int > > curveDimTags;
		std::vector < std::pair < int, int > > surfaceDimTags;
		std::vector < std::pair < int, int > > volumeDimTags;
		std::multimap<int, int > surfaceVolumeMap;
		gmsh::model::getEntities(curveDimTags, 1);
		gmsh::model::getEntities(surfaceDimTags, 2);
		gmsh::model::getEntities(volumeDimTags, 3);
		std::vector < size_t > nodeTags;
		std::vector < double > coord;
		std::vector < double > paramCoord;
		gmsh::model::mesh::getNodes(nodeTags, coord, paramCoord);
		std::vector < int > elemTypes;
		std::vector < std::vector < size_t > > elemTags;
		std::vector < std::vector < size_t > > elemNodeTags;
		std::ofstream os;
		os.open(filename);
		for (size_t i = 0; i < nodeTags.size(); ++i) {
			os << std::left << std::setw(8) << "GRID*"
				<< std::left << std::setw(16) << nodeTags[i]
				<< std::left << std::setw(16) << "0"
				<< std::left << std::setw(16) << coord[i * 3]
				<< std::left << std::setw(16) << coord[i * 3 + 1] << std::endl
				<< std::left << std::setw(8) << "*"
				<< std::left << std::setw(16) << coord[i * 3 + 2] << std::endl;
		}
		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PSOLID"
				<< std::left << std::setw(8) << volumeDimTags[i].second
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0" << std::endl;
		}
		std::vector < std::pair < int, int > > singleVolume(1);
		std::vector < std::pair < int, int > > singleSurface(1);
		int maxSurTag = gmsh::model::occ::getMaxTag(2);
		size_t maxElemTag;
		gmsh::model::mesh::getMaxElementTag(maxElemTag);


		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			singleVolume[0] = volumeDimTags[i];
			std::vector < std::pair < int, int > > outSurfaceDimTags;
			gmsh::model::getBoundary(singleVolume, outSurfaceDimTags, false);
			for (auto& surDimTag : outSurfaceDimTags)
			{
				surfaceVolumeMap.emplace(std::abs(surDimTag.second), singleVolume[0].second);
			}
		}

		for (size_t i = 0; i < surfaceDimTags.size(); ++i) {
			auto surfaceVolumeMapIte = surfaceVolumeMap.lower_bound(surfaceDimTags[i].second);
			auto surfaceVolumeMapIte_end = surfaceVolumeMap.upper_bound(surfaceDimTags[i].second);
			if (surfaceVolumeMapIte == surfaceVolumeMapIte_end)
			{
				os << std::left << std::setw(8) << "PSHELL"
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << -1
					<< std::left << std::setw(8) << "0" << std::endl;
			}
			for (; surfaceVolumeMapIte != surfaceVolumeMapIte_end; surfaceVolumeMapIte++)
			{
				os << std::left << std::setw(8) << "PSHELL"
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << surfaceVolumeMapIte->second
					<< std::left << std::setw(8) << "0" << std::endl;
			}
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, surfaceDimTags[i].first, std::abs(surfaceDimTags[i].second));
			for (size_t k = 0; k < elemTags[0].size(); ++k) {
				os << std::left << std::setw(8) << "CTRIA3"
					<< std::left << std::setw(8) << elemTags[0][k]
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][k * 3]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 3 + 1]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 3 + 2] << std::endl;
			}
		}

		//for (size_t i = 0; i < volumeDimTags.size(); ++i) {
		//	singleVolume[0] = volumeDimTags[i];
		//	std::vector < std::pair < int, int > > outSurfaceDimTags;
		//	gmsh::model::getBoundary(singleVolume, outSurfaceDimTags, false);
		//	for (size_t j = 0; j < outSurfaceDimTags.size(); ++j) {
		//		os << std::left << std::setw(8) << "PSHELL"
		//			<< std::left << std::setw(8) << std::abs(outSurfaceDimTags[j].second)
		//			<< std::left << std::setw(8) << volumeDimTags[i].second
		//			<< std::left << std::setw(8) << "0" << std::endl;
		//		gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, outSurfaceDimTags[j].first, std::abs(outSurfaceDimTags[j].second));
		//		for (size_t k = 0; k < elemTags[0].size(); ++k) {
		//			os << std::left << std::setw(8) << "CTRIA3"
		//				<< std::left << std::setw(8) << elemTags[0][k]
		//				<< std::left << std::setw(8) << std::abs(outSurfaceDimTags[j].second)
		//				<< std::left << std::setw(8) << elemNodeTags[0][k * 3]
		//				<< std::left << std::setw(8) << elemNodeTags[0][k * 3 + 1]
		//				<< std::left << std::setw(8) << elemNodeTags[0][k * 3 + 2] << std::endl;
		//		}

		//	}
		//}

		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, volumeDimTags[i].first, volumeDimTags[i].second);
			for (size_t j = 0; j < elemTags[0].size(); ++j) {
				os << std::left << std::setw(8) << "CTETRA"
					<< std::left << std::setw(8) << elemTags[0][j]
					<< std::left << std::setw(8) << volumeDimTags[i].second
					<< std::left << std::setw(8) << elemNodeTags[0][j * 4]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 4 + 1]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 4 + 2]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 4 + 3] << std::endl;
			}
		}
		for (size_t i = 0; i < curveDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PBAR"
				<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00" << std::endl;
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, curveDimTags[i].first, std::abs(curveDimTags[i].second));
			for (size_t j = 0; j < elemTags[0].size(); ++j) {
				os << std::left << std::setw(8) << "CBAR"
					<< std::left << std::setw(8) << elemTags[0][j]
					<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2 + 1]
					<< std::left << "1.0000001.0000001.000000" << std::endl;
			}
		}
		os << "ENDDATA" << std::endl;
		os.close();
		std::cout << "Done writing " << "'" << filename << "'" << std::endl;
		return 1;
	}

	int saveHexBDF(const std::string& filename
	)
	{
		std::vector < std::pair < int, int > > curveDimTags;
		std::vector < std::pair < int, int > > surfaceDimTags;
		std::vector < std::pair < int, int > > volumeDimTags;
		std::multimap<int, int > surfaceVolumeMap;
		gmsh::model::getEntities(curveDimTags, 1);
		gmsh::model::getEntities(surfaceDimTags, 2);
		gmsh::model::getEntities(volumeDimTags, 3);
		std::vector < size_t > nodeTags;
		std::vector < double > coord;
		std::vector < double > paramCoord;
		gmsh::model::mesh::getNodes(nodeTags, coord, paramCoord);
		std::vector < int > elemTypes;
		std::vector < std::vector < size_t > > elemTags;
		std::vector < std::vector < size_t > > elemNodeTags;
		std::ofstream os;
		os.open(filename);
		for (size_t i = 0; i < nodeTags.size(); ++i) {
			os << std::left << std::setw(8) << "GRID*"
				<< std::left << std::setw(16) << nodeTags[i]
				<< std::left << std::setw(16) << "0"
				<< std::left << std::setw(16) << coord[i * 3]
				<< std::left << std::setw(16) << coord[i * 3 + 1] << std::endl
				<< std::left << std::setw(8) << "*"
				<< std::left << std::setw(16) << coord[i * 3 + 2] << std::endl;
		}
		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PSOLID"
				<< std::left << std::setw(8) << volumeDimTags[i].second
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0" << std::endl;
		}
		std::vector < std::pair < int, int > > singleVolume(1);
		std::vector < std::pair < int, int > > singleSurface(1);
		int maxSurTag = gmsh::model::occ::getMaxTag(2);
		size_t maxElemTag;
		gmsh::model::mesh::getMaxElementTag(maxElemTag);

		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			singleVolume[0] = volumeDimTags[i];
			std::vector < std::pair < int, int > > outSurfaceDimTags;
			gmsh::model::getBoundary(singleVolume, outSurfaceDimTags, false);
			for (auto& surDimTag : outSurfaceDimTags)
			{
				surfaceVolumeMap.emplace(std::abs(surDimTag.second), singleVolume[0].second);
			}
		}

		for (size_t i = 0; i < surfaceDimTags.size(); ++i) {
			auto surfaceVolumeMapIte = surfaceVolumeMap.lower_bound(surfaceDimTags[i].second);
			auto surfaceVolumeMapIte_end = surfaceVolumeMap.upper_bound(surfaceDimTags[i].second);
			if (surfaceVolumeMapIte == surfaceVolumeMapIte_end)
			{
				os << std::left << std::setw(8) << "PSHELL"
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << -1
					<< std::left << std::setw(8) << "0" << std::endl;
			}
			for (; surfaceVolumeMapIte != surfaceVolumeMapIte_end; surfaceVolumeMapIte++)
			{
				os << std::left << std::setw(8) << "PSHELL"
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << surfaceVolumeMapIte->second
					<< std::left << std::setw(8) << "0" << std::endl;
			}
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, surfaceDimTags[i].first, std::abs(surfaceDimTags[i].second));
			for (size_t k = 0; k < elemTags[0].size(); ++k) {
				os << std::left << std::setw(8) << "CQUAD4"
					<< std::left << std::setw(8) << elemTags[0][k]
					<< std::left << std::setw(8) << std::abs(surfaceDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][k * 4]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 4 + 1]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 4 + 2]
					<< std::left << std::setw(8) << elemNodeTags[0][k * 4 + 3] << std::endl;
			}
		}

		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, volumeDimTags[i].first, volumeDimTags[i].second);
			for (size_t j = 0; j < elemTags[0].size(); ++j) {
				os << std::left << std::setw(8) << "CHEXA"
					<< std::left << std::setw(8) << elemTags[0][j]
					<< std::left << std::setw(8) << volumeDimTags[i].second
					<< std::left << std::setw(8) << elemNodeTags[0][j * 8]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 8 + 1]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 8 + 2]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 8 + 3]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 8 + 4]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 8 + 5] << std::endl
					<< std::left << std::setw(8) << ""
					<< std::left << std::setw(8) << elemNodeTags[0][j * 8 + 6]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 8 + 7] << std::endl;
			}
		}
		for (size_t i = 0; i < curveDimTags.size(); ++i) {
			os << std::left << std::setw(8) << "PBAR"
				<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
				<< std::left << std::setw(8) << "1"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00"
				<< std::left << std::setw(8) << "0.00" << std::endl;
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, curveDimTags[i].first, std::abs(curveDimTags[i].second));
			for (size_t j = 0; j < elemTags[0].size(); ++j) {
				os << std::left << std::setw(8) << "CBAR"
					<< std::left << std::setw(8) << elemTags[0][j]
					<< std::left << std::setw(8) << std::abs(curveDimTags[i].second)
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2]
					<< std::left << std::setw(8) << elemNodeTags[0][j * 2 + 1]
					<< std::left << "1.0000001.0000001.000000" << std::endl;
			}
		}
		os << "ENDDATA" << std::endl;
		os.close();
		std::cout << "Done writing " << "'" << filename << "'" << std::endl;
		return 1;
	}

	int saveBDFByGmsh(const std::string& filename,
		const std::string& meshType
	)
	{
		if (meshType == "Tri") {
			saveTriBDF(filename);
		}
		else if (meshType == "StructuredQuad") {
			saveQuadBDF(filename);
		}
		else if (meshType == "UnstructuredQuad") {
			saveQuadBDF(filename);
		}
		else if (meshType == "Tet") {
			saveTetBDF(filename);
		}
		else if (meshType == "Hex") {
			saveHexBDF(filename);
		}
		else {
			std::cout << "Unsupport Type" << std::endl;
			return 0;
		}
		return 1;
	}

	int saveBDFByGmsh_Face(const std::string& filename,
		const std::string& meshType
	)
	{
		if (meshType == "Tri") {
			saveTriBDF_Face(filename);
		}
		else if (meshType == "StructuredQuad") {
			saveQuadBDF_Face(filename);
		}
		else if (meshType == "UnstructuredQuad") {
			saveQuadBDF_Face(filename);
		}
		else {
			std::cout << "Unsupport Type" << std::endl;
			return 0;
		}
		return 1;
	}

	int save3dpmTriMesh(const std::string& filename)
	{
		std::vector < size_t > nodeTags;
		std::vector < double > coord;
		std::vector < double > paramCoord;
		std::vector < int > elemTypes;
		std::vector < std::vector < size_t > > elemTags;
		std::vector < std::vector < size_t > > elemNodeTags;
		gmsh::model::mesh::getNodes(nodeTags, coord, paramCoord);
		gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags);
		int triIdx;
		for (int i = 0; i < elemTypes.size(); ++i) {
			if (elemTypes[i] == 2) {
				triIdx = i;
				break;
			}
		}
		std::ofstream os;
		os.open(filename);
		os << "MeshVersionFormatted 3" << std::endl;
		os << std::endl;
		os << "Dimension 3" << std::endl;
		os << std::endl;
		os << "Vertices" << std::endl;
		os << nodeTags.size() << std::endl;
		for (size_t i = 0; i < coord.size(); i += 3) {
			os << coord[i] << " " << coord[i + 1] << " " << coord[i + 2] << " " << 0 << std::endl;
		}
		os << std::endl;
		os << "Triangles" << std::endl;
		os << elemTags[triIdx].size() << std::endl;
		for (size_t i = 0; i < elemNodeTags[triIdx].size(); i += 3) {
			os << elemNodeTags[triIdx][i] << " " << elemNodeTags[triIdx][i + 1] << " "
				<< elemNodeTags[triIdx][i + 2] << " " << 1 << std::endl;
		}
		os << std::endl;
		os << "End" << std::endl;
		os.close();
		std::cout << "Done writing " << "'" << filename << "'" << std::endl;
		return 1;
	}

	int gmshReader(const std::string& filename)
	{
		gmsh::clear();
		gmsh::open(filename);
		return 1;
	}

	int gmshWriter(const std::string& filename,
		const std::string& meshType
	)
	{
		std::string suffixStr = filename.substr(filename.find_last_of('.') + 1);
		if (suffixStr == "bdf" || suffixStr == "BDF") {
			if (!mesh_io::saveBDFByGmsh(filename, meshType)) {
				std::cout << "Save BDF File Failed" << std::endl;
				return 0;
			}
		}
		else if (suffixStr == "mesh" || suffixStr == "MESH" && meshType == "Tri") {
			if (!mesh_io::save3dpmTriMesh(filename)) {
				std::cout << "Save 3dpm Tri Mesh Failed" << std::endl;
				return 0;
			}
		}
		else {
			std::cout << "Unsupport Type" << std::endl;
			return 0;
		}
		return 1;
	}

	int gmshWriter_Face(const std::string& filename,
		const std::string& meshType
	)
	{
		std::string suffixStr = filename.substr(filename.find_last_of('.') + 1);
		if (suffixStr == "bdf" || suffixStr == "BDF") {
			if (!mesh_io::saveBDFByGmsh_Face(filename, meshType)) {
				std::cout << "Save BDF File Failed" << std::endl;
				return 0;
			}
		}
		else if (suffixStr == "mesh" || suffixStr == "MESH" && meshType == "Tri") {
			if (!mesh_io::save3dpmTriMesh(filename)) {
				std::cout << "Save 3dpm Tri Mesh Failed" << std::endl;
				return 0;
			}
		}
		else {
			std::cout << "Unsupport Type" << std::endl;
			return 0;
		}
		return 1;
	}

	int gmshWriter(const std::string& filename)
	{
		gmsh::write(filename);
		return 1;
	}

	int convertBDFToVTKByGmsh(const std::string& filename)
	{
		gmshReader(filename);
		std::string output = filename.substr(0, filename.rfind("."));
		gmshWriter(output + ".vtk");
		return 1;
	}

	int convertVTKToOBJ(const std::string& filename)
	{
		std::vector < std::vector < double > > points;
		std::vector < std::vector < size_t > > cells;
		if (!readVTK(filename, points, cells)) {
			std::cout << "Read VTK Failed\n";
			return 0;
		}
		std::string output = filename.substr(0, filename.rfind("."));
		if (!saveOBJ(output + ".obj", points, cells)) {
			std::cout << "Save OBJ Failed\n";
			return 0;
		}
		return 1;
	}
	int convertVTKToOBJ(const std::string& filename,
		std::vector < std::vector < double > >& points,
		std::vector < std::vector < size_t > >& cells
	)
	{
		points.clear();
		cells.clear();
		if (!readVTK(filename, points, cells)) {
			std::cout << "Read VTK Failed\n";
			return 0;
		}
		std::string output = filename.substr(0, filename.rfind("."));
		if (!saveOBJ(output + ".obj", points, cells)) {
			std::cout << "Save OBJ Failed\n";
			return 0;
		}
		return 1;
	}
}