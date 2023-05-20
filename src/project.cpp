#include "project.h"
#include <fstream>

void Project::geoToMesh_ST_test()
{
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < std::pair < int, int > > volumeDimTags;
	std::vector < std::pair < int, int > > faceDimTags;
	std::vector < std::pair < int, int > > allDimTags;

	volumeDimTags.emplace_back(std::pair<int,int>(3, 5));
	faceDimTags.emplace_back(std::pair<int,int>(2, 41));

	//gmsh::model::getEntities(volumeDimTags, 3);
	std::vector < std::pair < int, int > > outDimTags;
	std::vector < std::vector < std::pair < int, int > > > outDimTagsMap;
	gmsh::model::getEntities(allDimTags, 2);

	//outDimTags多出来部分？
	gmsh::model::occ::fragment(volumeDimTags, faceDimTags, outDimTags, outDimTagsMap);
	gmsh::model::occ::synchronize();
	gmsh::model::getEntities(allDimTags, 2);


	volumeDimTags.clear();
	faceDimTags.clear();
	volumeDimTags.emplace_back(std::pair<int, int>(3, 6));
	for (int i = 0; i < outDimTagsMap.at(volumeDimTags.size()).size(); i++)
	{
		faceDimTags.emplace_back(outDimTagsMap.at(volumeDimTags.size()).at(i));
	}
	gmsh::model::occ::fragment(volumeDimTags, faceDimTags, outDimTags, outDimTagsMap);
	gmsh::model::getEntities(allDimTags, 2);


	gmsh::model::occ::synchronize();
	generate_mesh::generateMesh(m_meshType);

	mesh_io::gmshWriter(m_outputFile);
	//mesh_io::gmshWriter(m_outputFile, m_meshType, modifyVolSurMap);
	gmsh::finalize();
}

void Project::geoToMesh()
{
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < std::pair < int, int > > volumeDimTags;
	std::vector < std::pair < int, int > > allDimTags;
	std::vector < int > centerTags;

	if (m_addMassCenter)
	{
		gmsh::model::getEntities(volumeDimTags, 3);
		for (auto& volumeTag : volumeDimTags)
		{
			std::vector < int > tmpCenterTags;
			double x, y, z;
			int tmpNewTag;
			gmsh::model::occ::getCenterOfMass(volumeTag.first, volumeTag.second, x, y, z);
			tmpNewTag = gmsh::model::occ::addPoint(x, y, z);
			tmpCenterTags.emplace_back(tmpNewTag);
			centerTags.emplace_back(tmpNewTag);
			gmsh::model::occ::synchronize();
			gmsh::model::mesh::embed(0, tmpCenterTags, volumeTag.first, volumeTag.second);
		}

		std::ofstream centerTagsOut;
		centerTagsOut.open("centers.txt", std::ios::out);
		for (int i = 0; i < centerTags.size(); i++)
		{
			centerTagsOut << volumeDimTags[i].second << " " << centerTags[i] << std::endl;
		}
	}

	

	gmsh::model::getEntities(allDimTags);
	gmsh::model::getEntities(volumeDimTags, 3);
	std::vector < std::pair < int, int > > outDimTags;
	std::vector < std::pair < int, int > > singleVolumeDimTags(1);
	std::vector < std::vector < std::pair < int, int > > > outDimTagsMap;
	
	gmsh::model::occ::intersect(volumeDimTags, volumeDimTags, outDimTags, outDimTagsMap);
	gmsh::model::occ::synchronize();
	for (int i = 0; i < volumeDimTags.size(); ++i) {
		gmsh::model::mesh::setOutwardOrientation(volumeDimTags[i].second);
	}
	gmsh::model::occ::synchronize();
	generate_mesh::generateMesh(m_meshType);
	std::unordered_map < size_t, std::vector < size_t > > volumeSurfaceMap;
	std::vector < int > elemTypes;
	std::vector < std::vector < size_t > > elemTags;
	std::vector < std::vector < size_t > > elemNodeTags;

	//save volumes that need to be "YiShun"
	std::vector < size_t > modifyVolSet;
	//volumes that already satisfy "YiShun"
	std::vector < size_t > preserveVolSet;

	//识别接触面
	std::vector < size_t > edge(2);
	for (size_t i = 0; i < volumeDimTags.size(); ++i) {
		singleVolumeDimTags[0] = volumeDimTags[i];
		gmsh::model::getBoundary(singleVolumeDimTags, outDimTags, false, false);
		std::vector < size_t > surface(outDimTags.size());
		for (size_t j = 0; j < surface.size(); ++j) {
			surface[j] = outDimTags[j].second;
		}
		volumeSurfaceMap.insert(std::pair < size_t, std::vector < size_t > >(volumeDimTags[i].second, surface));
		std::map < std::vector < size_t >, int > edgeMap;
		bool flag = true;
		for (size_t j = 0; j < outDimTags.size() && flag == true; ++j) {
			gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, outDimTags[j].first, outDimTags[j].second);
			//tri
			if (elemTypes[0] == 2) {
				for (size_t k = 0; k < elemNodeTags[0].size(); k += 3) {
					// query first edge
					edge[0] = elemNodeTags[0][k]; edge[1] = elemNodeTags[0][k + 1];
					if(edgeMap.count(edge)) {
						modifyVolSet.emplace_back(volumeDimTags[i].second);
						flag = false;
						break;
					}
					else {
						edgeMap.insert(std::pair < std::vector < size_t >, int >(edge, 1));
					}
					// query second edge
					edge[0] = elemNodeTags[0][k + 1]; edge[1] = elemNodeTags[0][k + 2];
					if(edgeMap.count(edge)) {
						modifyVolSet.emplace_back(volumeDimTags[i].second);
						flag = false;
						break;
					}
					else {
						edgeMap.insert(std::pair < std::vector < size_t >, int >(edge, 1));
					}
					// query third edge
					edge[0] = elemNodeTags[0][k + 2]; edge[1] = elemNodeTags[0][k];
					if(edgeMap.count(edge)) {
						modifyVolSet.emplace_back(volumeDimTags[i].second);
						flag = false;
						break;
					}
					else {
						edgeMap.insert(std::pair < std::vector < size_t >, int >(edge, 1));
					}
				}
			}
			//quad
			if (elemTypes[0] == 3) {
				for (size_t k = 0; k < elemNodeTags[0].size(); k += 4) {
					// query first edge
					edge[0] = elemNodeTags[0][k]; edge[1] = elemNodeTags[0][k + 1];
					if (edgeMap.count(edge)) {
						modifyVolSet.emplace_back(volumeDimTags[i].second);
						flag = false;
						break;
					}
					else {
						edgeMap.insert(std::pair < std::vector < size_t >, int >(edge, 1));
					}
					// query second edge
					edge[0] = elemNodeTags[0][k + 1]; edge[1] = elemNodeTags[0][k + 2];
					if (edgeMap.count(edge)) {
						modifyVolSet.emplace_back(volumeDimTags[i].second);
						flag = false;
						break;
					}
					else {
						edgeMap.insert(std::pair < std::vector < size_t >, int >(edge, 1));
					}
					// query third edge
					edge[0] = elemNodeTags[0][k + 2]; edge[1] = elemNodeTags[0][k + 3];
					if (edgeMap.count(edge)) {
						modifyVolSet.emplace_back(volumeDimTags[i].second);
						flag = false;
						break;
					}
					else {
						edgeMap.insert(std::pair < std::vector < size_t >, int >(edge, 1));
					}
					// query fourth edge
					edge[0] = elemNodeTags[0][k + 3]; edge[1] = elemNodeTags[0][k];
					if (edgeMap.count(edge)) {
						modifyVolSet.emplace_back(volumeDimTags[i].second);
						flag = false;
						break;
					}
					else {
						edgeMap.insert(std::pair < std::vector < size_t >, int >(edge, 1));
					}
				}
			}
		}
	}
	for (int i = 0; i < volumeDimTags.size(); ++i) {
		if (isInVector(modifyVolSet, volumeDimTags[i].second)) continue;
		preserveVolSet.emplace_back(volumeDimTags[i].second);
	}
	std::unordered_map < size_t, std::vector < size_t > > modifyVolSurMap;
	std::vector < size_t > modifyVec;
	std::vector < size_t > preserveVec;
	for (size_t i = 0; i < modifyVolSet.size(); ++i) {
		std::vector < size_t > res_vec;
		for (size_t j = 0; j < preserveVolSet.size(); ++j) {
			modifyVec = volumeSurfaceMap[modifyVolSet[i]];
			preserveVec = volumeSurfaceMap[preserveVolSet[j]];
			for (auto iter = modifyVec.begin(); iter != modifyVec.end(); ++iter) {
				if (std::find(preserveVec.begin(), preserveVec.end(), *iter) != preserveVec.end()) {
					res_vec.emplace_back(*iter);
				}
			}
		}
		modifyVolSurMap.insert(std::pair < size_t, std::vector < size_t > >(modifyVolSet[i], res_vec));
	}
	mesh_io::gmshWriter(m_outputFile, m_meshType/*, modifyVolSurMap*/);
	gmsh::finalize();
}

// In this function, meshsize(3D) = meshsize(2D) * Mesh.MeshSizeFactor
void Project::geoToMeshWith3DDensityImprove()
{
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	std::string tmpFile = "tmpfile.bdf";
	//gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < std::pair < int, int > > volumeDimTags;
	std::vector < std::pair < int, int > > allDimTags;

	gmsh::model::occ::synchronize();


	
	
	if (m_meshType == "Tet")
	{
		generate_mesh::generateMesh("Tri");
	}
	else if (m_meshType == "Hex")
	{
		generate_mesh::generateMesh("UnstructuredQuad");
	}
	else
	{
		return;
	}

	if (m_addMassCenter)
	{
		std::vector < double > centerCoors;
		gmsh::model::getEntities(volumeDimTags, -1);
		std::string tmpstr;
		for (auto& volumeTag : volumeDimTags)
		{
			
			
			
		}
		gmsh::model::getEntities(volumeDimTags, 3);
		for (auto& volumeTag : volumeDimTags)
		{


			double x,y,z;
			int tmpNewTag;

			gmsh::model::occ::getCenterOfMass(volumeTag.first, volumeTag.second, x, y, z);
			//tmpNewTag = gmsh::model::geo::addPoint(x, y, z);
			centerCoors.emplace_back(x);
			centerCoors.emplace_back(y);
			centerCoors.emplace_back(z);
		}
		std::ofstream centerTagsOut;
		centerTagsOut.open("centers.txt", std::ios::out);
		centerTagsOut << centerCoors.size() / 3 << std::endl;
		for (int i = 0; i < centerCoors.size(); i+=3)
		{
			centerTagsOut << centerCoors[i] << " " << centerCoors[i+1] << " " << centerCoors[i + 2] << std::endl;
		}
		centerTagsOut.close();
	}
	
	mesh_io::gmshWriter(tmpFile);
	gmsh::finalize();
	m_meshSizeMin *= m_meshSizeFactor;
	m_meshSizeMax *= m_meshSizeFactor;
	m_meshSizeFactor = 1;
	m_inputFile = tmpFile;

	if (!checkClosedMesh()) {
		std::cout << "Not a closed mesh" << std::endl;
		return;
	}
	if (m_addMassCenter)
		meshToMeshWithCenter();
	else
		meshToMesh();
}


void Project::geoToMeshTest()
{
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	std::string tmpFile = "tmpfile.bdf";
	//gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < std::pair < int, int > > volumeDimTags;
	std::vector < std::pair < int, int > > allDimTags;

	gmsh::model::occ::synchronize();




	if (m_meshType == "Tet")
	{
		generate_mesh::generateMesh("Tri");
	}
	else if (m_meshType == "Hex")
	{
		generate_mesh::generateMesh("UnstructuredQuad");
	}
	else
	{
		return;
	}



	mesh_io::gmshWriter(tmpFile);
	gmsh::finalize();
	m_meshSizeMin *= m_meshSizeFactor;
	m_meshSizeMax *= m_meshSizeFactor;
	m_meshSizeFactor = 1;
	m_inputFile = tmpFile;

	if (!checkClosedMesh()) {
		std::cout << "Not a closed mesh" << std::endl;
		return;
	}
	meshToMesh();
}

// In this function, meshsize(3D) = meshsize(2D) * Mesh.MeshSizeFactor
void Project::geoToMeshWith3DDensityImproveAndFragment()
{
	if (m_meshType != "Tet" && m_meshType != "Hex")
	{
		std::cout << "========ERROR========" << std::endl;
		std::cout << "Only Tet and Hex supported! \""<< std::endl;
		std::cout << "========ERROR========" << std::endl;
		return;
	}
	std::ifstream paramFin;
	std::ofstream bindingsFout;
	paramFin.open(m_meshParamsFile, std::ios::in);
	bindingsFout.open(m_bindingsOutFile, std::ios::out);
	if (!paramFin.is_open() || !bindingsFout.is_open())
	{
		std::cout << "========ERROR========" << std::endl;
		std::cout << "Cannot open file \"" << m_meshParamsFile << "\"or \"" << m_bindingsOutFile << "\"!" << std::endl;
		std::cout << "========ERROR========" << std::endl;
		return;
	}
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	std::string tmpFile = "tmpfile.bdf";
	//gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < std::pair < int, int > > surfaceDimTags;
	std::vector < std::pair < int, int > > testDimTags2;
	std::vector < std::pair < int, int > > pointDimTags;
	std::vector < std::pair < int, int > > outDimTags;
	std::vector < std::vector < std::pair < int, int > > > outDimTagsMap;
	std::vector<size_t> phygroupNodesTags;
	std::vector<double> phygroupCoorTags;
	std::vector<int> hardpointIDs;
	std::vector<double> hardpointCoors;
	int hardPointNum;

	paramFin >> hardPointNum;
	hardpointIDs.reserve(hardPointNum);
	hardpointCoors.reserve(hardPointNum * 3);
	for (int i = 0; i < hardPointNum; i++)
	{
		int tmpInputId;
		paramFin >> tmpInputId;
		hardpointIDs.emplace_back(tmpInputId);
	}
	for (int i = 0; i < hardPointNum * 3; i++)
	{
		double tmpDouble;
		paramFin >> tmpDouble;
		hardpointCoors.emplace_back(tmpDouble);
	}

	std::vector<int> pointIDsBeforeMesh, pointIDsAfterMesh;

	try
	{
		gmsh::model::getEntities(surfaceDimTags, -1);
		//gmsh::model::mesh::setSize(surfaceDimTags, m_meshSizeMax);

		for (int i = 0; i < hardPointNum * 3; i += 3)
		{
			int tmpID;
			tmpID = gmsh::model::occ::addPoint(hardpointCoors.at(i), hardpointCoors.at(i + 1), hardpointCoors.at(i + 2));

			pointDimTags.emplace_back(0, tmpID);
		}
		gmsh::model::occ::synchronize();
		gmsh::model::getEntities(surfaceDimTags, 3);
		gmsh::model::occ::fragment(surfaceDimTags, pointDimTags, outDimTags, outDimTagsMap);
		for (int i = surfaceDimTags.size(); i < outDimTagsMap.size(); i++)
		{
			int tmpID = -1;
			if (outDimTagsMap.at(i).size() > 0)
				tmpID = outDimTagsMap.at(i).at(0).second;
			pointIDsBeforeMesh.emplace_back(tmpID);
		}
		gmsh::model::occ::synchronize();
		for (int i = 0; i < hardPointNum; i++)
		{
			std::vector<int> tmpPointList;
			tmpPointList.emplace_back(pointIDsBeforeMesh.at(i));
			gmsh::model::addPhysicalGroup(0, tmpPointList, hardpointIDs.at(i));
		}
		gmsh::model::getEntities(surfaceDimTags, -1);
		gmsh::model::mesh::setSize(surfaceDimTags, m_meshSizeMax);
		gmsh::model::occ::synchronize();

		if (m_meshType == "Tet")
		{
			generate_mesh::generateMesh("Tri");
		}
		else if (m_meshType == "Hex")
		{
			generate_mesh::generateMesh("UnstructuredQuad");
		}

		gmsh::model::occ::synchronize();
		gmsh::model::getPhysicalGroups(testDimTags2, -1);

		for (auto& phyGroup : testDimTags2)
		{
			if (phyGroup.first != 0)
				continue;
			gmsh::model::mesh::getNodesForPhysicalGroup(0, phyGroup.second, phygroupNodesTags, phygroupCoorTags);
			std::cout << phyGroup.second << " " << phygroupNodesTags.at(0) << " " << phygroupCoorTags.at(0) << " " << phygroupCoorTags.at(1) << " " << phygroupCoorTags.at(2) << std::endl;
			bindingsFout << phyGroup.second << " " << phygroupNodesTags.at(0) << std::endl;
		}

		gmsh::model::occ::synchronize();

	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	paramFin.close();
	bindingsFout.close();
	mesh_io::gmshWriter(tmpFile);
	gmsh::finalize();
	m_meshSizeMin *= m_meshSizeFactor;
	m_meshSizeMax *= m_meshSizeFactor;
	m_meshSizeFactor = 1;
	m_inputFile = tmpFile;

	if (!checkClosedMesh()) {
		std::cout << "Not a closed mesh" << std::endl;
		return;
	}
	meshToMeshWithIdMap();
}

void Project::geoToMeshWithEmbeddings()
{
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < std::pair < int, int > > volumeDimTags;
	std::vector < std::pair < int, int > > allDimTags;

	std::vector<int> hardpointIDs;
	std::vector<double> hardpointCoors;
	std::vector<double> hardpointLC;
	std::vector<int> hardpointOutIDs;

	int hardPointNum;

	std::ifstream paramFin;
	std::ofstream bindingsFout;
	paramFin.open(m_meshParamsFile, std::ios::in);
	bindingsFout.open(m_bindingsOutFile, std::ios::out);
	if (!paramFin.is_open() || !bindingsFout.is_open())
	{
		std::cout << "========ERROR========" << std::endl;
		std::cout << "Cannot open file \"" << m_meshParamsFile << "\"or \"" << m_bindingsOutFile << "\"!" << std::endl;
		std::cout << "========ERROR========" << std::endl;
		return;
	}

	paramFin >> hardPointNum;
	hardpointIDs.reserve(hardPointNum);
	hardpointCoors.reserve(hardPointNum * 3);
	for (int i = 0; i < hardPointNum; i++)
	{
		int tmpInputId;
		paramFin >> tmpInputId;
		hardpointIDs.emplace_back(tmpInputId);
	}
	for (int i = 0; i < hardPointNum * 3; i++)
	{
		double tmpDouble;
		paramFin >> tmpDouble;
		hardpointCoors.emplace_back(tmpDouble);
	}
	
	gmsh::model::getEntities(allDimTags);
	gmsh::model::getEntities(volumeDimTags, 3);
	
	if (volumeDimTags.size() > 1)
	{
		std::cout << "========ERROR========" << std::endl;
		std::cout << "More than one body, not supported for \"GeometryWithEmbeddings\" now."<< std::endl;
		std::cout << "========ERROR========" << std::endl;
	}

	std::vector<int> embeddingTags;
	for (int i = 0; i < hardPointNum; i++)
	{
		int tmpNewTag;
		tmpNewTag = gmsh::model::occ::addPoint(hardpointCoors[i * 3], hardpointCoors[i * 3 + 1], hardpointCoors[i * 3 + 2]);
		embeddingTags.emplace_back(tmpNewTag);
	}
	
	for (int i = 0; i < hardPointNum; i++)
	{
		bindingsFout << hardpointIDs[i] << " " << embeddingTags[i] << std::endl;
	}

	gmsh::model::occ::synchronize();
	gmsh::model::mesh::embed(0, embeddingTags, 3, volumeDimTags[0].second);

	gmsh::model::occ::synchronize();
	generate_mesh::generateMesh(m_meshType);

	mesh_io::gmshWriter(m_outputFile);
	gmsh::finalize();

	paramFin.close();
	bindingsFout.close();
}

void Project::geoToMeshFace()
{
	std::ifstream paramFin;
	std::ofstream bindingsFout;
	paramFin.open(m_meshParamsFile, std::ios::in);
	bindingsFout.open(m_bindingsOutFile, std::ios::out);
	if (!paramFin.is_open() || !bindingsFout.is_open())
	{
		std::cout << "========ERROR========" << std::endl;
		std::cout << "Cannot open file \"" << m_meshParamsFile << "\"or \"" << m_bindingsOutFile << "\"!" << std::endl;
		std::cout << "========ERROR========" << std::endl;
		return;
	}
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	//gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	//gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	//gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < std::pair < int, int > > surfaceDimTags;
	std::vector < std::pair < int, int > > testDimTags2;
	std::vector < std::pair < int, int > > pointDimTags;
	std::vector < std::pair < int, int > > outDimTags;
	std::vector < std::vector < std::pair < int, int > > > outDimTagsMap;
	std::vector<size_t> phygroupNodesTags;
	std::vector<double> phygroupCoorTags;
	std::vector<int> hardpointIDs;
	std::vector<double> hardpointCoors;
	std::vector<double> hardpointLC;
	int hardPointNum;

	paramFin >> hardPointNum;
	hardpointIDs.reserve(hardPointNum);
	hardpointLC.reserve(hardPointNum);
	hardpointCoors.reserve(hardPointNum * 3);
	for (int i = 0; i < hardPointNum; i++)
	{
		int tmpInputId;
		paramFin >> tmpInputId;
		hardpointIDs.emplace_back(tmpInputId);
	}
	for (int i = 0; i < hardPointNum * 3; i++)
	{
		double tmpDouble;
		paramFin >> tmpDouble;
		hardpointCoors.emplace_back(tmpDouble);
	}
	for (int i = 0; i < hardPointNum; i++)
	{
		double tmpInputLC;
		paramFin >> tmpInputLC;
		hardpointLC.emplace_back(tmpInputLC);
	}

	std::vector<int> pointIDsBeforeMesh, pointIDsAfterMesh;

	try
	{
		gmsh::model::getEntities(surfaceDimTags, -1);
		//gmsh::model::mesh::setSize(surfaceDimTags, m_meshSizeMax);
		
		for (int i = 0; i < hardPointNum * 3; i+= 3)
		{
			int tmpID;
			tmpID = gmsh::model::occ::addPoint(hardpointCoors.at(i), hardpointCoors.at(i+1), hardpointCoors.at(i+2), hardpointLC.at(i/3));

			pointDimTags.emplace_back(0, tmpID);
		}
		gmsh::model::occ::synchronize();
		gmsh::model::getEntities(surfaceDimTags, -1);
		gmsh::model::occ::fragment(surfaceDimTags, pointDimTags, outDimTags, outDimTagsMap);
		for (int i = surfaceDimTags.size(); i < outDimTagsMap.size(); i++)
		{
			int tmpID = -1;
			if (outDimTagsMap.at(i).size() > 0)
				tmpID = outDimTagsMap.at(i).at(0).second;
			pointIDsBeforeMesh.emplace_back(tmpID);
		}
		gmsh::model::occ::synchronize();
		for (int i = 0; i < hardPointNum; i++)
		{
			std::vector<int> tmpPointList;
			tmpPointList.emplace_back(pointIDsBeforeMesh.at(i));
			gmsh::model::addPhysicalGroup(0, tmpPointList, hardpointIDs.at(i));
		}
		gmsh::model::getEntities(surfaceDimTags, -1);
		gmsh::model::mesh::setSize(surfaceDimTags, m_meshSizeMax);
		gmsh::model::occ::synchronize();
		generate_mesh::generateMesh(m_meshType);

		gmsh::model::occ::synchronize();
		gmsh::model::getPhysicalGroups(testDimTags2, -1);

		for (auto& phyGroup : testDimTags2)
		{
			if (phyGroup.first != 0)
				continue;
			gmsh::model::mesh::getNodesForPhysicalGroup(0, phyGroup.second, phygroupNodesTags, phygroupCoorTags);
			std::cout << phyGroup.second << " " << phygroupNodesTags.at(0) << " " << phygroupCoorTags.at(0) << " " << phygroupCoorTags.at(1) << " " << phygroupCoorTags.at(2) << std::endl;
			bindingsFout << phyGroup.second << " " << phygroupNodesTags.at(0) << std::endl;
		}
		
		gmsh::model::occ::synchronize();

	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	paramFin.close();
	bindingsFout.close();
	mesh_io::gmshWriter_Face(m_outputFile, m_meshType);
	gmsh::finalize();
}

void Project::geoToMeshWithFragments()
{
	std::ifstream paramFin;
	std::ofstream bindingsFout;
	paramFin.open(m_meshParamsFile, std::ios::in);
	bindingsFout.open(m_bindingsOutFile, std::ios::out);
	if (!paramFin.is_open() || !bindingsFout.is_open())
	{
		std::cout << "========ERROR========" << std::endl;
		std::cout << "Cannot open file \"" << m_meshParamsFile << "\"or \"" << m_bindingsOutFile << "\"!" << std::endl;
		std::cout << "========ERROR========" << std::endl;
		return;
	}
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < std::pair < int, int > > surfaceDimTags;
	std::vector < std::pair < int, int > > testDimTags2;
	std::vector < std::pair < int, int > > pointDimTags;
	std::vector < std::pair < int, int > > outDimTags;
	std::vector < std::vector < std::pair < int, int > > > outDimTagsMap;
	std::vector<size_t> phygroupNodesTags;
	std::vector<double> phygroupCoorTags;
	std::vector<int> hardpointIDs;
	std::vector<double> hardpointCoors;
	int hardPointNum;

	paramFin >> hardPointNum;
	hardpointIDs.reserve(hardPointNum);
	hardpointCoors.reserve(hardPointNum * 3);
	for (int i = 0; i < hardPointNum; i++)
	{
		int tmpInputId;
		paramFin >> tmpInputId;
		hardpointIDs.emplace_back(tmpInputId);
	}
	for (int i = 0; i < hardPointNum * 3; i++)
	{
		double tmpDouble;
		paramFin >> tmpDouble;
		hardpointCoors.emplace_back(tmpDouble);
	}

	std::vector<int> pointIDsBeforeMesh, pointIDsAfterMesh;

	try
	{
		gmsh::model::getEntities(surfaceDimTags, -1);
		//gmsh::model::mesh::setSize(surfaceDimTags, m_meshSizeMax);

		for (int i = 0; i < hardPointNum * 3; i += 3)
		{
			int tmpID;
			tmpID = gmsh::model::occ::addPoint(hardpointCoors.at(i), hardpointCoors.at(i + 1), hardpointCoors.at(i + 2));

			pointDimTags.emplace_back(0, tmpID);
		}
		gmsh::model::occ::synchronize();
		gmsh::model::getEntities(surfaceDimTags, 3);
		gmsh::model::occ::fragment(surfaceDimTags, pointDimTags, outDimTags, outDimTagsMap);
		for (int i = surfaceDimTags.size(); i < outDimTagsMap.size(); i++)
		{
			int tmpID = -1;
			if (outDimTagsMap.at(i).size() > 0)
				tmpID = outDimTagsMap.at(i).at(0).second;
			pointIDsBeforeMesh.emplace_back(tmpID);
		}
		gmsh::model::occ::synchronize();
		for (int i = 0; i < hardPointNum; i++)
		{
			std::vector<int> tmpPointList;
			tmpPointList.emplace_back(pointIDsBeforeMesh.at(i));
			gmsh::model::addPhysicalGroup(0, tmpPointList, hardpointIDs.at(i));
		}
		gmsh::model::getEntities(surfaceDimTags, -1);
		gmsh::model::mesh::setSize(surfaceDimTags, m_meshSizeMax);
		gmsh::model::occ::synchronize();
		generate_mesh::generateMesh(m_meshType);

		gmsh::model::occ::synchronize();
		gmsh::model::getPhysicalGroups(testDimTags2, -1);

		for (auto& phyGroup : testDimTags2)
		{
			if (phyGroup.first != 0)
				continue;
			gmsh::model::mesh::getNodesForPhysicalGroup(0, phyGroup.second, phygroupNodesTags, phygroupCoorTags);
			std::cout << phyGroup.second << " " << phygroupNodesTags.at(0) << " " << phygroupCoorTags.at(0) << " " << phygroupCoorTags.at(1) << " " << phygroupCoorTags.at(2) << std::endl;
			bindingsFout << phyGroup.second << " " << phygroupNodesTags.at(0) << std::endl;
		}

		gmsh::model::occ::synchronize();

	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	paramFin.close();
	bindingsFout.close();
	mesh_io::gmshWriter(m_outputFile);
	gmsh::finalize();
}



void Project::meshToMesh()
{
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < int > surfaceTags;
	std::vector < std::pair < int, int > > volumeDimTags(0);
	gmsh::model::getEntities(volumeDimTags, 3);
	//直接利用模型文件自带体信息
	if (volumeDimTags.size() > 0) {
		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			gmsh::model::mesh::setOutwardOrientation(volumeDimTags[i].second);
		}
		gmsh::model::geo::synchronize();
		gmsh::model::occ::synchronize();
		generate_mesh::generateMesh(m_meshType);
		mesh_io::gmshWriter(m_outputFile);
	}
	//若输入为bdf，则需要构造体信息
	else {
		std::vector < std::pair < int, int > > surfaceDimTags;
		gmsh::model::getEntities(surfaceDimTags, 2);
		for (size_t i = 0; i < surfaceDimTags.size(); ++i) {
			surfaceTags.emplace_back(surfaceDimTags[i].second);
		}
		int s11 = gmsh::model::geo::addSurfaceLoop(surfaceTags);
		int v1 = gmsh::model::geo::addVolume({ s11 });
		gmsh::model::geo::synchronize();
		gmsh::model::occ::synchronize();
		gmsh::model::mesh::setOutwardOrientation(v1);
		gmsh::model::geo::synchronize();
		gmsh::model::occ::synchronize();
		generate_mesh::generateMesh(m_meshType);
		mesh_io::gmshWriter(m_outputFile);
	}
	gmsh::finalize();
}

void Project::meshToMeshWithCenter()
{
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < int > surfaceTags;
	std::vector < std::pair < int, int > > volumeDimTags(0);
	gmsh::model::getEntities(volumeDimTags, 3);
	//直接利用模型文件自带体信息
	if (volumeDimTags.size() > 0) {
		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			gmsh::model::mesh::setOutwardOrientation(volumeDimTags[i].second);
		}
		gmsh::model::geo::synchronize();
		gmsh::model::occ::synchronize();
		generate_mesh::generateMesh(m_meshType);
		mesh_io::gmshWriter(m_outputFile);
	}
	//若输入为bdf，则需要构造体信息
	else {
		std::vector < std::pair < int, int > > surfaceDimTags;
		gmsh::model::getEntities(surfaceDimTags, 2);
		for (size_t i = 0; i < surfaceDimTags.size(); ++i) {
			surfaceTags.emplace_back(surfaceDimTags[i].second);
		}
		int s11 = gmsh::model::geo::addSurfaceLoop(surfaceTags);
		int v1 = gmsh::model::geo::addVolume({ s11 });
		gmsh::model::geo::synchronize();
		gmsh::model::occ::synchronize();
		gmsh::model::mesh::setOutwardOrientation(v1);
		gmsh::model::geo::synchronize();
		gmsh::model::occ::synchronize();

		std::vector < int > centerTags;
		if (m_addMassCenter)
		{
			std::ifstream centersFin;
			int centerNum;
			centersFin.open("centers.txt", std::ios::in);
			centersFin >> centerNum;

			gmsh::model::getEntities(volumeDimTags, 3);
			for (auto& volumeTag : volumeDimTags)
			{
				std::vector < int > tmpCenterTags;
				double x ,y,z;
				centersFin >> x;
				centersFin >> y;
				centersFin >> z;
				int tmpNewTag;
				
				//gmsh::model::occ::getCenterOfMass(volumeTag.first, volumeTag.second, x, y, z);
				tmpNewTag = gmsh::model::geo::addPoint(x, y, z);
				tmpCenterTags.emplace_back(tmpNewTag);
				centerTags.emplace_back(tmpNewTag);
				gmsh::model::geo::synchronize();
				gmsh::model::mesh::embed(0, tmpCenterTags, volumeTag.first, volumeTag.second);
			}

			std::ofstream centerTagsOut;
			centerTagsOut.open("centers.txt", std::ios::out);
			for (int i = 0; i < centerTags.size(); i++)
			{
				centerTagsOut << volumeDimTags[i].second << " " << centerTags[i] << std::endl;
			}
		}

		generate_mesh::generateMesh(m_meshType);
		mesh_io::gmshWriter(m_outputFile);
	}
	gmsh::finalize();
}

void Project::meshToMeshWithIdMap()
{
	std::ifstream bindingsFin;
	bindingsFin.open(m_bindingsOutFile, std::ios::in);
	if (!bindingsFin.is_open())
	{
		std::cout << "========ERROR========" << std::endl;
		std::cout << "Cannot open file \"" << m_meshParamsFile << "\"or \"" << m_bindingsOutFile << "\"!" << std::endl;
		std::cout << "========ERROR========" << std::endl;
		return;
	}
	
	std::vector<int> ori_hardpointsid;
	std::vector<int> tmpbdf_hardpointsid;
	char buf[2];
	while (!bindingsFin.eof())
	{
		int a, b;
		bindingsFin >> a;
		bindingsFin >> b;
		ori_hardpointsid.push_back(a);
		tmpbdf_hardpointsid.push_back(b);
	}

	bindingsFin.close();

	std::ofstream bindingsFout;
	bindingsFout.open(m_bindingsOutFile, std::ios::out);


	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);
	gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	gmsh::option::setNumber("Mesh.SaveAll", 1);
	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < int > surfaceTags;
	std::vector < std::pair < int, int > > volumeDimTags(0);
	std::vector < std::pair < int, int > > testDimTags2;
	std::vector<size_t> phygroupNodesTags;
	std::vector<double> phygroupCoorTags;
	gmsh::model::getEntities(volumeDimTags, 3);
	gmsh::model::getEntities(testDimTags2, -1);
	//直接利用模型文件自带体信息
	if (volumeDimTags.size() > 0) {
		for (size_t i = 0; i < volumeDimTags.size(); ++i) {
			gmsh::model::mesh::setOutwardOrientation(volumeDimTags[i].second);
		}
		gmsh::model::geo::synchronize();
		gmsh::model::occ::synchronize();
		generate_mesh::generateMesh(m_meshType);
		mesh_io::gmshWriter(m_outputFile);
	}
	//若输入为bdf，则需要构造体信息
	else {
		std::vector < std::pair < int, int > > surfaceDimTags;
		gmsh::model::getEntities(surfaceDimTags, 2);
		for (size_t i = 0; i < surfaceDimTags.size(); ++i) {
			surfaceTags.emplace_back(surfaceDimTags[i].second);
		}
		int s11 = gmsh::model::geo::addSurfaceLoop(surfaceTags);
		int v1 = gmsh::model::geo::addVolume({ s11 });
		gmsh::model::geo::synchronize();
		gmsh::model::occ::synchronize();
		gmsh::model::mesh::setOutwardOrientation(v1);
		
	}
	gmsh::model::getPhysicalGroups(testDimTags2, -1);
	gmsh::model::getEntities(testDimTags2, -1);
	for (int i = 0; i < ori_hardpointsid.size() - 1; i++)
	{
		std::vector<int> tmpPointList;
		tmpPointList.emplace_back(tmpbdf_hardpointsid.at(i));
		gmsh::model::addPhysicalGroup(0, tmpPointList, ori_hardpointsid.at(i));
	}
	gmsh::model::getPhysicalGroups(testDimTags2, -1);
	gmsh::model::geo::synchronize();
	gmsh::model::occ::synchronize();
	generate_mesh::generateMesh(m_meshType);
	gmsh::model::occ::synchronize();
	gmsh::model::getPhysicalGroups(testDimTags2, -1);

	for (auto& phyGroup : testDimTags2)
	{
		if (phyGroup.first != 0)
			continue;
		gmsh::model::mesh::getNodesForPhysicalGroup(0, phyGroup.second, phygroupNodesTags, phygroupCoorTags);
		std::cout << phyGroup.second << " " << phygroupNodesTags.at(0) << " " << phygroupCoorTags.at(0) << " " << phygroupCoorTags.at(1) << " " << phygroupCoorTags.at(2) << std::endl;
		bindingsFout << phyGroup.second << " " << phygroupNodesTags.at(0) << std::endl;
	}

	mesh_io::gmshWriter(m_outputFile);
	bindingsFout.close();
	gmsh::finalize();
}

void Project::meshRemeshToMesh()
{
	gmsh::initialize();
	gmsh::model::add("object");
	mesh_io::gmshReader(m_inputFile);
	//gmsh::merge(m_inputFile);
	gmsh::option::setNumber("Mesh.Algorithm", 6);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);

	std::vector<double> n;
	gmsh::onelab::getNumber("Parameters for surface detection", n);
	//double angle = n[0];
	//mesh_io::gmshReader(m_inputFile);
	//gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);


	//gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	//gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	//gmsh::option::setNumber("Mesh.SaveAll", 1);
	//gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < int > surfaceTags;
	std::vector < std::pair < int, int > > volumeDimTags(0);
	//gmsh::model::getEntities(volumeDimTags, 3);

	gmsh::model::mesh::classifySurfaces(10 * M_PI / 180.0, true, false, 180 * M_PI / 180.);

	gmsh::model::mesh::createGeometry();
	std::vector < std::pair < int, int > > surfaceDimTags;
	gmsh::model::getEntities(surfaceDimTags, 2);
	for (size_t i = 0; i < surfaceDimTags.size(); ++i) {
		surfaceTags.emplace_back(surfaceDimTags[i].second);
	}
	int s11 = gmsh::model::geo::addSurfaceLoop(surfaceTags);
	int v1 = gmsh::model::geo::addVolume({ s11 });
	gmsh::model::geo::synchronize();
	gmsh::model::occ::synchronize();
	gmsh::model::mesh::setOutwardOrientation(v1);
	gmsh::model::geo::synchronize();
	gmsh::model::occ::synchronize();
	generate_mesh::generateMesh(m_meshType);
	mesh_io::gmshWriter(m_outputFile);

	gmsh::finalize();
}

void Project::meshRemeshToMesh2D()
{
	gmsh::initialize();
	//mesh_io::gmshReader(m_inputFile);
	gmsh::merge(m_inputFile);
	gmsh::option::setNumber("Mesh.Algorithm", 6);
	gmsh::option::setNumber("Mesh.MeshSizeMin", m_meshSizeMin);
	gmsh::option::setNumber("Mesh.MeshSizeMax", m_meshSizeMax);

	//double angle = n[0];
	//mesh_io::gmshReader(m_inputFile);
	//gmsh::option::setNumber("Mesh.MeshSizeFactor", m_meshSizeFactor);


	//gmsh::option::setNumber("Mesh.OptimizeThreshold", m_optThreshold);
	//gmsh::option::setNumber("Mesh.Smoothing", m_smoothStep);
	//gmsh::option::setNumber("Mesh.SaveAll", 1);
	//gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);
	std::vector < int > surfaceTags;
	std::vector < std::pair < int, int > > surfaceDimTags(0);
	std::vector < std::pair < int, int > > pointDimTags(0);
	std::vector < std::pair < int, int > > outDimTags;
	std::vector < std::vector < std::pair < int, int > > > outDimTagsMap;
	gmsh::vectorpair dimTags;
	

	gmsh::model::mesh::classifySurfaces(45 * M_PI / 180.0, true, true, 30 * M_PI / 180.0);
	//gmsh::model::mesh::reclassifyNodes();
	//gmsh::model::geo::synchronize();
	//gmsh::model::occ::synchronize();
	
	try
	{
		gmsh::model::mesh::createGeometry();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	gmsh::model::geo::synchronize();
	gmsh::model::occ::synchronize();

	generate_mesh::generateMesh(m_meshType);
	mesh_io::gmshWriter(m_outputFile);

	gmsh::finalize();
}

int Project::checkClosedMesh()
{
	gmsh::initialize();
	mesh_io::gmshReader(m_inputFile);
	std::map < std::vector < size_t >, size_t > edgeMap;
	std::vector < int > elemTypes;
	std::vector < std::vector <size_t > > elemTags;
	std::vector < std::vector < size_t > > elemNodeTags;
	gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags, 2);
	std::vector < size_t > e0(2);
	std::vector < size_t > e1(2);
	std::vector < size_t > e2(2);
	std::vector < size_t > e3(2);
	// tri
	if (elemTypes[0] == 2) {
		for (size_t i = 0; i < elemNodeTags[0].size(); i += 3) {
			e0[0] = elemNodeTags[0][i]; e0[1] = elemNodeTags[0][i + 1];
			e1[0] = elemNodeTags[0][i + 1]; e1[1] = elemNodeTags[0][i + 2];
			e2[0] = elemNodeTags[0][i + 2]; e2[1] = elemNodeTags[0][i];
			std::sort(e0.begin(), e0.end());
			std::sort(e1.begin(), e1.end());
			std::sort(e2.begin(), e2.end());
			++edgeMap[e0];
			++edgeMap[e1];
			++edgeMap[e2];
		}
	}
	// quad
	if (elemTypes[0] == 3) {
		for (size_t i = 0; i < elemNodeTags[0].size(); i += 4) {
			e0[0] = elemNodeTags[0][i]; e0[1] = elemNodeTags[0][i + 1];
			e1[0] = elemNodeTags[0][i + 1]; e1[1] = elemNodeTags[0][i + 2];
			e2[0] = elemNodeTags[0][i + 2]; e2[1] = elemNodeTags[0][i + 3];
			e3[0] = elemNodeTags[0][i + 3]; e3[0] = elemNodeTags[0][i];
			std::sort(e0.begin(), e0.end());
			std::sort(e1.begin(), e1.end());
			std::sort(e2.begin(), e2.end());
			std::sort(e3.begin(), e3.end());
			++edgeMap[e0];
			++edgeMap[e1];
			++edgeMap[e2];
			++edgeMap[e3];
		}
	}
	std::map < std::vector < size_t >, size_t >::iterator it = edgeMap.begin();
	while (it != edgeMap.end()) {
		if (it->second != 2) return 0;
		++it;
	}
	gmsh::finalize();
	return 1;
}

int Project::start()
{
	if (m_inputType == "Geometry") {
		geoToMesh();
	}
	else if (m_inputType == "geoToMesh_ST") {
		geoToMesh_ST_test();
	}
	else if (m_inputType == "GeometryWith3DDensityImprove") {
		geoToMeshWith3DDensityImprove();
	}
	else if (m_inputType == "GeometryWith3DDensityImproveAndCenter") {
		m_addMassCenter = true;
		geoToMeshWith3DDensityImprove();
	}
	else if (m_inputType == "GeometryWith3DDensityImproveAndFragment") {
		if (m_meshParamsFile.size() < 1 || m_bindingsOutFile.size() < 1)
		{
			std::cout << "========ERROR========" << std::endl;
			std::cout << "GeometryWithEmbeddings NEEDS param and bindings file!" << std::endl;
			std::cout << "========ERROR========" << std::endl;
			return 0;
		}
		geoToMeshWith3DDensityImproveAndFragment();
	}
	else if (m_inputType == "GeometryWithCenter") {
		m_addMassCenter = true;
		geoToMesh();
	}
	else if (m_inputType == "GeometryWithEmbeddings") {
		if (m_meshParamsFile.size() < 1 || m_bindingsOutFile.size() < 1)
		{
			std::cout << "========ERROR========" << std::endl;
			std::cout << "GeometryWithEmbeddings NEEDS param and bindings file!" << std::endl;
			std::cout << "========ERROR========" << std::endl;
			return 0;
		}
		geoToMeshWithEmbeddings();
	}
	else if (m_inputType == "GeometryWithFragments") {
		if (m_meshParamsFile.size() < 1 || m_bindingsOutFile.size() < 1)
		{
			std::cout << "========ERROR========" << std::endl;
			std::cout << "GeometryWithEmbeddings NEEDS param and bindings file!" << std::endl;
			std::cout << "========ERROR========" << std::endl;
			return 0;
		}
		geoToMeshWithFragments();
	}
	else if (m_inputType == "GeometryFace") {
		if (m_meshParamsFile.size() < 1 || m_bindingsOutFile.size() < 1)
		{
			std::cout << "========ERROR========" << std::endl;
			std::cout << "GeometryFace NEEDS param and bindings file!" << std::endl;
			std::cout << "========ERROR========" << std::endl;
			return 0;
		}
		geoToMeshFace();
	}
	else if (m_inputType == "Mesh") {
		if (!checkClosedMesh()) {
			std::cout << "Not a closed mesh" << std::endl;
			return 0;
		}
		meshToMesh();
	}
	else if (m_inputType == "ReMesh2D") {
		
		meshRemeshToMesh2D();
	}
	else if (m_inputType == "test1") {

		geoToMeshTest();
	}
	else {
		std::cout << "Unsupport input type" << std::endl;
		return 0;
	}
	return 1;
}