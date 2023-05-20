#pragma once
#include "Params.h"

#define DEF_FAC_GMSH(name) virtual inline P##name create##name() {return std::make_shared<Gmsh##name>();}
//#define DEF_FAC_GMSH(name) template <class... _Valty>\
inline P##name create##name(_Valty&&... _Val) {return std::make_shared<Gmsh##name>(std::forward<_Valty>(_Val)...);}
#define DEF_STANDARD_GMSHSUBCLASS(name) class Gmsh##name : public name\
{\
public:\
using name::name;\
virtual void performParam() override;\
virtual void collectInfo(std::ostream& os) override;\
};\
DEF_SP(Gmsh##name)

// REMINDER: all structs should be added here
DEF_STANDARD_GMSHSUBCLASS(HardPoint);
DEF_STANDARD_GMSHSUBCLASS(GlobalSettings);
DEF_STANDARD_GMSHSUBCLASS(DistanceField);
DEF_STANDARD_GMSHSUBCLASS(ThresholdField);
DEF_STANDARD_GMSHSUBCLASS(OctTreeField);
DEF_STANDARD_GMSHSUBCLASS(ShareTopology);
DEF_STANDARD_GMSHSUBCLASS(MeshToMeshFlag);
DEF_STANDARD_GMSHSUBCLASS(MeshingMethod);


class GmshMesher : public Mesher
{

public:
	GmshMesher(){}

	//保证参数不冲突
	virtual void checkParamsAvail() override;
	//执行参数设置的入口
	virtual void performParams() override;
	//画完后收集信息
	virtual void collectInfo(std::string filepath = "") override;

	virtual void performMesh() override;

	virtual void mapFieldsByModifyMap() override;

};
