#include "Mesh.h"

#include <DirectXMath.h>

#include <algorithm>
#include <iterator>
#include <set>

Mesh::Mesh(std::vector<Vertex>&& vertices) 
	: m_vertices(std::move(vertices)) {

}
