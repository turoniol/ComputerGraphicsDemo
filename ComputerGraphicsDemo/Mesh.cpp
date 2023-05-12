#include "Mesh.h"

#include <DirectXMath.h>

#include <algorithm>
#include <iterator>
#include <set>

int Mesh::ComparePoints(const Point& left, const Point& right) {
	const float eps = 1e-6f;

	if (auto diff = left.x - right.x; abs(diff) > eps)
		return diff > 0 ? 1 : -1;
	if (auto diff = left.y - right.y; abs(diff) > eps)
		return diff > 0 ? 1 : -1;
	if (auto diff = left.z - right.z; abs(diff) > eps)
		return diff > 0 ? 1 : -1;

	return 0;
}

Mesh::Mesh(std::vector<Vertex>&& vertices) 
	: m_vertices(std::move(vertices)) {
}
