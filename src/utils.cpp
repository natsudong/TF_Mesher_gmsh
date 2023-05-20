#include "utils.h"

bool isInVector(const std::vector < size_t >& vec, const int& value)
{
	std::vector < size_t >::const_iterator it = std::find(vec.begin(), vec.end(), value);
	if (it != vec.end()) return true;
	else return false;
}

bool isInVector(const std::vector < size_t >& vec, const size_t& value)
{
	std::vector < size_t >::const_iterator it = std::find(vec.begin(), vec.end(), value);
	if (it != vec.end()) return true;
	else return false;
}