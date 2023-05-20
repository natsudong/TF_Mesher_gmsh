#pragma once
#include "Params.h"

template<typename _T>
inline std::list<std::shared_ptr<_T>> Mesher::getParamsByClass()
{
	std::list<std::shared_ptr<_T>> returnList;
	for (PParamBase pb : m_params)
	{
		std::shared_ptr<_T> pt = std::dynamic_pointer_cast<_T>(pb);
		if (pt != nullptr)
		{
			returnList.push_back(pt);
		}
	}
	return returnList;
}
