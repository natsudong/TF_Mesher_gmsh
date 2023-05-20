#include "Params.h"
#include <string>
#include <fstream>
#include <map>

#define IS_CATEGORY(str) (strcmp(str, buffer) == 0)


void Mesher::readParam(std::string filepath)
{
	std::ifstream infile;
	std::string buf;
	if (filepath.empty())
		infile.open(inParamFilePath);
	else
		infile.open(filepath);

	while (1)
	{
		infile >> buf;
		PParamBase pb = getParamInstanceByKey(buf);
		
		if (pb == nullptr)
			break;
		pb->readParam(infile);
		addParam(pb);
		buf = "";
	}
}

PParamBase Mesher::getParamInstanceByKey(const std::string& _key)
{
	return RegisterHelper::getInstance().produce_shared(_key);
}

void Mesher::addParam(PParamBase _pb)
{
	m_params.push_back(_pb);
}

void GlobalSettings::readParam(std::istream& is)
{
	int type;
	is >> type >> minSize >> maxSize;
	switch (type)
	{
	case 1:
		meshElemType = MeshingElemType::Tri;
		break;
	case 2:
		meshElemType = MeshingElemType::Quad;
		break;
	case 3:
		meshElemType = MeshingElemType::Quad_Mixed;
		break;
	case 4:
		meshElemType = MeshingElemType::Tet;
		break;
	case 5:
		meshElemType = MeshingElemType::Hex;
		break;
	default:
		meshElemType = MeshingElemType::Tri;
		break;
	}
	//meshElemType = type == 1 ? MeshingElemType::Tri : (type == 2 ? MeshingElemType::Quad : (type == 3 ? MeshingElemType::Tet : MeshingElemType::Hex));
}

//std::map<std::string, std::function< ParamBase* (void)>> RegisterHelper::map_;

void DistanceField::readParam(std::istream& is)
{
	int buf;
	is >> buf;
	setId(buf);
	is >> m_nodeNum >> m_lineNum >> m_surfaceNum;
	m_nodeList.reserve(m_nodeNum);
	m_lineList.reserve(m_lineNum);
	m_surfaceList.reserve(m_surfaceNum);
	for (int i = 0; i < m_nodeNum; i++)
	{
		is >> buf;
		m_nodeList.push_back(buf);
	}
	for (int i = 0; i < m_lineNum; i++)
	{
		is >> buf;
		m_lineList.push_back(buf);
	}
	for (int i = 0; i < m_surfaceNum; i++)
	{
		is >> buf;
		m_surfaceList.push_back(buf);
	}
}

void ThresholdField::readParam(std::istream& is)
{
	int buf;
	is >> buf;
	setId(buf);
	is >> m_inField;
	is >> m_distMin >> m_distMax >> m_sizeMin >> m_sizeMax;
}

void HardPoint::readParam(std::istream& is)
{
	is >> point_id >> x >> y >> z;
}

void ShareTopology::readParam(std::istream& is)
{
	is >> m_masterEntityDim >> m_masterEntityTag >> m_slaveEntityDim >> m_slaveEntityTag;
}

void MeshToMeshFlag::readParam(std::istream& is)
{
	return;
}

void MeshingMethod::readParam(std::istream& is)
{
	int type;
	is >> type;
	switch (type)
	{
	case 1:
		meshElemType = MeshingElemType::Tri;
		break;
	case 2:
		meshElemType = MeshingElemType::Quad;
		break;
	case 3:
		meshElemType = MeshingElemType::Quad_Mixed;
		break;
	case 4:
		meshElemType = MeshingElemType::Tet;
		break;
	case 5:
		meshElemType = MeshingElemType::Hex;
		break;
	case 6:
		meshElemType = MeshingElemType::Hex_Structural;
		break;
	default:
		meshElemType = MeshingElemType::Tri;
		break;
	}
}

void OctTreeField::readParam(std::istream& is)
{
	int buf;
	is >> buf;
	setId(buf);
	is >> m_inField;
}
