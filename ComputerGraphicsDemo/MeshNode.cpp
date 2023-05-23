#include "MeshNode.h"

MeshNode::MeshNode()
    : m_mesh{}, m_transform(DirectX::XMMatrixIdentity())
{
}

MeshNode::MeshNode(std::unique_ptr<Mesh> mesh)
    : m_mesh(std::move(mesh)), m_transform(DirectX::XMMatrixIdentity())
{
}

Matrix4x4 MeshNode::AbsoluteMatrix() const
{
    return m_parent ? DirectX::XMMatrixMultiply(m_transform, m_parent->AbsoluteMatrix()) : m_transform;
}

void MeshNode::Transform(Matrix4x4 m)
{
    m_transform = DirectX::XMMatrixMultiply(m_transform, m);
}

const Mesh* MeshNode::GetMesh() const
{
    return m_mesh.get();
}

void MeshNode::AddChild(std::unique_ptr<MeshNode> child)
{
    child->m_parent = this;
    m_children.push_back(std::move(child));
}

void MeshNode::SetParent(MeshNode* parent)
{
    m_parent = parent;
}

const MeshNode::ChildrenType& MeshNode::GetChildren() const
{
    return m_children;
}

void MeshNode::Traverse(std::function<void(const MeshNode*)> f) const
{
    f(this);

    for (const auto& c : m_children)
        c->Traverse(f);
}
