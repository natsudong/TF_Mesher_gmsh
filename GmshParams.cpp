#include "Params.h"
#include "GmshParams.h"
#include "gmsh.h"
#include "src/mesh_io/mesh_io.h"
#include <fstream>

void GmshGlobalSettings::performParam() 
{
	if (minSize > 0 || maxSize > 0)
	{
		gmsh::option::setNumber("Mesh.MeshSizeMin", minSize);
		gmsh::option::setNumber("Mesh.MeshSizeMax", maxSize);
		gmsh::option::setNumber("Mesh.MeshSizeFromCurvature", 100);
		gmsh::option::setNumber("Mesh.SmoothCrossField", 100);
	}
	//switch (meshElemType)
	//{
	//case MeshingElemType::Tri:
	//	break;
	//case MeshingElemType::Quad:
	//	gmsh::option::setNumber("Mesh.Algorithm", 11);
	//	break;
	//case MeshingElemType::Tet:
	//	break;
	//case MeshingElemType::Hex:
	//	gmsh::option::setNumber("Mesh.SubdivisionAlgorithm", 2);
	//	break;
	//default:
	//	break;
	//}
}

void GmshGlobalSettings::collectInfo(std::ostream& os)
{
}

//REMINDER: 检查physicalgroup是否会与其他功能的group重复
void GmshHardPoint::performParam()
{
	int tmpID;
	tmpID = gmsh::model::occ::addPoint(x, y, z);
	gmsh::model::occ::synchronize();



	std::vector < std::pair < int, int > > surfaceDimTags, pointDimTags;
	std::vector < std::pair < int, int > > outDimTags;
	std::vector < std::vector < std::pair < int, int > > > outDimTagsMap;
	gmsh::model::getEntities(surfaceDimTags, 3);
	gmsh::model::getEntities(pointDimTags, 0);
	pointDimTags.clear();
	pointDimTags.emplace_back(0, tmpID);
	gmsh::model::occ::fragment( surfaceDimTags, pointDimTags, outDimTags, outDimTagsMap, -1, true, true);
	gmsh::model::occ::synchronize();
	std::vector<int> tmpPointList;
	tmpPointList.emplace_back(tmpID);
	gmsh::model::addPhysicalGroup(0, tmpPointList, point_id);
	gmsh::model::occ::synchronize();
	gmsh::model::getEntities(pointDimTags, 0);
}

void GmshHardPoint::collectInfo(std::ostream& os)
{	
	std::vector<size_t> resultPointList;
	std::vector<double> resultPointCoor;

	gmsh::model::mesh::getNodesForPhysicalGroup(0, point_id, resultPointList, resultPointCoor);
	
	os << "HardPointMap" << " " << point_id << " " << resultPointList[0] << std::endl;
}

void GmshMeshToMeshFlag::performParam()
{
	if (preformed) return;
	std::vector < int > surfaceTags;
	std::vector < std::pair < int, int > > volumeDimTags(0);
	gmsh::model::getEntities(volumeDimTags, 3);

	bool forceParametrizablePatches = false;
	double angle = 45;
	bool includeBoundary = true;
	double curveAngle = 180;
	gmsh::model::mesh::classifySurfaces(angle * M_PI / 180., includeBoundary,
		forceParametrizablePatches,
		curveAngle * M_PI / 180.);
	gmsh::model::mesh::createGeometry();
	std::vector<std::pair<int, int> > s;
	gmsh::model::getEntities(s, 2);
	std::vector<int> sl;
	for (auto surf : s) sl.push_back(surf.second);
	int l = gmsh::model::geo::addSurfaceLoop(sl);
	gmsh::model::geo::addVolume({ l });
	gmsh::model::geo::synchronize();
	preformed = true;
}

void GmshMeshToMeshFlag::collectInfo(std::ostream& os)
{

}

void GmshDistanceField::performParam()
{
	int thisId = getId();
	gmsh::model::mesh::field::add("Distance", thisId);
	gmsh::model::mesh::field::setNumbers(thisId, "PointsList", m_nodeList );
	gmsh::model::mesh::field::setNumbers(thisId, "CurvesList", m_lineList);
	gmsh::model::mesh::field::setNumbers(thisId, "SurfacesList", m_surfaceList);
	gmsh::model::mesh::field::setNumber(thisId, "Sampling", 100);
	/// <summary>
	/// background mesh change!!!!!!!!!!!!
	/// </summary>
	gmsh::model::mesh::field::setAsBackgroundMesh(thisId);
}

void GmshDistanceField::collectInfo(std::ostream& os)
{
}

void GmshThresholdField::performParam()
{
	int thisId = getId();
	gmsh::model::mesh::field::add("Threshold", thisId);
	gmsh::model::mesh::field::setNumber(thisId, "InField", m_inField);
	gmsh::model::mesh::field::setNumber(thisId, "SizeMin", m_sizeMin);
	gmsh::model::mesh::field::setNumber(thisId, "SizeMax", m_sizeMax);
	gmsh::model::mesh::field::setNumber(thisId, "DistMin", m_distMin);
	gmsh::model::mesh::field::setNumber(thisId, "DistMax", m_distMax);
	/// <summary>
	/// background mesh change!!!!!!!!!!!!
	/// </summary>
	gmsh::model::mesh::field::setAsBackgroundMesh(thisId);
}

void GmshThresholdField::collectInfo(std::ostream& os)
{
}

void GmshMeshingMethod::performParam()
{
}

void GmshMeshingMethod::collectInfo(std::ostream& os)
{
}

void GmshOctTreeField::performParam()
{
	int thisId = getId();
	gmsh::model::mesh::field::add("Octree", thisId);
	gmsh::model::mesh::field::setNumber(thisId, "InField", m_inField);
	gmsh::model::mesh::field::setAsBackgroundMesh(thisId);
}

void GmshOctTreeField::collectInfo(std::ostream& os)
{

}


void GmshMesher::checkParamsAvail()
{
}

void GmshMesher::performParams()
{
	gmsh::initialize();
	gmsh::clear();
	gmsh::merge(inFilePath);
	readParam();

	std::list<PMeshToMeshFlag> mtmfList;
	mtmfList = getParamsByClass<MeshToMeshFlag>();
	if (mtmfList.size() > 0)mtmfList.front()->performParam();

	for (PParamBase pb : m_params)
	{
		pb->performParam();
	}
	//applyHardpoints()
	//return;
	std::vector < std::pair < int, int > > volumeDimTags, pointDimTags, surfaceDimTags;
	std::vector < std::pair < int, int > > outDimTags;
	std::vector < std::vector < std::pair < int, int > > > outDimTagsMap;
	gmsh::model::occ::synchronize();
	gmsh::model::getEntities(volumeDimTags, 3);
	gmsh::model::getEntities(surfaceDimTags, 2);
	gmsh::model::getEntities(pointDimTags, 0);
	//gmsh::model::occ::fragment(  surfaceDimTags, pointDimTags, outDimTags, outDimTagsMap, -1, true, true);
	//gmsh::model::occ::synchronize();
	mapFieldsByModifyMap();

	std::vector<int> fieldTags;
	std::vector<double> thresholdFieldTags;
	gmsh::model::mesh::field::list(fieldTags);
	for (auto& tag : fieldTags)
	{
		std::string type;
		gmsh::model::mesh::field::getType(tag, type);
		if (type == "Threshold")
		{
			thresholdFieldTags.emplace_back(tag);
		}
	}
	int minFieldTag = gmsh::model::mesh::field::add("Min");
	gmsh::model::mesh::field::setNumbers(minFieldTag, "FieldsList", thresholdFieldTags);
	gmsh::model::mesh::field::setAsBackgroundMesh(minFieldTag);

}

void GmshMesher::collectInfo(std::string filepath)
{
	std::ofstream outfile;
	if (filepath.empty())
		outfile.open(postInfoFilePath);
	else
		outfile.open(filepath);

	for (PParamBase pb : m_params)
	{
		pb->collectInfo(outfile);
	}
	gmsh::option::setNumber("Mesh.SaveAll", 1);

	gmsh::option::setNumber("Mesh.BdfFieldFormat", 2);

	std::list<PGmshMeshingMethod> tmplist = getParamsByClass<GmshMeshingMethod>();
	if (tmplist.empty())
	{
		mesh_io::gmshWriter(outFilePath, "Tet");
		return;
	}
	PGmshMeshingMethod pgmm = tmplist.front();
	switch (pgmm->meshElemType)
	{
	case MeshingElemType::Tri:
		mesh_io::gmshWriter(outFilePath, "Tri");
		break;
	case MeshingElemType::Quad:
		mesh_io::gmshWriter(outFilePath, "StructuredQuad");
		break;
	case MeshingElemType::Quad_Mixed:
		mesh_io::gmshWriter(outFilePath);
		break;
	case MeshingElemType::Tet:
		mesh_io::gmshWriter(outFilePath, "Tet");
		break;
	case MeshingElemType::Hex:
	case MeshingElemType::Hex_Structural:
		//gmsh::write(outFilePath);
		mesh_io::gmshWriter(outFilePath, "Hex");
		break;
	default:
		break;
	}
	//gmsh::write(outFilePath);
}

void GmshMesher::performMesh()
{
	gmsh::model::occ::synchronize();

	std::list<PGmshMeshingMethod> tmplist = getParamsByClass<GmshMeshingMethod>();
	if (tmplist.empty())
	{
		gmsh::option::setNumber("Mesh.Algorithm", 1);
		gmsh::option::setNumber("Mesh.Algorithm3D", 1);
		gmsh::model::mesh::generate(3);
		return;
	}
	//gmsh::option::setNumber("Mesh.SaveAll", 1);
	//MeshSizeFromCurvature
	PGmshMeshingMethod pgmms = tmplist.front();
	switch (pgmms->meshElemType)
	{
	case MeshingElemType::Tri:
		gmsh::option::setNumber("Mesh.Algorithm", 1);
		gmsh::model::mesh::generate(2);
		break;
	case MeshingElemType::Quad:
		gmsh::option::setNumber("Mesh.Algorithm", 11);
		gmsh::model::mesh::generate(2);
		break;
	case MeshingElemType::Quad_Mixed:
		gmsh::option::setNumber("Mesh.Algorithm", 1);
		gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 0);
		gmsh::option::setNumber("Mesh.RecombineAll", 1);
		gmsh::model::mesh::generate(2);
		break;
	case MeshingElemType::Tet:
		gmsh::option::setNumber("Mesh.Algorithm", 1);
		gmsh::option::setNumber("Mesh.Algorithm3D", 1);
		gmsh::model::mesh::generate(3);
		break;
	case MeshingElemType::Hex:
		gmsh::option::setNumber("Mesh.SubdivisionAlgorithm", 2);
		gmsh::model::mesh::generate(3);
		break;
	case MeshingElemType::Hex_Structural:
		gmsh::model::mesh::setTransfiniteAutomatic();
		//gmsh::model::geo::mesh::setRecombine(2, 1);
		gmsh::model::mesh::generate(3);
		break;
	default:
		break;
	}
	//if (pgmms->meshElemType == MeshingElemType::Tri || pgmms->meshElemType == MeshingElemType::Quad)
	//	gmsh::model::mesh::generate(2);
	//else
	//	gmsh::model::mesh::generate(3);
}

void GmshMesher::mapFieldsByModifyMap()
{
	std::list<std::shared_ptr<GmshShareTopology>> tmpSTList = getParamsByClass<GmshShareTopology>();
	std::function<int(int)> checkMap = [&tmpSTList](int key) { 
		for (auto& tmpST : tmpSTList)
			for (auto& tmpMapPair : tmpST->maplist)
				if (key == tmpMapPair.first)
					return tmpMapPair.second;
		return key;
	};

	std::vector<int> fieldTags;
	std::vector<double> valueBuf;
	std::string typeBuf;
	gmsh::model::mesh::field::list(fieldTags);
	for (auto tag : fieldTags)
	{
		gmsh::model::mesh::field::getType(tag, typeBuf);
		if (typeBuf != "Distance") continue;
		gmsh::model::mesh::field::getNumbers(tag, "PointsList", valueBuf);
		for (auto& listTags : valueBuf) listTags = checkMap(listTags);
		gmsh::model::mesh::field::setNumbers(tag, "PointsList", valueBuf);
		gmsh::model::mesh::field::getNumbers(tag, "CurvesList", valueBuf);
		for (auto& listTags : valueBuf) listTags = checkMap(listTags);
		gmsh::model::mesh::field::setNumbers(tag, "CurvesList", valueBuf);
		gmsh::model::mesh::field::getNumbers(tag, "SurfacesList", valueBuf);
		for (auto& listTags : valueBuf) listTags = checkMap(listTags);
		gmsh::model::mesh::field::setNumbers(tag, "SurfacesList", valueBuf);
	}
}

void GmshShareTopology::performParam()
{
	std::vector < std::pair < int, int > > masterDimTags, slaveDimTags, masterVolumeDimTags, slaveVolumeDimTags;
	std::vector < std::pair < int, int > > outDimTags;
	std::vector < std::pair < int, int > > allVolumeTags, allSurfTags, allLineTags;

	std::vector < std::vector<int>> surfacesInLoops;
	std::vector<int> surfaceLoops;
	std::vector<int> surfacesMaster, surfacesSlave;
	std::vector < std::vector < std::pair < int, int > > > outDimTagsMap;

	gmsh::model::getEntities(allVolumeTags, 3);
	for (auto& volume : allVolumeTags)
	{
		surfaceLoops.clear();
		gmsh::model::occ::getSurfaceLoops(volume.second, surfaceLoops, surfacesInLoops);
		for(auto& surfaces : surfacesInLoops)
			for (int surfaceTag : surfaces)
			{
				if (surfaceTag == m_masterEntityTag)
				{
					masterVolumeDimTags.emplace_back(volume.first, volume.second);
					for (int surfaceTag2 : surfaces)
						surfacesMaster.emplace_back(surfaceTag2);
				}
				if (surfaceTag == m_slaveEntityTag)
				{
					slaveVolumeDimTags.emplace_back(volume.first, volume.second);
					for (int surfaceTag2 : surfaces)
						surfacesSlave.emplace_back(surfaceTag2);
				}
			}
		
	}
	
	masterDimTags.emplace_back(m_masterEntityDim, m_masterEntityTag);
	slaveDimTags.emplace_back(m_slaveEntityDim, m_slaveEntityTag);
	gmsh::model::getEntities(allLineTags, 1);
	gmsh::model::getEntities(allSurfTags, 2);
	//for (auto& tag : surfacesMaster)
	//	masterVolumeDimTags.emplace_back(2, tag);
	//for (auto& tag : surfacesSlave)
	//	slaveVolumeDimTags.emplace_back(2, tag);
	if(masterVolumeDimTags[0].second < slaveVolumeDimTags[0].second)
		gmsh::model::occ::fragment(masterVolumeDimTags, slaveVolumeDimTags, outDimTags, outDimTagsMap);
	else
		gmsh::model::occ::fragment(slaveVolumeDimTags, masterVolumeDimTags, outDimTags, outDimTagsMap);
	//gmsh::model::occ::synchronize();
	//gmsh::model::geo::synchronize();
	gmsh::model::getEntities(allLineTags, 1);
	gmsh::model::getEntities(allSurfTags, 2);

	//return;
	//完成后，需检查是否有面消失，做好标记交由mesher处理field中的映射关系
	int outMapBias = masterDimTags.size();
	//maplist.emplace_back(1, 2);
	for (int i = 0; i < masterDimTags.size(); i++)
		for (int j = 0; j < outDimTagsMap[i].size(); j++)
			if (masterDimTags[i].first == outDimTagsMap[i][j].first
				&& masterDimTags[i].second != outDimTagsMap[i][j].second)
				if (masterDimTags[i].second > outDimTagsMap[i][j].second)
					maplist.emplace_back(masterDimTags[i].second, outDimTagsMap[i][j].second);
				else
					maplist.emplace_back(outDimTagsMap[i][j].second, masterDimTags[i].second);
	for (int i = 0; i < slaveDimTags.size(); i++)
		for (int j = 0; j < outDimTagsMap[i + outMapBias].size(); j++)
			if (slaveDimTags[i].first == outDimTagsMap[i + outMapBias][j].first
				&& slaveDimTags[i].second != outDimTagsMap[i + outMapBias][j].second)
				if (slaveDimTags[i].second > outDimTagsMap[i + outMapBias][j].second)
					maplist.emplace_back(slaveDimTags[i].second, outDimTagsMap[i + outMapBias][j].second);
				else
					maplist.emplace_back(outDimTagsMap[i + outMapBias][j].second, slaveDimTags[i].second);
}

void GmshShareTopology::collectInfo(std::ostream& os)
{
}

// REMINDER: all structs should be added here
REGISTER_PARAM(GmshGlobalSettings,"GlobalSettings")
REGISTER_PARAM(GmshHardPoint,"HardPoint")
REGISTER_PARAM(GmshDistanceField,"DistanceField")
REGISTER_PARAM(GmshThresholdField,"ThresholdField")
REGISTER_PARAM(GmshOctTreeField, "OctTreeField")
REGISTER_PARAM(GmshShareTopology,"ShareTopology")
REGISTER_PARAM(GmshMeshToMeshFlag,"MeshToMeshFlag")
REGISTER_PARAM(GmshMeshingMethod,"MeshingMethod")

