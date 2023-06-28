#include "Mesh.h"

#include <DirectXMath.h>

#include <algorithm>
#include <iterator>
#include <set>

Mesh::Mesh(std::vector<Vertex>&& vertices)
    : m_vertices(std::move(vertices)) {

}

DirectX::BoundingBox& Mesh::GetBoundingBox()
{
    DirectX::XMVECTOR vMin{}, vMax{};

    vMin = vMax = m_vertices.begin()->position;

    for (const auto& v : m_vertices) {
        vMin = DirectX::XMVectorMin(vMin, v.position);
        vMax = DirectX::XMVectorMax(vMax, v.position);
    }

    m_boundingBox.CreateFromPoints(m_boundingBox, vMin, vMax);

    return m_boundingBox;
}
