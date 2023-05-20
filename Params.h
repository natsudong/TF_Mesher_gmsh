#pragma once
#include <list>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <functional>

#define DEF_SP(name) typedef std::shared_ptr<name> P##name
//#define DEF_FAC_ABS(name) inline P##name create##name(){ return std::make_shared<name>();}
#define DEF_FAC_ABS(name) virtual inline P##name create##name() = 0
//#define DEF_FAC_ABS(name) virtual inline P##name create##name() = 0

#define DEF_PARAM_LIST(name) protected:					\
std::list<P##name>	m_##name##s;						\
public:													\
inline std::list<P##name>::iterator get##name##ListIte()\
{return m_##name##s.begin();}							\
inline void add##name(P##name param)					\
{m_##name##s.push_back(param);}							

//TODO: 增加自动加入对应列表的机制

//inline bool addParamBase(PParamBase _pb)				\
//{														\
//	P##name tmp_P##name = std::dynamic_pointer_cast<name>(_pb);\
//	if (tmp_P##name == nullptr)							\
//		return false;									\
//	m_##name##s.push_back(tmp_P##name);					\
//}

class ParamBase
{
public:
	virtual void performParam() = 0;
	virtual void collectInfo(std::ostream& os) = 0;
	virtual void readParam(std::istream& is) = 0;
private:

};

struct RegisterHelper
{
protected:
	RegisterHelper() {};
	RegisterHelper(const RegisterHelper&) = delete;
	RegisterHelper(RegisterHelper&&) = delete;
	std::map<std::string, std::function<ParamBase*() > > map_;

public:
	template<typename T>
	struct register_t
	{
		register_t(const std::string& key)
		{
			//RegisterHelper::getInstance().map_.emplace(key, [] {return new T(); });
			std::function<ParamBase* ()> tmpfunc = [] {return new T(); };
			RegisterHelper::getInstance().map_[key]= tmpfunc;
		}

		template<typename... Args>
		register_t(const std::string& key, Args... args)
		{
			RegisterHelper::getInstance().map_.emplace(key, [&] {return new T(args...); });
		}
	};

	static ParamBase* produce(const std::string& key)
	{
		if (RegisterHelper::getInstance().map_.find(key) == RegisterHelper::getInstance().map_.end())
			return nullptr;
		return RegisterHelper::getInstance().map_[key]();
	}

	static std::unique_ptr<ParamBase> produce_unique(const std::string& key)
	{
		return std::unique_ptr<ParamBase>(produce(key));
	}


	static std::shared_ptr<ParamBase> produce_shared(const std::string& key)
	{
		return std::shared_ptr<ParamBase>(produce(key));
	}

	static RegisterHelper& getInstance()
	{
		static RegisterHelper instance;
		return instance;
	}
};


#define REGISTER_PARAM_VNAME(T) reg_param_##T##_
//此处args可为同一子类的不同构造函数设置不同key
#define REGISTER_PARAM(T, key, ...) static RegisterHelper::register_t<T> REGISTER_PARAM_VNAME(T)(key, __VA_ARGS__);
#define REGISTER_PARAM_STANDARD(name) REGISTER_PARAM(name, #name)


enum class MeshingElemType
{
	Tri,
	Quad,
	Quad_Mixed,
	Tet,
	Hex,
	Hex_Structural,
};

enum class ParamType
{

};

class GlobalSettings : public ParamBase
{
public:
	virtual void readParam(std::istream& is);
	double maxSize, minSize;
	MeshingElemType meshElemType;
};

class HardPoint : public ParamBase
{
public:
	virtual void readParam(std::istream& is);
	double x, y, z;
	int point_id;
};

class MeshToMeshFlag : public ParamBase
{
public:
	virtual void readParam(std::istream& is);
	bool preformed = false;
};

//TODO: Field should add id!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class Field : public ParamBase
{
public:
	virtual void readParam(std::istream& is) {};
	inline int getId() { return m_id; };
protected:
	inline void setId(int _id) { m_id = _id; };
private:
	int m_id;
};

class DistanceField : public Field
{
public:
	void readParam(std::istream& is);
protected:
	int m_nodeNum, m_lineNum, m_surfaceNum;
	std::vector<double> m_nodeList, m_lineList, m_surfaceList;
};

class ThresholdField : public Field
{
public:
	void readParam(std::istream& is);
protected:
	int m_inField;
	double m_distMin, m_distMax, m_sizeMin, m_sizeMax;
};

class OctTreeField : public Field
{
public:
	void readParam(std::istream& is);
protected:
	double m_inField;
};

class ShareTopology : public ParamBase
{
public:
	virtual void readParam(std::istream& is);
	int m_masterEntityTag, m_slaveEntityTag;
	int m_masterEntityDim, m_slaveEntityDim;
	std::list<std::pair<int, int>> maplist;
};

class MeshingMethod : public ParamBase
{
public:
	virtual void readParam(std::istream& is);
	MeshingElemType meshElemType = MeshingElemType::Tri;
};

// REMINDER: all structs should be added here
DEF_SP(ParamBase);
DEF_SP(HardPoint);
DEF_SP(GlobalSettings);
DEF_SP(Field);
DEF_SP(DistanceField);
DEF_SP(ThresholdField);
DEF_SP(ShareTopology);
DEF_SP(MeshToMeshFlag);
DEF_SP(OctTreeField);


class Mesher
{
public:
	//保证参数不冲突
	virtual void checkParamsAvail() = 0;
	//执行参数设置的入口
	virtual void performParams() = 0;
	//画完后收集信息
	virtual void collectInfo(std::string filepath = "") = 0;

	virtual void performMesh() = 0;

	virtual void mapFieldsByModifyMap() = 0;

	void readParam(std::string filepath = "");

	PParamBase getParamInstanceByKey(const std::string& _key);

	template<typename _T>
	std::list<std::shared_ptr<_T>> getParamsByClass();

	void addParam(PParamBase _pb);

	std::string inFilePath, outFilePath, inParamFilePath, postInfoFilePath;

protected:

	// REMINDER: all structs should be added here

	int meshDim = 2;

	

	std::list<PParamBase> m_params;

	//DEF_PARAM_LIST(HardPoint);
	//DEF_PARAM_LIST(Field);
	
	
	// if other mesh generator needs other private members, define them here
	// -----------GMSH-----------

};

#include "Params.inl"