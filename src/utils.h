#ifndef UTILS_H_
#define UTILS_H_
#include <vector>

/**
* Update the size map of a cadsurf session.
* @param[in] vec : the MeshGems cadsurf session, an object created by @href <tt>api_3dpm_create_cadsurf_session</tt>.
* @param[in] value : a native MeshGems size map.
* @param[out] aoa : (optional) ACIS options
* @param[in] ao : (optional) ACIS options
* @return The outcome object indicating success or failure.
*/
bool isInVector(const std::vector < size_t >& vec, const int& value);

bool isInVector(const std::vector < size_t >& vec, const size_t& value);

#endif