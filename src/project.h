#ifndef PROJECT_H_
#define PROJECT_H_
#include <algorithm>
#include "gmsh.h"
#include "mesh_io/mesh_io.h"
#include "gmsh/generate_mesh.h"

class Project {
public:
	Project(const std::string& inputType,
		const std::string& inputFile,
		const std::string& outputFile,
		const double& meshSizeFactor,
		const double& meshSizeMin,
		const double& meshSizeMax,
		const double& optThreshold,
		const int& smoothStep,
		const std::string& meshType,
		const std::string& meshParamsFile = "",
		const std::string& bindingsOutFile = ""
	)
	{
		m_inputType = inputType;
		m_inputFile = inputFile;
		m_outputFile = outputFile;
		m_meshSizeFactor = meshSizeFactor;
		m_meshSizeMin = meshSizeMin;
		m_meshSizeMax = meshSizeMax;
		m_optThreshold = optThreshold;
		m_smoothStep = smoothStep;
		m_meshType = meshType;
		m_meshParamsFile = meshParamsFile;
		m_bindingsOutFile = bindingsOutFile;
		m_addMassCenter = false;
	}
	int start();
private:
	std::string m_inputType;
	std::string m_inputFile;
	std::string m_outputFile;
	double m_meshSizeFactor;
	double m_meshSizeMin;
	double m_meshSizeMax;
	double m_optThreshold;
	int m_smoothStep;
	bool m_addMassCenter;
	std::string m_meshType;
	std::string m_meshParamsFile;
	std::string m_bindingsOutFile;

	void geoToMesh();
	void geoToMesh_ST_test();
	void geoToMeshTest();
	void meshToMeshWithCenter();
	void geoToMeshWith3DDensityImprove(); // In this function, meshsize(3D) = meshsize(2D) * Mesh.MeshSizeFactor
	void geoToMeshWith3DDensityImproveAndFragment(); // In this function, meshsize(3D) = meshsize(2D) * Mesh.MeshSizeFactor
	void geoToMeshWithEmbeddings();
	void geoToMeshWithFragments();

	void meshToMesh();
	void meshToMeshWithIdMap();
	void meshRemeshToMesh();
	void meshRemeshToMesh2D();
	void geoToMeshFace();
	int checkClosedMesh();
};

#endif