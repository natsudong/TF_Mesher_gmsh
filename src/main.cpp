#include <iostream>
#include <algorithm>
#include <memory>
#include "gmsh.h"
#include "mesh_io/mesh_io.h"
#include "gmsh/generate_mesh.h"
#include "..\Params.h"
#include "..\GmshParams.h"
#include "project.h"

int main(int argc, char** argv)
{
	std::string inputFile = argv[1];
	std::string outputFile = argv[2];

	std::shared_ptr<Mesher> mesher = std::make_shared<GmshMesher>();

	mesher->inFilePath = inputFile;
	mesher->outFilePath = outputFile;
	mesher->inParamFilePath = argv[3];
	mesher->postInfoFilePath = argv[4];
	//mesher->readParam();
	mesher->performParams();
	mesher->performMesh();
	gmsh::model::occ::synchronize();

	mesher->collectInfo();
	//system("pause");
	return 0;
}

int main1(int argc, char** argv)
{
	if (argc < 10 || strcmp(argv[1], "-h") == 0)
	{
		std::cout << "Usage :\n\
\t<command> <MeshType> <inputFile> <outputFile> <meshSizeFactor> <meshSizeMin> <meshSizeMax> <optThreshold> <smoothStep> <meshType> [meshParamsFile] [bindingsOutFile]\n\
\tMeshType\t\"Geometry\" or \"GeometryFace\" or \"Mesh\" or \"ReMesh2D\"or \"GeometryWithCenter\" or \"GeometryWithEmbeddings\"or \"GeometryWithFragments\"or \"GeometryWith3DDensityImprove\"or \"GeometryWith3DDensityImproveAndFragment\"or \"GeometryWith3DDensityImproveAndCenter\"  \n\
\tinputFile\tInput File Path\n\
\toutputFile\tOutput File Path, only .bdf format is supported\n\
\tmeshSizeFactor\tSize Factor (double)\n\
\tmeshSizeMin\tMin Size (double)\n\
\tmeshSizeMax\tMax Size (double)\n\
\toptThreshold\tOptimize Threshold (double)\n\
\tsmoothStep\tSteps number of Smoothing (int)\n\
\tmeshType\tTypes of element, \"Tri\" or \"StructuredQuad\" or \"UnstructuredQuad\" or \"Tet\" or \"Hex\"\n\
\tmeshParamsFile\tParams File Path\n\
\tbindingsOutFile\tHard Points Binding Map File\
" << std::endl;
		return 0;
	}
	std::string inputType = argv[1];
	std::string inputFile = argv[2];
	std::string outputFile = argv[3];
	double meshSizeFactor = strtod(argv[4], NULL);
	double meshSizeMin = strtod(argv[5], NULL);
	double meshSizeMax = strtod(argv[6], NULL);
	double optThreshold = strtod(argv[7], NULL);
	int smoothStep = atoi(argv[8]);
	std::string meshType = argv[9];
	std::string meshParamsFile = "";
	std::string bindingsOutFile = "";

	if (argc > 11)
	{
		meshParamsFile = argv[10];
		bindingsOutFile = argv[11];
	}
	if (argc >= 13)
	{
		meshSizeFactor *= strtod(argv[12], NULL);
	}
	Project project(inputType, inputFile, outputFile, meshSizeFactor, meshSizeMin, meshSizeMax, optThreshold, smoothStep, meshType, meshParamsFile, bindingsOutFile);
	if (!project.start()) {
		std::cout << "========ERROR========" << std::endl;
		return 0;
	}
	//system("pause");
	return 0;
}